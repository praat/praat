#ifndef _LPC_and_Polynomial_h_
#define _LPC_and_Polynomial_h_
/* LPC_and_Polynomial.h
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 19990607
 djmw 20020812 GPL header
 djmw 20110307 Latest modification
*/


#include "LPC.h"
#include "Polynomial.h"

#ifdef __cplusplus
	extern "C" {
#endif

Polynomial LPC_to_Polynomial (LPC me, double t);

Polynomial LPC_Frame_to_Polynomial (LPC_Frame me);

#ifdef __cplusplus
	}
#endif

#endif /* _LPC_and_Polynomial_h_ */
