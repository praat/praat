/* ERPArea_prefs.h
 *
 * Copyright (C) 2022 Paul Boersma
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

Prefs_begin (ERPArea)

	InstancePrefs_overrideEnum    (ERPArea, scalingStrategy, 1, kSoundArea_scalingStrategy, DEFAULT)
	InstancePrefs_overrideDouble  (ERPArea, scaling_height,  1, U"20e-6")
	InstancePrefs_overrideDouble  (ERPArea, scaling_minimum, 1, U"-10e-6")
	InstancePrefs_overrideDouble  (ERPArea, scaling_maximum, 1, U"10e-6")

Prefs_end (ERPArea)

/* End of file ERPArea_prefs.h */
