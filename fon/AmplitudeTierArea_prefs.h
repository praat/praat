/* AmplitudeTierArea_prefs.h
 *
 * Copyright (C) 2012,2014-2016,2020,2021 Paul Boersma
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

prefs_begin (AmplitudeTierArea)

	prefs_override_double (AmplitudeTierArea, dataFreeMinimum, 1, U"-1.0")
	prefs_override_double (AmplitudeTierArea, dataFreeMaximum, 1, U"1.0")

prefs_end (AmplitudeTierArea)

/* End of file AmplitudeTierArea_prefs.h */
