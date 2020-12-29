/* Spectrum.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014-2020 Paul Boersma
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
 * pb 2001/11/30 Spectral moments
 * pb 2002/05/22 changed sign of imaginary part
 * pb 2006/02/06 better cepstral smoothing
 */

#include "Sound_and_Spectrum.h"
#include "SpectrumTier.h"

#include "oo_DESTROY.h"
#include "Spectrum_def.h"
#include "oo_COPY.h"
#include "Spectrum_def.h"
#include "oo_EQUAL.h"
#include "Spectrum_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Spectrum_def.h"
#include "oo_WRITE_TEXT.h"
#include "Spectrum_def.h"
#include "oo_READ_TEXT.h"
#include "Spectrum_def.h"
#include "oo_WRITE_BINARY.h"
#include "Spectrum_def.h"
#include "oo_READ_BINARY.h"
#include "Spectrum_def.h"
#include "oo_DESCRIPTION.h"
#include "Spectrum_def.h"

Thing_implement (Spectrum, Matrix, 2);

void structSpectrum :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Frequency domain:");
	MelderInfo_writeLine (U"   Lowest frequency: ", xmin, U" Hz");
	MelderInfo_writeLine (U"   Highest frequency: ", xmax, U" Hz");
	MelderInfo_writeLine (U"   Total bandwidth: ", xmax - xmin, U" Hz");
	MelderInfo_writeLine (U"Frequency sampling:");
	MelderInfo_writeLine (U"   Number of frequency bands (bins): ", nx);
	MelderInfo_writeLine (U"   Frequency step (bin width): ", dx, U" Hz");
	MelderInfo_writeLine (U"   First frequency band around (bin centre at): ", x1, U" Hz");
	MelderInfo_writeLine (U"Total energy: ", Melder_single (Spectrum_getBandEnergy (this, 0.0, 0.0)), U" Pa2 sec");
}

double structSpectrum :: v_getValueAtSample (integer isamp, integer which, int units) {
	if (units == 0) {
		return which == 1 ? z [1] [isamp] : which == 2 ? z [2] [isamp] : undefined;
	} else {
		/*
		 * The energy in a bin is 2 * (re^2 + im^2) times the bin width.
		 * The factor of 2 derives from the assumption that the spectrum contains positive-frequency values only,
		 * and that the negative-frequency values have the same norm, since they are the complex conjugates
		 * of the positive-frequency values.
		 */
		double energyDensity = 2.0 * (z [1] [isamp] * z [1] [isamp] + z [2] [isamp] * z [2] [isamp]);
			/* Pa2/Hz2; sum of positive and negative frequencies */
		if (units == 1) {
			return energyDensity;
		} else {
			double powerDensity = energyDensity * dx;   // Pa^2 Hz-2 s-1, after division by approximate duration
			if (units == 2) {
				/* "dB/Hz" */
				return powerDensity == 0.0 ? -300.0 : 10.0 * log10 (powerDensity / 4.0e-10);
			}
		}
	}
	return undefined;
}

