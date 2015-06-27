#ifndef _SpectrumTier_h_
#define _SpectrumTier_h_
/* SpectrumTier.h
 *
 * Copyright (C) 2007-2011,2014,2015 Paul Boersma
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

#include "RealTier.h"
#include "Graphics.h"
#include "Spectrum.h"

/********** class SpectrumTier **********/

Thing_define (SpectrumTier, RealTier) {
	void v_info ()
		override;
	int v_domainQuantity ()
		override { return MelderQuantity_FREQUENCY_HERTZ; }
	const char32 * v_getUnitText (long /* ilevel */, int /* unit */, unsigned long /* flags */)
		override { return U"Frequency (Hz)"; }
};

SpectrumTier SpectrumTier_create (double fmin, double fmax);
/*
	Postconditions:
		result -> xmin == fmin;
		result -> xmax == fmax;
		result -> points -> size == 0;
*/

void SpectrumTier_draw (SpectrumTier me, Graphics g, double fmin, double fmax,
	double pmin, double pmax, int garnish, const char32 *method);

void SpectrumTier_list (SpectrumTier me, bool includeIndexes, bool includeFrequency, bool includePowerDensity);

Table SpectrumTier_downto_Table (SpectrumTier me, bool includeIndexes, bool includeFrequency, bool includePowerDensity);

SpectrumTier Spectrum_to_SpectrumTier_peaks (Spectrum me);

/* End of file SpectrumTier.h */
#endif
