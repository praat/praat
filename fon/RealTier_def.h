/* RealTier_def.h
 *
 * Copyright (C) 1992-2012,2014,2015 Paul Boersma
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#define ooSTRUCT RealPoint
oo_DEFINE_CLASS (RealPoint, AnyPoint)

	oo_DOUBLE (value)

oo_END_CLASS (RealPoint)
#undef ooSTRUCT


#define ooSTRUCT RealTier
oo_DEFINE_CLASS (RealTier, Function)

	oo_COLLECTION_OF (SortedSetOfDoubleOf, points, RealPoint, 0)   // a kind of AnyTier though

	#if oo_DECLARING
		AnyTier_METHODS

		void v_info ()
			override;
		bool v_hasGetNx ()
			override { return true; }
		double v_getNx ()
			override { return points.size; }
		bool v_hasGetX ()
			override { return true; }
		double v_getX (long ix)
			override { return points.at [ix] -> number; }
		bool v_hasGetNcol ()
			override { return true; }
		double v_getNcol ()
			override { return points.size; }
		bool v_hasGetVector ()
			override { return true; }
		double v_getVector (long irow, long icol)
			override;
		bool v_hasGetFunction1 ()
			override { return true; }
		double v_getFunction1 (long irow, double x)
			override;
		const char32 * v_getUnitText (long /* ilevel */, int /* unit */, unsigned long /* flags */)
			override { return U"Time (s)"; }
	#endif

oo_END_CLASS (RealTier)
#undef ooSTRUCT


/* End of file RealTier_def.h */
