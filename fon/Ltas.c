/* Ltas.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2003/04/16 Ltases_merge
 * pb 2003/06/19 Ltas_computeTrendLine, Ltas_subtractTrendLine
 * pb 2004/05/05 more drawing options
 * pb 2004/05/13 pitch-corrected LTAS
 * pb 2004/10/24 Sampled statistics
 * pb 2004/10/31 info
 * pb 2004/11/22 simplified Sound_to_Spectrum
 * pb 2005/06/16 units
 * pb 2005/11/26 calibrated pitch-corrected Ltas
 * pb 2005/11/27 Sound_to_Ltas_pitchCorrected
 * pb 2005/12/10 Ltases_average
 * pb 2006/12/08 MelderInfo
 * pb 2007/03/17 domain quantity
 */

#include "Ltas.h"
#include "Sound_and_Spectrum.h"
#include "Sound_to_PointProcess.h"

static void info (I) {
	iam (Ltas);
	double meanPowerDensity;
	classData -> info (me);
	MelderInfo_writeLine1 (L"Frequency domain:");
	MelderInfo_writeLine3 (L"   Lowest frequency: ", Melder_double (my xmin), L" Hz");
	MelderInfo_writeLine3 (L"   Highest frequency: ", Melder_double (my xmax), L" Hz");
	MelderInfo_writeLine3 (L"   Total frequency domain: ", Melder_double (my xmax - my xmin), L" Hz");
	MelderInfo_writeLine1 (L"Frequency sampling:");
	MelderInfo_writeLine2 (L"   Number of frequency bands: ", Melder_integer (my nx));
	MelderInfo_writeLine3 (L"   Width of each band: ", Melder_double (my dx), L" Hz");
	MelderInfo_writeLine3 (L"   First band centred at: ", Melder_double (my x1), L" Hz");
	meanPowerDensity = Sampled_getMean (me, my xmin, my xmax, 0, 1, FALSE);
	MelderInfo_writeLine3 (L"Total SPL: ", Melder_single (10 * log10 (meanPowerDensity * (my xmax - my xmin))), L" dB");
}

static double convertStandardToSpecialUnit (I, double value, long ilevel, int unit) {
	(void) void_me;
	(void) ilevel;
	if (unit == 1) {
		return pow (10.0, 0.1 * value);   /* energy */
	} else if (unit == 2) {
		return pow (2.0, 0.1 * value);   /* sones */
	}
	return value;
}

static double convertSpecialToStandardUnit (I, double value, long ilevel, int unit) {
	(void) void_me;
	(void) ilevel;
	return
		unit == 1 ?
			10.0 * log10 (value) :   /* value = energy */
		unit == 2 ?
			10.0 * NUMlog2 (value) :   /* value = sones */
		value;   /* value = dB */
}

class_methods (Ltas, Vector)
	class_method (info)
	us -> domainQuantity = MelderQuantity_FREQUENCY_HERTZ;
	class_method (convertStandardToSpecialUnit)
	class_method (convertSpecialToStandardUnit)
class_methods_end

Ltas Ltas_create (long nx, double dx) {
	Ltas me = new (Ltas);
	if (! me || ! Matrix_init (me, 0, nx * dx, nx, dx, 0.5 * dx, 1, 1, 1, 1, 1)) return NULL;
	return me;
}

void Ltas_draw (Ltas me, Graphics g, double fmin, double fmax, double minimum, double maximum, int garnish, const wchar_t *method) {
	Vector_draw (me, g, & fmin, & fmax, & minimum, & maximum, 1.0, method);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Frequency (Hz)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, L"Sound pressure level (dB/Hz)");
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

double Ltas_getSlope (Ltas me, double f1min, double f1max, double f2min, double f2max, int averagingUnits) {
	double low = Sampled_getMean (me, f1min, f1max, 0, averagingUnits, FALSE);
	double high = Sampled_getMean (me, f2min, f2max, 0, averagingUnits, FALSE);
	if (low == NUMundefined || high == NUMundefined) return NUMundefined;
	return averagingUnits == 3 ? high - low : ClassFunction_convertSpecialToStandardUnit (classLtas, high / low, 0, averagingUnits);
}

