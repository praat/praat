#ifndef _ContingencyTable_h_
#define _ContingencyTable_h_
/* ContingencyTable.h
 *
 * Copyright (C) 1993-2011, 2015 David Weenink
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
#include "Confusion.h"

Thing_define (ContingencyTable, TableOfReal) {
	void v_info ()
		override;
};

// entries must be nonnegative numbers

autoContingencyTable ContingencyTable_create (long numberOfRows, long numberOfColumns);

double ContingencyTable_chisqProbability (ContingencyTable me);

double ContingencyTable_cramersStatistic (ContingencyTable me);

double ContingencyTable_contingencyCoefficient (ContingencyTable me);

void ContingencyTable_chisq (ContingencyTable me, double *chisq, double *df);

void ContingencyTable_entropies (ContingencyTable me, double *h, double *hx, double *hy, double *hygx, double *hxgy, double *uygx, double *uxgy, double *uxy);

autoContingencyTable Confusion_to_ContingencyTable (Confusion me);

autoContingencyTable TableOfReal_to_ContingencyTable (TableOfReal me);

#endif // _ContingencyTable_h_
