/* Spectrum.c
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
 * pb 2001/11/30 Spectral moments
 * pb 2002/05/22 changed sign of imaginary part
 * pb 2002/07/16 GPL
 * pb 2004/05/07 support for odd number of samples
 * pb 2004/10/31 Sampled statistics
 * pb 2004/11/22 simplified Sound_to_Spectrum ()
 * pb 2006/02/06 better cepstral smoothing
 * pb 2007/03/17 domain quantity
 * pb 2007/03/30 Spectrum_downto_Table
 * pb 2007/08/12 wchar_t
 */

#include "Sound_and_Spectrum.h"

#include "oo_READ_TEXT.h"
#include "Spectrum_def.h"
#include "oo_READ_BINARY.h"
#include "Spectrum_def.h"

static void info (I) {
	iam (Spectrum);
	classData -> info (me);
	MelderInfo_writeLine1 (L"Frequency domain:");
	MelderInfo_writeLine3 (L"   Lowest frequency: ", Melder_double (my xmin), L" Hz");
	MelderInfo_writeLine3 (L"   Highest frequency: ", Melder_double (my xmax), L" Hz");
	MelderInfo_writeLine3 (L"   Total bandwidth: ", Melder_double (my xmax - my xmin), L" Hz");
	MelderInfo_writeLine1 (L"Frequency sampling:");
	MelderInfo_writeLine2 (L"   Number of frequency bands (bins): ", Melder_integer (my nx));
	MelderInfo_writeLine3 (L"   Frequency step (bin width): ", Melder_double (my dx), L" Hz");
	MelderInfo_writeLine3 (L"   First frequency band around (bin centre at): ", Melder_double (my x1), L" Hz");
	MelderInfo_writeLine3 (L"Total energy: ", Melder_single (Spectrum_getBandEnergy (me, 0.0, 0.0)), L" Pa2 sec");
}

static double getValueAtSample (I, long isamp, long which, int units) {
	iam (Spectrum);
	if (units == 0) {
		return which == 1 ? my z [1] [isamp] : which == 2 ? my z [2] [isamp] : NUMundefined;
	} else {
		/*
		 * The energy in a bin is 2 * (re^2 + im^2) times the bin width.
		 * The factor of 2 derives from the assumption that the spectrum contains positive-frequency values only,
		 * and that the negative-frequency values have the same norm, since they are the complex conjugates
		 * of the positive-frequency values.
		 */
		double energyDensity = 2.0 * (my z [1] [isamp] * my z [1] [isamp] + my z [2] [isamp] * my z [2] [isamp]);
			/* Pa2/Hz2; sum of positive and negative frequencies */
		if (units == 1) {
			return energyDensity;
		} else {
			double powerDensity = energyDensity * my dx;   /* Pa^2 Hz-2 s-1, after division by approximate duration */
			if (units == 2) {
				/* "dB/Hz" */
				return powerDensity == 0.0 ? -300.0 : 10 * log10 (powerDensity / 4.0e-10);
			}
		}
	}
	return NUMundefined;
}

class_methods (Spectrum, Matrix)
	us -> version = 1;   /* Changed sign of imaginary part. */
	class_method (info)
	class_method_local (Spectrum, readText)
	class_method_local (Spectrum, readBinary)
	us -> domainQuantity = MelderQuantity_FREQUENCY_HERTZ;
	class_method (getValueAtSample)
class_methods_end

Spectrum Spectrum_create (double fmax, long nf) {
	Spectrum me = new (Spectrum);
	if (! me || ! Matrix_init (me, 0.0, fmax, nf, fmax / (nf - 1), 0.0, 1, 2, 2, 1, 1))
		forget (me);
	return me;
}

int Spectrum_getPowerDensityRange (Spectrum me, double *minimum, double *maximum) {
	long ifreq;
	*minimum = 1e300, *maximum = 0;
	for (ifreq = 1; ifreq <= my nx; ifreq ++) {
		double oneSidedPowerSpectralDensity =   /* Pa2 Hz-2 s-1 */
			2 * (my z [1] [ifreq] * my z [1] [ifreq] + my z [2] [ifreq] * my z [2] [ifreq]) * my dx;
		if (oneSidedPowerSpectralDensity < *minimum) *minimum = oneSidedPowerSpectralDensity;
		if (oneSidedPowerSpectralDensity > *maximum) *maximum = oneSidedPowerSpectralDensity;
	}
	if (*maximum == 0.0) return 0;
	*minimum = 10 * log10 (*minimum / 4.0e-10);
	*maximum = 10 * log10 (*maximum / 4.0e-10);
	return 1;
}

