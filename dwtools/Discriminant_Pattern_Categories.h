#ifndef _Discriminant_Pattern_Categories_h_
#define _Discriminant_Pattern_Categories_h_
/* Discriminant_Pattern_Categories.h
 *
 * Copyright (C) 2004-2011 David Weenink
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
 djmw 20040422 Initial version
 djmw 20110307 Latest modification
*/

#ifndef _Discriminant_h_
	#include "Discriminant.h"
#endif
#ifndef _Pattern_h_
	#include "Pattern.h"
#endif
#ifndef _Categories_h_	
	#include "Categories.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

Discriminant Pattern_and_Categories_to_Discriminant (Pattern me, Categories thee);

Categories Discriminant_and_Pattern_to_Categories (Discriminant me, Pattern thee, 
	int poolCovarianceMatrices,int useAprioriProbabilities);

#ifdef __cplusplus
	}
#endif

#endif /* _Discriminant_Pattern_Categories_h_ */
