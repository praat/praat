/* VocalTractTier.cpp
 *
 * Copyright (C) 2012-2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "NUM2.h"
#include "RealTier.h"
#include "VocalTractTier.h"

#include "oo_DESTROY.h"
#include "VocalTractTier_def.h"
#include "oo_COPY.h"
#include "VocalTractTier_def.h"
#include "oo_EQUAL.h"
#include "VocalTractTier_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "VocalTractTier_def.h"
#include "oo_WRITE_TEXT.h"
#include "VocalTractTier_def.h"
#include "oo_READ_TEXT.h"
#include "VocalTractTier_def.h"
#include "oo_WRITE_BINARY.h"
#include "VocalTractTier_def.h"
#include "oo_READ_BINARY.h"
#include "VocalTractTier_def.h"
#include "oo_DESCRIPTION.h"
#include "VocalTractTier_def.h"

Thing_implement (VocalTractPoint, AnyPoint, 0);

autoVocalTractPoint VocalTractPoint_create (VocalTract me, double time) {
	try {
		autoVocalTractPoint thee = Thing_new (VocalTractPoint);
		thy number = time;
		thy d_vocalTract = Data_copy (me);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": VocalTractPoint not created.");
	}
}

void VocalTract_drawSegments (VocalTract me, Graphics g, double maxLength, double maxArea, bool closedAtGlottis)
{
	Graphics_setInner (g);
	const double maxCrossection = sqrt (maxArea);
	Graphics_setWindow (g, 0.0, maxLength, -maxCrossection, maxCrossection);
	for (integer isection = 1; isection <= my nx; isection ++) {
		const double x1 = (isection - 1.0) * my dx, x2 = x1 + my dx;
		const double crosssection2 = sqrt (my z [1] [isection]);
		Graphics_line (g, x1, crosssection2, x2, crosssection2);
		Graphics_line (g, x1, -crosssection2, x2, -crosssection2);
		if (isection > 1) {
			double crosssection1 = sqrt (my z [1] [isection - 1]);
			Graphics_line (g, x1, crosssection1, x1, crosssection2);
			Graphics_line (g, x1, -crosssection1, x1, -crosssection2);
		} else if (isection == 1 and closedAtGlottis) {
			Graphics_line (g, x1, crosssection2, x1, -crosssection2);
		}
	}
	Graphics_unsetInner (g);
}

Thing_implement (VocalTractTier, Function, 0);

autoVocalTractTier VocalTractTier_create (double fromTime, double toTime) {
	try {
		autoVocalTractTier me = Thing_new (VocalTractTier);
		Function_init (me.get(), fromTime, toTime);
		return me;
	} catch (MelderError) {
		Melder_throw (U": VocalTractTier not created.");
	}
}

autoVocalTractTier VocalTract_to_VocalTractTier (VocalTract me, double startTime, double endTime, double time) {
	try {
		autoVocalTractTier thee = VocalTractTier_create (startTime, endTime);
		VocalTractTier_addVocalTract (thee.get(), time, me);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to VocalTractTier");
	}
}

void VocalTractTier_addVocalTract (VocalTractTier me, double time, VocalTract vocaltract) {
	try {
		autoVocalTractPoint thee = VocalTractPoint_create (vocaltract, time);
		my d_vocalTracts. addItem_move (thee.move());
	} catch (MelderError) {
		Melder_throw (me, U": no VocalTract added.");
	}
}

autoVocalTract VocalTractTier_to_VocalTract (VocalTractTier me, double time) {
	try {
		Melder_assert (my d_vocalTracts.size > 0);
		const VocalTractPoint vtp = my d_vocalTracts.at [1];
		const integer numberOfSections = vtp -> d_vocalTract -> nx;
		autoVocalTract thee = VocalTract_create (numberOfSections, vtp -> d_vocalTract -> dx);
		for (integer isection = 1; isection <= numberOfSections; isection ++) {
			autoRealTier section = RealTier_create (my xmin, my xmax);
			for (integer i = 1; i <= my d_vocalTracts.size; i ++) {
				const VocalTractPoint vtpi = my d_vocalTracts.at [i];
				const double areai = vtpi -> d_vocalTract -> z [1] [isection];
				RealTier_addPoint (section.get(), vtpi -> number, areai);
			}
			thy z[1] [isection] = RealTier_getValueAtTime (section.get(), time);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no VocalTract created.");
	}
}

autoLPC VocalTractTier_to_LPC (VocalTractTier me, double timeStep) {
	try {
		Melder_require (my d_vocalTracts.size > 0.0,
			U"The VocalTractTier should not be empty.");
		const integer numberOfFrames = Melder_ifloor ((my xmax - my xmin) / timeStep);
		const VocalTractPoint vtp = my d_vocalTracts.at [1];
		integer numberOfSections = vtp -> d_vocalTract -> nx;
		const double samplingPeriod = 1.0 / (1000.0 * numberOfSections);
		
		autoMAT area = zero_MAT (numberOfFrames, numberOfSections);
		autoVEC areavec = raw_VEC (numberOfSections);
		autoLPC thee = LPC_create (my xmin, my xmax, numberOfFrames, timeStep, timeStep / 2.0, numberOfSections, samplingPeriod);
		// interpolate each section
		for (integer isection = 1; isection <= numberOfSections; isection ++) {
			autoRealTier sectioni = RealTier_create (my xmin, my xmax);
			for (integer i = 1; i <= my d_vocalTracts.size; i ++) {
				const VocalTractPoint vtpi = my d_vocalTracts.at [i];
				const double areai = vtpi -> d_vocalTract -> z [1] [isection];
				RealTier_addPoint (sectioni.get(), vtpi -> number, areai);
			}
			for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
				const double time = thy x1 + (iframe - 1) * thy dx;
				area [iframe] [isection] = RealTier_getValueAtTime (sectioni.get(), time);
			}
		}
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const LPC_Frame frame = & thy d_frames [iframe];
			LPC_Frame_init (frame, numberOfSections);
			for (integer i = 1; i <= numberOfSections; i ++)
				areavec [i] = area [iframe] [numberOfSections + 1 - i]; // reverse
			VEClpc_from_area (frame -> a.get(), areavec.get());
			frame -> gain = 1e-6;   // something
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U": not converted to LPC.");
	}
}


/* End of file VocalTractTier.cpp */
