/* VocalTractTier.cpp
 *
 * Copyright (C) 2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

/***** VocalTractPoint *****/

void VocalTract_drawSegments (VocalTract me, Graphics g, double maxLength, double maxArea, bool closedAtGlottis)
{
	Graphics_setInner (g);
	double maxCrossection = sqrt (maxArea);
	Graphics_setWindow (g, 0, maxLength, -maxCrossection, maxCrossection);
	for (long isection = 1; isection <= my nx; isection++) {
		double x1 = (isection - 1) * my dx, x2 = x1 + my dx;
		double crosssection2 = sqrt (my z[1][isection]);
		Graphics_line (g, x1, crosssection2, x2, crosssection2);
		Graphics_line (g, x1, -crosssection2, x2, -crosssection2);
		if (isection > 1) {
			double crosssection1 = sqrt (my z[1][isection - 1]);
			Graphics_line (g, x1, crosssection1, x1, crosssection2);
			Graphics_line (g, x1, -crosssection1, x1, -crosssection2);
		} else if (isection == 1 and closedAtGlottis) {
			Graphics_line (g, x1, crosssection2, x1, -crosssection2);
		}
	}
	Graphics_unsetInner (g);
}

Thing_implement (VocalTractPoint, AnyPoint, 0);

VocalTractPoint VocalTract_to_VocalTractPoint (VocalTract me, double time) {
	try {
		autoVocalTractPoint thee = Thing_new (VocalTractPoint);
		thy number = time;
		thy d_vocalTract = (VocalTract) Data_copy (me);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to VocalTractPoint.");
	}
}

/***** VocalTractTier *****/

Thing_implement (VocalTractTier, Function, 0);

VocalTractTier VocalTractTier_create (double fromTime, double toTime) {
	try {
		autoVocalTractTier me = Thing_new (VocalTractTier);
		Function_init (me.peek(), fromTime, toTime);
		my d_vocalTracts = SortedSetOfDouble_create ();
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (": VocalTractTier not created.");
	}
}

VocalTractTier VocalTract_to_VocalTractTier (VocalTract me, double startTime, double endTime, double time) {
	try {
		autoVocalTractTier thee = VocalTractTier_create (startTime, endTime);
		VocalTractTier_addVocalTract (thee.peek(), time, me);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to VocalTractTier");
	}
}

void VocalTractTier_addVocalTract (VocalTractTier me, double time, VocalTract vocaltract) {
	try {
		autoVocalTractPoint thee =  VocalTract_to_VocalTractPoint (vocaltract, time);
		if (my d_vocalTracts -> size > 1) {
			VocalTractPoint vtp = (VocalTractPoint) my d_vocalTracts -> item[1];
			long numberOfSections = vtp -> d_vocalTract -> nx;
			if (numberOfSections != vocaltract -> nx) {
				forget (vocaltract);
				Melder_throw ("The number of sections must be equal to ", Melder_integer (numberOfSections), ".");
			}
		}
		Collection_addItem (my d_vocalTracts, thee.transfer());
	} catch (MelderError) {
		Melder_throw (me, ": no VocalTract added.");
	}
}

VocalTract VocalTractTier_to_VocalTract (VocalTractTier me, double time) {
	try {
		VocalTractPoint vtp = (VocalTractPoint) my d_vocalTracts -> item[1];
		long numberOfSections = vtp -> d_vocalTract -> nx;
		autoVocalTract thee = VocalTract_create (numberOfSections, vtp -> d_vocalTract -> dx);
		for (long isection = 1; isection <= numberOfSections; isection++) {
			autoRealTier section = RealTier_create (my xmin, my xmax);
			for (long i = 1; i <= my d_vocalTracts -> size; i++) {
				VocalTractPoint vtpi = (VocalTractPoint) my d_vocalTracts -> item[i];
				double areai = vtpi -> d_vocalTract -> z[1][isection];
				RealTier_addPoint (section.peek(), vtpi -> number, areai);
			}
			thy z[1][isection] = RealTier_getValueAtTime (section.peek(), time);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no VocalTract created.");
	}
}

LPC VocalTractTier_to_LPC (VocalTractTier me, double timeStep) {
	try {
		if (my d_vocalTracts -> size == 0) {
			Melder_throw ("Empty VocalTractTier");
		}
		long numberOfFrames = floor((my xmax - my xmin) / timeStep);
		VocalTractPoint vtp = (VocalTractPoint) my d_vocalTracts -> item[1];
		long numberOfSections = vtp -> d_vocalTract -> nx;
		double samplingPeriod = 1.0 / (1000.0 * numberOfSections);
		autoNUMmatrix<double> area (1, numberOfFrames, 1, numberOfSections + 1);
		autoNUMvector<double> areavec (1, numberOfSections + 1);
		autoLPC thee = LPC_create (my xmin, my xmax, numberOfFrames, timeStep, timeStep / 2, numberOfSections, samplingPeriod);
		// interpolate each section
		for (long isection = 1; isection <= numberOfSections; isection++) {
			autoRealTier sectioni = RealTier_create (my xmin, my xmax);
			for (long i = 1; i <= my d_vocalTracts -> size; i++) {
				VocalTractPoint vtpi = (VocalTractPoint) my d_vocalTracts -> item[i];
				double areai = vtpi -> d_vocalTract -> z[1][isection];
				RealTier_addPoint (sectioni.peek(), vtpi -> number, areai);
			}
			for (long iframe = 1; iframe <= numberOfFrames; iframe++) {
				double time = thy x1 + (iframe - 1) * thy dx;
				area[iframe][isection] = RealTier_getValueAtTime (sectioni.peek(), time);
				area[iframe][numberOfSections + 1] = 0.0001; // normalisation is area[n+1] = 0.0001
			}
		}
		for (long iframe = 1; iframe <= numberOfFrames; iframe++) {
			LPC_Frame frame = &thy d_frames[iframe];
			LPC_Frame_init (frame, numberOfSections);
			for (long i = 1; i <= numberOfSections + 1; i++) {
				areavec[i] = area[iframe][numberOfSections + 1 - i];
			}
			NUMlpc_area_to_lpc (areavec.peek(), numberOfSections + 1, frame -> a);
			frame -> gain = 1e-6; // something
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (": not converted to LPC.");
	}
}


/* End of file VocalTractTier.cpp */
