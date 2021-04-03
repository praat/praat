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
			override{ return y1 + (iy - 1) * dy; }
		double v_yToIndex (double y) 
			{ return (y - y1) / dy + 1.0; }
		integer  v_yToLowIndex (double y)
			{ return Melder_ifloor ((y - y1) / dy + 1.0); }
		integer  v_yToHighIndex (double y) 
			{ return Melder_iceiling ((y - y1) / dy + 1.0); }
		integer v_yToNearestIndex (double y) 
			{ return Melder_iround ((y - y1) / dy + 1.0); }
		integer v_getWindowSamplesY (double yymin, double yymax, integer *iymin, integer *iymax) {
			const double iymin_real = 1.0 + Melder_roundUp   ((yymin - y1) / dy);
			const double iymax_real = 1.0 + Melder_roundDown ((yymax - y1) / dy);   // could be above 32-bit LONG_MAX
			*iymin = ( iymin_real < 1.0 ? 1 : (integer) iymin_real );
			*iymax = ( iymax_real > (double) ny ? ny : (integer) iymax_real );
			if (*iymin > *iymax)
				return 0;
			return *iymax - *iymin + 1;
		}

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