void Spectrum_drawInside (Spectrum me, Graphics g, double fmin, double fmax, double minimum, double maximum) {
	float *yWC = NULL;
	long ifreq, ifmin, ifmax;
	int autoscaling = minimum >= maximum;

	if (fmax <= fmin) { fmin = my xmin; fmax = my xmax; }
	if (! Matrix_getWindowSamplesX (me, fmin, fmax, & ifmin, & ifmax)) return;

	yWC = NUMfvector (ifmin, ifmax); cherror

	/*
	 * First pass: compute power density.
	 */
	if (autoscaling) maximum = -1e30;
	for (ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		double y = our getValueAtSample (me, ifreq, 0, 2);
		if (autoscaling && y > maximum) maximum = y;
		yWC [ifreq] = y;
	}
	if (autoscaling) minimum = maximum - 60;   /* Default dynamic range is 60 dB. */

	/*
	 * Second pass: clip.
	 */
	for (ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		if (yWC [ifreq] < minimum) yWC [ifreq] = minimum;
		else if (yWC [ifreq] > maximum) yWC [ifreq] = maximum;
	}

	Graphics_setWindow (g, fmin, fmax, minimum, maximum);
	Graphics_function (g, yWC, ifmin, ifmax, Matrix_columnToX (me, ifmin), Matrix_columnToX (me, ifmax));
end:
	NUMfvector_free (yWC, ifmin);
	Melder_clearError ();
}

void Spectrum_draw (Spectrum me, Graphics g, double fmin, double fmax, double minimum, double maximum, int garnish) {
	Graphics_setInner (g);
	Spectrum_drawInside (me, g, fmin, fmax, minimum, maximum);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Frequency (Hz)");
		Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);
		Graphics_textLeft (g, 1, L"Sound pressure level (dB/Hz)");
		Graphics_marksLeftEvery (g, 1.0, 20.0, TRUE, TRUE, FALSE);
	}
}

void Spectrum_drawLogFreq (Spectrum me, Graphics g, double fmin, double fmax, double minimum, double maximum, int garnish) {
	float *xWC = NULL, *yWC = NULL;
	long ifreq, ifmin, ifmax;
	int autoscaling = minimum >= maximum;
	if (fmax <= fmin) { fmin = my xmin; fmax = my xmax; }
	if (! Matrix_getWindowSamplesX (me, fmin, fmax, & ifmin, & ifmax)) return;
if(ifmin==1)ifmin=2;  /* BUG */
	xWC = NUMfvector (ifmin, ifmax); cherror
	yWC = NUMfvector (ifmin, ifmax); cherror

	/*
	 * First pass: compute power density.
	 */
	if (autoscaling) maximum = -1e6;
	for (ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		xWC [ifreq] = log10 (my x1 + (ifreq - 1) * my dx);
		yWC [ifreq] = our getValueAtSample (me, ifreq, 0, 2);
		if (autoscaling && yWC [ifreq] > maximum) maximum = yWC [ifreq];
	}
	if (autoscaling) minimum = maximum - 60;   /* Default dynamic range is 60 dB. */

	/*
	 * Second pass: clip.
	 */
	for (ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		if (yWC [ifreq] < minimum) yWC [ifreq] = minimum;
		else if (yWC [ifreq] > maximum) yWC [ifreq] = maximum;
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, log10 (fmin), log10 (fmax), minimum, maximum);
	Graphics_polyline (g, ifmax - ifmin + 1, & xWC [ifmin], & yWC [ifmin]);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Frequency (Hz)");
		Graphics_marksBottomLogarithmic (g, 3, TRUE, TRUE, FALSE);
		Graphics_textLeft (g, 1, L"Sound pressure level (dB/Hz)");
		Graphics_marksLeftEvery (g, 1.0, 20.0, TRUE, TRUE, FALSE);
	}
end:
	NUMfvector_free (xWC, ifmin);
	NUMfvector_free (yWC, ifmin);
	Melder_clearError ();
}

