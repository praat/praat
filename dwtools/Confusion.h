#ifndef _Confusion_h_
#define _Confusion_h_
/* Confusion.h
 *
 * Copyright (C) 1993-2019 David Weenink
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

#include "TableOfReal.h"
#include "Categories.h"
#include "Graphics.h"
#include "Matrix.h"

Thing_define (Confusion, TableOfReal) {
	void v_info ()
		override;
};

/*
	A Confusion matrix has both row and column labels. Row labels represent the stimulus names, 
	column labels represent the responses.
*/

autoConfusion Confusion_create (integer numberOfStimuli, integer numberOfResponses);

autoConfusion Confusion_createSimple (conststring32 labels);

autoConfusion Confusion_createFromStringses (Strings stimulusLabels, Strings responseLabels);

autoConfusion Categories_to_Confusion (Categories me, Categories thee);

void Confusion_increase (Confusion me, conststring32 stimulus, conststring32 response);
/* Increase the confusion count by one: data['stim']['resp'] += 1; */

double Confusion_getValue (Confusion me, conststring32 stimulus, conststring32 response);
/* data['stim']['resp'] ; */

void Confusion_getEntropies (Confusion me, double *out_h, double *out_hx, double *out_hy,
    double *out_hygx, double *out_hxgy, double *out_uygx, double *out_uxgy, double *out_uxy);
/*  x is column variable, y is row variable
	*out_h	    entropy of whole table;
	*out_hx	    entropy of x variable
	*out_hy	    entropy of y variable
	*out_hygx   entropy of y given x
	*out_hxgy   entropy of x given y
	*out_uygx   dependency of y on x
	*out_uxgy   dependency of x on y
	*out_uxy    symmetrical dependency
 */

void Confusion_getFractionCorrect (Confusion me, double *out_fraction, integer *out_numberOfCorrect);

void Confusion_Matrix_draw (Confusion me, Matrix thee, Graphics g, integer index, double lowerPercentage, double xmin, double xmax, double ymin, double ymax, bool garnish);
/* 1. Draw my rowLabels centered at ( matrix->z[i][1], matrix->z[i][2]).
	2. Draw arrows and circles according to:
	for (i=1; i <= my numberOfRows; i++)
	{
		if (index != 0 && index != i) continue;
		draw circle at i of width: my z[i][i]/rowSum;
		for (j=1; j <= my numberOfColumns; j++)
		{
			if (i != j && 100*my data[i][j]/rowSum > lowerPercentage)
				draw arrow from i to j of width: my data[i][j]/rowSum;
		}
	}
 */

autoMatrix Confusion_difference (Confusion me, Confusion thee);
/* return matrix with the difference between the two confusion matrices */

integer Confusion_getNumberOfEntries (Confusion me);

autoConfusion Confusion_groupStimuli (Confusion me, conststring32 labels, conststring32 newLabel, integer newpos);

autoConfusion Confusion_groupResponses (Confusion me, conststring32 labels, conststring32 newLabel, integer newpos);

autoConfusion Confusion_group (Confusion me, conststring32 labels, conststring32 newLabel, integer newpos);

autoConfusion Confusion_condense (Confusion me, conststring32 search, conststring32 replace, integer maximumNumberOfReplaces, bool use_regexp);
/* 
	Group row and column labels according to search and replace.
*/

autoConfusion TableOfReal_to_Confusion (TableOfReal me);

autoTableOfReal Confusion_to_TableOfReal_marginals (Confusion me);
/*
	Create a table with one extra row and one extra column with marginals,
	i.e., column and row sums.
*/

void Confusion_drawAsNumbers (Confusion me, Graphics g, bool marginals, int iformat, int precision);
/* option marginals draw one extra row and column with marginal sums. */

#endif /* _Confusion_h_ */