double Ltas_getLocalPeakHeight (Ltas me, double environmentMin, double environmentMax, double peakMin, double peakMax, int averagingUnits) {
	double environmentLow = Sampled_getMean (me, environmentMin, peakMin, 0, averagingUnits, FALSE);
	double environmentHigh = Sampled_getMean (me, peakMax, environmentMax, 0, averagingUnits, FALSE);
	double peak = Sampled_getMean (me, peakMin, peakMax, 0, averagingUnits, FALSE);
	if (environmentLow == NUMundefined || environmentHigh == NUMundefined || peak == NUMundefined) return NUMundefined;
	return averagingUnits == 3 ? peak - 0.5 * (environmentLow + environmentHigh) :
		ClassFunction_convertSpecialToStandardUnit (classLtas, peak / (0.5 * (environmentLow + environmentHigh)), 0, averagingUnits);
}

Matrix Ltas_to_Matrix (Ltas me) {
	Matrix thee = Data_copy (me);
	if (! thee) return NULL;
	Thing_overrideClass (thee, classMatrix);
	return thee;
}

Ltas Matrix_to_Ltas (Matrix me) {
	Ltas thee = Data_copy (me);
	if (! thee) return NULL;
	Melder_assert (sizeof (struct structLtas) == sizeof (struct structMatrix));
	Thing_overrideClass (thee, classLtas);
	return thee;
}

Ltas Ltases_merge (Collection ltases) {
	Ltas me, thee = NULL;
	long iband, ispec;
	if (ltases -> size < 1) error1 (L"Cannot merge zero Ltas objects.")
	me = ltases -> item [1];
	thee = Data_copy (me); cherror
	/*
	 * Convert to energy.
	 */
	for (iband = 1; iband <= thy nx; iband ++) {
		thy z [1] [iband] = pow (10.0, thy z [1] [iband] / 10.0);
	}
	for (ispec = 2; ispec <= ltases -> size; ispec ++) {
		Ltas him = ltases -> item [ispec];
		if (his xmin != thy xmin || his xmax != thy xmax) error1 (L"Frequency domains do not match.")
		if (his dx != thy dx) error1 (L"Bandwidths do not match.")
		if (his nx != thy nx || his x1 != thy x1) error1 (L"Frequency bands do not match.")
		/*
		 * Add band energies.
		 */
		for (iband = 1; iband <= thy nx; iband ++) {
			thy z [1] [iband] += pow (10.0, his z [1] [iband] / 10.0);
		}
	}
	/*
	 * Convert back to dB.
	 */
	for (iband = 1; iband <= thy nx; iband ++) {
		thy z [1] [iband] = 10.0 * log10 (thy z [1] [iband]);
	}
end:
	iferror forget (thee);
	return thee;
}

Ltas Ltases_average (Collection ltases) {
	Ltas thee = NULL;
	long iband;
	double factor = -10.0 * log10 (ltases -> size);
	thee = Ltases_merge (ltases); cherror
	for (iband = 1; iband <= thy nx; iband ++) {
		thy z [1] [iband] += factor;
	}	
end:
	iferror forget (thee);
	return thee;
}

Ltas Ltas_computeTrendLine (Ltas me, double fmin, double fmax) {
	Ltas thee = NULL;
	long imin, imax, n, i;
	double sum = 0.0, amean, fmean, numerator = 0.0, denominator = 0.0, slope;
	/*
	 * Find the first and last bin.
	 */
	if ((n = Sampled_getWindowSamples (me, fmin, fmax, & imin, & imax)) < 2) {
		return Melder_errorp ("(Ltas_computeTrendLine:) Number of bins too low (%ld). Should be at least 2.", n);
	}
	thee = Data_copy (me);
	/*
	 * Compute average amplitude and frequency.
	 */
	for (i = imin; i <= imax; i ++) {
		sum += thy z [1] [i];
	}
	amean = sum / n;
	fmean = thy x1 + (0.5 * (imin + imax) - 1) * thy dx;
	/*
	 * Compute slope.
	 */
	for (i = imin; i <= imax; i ++) {
		double da = thy z [1] [i] - amean, df = thy x1 + (i - 1) * thy dx - fmean;
		numerator += da * df;
		denominator += df * df;
	}
	slope = numerator / denominator;
	/*
	 * Modify bins.
	 */
	for (i = 1; i <= thy nx; i ++) {
		double df = thy x1 + (i - 1) * thy dx - fmean;
		thy z [1] [i] = amean + slope * df;
	}
	return thee;
}

