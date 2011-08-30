#ifndef _Confusion_h_
#define _Confusion_h_
/* Confusion.h
 *
 * Copyright (C) 1993-2011 David Weenink
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

#include "TableOfReal.h"
#include "Categories.h"
#include "Graphics.h"
#include "Matrix.h"

Thing_define (Confusion, TableOfReal) {
	// overridden methods:
		virtual void v_info ();
};

/*
	A Confusion matrix has both row and column labels.
*/

Confusion Confusion_create (long numberOfStimuli, long numberOfResponses);

Confusion Confusion_createSimple (const wchar *labels);

Confusion Categories_to_Confusion (Categories me, Categories thee);

void Confusion_increase (Confusion me, const wchar_t *stimulus, const wchar_t *response);
/* data['stim']['resp'] += 1; */

double Confusion_getValue (Confusion me, const wchar_t *stim, const wchar_t *resp);
/* data['stim']['resp'] ; */

void Confusion_getEntropies (Confusion me, double *h, double *hx, double *hy,
    double *hygx, double *hxgy, double *uygx, double *uxgy, double *uxy);
/*  x is column variable, y is row variable
 *  *h	    entropy of whole table;
 *  *hx	    entropy of x variable
 *  *hy	    entropy of y variable
 *  *hygx   entropy of y given x
 *  *hxgy   entropy of x given y
 *  *uygx   dependency of y on x
 *  *uxgy   dependency of x on y
 *  *uxy    symmetrical dependency
 */

void Confusion_getFractionCorrect (Confusion me, double *fraction, long *numberOfCorrect);

void Confusion_Matrix_draw (Confusion me, Matrix thee, Graphics g,
	long index, double lowerPercentage, double xmin, double xmax,
	double ymin, double ymax, int garnish);
/* 1. Draw my rowLabels centered at ( matrix->z[i][1], matrix->z[i][2]).
 * 2. Draw arrows and circles according to:
 *	for (i=1; i <= my numberOfRows; i++)
 *	{
 *		if (index != 0 && index != i) continue;
 *      draw circle at i of width: my z[i][i]/rowSum;
 *		for (j=1; j <= my numberOfColumns; j++)
 *		{
 *			if (i != j && 100*my data[i][j]/rowSum > lowerPercentage)
 *				draw arrow from i to j of width: my data[i][j]/rowSum;
 *		}
 *	}
 */

Matrix Confusion_difference (Confusion me, Confusion thee);
/* return matrix with the difference between the two confusion matrices */

long Confusion_getNumberOfEntries (Confusion me);

Confusion Confusion_groupStimuli (Confusion me, const wchar_t *labels, const wchar_t *newLabel, long newpos);
Confusion Confusion_groupResponses (Confusion me, const wchar_t *labels, const wchar_t *newLabel, long newpos);
Confusion Confusion_group (Confusion me, const wchar_t *labels, const wchar_t *newLabel, long newpos);

Confusion Confusion_condense (Confusion me, const wchar_t *search, const wchar_t *replace,
	long maximumNumberOfReplaces, int use_regexp);
/*
	Group row and column labels according to search and replace.
*/

Confusion TableOfReal_to_Confusion (I);

TableOfReal Confusion_to_TableOfReal_marginals (I);
/*
	Create a table with one extra row and one extra column with marginals,
	i.e., column and row sums.
*/

void Confusion_drawAsNumbers (I, Graphics g, int marginals, int iformat, int precision);
// option marginals draw one extra row and column with sums.

#endif /* _Confusion_h_ */
