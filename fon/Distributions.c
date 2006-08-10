/* Distributions.c
 *
 * Copyright (C) 1997-2003 Paul Boersma
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
 * pb 1999/09/08
 * pb 2002/07/16 GPL
 * pb 2003/07/28 Distributions_peek
 */

#include "Distributions.h"

static void info (I) {
	iam (TableOfReal);
	Melder_information ("Number of distributions = %ld\n"
		"Number of values = %ld",
		my numberOfColumns, my numberOfRows);
}

class_methods (Distributions, TableOfReal)
	class_method (info)
class_methods_end

Distributions Distributions_create (long numberOfRows, long numberOfColumns) {
	Distributions me = new (Distributions);
	if (! me || ! TableOfReal_init (me, numberOfRows, numberOfColumns)) forget (me);
	return me;
}

int Distributions_peek (Distributions me, long column, char **string) {
	double total = 0.0;
	long irow;
	if (column > my numberOfColumns) { Melder_error ("No column %ld.", column); goto end; }
	if (my numberOfRows < 1) { Melder_error ("No candidates."); goto end; }
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		total += my data [irow] [column];
	}
	if (total <= 0.0) { Melder_error ("Column total not positive."); goto end; }
	do {
		double rand = NUMrandomUniform (0, total), sum = 0.0;
		for (irow = 1; irow <= my numberOfRows; irow ++) {
			sum += my data [irow] [column];
			if (rand <= sum) break;
		}
	} while (irow > my numberOfRows);   /* Guard against rounding errors. */
	*string = my rowLabels [irow];
	if (! *string) { Melder_error ("No string in row %ld.", irow); goto end; }
end:
	iferror return Melder_error ("(Distributions_peek:) Not performed.");
	return 1;
}

double Distributions_getProbability (Distributions me, const char *string, long column) {
	long row, rowOfString = 0;
	double total = 0.0;
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	for (row = 1; row <= my numberOfRows; row ++) {
		total += my data [row] [column];
		if (my rowLabels [row] && strequ (my rowLabels [row], string))
			rowOfString = row;
	}
	if (total <= 0.0) return NUMundefined;
	if (rowOfString == 0) return 0.0;
	return my data [rowOfString] [column] / total;
}

double Distributionses_getMeanAbsoluteDifference (Distributions me, Distributions thee, long column) {
	long row;
	double total = 0.0;
	if (column < 1 || column > my numberOfColumns || column > thy numberOfColumns ||
	    my numberOfRows != thy numberOfRows) return NUMundefined;
	for (row = 1; row <= my numberOfRows; row ++) {
		total += fabs (my data [row] [column] - thy data [row] [column]);
	}
	return total / my numberOfRows;
}

static void unicize (Distributions me) {
	/* Must have been sorted beforehand. */
	long nrow = 0, ifrom = 1, ito, i, j, icol;
	for (i = 1; i <= my numberOfRows; i ++) {
		if (i == my numberOfRows || (my rowLabels [i] == NULL) != (my rowLabels [i + 1] == NULL) ||
		    (my rowLabels [i] != NULL && ! strequ (my rowLabels [i], my rowLabels [i + 1])))
		{
			/*
			 * Detected a change.
			 */
			nrow ++;
			ito = i;
			/*
			 * Move row 'ifrom' to 'nrow'. May be the same row.
			 */
			if (ifrom != nrow) {
				Melder_free (my rowLabels [nrow]);
				my rowLabels [nrow] = my rowLabels [ifrom];   /* Surface copy. */
				my rowLabels [ifrom] = NULL;   /* Undangle. */
				for (icol = 1; icol <= my numberOfColumns; icol ++)
					my data [nrow] [icol] = my data [ifrom] [icol];
			}
			/*
			 * Purge rows from 'ifrom'+1 to 'ito'.
			 */
			for (j = ifrom + 1; j <= ito; j ++) {
				Melder_free (my rowLabels [j]);
				for (icol = 1; icol <= my numberOfColumns; icol ++)
					my data [nrow] [icol] += my data [j] [icol];
			}
			ifrom = ito + 1;
		}
	}
	my numberOfRows = nrow;
}

Distributions Distributions_addTwo (Distributions me, Distributions thee) {
	Distributions him = TablesOfReal_append (me, thee);
	if (! him) return NULL;
	TableOfReal_sortByLabel (him, 0, 0);
	unicize (him);
	return him;
}

Distributions Distributions_addMany (Collection me) {
	Distributions thee = TablesOfReal_appendMany (me);
	if (! thee) return NULL;
	TableOfReal_sortByLabel (thee, 0, 0);
	unicize (thee);
	return thee;
}

/* End of file Distributions.c */