Ltas Ltas_subtractTrendLine (Ltas me, double fmin, double fmax) {
	Ltas thee = NULL;
	long imin, imax, n, i;
	double sum = 0.0, amean, fmean, numerator = 0.0, denominator = 0.0, slope;
	/*
	 * Find the first and last bin.
	 */
	if ((n = Sampled_getWindowSamples (me, fmin, fmax, & imin, & imax)) < 2) {
		return Melder_errorp ("(Ltas_subtractTrendLine:) Number of bins too low (%ld). Should be at least 2.", n);
	}
	thee = Data_copy (me);
	/*
	 * Compute average amplitude and frequency.
	 */
	for (i = imin; i <= imax; i ++) {
		sum += thy z [1] [i];
	}
	amean = sum / n;
	fmean = thy x1 + (0.5 * (imin + imax) - 1) * thy dx;
	/*
	 * Compute slope.
	 */
	for (i = imin; i <= imax; i ++) {
		double da = thy z [1] [i] - amean, df = thy x1 + (i - 1) * thy dx - fmean;
		numerator += da * df;
		denominator += df * df;
	}
	slope = numerator / denominator;
	/*
	 * Modify bins.
	 */
	for (i = 1; i < imin; i ++) {
		thy z [1] [i] = 0.0;
	}
	for (i = imin; i <= imax; i ++) {
		double df = thy x1 + (i - 1) * thy dx - fmean;
		thy z [1] [i] -= amean + slope * df;
	}
	for (i = imax + 1; i <= thy nx; i ++) {
		thy z [1] [i] = 0.0;
	}
	return thee;
}

Ltas Spectrum_to_Ltas (Spectrum me, double bandWidth) {
	Ltas thee = NULL;
	long numberOfBands = ceil ((my xmax - my xmin) / bandWidth), iband;
	if (bandWidth <= my dx) error3 (L"Bandwidth must be greater than ", Melder_double (my dx), L".")
	thee = new (Ltas); cherror
	Matrix_init (thee, my xmin, my xmax, numberOfBands, bandWidth, my xmin + 0.5 * bandWidth, 1, 1, 1, 1, 1); cherror
	for (iband = 1; iband <= numberOfBands; iband ++) {
		double fmin = thy xmin + (iband - 1) * bandWidth;
		double meanEnergyDensity = Sampled_getMean (me, fmin, fmin + bandWidth, 0, 1, FALSE);
		double meanPowerDensity = meanEnergyDensity * my dx;   /* As an approximation for a division by the original duration. */
		thy z [1] [iband] = meanPowerDensity == 0.0 ? -300.0 : 10 * log10 (meanPowerDensity / 4.0e-10);
	}
end:
	iferror forget (thee);
	return thee;
}

Ltas Spectrum_to_Ltas_1to1 (Spectrum me) {
	long iband;
	Ltas thee = new (Ltas); cherror
	Matrix_init (thee, my xmin, my xmax, my nx, my dx, my x1, 1, 1, 1, 1, 1); cherror
	for (iband = 1; iband <= my nx; iband ++) {
		thy z [1] [iband] = Sampled_getValueAtSample (me, iband, 0, 2);
	}
end:
	iferror forget (thee);
	return thee;
}

Ltas Sound_to_Ltas (Sound me, double bandwidth) {
	Spectrum thee = NULL;
	Ltas him = NULL;
	long iband;
	double correction;
	thee = Sound_to_Spectrum (me, TRUE); cherror
	him = Spectrum_to_Ltas (thee, bandwidth); cherror
	correction = -10 * log10 (thy dx * my nx * my dx);
	for (iband = 1; iband <= his nx; iband ++) {
		his z [1] [iband] += correction;
	}
end:
	iferror forget (him);
	forget (thee);
	return him;
}

