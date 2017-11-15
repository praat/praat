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

#define my  me ->

/**
 * Given a tierFeatures it returns a pointer to the current feature.
 * If there is no feature created then it creates the first and returns it.
 */
Feature* TierFeatures_getCurrentFeature(TierFeatures* data){
	if(data->currentFeature == nullptr){
		data->firstFeature = new Feature();
		data->currentFeature = data->firstFeature;
	}
	return data->currentFeature;
}

/**
 * Given a tierFeatures it returns a pointer to the next feature after the current.
 * If there is no feature after the current one, then it creates and returns it.
 */
Feature* TierFeatures_getNextFeature(TierFeatures* data){
	if(data->currentFeature == nullptr){
		data->firstFeature = new Feature();
		data->currentFeature = data->firstFeature;
	}else{
		if(data->currentFeature->nxtPtr == nullptr){
			data->currentFeature->nxtPtr = new Feature();
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
TierFeatures* TierFeatures_extractFromText(const char32* text){
	if(text == nullptr) return new TierFeatures();
	int state = 0;
	int64 pos = 0;
	const char32* currentPos = text;
	const char32* lastCheckPoint = text;
	TierFeatures* data = new TierFeatures();
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
				MelderString_ncopy(&data->headText, lastCheckPoint, pos-1);
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
			MelderString_ncopy(&TierFeatures_getNextFeature(data)->label, lastCheckPoint, pos-1);
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
			MelderString_ncopy(&TierFeatures_getCurrentFeature(data)->value, lastCheckPoint, pos-1);
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
		data = new TierFeatures();
		MelderString_copy(&data->headText, text);
	}

	return data;
}

/**
 * It encodes a Feature text by adding backslash before special characters so it can be stores inside Tierfeatures text
 */
void Feature_encodeText(MelderString* me){
	int sizeNeeded = my length;
	for(int i = 0;i < my length; i++){
		if(my string [i] == U'"' || my string [i] == U'\\')
			sizeNeeded++;
	}

	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);

	for(int newSize = sizeNeeded, oldSize = my length; newSize >= 0 && oldSize >= 0; newSize--, oldSize--){
		my string [newSize] = my string [oldSize];
		if(my string [newSize] == U'"' || my string [newSize] == U'\\'){
			newSize --;
			my string [newSize] = U'\\';
		}
	}
	my length = sizeNeeded;
}

/**
 * It decodes a Feature text coming from TierFeatures text by deleting backslash preceding special characters
 */
void Feature_decodeText(MelderString* me){
	int newLenght = my length;
	for(int newPos = 0, oldPos = 0; newPos <= my length && oldPos <= my length; newPos++, oldPos++){
		if(my string [oldPos] == U'\\'){
			oldPos ++;
			newLenght --;
		}
		my string [newPos] = my string [oldPos];
	}
	my length = newLenght;
}

/**
 * Given a tierFeatures and a label, returns the Feature with the same label. Null if the feature does not exist.
 */
Feature* TierFeatures_getExistentFeature(const TierFeatures* data, const char32* label){
	if(data == nullptr) return nullptr;

	Feature* current = data->firstFeature;
	while(current != nullptr){
		if(str32equ(current->label.string, label))
			return current;
		current = current->nxtPtr;
	}
	return nullptr;
}

/**
 * Adds a new feature with label and value to the given TierFeatures.
 * If the feature already exists overwrites its value.
 */
void Feature_addFeatureToTierFeatures(TierFeatures* data, MelderString* label, MelderString* value){
	if(data == nullptr) return;
	Feature_encodeText(label);
	Feature_encodeText(value);

	Feature* ftr = TierFeatures_getExistentFeature(data, label->string);
	if(ftr != nullptr){
		MelderString_copy(&ftr->value, value->string);
	}else{
		MelderString_copy(&TierFeatures_getNextFeature(data)->label, label->string);
		MelderString_copy(&TierFeatures_getCurrentFeature(data)->value, value->string);
	}

}

/**
 * Deletes the Feature with given label from the TierFeatures.
 */
void Feature_deleteFeatureFromTierFeatures(TierFeatures* data, MelderString* label){
	if(data == nullptr) return;
	Feature_encodeText(label);
	Feature* ftr = TierFeatures_getExistentFeature(data, label->string);
	if(ftr == nullptr){
		return;
	}
	Feature* tmp = data->firstFeature;
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
 * Returns an integer representing the total length of the text of the TierFeatures.
 */
int countTierFeaturesLength(TierFeatures* data){
	if(data == nullptr) return 0;
	int totalLenght = 0;
	//if(data->text != nullptr)
	totalLenght += data->headText.length;
	totalLenght += 4;
	Feature* tempPtr = data->firstFeature;
	while(tempPtr != nullptr){
		totalLenght += 1;
		totalLenght += tempPtr->label.length;
		totalLenght += 5;
		totalLenght += tempPtr->value.length;
		totalLenght += 1;
		tempPtr = tempPtr->nxtPtr;
		if(tempPtr != nullptr)
			totalLenght += 2;
	}
	totalLenght += 5;
	return totalLenght;
}

/**
 * It sets in a result MelderString the text of the TierFeatures.
 */
void TierFeatures_generateText(TierFeatures* data, MelderString * result){
	if(data == nullptr) return;
	int totalLenght = countTierFeaturesLength(data);
	if(totalLenght <= 0) return;

	if(data->headText.bufferSize > 0)
		MelderString_append(result, data->headText.string);
	if(data->firstFeature != nullptr){
		MelderString_append(result, U"@{@ ");
		Feature* tempPtr = data->firstFeature;
		while(tempPtr != nullptr){
			MelderString_append(result, U"\"");
			if(tempPtr->label.bufferSize > 0)
				MelderString_append(result, tempPtr->label.string);
			MelderString_append(result, U"\" = \"");
			if(tempPtr->value.bufferSize > 0)
				MelderString_append(result, tempPtr->value.string);
			MelderString_append(result, U"\"");
			tempPtr = tempPtr->nxtPtr;
			if(tempPtr != nullptr)
				MelderString_append(result, U", ");
		}
		MelderString_append(result, U" @}@");
	}
	MelderString_append(result, U"\0");
}

/**
 * Sets in result MelderString the given text plus a new feature with the given label and value.
 * If feature already exists it is overwritten with given value.
 */
void Feature_addFeatureToText(const char32* text, MelderString* label, MelderString* value, MelderString* result){
	TierFeatures* features = TierFeatures_extractFromText(text);
	Feature_addFeatureToTierFeatures(features, label, value);
	TierFeatures_generateText(features, result);
}

/**
 * Sets in result MelderString the given text without the feature with the given label.
 */
void Feature_deleteFeatureFromText(const char32* text, MelderString* label, MelderString* result){
	TierFeatures* features = TierFeatures_extractFromText(text);
	Feature_deleteFeatureFromTierFeatures(features, label);
	TierFeatures_generateText(features, result);
}

/**
 * Changes the content of headText in the TierFeatures for a copy of the given one.
 */
void TierFeatures_replaceHeadText(TierFeatures* data, const char32* text){
	if(data == nullptr) return;
	MelderString_ncopy(&data->headText, text, str32len(text));
}

/**
 * Returns a pointer to the Feature in the given position. Melder error if feature does not exist.
 */
Feature* TierFeatures_getFeature(TierFeatures* data, int position){
	Feature* tmp = data->firstFeature;
	int count = 0;
	while(tmp != nullptr && count < position -1){
		count ++;
		tmp = tmp->nxtPtr;
	}
	if(tmp == nullptr){
		Melder_throw (U"Feature does not exist.");
	}
}

/**
 * Return the number of Features in a TierFeature.
 */
int TierFeatures_getNumberOfFeatures(TierFeatures* data){
	Feature* tmp = data->firstFeature;
	int result = 0;
	while(tmp != nullptr){
		result ++;
		tmp = tmp->nxtPtr;
	}
	return result;
}

/**
 * Sets in result MelderString a readable representation of the features in the TierFeatures.
 */
void TierFeatures_getFeaturesString(TierFeatures* data, MelderString* result){
	Feature* tmp = data->firstFeature;
	while(tmp != nullptr){
		Feature_decodeText(&tmp->label);
		Feature_decodeText(&tmp->value);
		MelderString_append(result, tmp->label.string, U": ", tmp->value.string, U"\n");

		tmp = tmp->nxtPtr;
	}
}


