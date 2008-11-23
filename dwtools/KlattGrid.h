#ifndef _KlattGrid_h_
#define _KlattGrid_h_
/* KlattGrid.h
 *
 * Copyright (C) 2008 David Weenink
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
 * djmw 20080917 Initial version
 */

#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _IntensityTier_h_
	#include "IntensityTier.h"
#endif
#ifndef _PitchTier_h_
	#include "PitchTier.h"
#endif
#ifndef _FormantGrid_h_
	#include "FormantGrid.h"
#endif

#include "KlattGrid_def.h"

#define FormantGridP_members FormantGrid_members \
	Ordered amplitudes;

#define FormantGridP_methods FormantGrid_methods
oo_CLASS_CREATE (FormantGridP, FormantGrid);

#define PhonationGrid_methods Function_methods
oo_CLASS_CREATE (PhonationGrid, Function);

#define KlattGrid_methods Function_methods
oo_CLASS_CREATE (KlattGrid, Function);

FormantGridP FormantGridP_create (double tmin, double tmax, long numberOfFormants);

PhonationGrid PhonationGrid_create (double tmin, double tmax);

KlattGrid KlattGrid_create (double tmin, double tmax);

Sound KlattGrid_to_Sound (KlattGrid me, double samplingFrequency, double maximumPeriod, int parallel);

int KlattGrid_synthesize (KlattGrid me, double t1, double t2, double samplingFrequency, double maximumPeriod);

KlattGrid KlattGrid_createExample (void);

#endif /* _KlattGrid_h_ */
