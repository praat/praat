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
FormantGrid FormantGridP_downto_FormantGrid (FormantGridP me);

PhonationGrid PhonationGrid_create (double tmin, double tmax);

KlattGrid KlattGrid_create (double tmin, double tmax);

Sound KlattGrid_to_Sound_simple (KlattGrid me, double samplingFrequency, double maximumPeriod, int parallel);

Sound KlattGrid_to_Sound (KlattGrid me, double samplingFrequency, double maximumPeriod, int parallel,
	long startFormant, long endFormant, long startNasalFormant, long endNasalFormant, long startTrachealFormant, long endTrachealFormant,
	long startNasalAntiFormant, long endNasalAntiFormant, long startTrachealAntiFormant, long endTrachealAntiFormant,
	int noVoicing, int noAspiration, int noFrication);

int KlattGrid_synthesize (KlattGrid me, double t1, double t2, double samplingFrequency, double maximumPeriod);

KlattGrid KlattGrid_createExample (void);

/*** temporarily to test source */
KlattGrid KlattGrid_test (double tmin, double tmax, double f0b, double f0e, double flutterb, double fluttere, double openphb, double openphe, double p1b, double p1e, double p2b, double p2e, double colphb, double colphe, double dpulsb, double dpulse, double avdbb, double avdbe, double aspNoisedBb, double aspNoisedBe, double breathyb, double breathye, double tiltb, double tilte,
double f1b, double f1e, double b1b, double b1e, double df1b, double df1e, double f2b, double f2e, double b2b, double b2e);

#endif /* _KlattGrid_h_ */