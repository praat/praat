/* PointArea_prefs.h
 *
 * Copyright (C) 2023 Paul Boersma
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

Prefs_begin (PointArea)

	InstancePrefs_addDouble (PointArea, periodFloor,            1, U"1e-4")   // seconds
	InstancePrefs_addDouble (PointArea, periodCeiling,          1, U"0.02")   // seconds
	InstancePrefs_addDouble (PointArea, maximumPeriodFactor,    1, U"1.3")
	InstancePrefs_addDouble (PointArea, maximumAmplitudeFactor, 1, U"1.6")

Prefs_end (PointArea)

/* End of file PointArea_prefs.h */
