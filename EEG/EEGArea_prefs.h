/* EEGArea_prefs.h
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

Prefs_begin (EEGArea)

	InstancePrefs_overrideEnum    (EEGArea, scalingStrategy, 1, kSoundArea_scalingStrategy, BY_WINDOW_AND_CHANNEL)
	InstancePrefs_overrideDouble  (EEGArea, scaling_height,  1, U"20e-6")
	InstancePrefs_overrideDouble  (EEGArea, scaling_minimum, 1, U"-10e-6")
	InstancePrefs_overrideDouble  (EEGArea, scaling_maximum, 1, U"10e-6")

Prefs_end (EEGArea)

/* End of file EEGArea_prefs.h */
