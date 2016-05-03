#ifndef _LPC_and_Cepstrumc_h_
#define _LPC_and_Cepstrumc_h_
/* LPC_and_Cepstrumc.h
 *
 * Copyright (C) 1994-2011, 2015 David Weenink
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
 djmw 19950410
 djmw 20020812 GPL header
 djmw 20110307 Latest modification
*/

#include "LPC.h"
#include "Cepstrumc.h"

autoCepstrumc LPC_to_Cepstrumc (LPC me);

autoLPC Cepstrumc_to_LPC (Cepstrumc me);

void LPC_Frame_into_Cepstrumc_Frame (LPC_Frame me, Cepstrumc_Frame thee);
void Cepstrumc_Frame_into_LPC_Frame (Cepstrumc_Frame me, LPC_Frame thee);

#endif /* _LPC_and_Cepstrumc_h_ */
