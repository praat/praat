/*
 * Feature.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Created on: Feb 4, 2016
 *      Author: ivan
 */

#include "Feature.h"

/********** char32 UTILITIES **********/

/**
 * Allocates memory for a char32_t string of length "size", and initializes all its positions to '\0'.
 * It returns a pointer to its first position.
 */
inline static char32* allocChar32(int size){
	char32* string = (char32*) malloc ((size_t) size * sizeof (char32));
	char32* currentPos = string;
	for(int i=0;i<size;i++)
		*currentPos ++= U'\0';
	return string;
}

/**
 * It appends the source char32_t string to the "pos" position of the target string.
 * It returns a pointer to the first position of the target string.
 *
 * WARNING: We assume that the target can hold the new content in its allocated memory, otherwise a memory fault will occur.
 */
inline static char32 * str32append (char32 *target, const char32 *source, int64 pos) {
	char32 *p = target;
	for(;pos > 0;p++, pos--);
	while (* source != U'\0') * p ++ = * source ++;
	*p = U'\0';
	return target;
}

/**
 * Concatenates the contents of both source strings in a new one.
 * It returns a pointer to the first position of the new string.
 */
char32* str32cat(char32 *source1, const char32 *source2){
	if(source1 == nullptr || source2 == nullptr) return nullptr;
	char32 * result = allocChar32(str32len(source1) + str32len(source2) + 1);
	str32cpy(result, source1);
	str32append(result, source2, str32len(result));
	return result;
}

/**
 * Returns a new string with the trimmed version of the source text
 */
char32* trim(const char32 *text) {
	if(str32len(text) == 0) return nullptr;
	const char32 * forwardPtr = text;
	int initialBlanks = 0;
	while(*forwardPtr != U'\0' && *forwardPtr == U' '){
		initialBlanks++;
		forwardPtr++;
	}

	char32 * result = allocChar32(str32len(text) - initialBlanks + 1);
	str32cpy(result, forwardPtr);

	char32 * backwardPtr = result + str32len(result)-1;
	while(backwardPtr+1 != result && *backwardPtr == U' '){
		*backwardPtr-- = U'\0';
	}

	return result;
}

/**
 * Returns whether or not the string is empty or null.
 */
bool str32IsEmpty(const char32 *text){
	if(text == nullptr) return true;
	bool result = false;
	char32* trimmed = trim(text);
	if(trimmed == nullptr || str32len(trimmed) == 0)
		result = true;
	delete trimmed;
	return result;
}

/*---------------------------------------------------*/

/**
 * Given a tierFeatures it returns a pointer to the current feature.
 * If there is no feature created then it creates the first and returns it.
 */
feature* getCurrentFeature(tierFeatures* data){
	if(data->currentFeature == nullptr){
		data->firstFeature = new feature();
		data->currentFeature = data->firstFeature;
	}
	return data->currentFeature;
}

/**
 * Given a tierFeatures it returns a pointer to the next feature after the current.
 * If there is no feature after the current one, then it creates and returns it.
 */
feature* getNextFeature(tierFeatures* data){
	if(data->currentFeature == nullptr){
		data->firstFeature = new feature();
		data->currentFeature = data->firstFeature;
	}else{
		if(data->currentFeature->nxtPtr == nullptr){
			data->currentFeature->nxtPtr = new feature();
			data->currentFeature = data->currentFeature->nxtPtr;
		}
	}
	return data->currentFeature;
}


/**
 * A states machine that reads the text and extracts its features data creating and saving it into a tierFeatures type structure.
 * Structure as follows:
 * State 0: Accepts any char until "@{@". Everything from the beginning is the interval or point text.
 * State 1: Accepts blank spaces until "\"".
 * State 2: Accepts any char until "\"". Everything from state 1 is a feature label.
 * State 3: Accepts blank spaces until "=".
 * State 4: Accepts blank spaces until "\"".
 * State 5: Accepts any char until "\"". Everything from state 4 is a feature value.
 * State 6: Accepts blank spaces until "," in which case it returns to state 2 and expects a new feature or "@}@" in which case it finishes.
 * State 7: Ended correctly
 * State 8: Error the whole string will be considered interval or point text.
 */
