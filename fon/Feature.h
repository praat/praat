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

struct Feature{
	autoMelderString label;
	autoMelderString value;
	Feature* nxtPtr;
	Feature()
	{
		nxtPtr=nullptr;
	}
	~Feature(){
	}
};

struct TierFeatures{
	autoMelderString headText;
	Feature* firstFeature;
	Feature* currentFeature;
	TierFeatures()
	{
		firstFeature=nullptr;
		currentFeature=nullptr;
	}
	~TierFeatures(){
		Feature* tmp = firstFeature;
		while(tmp != nullptr){
			Feature* toDelete = tmp;
			tmp = tmp->nxtPtr;
			delete toDelete;
		}
	}
};


void Feature_encodeText(MelderString *me);
void Feature_decodeText(MelderString *me);
void Feature_addFeatureToText(const char32* text, MelderString* label, MelderString* value, MelderString* result);
void Feature_deleteFeatureFromText(const char32* text, MelderString* label, MelderString* result);

TierFeatures* TierFeatures_extractFromText(const char32* text);
Feature* TierFeatures_getCurrentFeature(TierFeatures* data);
Feature* TierFeatures_getNextFeature(TierFeatures* data);
Feature* TierFeatures_getExistentFeature(const TierFeatures* data, const char32* label);
void TierFeatures_generateText(TierFeatures* data, MelderString * result);
void TierFeatures_replaceHeadText(TierFeatures* data, const char32* text);
Feature* TierFeatures_getFeature(TierFeatures* data, int position);
int TierFeatures_getNumberOfFeatures(TierFeatures* data);
void TierFeatures_getFeaturesString(TierFeatures* data, MelderString* result);

/* End of file Feature.h */
#endif
