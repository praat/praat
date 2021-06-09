#ifndef _OptimalCeilingTierArea_h_
#define _OptimalCeilingTierArea_h_
/* OptimalCeilingTierArea.h
 *
 * Copyright (C) 2015 David Weenink, 2017,2020,2021 Paul Boersma
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

#include "RealTierArea.h"
#include "OptimalCeilingTier.h"

Thing_define (OptimalCeilingTierArea, RealTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" Hz"; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }

	#include "OptimalCeilingTierArea_prefs.h"
};

/* End of file OptimalCeilingTierArea.h */
#endif
