/* EEGWindow_prefs.h
 *
 * Copyright (C) 2013,2015,2017,2022 Paul Boersma
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

Prefs_begin (EEGWindow)

	InstancePrefs_overrideBool    (EEGWindow, showSelectionViewer,            1, false)
	ClassPrefs_addDouble     (EEGWindow, picture_bottom,           1, U"0.0")   // BUG: should override in area
	ClassPrefs_addDouble     (EEGWindow, picture_top,              1, U"0.0 (= auto)")   // BUG: should override in area
	InstancePrefs_overrideBool    (EEGWindow, spectrogram_show,               1, false);
	InstancePrefs_overrideDouble  (EEGWindow, spectrogram_viewFrom,           1, U"0.0")   // Hz
	InstancePrefs_overrideDouble  (EEGWindow, spectrogram_viewTo,             1, U"60.0")   // Hz
	InstancePrefs_overrideDouble  (EEGWindow, spectrogram_windowLength,       1, U"0.5")   // seconds
	InstancePrefs_overrideDouble  (EEGWindow, spectrogram_dynamicRange,       1, U"40.0")   // dB
	InstancePrefs_overrideInteger (EEGWindow, spectrogram_timeSteps,          1, U"1000")
	InstancePrefs_overrideInteger (EEGWindow, spectrogram_frequencySteps,     1, U"250")
	InstancePrefs_overrideEnum    (EEGWindow, spectrogram_method,             1, kSound_to_Spectrogram_method, DEFAULT)
	InstancePrefs_overrideEnum    (EEGWindow, spectrogram_windowShape,        1, kSound_to_Spectrogram_windowShape, DEFAULT)
	InstancePrefs_overrideBool    (EEGWindow, spectrogram_autoscaling,        1, true)
	InstancePrefs_overrideDouble  (EEGWindow, spectrogram_maximum,            1, U"100.0")   // dB/Hz
	InstancePrefs_overrideDouble  (EEGWindow, spectrogram_preemphasis,        1, U"0.0")   // dB/octave
	InstancePrefs_overrideDouble  (EEGWindow, spectrogram_dynamicCompression, 1, U"0.0")
	InstancePrefs_overrideBool    (EEGWindow, spectrogram_picture_garnish,    1, true)

Prefs_end (EEGWindow)

/* End of file EEGWindow_prefs.h */
