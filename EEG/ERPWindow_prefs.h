/* ERPWindow_prefs.h
 *
 * Copyright (C) 2013 Paul Boersma
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

prefs_begin (ERPWindow)
	// overridden:
		prefs_add_bool   (ERPWindow, showSelectionViewer,   1, true)
		prefs_add_enum   (ERPWindow, sound_scalingStrategy, 1, kTimeSoundEditor_scalingStrategy, DEFAULT)
		prefs_add_double (ERPWindow, sound_scaling_height,  1, L"20e-6")
		prefs_add_double (ERPWindow, sound_scaling_minimum, 1, L"-10e-6")
		prefs_add_double (ERPWindow, sound_scaling_maximum, 1, L"10e-6")
prefs_end (ERPWindow)

/* End of file ERPWindow_prefs.h */
