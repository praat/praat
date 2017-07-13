#ifndef _Feature_h_
#define _Feature_h_
/*
 * Feature.h
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
 *      Author: Ivan Latorre
 */

#include "Preferences.h"

struct feature{
	char32* label;
	char32* value;
	feature* nxtPtr;
	feature()
	{
		label=nullptr;
		value=nullptr;
		nxtPtr=nullptr;
	}
	~feature(){
		delete label;
		delete value;
	}
};

struct tierFeatures{
	char32* text;
	feature* firstFeature;
	feature* currentFeature;
	tierFeatures()
	{
		text=nullptr;
		firstFeature=nullptr;
		currentFeature=nullptr;
	}
	~tierFeatures(){
		delete text;
		feature* tmp = firstFeature;
		while(tmp != nullptr){
			feature* toDelete = tmp;
			tmp = tmp->nxtPtr;
			delete toDelete;
		}
	}
};

/*---char32 UTILITIES---*/
char32* trim(const char32 *text);
bool str32IsEmpty(const char32 *text);
char32* str32cat(char32 *source1, const char32 *source2);

/*---Features treatment---*/
char32* addBackslashes(const char32* text);
char32* removeBackslashes(const char32* text);
tierFeatures* extractTierFeatures(const char32* text);
char32* addFeatureToText(const char32* text, const char32* label, const char32* value);
feature* getCurrentFeature(tierFeatures* data);
feature* getNextFeature(tierFeatures* data);
feature* getExistentFeature(const tierFeatures* data, const char32* label);
void deleteFeatureFromTierFeatures(tierFeatures* data, const char32* label);
char32* generateTextFromTierFeatures(tierFeatures* data);
void replaceTierFeaturesText(tierFeatures* data, const char32* text);

/* End of file Feature.h */
#endif
