/* Ltas_to_SpectrumTier.cpp
 *
 * Copyright (C) 2007-2011,2015 Paul Boersma
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

#include "Ltas_to_SpectrumTier.h"

autoSpectrumTier Ltas_to_SpectrumTier_peaks (Ltas me) {
	try {
		autoSpectrumTier thee = Vector_to_RealTier_peaks (me, 1, classSpectrumTier).static_cast_move<structSpectrumTier>();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": peaks not analyzed as SpectrumTier.");
	}
}

/* End of file Ltas_to_SpectrumTier.cpp */
