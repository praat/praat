/* SoundRecorder_prefs.h
 *
 * Copyright (C) 2013 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

prefs_begin (SoundRecorder)
	// new:
		prefs_add_enum_with_data   (SoundRecorder, meter_which, 1, kSoundRecorder_meter, INTENSITY)
		prefs_add_double_with_data (SoundRecorder, meter_intensity_minimum,  1, L"50.0")   // dB
		prefs_add_double_with_data (SoundRecorder, meter_intensity_maximum,  1, L"94.0")   // dB
		prefs_add_double_with_data (SoundRecorder, meter_centreOfGravity_minimum,  1,  L"500.0")   // Hz
		prefs_add_double_with_data (SoundRecorder, meter_centreOfGravity_maximum,  1, L"8000.0")   // Hz
prefs_end (SoundRecorder)

/* End of file SoundRecorder_prefs.h */
