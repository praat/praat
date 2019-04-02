/* Electroglottogram_enums.h
 *
 * Copyright (C) 2019 David Weenink
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

enums_begin (kElectroglottogram_findClosedIntervalMethod, 1)
	enums_add (kElectroglottogram_findClosedIntervalMethod, 1, Peaks, U"Peaks")
	enums_add (kElectroglottogram_findClosedIntervalMethod, 2, Derivative, U"Derivative")
enums_end (kElectroglottogram_findClosedIntervalMethod, 2, Derivative)

/* End of file Electroglottogram_enums.h */
