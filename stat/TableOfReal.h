#ifndef _TableOfReal_h_
#define _TableOfReal_h_
/* TableOfReal.h
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
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

/* TableOfReal inherits from Data */
#include "Collection.h"
#include "Strings_.h"
#include "Table.h"
Thing_declare (Interpreter);

#include "TableOfReal_def.h"

void TableOfReal_init (TableOfReal me, integer numberOfRows, integer numberOfColumns);
autoTableOfReal TableOfReal_create (integer numberOfRows, integer numberOfColumns);
void TableOfReal_removeRow (TableOfReal me, integer rowNumber);
void TableOfReal_removeColumn (TableOfReal me, integer columnNumber);
void TableOfReal_insertRow (TableOfReal me, integer rowNumber);
void TableOfReal_insertColumn (TableOfReal me, integer columnNumber);
void TableOfReal_setRowLabel    (TableOfReal me, integer rowNumber, conststring32 label /* cattable */);
void TableOfReal_setColumnLabel (TableOfReal me, integer columnNumber, conststring32 label /* cattable */);
integer TableOfReal_rowLabelToIndex    (TableOfReal me, conststring32 label /* cattable */);
integer TableOfReal_columnLabelToIndex (TableOfReal me, conststring32 label /* cattable */);
double TableOfReal_getColumnMean (TableOfReal me, integer columnNumber);
double TableOfReal_getColumnStdev (TableOfReal me, integer columnNumber);

autoTableOfReal Table_to_TableOfReal (Table me, integer labelColumn);
autoTable TableOfReal_to_Table (TableOfReal me, conststring32 labelOfFirstColumn);
void TableOfReal_formula (TableOfReal me, conststring32 expression, Interpreter interpreter, TableOfReal target);
void TableOfReal_drawAsNumbers (TableOfReal me, Graphics g, integer rowmin, integer rowmax, int iformat, int precision);
void TableOfReal_drawAsNumbers_if (TableOfReal me, Graphics g, integer rowmin, integer rowmax, int iformat, int precision,
	conststring32 conditionFormula, Interpreter interpreter);
void TableOfReal_drawAsSquares (TableOfReal me, Graphics g, integer rowmin, integer rowmax,
	integer colmin, integer colmax, bool garnish);
void TableOfReal_drawVerticalLines (TableOfReal me, Graphics g, integer rowmin, integer rowmax);
void TableOfReal_drawHorizontalLines (TableOfReal me, Graphics g, integer rowmin, integer rowmax);
void TableOfReal_drawLeftAndRightLines (TableOfReal me, Graphics g, integer rowmin, integer rowmax);
void TableOfReal_drawTopAndBottomLines (TableOfReal me, Graphics g, integer rowmin, integer rowmax);

autoTableOfReal TablesOfReal_append (TableOfReal me, TableOfReal thee);
autoTableOfReal TablesOfReal_appendMany (OrderedOf<structTableOfReal>* me);
void TableOfReal_sortByLabel (TableOfReal me, integer column1, integer column2);
void TableOfReal_sortByColumn (TableOfReal me, integer column1, integer column2);

void TableOfReal_writeToHeaderlessSpreadsheetFile (TableOfReal me, MelderFile file);
autoTableOfReal TableOfReal_readFromHeaderlessSpreadsheetFile (MelderFile file);

autoTableOfReal TableOfReal_extractRowRanges (TableOfReal me, conststring32 ranges);
autoTableOfReal TableOfReal_extractColumnRanges (TableOfReal me, conststring32 ranges);

autoTableOfReal TableOfReal_extractRowsWhereColumn (TableOfReal me, integer icol, kMelder_number which, double criterion);
autoTableOfReal TableOfReal_extractColumnsWhereRow (TableOfReal me, integer icol, kMelder_number which, double criterion);

autoTableOfReal TableOfReal_extractRowsWhereLabel (TableOfReal me, kMelder_string which, conststring32 criterion);
autoTableOfReal TableOfReal_extractColumnsWhereLabel (TableOfReal me, kMelder_string which, conststring32 criterion);

autoTableOfReal TableOfReal_extractRowsWhere (TableOfReal me, conststring32 condition, Interpreter interpreter);
autoTableOfReal TableOfReal_extractColumnsWhere (TableOfReal me, conststring32 condition, Interpreter interpreter);

autoStrings TableOfReal_extractRowLabelsAsStrings (TableOfReal me);
autoStrings TableOfReal_extractColumnLabelsAsStrings (TableOfReal me);

#pragma mark - class TableOfRealList

Collection_define (TableOfRealList, OrderedOf, TableOfReal) {
};

/* End of file TableOfReal.h */
#endif