autoSpectrum Spectrum_create (double fmax, integer nf) {
	try {
		autoSpectrum me = Thing_new (Spectrum);
		Matrix_init (me.get(), 0.0, fmax, nf, fmax / (nf - 1), 0.0, 1.0, 2.0, 2, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Spectrum not created.");
	}
}

int Spectrum_getPowerDensityRange (Spectrum me, double *minimum, double *maximum) {
	*minimum = 1e308;
	*maximum = 0.0;
	for (integer ifreq = 1; ifreq <= my nx; ifreq ++) {
		double oneSidedPowerSpectralDensity =   // Pa2 Hz-2 s-1
			2.0 * (my z [1] [ifreq] * my z [1] [ifreq] + my z [2] [ifreq] * my z [2] [ifreq]) * my dx;
		if (oneSidedPowerSpectralDensity < *minimum)
			*minimum = oneSidedPowerSpectralDensity;
		if (oneSidedPowerSpectralDensity > *maximum)
			*maximum = oneSidedPowerSpectralDensity;
	}
	if (*maximum == 0.0)
		return 0;
	*minimum = 10.0 * log10 (*minimum / 4.0e-10);
	*maximum = 10.0 * log10 (*maximum / 4.0e-10);
	return 1;
}

void Spectrum_drawInside (Spectrum me, Graphics g, double fmin, double fmax, double minimum, double maximum) {
	bool autoscaling = ( minimum >= maximum );
	if (fmax <= fmin) {
		fmin = my xmin;
		fmax = my xmax;
	}
	integer ifmin, ifmax;
	const integer nf = Matrix_getWindowSamplesX (me, fmin, fmax, & ifmin, & ifmax);
	if (nf == 0)
		return;
	autoVEC ybuffer = zero_VEC (nf);
	double *yWC = & ybuffer [1 - ifmin];

	/*
		First pass: compute power density.
	*/
	if (autoscaling)
		maximum = -1e308;
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		double y = my v_getValueAtSample (ifreq, 0, 2);
		if (autoscaling && y > maximum)
			maximum = y;
		yWC [ifreq] = y;
	}
	if (autoscaling) {
		constexpr double defaultDynamicRange_dB { 60.0 };
		minimum = maximum - defaultDynamicRange_dB;
		if (minimum == maximum) {   // because infinity minus something is still infinity
			Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
			Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
			Graphics_text (g, 0.5, 0.5, U"(undefined spectrum values cannot be drawn)");
			return;
		}
	}

	/*
		Second pass: clip.
	*/
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++)
		Melder_clip (minimum, & yWC [ifreq], maximum);

	Graphics_setWindow (g, fmin, fmax, minimum, maximum);
	Graphics_function (g, yWC, ifmin, ifmax, Matrix_columnToX (me, ifmin), Matrix_columnToX (me, ifmax));
}

void Spectrum_draw (Spectrum me, Graphics g, double fmin, double fmax, double minimum, double maximum, bool garnish) {
	Graphics_setInner (g);
	Spectrum_drawInside (me, g, fmin, fmax, minimum, maximum);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Sound pressure level (dB/Hz)");
		Graphics_marksLeftEvery (g, 1.0, 20.0, true, true, false);
	}
}

void Spectrum_drawLogFreq (Spectrum me, Graphics g, double fmin, double fmax, double minimum, double maximum, bool garnish) {
	bool autoscaling = ( minimum >= maximum );
	if (fmax <= fmin) {
		fmin = my xmin;
		fmax = my xmax;
	}
	integer ifmin, ifmax;
	const integer nf = Matrix_getWindowSamplesX (me, fmin, fmax, & ifmin, & ifmax);
	if (nf == 0)
		return;
if(ifmin==1)ifmin=2;  /* BUG */
	auto xbuffer = zero_VEC (nf), ybuffer = zero_VEC (nf);
	double *xWC = & xbuffer [1 - ifmin], *yWC = & ybuffer [1 - ifmin];

	/*
		First pass: compute power density.
	*/
	if (autoscaling)
		maximum = -1e6;
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		xWC [ifreq] = log10 (my x1 + (ifreq - 1) * my dx);
		yWC [ifreq] = my v_getValueAtSample (ifreq, 0, 2);
		if (autoscaling && yWC [ifreq] > maximum)
			maximum = yWC [ifreq];
	}
	if (autoscaling)
		minimum = maximum - 60;   // default dynamic range is 60 dB

	/*
		Second pass: clip.
	*/
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++)
		Melder_clip (minimum, & yWC [ifreq], maximum);

	Graphics_setInner (g);
	Graphics_setWindow (g, log10 (fmin), log10 (fmax), minimum, maximum);
	Graphics_polyline (g, ifmax - ifmin + 1, & xWC [ifmin], & yWC [ifmin]);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Frequency (Hz)");
		Graphics_marksBottomLogarithmic (g, 3, true, true, false);
		Graphics_textLeft (g, true, U"Sound pressure level (dB/Hz)");
		Graphics_marksLeftEvery (g, 1.0, 20.0, true, true, false);
	}
}

