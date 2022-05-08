/* Editor_prefs.h
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

EditorPrefs_begin (Editor)

	EditorClassPrefs_addInt        (Editor, shellWidth,             1, U"260")
	EditorClassPrefs_addInt        (Editor, shellHeight,            1, U"260")
	EditorInstancePrefs_addBool    (Editor, picture_eraseFirst,     1, true)
	EditorClassPrefs_addEnum       (Editor, picture_writeNameAtTop, 1, kEditor_writeNameAtTop, DEFAULT)

EditorPrefs_end (Editor)

/* End of file Editor_prefs.h */
