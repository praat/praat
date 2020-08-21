#ifndef _TextGrid_h_
#define _TextGrid_h_
/* TextGrid.h
 *
 * Copyright (C) 1992-2012,2014-2018,2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AnyTier.h"
#include "Label.h"
#include "Graphics.h"
#include "TableOfReal.h"
#include "Table.h"

Collection_define (FunctionList, OrderedOf, Function) {
};

#include "TextGrid_def.h"

autoTextPoint TextPoint_create (double time, conststring32 mark);

void TextPoint_setText (TextPoint me, conststring32 text);

autoTextInterval TextInterval_create (double tmin, double tmax, conststring32 text);

void TextInterval_setText (TextInterval me, conststring32 text);

autoTextTier TextTier_create (double tmin, double tmax);

void TextTier_addPoint (TextTier me, double time, conststring32 mark);
autoTextTier TextTier_readFromXwaves (MelderFile file);
autoPointProcess TextTier_getPoints (TextTier me, conststring32 text);

autoIntervalTier IntervalTier_create (double tmin, double tmax);
autoIntervalTier IntervalTier_readFromXwaves (MelderFile file);
void IntervalTier_writeToXwaves (IntervalTier me, MelderFile file);

integer IntervalTier_timeToLowIndex (IntervalTier me, double t);
integer IntervalTier_timeToIndex (IntervalTier me, double t);   // obsolete
integer IntervalTier_timeToHighIndex (IntervalTier me, double t);
integer IntervalTier_hasTime (IntervalTier me, double t);
integer IntervalTier_hasBoundary (IntervalTier me, double t);
autoPointProcess IntervalTier_getStartingPoints (IntervalTier me, conststring32 text);
autoPointProcess IntervalTier_getEndPoints (IntervalTier me, conststring32 text);
autoPointProcess IntervalTier_getCentrePoints (IntervalTier me, conststring32 text);
autoPointProcess IntervalTier_PointProcess_startToCentre (IntervalTier tier, PointProcess point, double phase);
autoPointProcess IntervalTier_PointProcess_endToCentre (IntervalTier tier, PointProcess point, double phase);
void IntervalTier_removeLeftBoundary (IntervalTier me, integer intervalNumber);

void TextTier_removePoint (TextTier me, integer pointNumber);

autoTextGrid TextGrid_createWithoutTiers (double tmin, double tmax);
autoTextGrid TextGrid_create (double tmin, double tmax, conststring32 tierNames, conststring32 pointTiers);

integer TextGrid_countLabels (TextGrid me, integer itier, conststring32 text);
integer TextGrid_countIntervalsWhere (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
integer TextGrid_countPointsWhere (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getStartingPoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getEndPoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getCentrePoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getPoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getPoints_preceded (TextGrid me, integer tierNumber,
	kMelder_string which, conststring32 criterion,
	kMelder_string precededBy, conststring32 criterion_precededBy);
autoPointProcess TextGrid_getPoints_followed (TextGrid me, integer tierNumber,
	kMelder_string which, conststring32 criterion,
	kMelder_string followedBy, conststring32 criterion_followedBy);

Function TextGrid_checkSpecifiedTierNumberWithinRange (TextGrid me, integer tierNumber);
IntervalTier TextGrid_checkSpecifiedTierIsIntervalTier (TextGrid me, integer tierNumber);
TextTier TextGrid_checkSpecifiedTierIsPointTier (TextGrid me, integer tierNumber);
void AnyTextGridTier_identifyClass (Function anyTextGridTier, IntervalTier *intervalTier, TextTier *textTier);

void TextGrid_addTier_copy (TextGrid me, Function tier);
autoTextGrid TextGrids_merge (OrderedOf<structTextGrid>* textGrids);
autoTextGrid TextGrid_extractPart (TextGrid me, double tmin, double tmax, bool preserveTimes);

autoTextGrid Label_to_TextGrid (Label me, double duration);
autoTextGrid Label_Function_to_TextGrid (Label me, Function function);

autoTextTier PointProcess_upto_TextTier (PointProcess me, conststring32 text);
autoTableOfReal IntervalTier_downto_TableOfReal (IntervalTier me, conststring32 label);
autoTableOfReal IntervalTier_downto_TableOfReal_any (IntervalTier me);
autoTableOfReal TextTier_downto_TableOfReal (TextTier me, conststring32 label);
autoTableOfReal TextTier_downto_TableOfReal_any (TextTier me);

autoTextGrid PointProcess_to_TextGrid_vuv (PointProcess me, double maxT, double meanT);

integer TextInterval_labelLength (TextInterval me);
integer TextPoint_labelLength (TextPoint me);
integer IntervalTier_maximumLabelLength (IntervalTier me);
integer TextTier_maximumLabelLength (TextTier me);
integer TextGrid_maximumLabelLength (TextGrid me);
void TextGrid_convertToBackslashTrigraphs (TextGrid me);
void TextGrid_convertToUnicode (TextGrid me);

void TextInterval_removeText (TextInterval me);
void TextPoint_removeText (TextPoint me);
void IntervalTier_removeText (IntervalTier me);
void TextTier_removeText (TextTier me);

void TextTier_removePoints (TextTier me, kMelder_string which, conststring32 criterion);
void TextGrid_removePoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);

void TextGrid_insertBoundary (TextGrid me, integer tierNumber, double t);
void TextGrid_removeBoundaryAtTime (TextGrid me, integer tierNumber, double t);
void TextGrid_setIntervalText (TextGrid me, integer tierNumber, integer intervalNumber, conststring32 text);
void TextGrid_insertPoint (TextGrid me, integer tierNumber, double t, conststring32 mark);
void TextGrid_setPointText (TextGrid me, integer tierNumber, integer pointNumber, conststring32 text);

void TextGrid_writeToChronologicalTextFile (TextGrid me, MelderFile file);
autoTextGrid TextGrid_readFromChronologicalTextFile (MelderFile file);
autoTextGrid TextGrid_readFromCgnSyntaxFile (MelderFile file);

autoTable TextGrid_downto_Table (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals);
autoTable TextGrid_tabulateOccurrences (TextGrid me, constVEC searchTiers, kMelder_string which, conststring32 criterion, bool caseSensitive);
void TextGrid_list (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals);

void TextGrid_correctRoundingErrors (TextGrid me);
autoTextGrid TextGrids_concatenate (OrderedOf<structTextGrid>* me);

/* End of file TextGrid.h */
#endif
