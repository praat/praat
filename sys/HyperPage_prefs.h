/* HyperPage_prefs.h
 *
 * Copyright (C) 2013,2015 Paul Boersma
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

prefs_begin (HyperPage)

	prefs_add_enum_with_data   (HyperPage, font,     1, kGraphics_font, DEFAULT)
	prefs_add_double_with_data (HyperPage, fontSize, 1, U"12")

prefs_end (HyperPage)

/* End of file HyperPage_prefs.h */
