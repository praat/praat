/* Spectrum_and_AnalyticSound.cpp
 *
 * Copyright (C) 2021-2022 David Weenink
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

#include "Spectrum_and_AnalyticSound.h"
#include "Sound_and_Spectrum.h"
#include "Spectrum_extensions.h"

autoAnalyticSound Spectrum_to_AnalyticSound (Spectrum me) {
	try {
		autoSound him = Spectrum_to_Sound (me);
		autoAnalyticSound thee = AnalyticSound_create (his xmin, his xmax, his nx, his dx, his x1);
		thy z.row (1)  <<=  his z.row (1);
		Spectrum_shiftPhaseBy90Degrees (me);
		him = Spectrum_to_Sound (me);
		thy z.row (2)  <<=  his z.row (1);
		Spectrum_unshiftPhaseBy90Degrees (me); // restore phase
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create AnalyticSound.");
	}
}

/*
	

*/
autoAnalyticSound Spectrum_to_AnalyticSound_demodulateBand (Spectrum me, integer startSample, integer endSample, double approximateOverSampling, const VEC & window) {
	try {
		Melder_clipLeft (1_integer, & startSample);
		Melder_clipRight (& endSample, my nx);
		Melder_require (startSample < endSample,
			U"The start spectral sample should lie before the end spectral sample.");
		const integer extraSpectralSample = ( startSample > 1 ? 1 : 0 ); // first spectral value cannot be imaginary
		const integer numberOfSamplesFromSpectrum = endSample - startSample + 1;
		Melder_require (window.size <= numberOfSamplesFromSpectrum,
			U"The window size should not exceed the number of selected spectral values.");
		const integer numberOfValuesInBand = extraSpectralSample + ( approximateOverSampling <= 1.0 ? numberOfSamplesFromSpectrum :
			 Melder_iroundUp (approximateOverSampling * numberOfSamplesFromSpectrum) );
		const integer numberOfSamplesFFT = Melder_iroundUpToPowerOfTwo (numberOfValuesInBand);
		const integer numberOfFrequencies = numberOfSamplesFFT + 1;
		const double demod_fmax = my dx * numberOfSamplesFFT;
		autoSpectrum demodSpectrum = Spectrum_create (demod_fmax, numberOfFrequencies);
		demodSpectrum -> z.part (1, 2, 1 + extraSpectralSample, numberOfSamplesFromSpectrum + extraSpectralSample)  
			<<=  my z.part (1, 2, startSample, endSample);
		if (window.size > 0) {
			const integer startWindowing = ( startSample == 1 ? numberOfSamplesFromSpectrum - window.size + 1 : 2 );
			const integer endWindowing = startWindowing + window.size - 1;
			demodSpectrum -> z.part (1, 2, startWindowing, endWindowing)  *=  window;
		}
		return Spectrum_to_AnalyticSound (demodSpectrum.get());
	} catch (MelderError) {
		Melder_throw (me, U": could not create AnalyticSound.");
	}
	
}
/* End of file Spectrum_and_AnalyticSound.h */
