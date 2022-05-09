/* IntensityTierArea_prefs.h
 *
 * Copyright (C) 2012,2014-2016,2020-2022 Paul Boersma
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

Prefs_begin (IntensityTierArea)

	InstancePrefs_overrideDouble (IntensityTierArea, dataFreeMinimum, 1, U"50.0")   // dB
	InstancePrefs_overrideDouble (IntensityTierArea, dataFreeMaximum, 1, U"100.0")   // dB

Prefs_end (IntensityTierArea)

/* End of file IntensityTierArea_prefs.h */
