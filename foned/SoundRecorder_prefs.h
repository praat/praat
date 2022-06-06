/* SoundRecorder_prefs.h
 *
 * Copyright (C) 2013,2015,2016,2022 Paul Boersma
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

Prefs_begin (SoundRecorder)

	InstancePrefs_addEnum   (SoundRecorder, meter_which,                    1, kSoundRecorder_meter, INTENSITY)
	InstancePrefs_addDouble (SoundRecorder, meter_intensity_minimum,  1, U"50.0")   // dB
	InstancePrefs_addDouble (SoundRecorder, meter_intensity_maximum,  1, U"94.0")   // dB
	InstancePrefs_addDouble (SoundRecorder, meter_centreOfGravity_minimum,  1,  U"500.0")   // Hz
	InstancePrefs_addDouble (SoundRecorder, meter_centreOfGravity_maximum,  1, U"8000.0")   // Hz

Prefs_end (SoundRecorder)

/* End of file SoundRecorder_prefs.h */
