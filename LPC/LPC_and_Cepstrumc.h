#ifndef _LPC_and_Cepstrumc_h_
#define _LPC_and_Cepstrumc_h_
/* LPC_and_Cepstrumc.h
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
 djmw 19950410
 djmw 20020812 GPL header
 djmw 20110307 Latest modification
*/

#include "LPC.h"
#include "Cepstrumc.h"

#ifdef __cplusplus
	extern "C" {
#endif

Cepstrumc LPC_to_Cepstrumc (LPC me);

LPC Cepstrumc_to_LPC (Cepstrumc me);

void LPC_Frame_into_Cepstrumc_Frame (LPC_Frame me, Cepstrumc_Frame thee);
void Cepstrumc_Frame_into_LPC_Frame (Cepstrumc_Frame me, LPC_Frame thee);

#ifdef __cplusplus
	}
#endif

#endif /* _LPC_and_Cepstrumc_h_ */
