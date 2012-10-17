#ifndef _EditDistanceTable_h_
#define _EditDistanceTable_h_
/* EditDistanceTable.h
 *
 * Copyright (C) 2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Strings_extensions.h"
#include "TableOfReal.h"

#define WARPING_fromLeft 1
#define WARPING_fromBelow 2
#define WARPING_fromDiag 4

#include "EditDistanceTable_def.h"
oo_CLASS_CREATE (WarpingPath, Data);
oo_CLASS_CREATE (EditCostsTable, TableOfReal);
oo_CLASS_CREATE (EditDistanceTable, TableOfReal);

WarpingPath WarpingPath_create (long length);

// Search the path for the corresponding axis value. If path is horizontal/vertical
//  ivar1 and ivar2 will not ge eqal. The return value is the length of the path segment (ivar2-ivar1 +1)
long WarpingPath_getColumnsFromRowIndex (WarpingPath me, long irow, long *icol1, long *icol2);
long WarpingPath_getRowsFromColumnIndex (WarpingPath me, long icol, long *irow1, long *irow2);


EditCostsTable EditCostsTable_create (long targetAlphabetSize, long sourceAlphabetSize);
/* The insertion, deletion and substitution costs are specified in this table
 * 1..n-2 target symbols (target alphabet)
 * 1..m-2 source symbols (source alphabet)
 * row n-1 and col m-1 specify no-match symbols
 * cells [n][1..m-1] specify insertion costs
 * cells [1..n-1][m] specify deletion costs
 * cell [n-1][m-1] no-match target == no-match source
 * cell [n][m] no-match target != no-match source
 */

void EditCostsTable_setDefaultCosts (EditCostsTable me, double insertionCosts, double deletionCosts, double substitutionCosts);
long EditCostsTable_getTargetIndex (EditCostsTable me, const wchar_t *symbol);
long EditCostsTable_getSourceIndex (EditCostsTable me, const wchar_t *symbol);
double EditCostsTable_getInsertionCost (EditCostsTable me, const wchar_t *symbol);
void EditCostsTable_setInsertionCosts (EditCostsTable me, wchar_t *targets, double cost);
void EditCostsTable_setOthersCosts (EditCostsTable me, double insertionCosts, double deletionCost, double substitutionCost_equal, double substitutionCost_unequal);
double EditCostsTable_getOthersCost (EditCostsTable me, int type);
double EditCostsTable_getDeletionCost (EditCostsTable me, const wchar_t *symbol);
void EditCostsTable_setDeletionCosts (EditCostsTable me, wchar_t *sources, double cost);
double EditCostsTable_getSubstitutionCost (EditCostsTable me, const wchar_t *symbol, const wchar *replacement);
void EditCostsTable_setSubstitutionCosts (EditCostsTable me, wchar_t *targets, wchar_t *sources, double cost);
TableOfReal EditCostsTable_to_TableOfReal (EditCostsTable me);


EditDistanceTable EditDistanceTable_create (Strings target, Strings source);
EditDistanceTable EditDistanceTable_createFromCharacterStrings (const wchar_t *chars1, const wchar_t *chars2);
void EditDistanceTable_draw (EditDistanceTable me, Graphics graphics, int iformat, int precision, double angle);
void EditDistanceTable_drawEditOperations (EditDistanceTable me, Graphics graphics);
void EditDistanceTable_setDefaultCosts (EditDistanceTable me, double insertionCosts, double deletionCosts, double substitutionCosts);
void EditDistanceTable_findPath (EditDistanceTable me, TableOfReal *directions);

void EditDistanceTable_setEditCosts (EditDistanceTable me, EditCostsTable thee);

TableOfReal EditDistanceTable_to_TableOfReal_directions (EditDistanceTable me);

TableOfReal EditDistanceTable_to_TableOfReal (EditDistanceTable me);

#endif /* _EditDistanceTable_h_ */
