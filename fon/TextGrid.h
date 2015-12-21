#ifndef _TextGrid_h_
#define _TextGrid_h_
/* TextGrid.h
 *
 * Copyright (C) 1992-2012,2014,2015 Paul Boersma
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
 */

#include "AnyTier.h"
#include "Label.h"
#include "Graphics.h"
#include "TableOfReal.h"
#include "Table.h"

#include "TextGrid_def.h"

oo_CLASS_CREATE (TextPoint);
autoTextPoint TextPoint_create (double time, const char32 *mark);

void TextPoint_setText (TextPoint me, const char32 *text);

oo_CLASS_CREATE (TextInterval);
autoTextInterval TextInterval_create (double tmin, double tmax, const char32 *text);

void TextInterval_setText (TextInterval me, const char32 *text);

oo_CLASS_CREATE (TextTier);
autoTextTier TextTier_create (double tmin, double tmax);

void TextTier_addPoint (TextTier me, double time, const char32 *mark);
autoTextTier TextTier_readFromXwaves (MelderFile file);
autoPointProcess TextTier_getPoints (TextTier me, const char32 *text);

oo_CLASS_CREATE (IntervalTier);
autoIntervalTier IntervalTier_create (double tmin, double tmax);
autoIntervalTier IntervalTier_readFromXwaves (MelderFile file);
void IntervalTier_writeToXwaves (IntervalTier me, MelderFile file);

long IntervalTier_timeToLowIndex (IntervalTier me, double t);
long IntervalTier_timeToIndex (IntervalTier me, double t);   // obsolete
long IntervalTier_timeToHighIndex (IntervalTier me, double t);
long IntervalTier_hasTime (IntervalTier me, double t);
long IntervalTier_hasBoundary (IntervalTier me, double t);
autoPointProcess IntervalTier_getStartingPoints (IntervalTier me, const char32 *text);
autoPointProcess IntervalTier_getEndPoints (IntervalTier me, const char32 *text);
autoPointProcess IntervalTier_getCentrePoints (IntervalTier me, const char32 *text);
autoPointProcess IntervalTier_PointProcess_startToCentre (IntervalTier tier, PointProcess point, double phase);
autoPointProcess IntervalTier_PointProcess_endToCentre (IntervalTier tier, PointProcess point, double phase);
void IntervalTier_removeLeftBoundary (IntervalTier me, long iinterval);

void TextTier_removePoint (TextTier me, long ipoint);

oo_CLASS_CREATE (TextGrid);
autoTextGrid TextGrid_createWithoutTiers (double tmin, double tmax);
autoTextGrid TextGrid_create (double tmin, double tmax, const char32 *tierNames, const char32 *pointTiers);

long TextGrid_countLabels (TextGrid me, long itier, const char32 *text);
long TextGrid_countIntervalsWhere (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion);
long TextGrid_countPointsWhere (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion);
autoPointProcess TextGrid_getStartingPoints (TextGrid me, long itier, int which_Melder_STRING, const char32 *criterion);
autoPointProcess TextGrid_getEndPoints (TextGrid me, long itier, int which_Melder_STRING, const char32 *criterion);
autoPointProcess TextGrid_getCentrePoints (TextGrid me, long itier, int which_Melder_STRING, const char32 *criterion);
autoPointProcess TextGrid_getPoints (TextGrid me, long itier, int which_Melder_STRING, const char32 *criterion);
autoPointProcess TextGrid_getPoints_preceded (TextGrid me, long tierNumber,
	int which_Melder_STRING, const char32 *criterion,
	int which_Melder_STRING_precededBy, const char32 *criterion_precededBy);
autoPointProcess TextGrid_getPoints_followed (TextGrid me, long tierNumber,
	int which_Melder_STRING, const char32 *criterion,
	int which_Melder_STRING_followedBy, const char32 *criterion_followedBy);

Function TextGrid_checkSpecifiedTierNumberWithinRange (TextGrid me, long tierNumber);
IntervalTier TextGrid_checkSpecifiedTierIsIntervalTier (TextGrid me, long tierNumber);
TextTier TextGrid_checkSpecifiedTierIsPointTier (TextGrid me, long tierNumber);

void TextGrid_addTier_copy (TextGrid me, Function tier);
autoTextGrid TextGrid_merge (Collection textGrids);
autoTextGrid TextGrid_extractPart (TextGrid me, double tmin, double tmax, int preserveTimes);

autoTextGrid Label_to_TextGrid (Label me, double duration);
autoTextGrid Label_Function_to_TextGrid (Label me, Function function);

autoTextTier PointProcess_upto_TextTier (PointProcess me, const char32 *text);
autoTableOfReal IntervalTier_downto_TableOfReal (IntervalTier me, const char32 *label);
autoTableOfReal IntervalTier_downto_TableOfReal_any (IntervalTier me);
autoTableOfReal TextTier_downto_TableOfReal (TextTier me, const char32 *label);
autoTableOfReal TextTier_downto_TableOfReal_any (TextTier me);

autoTextGrid PointProcess_to_TextGrid_vuv (PointProcess me, double maxT, double meanT);

long TextInterval_labelLength (TextInterval me);
long TextPoint_labelLength (TextPoint me);
long IntervalTier_maximumLabelLength (IntervalTier me);
long TextTier_maximumLabelLength (TextTier me);
long TextGrid_maximumLabelLength (TextGrid me);
void TextGrid_genericize (TextGrid me);
void TextGrid_nativize (TextGrid me);

void TextInterval_removeText (TextInterval me);
void TextPoint_removeText (TextPoint me);
void IntervalTier_removeText (IntervalTier me);
void TextTier_removeText (TextTier me);

void TextTier_removePoints (TextTier me, int which_Melder_STRING, const char32 *criterion);
void TextGrid_removePoints (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion);

void TextGrid_insertBoundary (TextGrid me, int itier, double t);
void TextGrid_removeBoundaryAtTime (TextGrid me, int itier, double t);
void TextGrid_setIntervalText (TextGrid me, int itier, long iinterval, const char32 *text);
void TextGrid_insertPoint (TextGrid me, int itier, double t, const char32 *mark);
void TextGrid_setPointText (TextGrid me, int itier, long ipoint, const char32 *text);

void TextGrid_writeToChronologicalTextFile (TextGrid me, MelderFile file);
autoTextGrid TextGrid_readFromChronologicalTextFile (MelderFile file);
autoTextGrid TextGrid_readFromCgnSyntaxFile (MelderFile file);

autoTable TextGrid_downto_Table (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals);
void TextGrid_list (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals);

void TextGrid_correctRoundingErrors (TextGrid me);
autoTextGrid TextGrids_concatenate (Collection me);

/* End of file TextGrid.h */
#endif
