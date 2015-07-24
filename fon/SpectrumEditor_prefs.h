/* SpectrumEditor_prefs.h
 *
 * Copyright (C) 2013,2015 Paul Boersma
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

prefs_begin (SpectrumEditor)

	prefs_override_int         (SpectrumEditor, shellWidth,      1, U"700")
	prefs_override_int         (SpectrumEditor, shellHeight,     1, U"440")
	prefs_override_double      (SpectrumEditor, arrowScrollStep, 1, U"100.0")

	prefs_add_double_with_data (SpectrumEditor, bandSmoothing,   1, U"100.0")
	prefs_add_double_with_data (SpectrumEditor, dynamicRange,    1, U"60.0")

prefs_end (SpectrumEditor)

/* End of file SpectrumEditor_prefs.h */