autoTable Spectrum_tabulate (Spectrum me, bool includeBinNumbers, bool includeFrequency,
	bool includeRealPart, bool includeImaginaryPart, bool includeEnergyDensity, bool includePowerDensity)
{
	try {
		autoTable thee = Table_createWithoutColumnNames (my nx,
				includeBinNumbers + includeFrequency + includeRealPart + includeImaginaryPart + includeEnergyDensity + includePowerDensity);
		integer icol = 0;
		if (includeBinNumbers)
			Table_setColumnLabel (thee.get(), ++ icol, U"bin");
		if (includeFrequency)
			Table_setColumnLabel (thee.get(), ++ icol, U"freq(Hz)");
		if (includeRealPart)
			Table_setColumnLabel (thee.get(), ++ icol, U"re(Pa/Hz)");
		if (includeImaginaryPart)
			Table_setColumnLabel (thee.get(), ++ icol, U"im(Pa/Hz)");
		if (includeEnergyDensity)
			Table_setColumnLabel (thee.get(), ++ icol, U"energy(Pa^2/Hz^2)");
		if (includePowerDensity)
			Table_setColumnLabel (thee.get(), ++ icol, U"pow(dB/Hz)");
		for (integer ibin = 1; ibin <= my nx; ibin ++) {
			icol = 0;
			if (includeBinNumbers)
				Table_setNumericValue (thee.get(), ibin, ++ icol, ibin);
			if (includeFrequency)
				Table_setNumericValue (thee.get(), ibin, ++ icol, my x1 + (ibin - 1) * my dx);
			if (includeRealPart)
				Table_setNumericValue (thee.get(), ibin, ++ icol, my z [1] [ibin]);
			if (includeImaginaryPart)
				Table_setNumericValue (thee.get(), ibin, ++ icol, my z [2] [ibin]);
			if (includeEnergyDensity)
				Table_setNumericValue (thee.get(), ibin, ++ icol, Sampled_getValueAtSample (me, ibin, 0, 1));
			if (includePowerDensity)
				Table_setNumericValue (thee.get(), ibin, ++ icol, Sampled_getValueAtSample (me, ibin, 0, 2));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Table.");
	}
}

autoSpectrum Matrix_to_Spectrum (Matrix me) {
	try {
		if (my ny != 2)
			Melder_throw (U"The Matrix should have exactly 2 rows.");
		autoSpectrum thee = Thing_new (Spectrum);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Spectrum.");
	}
}

autoMatrix Spectrum_to_Matrix (Spectrum me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoSpectrum Spectrum_cepstralSmoothing (Spectrum me, double bandWidth) {
	try {
		/*
			dB-spectrum is log (power).
		*/
		autoSpectrum dBspectrum = Data_copy (me);
		VEC re = dBspectrum -> z.row (1), im = dBspectrum -> z.row (2);
		for (integer i = 1; i <= dBspectrum -> nx; i ++) {
			re [i] = log (re [i] * re [i] + im [i] * im [i] + 1e-308);
			im [i] = 0.0;
		}

		/*
			Cepstrum is Fourier transform of dB-spectrum.
		*/
		autoSound cepstrum = Spectrum_to_Sound (dBspectrum.get());

		/*
			Multiply cepstrum by a Gaussian.
		*/
		const double factor = - bandWidth * bandWidth;
		for (integer i = 1; i <= cepstrum -> nx; i ++) {
			double t = (i - 1) * cepstrum -> dx;
			cepstrum -> z [1] [i] *= exp (factor * t * t) * ( i == 1 ? 1.0 : 2.0 );
		}

		/*
			Smoothed power spectrum is original power spectrum convolved with a Gaussian.
		*/
		autoSpectrum thee = Sound_to_Spectrum (cepstrum.get(), true);

		/*
			Convert power spectrum back into a "complex" spectrum without phase information.
		*/
		re = thy z.row (1);
		im = thy z.row (2);
		for (integer i = 1; i <= thy nx; i ++) {
			re [i] = exp (0.5 * re [i]);   // i.e., sqrt (exp (re [i]))
			im [i] = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cepstral smoothing not computed.");
	}
}

void Spectrum_passHannBand (Spectrum me, double fmin, double fmax0, double smooth) {
	double fmax = fmax0 == 0.0 ? my xmax : fmax0;
	double f1 = fmin - smooth, f2 = fmin + smooth, f3 = fmax - smooth, f4 = fmax + smooth;
	double halfpibysmooth = smooth != 0.0 ? NUMpi / (2.0 * smooth) : 0.0;
	double *re = & my z [1] [0], *im = & my z [2] [0];
	for (integer i = 1; i <= my nx; i ++) {
		double frequency = my x1 + (i - 1) * my dx;
		if (frequency < f1 || frequency > f4) re [i] = im [i] = 0.0;
		if (frequency < f2 && fmin > 0.0) {
			double factor = 0.5 - 0.5 * cos (halfpibysmooth * (frequency - f1));
			re [i] *= factor;
			im [i] *= factor;
		} else if (frequency > f3 && fmax < my xmax) {
			double factor = 0.5 + 0.5 * cos (halfpibysmooth * (frequency - f3));
			re [i] *= factor;
			im [i] *= factor;
		}
	}
}

void Spectrum_stopHannBand (Spectrum me, double fmin, double fmax0, double smooth) {
	double fmax = fmax0 == 0.0 ? my xmax : fmax0;
	double f1 = fmin - smooth, f2 = fmin + smooth, f3 = fmax - smooth, f4 = fmax + smooth;
	double halfpibysmooth = smooth != 0.0 ? NUMpi / (2.0 * smooth) : 0.0;
	VEC re = my z.row (1), im = my z.row (2);
	for (integer i = 1; i <= my nx; i ++) {
		double frequency = my x1 + (i - 1) * my dx;
		if (frequency < f1 || frequency > f4) continue;
		if (frequency < f2 && fmin > 0.0) {
			double factor = 0.5 + 0.5 * cos (halfpibysmooth * (frequency - f1));
			re [i] *= factor;
			im [i] *= factor;
		} else if (frequency > f3 && fmax < my xmax) {
			double factor = 0.5 - 0.5 * cos (halfpibysmooth * (frequency - f3));
			re [i] *= factor;
			im [i] *= factor;
		} else re [i] = im [i] = 0.0;
	}
}

double Spectrum_getBandEnergy (Spectrum me, double fmin, double fmax) {
	/*
	 * The computation requires that the negative-frequency values are the complex conjugates
	 * of the positive-frequency values, and that only the positive-frequency values are included
	 * in the spectrum.
	 */
	if (my xmin < 0.0) return undefined;
	/*
	 * Any energy outside [my xmin, my xmax] is ignored.
	 * This is very important, since my xmin and my xmax determine the meaning of the first and last bins; see below.
	 *
	 * The width of most bins is my dx, but the first and last bins can be truncated by fmin and fmax.
	 *
	 * This truncation even applies in the case of autowindowing,
	 * i.e. if the total energy in the spectrum is computed.
	 * In that case, the first and last bins can be truncated by my xmin and my xmax.
	 * This happens almost always for the first bin,
	 * which is usually centred at f=0, hence has a width of 0.5 * my dx,
	 * and quite often for the last bin as well (namely if the original sound had an even number of samples),
	 * which is then centred at f=nyquist, hence has a width of 0.5 * my dx.
	 *
	 * All this truncation is automatically performed by Sampled_getMean ().
	 */
	return Sampled_getIntegral (me, fmin, fmax, 0, 1, false);
}

double Spectrum_getBandDensity (Spectrum me, double fmin, double fmax) {
	if (my xmin < 0.0) return undefined;   // no negative frequencies allowed in one-sided spectral density
	return Sampled_getMean (me, fmin, fmax, 0, 1, false);
}

double Spectrum_getBandDensityDifference (Spectrum me, double lowBandMin, double lowBandMax, double highBandMin, double highBandMax) {
	double lowBandDensity = Spectrum_getBandDensity (me, lowBandMin, lowBandMax);
	double highBandDensity = Spectrum_getBandDensity (me, highBandMin, highBandMax);
	if (isundef (lowBandDensity) || isundef (highBandDensity)) return undefined;
	if (lowBandDensity == 0.0 || highBandDensity == 0.0) return undefined;
	return 10.0 * log10 (highBandDensity / lowBandDensity);
}

double Spectrum_getBandEnergyDifference (Spectrum me, double lowBandMin, double lowBandMax, double highBandMin, double highBandMax) {
	double lowBandEnergy = Spectrum_getBandEnergy (me, lowBandMin, lowBandMax);
	double highBandEnergy = Spectrum_getBandEnergy (me, highBandMin, highBandMax);
	if (isundef (lowBandEnergy) || isundef (highBandEnergy)) return undefined;
	if (lowBandEnergy == 0.0 || highBandEnergy == 0.0) return undefined;
	return 10.0 * log10 (highBandEnergy / lowBandEnergy);
}

double Spectrum_getCentreOfGravity (Spectrum me, double power) {
	double halfpower = 0.5 * power;
	longdouble sumenergy = 0.0, sumfenergy = 0.0;
	for (integer i = 1; i <= my nx; i ++) {
		double re = my z [1] [i], im = my z [2] [i], energy = re * re + im * im;
		double f = my x1 + (i - 1) * my dx;
		if (halfpower != 1.0) energy = pow (energy, halfpower);
		sumenergy += energy;
		sumfenergy += f * energy;
	}
	return sumenergy == 0.0 ? undefined : double (sumfenergy / sumenergy);
}

double Spectrum_getCentralMoment (Spectrum me, double moment, double power) {
	double fmean = Spectrum_getCentreOfGravity (me, power);
	if (isundef (fmean)) return undefined;
	double halfpower = 0.5 * power;
	longdouble sumenergy = 0.0, sumfenergy = 0.0;
	for (integer i = 1; i <= my nx; i ++) {
		double re = my z [1] [i], im = my z [2] [i], energy = re * re + im * im;
		double f = my x1 + (i - 1) * my dx;
		if (halfpower != 1.0) energy = pow (energy, halfpower);
		sumenergy += energy;
		sumfenergy += pow (f - fmean, moment) * energy;
	}
	return double (sumfenergy / sumenergy);
}

double Spectrum_getStandardDeviation (Spectrum me, double power) {
	return sqrt (Spectrum_getCentralMoment (me, 2.0, power));
}

double Spectrum_getSkewness (Spectrum me, double power) {
	double m2 = Spectrum_getCentralMoment (me, 2.0, power);
	double m3 = Spectrum_getCentralMoment (me, 3.0, power);
	if (isundef (m2) || isundef (m3) || m2 == 0.0) return undefined;
	return m3 / (m2 * sqrt (m2));
}

double Spectrum_getKurtosis (Spectrum me, double power) {
	double m2 = Spectrum_getCentralMoment (me, 2.0, power);
	double m4 = Spectrum_getCentralMoment (me, 4.0, power);
	if (isundef (m2) || isundef (m4) || m2 == 0.0) return undefined;
	return m4 / (m2 * m2) - 3;
}

MelderPoint Spectrum_getNearestMaximum (Spectrum me, double frequency) {
	try {
		autoSpectrumTier thee = Spectrum_to_SpectrumTier_peaks (me);
		integer index = AnyTier_timeToNearestIndex (thee.get()->asAnyTier(), frequency);
		if (index == 0)
			Melder_throw (U"No peak.");
		RealPoint point = thy points.at [index];
		MelderPoint result;
		result. x = point -> number;
		result. y = point -> value;
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": no nearest maximum found.");
	}
}

/* End of file Spectrum.cpp */
