#ifndef _FormantTier_h_
#define _FormantTier_h_
/* FormantTier.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1999/11/19
 * pb 2002/07/16 GPL
 */

#ifndef _AnyTier_h_
	#include "AnyTier.h"
#endif
#ifndef _Formant_h_
	#include "Formant.h"
#endif
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

#include "FormantTier_def.h"
#define FormantPoint_methods Data_methods
oo_CLASS_CREATE (FormantPoint, Data)
#define FormantTier_methods Function_methods
oo_CLASS_CREATE (FormantTier, Function)

FormantPoint FormantPoint_create (double time);
FormantTier FormantTier_create (double tmin, double tmax);
double FormantTier_getValueAtTime (FormantTier me, int iformant, double t);
double FormantTier_getBandwidthAtTime (FormantTier me, int iformant, double t);
int FormantTier_getMinNumFormants (FormantTier me);
int FormantTier_getMaxNumFormants (FormantTier me);
void FormantTier_speckle (FormantTier me, Graphics g, double tmin, double tmax, double fmax, int garnish);
FormantTier Formant_downto_FormantTier (Formant me);
FormantTier Formant_PointProcess_to_FormantTier (Formant me, PointProcess pp);
TableOfReal FormantTier_downto_TableOfReal (FormantTier me, int includeFormants, int includeBandwidths);

void Sound_FormantTier_filter_inline (Sound me, FormantTier formantTier);
Sound Sound_FormantTier_filter (Sound me, FormantTier formantTier);
Sound Sound_FormantTier_filter_noscale (Sound me, FormantTier formantTier);
Sound Sound_Formant_filter (Sound me, Formant formant);
Sound Sound_Formant_filter_noscale (Sound me, Formant formant);

/* End of file FormantTier.h */
#endif
