#ifndef _FormantTier_h_
#define _FormantTier_h_
/* FormantTier.h
 *
 * Copyright (C) 1992-2011,2015,2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PointProcess.h"
#include "Formant.h"
#include "TableOfReal.h"
#include "Sound.h"
#include "AnyTier.h"

#include "FormantTier_def.h"

autoFormantPoint FormantPoint_create (double time, integer numberOfFormants);
autoFormantTier FormantTier_create (double tmin, double tmax);
double FormantTier_getValueAtTime (FormantTier me, integer iformant, double t);
double FormantTier_getBandwidthAtTime (FormantTier me, integer iformant, double t);
integer FormantTier_getMinNumFormants (FormantTier me);
integer FormantTier_getMaxNumFormants (FormantTier me);
void FormantTier_speckle (FormantTier me, Graphics g, double tmin, double tmax, double fmax, bool garnish);
autoFormantTier Formant_downto_FormantTier (Formant me);
autoFormantTier Formant_PointProcess_to_FormantTier (Formant me, PointProcess pp);
autoTableOfReal FormantTier_downto_TableOfReal (FormantTier me, bool includeFormants, bool includeBandwidths);

void Sound_FormantTier_filter_inplace (Sound me, FormantTier formantTier);
autoSound Sound_FormantTier_filter (Sound me, FormantTier formantTier);
autoSound Sound_FormantTier_filter_noscale (Sound me, FormantTier formantTier);

/* End of file FormantTier.h */
#endif
