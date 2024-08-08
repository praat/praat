/* Spectrogram.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2015-2020,2022-2024 Paul Boersma
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

#include "Spectrogram.h"

Thing_implement (Spectrogram, Matrix, 2);

void structSpectrogram :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of time slices (frames): ", nx);
	MelderInfo_writeLine (U"   Time step (frame distance): ", dx, U" seconds");
	MelderInfo_writeLine (U"   First time slice (frame centre) at: ", x1, U" seconds");
	MelderInfo_writeLine (U"Frequency domain:");
	MelderInfo_writeLine (U"   Lowest frequency: ", ymin, U" Hz");
	MelderInfo_writeLine (U"   Highest frequency: ", ymax, U" Hz");
	MelderInfo_writeLine (U"   Total bandwidth: ", ymax - ymin, U" Hz");
	MelderInfo_writeLine (U"Frequency sampling:");
	MelderInfo_writeLine (U"   Number of frequency bands (bins): ", ny);
	MelderInfo_writeLine (U"   Frequency step (bin width): ", dy, U" Hz");
	MelderInfo_writeLine (U"   First frequency band around (bin centre at): ", y1, U" Hz");
}

autoSpectrogram Spectrogram_create (double tmin, double tmax, integer nt, double dt, double t1,
	double fmin, double fmax, integer nf, double df, double f1)
{
	try {
		autoSpectrogram me = Thing_new (Spectrogram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Spectrogram not created.");
	}
}

void Spectrogram_paintInside (const constSpectrogram me, const Graphics g,
	/* mutable autowindow */ double tmin, /* mutable autowindow */ double tmax,
	/* mutable autowindow */ double fmin, /* mutable autowindow */ double fmax,
	/* mutable autoscaling */ double maximum,
	const bool autoscaling,
	const double dynamic,
	const double preemphasis_dbPerOctave,
	const double dynamicCompression
) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	SampledXY_unidirectionalAutowindowY (me, & fmin, & fmax);
	integer itmin, itmax, ifmin, ifmax;
	const integer nt = Matrix_getWindowSamplesX (me, tmin - 0.49999 * my dx, tmax + 0.49999 * my dx, & itmin, & itmax);
	const integer nf = Matrix_getWindowSamplesY (me, fmin - 0.49999 * my dy, fmax + 0.49999 * my dy, & ifmin, & ifmax);
	if (nt == 0 || nf == 0)
		return;
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	autoVEC dynamicFactors = zero_VEC (nt);
	autoMAT part = part_MAT (my z.get(), ifmin, ifmax, itmin, itmax);
	const integer ifshift = ifmin - 1;
	/*
		Pre-emphasis in place; also compute maximum after pre-emphasis.
	*/
	for (integer ifreq = 1; ifreq <= nf; ifreq ++) {
		const double frequency = Matrix_rowToY (me, ifreq + ifshift);
		/*
			The preemphasis term for a frequency of 1000 Hz is always 0 dB;
			the preemphasis term (in dB) for a frequency f is then
				preemphasis_dbPerOctave * log2 (f / 1000 Hz)
			To speed this up, we convert to natural logarithms, according to
				log2 (x) = ln (x) / ln (2)
			Before taking the logarithm, we add a tiny number, namely 1e-308,
			so that for a frequency of 0 Hz the preemphasis term is typically 6.0 * log2 (1e-308) = -6139 dB,
			or 0 dB in case the preemphasis is 0 dB/octave.
			For purposes of preemphasis, negative frequencies (which don't normally occur) are treated as 0 Hz.
		*/
		const double preemphasisTerm_db = (preemphasis_dbPerOctave / NUMln2) * log (Melder_clippedLeft (0.0, frequency) / 1000.0 + 1e-308);
		for (integer itime = 1; itime <= nt; itime ++) {
			const double psd_pascal2PerHz = part [ifreq] [itime];
			/*
				The power spectral density in dB is
					psd_db = 10.0 * log10 (psd_pascal2PerHz / referencePsd_pascal2)
				To speed this up, we convert to natural logarithms, according to
					log10 (x) = ln (x) / ln (10)
				Before taking the logarithm, we add a tiny number, namely 1e-308,
				so that in silence the PSD (before adding preemphasis) is -3080 dB;
				this becomes visible if you `Paint` the Spectrogram under the following settings:
				- `autoscaling` off
				- `maximum` 0.0 dB/Hz
				- `pre-emphasis` 0 dB/oct
				- `dynamic compression` 0
				Then when the `dynamic range` passes 3080 dB, you will start to see some light grey.
			*/
			constexpr double oneByReferencePsd_pascal2 = 1.0 / 4.0e-10;
			const double psd_db = (10.0/NUMln10) * log (oneByReferencePsd_pascal2 * psd_pascal2PerHz + 1e-308) + preemphasisTerm_db;
			if (psd_db > dynamicFactors [itime])
				dynamicFactors [itime] = psd_db;   // local maximum
			part [ifreq] [itime] = psd_db;
		}
	}
	/*
		Compute global maximum.
	*/
	if (autoscaling) {
		maximum = 0.0;
		for (integer itime = 1; itime <= nt; itime ++)
			if (dynamicFactors [itime] > maximum)
				maximum = dynamicFactors [itime];
	}
	/*
		Dynamic compression in place.
	*/
	for (integer itime = 1; itime <= nt; itime ++) {
		dynamicFactors [itime] = dynamicCompression * (maximum - dynamicFactors [itime]);
		for (integer ifreq = 1; ifreq <= nf; ifreq ++)
			part [ifreq] [itime] += dynamicFactors [itime];
	}
	Graphics_image (g, part.all(),
		Matrix_columnToX (me, itmin - 0.5),
		Matrix_columnToX (me, itmax + 0.5),
		Matrix_rowToY (me, ifmin - 0.5),
		Matrix_rowToY (me, ifmax + 0.5),
		maximum - dynamic, maximum
	);
}

void Spectrogram_paint (const constSpectrogram me, const Graphics g,
	const double tmin, const double tmax, const double fmin, const double fmax,
	const double maximum, const bool autoscaling,
	const double dynamic, const double preemphasis, const double dynamicCompression,
	const bool garnish)
{
	Graphics_setInner (g);
	Spectrogram_paintInside (me, g, tmin, tmax, fmin, fmax, maximum, autoscaling, dynamic, preemphasis, dynamicCompression);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Frequency (Hz)");
	}
}

autoSpectrogram Matrix_to_Spectrogram (constMatrix me) {
	try {
		autoSpectrogram thee = Spectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all()  <<=  my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Spectrogram.");
	}
}

autoMatrix Spectrogram_to_Matrix (constSpectrogram me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all()  <<=  my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

/* End of Spectrogram.cpp */ 
