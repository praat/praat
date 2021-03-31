/* MultiSampledSpectrogram_def.h
 * 
 * Copyright (C) 2021 David Weenink
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

#define ooSTRUCT FunctionXSampledY
oo_DEFINE_CLASS (FunctionXSampledY, Function)

	oo_DOUBLE (ymin)
	oo_DOUBLE (ymax)
	oo_INTEGER (ny)
	oo_DOUBLE (dy)
	oo_DOUBLE (y1)

	#if oo_READING
		if (ymin > ymax) {
			Melder_throw (U"ymax should be at least as great as ymin.");
		}
		if (ny < 1) {
			Melder_throw (U"ny should be at least 1.");
		}
		if (dy <= 0.0) {
			Melder_throw (U"dy should be positive.");
		}
	#endif

	#if oo_DECLARING
		bool v_hasGetYmin ()
			override { return true; }
		double v_getYmin ()
			override { return ymin; }
		bool v_hasGetYmax ()
			override { return true; }
		double v_getYmax ()
			override { return ymax; }
		bool v_hasGetNy ()
			override { return true; }
		double v_getNy ()
			override { return ny; }
		bool v_hasGetDy ()
			override { return true; }
		double v_getDy ()
			override { return dy; }
		bool v_hasGetY ()
			override { return true; }
		double v_getY (integer iy)
			override { return y1 + (iy - 1) * dy; }
	#endif

oo_END_CLASS (FunctionXSampledY)
#undef ooSTRUCT

#define ooSTRUCT FrequencyBin
oo_DEFINE_CLASS (FrequencyBin, Sampled)

	oo_MAT (z, 2, nx) // re & im 

oo_END_CLASS (FrequencyBin)
#undef ooSTRUCT

#define ooSTRUCT  MultiSampledSpectrogram
oo_DEFINE_CLASS (MultiSampledSpectrogram, FunctionXSampledY)

	oo_COLLECTION_OF (OrderedOf, frequencyBins, FrequencyBin, 0) // invariant frequencyBins.size == my ny
	
	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (MultiSampledSpectrogram)
#undef ooSTRUCT

/* End of file MultiSampledSpectrogram_def.h */
