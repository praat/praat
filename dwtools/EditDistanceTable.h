#ifndef _EditDistanceTable_h_
#define _EditDistanceTable_h_
/* EditDistanceTable.h
 *
 * Copyright (C) 2012,2015-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Strings_extensions.h"
#include "TableOfReal.h"

#define WARPING_fromLeft 1
#define WARPING_fromBelow 2
#define WARPING_fromDiag 4

#include "EditDistanceTable_def.h"

autoWarpingPath WarpingPath_create (integer length);

// Search the path for the corresponding axis value. If path is horizontal/vertical
//  ivar1 and ivar2 will not be equal. The return value is the length of the path segment (ivar2-ivar1 +1)
integer WarpingPath_getColumnsFromRowIndex (WarpingPath me, integer irow, integer *icol1, integer *icol2);

integer WarpingPath_getRowsFromColumnIndex (WarpingPath me, integer icol, integer *irow1, integer *irow2);


autoEditCostsTable EditCostsTable_create (integer targetAlphabetSize, integer sourceAlphabetSize);
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

integer EditCostsTable_getTargetIndex (EditCostsTable me, conststring32 symbol);

integer EditCostsTable_getSourceIndex (EditCostsTable me, conststring32 symbol);

double EditCostsTable_getInsertionCost (EditCostsTable me, conststring32 symbol);

void EditCostsTable_setInsertionCosts (EditCostsTable me, conststring32 targets, double cost);

void EditCostsTable_setOthersCosts (EditCostsTable me, double insertionCosts, double deletionCost, double substitutionCost_equal, double substitutionCost_unequal);

double EditCostsTable_getOthersCost (EditCostsTable me, int type);

double EditCostsTable_getDeletionCost (EditCostsTable me, conststring32 symbol);

void EditCostsTable_setDeletionCosts (EditCostsTable me, conststring32 sources, double cost);

double EditCostsTable_getSubstitutionCost (EditCostsTable me, conststring32 symbol, conststring32 replacement);

void EditCostsTable_setSubstitutionCosts (EditCostsTable me, conststring32 targets, conststring32 sources, double cost);

autoTableOfReal EditCostsTable_to_TableOfReal (EditCostsTable me);


autoEditDistanceTable EditDistanceTable_create (Strings target, Strings source);

autoEditDistanceTable EditDistanceTable_createFromCharacterStrings (const char32 chars1 [], const char32 chars2 []);

void EditDistanceTable_draw (EditDistanceTable me, Graphics graphics, int iformat, int precision, double angle);

void EditDistanceTable_drawEditOperations (EditDistanceTable me, Graphics graphics);

void EditDistanceTable_setDefaultCosts (EditDistanceTable me, double insertionCosts, double deletionCosts, double substitutionCosts);

void EditDistanceTable_findPath (EditDistanceTable me, autoTableOfReal *directions);

void EditDistanceTable_setEditCosts (EditDistanceTable me, EditCostsTable thee);

autoTableOfReal EditDistanceTable_to_TableOfReal_directions (EditDistanceTable me);

autoTableOfReal EditDistanceTable_to_TableOfReal (EditDistanceTable me);

#endif /* _EditDistanceTable_h_ */
