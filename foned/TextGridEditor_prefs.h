/* TextGridEditor_prefs.h
 *
 * Copyright (C) 2013,2015,2016,2019,2022 Paul Boersma
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

Prefs_begin (TextGridEditor)

	ClassPrefs_overrideInt        (TextGridEditor, shellWidth,               1, U"800")
	ClassPrefs_overrideInt        (TextGridEditor, shellHeight,              1, U"600")
	InstancePrefs_overrideBool    (TextGridEditor, showSelectionViewer,      1, true)

Prefs_end (TextGridEditor)

/* End of file TextGridEditor_prefs.h */
