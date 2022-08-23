/* DurationTierArea.cpp
 *
 * Copyright (C) 1992-2012,2014-2016,2018,2020,2022 Paul Boersma
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

#include "DurationTierArea.h"

Thing_implement (DurationTierArea, RealTierArea, 0);

#include "Prefs_define.h"
#include "DurationTierArea_prefs.h"
#include "Prefs_install.h"
#include "DurationTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "DurationTierArea_prefs.h"

void structDurationTierArea :: v9_repairPreferences () {
	if (our instancePref_dataFreeMinimum() > 1.0)
		our setInstancePref_dataFreeMinimum (Melder_atof (our default_dataFreeMinimum()));
	if (our instancePref_dataFreeMaximum() < 1.0)
		our setInstancePref_dataFreeMaximum (Melder_atof (our default_dataFreeMaximum()));
	Melder_assert (our instancePref_dataFreeMinimum() < our instancePref_dataFreeMaximum());
	DurationTierArea_Parent :: v9_repairPreferences ();
}

/* End of file DurationTierArea.cpp */
