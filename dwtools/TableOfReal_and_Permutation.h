#ifndef _TableOfReal_and_Permutation_h_
#define _TableOfReal_and_Permutation_h_
/* TableOfReal_and_Permutation.h
 *
 * Copyright (C) 2005-2017 David Weenink
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

/*
 djmw 20050708
 djmw 20110307 Latest modification
*/

#include "Permutation.h"
#include "TableOfReal.h"

autoTableOfReal TableOfReal_Permutation_permuteRows (TableOfReal me, Permutation thee);
/*
	Permutation (n1,n2,..nn) new his z[1] = my z[n1], his z[2] = my z[n2], ..*/

autoPermutation TableOfReal_to_Permutation_sortRowLabels (TableOfReal me);

#endif /* _TableOfReal_and_Permutation_h_ */
