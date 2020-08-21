/* PitchTierArea_prefs.h
 *
 * Copyright (C) 2013,2015-2017,2020 Paul Boersma
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

prefs_begin (PitchTierArea)

	prefs_add_enum_with_data (PitchTierArea, pitch_units, 1, kPitchTierArea_units, DEFAULT)

prefs_end (PitchTierArea)

/* End of file PitchTierArea_prefs.h */
