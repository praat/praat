/* PowerCepstrogramFrameIntoMatrixFrame.cpp
 *
 * Copyright (C) 2025 David Weenink
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

#include "PowerCepstrogramFrameIntoMatrixFrame.h"

#include "oo_DESTROY.h"
#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"
#include "oo_COPY.h"
#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"
#include "oo_EQUAL.h"
#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"
#include "oo_READ_TEXT.h"
#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"
#include "oo_READ_BINARY.h"
#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"

Thing_implement (PowerCepstrogramFrameIntoMatrixFrame, SampledFrameIntoSampledFrame, 0);

void structPowerCepstrogramFrameIntoMatrixFrame :: getInputFrame () {
	powerCepstrum -> z.row (1)  <<=  powercepstrogram -> z.column (currentFrame);
	powerCepstrumWs -> newData (powerCepstrum.get()); // powercepstrum is in dB's now
}

bool structPowerCepstrogramFrameIntoMatrixFrame :: inputFrameToOutputFrame () {
	PowerCepstrogramIntoMatrixStatus myStatus = reinterpret_cast<PowerCepstrogramIntoMatrixStatus> (status);
	if (getSlopeAndIntercept) {
		powerCepstrumWs -> getSlopeAndIntercept ();
		powerCepstrumWs -> slopeKnown = true;
		myStatus -> slopes [currentFrame] = powerCepstrumWs -> slope;
		myStatus -> intercepts [currentFrame] = powerCepstrumWs -> intercept;
		myStatus -> startFrames [currentFrame] = startFrame;
		myStatus -> numberOfTries [currentFrame] = powerCepstrumWs -> slopeSelector -> numberOfTries;
	}
	if (subtractTrend) {
		Melder_assert (powerCepstrumWs -> slopeKnown);
		powerCepstrumWs -> subtractTrend ();
	}
	if (getPeakAndPosition) {
		powerCepstrumWs -> getPeakAndPosition ();
		powerCepstrumWs -> peakKnown = true;
	}
	return true;
}

void structPowerCepstrogramFrameIntoMatrixFrame :: saveOutputFrame (void) {
	/* time, slope, intercept, peakdB, peakQuefrency, cpp, */
	const integer localFrame = currentFrame - startFrame + 1;
	if (subtractTrend) {
		Melder_assert (Thing_isa (matrix, classPowerCepstrogram));
		localOutput.column (localFrame)  <<=  powerCepstrum -> z.row (1);
	} else {
		localOutput [1] [localFrame] = Sampled_indexToX (matrix, currentFrame);
		if (getSlopeAndIntercept) {
			localOutput [2] [localFrame] = powerCepstrumWs -> slope;
			localOutput [3] [localFrame] = powerCepstrumWs -> intercept;
		}
		if (getPeakAndPosition) {
			localOutput [4] [localFrame] = powerCepstrumWs -> peakdB;
			localOutput [5] [localFrame] = powerCepstrumWs -> peakQuefrency;
			powerCepstrumWs -> getCPP ();
			localOutput [6] [localFrame] = powerCepstrumWs -> cpp;
		}
	}
}

void structPowerCepstrogramFrameIntoMatrixFrame :: saveLocalOutputFrames (void) {
	Melder_assert (localOutput.ncol == maximumNumberOfFrames);
	Melder_assert (localOutput.nrow == matrix -> ny);
	for (integer irow = 1; irow <= localOutput.nrow; irow ++)
		for (integer outputcol = startFrame, icol = 1; icol <=currentNumberOfFrames; icol ++, outputcol ++)
			matrix -> z [irow] [outputcol] = localOutput [irow] [icol];
}

void PowerCepstrogramFrameIntoMatrixFrame_init (PowerCepstrogramFrameIntoMatrixFrame me, constPowerCepstrogram thee, mutableMatrix him,
	double qminFit, double qmaxFit,	kCepstrum_trendType trendLineType, kCepstrum_trendFit fitMethod) {
	SampledFrameIntoMatrixFrame_init (me, him);
	my powercepstrogram = thee;
	my powerCepstrum = PowerCepstrum_create (thy ymax, thy ny);
	my powerCepstrumWs = PowerCepstrumWorkspace_create (my powerCepstrum.get(), qminFit, qmaxFit, trendLineType, fitMethod);
}

autoPowerCepstrogramFrameIntoMatrixFrame PowerCepstrogramFrameIntoMatrixFrame_create (constPowerCepstrogram pc, mutableMatrix matrix,
	double qminFit, double qmaxFit,	kCepstrum_trendType trendLineType, kCepstrum_trendFit fitMethod)
{
	try {
		autoPowerCepstrogramFrameIntoMatrixFrame me = Thing_new (PowerCepstrogramFrameIntoMatrixFrame);
		PowerCepstrogramFrameIntoMatrixFrame_init (me.get(), pc, matrix, qminFit, qmaxFit, trendLineType, fitMethod);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create PowerCepstrogramFrameIntoMatrixFrame.");
	}
}

/* End of file PowerCepstrogramFrameIntoMatrixFrame.cpp */
