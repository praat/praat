#ifndef _Sound_and_ConstantQLogFSpectrogram_h_
#define _Sound_and_ConstantQLogFSpectrogram_h_
/* Sound_and_ConstantQLogFSpectrogram.h
 * 
 * Copyright (C) 2021 David Weenink
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

#include "ConstantQLogFSpectrogram.h"
#include "Sound.h"

autoConstantQLogFSpectrogram Sound_to_ConstantQLogFSpectrogram (Sound me, double f1, double fmax, integer numberOfBinsPerOctave, double frequencyResolutionInBins, double timeOversamplingFactor);

autoSound ConstantQLogFSpectrogram_to_Sound (ConstantQLogFSpectrogram me);

autoSound ConstantQLogFSpectrogram_to_Sound_frequencyBin (ConstantQLogFSpectrogram me, integer frequencyBinNumber);

#endif /* Sound_and_ConstantQLogFSpectrogram_h_ */