tierFeatures* extractTierFeatures(const char32* text){
	if(text == nullptr) return new tierFeatures();
	int state = 0;
	int64 pos = 0;
	const char32* currentPos = text;
	const char32* lastCheckPoint = text;
	tierFeatures* data = new tierFeatures();
	const char32* temp;
	do{
		char32 currentChar = *currentPos++;
		pos++;

		switch(state){
		case 0:
			if(currentChar != U'@') break;
			if(str32len(currentPos) < 2) break;
			temp = currentPos;
			if(*temp++ == U'{' && *temp++ == U'@'){
				currentPos = temp;
				data->text = allocChar32(pos);
				str32ncpy(data->text, lastCheckPoint, pos-1);
				lastCheckPoint = currentPos;
				pos = 0;
				state = 1;
			}
			break;
		case 1:
			if(currentChar == U'@'){
				if(str32len(currentPos) < 2){ state = 8; break;}
				temp = currentPos;
				if(*temp++ == U'}' && *temp++ == U'@'){
					currentPos = temp;
					pos+=2;
					lastCheckPoint = currentPos;
					pos = 0;
					state = 7;
					break;
				}else{
					state = 8;
					break;
				}
			}
			if(currentChar != U'"' && currentChar != U' '){ state = 8; break;}
			if(currentChar != U'"') break;
			lastCheckPoint = currentPos;
			pos = 0;
			state = 2;
			break;
		case 2:
			if(currentChar == U'\\'){
				if(str32len(currentPos) < 1) break;
				currentPos++;
				pos++;
				break;
			}
			if(currentChar != U'"') break;
			getNextFeature(data)->label = allocChar32(pos);
			str32ncpy(getCurrentFeature(data)->label, lastCheckPoint, pos-1);
			lastCheckPoint = currentPos;
			pos = 0;
			state = 3;
			break;
		case 3:
			if(currentChar != U'=' && currentChar != U' '){ state = 8; break;}
			if(currentChar != U'=') break;
			lastCheckPoint = currentPos;
			pos = 0;
			state = 4;
			break;
		case 4:
			if(currentChar != U'"' && currentChar != U' '){ state = 8; break;}
			if(currentChar != U'"') break;
			lastCheckPoint = currentPos;
			pos = 0;
			state = 5;
			break;
		case 5:
			if(currentChar == U'\\'){
				if(str32len(currentPos) < 1) break;
				currentPos++;
				pos++;
				break;
			}
			if(currentChar != U'"') break;
			getCurrentFeature(data)->value = allocChar32(pos);
			str32ncpy(getCurrentFeature(data)->value, lastCheckPoint, pos-1);
			lastCheckPoint = currentPos;
			pos = 0;
			state = 6;
			break;
		case 6:
			if(currentChar == U'@'){
				if(str32len(currentPos) < 2){ state = 8; break;}
				temp = currentPos;
				if(*temp++ == U'}' && *temp++ == U'@'){
					currentPos = temp;
					pos+=2;
					lastCheckPoint = currentPos;
					pos = 0;
					state = 7;
					break;
				}else{
					state = 8;
					break;
				}
			}
			if(currentChar != U',' && currentChar != U' '){ state = 8; break;}
			if(currentChar != U',') break;
			lastCheckPoint = currentPos;
			pos = 0;
			state = 1;
			break;
		}
	}while(state != 8 && state != 7 && *currentPos != U'\0');

	if(state != 7){
		delete data;
		data = new tierFeatures();
		data->text = allocChar32(str32len(text)+1);
		str32ncpy(data->text, text, str32len(text));
	}

	return data;
}

/**
 * It returns a new char32_t string with the content of text but with backslashes before \ and "
 */
char32* addBackslashes(const char32* text){
	int neededSpace = 0;
	const char32* tmp = text;
	for(;*tmp != U'\0';tmp++){
		neededSpace++;
		if(*tmp == U'"' || *tmp == U'\\')
			neededSpace++;
	}

	char32* result = allocChar32(neededSpace+1);
	tmp = text;
	char32* aux = result;
	for(;*tmp != U'\0';){
		if(*tmp == U'"' || *tmp == U'\\')
			*aux++ = U'\\';
		*aux++ = *tmp++;
	}
	return result;
}

/**
 * It returns a new char32_t string with the content of text but without initial backslashes of special characters
 */
char32* removeBackslashes(const char32* text){
	char32* result = allocChar32(str32len(text)+1);
	const char32* tmp = text;
	char32* aux = result;
	for(;*tmp != U'\0';){
		if(*tmp == U'\\' && str32len(tmp) > 1)
			tmp++;
		*aux++ = *tmp++;
	}
	return result;
}

/**
 * Given a tierFeatures and a label, returns the feature (feature) with the same label. Null if the feature does not exist.
 */
feature* getExistentFeature(const tierFeatures* data, const char32* label){
	if(data == nullptr) return nullptr;

	feature* current = data->firstFeature;
	while(current != nullptr){
		if(str32equ(current->label, label))
			return current;
		current = current->nxtPtr;
	}
	return nullptr;
}

/**
 * Adds a new feature with label and value to the given tierFeatures.
 * If the feature already exists overwrites its value.
 */
