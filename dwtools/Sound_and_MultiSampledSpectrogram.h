#ifndef _Sound_and_MultiSampledSpectrogram_h_
#define _Sound_and_MultiSampledSpectrogram_h_
/* Sound_and_MultiSampledSpectrogram.h
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

#include "AnalyticSound.h"
#include "ConstantQSpectrograms.h"
#include "Sound.h"

autoSound MultiSampledSpectrogram_to_Sound (MultiSampledSpectrogram me);

autoConstantQLog2FSpectrogram Sound_to_ConstantQLog2FSpectrogram (Sound me, double f1, double fmax, 
	integer numberOfBinsPerOctave, double frequencyResolutionInBins, double timeOversamplingFactor, 
	kSound_windowShape filterShape);

autoGaborSpectrogram Sound_to_GaborSpectrogram (Sound me, double fmax, double filterBandwidth,
	double frequencyStep, double timeOversamplingFactor, kSound_windowShape filterShape);

autoSound ConstantQLog2FSpectrogram_to_Sound (ConstantQLog2FSpectrogram me);

autoSound ConstantQLog2FSpectrogram_to_Sound_frequencyBin (ConstantQLog2FSpectrogram me, integer frequencyBinNumber);

autoAnalyticSound ConstantQLog2FSpectrogram_to_AnalyticSound_frequencyBin (ConstantQLog2FSpectrogram me, integer frequencyBinNumber);

#endif /* Sound_and_MultiSampledSpectrogram_h_ */
