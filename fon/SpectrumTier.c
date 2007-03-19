/* SpectrumTier.c
 *
 * Copyright (C) 2007 Paul Boersma
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
 * pb 2007/03/19
 */

#include "Ltas_to_SpectrumTier.h"

static void info (I) {
	iam (SpectrumTier);
	classData -> info (me);
	MelderInfo_writeLine1 ("Frequency domain:");
	MelderInfo_writeLine3 ("   Lowest frequency: ", Melder_double (my xmin), " Hz");
	MelderInfo_writeLine3 ("   Highest frequency: ", Melder_double (my xmax), " Hz");
	MelderInfo_writeLine3 ("   Total bandwidth: ", Melder_double (my xmax - my xmin), " Hz");
	MelderInfo_writeLine2 ("Number of points: ", Melder_integer (my points -> size));
	MelderInfo_writeLine3 ("Minimum power value: ", Melder_double (RealTier_getMinimumValue (me)), " Hertz");
	MelderInfo_writeLine3 ("Maximum power value: ", Melder_double (RealTier_getMaximumValue (me)), " Hertz");
}

class_methods (SpectrumTier, RealTier)
	class_method (info)
	us -> domainQuantity = MelderQuantity_FREQUENCY_HERTZ;
class_methods_end

SpectrumTier SpectrumTier_create (double fmin, double fmax) {
	SpectrumTier me = new (SpectrumTier);
	if (! me || ! RealTier_init (me, fmin, fmax)) { forget (me); return NULL; }
	return me;
}

void SpectrumTier_draw (SpectrumTier me, Graphics g, double fmin, double fmax,
	double pmin, double pmax, int garnish)
{
	RealTier_draw (me, g, fmin, fmax, pmin, pmax, garnish, "Power spectral density (dB)");
}

void SpectrumTier_list (SpectrumTier me, bool includeIndexes, bool includeFrequency, bool includePowerDensity) {
	MelderInfo_open ();
	bool on = false;
	if (includeIndexes) { MelderInfo_write1 ("point"); on = true; }
	if (includeFrequency) { if (on) MelderInfo_write1 ("\t"); MelderInfo_write1 ("freq(Hz)"); on = true; }
	if (includePowerDensity) { if (on) MelderInfo_write1 ("\t"); MelderInfo_write1 ("pow(dB/Hz)"); on = true; }
	MelderInfo_write1 ("\n");
	for (long ipoint = 1; ipoint <= my points -> size; ipoint ++) {
		bool on = false;
		if (includeIndexes) { MelderInfo_write1 (Melder_integer (ipoint)); on = true; }
		RealPoint point = my points -> item [ipoint];
		if (includeFrequency) { 
			if (on) MelderInfo_write1 ("\t");
			MelderInfo_write1 (Melder_double (point -> time));
			on = true;
		}
		if (includePowerDensity) { 
			if (on) MelderInfo_write1 ("\t");
			MelderInfo_write1 (Melder_double (point -> value));
			on = true;
		}
		MelderInfo_write1 ("\n");
	}
	MelderInfo_close ();
}

Table SpectrumTier_downto_Table (SpectrumTier me) {
	return RealTier_downto_Table (me, "freq(Hz)", "power(dB)");
}

SpectrumTier Spectrum_to_SpectrumTier_peaks (Spectrum me) {
	Ltas ltas = NULL;
	SpectrumTier thee = NULL;
	ltas = Spectrum_to_Ltas_1to1 (me); cherror
	thee = Ltas_to_SpectrumTier_peaks (ltas); cherror
end:
	iferror forget (thee);
	forget (ltas);
	return thee;
}

/* End of file PitchTier.c */
