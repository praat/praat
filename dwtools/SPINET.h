#ifndef _SPINET_h_
#define _SPINET_h_
/* SPINET.h
 *
 * Copyright (C) 1993-2019 David Weenink, 2015 Paul Boersma
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
*/

#include "SampledXY.h"
#include "Graphics.h"

#include "SPINET_def.h"

autoSPINET SPINET_create (double tmin, double tmax, integer nt, double dt, double t1,
	 double minimumFrequency, double maximumFrequency, integer nFilters,
	 double excitationErbProportion, double inhibitionErbProportion);

void SPINET_spectralRepresentation (SPINET me, Graphics g, double fromTime, double toTime,
	double fromErb, double toErb, double minimum, double maximum, bool enhanced,
	bool garnish);

void SPINET_drawSpectrum (SPINET me, Graphics g, double time, double fromErb, double toErb,
	double minimum, double maximum, bool enhanced, bool garnish);

#endif /* _SPINET_h_ */
