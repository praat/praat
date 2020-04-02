/* LPC_and_Tube.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020612 GPL header
 djmw 20041020 struct Tube_Frame -> struct structTube_Frame; struct LPC_Frame -> struct structLPC_Frame;
 	struct Formant_Frame->struct structFormant_Frame
 djmw 20051005 Always make a VocalTract with length 0.01 m when isundef(wakita_length).
*/

#include "LPC_and_Tube.h"
#include "LPC_and_Formant.h"
#include "LPC_to_Spectrum.h"
#include "SpectrumTier.h"
#include "VocalTract_to_Spectrum.h"
#include "NUM2.h"

// IEEE: Programs fo digital signal processing section 4.3 LPTRN

void LPC_Frame_into_Tube_Frame_rc (LPC_Frame me, Tube_Frame thee) {
	Melder_assert (my nCoefficients == my a.size); // check invariant
	thy c.resize (my nCoefficients);
	thy numberOfSegments = thy c.size; // maintain invariant
	VECrc_from_lpc (thy c.get(), my a.get());
}

void LPC_Frame_into_Tube_Frame_area (LPC_Frame me, Tube_Frame thee) {
	VECarea_from_lpc (thy c.part (1, my nCoefficients), my a.part (1, my nCoefficients));
}

double VocalTract_LPC_Frame_getMatchingLength (VocalTract me, LPC_Frame thee, double glottalDamping, bool radiationDamping, bool internalDamping) {
	try {
		/*
			Match the average distance between the first two formants in the VocaTract and the LPC spectrum
		*/
		const integer numberOfFrequencies = 1000;
		const double maximumFrequency = 5000.0;
		autoSpectrum vts = VocalTract_to_Spectrum (me, numberOfFrequencies, maximumFrequency, glottalDamping, radiationDamping, internalDamping);
		const double samplingFrequency =  1000.0 * my nx;
		autoSpectrum lps = Spectrum_create (0.5 * samplingFrequency, numberOfFrequencies);
		LPC_Frame_into_Spectrum (thee, lps.get(), 0, 50);
		autoSpectrumTier vtst = Spectrum_to_SpectrumTier_peaks (vts.get());
		autoSpectrumTier lpst = Spectrum_to_SpectrumTier_peaks (lps.get());
		const double vt_f1 = vtst -> points.at [1] -> number, vt_f2 = vtst -> points.at [2] -> number;
		const double lp_f1 = lpst -> points.at [1] -> number, lp_f2 = lpst -> points.at [2] -> number;
		const double df1 = lp_f1 - vt_f1, df2 =  lp_f2 - vt_f2, df = 0.5 * (df1 + df2);
		const double dl = - df / lp_f2;
		return my dx * my nx * (1 + dl);
	} catch (MelderError) {
		Melder_throw (U"Length could not be determined from VocalTract and LPC_Frame.");
	}
}

