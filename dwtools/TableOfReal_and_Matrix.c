/* TableOfReal_and_Matrix.c
 *
 * Copyright (C) 1993-2002 David Weenink
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

/*
 djmw 20020813 GPL header
*/

#include "TableOfReal_and_Matrix.h"

/*
	Because TableOfReal::Formula... cannot handle references to Matrices...
*/

int TableOfReal_matrixColumnsIntoRows (I, thou, long cb_from, long ce_from,
	long rb_to, long cb_to)
{
	iam (TableOfReal);
	thouart (Matrix);
	long i, j, ncols_from;
	
	if (cb_from == 0 && ce_from == 0)
	{
		cb_from = 1; ce_from = thy nx;
	}
	if (cb_from > ce_from || cb_from < 1 || ce_from > thy nx)
		return Melder_error ("TableOfReal_matrixColumnsIntoRows: "
			"Column indices out of range.");
			
	ncols_from = ce_from - cb_from + 1;
	
	if (rb_to == 0) rb_to = 1;
	if (rb_to < 1 || rb_to > my numberOfRows) return Melder_error
		("TableOfReal_matrixColumnsIntoRows: Row index out of range.");
	if (rb_to + ncols_from - 1 > my numberOfRows) return Melder_error
		("TableOfReal_matrixColumnsIntoRows: not enough rows available.");
	if (cb_to + thy ny - 1 > my numberOfColumns) return Melder_error
		("TableOfReal_matrixColumnsIntoRows: not enough columns available.");
	
	for (i = rb_to; i < rb_to + ncols_from; i++)
	{
		for (j = cb_to; j < cb_to + thy ny; j++)
		{
			my data[i][j] = thy z[j - cb_to + 1][i - rb_to + cb_from];
		}
	}
	return 1;		
}

/* End of file TableOfReal_and_Matrix.c */
