#ifndef _ContingencyTable_h_
#define _ContingencyTable_h_
/* ContingencyTable.h
 *
 * Copyright (C) 1993-2017 David Weenink
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
#include "Confusion.h"

Thing_define (ContingencyTable, TableOfReal) {
	void v_info ()
		override;
};

// entries must be nonnegative numbers

autoContingencyTable ContingencyTable_create (integer numberOfRows, integer numberOfColumns);

double ContingencyTable_chisqProbability (ContingencyTable me);

double ContingencyTable_cramersStatistic (ContingencyTable me);

double ContingencyTable_contingencyCoefficient (ContingencyTable me);

void ContingencyTable_chisq (ContingencyTable me, double *chisq, double *df);

void ContingencyTable_entropies (ContingencyTable me, double *h, double *hx, double *hy, double *hygx, double *hxgy, double *uygx, double *uxgy, double *uxy);

autoContingencyTable Confusion_to_ContingencyTable (Confusion me);

autoContingencyTable TableOfReal_to_ContingencyTable (TableOfReal me);

#endif // _ContingencyTable_h_
