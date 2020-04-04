#ifndef _MDS_enums_h_
#define _MDS_enums_h_

/* MDS_enums.h
 *
 * Copyright (C) 2018-2020 David Weenink
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

enums_begin (kMDS_AnalysisScale, 0)
	enums_add (kMDS_AnalysisScale, 0, ABSOLUTE_, U"Absolute")
	enums_add (kMDS_AnalysisScale, 1, RATIO, U"Ratio")
	enums_add (kMDS_AnalysisScale, 2, INTERVAL, U"Interval")
	enums_add (kMDS_AnalysisScale, 3, SPLINE, U"Spline")
	enums_add (kMDS_AnalysisScale, 4, ORDINAL, U"Ordinal")
	enums_add (kMDS_AnalysisScale, 5, NOMINAL, U"Nominal")
enums_end (kMDS_AnalysisScale, 5, NOMINAL)

enums_begin (kMDS_TiesHandling, 0)
	enums_add (kMDS_TiesHandling, 0, NONE, U"None")
	enums_add (kMDS_TiesHandling, 1, PRIMARY_APPROACH, U"Primary approach")
	enums_add (kMDS_TiesHandling, 2, SECONDARY_APPROACH, U"Secondary approach")
enums_end (kMDS_TiesHandling, 2, PRIMARY_APPROACH)

enums_begin (kMDS_stressMeasure, 1)
	enums_add (kMDS_stressMeasure, 1, NORMALIZED, U"Normalized")
	enums_add (kMDS_stressMeasure, 2, KRUSKAL_1, U"Kruskal's stress-1")
	enums_add (kMDS_stressMeasure, 3, KRUSKAL_2, U"Kruskal's stress-2")
	enums_add (kMDS_stressMeasure, 4, RAW, U"Raw")
enums_end (kMDS_stressMeasure, 4, NORMALIZED)

enums_begin (kMDS_KruskalStress, 2)
	enums_add (kMDS_KruskalStress, 2, KRUSKAL_1, U"Kruskal's stress-1")
	enums_add (kMDS_KruskalStress, 3, KRUSKAL_2, U"Kruskal's stress-2")
enums_end (kMDS_KruskalStress, 3, KRUSKAL_1)

enums_begin (kMDS_splineType, 1)
	enums_add (kMDS_splineType, 1, M_SPLINE, U"M-spline")
	enums_add (kMDS_splineType, 2, I_SPLINE , U"I-spline")
enums_end (kMDS_splineType, 2, M_SPLINE)

#endif /* _MDS_enums_h_ */
