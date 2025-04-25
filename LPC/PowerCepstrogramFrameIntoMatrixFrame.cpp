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

void structPowerCepstrogramFrameIntoMatrixFrame :: allocateOutputFrames (void) {
	// nothing to do: input and output are Matrix and already allocated at creation
}

void structPowerCepstrogramFrameIntoMatrixFrame :: getInputFrame () {
	powerCepstrum -> z.row (1)  <<=  input -> z.column (currentFrame);
	powerCepstrumWs -> newData (powerCepstrum.get()); // powercepstrum is in dB's now
}

bool structPowerCepstrogramFrameIntoMatrixFrame :: inputFrameToOutputFrame () {
	if (getSlopeAndIntercept) {
		powerCepstrumWs -> getSlopeAndIntercept ();
		powerCepstrumWs -> slopeKnown = true;
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
	if (subtractTrend) {
		Melder_assert (Thing_isa (output, classPowerCepstrogram));
		output -> z.column (currentFrame)  <<=  powerCepstrum -> z.row (1);
	} else {
		output -> z [1] [currentFrame] = Sampled_indexToX (output, currentFrame);
		if (getSlopeAndIntercept) {
			output -> z [2] [currentFrame] = powerCepstrumWs -> slope;
			output -> z [3] [currentFrame] = powerCepstrumWs -> intercept;
		}
		if (getPeakAndPosition) {
			output -> z [4] [currentFrame] = powerCepstrumWs -> peakdB;
			output -> z [5] [currentFrame] = powerCepstrumWs -> peakQuefrency;
			powerCepstrumWs -> getCPP ();
			output -> z [6] [currentFrame] = powerCepstrumWs -> cpp;
		}
	}
}

void PowerCepstrogramFrameIntoMatrixFrame_init (PowerCepstrogramFrameIntoMatrixFrame me, constPowerCepstrogram thee, mutableMatrix him) {
	my input = thee;
	my output = him;
	const integer numberOfPoints = thy ny;
	my powerCepstrum = PowerCepstrum_create (thy ymax, thy ny);
	my powerCepstrumWs = Thing_new (PowerCepstrumWorkspace);
}

autoPowerCepstrogramFrameIntoMatrixFrame PowerCepstrogramFrameIntoMatrixFrame_create (constPowerCepstrogram pc, mutableMatrix m) {
	try {
		autoPowerCepstrogramFrameIntoMatrixFrame me = Thing_new (PowerCepstrogramFrameIntoMatrixFrame);
		PowerCepstrogramFrameIntoMatrixFrame_init (me.get(), pc, m);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create PowerCepstrogramFrameIntoMatrixFrame.");
	}
}

/* End of file PowerCepstrogramFrameIntoMatrixFrame.cpp */
