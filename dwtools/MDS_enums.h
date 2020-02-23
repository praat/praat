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
	enums_add (kMDS_AnalysisScale, 0, Absolute, U"Absolute")
	enums_add (kMDS_AnalysisScale, 1, Ratio, U"Ratio")
	enums_add (kMDS_AnalysisScale, 2, Interval, U"Interval")
	enums_add (kMDS_AnalysisScale, 3, Spline, U"Spline")
	enums_add (kMDS_AnalysisScale, 4, Ordinal, U"Ordinal")
	enums_add (kMDS_AnalysisScale, 5, Nominal, U"Nominal")
enums_end (kMDS_AnalysisScale, 5, Nominal)

enums_begin (kMDS_TiesHandling, 0)
	enums_add (kMDS_TiesHandling, 0, None, U"None")
	enums_add (kMDS_TiesHandling, 1, PrimaryApproach, U"Primary approach")
	enums_add (kMDS_TiesHandling, 2, SecondaryApproach, U"Secondary approach")
enums_end (kMDS_TiesHandling, 2, PrimaryApproach)

enums_begin (kMDS_stressMeasure, 1)
	enums_add (kMDS_stressMeasure, 1, Normalized, U"Normalized")
	enums_add (kMDS_stressMeasure, 2, Kruskal_1, U"Kruskal's stress-1")
	enums_add (kMDS_stressMeasure, 3, Kruskal_2, U"Kruskal's stress-2")
	enums_add (kMDS_stressMeasure, 4, Raw, U"Raw")
enums_end (kMDS_stressMeasure, 4, Normalized)

enums_begin (kMDS_KruskalStress, 2)
	enums_add (kMDS_KruskalStress, 2, Kruskal_1, U"Kruskal's stress-1")
	enums_add (kMDS_KruskalStress, 3, Kruskal_2, U"Kruskal's stress-2")
enums_end (kMDS_KruskalStress, 3, Kruskal_1)

enums_begin (kMDS_splineType, 1)
	enums_add (kMDS_splineType, 1, MSpline, U"M-spline")
	enums_add (kMDS_splineType, 2, ISpline , U"I-spline")
enums_end (kMDS_splineType, 2, MSpline)

#endif /* _MDS_enums_h_ */
