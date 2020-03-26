#ifndef _DataModeler_enums_h_
#define _DataModeler_enums_h_

/* DataModeler_enums.h
 *
 * Copyright (C) 2020 David Weenink
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

enums_begin (kDataModeler_AnalysisScale, 0)
	enums_add (kMDS_AnalysisScale, 0, Absolute, U"Absolute")
	enums_add (kMDS_AnalysisScale, 1, Ratio, U"Ratio")
	enums_add (kMDS_AnalysisScale, 2, Interval, U"Interval")
	enums_add (kMDS_AnalysisScale, 3, Spline, U"Spline")
	enums_add (kMDS_AnalysisScale, 4, Ordinal, U"Ordinal")
	enums_add (kMDS_AnalysisScale, 5, Nominal, U"Nominal")
enums_end (kMDS_AnalysisScale, 5, Nominal)

e

#endif /* _DataModeler_enums_h_ */
