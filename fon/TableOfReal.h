#ifndef _TableOfReal_h_
#define _TableOfReal_h_
/* TableOfReal.h
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2005/03/04
 */


/* TableOfReal inherits from Data */
#ifndef _Matrix_h_
	#include "Matrix.h"
#endif
#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _Strings_h_
	#include "Strings.h"
#endif

/* For the inheritors. */
#define TableOfReal_members Data_members \
	long numberOfRows, numberOfColumns; \
	char **rowLabels, **columnLabels; \
	double **data;
#define TableOfReal_methods Data_methods
class_create (TableOfReal, Data)

int TableOfReal_init (I, long numberOfRows, long numberOfColumns);
TableOfReal TableOfReal_create (long numberOfRows, long numberOfColumns);
int TableOfReal_removeRow (I, long irow);
int TableOfReal_removeColumn (I, long icol);
int TableOfReal_insertRow (I, long irow);
int TableOfReal_insertColumn (I, long icol);
void TableOfReal_setRowLabel (I, long irow, const char *label);
void TableOfReal_setColumnLabel (I, long icol, const char *label);
long TableOfReal_rowLabelToIndex (I, const char *label);
long TableOfReal_columnLabelToIndex (I, const char *label);
double TableOfReal_getColumnMean (I, long icol);
double TableOfReal_getColumnStdev (I, long icol);

Matrix TableOfReal_to_Matrix (I);
TableOfReal Matrix_to_TableOfReal (I);
int TableOfReal_formula (I, const char *expression, Any /* TableOfReal */ target);
void TableOfReal_drawAsNumbers (I, Graphics g, long rowmin, long rowmax, int iformat, int precision);
void TableOfReal_drawAsNumbers_if (I, Graphics g, long rowmin, long rowmax, int iformat, int precision,
	const char *conditionFormula);
void TableOfReal_drawAsSquares (I, Graphics g, long rowmin, long rowmax,
	long colmin, long colmax, int garnish);
void TableOfReal_drawVerticalLines (I, Graphics g, long rowmin, long rowmax);
void TableOfReal_drawHorizontalLines (I, Graphics g, long rowmin, long rowmax);
void TableOfReal_drawLeftAndRightLines (I, Graphics g, long rowmin, long rowmax);
void TableOfReal_drawTopAndBottomLines (I, Graphics g, long rowmin, long rowmax);

Any TablesOfReal_append (I, thou);
Any TablesOfReal_appendMany (Collection me);
void TableOfReal_sortByLabel (I, long column1, long column2);
void TableOfReal_sortByColumn (I, long column1, long column2);

int TableOfReal_writeToHeaderlessSpreadsheetFile (TableOfReal me, MelderFile file);
TableOfReal TableOfReal_readFromHeaderlessSpreadsheetFile (MelderFile file);

TableOfReal TableOfReal_extractRowRanges (I, const char *ranges);
TableOfReal TableOfReal_extractColumnRanges (I, const char *ranges);

TableOfReal TableOfReal_extractRowsWhereColumn (I, long icol, enum Melder_NUMBER which, double criterion);
TableOfReal TableOfReal_extractColumnsWhereRow (I, long icol, enum Melder_NUMBER which, double criterion);

TableOfReal TableOfReal_extractRowsWhereLabel (I, enum Melder_STRING which, const char *criterion);
TableOfReal TableOfReal_extractColumnsWhereLabel (I, enum Melder_STRING which, const char *criterion);

TableOfReal TableOfReal_extractRowsWhere (I, const char *condition);
TableOfReal TableOfReal_extractColumnsWhere (I, const char *condition);

Strings TableOfReal_extractRowLabelsAsStrings (I);
Strings TableOfReal_extractColumnLabelsAsStrings (I);

#endif
/* End of file TableOfReal.h */