double LPC_Frame_getVTL_wakita (LPC_Frame me, double samplingPeriod, double refLength) {
	struct structLPC_Frame lpc_struct;
	const LPC_Frame lpc = & lpc_struct;
	struct structFormant_Frame f_struct;
	const Formant_Frame f = & f_struct;
	struct structTube_Frame rc_struct, af_struct;
	const Tube_Frame rc = & rc_struct, af = & af_struct;
	try {
		const integer numberOfCoefficients = my nCoefficients;
		const double dlength = 0.001;
		double length, wakita_length = undefined;
		double varMin = 1e308;

		memset (& lpc_struct, 0, sizeof (lpc_struct));
		memset (& f_struct, 0, sizeof (f_struct));
		memset (& rc_struct, 0, sizeof (rc_struct));
		memset (& af_struct, 0, sizeof (af_struct));


		LPC_Frame_init (lpc, numberOfCoefficients);
		Tube_Frame_init (rc, numberOfCoefficients, refLength);
		Tube_Frame_init (af, numberOfCoefficients, refLength);
		/*
			Step 2
		*/
		LPC_Frame_into_Formant_Frame (me, f, samplingPeriod, 0.0);
		/*
			LPC_Frame_into_Formant_Frame performs the Formant_Frame_init !!
		*/
		Melder_require (f -> numberOfFormants > 0,
			U"Not enough formants.");
		VEC area = af -> c.get(); // TODO
		double lmin = length = 0.10;
		double plength = refLength;
		while (length <= 0.25) {
			/*
				Step 3
			*/
			const double fscale = plength / length;
			for (integer i = 1; i <= f -> numberOfFormants; i ++) {
				f -> formant [i]. frequency *= fscale;
				f -> formant [i]. bandwidth *= fscale;
			}
			/*
				20000125: Bovenstaande schaling van f1/b1 kan ook gedaan worden door
				MGfb_to_a (f, b, nf, samplingFrequency*length/refLength, a1)
				De berekening is extreem gevoelig voor de samplefrequentie: een zelfde
				stel f,b waardes geven andere lengtes afhankelijk van Fs. Ook het
				weglaten van een hogere formant heeft consekwenties.
				De refLength zou eigenlijk vast moeten liggen op
				refLength=c*aantalFormanten/Fs waarbij c=340 m/s (geluidssnelheid).
				Bij Fs=10000 zou aantalFormanten=5 zijn en refLength -> 0.17 m
			*/
			/*
				Step 4
			*/
			Formant_Frame_into_LPC_Frame (f, lpc, samplingPeriod);
			/*
				Step 5
			*/
			rc -> length = length;
			LPC_Frame_into_Tube_Frame_rc (lpc, rc);
			/*
				Step 6.1
			*/
			Tube_Frames_rc_into_area (rc, af);
			/*
				Step 6.2 Log(areas)
			*/
			double logSum = 0.0;
			for (integer i = 1; i <= af -> numberOfSegments; i ++) {
				area [i] = log (area [i]);
				logSum += area [i];
			}
			/*
				Steps 6.3 and 7
			*/
			double var = 0.0;
			for (integer i = 1; i <= af -> numberOfSegments; i ++) {
				const double delta = area [i] - logSum / af -> numberOfSegments;
				var += delta * delta;
			}
			if (var < varMin) {
				lmin = length;
				varMin = var;
			}
			plength = length;
			length += dlength;
		}

		wakita_length = lmin;
		f -> destroy ();
		lpc -> destroy ();
		rc -> destroy ();
		af -> destroy ();
		return wakita_length;
	} catch (MelderError) {
		f -> destroy ();
		lpc -> destroy ();
		rc -> destroy ();
		af -> destroy ();
		return undefined;
	}
}

int Tube_Frame_into_LPC_Frame_area (Tube_Frame me, LPC_Frame thee) {
	(void) me;
	(void) thee;
	return 0;
}

int Tube_Frame_into_LPC_Frame_rc (Tube_Frame me, LPC_Frame thee) {
	(void) me;
	(void) thee;
	return 0;
}

void VocalTract_setLength (VocalTract me, double newLength) {
	my xmax = newLength;
	my dx = newLength / my nx;
	my x1 = 0.5 * my dx;
}

autoVocalTract LPC_to_VocalTract_slice_special (LPC me, double time, double glottalDamping, bool radiationDamping, bool internalDamping) {
	try {
		integer frameNumber = Sampled_xToNearestIndex (me, time);
		Melder_clip (1_integer, & frameNumber, my nx);   // constant extrapolation
		LPC_Frame lpc = & my d_frames [frameNumber];
		autoVocalTract thee = LPC_Frame_to_VocalTract (lpc, 0.17);
		const double length = VocalTract_LPC_Frame_getMatchingLength (thee.get(), lpc, glottalDamping, radiationDamping, internalDamping);
		VocalTract_setLength (thee.get(), length);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no VocalTract created.");
	}
}

autoVocalTract LPC_Frame_to_VocalTract (LPC_Frame me, double length) {
	try {
		const integer m = my nCoefficients;
		autoVocalTract thee = VocalTract_create (m, length / m);
		VECarea_from_lpc (thy z.row (1), my a.part (1, m));
		// area [lips..glottis] (m^2) to VocalTract [glottis..lips] (m^2)

		for (integer i = 1; i <= m / 2; i ++)
			std::swap (thy z [1] [i], thy z [1] [m + 1 - i]);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No VocalTract created from LPC_Frame.");
	}
}

autoVocalTract LPC_to_VocalTract_slice (LPC me, double time, double length) {
	try {
		integer frameNumber = Sampled_xToNearestIndex (me, time);
		Melder_clip (1_integer, & frameNumber, my nx);   // constant extrapolation
		const LPC_Frame lpc = & my d_frames [frameNumber];
		autoVocalTract thee = LPC_Frame_to_VocalTract (lpc, length);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no VocalTract created.");
	}
}

/* End of file LPC_and_Tube.cpp */