Table Spectrum_downto_Table (Spectrum me, bool includeBinNumbers, bool includeFrequency,
	bool includeRealPart, bool includeImaginaryPart, bool includeEnergyDensity, bool includePowerDensity)
{
	Table thee = Table_createWithoutColumnNames (my nx,
		includeBinNumbers + includeFrequency + includeRealPart + includeImaginaryPart + includeEnergyDensity + includePowerDensity); cherror
	long icol = 0;
	if (includeBinNumbers) { Table_setColumnLabel (thee, ++ icol, L"bin"); cherror }
	if (includeFrequency) { Table_setColumnLabel (thee, ++ icol, L"freq(Hz)"); cherror }
	if (includeRealPart) { Table_setColumnLabel (thee, ++ icol, L"re(Pa/Hz)"); cherror }
	if (includeImaginaryPart) { Table_setColumnLabel (thee, ++ icol, L"im(Pa/Hz)"); cherror }
	if (includeEnergyDensity) { Table_setColumnLabel (thee, ++ icol, L"energy(Pa^2/Hz^2)"); cherror }
	if (includePowerDensity) { Table_setColumnLabel (thee, ++ icol, L"pow(dB/Hz)"); cherror }
	for (long ibin = 1; ibin <= my nx; ibin ++) {
		icol = 0;
		if (includeBinNumbers) { Table_setNumericValue (thee, ibin, ++ icol, ibin); cherror }
		if (includeFrequency) { Table_setNumericValue (thee, ibin, ++ icol, my x1 + (ibin - 1) * my dx); cherror }
		if (includeRealPart) { Table_setNumericValue (thee, ibin, ++ icol, my z [1] [ibin]); cherror }
		if (includeImaginaryPart) { Table_setNumericValue (thee, ibin, ++ icol, my z [2] [ibin]); cherror }
		if (includeEnergyDensity) { Table_setNumericValue (thee, ibin, ++ icol, Sampled_getValueAtSample (me, ibin, 0, 1)); cherror }
		if (includePowerDensity) { Table_setNumericValue (thee, ibin, ++ icol, Sampled_getValueAtSample (me, ibin, 0, 2)); cherror }
	}
end:
	iferror forget (thee);
	return thee;
}

void Spectrum_list (Spectrum me, bool includeBinNumbers, bool includeFrequency,
	bool includeRealPart, bool includeImaginaryPart, bool includeEnergyDensity, bool includePowerDensity)
{
	Table table = Spectrum_downto_Table (me, includeBinNumbers, includeFrequency,
		includeRealPart, includeImaginaryPart, includeEnergyDensity, includePowerDensity); cherror
	Table_list (table, false);
end:
	iferror { Melder_clearError (); Melder_information1 (L"Nothing to list."); }
	forget (table);
}

Spectrum Matrix_to_Spectrum (Matrix me) {
	Spectrum thee = NULL;
	if (my ny != 2)
		return Melder_errorp ("(Matrix_to_Spectrum:) Matrix must have exactly 2 rows.");
	if (! (thee = Data_copy (me))) return NULL;
	Thing_overrideClass (thee, classSpectrum);
	return thee;
}

Matrix Spectrum_to_Matrix (Spectrum me) {
	Matrix thee = Data_copy (me);
	if (! thee) return NULL;
	Thing_overrideClass (thee, classMatrix);
	return thee;
}

Spectrum Spectrum_cepstralSmoothing (Spectrum me, double bandWidth) {
	Spectrum dBspectrum = NULL, thee = NULL;
	Sound cepstrum = NULL;
	float *re, *im;
	double factor = - bandWidth * bandWidth;
	long i;

	/*
	 * dB-spectrum is log (power).
	 */
	if (! (dBspectrum = Data_copy (me))) goto cleanUp;
	re = dBspectrum -> z [1], im = dBspectrum -> z [2];
	for (i = 1; i <= dBspectrum -> nx; i ++)
		{ re [i] = log (re [i] * re [i] + im [i] * im [i] + 1e-300); im [i] = 0; }

	/*
	 * Cepstrum is Fourier transform of dB-spectrum.
	 */
	if (! (cepstrum = Spectrum_to_Sound (dBspectrum))) goto cleanUp;

	/*
	 * Multiply cepstrum by a Gaussian.
	 */
	for (i = 1; i <= cepstrum -> nx; i ++) {
		double t = (i - 1) * cepstrum -> dx;
		cepstrum -> z [1] [i] *= exp (factor * t * t) * ( i == 1 ? 1 : 2 );
	}

	/*
	 * Smoothed power spectrum is original power spectrum convolved with a Gaussian.
	 */
	if (! (thee = Sound_to_Spectrum (cepstrum, TRUE))) goto cleanUp;

	/*
	 * Convert power spectrum back into a "complex" spectrum without phase information.
	 */
	re = thy z [1], im = thy z [2];
	for (i = 1; i <= thy nx; i ++)
		{ re [i] = exp (0.5 * re [i]); im [i] = 0; }   /* I.e., sqrt (exp (re [i])). */

cleanUp:
	forget (dBspectrum);
	forget (cepstrum);
	if (Melder_hasError ()) { forget (thee); Melder_error ("(Spectrum_cepstralSmoothing:) Not performed."); }
	return thee;
}

