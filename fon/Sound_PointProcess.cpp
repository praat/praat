/* Sound_PointProcess.cpp
 *
 * Copyright (C) 2010-2011,2015 Paul Boersma
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

/*
 * pb 2010/12/09 created
 * pb 2011/06/08 C++
 */

#include "Sound_PointProcess.h"

autoSound Sound_PointProcess_to_SoundEnsemble_correlate (Sound me, PointProcess thee, double fromLag, double toLag) {
	try {
		if (my ny > 1)
			Melder_throw (U"Sound has to be mono.");
		long numberOfPoints = thy nt;
		double hisDuration = toLag - fromLag;
		integer numberOfSamples = Melder_iroundDown (hisDuration / my dx) + 1;
		if (numberOfSamples < 1)
			Melder_throw (U"Time window too short.");
		double midTime = 0.5 * (fromLag + toLag);
		double hisPhysicalDuration = numberOfSamples * my dx;
		double firstTime = midTime - 0.5 * hisPhysicalDuration + 0.5 * my dx;   // distribute the samples evenly over the time domain
		autoSound him = Sound_create (numberOfPoints, fromLag, toLag, numberOfSamples, my dx, firstTime);
		for (long ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
			double myTimeOfPoint = thy t [ipoint];
			double hisTimeOfPoint = 0.0;
			double mySample = 1 + (myTimeOfPoint - my x1) / my dx;
			double hisSample = 1 + (hisTimeOfPoint - his x1) / my dx;
			long sampleDifference = lround (mySample - hisSample);
			for (long isample = 1; isample <= numberOfSamples; isample ++) {
				long jsample = isample + sampleDifference;
				his z [ipoint] [isample] = jsample < 1 || jsample > my nx ? 0.0 : my z [1] [jsample];
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": Sound ensemble not created.");
	}
}

/* End of file Sound_PointProcess.cpp */
