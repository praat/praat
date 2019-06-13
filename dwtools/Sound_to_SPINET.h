#ifndef _Sound_to_SPINET_h_
#define _Sound_to_SPINET_h_
/* Sound_to_SPINET.h
 *
 * Copyright (C) 1993-2019 David Weenink
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
 djmw 19970408
 djmw 20020813 GPL header
 djmw 20110307 Latest modification
*/

#include "SPINET.h"
#include "Sound_extensions.h"

autoSPINET Sound_to_SPINET (Sound me, double timeStep, double windowDuration,
	double minimumFrequencyHz, double maximumFrequencyHz, integer numberOfGammaFilters,
	double excitationErbProportion, double inhibitionErbProportion);

#endif /* _Sound_to_SPINET_h_ */
