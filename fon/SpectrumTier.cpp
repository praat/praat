/* SpectrumTier.cpp
 *
 * Copyright (C) 2007-2012,2015 Paul Boersma
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

#include "Ltas_to_SpectrumTier.h"

Thing_implement (SpectrumTier, RealTier, 0);

void structSpectrumTier :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Frequency domain:");
	MelderInfo_writeLine (U"   Lowest frequency: ", xmin, U" Hz");
	MelderInfo_writeLine (U"   Highest frequency: ", xmax, U" Hz");
	MelderInfo_writeLine (U"   Total bandwidth: ", xmax - xmin, U" Hz");
	MelderInfo_writeLine (U"Number of points: ", points.size());
	MelderInfo_writeLine (U"Minimum power value: ", RealTier_getMinimumValue (this), U" dB/Hz");
	MelderInfo_writeLine (U"Maximum power value: ", RealTier_getMaximumValue (this), U" dB/Hz");
}

autoSpectrumTier SpectrumTier_create (double fmin, double fmax) {
	try {
		autoSpectrumTier me = Thing_new (SpectrumTier);
		RealTier_init (me.peek(), fmin, fmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SpectrumTier not created.");
	}
}

void SpectrumTier_draw (SpectrumTier me, Graphics g, double fmin, double fmax,
	double pmin, double pmax, int garnish, const char32 *method)
{
	RealTier_draw (me, g, fmin, fmax, pmin, pmax, garnish, method, U"Power spectral density (dB)");
}

void SpectrumTier_list (SpectrumTier me, bool includeIndexes, bool includeFrequency, bool includePowerDensity) {
	try {
		autoTable table = SpectrumTier_downto_Table (me, includeIndexes, includeFrequency, includePowerDensity);
		Table_list (table.peek(), false);
	} catch (MelderError) {
		Melder_throw (me, U": not listed.");
	}
}

autoTable SpectrumTier_downto_Table (SpectrumTier me, bool includeIndexes, bool includeFrequency, bool includePowerDensity) {
	return RealTier_downto_Table (me,
		includeIndexes ? U"index" : nullptr,
		includeFrequency ? U"freq(Hz)" : nullptr,
		includePowerDensity ? U"pow(dB/Hz)" : nullptr);
}

autoSpectrumTier Spectrum_to_SpectrumTier_peaks (Spectrum me) {
	try {
		autoLtas ltas = Spectrum_to_Ltas_1to1 (me);
		autoSpectrumTier thee = Ltas_to_SpectrumTier_peaks (ltas.peek());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": peaks not converted to SpectrumTier.");
	}
}

/* End of file SpectrumTier.cpp */