void Spectrum_passHannBand (Spectrum me, double fmin, double fmax0, double smooth) {
	long i;
	double fmax = fmax0 == 0.0 ? my xmax : fmax0;
	double f1 = fmin - smooth, f2 = fmin + smooth, f3 = fmax - smooth, f4 = fmax + smooth;
	double halfpibysmooth = smooth != 0.0 ? NUMpi / (2 * smooth) : 0.0;
	float *re = my z [1], *im = my z [2];
	for (i = 1; i <= my nx; i ++) {
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
	long i;
	double fmax = fmax0 == 0.0 ? my xmax : fmax0;
	double f1 = fmin - smooth, f2 = fmin + smooth, f3 = fmax - smooth, f4 = fmax + smooth;
	double halfpibysmooth = smooth != 0.0 ? NUMpi / (2 * smooth) : 0.0;
	float *re = my z [1], *im = my z [2];
	for (i = 1; i <= my nx; i ++) {
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
	if (my xmin < 0.0) return NUMundefined;
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
	return Sampled_getIntegral (me, fmin, fmax, 0, 1, FALSE);
}

double Spectrum_getBandDensity (Spectrum me, double fmin, double fmax) {
	if (my xmin < 0.0) return NUMundefined;   /* No negative frequencies allowed in one-sided spectral density. */
	return Sampled_getMean (me, fmin, fmax, 0, 1, FALSE);
}

double Spectrum_getBandDensityDifference (Spectrum me, double lowBandMin, double lowBandMax, double highBandMin, double highBandMax) {
	double lowBandDensity = Spectrum_getBandDensity (me, lowBandMin, lowBandMax);
	double highBandDensity = Spectrum_getBandDensity (me, highBandMin, highBandMax);
	if (lowBandDensity == NUMundefined || highBandDensity == NUMundefined) return NUMundefined;
	if (lowBandDensity == 0.0 || highBandDensity == 0.0) return NUMundefined;
	return 10 * log10 (highBandDensity / lowBandDensity);
}

double Spectrum_getBandEnergyDifference (Spectrum me, double lowBandMin, double lowBandMax, double highBandMin, double highBandMax) {
	double lowBandEnergy = Spectrum_getBandEnergy (me, lowBandMin, lowBandMax);
	double highBandEnergy = Spectrum_getBandEnergy (me, highBandMin, highBandMax);
	if (lowBandEnergy == NUMundefined || highBandEnergy == NUMundefined) return NUMundefined;
	if (lowBandEnergy == 0.0 || highBandEnergy == 0.0) return NUMundefined;
	return 10 * log10 (highBandEnergy / lowBandEnergy);
}

double Spectrum_getCentreOfGravity (Spectrum me, double power) {
	long i;
	double halfpower = 0.5 * power, sumenergy = 0.0, sumfenergy = 0.0;
	for (i = 1; i <= my nx; i ++) {
		double re = my z [1] [i], im = my z [2] [i], energy = re * re + im * im;
		double f = my x1 + (i - 1) * my dx;
		if (halfpower != 1.0) energy = pow (energy, halfpower);
		sumenergy += energy;
		sumfenergy += f * energy;
	}
	return sumenergy == 0.0 ? NUMundefined : sumfenergy / sumenergy;
}

double Spectrum_getCentralMoment (Spectrum me, double moment, double power) {
	long i;
	double halfpower = 0.5 * power, sumenergy = 0.0, sumfenergy = 0.0;
	double fmean = Spectrum_getCentreOfGravity (me, power);
	if (fmean == NUMundefined) return NUMundefined;
	for (i = 1; i <= my nx; i ++) {
		double re = my z [1] [i], im = my z [2] [i], energy = re * re + im * im;
		double f = my x1 + (i - 1) * my dx;
		if (halfpower != 1.0) energy = pow (energy, halfpower);
		sumenergy += energy;
		sumfenergy += pow (f - fmean, moment) * energy;
	}
	return sumfenergy / sumenergy;
}

double Spectrum_getStandardDeviation (Spectrum me, double power) {
	return sqrt (Spectrum_getCentralMoment (me, 2.0, power));
}

double Spectrum_getSkewness (Spectrum me, double power) {
	double m2 = Spectrum_getCentralMoment (me, 2.0, power);
	double m3 = Spectrum_getCentralMoment (me, 3.0, power);
	if (m2 == NUMundefined || m3 == NUMundefined || m2 == 0) return NUMundefined;
	return m3 / (m2 * sqrt (m2));
}

double Spectrum_getKurtosis (Spectrum me, double power) {
	double m2 = Spectrum_getCentralMoment (me, 2.0, power);
	double m4 = Spectrum_getCentralMoment (me, 4.0, power);
	if (m2 == NUMundefined || m4 == NUMundefined || m2 == 0) return NUMundefined;
	return m4 / (m2 * m2) - 3;
}

/* End of file Spectrum.c */
