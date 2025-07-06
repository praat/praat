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
#include "SampledIntoSampledStatus.h"
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

Thing_implement (SoundFrameIntoPowerCepstrogramFrame, SoundFrameIntoMatrixFrame, 0);

bool structSoundFrameIntoPowerCepstrogramFrame :: inputFrameToOutputFrame () {
	/*
		Step 1: spectrum of the sound frame
	*/
	fftData.part (1, soundFrameSize)  <<=  soundFrame;
	if (numberOfFourierSamples > soundFrameSize)
		fftData.part (soundFrameSize + 1, numberOfFourierSamples)  <<=  0.0;
	NUMfft_forward (fourierTable.get(), fftData.get());
	for (integer i = 1 ; i <= numberOfFourierSamples; i ++)
		fftData [i] *= sound -> dx;

	/*
		step 2: log of the spectrum power values log (re * re + im * im)
	*/
	fftData [1] = log (fftData [1] * fftData [1] + 1e-300);
	for (integer i = 1; i < numberOfFourierSamples / 2; i ++) {
		const double re = fftData [2 * i], im = fftData [2 * i + 1];
		fftData [2 * i] = log (re * re + im * im + 1e-300);
		fftData [2 * i + 1] = 0.0;
	}
	fftData [numberOfFourierSamples] = log (fftData [numberOfFourierSamples] * fftData [numberOfFourierSamples] + 1e-300);
	/*
		Step 3: inverse fft of the log spectrum
	*/
	NUMfft_backward (fourierTable.get(), fftData.get());
	const double df = 1.0 / (sound -> dx * numberOfFourierSamples);
	for (integer i = 1; i <= powercepstrum -> nx; i ++) {
		const double val = fftData [i] * df;
		powercepstrum -> z [1] [i] = val * val;
	}
	return true;
}

void structSoundFrameIntoPowerCepstrogramFrame :: saveOutputFrame () {
	const integer localFrame = currentFrame - startFrame + 1;
	localOutput.column (localFrame)  <<=  powercepstrum -> z.row (1);	
}

autoSoundFrameIntoPowerCepstrogramFrame SoundFrameIntoPowerCepstrogramFrame_create (constSound input, mutablePowerCepstrogram output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape)
{
	try {
		autoSoundFrameIntoPowerCepstrogramFrame me = Thing_new (SoundFrameIntoPowerCepstrogramFrame);
		SoundFrameIntoMatrixFrame_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		my powercepstrogram = output;
		const integer numberOfFourierSamples = Melder_iroundUpToPowerOfTwo (my frameAsSound -> nx);
		my numberOfFourierSamples = numberOfFourierSamples;
		my fourierTable = NUMFourierTable_create (numberOfFourierSamples);
		my fftData = raw_VEC (numberOfFourierSamples);
		my powercepstrum = PowerCepstrum_create (output -> ymax, output -> ny);
		my subtractFrameMean = true;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundFrameIntoPowerCepstrogramFrame.");
	}
}

/* End of file SoundFrameIntoPowerCepstrogramFrame.cpp */
