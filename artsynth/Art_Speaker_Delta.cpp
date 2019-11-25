/* Art_Speaker_Delta.cpp
 *
 * Copyright (C) 1992-2005,2009,2011,2016-2019 Paul Boersma
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

#include "Art_Speaker_Delta.h"
#include "Art_Speaker.h"

void Art_Speaker_intoDelta (Art art, Speaker speaker, Delta delta)
{
	double f = speaker -> relativeSize * 1e-3;
	double xe [30], ye [30], xi [30], yi [30], xmm [30], ymm [30];

	/* Lungs. */

	for (integer itube = 7; itube <= 18; itube ++)
		delta -> tubes [itube]. Dyeq = 120 * f * (1 + art -> art [(int) kArt_muscle::LUNGS]);

	/* Glottis. */

	{
		Delta_Tube t = & delta -> tubes [36];
		t -> Dyeq = f * (5 - 10 * art -> art [(int) kArt_muscle::INTERARYTENOID]
		      + 3 * art -> art [(int) kArt_muscle::POSTERIOR_CRICOARYTENOID]
		      - 3 * art -> art [(int) kArt_muscle::LATERAL_CRICOARYTENOID]);   // 4.38
		t -> k1 = speaker -> lowerCord.k1 * (1 + art -> art [(int) kArt_muscle::CRICOTHYROID]);
		t -> k3 = t -> k1 * (20 / t -> Dz) * (20 / t -> Dz);
	}
	if (speaker -> cord.numberOfMasses >= 2) {
		Delta_Tube t = & delta -> tubes [37];
		t -> Dyeq = delta -> tubes [36]. Dyeq;
		t -> k1 = speaker -> upperCord.k1 * (1 + art -> art [(int) kArt_muscle::CRICOTHYROID]);
		t -> k3 = t -> k1 * (20 / t -> Dz) * (20 / t -> Dz);
	}
	if (speaker -> cord.numberOfMasses >= 10) {
		delta -> tubes [84]. Dyeq = 0.75 * 1 * f + 0.25 * delta -> tubes [36]. Dyeq;
		delta -> tubes [85]. Dyeq = 0.50 * 1 * f + 0.50 * delta -> tubes [36]. Dyeq;
		delta -> tubes [86]. Dyeq = 0.25 * 1 * f + 0.75 * delta -> tubes [36]. Dyeq;
		delta -> tubes [84]. k1 = 0.75 * 160 + 0.25 * delta -> tubes [36]. k1;
		delta -> tubes [85]. k1 = 0.50 * 160 + 0.50 * delta -> tubes [36]. k1;
		delta -> tubes [86]. k1 = 0.25 * 160 + 0.75 * delta -> tubes [36]. k1;
		for (integer itube = 84; itube <= 86; itube ++)
			delta -> tubes [itube]. k3 = delta -> tubes [itube]. k1 *
				(20 / delta -> tubes [itube]. Dz) * (20 / delta -> tubes [itube]. Dz);
	}

	/* Vocal tract. */

	bool closed [40];
	Art_Speaker_meshVocalTract (art, speaker, xi, yi, xe, ye, xmm, ymm, closed);
	for (integer itube = 38; itube <= 64; itube ++) {
		Delta_Tube t = & delta -> tubes [itube];
		integer i = itube - 37;
		double dx = xmm [i] - xmm [i + 1];
		double dy = ymm [i] - ymm [i + 1];
		t -> Dxeq = sqrt (dx * dx + dy * dy);
		dx = xe [i] - xi [i];
		dy = ye [i] - yi [i];
		t -> Dyeq = sqrt (dx * dx + dy * dy);
		if (closed [i])
			t -> Dyeq = - t -> Dyeq;
	}
	delta -> tubes [65]. Dxeq = delta -> tubes [51]. Dxeq = delta -> tubes [50]. Dxeq;
	/* Voor [r]:  thy tubes [60]. Brel = 0.1; thy tubes [60]. k1 = 3; */

	/* Nasopharyngeal port. */

	delta -> tubes [65]. Dyeq = f * (18 - 25 * art -> art [(int) kArt_muscle::LEVATOR_PALATINI]);   // 4.40

	for (integer itube = 1; itube <= delta -> numberOfTubes; itube ++) {
		Delta_Tube t = & delta -> tubes [itube];
		t -> s1 = 5e6 * t -> Dxeq * t -> Dzeq;
		t -> s3 = t -> s1 / (0.9e-3 * 0.9e-3);
	}
}

/* End of file Art_Speaker_Delta.cpp */
