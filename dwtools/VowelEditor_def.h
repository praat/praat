/* VowelEditor_def.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#define ooSTRUCT TrajectoryPoint
oo_DEFINE_CLASS (TrajectoryPoint, AnyPoint)

	oo_DOUBLE (f1)
	oo_DOUBLE (f2)
	oo_STRUCT (MelderColour, colour)

oo_END_CLASS(TrajectoryPoint)
#undef ooSTRUCT

#define ooSTRUCT Trajectory
oo_DEFINE_CLASS (Trajectory, Function)

	oo_COLLECTION_OF (SortedSetOfDoubleOf, points, TrajectoryPoint, 0)
	
	#if oo_DECLARING
		AnyTier_METHODS
	#endif

oo_END_CLASS(Trajectory)
#undef ooSTRUCT

/* End of file VowelEditor_def.h */
