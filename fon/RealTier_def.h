/* RealTier_def.h
 *
 * Copyright (C) 1992-2012,2014-2018,2022,2023 Paul Boersma
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


#define ooSTRUCT RealPoint
oo_DEFINE_CLASS (RealPoint, AnyPoint)

	oo_DOUBLE (value)

oo_END_CLASS (RealPoint)
#undef ooSTRUCT


/*
	RealTier inherits from Function,
	but most of the time a RealTier should also be able to pose as an AnyTier,
	because the SortedSetDoubleOf contains RealPoints, which inherit from AnyPoints.
*/
#define ooSTRUCT RealTier
oo_DEFINE_CLASS (RealTier, Function)   // syntactic inheritance

	oo_COLLECTION_OF (SortedSetOfDoubleOf, points, RealPoint, 0)   // semantically a kind of AnyTier though

	#if oo_DECLARING
		AnyTier_METHODS   // the semantic superclass has to be copied in by hand (for shift() and scale())

		void v1_info ()
			override;
		bool v_hasGetNx () const
			override { return true; }
		double v_getNx () const
			override { return points.size; }
		bool v_hasGetX () const
			override { return true; }
		double v_getX (const integer ix) const
			override { return points.at [ix] -> number; }
		bool v_hasGetNcol () const
			override { return true; }
		double v_getNcol () const
			override { return points.size; }
		bool v_hasGetVector () const
			override { return true; }
		double v_getVector (integer irow, integer icol) const
			override;
		bool v_hasGetFunction1 () const
			override { return true; }
		double v_getFunction1 (integer irow, double x) const
			override;
		conststring32 v_getUnitText (integer /* level */, int /* unit */, uint32 /* flags */) const
			override { return U"Time (s)"; }
	#endif

oo_END_CLASS (RealTier)
#undef ooSTRUCT


/* End of file RealTier_def.h */
