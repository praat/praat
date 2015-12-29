#ifndef _TableOfReal_h_
#define _TableOfReal_h_
/* TableOfReal.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

/* TableOfReal inherits from Data */
#include "Collection.h"
#include "Strings_.h"
#include "Table.h"
Thing_declare (Interpreter);

#include "TableOfReal_def.h"

void TableOfReal_init (TableOfReal me, long numberOfRows, long numberOfColumns);
autoTableOfReal TableOfReal_create (long numberOfRows, long numberOfColumns);
void TableOfReal_removeRow (TableOfReal me, long irow);
void TableOfReal_removeColumn (TableOfReal me, long icol);
void TableOfReal_insertRow (TableOfReal me, long irow);
void TableOfReal_insertColumn (TableOfReal me, long icol);
void TableOfReal_setRowLabel    (TableOfReal me, long irow, const char32 *label /* cattable */);
void TableOfReal_setColumnLabel (TableOfReal me, long icol, const char32 *label /* cattable */);
long TableOfReal_rowLabelToIndex    (TableOfReal me, const char32 *label /* cattable */);
long TableOfReal_columnLabelToIndex (TableOfReal me, const char32 *label /* cattable */);
double TableOfReal_getColumnMean (TableOfReal me, long icol);
double TableOfReal_getColumnStdev (TableOfReal me, long icol);

autoTableOfReal Table_to_TableOfReal (Table me, long labelColumn);
autoTable TableOfReal_to_Table (TableOfReal me, const char32 *labelOfFirstColumn);
void TableOfReal_formula (TableOfReal me, const char32 *expression, Interpreter interpreter, TableOfReal target);
void TableOfReal_drawAsNumbers (TableOfReal me, Graphics g, long rowmin, long rowmax, int iformat, int precision);
void TableOfReal_drawAsNumbers_if (TableOfReal me, Graphics g, long rowmin, long rowmax, int iformat, int precision,
	const char32 *conditionFormula, Interpreter interpreter);
void TableOfReal_drawAsSquares (TableOfReal me, Graphics g, long rowmin, long rowmax,
	long colmin, long colmax, int garnish);
void TableOfReal_drawVerticalLines (TableOfReal me, Graphics g, long rowmin, long rowmax);
void TableOfReal_drawHorizontalLines (TableOfReal me, Graphics g, long rowmin, long rowmax);
void TableOfReal_drawLeftAndRightLines (TableOfReal me, Graphics g, long rowmin, long rowmax);
void TableOfReal_drawTopAndBottomLines (TableOfReal me, Graphics g, long rowmin, long rowmax);

autoTableOfReal TablesOfReal_append (TableOfReal me, TableOfReal thee);
autoTableOfReal TablesOfReal_appendMany (OrderedOf<structTableOfReal>* me);
void TableOfReal_sortByLabel (TableOfReal me, long column1, long column2);
void TableOfReal_sortByColumn (TableOfReal me, long column1, long column2);

void TableOfReal_writeToHeaderlessSpreadsheetFile (TableOfReal me, MelderFile file);
autoTableOfReal TableOfReal_readFromHeaderlessSpreadsheetFile (MelderFile file);

autoTableOfReal TableOfReal_extractRowRanges (TableOfReal me, const char32 *ranges);
autoTableOfReal TableOfReal_extractColumnRanges (TableOfReal me, const char32 *ranges);

autoTableOfReal TableOfReal_extractRowsWhereColumn (TableOfReal me, long icol, int which_Melder_NUMBER, double criterion);
autoTableOfReal TableOfReal_extractColumnsWhereRow (TableOfReal me, long icol, int which_Melder_NUMBER, double criterion);

autoTableOfReal TableOfReal_extractRowsWhereLabel (TableOfReal me, int which_Melder_STRING, const char32 *criterion);
autoTableOfReal TableOfReal_extractColumnsWhereLabel (TableOfReal me, int which_Melder_STRING, const char32 *criterion);

autoTableOfReal TableOfReal_extractRowsWhere (TableOfReal me, const char32 *condition, Interpreter interpreter);
autoTableOfReal TableOfReal_extractColumnsWhere (TableOfReal me, const char32 *condition, Interpreter interpreter);

autoStrings TableOfReal_extractRowLabelsAsStrings (TableOfReal me);
autoStrings TableOfReal_extractColumnLabelsAsStrings (TableOfReal me);

Collection_declare (OrderedOfTableOfReal, OrderedOf, TableOfReal);

Thing_define (TableOfRealList, OrderedOfTableOfReal) {
	structTableOfRealList () {
		our classInfo = classTableOfRealList;
	}
};

/* End of file TableOfReal.h */
#endif
