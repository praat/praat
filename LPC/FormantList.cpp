/* FormantList.cpp
 *
 * Copyright (C) 2020 David Weenink
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

#include "FormantList.h"
#include "LPC_and_Formant.h"
#include "Sound_to_Formant.h"
#include "Sound_and_LPC.h"
#include "Sound.h"
#include "Sound_and_LPC_robust.h"

#include "oo_DESTROY.h"
#include "FormantList_def.h"
#include "oo_COPY.h"
#include "FormantList_def.h"
#include "oo_EQUAL.h"
#include "FormantList_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FormantList_def.h"
#include "oo_WRITE_TEXT.h"
#include "FormantList_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantList_def.h"
#include "oo_READ_TEXT.h"
#include "FormantList_def.h"
#include "oo_READ_BINARY.h"
#include "FormantList_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantList_def.h"

void structFormantList :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Formants:");
	MelderInfo_writeLine (U"   Number of Formants: ", formants . size);

}

Thing_implement (FormantList, Function, 0);

autoFormantList FormantList_create (double fromTime, double toTime, integer numberOfFormantObjects) {
	autoFormantList me = Thing_new (FormantList);
	Function_init (me.get(), fromTime, toTime);
	my identifier = autoSTRVEC (numberOfFormantObjects);
	my numberOfFormantObjects = numberOfFormantObjects;
	return me;
}

autoFormantList Sound_to_FormantList_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumFrequency, double maximumNumberOfFormants, double windowLength, double preemphasisFrequency, double minimumCeiling, double maximumCeiling, double ceilingStep, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol, integer huber_maximumNumberOfIterations) {
	try {
		const double nyquistFrequency = 0.5 / my dx;		
		Melder_require (minimumCeiling < maximumCeiling,
			U"The minimum ceiling must be smaller than the maximum ceiling.");
		Melder_require (maximumCeiling <= nyquistFrequency,
			U"The maximum ceiling should not be larger than ", nyquistFrequency, U" Hz.");		
		autoVEC ceilings = newVECfrom_to_by (minimumCeiling, maximumCeiling, ceilingStep);
		Melder_require (ceilings.size > 1,
			U"There should be more than one ceiling.");
		autoFormantList thee = FormantList_create (my xmin, my xmax, ceilings.size);
		thy defaultFormantObject = 0;

		for (integer ic = 1; ic <= ceilings.size; ic ++)
			if (Melder_iround (ceilings [ic]) == Melder_iround (maximumFrequency)) {
				thy defaultFormantObject = ic;
				break;
			}
		Melder_require (thy defaultFormantObject > 0,
			U"The 'Maximum formant' frequency (", maximumFrequency, U") should also occur as a in the list of frequencies between 'Minimum ceiling' and 'Maximum ceiling'.");
		const double formantSafetyMargin = 50.0;
		const integer predictionOrder = Melder_iround (2.0 * maximumNumberOfFormants);
		for (integer ic  = 1; ic <= ceilings.size; ic ++) {
			autoSound resampled = Sound_resample (me, 2.0 * ceilings [ic], 50);
			autoLPC lpc;
			if (lpcType == kLPC_Analysis::BURG)
				lpc = Sound_to_LPC_burg (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::AUTOCORRELATION)
				lpc = Sound_to_LPC_auto (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::COVARIANCE)
				lpc = Sound_to_LPC_covar (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::MARPLE)
				lpc = Sound_to_LPC_marple (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency, marple_tol1, marple_tol2);
			else if (lpcType == kLPC_Analysis::ROBUST) {
				autoLPC lpc_in = Sound_to_LPC_auto (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
				lpc = LPC_Sound_to_LPC_robust (lpc_in.get(), resampled.get(), windowLength, preemphasisFrequency, huber_numberOfStdDev, huber_maximumNumberOfIterations, huber_tol, true);
			}
			autoFormant formant = LPC_to_Formant (lpc.get(), formantSafetyMargin);
			thy identifier [ic] =  Melder_dup (Melder_double (ceilings [ic]));
			thy formants . addItem_move (formant.move());
		}
		Melder_assert (thy formants.size == thy numberOfFormantObjects); // maintain invariant
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": FormantList not created.");
	}
}

/* End of file FormantList.cpp */
