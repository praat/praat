#ifndef _LPC_and_Tube_h_
#define _LPC_and_Tube_h_
/* LPC_and_Tube.h
 *
 * Copyright (C) 1994-2012 David Weenink
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
 djmw 20030612 GPL header
 djmw 20110307 Latest modification
*/

#include "LPC.h"
#include "Tube.h"
#include "VocalTract.h"

void LPC_Frame_into_Tube_Frame_rc (LPC_Frame me, Tube_Frame thee);
void LPC_Frame_into_Tube_Frame_area (LPC_Frame me, Tube_Frame thee);

VocalTract LPC_Frame_to_VocalTract (LPC_Frame me, double length);

double LPC_Frame_getVTL_wakita (LPC_Frame me, double samplingPeriod, double refLength);
double VocalTract_and_LPC_Frame_getMatchingLength (VocalTract me, LPC_Frame thee, double glottalDamping, bool radiationDamping, bool internalDamping);

int Tube_Frame_into_LPC_Frame_area (Tube_Frame me, LPC_Frame thee);

int Tube_Frame_into_LPC_Frame_rc (Tube_Frame me, LPC_Frame thee);

VocalTract LPC_to_VocalTract (LPC me, double time, double length);
void VocalTract_setLength (VocalTract me, double newLength);

VocalTract LPC_to_VocalTract (LPC me, double time, double glottalDamping, bool radiationDamping, bool internalDamping);

#endif /* _LPC_and_Tube_h_ */
