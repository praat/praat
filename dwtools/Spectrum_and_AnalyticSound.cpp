/* Spectrum_and_AnalyticSound.cpp
 *
 * Copyright (C) 2021-2022 David Weenink
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

#include "Spectrum_and_AnalyticSound.h"
#include "Sound_and_Spectrum.h"
#include "Spectrum_extensions.h"

void Spectrum_into_AnalyticSound (Spectrum me, AnalyticSound thee) {
	try {
		autoSound him = Spectrum_to_Sound (me);
		const integer nx = std::min (his nx, thy nx);
		thy z.row (1).part (1, nx)  <<=  his z.row (1).part (1, nx);
		Spectrum_shiftPhaseBy90Degrees (me);
		him = Spectrum_to_Sound (me);
		thy z.row (2).part (1, nx)  <<=  his z.row (1).part (1, nx);
		Spectrum_unshiftPhaseBy90Degrees (me); // restore original
	} catch (MelderError) {
		Melder_throw (me, U": cannot ", thee);
	}
}

autoAnalyticSound Spectrum_to_AnalyticSound (Spectrum me) {
	try {
		autoSound him = Spectrum_to_Sound (me);
		autoAnalyticSound thee = AnalyticSound_create (his xmin, his xmax, his nx, his dx, his x1);
		Spectrum_into_AnalyticSound (me, thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create AnalyticSound.");
	}
}

/* End of file Spectrum_and_AnalyticSound.h */
