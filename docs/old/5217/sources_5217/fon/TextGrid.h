#ifndef _TextGrid_h_
#define _TextGrid_h_
/* TextGrid.h
 *
 * Copyright (C) 1992-2010 Paul Boersma
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

/*
 * pb 2010/12/08
 */

#ifndef _AnyTier_h_
	#include "AnyTier.h"
#endif
#ifndef _Label_h_
	#include "Label.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif
#ifndef _Table_h_
	#include "Table.h"
#endif

#include "TextGrid_def.h"

#define TextPoint_methods AnyPoint_methods
oo_CLASS_CREATE (TextPoint, AnyPoint);
TextPoint TextPoint_create (double time, const wchar_t *mark);

int TextPoint_setText (TextPoint me, const wchar_t *text);

#define TextInterval_methods Function_methods
oo_CLASS_CREATE (TextInterval, Function);
TextInterval TextInterval_create (double tmin, double tmax, const wchar_t *text);

long TextGrid_countLabels (TextGrid me, long itier, const wchar_t *text);
int TextInterval_setText (TextInterval me, const wchar_t *text);

#define TextTier_methods Function_methods
oo_CLASS_CREATE (TextTier, Function);
TextTier TextTier_create (double tmin, double tmax);

int TextTier_addPoint (TextTier me, double time, const wchar_t *mark);
TextTier TextTier_readFromXwaves (MelderFile file);
PointProcess TextTier_getPoints (TextTier me, const wchar_t *text);

#define IntervalTier_methods Function_methods
oo_CLASS_CREATE (IntervalTier, Function);
IntervalTier IntervalTier_create (double tmin, double tmax);
IntervalTier IntervalTier_readFromXwaves (MelderFile file);
int IntervalTier_writeToXwaves (IntervalTier me, MelderFile file);

long IntervalTier_timeToLowIndex (IntervalTier me, double t);
long IntervalTier_timeToIndex (IntervalTier me, double t);   /* Obsolete. */
long IntervalTier_timeToHighIndex (IntervalTier me, double t);
long IntervalTier_hasTime (IntervalTier me, double t);
long IntervalTier_hasBoundary (IntervalTier me, double t);
PointProcess IntervalTier_getStartingPoints (IntervalTier me, const wchar_t *text);
PointProcess IntervalTier_getEndPoints (IntervalTier me, const wchar_t *text);
PointProcess IntervalTier_getCentrePoints (IntervalTier me, const wchar_t *text);
PointProcess TextGrid_getStartingPoints (TextGrid me, long itier, int which_Melder_STRING, const wchar_t *criterion);
PointProcess TextGrid_getEndPoints (TextGrid me, long itier, int which_Melder_STRING, const wchar_t *criterion);
PointProcess TextGrid_getCentrePoints (TextGrid me, long itier, int which_Melder_STRING, const wchar_t *criterion);
PointProcess TextGrid_getPoints (TextGrid me, long itier, int which_Melder_STRING, const wchar_t *criterion);

PointProcess IntervalTier_PointProcess_startToCentre (IntervalTier tier, PointProcess point, double phase);
PointProcess IntervalTier_PointProcess_endToCentre (IntervalTier tier, PointProcess point, double phase);
int IntervalTier_removeLeftBoundary (IntervalTier me, long iinterval);

void TextTier_removePoint (TextTier me, long ipoint);

#define TextGrid_methods Function_methods
oo_CLASS_CREATE (TextGrid, Function);
TextGrid TextGrid_createWithoutTiers (double tmin, double tmax);
TextGrid TextGrid_create (double tmin, double tmax, const wchar_t *tierNames, const wchar_t *pointTiers);

int TextGrid_add (TextGrid me, Any tier);
TextGrid TextGrid_merge (Collection textGrids);
TextGrid TextGrid_extractPart (TextGrid me, double tmin, double tmax, int preserveTimes);

TextGrid Label_to_TextGrid (Label me, double duration);
TextGrid Label_Function_to_TextGrid (Label me, Any function);

TextTier PointProcess_upto_TextTier (PointProcess me, const wchar_t *text);
TableOfReal IntervalTier_downto_TableOfReal (IntervalTier me, const wchar_t *label);
TableOfReal IntervalTier_downto_TableOfReal_any (IntervalTier me);
TableOfReal TextTier_downto_TableOfReal (TextTier me, const wchar_t *label);
TableOfReal TextTier_downto_TableOfReal_any (TextTier me);

TextGrid PointProcess_to_TextGrid_vuv (PointProcess me, double maxT, double meanT);

long TextInterval_labelLength (TextInterval me);
long TextPoint_labelLength (TextPoint me);
long IntervalTier_maximumLabelLength (IntervalTier me);
long TextTier_maximumLabelLength (TextTier me);
long TextGrid_maximumLabelLength (TextGrid me);
int TextGrid_genericize (TextGrid me);
int TextGrid_nativize (TextGrid me);

void TextInterval_removeText (TextInterval me);
void TextPoint_removeText (TextPoint me);
void IntervalTier_removeText (IntervalTier me);
void TextTier_removeText (TextTier me);

int TextGrid_insertBoundary (TextGrid me, int itier, double t);
int TextGrid_removeBoundaryAtTime (TextGrid me, int itier, double t);
int TextGrid_setIntervalText (TextGrid me, int itier, long iinterval, const wchar_t *text);
int TextGrid_insertPoint (TextGrid me, int itier, double t, const wchar_t *mark);
int TextGrid_setPointText (TextGrid me, int itier, long ipoint, const wchar_t *text);

int TextGrid_writeToChronologicalTextFile (TextGrid me, MelderFile file);
TextGrid TextGrid_readFromChronologicalTextFile (MelderFile file);
TextGrid TextGrid_readFromCgnSyntaxFile (MelderFile file);

Table TextGrid_downto_Table (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals);
void TextGrid_list (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals);

#endif
/* End of file TextGrid.h */
