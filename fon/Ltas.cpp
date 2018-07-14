/* Ltas.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2017 Paul Boersma
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
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Frequency domain:");
	MelderInfo_writeLine (U"   Lowest frequency: ", xmin, U" Hz");
	MelderInfo_writeLine (U"   Highest frequency: ", xmax, U" Hz");
	MelderInfo_writeLine (U"   Total frequency domain: ", xmax - xmin, U" Hz");
	MelderInfo_writeLine (U"Frequency sampling:");
	MelderInfo_writeLine (U"   Number of frequency bands: ", nx);
	MelderInfo_writeLine (U"   Width of each band: ", dx, U" Hz");
	MelderInfo_writeLine (U"   First band centred at: ", x1, U" Hz");
	meanPowerDensity = Sampled_getMean (this, xmin, xmax, 0, 1, false);
	MelderInfo_writeLine (U"Total SPL: ", Melder_single (10.0 * log10 (meanPowerDensity * (xmax - xmin))), U" dB");
}

double structLtas :: v_convertStandardToSpecialUnit (double value, integer /* level */, int unit) {
	if (unit == 1) {
		return pow (10.0, 0.1 * value);   // energy
	} else if (unit == 2) {
		return pow (2.0, 0.1 * value);   // sones
	}
	return value;
}

double structLtas :: v_convertSpecialToStandardUnit (double value, integer /* level */, int unit) {
	return
		unit == 1 ?
			10.0 * log10 (value) :   // value = energy
		unit == 2 ?
			10.0 * NUMlog2 (value) :   // value = sones
		value;   // value = dB
}

