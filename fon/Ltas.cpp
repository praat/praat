/* Ltas.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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
 * a selection of changes:
 * pb 2005/11/26 pitch-corrected Ltas
 */

#include "Ltas.h"
#include "Sound_and_Spectrum.h"
#include "Sound_to_PointProcess.h"

Thing_implement (Ltas, Vector, 2);

void structLtas :: v_info () {
	double meanPowerDensity;
	structData :: v_info ();
	MelderInfo_writeLine (L"Frequency domain:");
	MelderInfo_writeLine (L"   Lowest frequency: ", Melder_double (xmin), L" Hz");
	MelderInfo_writeLine (L"   Highest frequency: ", Melder_double (xmax), L" Hz");
	MelderInfo_writeLine (L"   Total frequency domain: ", Melder_double (xmax - xmin), L" Hz");
	MelderInfo_writeLine (L"Frequency sampling:");
	MelderInfo_writeLine (L"   Number of frequency bands: ", Melder_integer (nx));
	MelderInfo_writeLine (L"   Width of each band: ", Melder_double (dx), L" Hz");
	MelderInfo_writeLine (L"   First band centred at: ", Melder_double (x1), L" Hz");
	meanPowerDensity = Sampled_getMean (this, xmin, xmax, 0, 1, FALSE);
	MelderInfo_writeLine (L"Total SPL: ", Melder_single (10.0 * log10 (meanPowerDensity * (xmax - xmin))), L" dB");
}

double structLtas :: v_convertStandardToSpecialUnit (double value, long ilevel, int unit) {
	(void) ilevel;
	if (unit == 1) {
		return pow (10.0, 0.1 * value);   // energy
	} else if (unit == 2) {
		return pow (2.0, 0.1 * value);   // sones
	}
	return value;
}

double structLtas :: v_convertSpecialToStandardUnit (double value, long ilevel, int unit) {
	(void) ilevel;
	return
		unit == 1 ?
			10.0 * log10 (value) :   // value = energy
		unit == 2 ?
			10.0 * NUMlog2 (value) :   // value = sones
		value;   // value = dB
}

Ltas Ltas_create (long nx, double dx) {
	try {
		autoLtas me = Thing_new (Ltas);
		Matrix_init (me.peek(), 0.0, nx * dx, nx, dx, 0.5 * dx, 1.0, 1.0, 1, 1.0, 1.0);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Ltas not created.");
	}
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
	return averagingUnits == 3 ? high - low : Function_convertSpecialToStandardUnit (me, high / low, 0, averagingUnits);
}

double Ltas_getLocalPeakHeight (Ltas me, double environmentMin, double environmentMax, double peakMin, double peakMax, int averagingUnits) {
	double environmentLow = Sampled_getMean (me, environmentMin, peakMin, 0, averagingUnits, FALSE);
	double environmentHigh = Sampled_getMean (me, peakMax, environmentMax, 0, averagingUnits, FALSE);
	double peak = Sampled_getMean (me, peakMin, peakMax, 0, averagingUnits, FALSE);
	if (environmentLow == NUMundefined || environmentHigh == NUMundefined || peak == NUMundefined) return NUMundefined;
	return averagingUnits == 3 ? peak - 0.5 * (environmentLow + environmentHigh) :
		Function_convertSpecialToStandardUnit (me, peak / (0.5 * (environmentLow + environmentHigh)), 0, averagingUnits);
}

