#ifndef _Distributions_h_
#define _Distributions_h_
/* Distributions.h
 *
 * Copyright (C) 1997-2011 Paul Boersma
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

/* Distributions inherits from TableOfReal */
#include "TableOfReal.h"

Thing_define (Distributions, TableOfReal) {
	// overridden methods:
	protected:
		virtual void v_info ();
};

Distributions Distributions_create (long numberOfRows, long numberOfColumns);

int Distributions_peek (Distributions me, long column, wchar_t **string);
int Distributions_peek_opt (Distributions me, long column, long *row);

double Distributions_getProbability (Distributions me, const wchar_t *string, long column);
double Distributionses_getMeanAbsoluteDifference (Distributions me, Distributions thee, long column);

Distributions Distributions_addTwo (Distributions me, Distributions thee);
Distributions Distributions_addMany (Collection me);

void Distributions_checkSpecifiedColumnNumberWithinRange (Distributions me, long columnNumber);

/* End of file Distributions.h */
#endif