Ltas PointProcess_Sound_to_Ltas (PointProcess pulses, Sound sound,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor)
{
	Ltas ltas = NULL, numbers = NULL;
	Sound period = NULL;
	Spectrum spectrum = NULL;
	long numberOfPeriods = pulses -> nt - 2, ipulse, ifreq, iband, totalNumberOfEnergies = 0;
	ltas = Ltas_create (maximumFrequency / bandWidth, bandWidth); cherror
	ltas -> xmax = maximumFrequency;
	numbers = Data_copy (ltas);
	if (numberOfPeriods < 1) error1 (L"Cannot compute an Ltas if there are no periods in the point process.")
	for (ipulse = 2; ipulse < pulses -> nt; ipulse ++) {
		double leftInterval = pulses -> t [ipulse] - pulses -> t [ipulse - 1];
		double rightInterval = pulses -> t [ipulse + 1] - pulses -> t [ipulse];
		double intervalFactor = leftInterval > rightInterval ? leftInterval / rightInterval : rightInterval / leftInterval;
		Melder_progress4 ((double) ipulse / pulses -> nt, L"Sound & PointProcess: To Ltas: pulse ", Melder_integer (ipulse), L" out of ", Melder_integer (pulses -> nt));
		if (leftInterval >= shortestPeriod && leftInterval <= longestPeriod &&
		    rightInterval >= shortestPeriod && rightInterval <= longestPeriod &&
		    intervalFactor <= maximumPeriodFactor)
		{
			/*
			 * We have a period! Compute the spectrum.
			 */
			period = Sound_extractPart (sound,
				pulses -> t [ipulse] - 0.5 * leftInterval, pulses -> t [ipulse] + 0.5 * rightInterval,
				kSound_windowShape_RECTANGULAR, 1.0, FALSE); cherror
			spectrum = Sound_to_Spectrum (period, FALSE); cherror
			for (ifreq = 1; ifreq <= spectrum -> nx; ifreq ++) {
				double frequency = spectrum -> xmin + (ifreq - 1) * spectrum -> dx;
				double realPart = spectrum -> z [1] [ifreq];
				double imaginaryPart = spectrum -> z [2] [ifreq];
				double energy = (realPart * realPart + imaginaryPart * imaginaryPart) * 2.0 * spectrum -> dx /* OLD: * sound -> nx */;
				iband = ceil (frequency / bandWidth);
				if (iband >= 1 && iband <= ltas -> nx) {
					ltas -> z [1] [iband] += energy;
					numbers -> z [1] [iband] += 1;
					totalNumberOfEnergies += 1;
				}
			}
			forget (spectrum);
			forget (period);
		} else {
			numberOfPeriods -= 1;
		}
	}
	if (numberOfPeriods < 1) error1 (L"Cannot compute an Ltas if there are no periods in the point process.")
	for (iband = 1; iband <= ltas -> nx; iband ++) {
		if (numbers -> z [1] [iband] == 0) {
			ltas -> z [1] [iband] = NUMundefined;
		} else {
			/*
			 * Each bin now contains a total energy in Pa2 sec.
			 * To convert this to power density, we
			 */
			double totalEnergyInThisBand = ltas -> z [1] [iband];
			if (0 /* i.e. if you just want to have a spectrum of the voiced parts... */) {
				double energyDensityInThisBand = totalEnergyInThisBand / ltas -> dx;
				double powerDensityInThisBand = energyDensityInThisBand / (sound -> xmax - sound -> xmin);
				ltas -> z [1] [iband] = 10.0 * log10 (powerDensityInThisBand / 4.0e-10);
			} else {
				/*
				 * And this is what we really want. The total energy has to be redistributed.
				 */
				double meanEnergyInThisBand = totalEnergyInThisBand / numbers -> z [1] [iband];
				double meanNumberOfEnergiesPerBand = (double) totalNumberOfEnergies / ltas -> nx;
				double redistributedEnergyInThisBand = meanEnergyInThisBand * meanNumberOfEnergiesPerBand;
				double redistributedEnergyDensityInThisBand = redistributedEnergyInThisBand / ltas -> dx;
				double redistributedPowerDensityInThisBand = redistributedEnergyDensityInThisBand / (sound -> xmax - sound -> xmin);
				ltas -> z [1] [iband] = 10.0 * log10 (redistributedPowerDensityInThisBand / 4.0e-10);
				/* OLD: ltas -> z [1] [iband] = 10.0 * log10 (ltas -> z [1] [iband] / numbers -> z [1] [iband] * sound -> nx);*/
			}
		}
	}
	for (iband = 1; iband <= ltas -> nx; iband ++) {
		if (ltas -> z [1] [iband] == NUMundefined) {
			long ibandleft = iband - 1, ibandright = iband + 1;
			while (ibandleft >= 1 && ltas -> z [1] [ibandleft] == NUMundefined) ibandleft --;
			while (ibandright <= ltas -> nx && ltas -> z [1] [ibandright] == NUMundefined) ibandright ++;
			if (ibandleft < 1 && ibandright > ltas -> nx)
				error1 (L"Cannot create an Ltas without energy in any bins.")
			if (ibandleft < 1) {
				ltas -> z [1] [iband] = ltas -> z [1] [ibandright];
			} else if (ibandright > ltas -> nx) {
				ltas -> z [1] [iband] = ltas -> z [1] [ibandleft];
			} else {
				double frequency = ltas -> x1 + (iband - 1) * ltas -> dx;
				double fleft = ltas -> x1 + (ibandleft - 1) * ltas -> dx;
				double fright = ltas -> x1 + (ibandright - 1) * ltas -> dx;
				ltas -> z [1] [iband] = ((fright - frequency) * ltas -> z [1] [ibandleft]
					+ (frequency - fleft) * ltas -> z [1] [ibandright]) / (fright - fleft);
			}
		}
	}
end:
	Melder_progress1 (1.0, NULL);
	iferror forget (ltas);
	forget (numbers);
	forget (period);
	forget (spectrum);
	return ltas;
}