void addFeatureToTierFeatures(tierFeatures* data, const char32* label, const char32* value){
	if(data == nullptr) return;
	char32* fLabel = addBackslashes(label);
	char32* fValue = addBackslashes(value);

	feature* ftr = getExistentFeature(data, fLabel);
	if(ftr != nullptr){
		delete ftr->value;
		ftr->value = allocChar32(str32len(fValue)+1);
		str32ncpy(ftr->value, fValue, str32len(fValue));
	}else{
		getNextFeature(data)->label = allocChar32(str32len(fLabel)+1);
		str32ncpy(getCurrentFeature(data)->label, fLabel, str32len(fLabel));
		getCurrentFeature(data)->value = allocChar32(str32len(fValue)+1);
		str32ncpy(getCurrentFeature(data)->value, fValue, str32len(fValue));
	}

	delete fLabel;
	delete fValue;
}

/**
 * Deletes the given feature from the tierFeatures.
 */
void deleteFeatureFromTierFeatures(tierFeatures* data, const char32* label){
	if(data == nullptr) return;
	char32* fLabel = addBackslashes(label);
	feature* ftr = getExistentFeature(data, fLabel);
	delete fLabel;
	if(ftr == nullptr){
		return;
	}
	feature* tmp = data->firstFeature;
	if(tmp->nxtPtr == nullptr){
		data->firstFeature = nullptr;
		data->currentFeature = nullptr;
		delete ftr;
		return;
	}else if(tmp == ftr){
		data->firstFeature = tmp->nxtPtr;
		delete ftr;
		return;
	}else{
		while(tmp->nxtPtr != ftr){
			tmp = tmp->nxtPtr;
		}
		if(data->currentFeature == ftr) data->currentFeature = tmp;
		if(tmp->nxtPtr->nxtPtr != nullptr) tmp->nxtPtr = tmp->nxtPtr->nxtPtr;
		else tmp->nxtPtr = nullptr;
		delete ftr;
	}
}

/**
 * Returns an integer representing the total length of the string representation of the whole tierFeatures.
 */
int countTierFeaturesLength(tierFeatures* data){
	if(data == nullptr) return 0;
	int totalLenght = 0;
	if(data->text != nullptr)
		totalLenght += str32len(data->text);
	totalLenght += 4;
	feature* tempPtr = data->firstFeature;
	while(tempPtr != nullptr){
		totalLenght += 1;
		if(tempPtr->label != nullptr)
			totalLenght += str32len(tempPtr->label);
		totalLenght += 5;
		if(tempPtr->value != nullptr)
			totalLenght += str32len(tempPtr->value);
		totalLenght += 1;
		tempPtr = tempPtr->nxtPtr;
		if(tempPtr != nullptr)
			totalLenght += 2;
	}
	totalLenght += 5;
	return totalLenght;
}

/**
 * Return a new char32_t string representing the data held in the given tierFeatures.
 */
char32* generateTextFromTierFeatures(tierFeatures* data){
	if(data == nullptr) return nullptr;
	int totalLenght = countTierFeaturesLength(data);
	if(totalLenght <= 0) return nullptr;

	char32* result = allocChar32(totalLenght);
	if(data->text != nullptr)
		str32append(result, data->text, str32len(result));
	if(data->firstFeature != nullptr){
		str32append(result, U"@{@ ", str32len(result));
		feature* tempPtr = data->firstFeature;
		while(tempPtr != nullptr){
			str32append(result, U"\"", str32len(result));
			if(tempPtr->label != nullptr)
				str32append(result, tempPtr->label, str32len(result));
			str32append(result, U"\" = \"", str32len(result));
			if(tempPtr->value != nullptr)
				str32append(result, tempPtr->value, str32len(result));
			str32append(result, U"\"", str32len(result));
			tempPtr = tempPtr->nxtPtr;
			if(tempPtr != nullptr)
				str32append(result, U", ", str32len(result));
		}
		str32append(result, U" @}@", str32len(result));
	}
	str32append(result, U"\0", str32len(result));
	return result;
}

/**
 * Returns a new char32_t string consisting of the content within given text plus a new feature with the given label and value.
 * If feature already exists it is overwritten with given value.
 */
char32* addFeatureToText(const char32* text, const char32* label, const char32* value){
	tierFeatures* res = extractTierFeatures(text);
	addFeatureToTierFeatures(res, label, value);
	return generateTextFromTierFeatures(res);
}

/**
 * Changes the content in the text of TierFeatures for a copy of the given one.
 */
void replaceTierFeaturesText(tierFeatures* data, const char32* text){
	if(data == nullptr) return;
	delete data->text;
	data->text = allocChar32(str32len(text)+1);
	str32ncpy(data->text, text, str32len(text));
}
