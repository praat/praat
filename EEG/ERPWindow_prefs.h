/* ERPWindow_prefs.h
 *
 * Copyright (C) 2013-2015,2017,2022 Paul Boersma
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

Prefs_begin (ERPWindow)

	InstancePrefs_overrideBool    (ERPWindow, showSelectionViewer,            1, true)
	ClassPrefs_overrideDouble     (ERPWindow, sound_picture_bottom,           1, U"0.0")
	ClassPrefs_overrideDouble     (ERPWindow, sound_picture_top,              1, U"0.0 (= auto)")
	InstancePrefs_overrideBool    (ERPWindow, spectrogram_show,               1, false);
	InstancePrefs_overrideDouble  (ERPWindow, spectrogram_viewFrom,           1, U"0.0")   // Hz
	InstancePrefs_overrideDouble  (ERPWindow, spectrogram_viewTo,             1, U"60.0")   // Hz
	InstancePrefs_overrideDouble  (ERPWindow, spectrogram_windowLength,       1, U"0.5")   // seconds
	InstancePrefs_overrideDouble  (ERPWindow, spectrogram_dynamicRange,       1, U"40.0")   // dB
	InstancePrefs_overrideInteger (ERPWindow, spectrogram_timeSteps,          1, U"1000")
	InstancePrefs_overrideInteger (ERPWindow, spectrogram_frequencySteps,     1, U"250")
	InstancePrefs_overrideEnum    (ERPWindow, spectrogram_method,             1, kSound_to_Spectrogram_method, DEFAULT)
	InstancePrefs_overrideEnum    (ERPWindow, spectrogram_windowShape,        1, kSound_to_Spectrogram_windowShape, DEFAULT)
	InstancePrefs_overrideBool    (ERPWindow, spectrogram_autoscaling,        1, true)
	InstancePrefs_overrideDouble  (ERPWindow, spectrogram_maximum,            1, U"100.0")   // dB/Hz
	InstancePrefs_overrideDouble  (ERPWindow, spectrogram_preemphasis,        1, U"0.0")   // dB/octave
	InstancePrefs_overrideDouble  (ERPWindow, spectrogram_dynamicCompression, 1, U"0.0")
	InstancePrefs_overrideBool    (ERPWindow, spectrogram_picture_garnish,    1, true)

	InstancePrefs_addEnum         (ERPWindow, scalp_colourScale,              1, kGraphics_colourScale, BLUE_TO_RED)

Prefs_end (ERPWindow)

/* End of file ERPWindow_prefs.h */