Ltas Sound_to_Ltas_pitchCorrected (Sound sound, double minimumPitch, double maximumPitch,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor)
{
	PointProcess pulses = NULL;
	Ltas ltas = NULL;
	pulses = Sound_to_PointProcess_periodic_cc (sound, minimumPitch, maximumPitch); cherror
	ltas = PointProcess_Sound_to_Ltas (pulses, sound, maximumFrequency, bandWidth,
		shortestPeriod, longestPeriod, maximumPeriodFactor); cherror
end:
	iferror forget (ltas);
	forget (pulses);
	return ltas;
}

Ltas PointProcess_Sound_to_Ltas_harmonics (PointProcess pulses, Sound sound,
	long maximumHarmonic,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor)
{
	Ltas ltas = NULL;
	Sound period = NULL;
	Spectrum spectrum = NULL;
	long numberOfPeriods = pulses -> nt - 2, ipulse, iharm;
	ltas = Ltas_create (maximumHarmonic, 1.0); cherror
	ltas -> xmax = maximumHarmonic;
	if (numberOfPeriods < 1)
		error1 (L"Cannot compute an Ltas if there are no periods in the point process.")
	for (ipulse = 2; ipulse < pulses -> nt; ipulse ++) {
		double leftInterval = pulses -> t [ipulse] - pulses -> t [ipulse - 1];
		double rightInterval = pulses -> t [ipulse + 1] - pulses -> t [ipulse];
		double intervalFactor = leftInterval > rightInterval ? leftInterval / rightInterval : rightInterval / leftInterval;
		Melder_progress4 ((double) ipulse / pulses -> nt, L"Sound & PointProcess: To Ltas: pulse ", Melder_integer (ipulse), L" out of ", Melder_integer (pulses -> nt));
		if (leftInterval >= shortestPeriod && leftInterval <= longestPeriod &&
		    rightInterval >= shortestPeriod && rightInterval <= longestPeriod &&
		    intervalFactor <= maximumPeriodFactor)
		{
			/*
			 * We have a period! Compute the spectrum.
			 */
			long localMaximumHarmonic;
			period = Sound_extractPart (sound,
				pulses -> t [ipulse] - 0.5 * leftInterval, pulses -> t [ipulse] + 0.5 * rightInterval,
				kSound_windowShape_RECTANGULAR, 1.0, FALSE); cherror
			spectrum = Sound_to_Spectrum (period, FALSE); cherror
			localMaximumHarmonic = maximumHarmonic < spectrum -> nx ? maximumHarmonic : spectrum -> nx;
			for (iharm = 1; iharm <= localMaximumHarmonic; iharm ++) {
				double realPart = spectrum -> z [1] [iharm];
				double imaginaryPart = spectrum -> z [2] [iharm];
				double energy = (realPart * realPart + imaginaryPart * imaginaryPart) * 2.0 * spectrum -> dx;
				ltas -> z [1] [iharm] += energy;
			}
			forget (spectrum);
			forget (period);
		} else {
			numberOfPeriods -= 1;
		}
	}
	if (numberOfPeriods < 1)
		error1 (L"Cannot compute an Ltas if there are no periods in the point process.")
	for (iharm = 1; iharm <= ltas -> nx; iharm ++) {
		if (ltas -> z [1] [iharm] == 0) {
			ltas -> z [1] [iharm] = -300;
		} else {
			double energyInThisBand = ltas -> z [1] [iharm];
			double powerInThisBand = energyInThisBand / (sound -> xmax - sound -> xmin);
			ltas -> z [1] [iharm] = 10.0 * log10 (powerInThisBand / 4.0e-10);
		}
	}
end:
	Melder_progress1 (1.0, NULL);
	iferror forget (ltas);
	forget (period);
	forget (spectrum);
	return ltas;
}

/* End of file Ltas.c */
