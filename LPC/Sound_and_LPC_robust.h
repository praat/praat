#ifndef _Sound_and_LPC_robust_h_
#define _Sound_and_LPC_robust_h_
/* Sound_and_LPC_robust.h
 *
 * Copyright (C) 1993-2024 David Weenink
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
 djmw 20030815 GPL header
*/

#include "LPC.h"
#include "Formant.h"
#include "Sound_and_LPC.h"

autoFormant Sound_to_Formant_robust (Sound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double halfdt_window, double preemphasisFrequency, double safetyMargin, double k, integer itermax, double tol, double location, bool wantlocation);

#endif /* _Sound_and_LPC_robust_h_ */
