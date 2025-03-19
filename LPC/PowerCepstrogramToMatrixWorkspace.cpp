/* PowerCepstrogramToMatrixWorkspace.cpp
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

#include "PowerCepstrogramToMatrixWorkspace.h"


#include "oo_DESTROY.h"
#include "PowerCepstrogramToMatrixWorkspace_def.h"
#include "oo_COPY.h"
#include "PowerCepstrogramToMatrixWorkspace_def.h"
#include "oo_EQUAL.h"
#include "PowerCepstrogramToMatrixWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "PowerCepstrogramToMatrixWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "PowerCepstrogramToMatrixWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "PowerCepstrogramToMatrixWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "PowerCepstrogramToMatrixWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "PowerCepstrogramToMatrixWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "PowerCepstrogramToMatrixWorkspace_def.h"

Thing_implement (PowerCepstrogramToMatrixWorkspace, SampledToSampledWorkspace, 0);

void structPowerCepstrogramToMatrixWorkspace :: allocateOutputFrames (void) {
}

void structPowerCepstrogramToMatrixWorkspace :: saveOutputFrame (void) {
	/* time, cpp, slope, intercept, peakdB, peakQuefrency */
	Matrix out = reinterpret_cast<Matrix> (output);
	out -> z [1] [currentFrame] = Sampled_indexToX (out, currentFrame);
	out -> z [2] [currentFrame] = powerCepstrumWs -> cpp;
	out -> z [3] [currentFrame] = powerCepstrumWs -> slope;
	out -> z [4] [currentFrame] = powerCepstrumWs -> intercept;
	out -> z [5] [currentFrame] = powerCepstrumWs -> peakdB;
	out -> z [6] [currentFrame] = peakQuefrency;
}

void structPowerCepstrogramToMatrixWorkspace :: getInputFrame () {
	constPowerCepstrogram me = reinterpret_cast<constPowerCepstrogram> (input);
	our powerCepstrum -> z.row (1)  <<=  my z.column (currentFrame);
	PowerCepstrumWorkspace_getNewData (our powerCepstrumWs.get(), our powerCepstrum.get());
}

bool structPowerCepstrogramToMatrixWorkspace :: inputFrameToOutputFrame () {
	constPowerCepstrogram me = reinterpret_cast<constPowerCepstrogram> (input);
	PowerCepstrum_getPeakProminence_inplace (powerCepstrum.get(), our powerCepstrumWs.get());
	return true;
}

void PowerCepstrogramToMatrixWorkspace_init (PowerCepstrogramToMatrixWorkspace me, constPowerCepstrogram thee, mutableMatrix him) {
	SampledToSampledWorkspace_init (me, thee, him);
	const integer numberOfPoints = thy ny;
	my powerCepstrum = PowerCepstrum_create (thy ymax, thy ny);
	//my powerCepstrumWs has to wait until we know more
}

autoPowerCepstrogramToMatrixWorkspace PowerCepstrogramToMatrixWorkspace_create (constPowerCepstrogram pc, mutableMatrix m) {
	try {
		autoPowerCepstrogramToMatrixWorkspace me = Thing_new (PowerCepstrogramToMatrixWorkspace);
		PowerCepstrogramToMatrixWorkspace_init (me.get(), pc, m);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create PowerCepstrogramToMatrixWorkspace.");
	}
}

/* End of file PowerCepstrogramToMatrixWorkspace.cpp */
