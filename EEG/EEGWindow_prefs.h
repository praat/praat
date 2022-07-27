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

	ClassPrefs_overrideInt     (EEGWindow, shellWidth,               1, U"800")
	ClassPrefs_overrideInt     (EEGWindow, shellHeight,              1, U"600")
	InstancePrefs_overrideBool (EEGWindow, showSelectionViewer,      1, false)
	ClassPrefs_addDouble       (EEGWindow, picture_bottom,           1, U"0.0")   // BUG: should override in area
	ClassPrefs_addDouble       (EEGWindow, picture_top,              1, U"0.0 (= auto)")   // BUG: should override in area

Prefs_end (EEGWindow)

/* End of file EEGWindow_prefs.h */