Matrix Ltas_to_Matrix (Ltas me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

Ltas Matrix_to_Ltas (Matrix me) {
	try {
		autoLtas thee = Thing_new (Ltas);
		my structMatrix :: v_copy (thee.peek());   // because copying a descendant of Matrix with additional members should not crash
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Ltas.");
	}
}

Ltas Ltases_merge (Collection ltases) {
	try {
		if (ltases -> size < 1)
			Melder_throw ("Cannot merge zero Ltas objects.");
		Ltas me = (Ltas) ltases -> item [1];
		autoLtas thee = Data_copy (me);
		/*
		 * Convert to energy.
		 */
		for (long iband = 1; iband <= thy nx; iband ++) {
			thy z [1] [iband] = pow (10.0, thy z [1] [iband] / 10.0);
		}
		for (long ispec = 2; ispec <= ltases -> size; ispec ++) {
			Ltas him = (Ltas) ltases -> item [ispec];
			if (his xmin != thy xmin || his xmax != thy xmax)
				Melder_throw ("Frequency domains do not match.");
			if (his dx != thy dx)
				Melder_throw ("Bandwidths do not match.");
			if (his nx != thy nx || his x1 != thy x1)
				Melder_throw ("Frequency bands do not match.");
			/*
			 * Add band energies.
			 */
			for (long iband = 1; iband <= thy nx; iband ++) {
				thy z [1] [iband] += pow (10.0, his z [1] [iband] / 10.0);
			}
		}
		/*
		 * Convert back to dB.
		 */
		for (long iband = 1; iband <= thy nx; iband ++) {
			thy z [1] [iband] = 10.0 * log10 (thy z [1] [iband]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Ltas objects not merged.");
	}
}

Ltas Ltases_average (Collection ltases) {
	try {
		double factor = -10.0 * log10 (ltases -> size);
		autoLtas thee = Ltases_merge (ltases);
		for (long iband = 1; iband <= thy nx; iband ++) {
			thy z [1] [iband] += factor;
		}	
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Ltas objects not averaged.");
	}
}

Ltas Ltas_computeTrendLine (Ltas me, double fmin, double fmax) {
	try {
		/*
		 * Find the first and last bin.
		 */
		long imin, imax, n;
		if ((n = Sampled_getWindowSamples (me, fmin, fmax, & imin, & imax)) < 2)
			Melder_throw ("Number of bins too low (", n, "). Should be at least 2.");
		autoLtas thee = Data_copy (me);
		/*
		 * Compute average amplitude and frequency.
		 */
		double sum = 0.0, amean, fmean, numerator = 0.0, denominator = 0.0, slope;
		for (long i = imin; i <= imax; i ++) {
			sum += thy z [1] [i];
		}
		amean = sum / n;
		fmean = thy x1 + (0.5 * (imin + imax) - 1) * thy dx;
		/*
		 * Compute slope.
		 */
		for (long i = imin; i <= imax; i ++) {
			double da = thy z [1] [i] - amean, df = thy x1 + (i - 1) * thy dx - fmean;
			numerator += da * df;
			denominator += df * df;
		}
		slope = numerator / denominator;
		/*
		 * Modify bins.
		 */
		for (long i = 1; i <= thy nx; i ++) {
			double df = thy x1 + (i - 1) * thy dx - fmean;
			thy z [1] [i] = amean + slope * df;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": trend line not computed.");
	}
}

Ltas Ltas_subtractTrendLine (Ltas me, double fmin, double fmax) {
	try {
		/*
		 * Find the first and last bin.
		 */
		long imin, imax, n;
		if ((n = Sampled_getWindowSamples (me, fmin, fmax, & imin, & imax)) < 2)
			Melder_throw ("Number of bins too low (", n, "). Should be at least 2.");
		autoLtas thee = Data_copy (me);
		/*
		 * Compute average amplitude and frequency.
		 */
		double sum = 0.0, amean, fmean, numerator = 0.0, denominator = 0.0, slope;
		for (long i = imin; i <= imax; i ++) {
			sum += thy z [1] [i];
		}
		amean = sum / n;
		fmean = thy x1 + (0.5 * (imin + imax) - 1) * thy dx;
		/*
		 * Compute slope.
		 */
		for (long i = imin; i <= imax; i ++) {
			double da = thy z [1] [i] - amean, df = thy x1 + (i - 1) * thy dx - fmean;
			numerator += da * df;
			denominator += df * df;
		}
		slope = numerator / denominator;
		/*
		 * Modify bins.
		 */
		for (long i = 1; i < imin; i ++) {
			thy z [1] [i] = 0.0;
		}
		for (long i = imin; i <= imax; i ++) {
			double df = thy x1 + (i - 1) * thy dx - fmean;
			thy z [1] [i] -= amean + slope * df;
		}
		for (long i = imax + 1; i <= thy nx; i ++) {
			thy z [1] [i] = 0.0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": trend line not subtracted.");
	}
}

Ltas Spectrum_to_Ltas (Spectrum me, double bandWidth) {
	try {
		long numberOfBands = ceil ((my xmax - my xmin) / bandWidth);
		if (bandWidth <= my dx)
			Melder_throw ("Bandwidth must be greater than ", my dx, ".");
		autoLtas thee = Thing_new (Ltas);
		Matrix_init (thee.peek(), my xmin, my xmax, numberOfBands, bandWidth, my xmin + 0.5 * bandWidth, 1, 1, 1, 1, 1);
		for (long iband = 1; iband <= numberOfBands; iband ++) {
			double fmin = thy xmin + (iband - 1) * bandWidth;
			double meanEnergyDensity = Sampled_getMean (me, fmin, fmin + bandWidth, 0, 1, FALSE);
			double meanPowerDensity = meanEnergyDensity * my dx;   // as an approximation for a division by the original duration
			thy z [1] [iband] = meanPowerDensity == 0.0 ? -300.0 : 10.0 * log10 (meanPowerDensity / 4.0e-10);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Ltas.");
	}
}

Ltas Spectrum_to_Ltas_1to1 (Spectrum me) {
	try {
		autoLtas thee = Thing_new (Ltas);
		Matrix_init (thee.peek(), my xmin, my xmax, my nx, my dx, my x1, 1.0, 1.0, 1, 1.0, 1.0);
		for (long iband = 1; iband <= my nx; iband ++) {
			thy z [1] [iband] = Sampled_getValueAtSample (me, iband, 0, 2);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Ltas.");
	}
}

Ltas Sound_to_Ltas (Sound me, double bandwidth) {
	try {
		autoSpectrum thee = Sound_to_Spectrum (me, TRUE);
		autoLtas him = Spectrum_to_Ltas (thee.peek(), bandwidth);
		double correction = -10.0 * log10 (thy dx * my nx * my dx);
		for (long iband = 1; iband <= his nx; iband ++) {
			his z [1] [iband] += correction;
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": LTAS analysis not performed.");
	}
}

Ltas PointProcess_Sound_to_Ltas (PointProcess pulses, Sound sound,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor)
{
	try {
		long numberOfPeriods = pulses -> nt - 2, totalNumberOfEnergies = 0;
		autoLtas ltas = Ltas_create (maximumFrequency / bandWidth, bandWidth);
		ltas -> xmax = maximumFrequency;
		autoLtas numbers = Data_copy (ltas.peek());
		if (numberOfPeriods < 1)
			Melder_throw ("Cannot compute an Ltas if there are no periods in the point process.");
		autoMelderProgress progress (L"Ltas analysis...");
		for (long ipulse = 2; ipulse < pulses -> nt; ipulse ++) {
			double leftInterval = pulses -> t [ipulse] - pulses -> t [ipulse - 1];
			double rightInterval = pulses -> t [ipulse + 1] - pulses -> t [ipulse];
			double intervalFactor = leftInterval > rightInterval ? leftInterval / rightInterval : rightInterval / leftInterval;
			Melder_progress ((double) ipulse / pulses -> nt, L"Sound & PointProcess: To Ltas: pulse ", Melder_integer (ipulse), L" out of ", Melder_integer (pulses -> nt));
			if (leftInterval >= shortestPeriod && leftInterval <= longestPeriod &&
				rightInterval >= shortestPeriod && rightInterval <= longestPeriod &&
				intervalFactor <= maximumPeriodFactor)
			{
				/*
				 * We have a period! Compute the spectrum.
				 */
				autoSound period = Sound_extractPart (sound,
					pulses -> t [ipulse] - 0.5 * leftInterval, pulses -> t [ipulse] + 0.5 * rightInterval,
					kSound_windowShape_RECTANGULAR, 1.0, FALSE);
				autoSpectrum spectrum = Sound_to_Spectrum (period.peek(), FALSE);
				for (long ifreq = 1; ifreq <= spectrum -> nx; ifreq ++) {
					double frequency = spectrum -> xmin + (ifreq - 1) * spectrum -> dx;
					double realPart = spectrum -> z [1] [ifreq];
					double imaginaryPart = spectrum -> z [2] [ifreq];
					double energy = (realPart * realPart + imaginaryPart * imaginaryPart) * 2.0 * spectrum -> dx /* OLD: * sound -> nx */;
					long iband = ceil (frequency / bandWidth);
					if (iband >= 1 && iband <= ltas -> nx) {
						ltas -> z [1] [iband] += energy;
						numbers -> z [1] [iband] += 1;
						totalNumberOfEnergies += 1;
					}
				}
			} else {
				numberOfPeriods -= 1;
			}
		}
		if (numberOfPeriods < 1)
			Melder_throw ("There are no periods in the point process.");
		for (long iband = 1; iband <= ltas -> nx; iband ++) {
			if (numbers -> z [1] [iband] == 0.0) {
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
		for (long iband = 1; iband <= ltas -> nx; iband ++) {
			if (ltas -> z [1] [iband] == NUMundefined) {
				long ibandleft = iband - 1, ibandright = iband + 1;
				while (ibandleft >= 1 && ltas -> z [1] [ibandleft] == NUMundefined) ibandleft --;
				while (ibandright <= ltas -> nx && ltas -> z [1] [ibandright] == NUMundefined) ibandright ++;
				if (ibandleft < 1 && ibandright > ltas -> nx)
					Melder_throw ("Cannot create an Ltas without energy in any bins.");
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
		return ltas.transfer();
	} catch (MelderError) {
		Melder_throw (sound, " & ", pulses, ": LTAS analysis not performed.");
	}
}

Ltas Sound_to_Ltas_pitchCorrected (Sound sound, double minimumPitch, double maximumPitch,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor)
{
	try {
		autoPointProcess pulses = Sound_to_PointProcess_periodic_cc (sound, minimumPitch, maximumPitch);
		autoLtas ltas = PointProcess_Sound_to_Ltas (pulses.peek(), sound, maximumFrequency, bandWidth,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
		return ltas.transfer();
	} catch (MelderError) {
		Melder_throw (sound, ": pitch-corrected LTAS analysis not performed.");
	}
}

Ltas PointProcess_Sound_to_Ltas_harmonics (PointProcess pulses, Sound sound,
	long maximumHarmonic,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor)
{
	try {
		long numberOfPeriods = pulses -> nt - 2;
		autoLtas ltas = Ltas_create (maximumHarmonic, 1.0);
		ltas -> xmax = maximumHarmonic;
		if (numberOfPeriods < 1)
			Melder_throw ("There are no periods in the point process.");
		autoMelderProgress progress (L"LTAS (harmonics) analysis...");
		for (long ipulse = 2; ipulse < pulses -> nt; ipulse ++) {
			double leftInterval = pulses -> t [ipulse] - pulses -> t [ipulse - 1];
			double rightInterval = pulses -> t [ipulse + 1] - pulses -> t [ipulse];
			double intervalFactor = leftInterval > rightInterval ? leftInterval / rightInterval : rightInterval / leftInterval;
			Melder_progress ((double) ipulse / pulses -> nt, L"Sound & PointProcess: To Ltas: pulse ", Melder_integer (ipulse), L" out of ", Melder_integer (pulses -> nt));
			if (leftInterval >= shortestPeriod && leftInterval <= longestPeriod &&
				rightInterval >= shortestPeriod && rightInterval <= longestPeriod &&
				intervalFactor <= maximumPeriodFactor)
			{
				/*
				 * We have a period! Compute the spectrum.
				 */
				long localMaximumHarmonic;
				autoSound period = Sound_extractPart (sound,
					pulses -> t [ipulse] - 0.5 * leftInterval, pulses -> t [ipulse] + 0.5 * rightInterval,
					kSound_windowShape_RECTANGULAR, 1.0, FALSE);
				autoSpectrum spectrum = Sound_to_Spectrum (period.peek(), FALSE);
				localMaximumHarmonic = maximumHarmonic < spectrum -> nx ? maximumHarmonic : spectrum -> nx;
				for (long iharm = 1; iharm <= localMaximumHarmonic; iharm ++) {
					double realPart = spectrum -> z [1] [iharm];
					double imaginaryPart = spectrum -> z [2] [iharm];
					double energy = (realPart * realPart + imaginaryPart * imaginaryPart) * 2.0 * spectrum -> dx;
					ltas -> z [1] [iharm] += energy;
				}
			} else {
				numberOfPeriods -= 1;
			}
		}
		if (numberOfPeriods < 1)
			Melder_throw (L"There are no periods in the point process.");
		for (long iharm = 1; iharm <= ltas -> nx; iharm ++) {
			if (ltas -> z [1] [iharm] == 0.0) {
				ltas -> z [1] [iharm] = -300.0;
			} else {
				double energyInThisBand = ltas -> z [1] [iharm];
				double powerInThisBand = energyInThisBand / (sound -> xmax - sound -> xmin);
				ltas -> z [1] [iharm] = 10.0 * log10 (powerInThisBand / 4.0e-10);
			}
		}
		return ltas.transfer();
	} catch (MelderError) {
		Melder_throw (sound, " & ", pulses, ": LTAS analysis (harmonics) not performed.");
	}
}

/* End of file Ltas.cpp */
