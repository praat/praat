#ifndef _Spectrum_and_AnalyticSound_h_
#define _Spectrum_and_AnalyticSound_h_
/* Spectrum_and_AnalyticSound.h
 *
 * Copyright (C) 2022 David Weenink
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

#include "AnalyticSound.h"
#include "Spectrum.h"


void Spectrum_into_AnalyticSound (Spectrum me, AnalyticSound thee);

autoAnalyticSound Spectrum_to_AnalyticSound (Spectrum me);


#endif /* _Spectrum_and_AnalyticSound_h_ */
