/* SoundFrameIntoPowerCepstrogramFrame.cpp
 *
 * Copyright (C) 2024-2025 David Weenink
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

#include "SoundFrameIntoPowerCepstrogramFrame.h"
#include "NUMFourier.h"

#include "oo_DESTROY.h"
#include "SoundFrameIntoPowerCepstrogramFrame_def.h"
#include "oo_COPY.h"
#include "SoundFrameIntoPowerCepstrogramFrame_def.h"
#include "oo_EQUAL.h"
#include "SoundFrameIntoPowerCepstrogramFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundFrameIntoPowerCepstrogramFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundFrameIntoPowerCepstrogramFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundFrameIntoPowerCepstrogramFrame_def.h"
#include "oo_READ_TEXT.h"
#include "SoundFrameIntoPowerCepstrogramFrame_def.h"
#include "oo_READ_BINARY.h"
#include "SoundFrameIntoPowerCepstrogramFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundFrameIntoPowerCepstrogramFrame_def.h"

Thing_implement (SoundFrameIntoPowerCepstrogramFrame, SoundFrameIntoSampledFrame, 0);

void structSoundFrameIntoPowerCepstrogramFrame :: saveOutputFrame () {
	powercepstrogram ->z.column (currentFrame)  <<=  powercepstrum -> z.row(1);
}

bool structSoundFrameIntoPowerCepstrogramFrame :: inputFrameToOutputFrame () {
	fftData.part (1, soundFrameSize)  <<=  soundFrame;
	if (numberOfFourierSamples > soundFrameSize)
		fftData.part (soundFrameSize + 1, numberOfFourierSamples)  <<=  0.0;
	NUMfft_forward (fourierTable.get(), fftData.get());
	/*
		log of the spectrum
	*/
	fftData [1] = log (fftData [1] * fftData [1] + 1e-300);
	for (integer i = 1; i < numberOfFourierSamples / 2; i ++) {
		const double re = fftData [2 * i], im = fftData [2 * i + 1];
		fftData [i + 1] = log (re * re + im * im + 1e-300);
		fftData [i + 2] = 0.0;
	}
	fftData [numberOfFourierSamples] = log (fftData [numberOfFourierSamples] * fftData [numberOfFourierSamples] + 1e-300);
	
	NUMfft_backward (fourierTable.get(), fftData.get());
	const double scaling = 1.0 / numberOfFourierSamples;
	for (integer i = 1; i <= powercepstrum -> nx; i ++) {
		const double val = fftData [i];
		powercepstrum -> z [1] [i] = val * val * scaling;
	}
	return true;
}

autoSoundFrameIntoPowerCepstrogramFrame SoundFrameIntoPowerCepstrogramFrame_create (constSound input, mutablePowerCepstrogram output, double effectiveAnalysisWidth, 
	kSound_windowShape windowShape) {
	try {
		autoSoundFrameIntoPowerCepstrogramFrame me = Thing_new (SoundFrameIntoPowerCepstrogramFrame);
		SoundFrameIntoSampledFrame_init (me.get(), input, effectiveAnalysisWidth, windowShape);
		my powercepstrogram = output;
		const integer numberOfFourierSamples = Melder_iroundUpToPowerOfTwo (my frameAsSound -> nx);
		my numberOfFourierSamples = numberOfFourierSamples;
		my fourierTable = NUMFourierTable_create (numberOfFourierSamples);
		my fftData = raw_VEC (numberOfFourierSamples);
		my powercepstrum = PowerCepstrum_create (output -> ymax, output -> ny);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundFrameIntoPowerCepstrogramFrame.");
	}
}

/* End of file SoundFrameIntoPowerCepstrogramFrame.cpp */
