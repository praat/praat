/* DurationTierArea_prefs.h
 *
 * Copyright (C) 2020-2022 Paul Boersma
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

Prefs_begin (DurationTierArea)

	InstancePrefs_overrideDouble (DurationTierArea, dataFreeMinimum, 1, U"0.25")
	InstancePrefs_overrideDouble (DurationTierArea, dataFreeMaximum, 1, U"3.0")

Prefs_end (DurationTierArea)

/* End of file DurationTierArea_prefs.h */
