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
	ClassPrefs_addDouble     (ERPWindow, sound_picture_bottom,           1, U"0.0")   // BUG: should override in area
	ClassPrefs_addDouble     (ERPWindow, sound_picture_top,              1, U"0.0 (= auto)")   // BUG: should override in area

	InstancePrefs_addEnum         (ERPWindow, scalp_colourScale,              1, kGraphics_colourScale, BLUE_TO_RED)

Prefs_end (ERPWindow)

/* End of file ERPWindow_prefs.h */
