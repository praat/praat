#ifndef _Distributions_h_
#define _Distributions_h_
/* Distributions.h
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
 * pb 2003/07/28
 */

/* Distributions inherits from TableOfReal */
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif

#define Distributions_members TableOfReal_members
#define Distributions_methods TableOfReal_methods
class_create (Distributions, TableOfReal);

Distributions Distributions_create (long numberOfRows, long numberOfColumns);

int Distributions_peek (Distributions me, long column, char **string);

double Distributions_getProbability (Distributions me, const char *string, long column);
double Distributionses_getMeanAbsoluteDifference (Distributions me, Distributions thee, long column);

Distributions Distributions_addTwo (Distributions me, Distributions thee);
Distributions Distributions_addMany (Collection me);

/* End of file Distributions.h */
#endif
