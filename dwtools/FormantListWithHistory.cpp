/* FormantListWithHistory.cpp
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

#include "FormantListWithHistory.h"
#include "LPC_and_Formant.h"
#include "Sound_to_Formant.h"
#include "Sound_and_LPC.h"
#include "Sound.h"
#include "Sound_and_LPC_robust.h"

Thing_implement (FormantListWithHistory, Function, 0);

#include "oo_DESTROY.h"
#include "FormantListWithHistory_def.h"
#include "oo_COPY.h"
#include "FormantListWithHistory_def.h"
#include "oo_EQUAL.h"
#include "FormantListWithHistory_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FormantListWithHistory_def.h"
#include "oo_WRITE_TEXT.h"
#include "FormantListWithHistory_def.h"
#include "oo_READ_TEXT.h"
#include "FormantListWithHistory_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantListWithHistory_def.h"
#include "oo_READ_BINARY.h"
#include "FormantListWithHistory_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantListWithHistory_def.h"

void structFormantListWithHistory :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", our xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", our xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", our xmax - our xmin, U" seconds");
	MelderInfo_writeLine (U"Formants:");
	MelderInfo_writeLine (U"   Number of Formants: ", our numberOfElements);

}


autoFormantListWithHistory FormantListWithHistory_create (double fromTime, double toTime) {
	autoFormantListWithHistory me = Thing_new (FormantListWithHistory);
	Function_init (me.get(), fromTime, toTime);
	return me;
}

autoFormantListWithHistory Sound_to_FormantListWithHistory_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumNumberOfFormants, double windowLength, double preemphasisFrequency, double minimumCeiling, double maximumCeiling, integer numberOfCeilings, double tol1, double tol2, double huberNumberOfStdDev, double tol, integer maximumNumberOfIterations) {
	try {
		double nyquistFrequency = 0.5 / my dx;		
		Melder_require (minimumCeiling < maximumCeiling,
			U"The minimum ceiling must be smaller than the maximum ceiling.");
		Melder_require (maximumCeiling <= nyquistFrequency,
			U"The maximum ceiling should not be larger than ", nyquistFrequency, U" Hz.");
		Melder_require (numberOfCeilings > 1,
			U"There should be more than one ceiling.");
		autoFormantListWithHistory thee = FormantListWithHistory_create (my xmin, my xmax);
		thy formantAnalysisHistory. lpcType = lpcType;
		thy formantAnalysisHistory. timeStep = timeStep;
		thy formantAnalysisHistory. maximumNumberOfFormants = maximumNumberOfFormants;
		thy formantAnalysisHistory. windowLength = windowLength;
		thy formantAnalysisHistory. preemphasisFrequency = preemphasisFrequency;
		thy formantAnalysisHistory. tol1 = tol1;
		thy formantAnalysisHistory. tol2 = tol2;
		thy formantAnalysisHistory. huberNumberOfStdDev = huberNumberOfStdDev;
		thy formantAnalysisHistory. tol = tol;
		thy formantAnalysisHistory. maximumNumberOfIterations = maximumNumberOfIterations;		
		thy numberOfElements = numberOfCeilings;
		
		const double frequencyStep = (maximumCeiling - minimumCeiling) / (numberOfCeilings - 1);
		const double formantSafetyMargin = 50.0;
		const integer predictionOrder = Melder_iround (2.0 * maximumNumberOfFormants);
		for (integer ic  = 1; ic <= numberOfCeilings; ic ++) {
			const double ceiling = minimumCeiling + (ic - 1) * frequencyStep;
			double *next = thy ceilings.append();
			*next = ceiling;
			autoSound resampled = Sound_resample (me, ceiling * 2, 50);
			autoLPC lpc;
			if (lpcType == kLPC_Analysis::BURG)
				lpc = Sound_to_LPC_burg (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::AUTOCORRELATION)
				lpc = Sound_to_LPC_auto (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::COVARIANCE)
				lpc = Sound_to_LPC_covar (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::MARPLE)
				lpc = Sound_to_LPC_marple (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency, tol1, tol2);
			else if (lpcType == kLPC_Analysis::ROBUST) {
				autoLPC lpc_in = Sound_to_LPC_auto (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
				lpc = LPC_Sound_to_LPC_robust (lpc_in.get(), resampled.get(), windowLength, preemphasisFrequency, huberNumberOfStdDev, maximumNumberOfIterations, tol, true);
			}
			autoFormant formant = LPC_to_Formant (lpc.get(), formantSafetyMargin);
			thy formants. addItem_move (formant.move());
			thy sounds. addItem_move (resampled.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": FormantListWithHistory not created.");
	}
}

/* End of file FormantList.cpp */
