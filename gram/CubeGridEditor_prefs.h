/* CubeGridEditor_prefs.h
 *
 * Copyright (C) 2023 Paul Boersma
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

Prefs_begin (CubeGridEditor)

	InstancePrefs_overrideDouble  (CubeGridEditor, arrowScrollStep,                  1, U"1.0")
	InstancePrefs_overrideBool    (CubeGridEditor, showSelectionViewer,              1, true)

Prefs_end (CubeGridEditor)

/* End of file CubeGridEditor_prefs.h */
