/* Sound_and_WarpedLPC.cpp
 *
 * Copyright (C) 2024 David Weenink
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

#include "Sound_and_WarpedLPC.h"
#include "Sound_extensions.h"
#include "Vector.h"
#include "Spectrum.h"
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include "NUM2.h"

static integer getWarpedLPCAnalysisWorkspaceSize (integer numberOfSamples, integer numberOfCoefficients, kWarpedLPC_Analysis method) {
	integer size = 0;
	if (method == kWarpedLPC_Analysis :: PLP) {
		const integer numberOfFourierSamples = Melder_iroundUpToPowerOfTwo (numberOfSamples);
		size = 
		size = 3 * numberOfCoefficients + 2;
	}
	return size;
}

static autoVEC getWarpedLPCAnalysisWorkspace (integer numberOfSamples, integer numberOfCoefficients, kWarpedLPC_Analysis method) {
	integer size = getWarpedLPCAnalysisWorkspaceSize (numberOfSamples, numberOfCoefficients, method);
	autoVEC result = raw_VEC (size);
	return result;
}

static int Sound_into_LPC_Frame_PLP (Sound me, LPC_Frame thee, VEC const& workspace) {
	Melder_assert (thy nCoefficients == thy a.size); // check invariant
	const integer numberOfCoefficients = thy nCoefficients, np1 = numberOfCoefficients + 1;

	//workspace  <<=  0.0; not necessary !
}



/* End of file Sound_and_WarpedLPC.cpp */