autoLtas Ltas_create (integer nx, double dx) {
	try {
		autoLtas me = Thing_new (Ltas);
		Matrix_init (me.get(), 0.0, nx * dx, nx, dx, 0.5 * dx, 1.0, 1.0, 1, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Ltas not created.");
	}
}

void Ltas_draw (Ltas me, Graphics g, double fmin, double fmax, double minimum, double maximum, bool garnish, conststring32 method) {
	Vector_draw (me, g, & fmin, & fmax, & minimum, & maximum, 1.0, method);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Sound pressure level (dB/Hz)");
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

double Ltas_getSlope (Ltas me, double f1min, double f1max, double f2min, double f2max, int averagingUnits) {
	double low = Sampled_getMean (me, f1min, f1max, 0, averagingUnits, false);
	double high = Sampled_getMean (me, f2min, f2max, 0, averagingUnits, false);
	if (isundef (low) || isundef (high)) return undefined;
	return averagingUnits == 3 ? high - low : Function_convertSpecialToStandardUnit (me, high / low, 0, averagingUnits);
}

double Ltas_getLocalPeakHeight (Ltas me, double environmentMin, double environmentMax, double peakMin, double peakMax, int averagingUnits) {
	double environmentLow = Sampled_getMean (me, environmentMin, peakMin, 0, averagingUnits, false);
	double environmentHigh = Sampled_getMean (me, peakMax, environmentMax, 0, averagingUnits, false);
	double peak = Sampled_getMean (me, peakMin, peakMax, 0, averagingUnits, false);
	if (isundef (environmentLow) || isundef (environmentHigh) || isundef (peak)) return undefined;
	return averagingUnits == 3 ? peak - 0.5 * (environmentLow + environmentHigh) :
		Function_convertSpecialToStandardUnit (me, peak / (0.5 * (environmentLow + environmentHigh)), 0, averagingUnits);
}

autoMatrix Ltas_to_Matrix (Ltas me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoLtas Matrix_to_Ltas (Matrix me) {
	try {
		autoLtas thee = Thing_new (Ltas);
		my structMatrix :: v_copy (thee.get());   // because copying to a descendant of Matrix with additional members should not crash
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Ltas.");
	}
}

autoLtas Ltases_merge (LtasBag ltases) {
	try {
		if (ltases->size < 1)
			Melder_throw (U"Cannot merge zero Ltas objects.");
		Ltas me = ltases->at [1];
		autoLtas thee = Data_copy (me);
		/*
		 * Convert to energy.
		 */
		for (integer iband = 1; iband <= thy nx; iband ++) {
			thy z [1] [iband] = pow (10.0, thy z [1] [iband] / 10.0);
		}
		for (integer ispec = 2; ispec <= ltases->size; ispec ++) {
			Ltas him = ltases->at [ispec];
			if (his xmin != thy xmin || his xmax != thy xmax)
				Melder_throw (U"Frequency domains do not match.");
			if (his dx != thy dx)
				Melder_throw (U"Bandwidths do not match.");
			if (his nx != thy nx || his x1 != thy x1)
				Melder_throw (U"Frequency bands do not match.");
			/*
			 * Add band energies.
			 */
			for (integer iband = 1; iband <= thy nx; iband ++) {
				thy z [1] [iband] += pow (10.0, his z [1] [iband] / 10.0);
			}
		}
		/*
		 * Convert back to dB.
		 */
		for (integer iband = 1; iband <= thy nx; iband ++) {
			thy z [1] [iband] = 10.0 * log10 (thy z [1] [iband]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Ltas objects not merged.");
	}
}

Thing_implement (LtasBag, Collection, 0);

autoLtas Ltases_average (LtasBag ltases) {
	try {
		double factor = -10.0 * log10 (ltases->size);
		autoLtas thee = Ltases_merge (ltases);
		for (integer iband = 1; iband <= thy nx; iband ++) {
			thy z [1] [iband] += factor;
		}	
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Ltas objects not averaged.");
	}
}

autoLtas Ltas_computeTrendLine (Ltas me, double fmin, double fmax) {
	try {
		/*
		 * Find the first and last bin.
		 */
		integer imin, imax, n;
		if ((n = Sampled_getWindowSamples (me, fmin, fmax, & imin, & imax)) < 2)
			Melder_throw (U"Number of bins too low (", n, U"). Should be at least 2.");
		autoLtas thee = Data_copy (me);
		/*
		 * Compute average amplitude and frequency.
		 */
		longdouble sum = 0.0, numerator = 0.0, denominator = 0.0;
		for (integer i = imin; i <= imax; i ++) {
			sum += thy z [1] [i];
		}
		double amean = double (sum / n);
		double fmean = thy x1 + (0.5 * (imin + imax) - 1) * thy dx;
		/*
		 * Compute slope.
		 */
		for (integer i = imin; i <= imax; i ++) {
			double da = thy z [1] [i] - amean, df = thy x1 + (i - 1) * thy dx - fmean;
			numerator += da * df;
			denominator += df * df;
		}
		double slope = double (numerator / denominator);
		/*
		 * Modify bins.
		 */
		for (integer i = 1; i <= thy nx; i ++) {
			double df = thy x1 + (i - 1) * thy dx - fmean;
			thy z [1] [i] = amean + slope * df;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": trend line not computed.");
	}
}

autoLtas Ltas_subtractTrendLine (Ltas me, double fmin, double fmax) {
	try {
		/*
		 * Find the first and last bin.
		 */
		integer imin, imax, n;
		if ((n = Sampled_getWindowSamples (me, fmin, fmax, & imin, & imax)) < 2)
			Melder_throw (U"Number of bins too low (", n, U"). Should be at least 2.");
		autoLtas thee = Data_copy (me);
		/*
		 * Compute average amplitude and frequency.
		 */
		longdouble sum = 0.0;
		for (integer i = imin; i <= imax; i ++) {
			sum += thy z [1] [i];
		}
		double amean = (double) sum / n;
		double fmean = thy x1 + (0.5 * (imin + imax) - 1) * thy dx;
		/*
		 * Compute slope.
		 */
		longdouble numerator = 0.0, denominator = 0.0;
		for (integer i = imin; i <= imax; i ++) {
			double da = thy z [1] [i] - amean, df = thy x1 + (i - 1) * thy dx - fmean;
			numerator += da * df;
			denominator += df * df;
		}
		double slope = (double) (numerator / denominator);
		/*
		 * Modify bins.
		 */
		for (integer i = 1; i < imin; i ++) {
			thy z [1] [i] = 0.0;
		}
		for (integer i = imin; i <= imax; i ++) {
			double df = thy x1 + (i - 1) * thy dx - fmean;
			thy z [1] [i] -= amean + slope * df;
		}
		for (integer i = imax + 1; i <= thy nx; i ++) {
			thy z [1] [i] = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": trend line not subtracted.");
	}
}

autoLtas Spectrum_to_Ltas (Spectrum me, double bandWidth) {
	try {
		integer numberOfBands = Melder_iceiling ((my xmax - my xmin) / bandWidth);
		if (bandWidth <= my dx)
			Melder_throw (U"Bandwidth must be greater than ", my dx, U".");
		autoLtas thee = Thing_new (Ltas);
		Matrix_init (thee.get(), my xmin, my xmax, numberOfBands, bandWidth, my xmin + 0.5 * bandWidth, 1.0, 1.0, 1, 1.0, 1.0);
		for (integer iband = 1; iband <= numberOfBands; iband ++) {
			double fmin = thy xmin + (iband - 1) * bandWidth;
			double meanEnergyDensity = Sampled_getMean (me, fmin, fmin + bandWidth, 0, 1, false);
			double meanPowerDensity = meanEnergyDensity * my dx;   // as an approximation for a division by the original duration
			thy z [1] [iband] = meanPowerDensity == 0.0 ? -300.0 : 10.0 * log10 (meanPowerDensity / 4.0e-10);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Ltas.");
	}
}

autoLtas Spectrum_to_Ltas_1to1 (Spectrum me) {
	try {
		autoLtas thee = Thing_new (Ltas);
		Matrix_init (thee.get(), my xmin, my xmax, my nx, my dx, my x1, 1.0, 1.0, 1, 1.0, 1.0);
		for (integer iband = 1; iband <= my nx; iband ++) {
			thy z [1] [iband] = Sampled_getValueAtSample (me, iband, 0, 2);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Ltas.");
	}
}

autoLtas Sound_to_Ltas (Sound me, double bandwidth) {
	try {
		autoSpectrum thee = Sound_to_Spectrum (me, true);
		autoLtas him = Spectrum_to_Ltas (thee.get(), bandwidth);
		double correction = -10.0 * log10 (thy dx * my nx * my dx);
		for (integer iband = 1; iband <= his nx; iband ++) {
			his z [1] [iband] += correction;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": LTAS analysis not performed.");
	}
}

autoLtas PointProcess_Sound_to_Ltas (PointProcess pulses, Sound sound,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor)
{
	try {
		integer numberOfPeriods = pulses -> nt - 2, totalNumberOfEnergies = 0;
		autoLtas ltas = Ltas_create (Melder_ifloor (maximumFrequency / bandWidth), bandWidth);
		ltas -> xmax = maximumFrequency;
		autoLtas numbers = Data_copy (ltas.get());
		if (numberOfPeriods < 1)
			Melder_throw (U"Cannot compute an Ltas if there are no periods in the point process.");
		autoMelderProgress progress (U"Ltas analysis...");
		for (integer ipulse = 2; ipulse < pulses -> nt; ipulse ++) {
			double leftInterval = pulses -> t [ipulse] - pulses -> t [ipulse - 1];
			double rightInterval = pulses -> t [ipulse + 1] - pulses -> t [ipulse];
			double intervalFactor = leftInterval > rightInterval ? leftInterval / rightInterval : rightInterval / leftInterval;
			Melder_progress ((double) ipulse / pulses -> nt, U"Sound & PointProcess: To Ltas: pulse ", ipulse, U" out of ", pulses -> nt);
			if (leftInterval >= shortestPeriod && leftInterval <= longestPeriod &&
				rightInterval >= shortestPeriod && rightInterval <= longestPeriod &&
				intervalFactor <= maximumPeriodFactor)
			{
				/*
				 * We have a period! Compute the spectrum.
				 */
				autoSound period = Sound_extractPart (sound,
					pulses -> t [ipulse] - 0.5 * leftInterval, pulses -> t [ipulse] + 0.5 * rightInterval,
					kSound_windowShape::RECTANGULAR, 1.0, false);
				autoSpectrum spectrum = Sound_to_Spectrum (period.get(), false);
				for (integer ifreq = 1; ifreq <= spectrum -> nx; ifreq ++) {
					double frequency = spectrum -> xmin + (ifreq - 1) * spectrum -> dx;
					double realPart = spectrum -> z [1] [ifreq];
					double imaginaryPart = spectrum -> z [2] [ifreq];
					double energy = (realPart * realPart + imaginaryPart * imaginaryPart) * 2.0 * spectrum -> dx /* OLD: * sound -> nx */;
					integer iband = Melder_iceiling (frequency / bandWidth);
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
			Melder_throw (U"There are no periods in the point process.");
		for (integer iband = 1; iband <= ltas -> nx; iband ++) {
			if (numbers -> z [1] [iband] == 0.0) {
				ltas -> z [1] [iband] = undefined;
			} else {
				/*
				 * Each bin now contains a total energy in Pa2 sec.
				 * To convert this to power density, we
				 */
				double totalEnergyInThisBand = ltas -> z [1] [iband];
				if (false /* i.e. if you just want to have a spectrum of the voiced parts... */) {
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
		for (integer iband = 1; iband <= ltas -> nx; iband ++) {
			if (isundef (ltas -> z [1] [iband])) {
				integer ibandleft = iband - 1, ibandright = iband + 1;
				while (ibandleft >= 1 && isundef (ltas -> z [1] [ibandleft])) ibandleft --;
				while (ibandright <= ltas -> nx && isundef (ltas -> z [1] [ibandright])) ibandright ++;
				if (ibandleft < 1 && ibandright > ltas -> nx)
					Melder_throw (U"Cannot create an Ltas without energy in any bins.");
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
		return ltas;
	} catch (MelderError) {
		Melder_throw (sound, U" & ", pulses, U": LTAS analysis not performed.");
	}
}

autoLtas Sound_to_Ltas_pitchCorrected (Sound sound, double minimumPitch, double maximumPitch,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor)
{
	try {
		autoPointProcess pulses = Sound_to_PointProcess_periodic_cc (sound, minimumPitch, maximumPitch);
		autoLtas ltas = PointProcess_Sound_to_Ltas (pulses.get(), sound, maximumFrequency, bandWidth,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
		return ltas;
	} catch (MelderError) {
		Melder_throw (sound, U": pitch-corrected LTAS analysis not performed.");
	}
}

autoLtas PointProcess_Sound_to_Ltas_harmonics (PointProcess pulses, Sound sound,
	integer maximumHarmonic,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor)
{
	try {
		integer numberOfPeriods = pulses -> nt - 2;
		autoLtas ltas = Ltas_create (maximumHarmonic, 1.0);
		ltas -> xmax = maximumHarmonic;
		if (numberOfPeriods < 1)
			Melder_throw (U"There are no periods in the point process.");
		autoMelderProgress progress (U"LTAS (harmonics) analysis...");
		for (integer ipulse = 2; ipulse < pulses -> nt; ipulse ++) {
			double leftInterval = pulses -> t [ipulse] - pulses -> t [ipulse - 1];
			double rightInterval = pulses -> t [ipulse + 1] - pulses -> t [ipulse];
			double intervalFactor = leftInterval > rightInterval ? leftInterval / rightInterval : rightInterval / leftInterval;
			Melder_progress ((double) ipulse / pulses -> nt, U"Sound & PointProcess: To Ltas: pulse ", ipulse, U" out of ", pulses -> nt);
			if (leftInterval >= shortestPeriod && leftInterval <= longestPeriod &&
				rightInterval >= shortestPeriod && rightInterval <= longestPeriod &&
				intervalFactor <= maximumPeriodFactor)
			{
				/*
				 * We have a period! Compute the spectrum.
				 */
				integer localMaximumHarmonic;
				autoSound period = Sound_extractPart (sound,
					pulses -> t [ipulse] - 0.5 * leftInterval, pulses -> t [ipulse] + 0.5 * rightInterval,
					kSound_windowShape::RECTANGULAR, 1.0, false);
				autoSpectrum spectrum = Sound_to_Spectrum (period.get(), false);
				localMaximumHarmonic = maximumHarmonic < spectrum -> nx ? maximumHarmonic : spectrum -> nx;
				for (integer iharm = 1; iharm <= localMaximumHarmonic; iharm ++) {
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
			Melder_throw (U"There are no periods in the point process.");
		for (integer iharm = 1; iharm <= ltas -> nx; iharm ++) {
			if (ltas -> z [1] [iharm] == 0.0) {
				ltas -> z [1] [iharm] = -300.0;
			} else {
				double energyInThisBand = ltas -> z [1] [iharm];
				double powerInThisBand = energyInThisBand / (sound -> xmax - sound -> xmin);
				ltas -> z [1] [iharm] = 10.0 * log10 (powerInThisBand / 4.0e-10);
			}
		}
		return ltas;
	} catch (MelderError) {
		Melder_throw (sound, U" & ", pulses, U": LTAS analysis (harmonics) not performed.");
	}
}

/* End of file Ltas.cpp */
