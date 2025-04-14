#ifndef _SoundFrameIntoLPCFrame_h_
#define _SoundFrameIntoLPCFrame_h_
/* SoundFrameIntoLPCFrame_def.h
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

#include "LPC.h"
#include "Sound.h"
#include "SVD.h"
#include "SoundFrameIntoSampledFrame.h"
#include "SampledIntoSampledStatus.h"

#include "SoundFrameIntoLPCFrame_def.h"

void SoundFrameIntoLPCFrame_init (mutableSoundFrameIntoLPCFrame me, constSound input, mutableLPC output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape
);

autoSoundFrameIntoLPCFrameAuto SoundFrameIntoLPCFrameAuto_create (constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape);

autoSoundFrameIntoLPCFrameCovar SoundFrameIntoLPCFrameCovar_create (constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape);

autoSoundFrameIntoLPCFrameBurg SoundFrameIntoLPCFrameBurg_create (constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape);

autoSoundFrameIntoLPCFrameMarple SoundFrameIntoLPCFrameMarple_create (constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape,
		double tol1, double tol2
);

autoLPCAndSoundFramesIntoLPCFrameRobust LPCAndSoundFramesIntoLPCFrameRobust_create (constLPC inputlpc, constSound inputsound, mutableLPC outputlpc,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double k_stdev, integer itermax, double tol, double location, bool wantlocation
);

autoSoundFrameIntoLPCFrameRobust SoundFrameIntoLPCFrameRobust_create (autoSoundFrameIntoLPCFrame ws1, autoLPCAndSoundFramesIntoLPCFrameRobust ws2);

#endif /*_SoundFrameIntoLPCFrame_h_ */
