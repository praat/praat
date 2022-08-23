/* SpectrumEditor_prefs.h
 *
 * Copyright (C) 2013,2015,2022 Paul Boersma
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

Prefs_begin (SpectrumEditor)

	ClassPrefs_overrideInt        (SpectrumEditor, shellWidth,      1, U"700")
	ClassPrefs_overrideInt        (SpectrumEditor, shellHeight,     1, U"440")
	InstancePrefs_overrideDouble  (SpectrumEditor, arrowScrollStep, 1, U"100.0")

Prefs_end (SpectrumEditor)

/* End of file SpectrumEditor_prefs.h */
