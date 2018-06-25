/* TextGrid_def.h
 *
 * Copyright (C) 1992-2011,2014,2015 Paul Boersma
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


#define ooSTRUCT TextPoint
oo_DEFINE_CLASS (TextPoint, AnyPoint)

	oo_STRING (mark)

oo_END_CLASS (TextPoint)
#undef ooSTRUCT


#define ooSTRUCT TextInterval
oo_DEFINE_CLASS (TextInterval, Function)

	oo_STRING (text)

	#if oo_DECLARING
		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
	#endif

oo_END_CLASS (TextInterval)
#undef ooSTRUCT


#define ooSTRUCT TextTier
oo_DEFINE_CLASS (TextTier, Function)   // a kind of AnyTier though

	oo_COLLECTION_OF (SortedSetOfDoubleOf, points, TextPoint, 0)

	#if oo_DECLARING
		AnyTier_METHODS

		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
	#endif

oo_END_CLASS (TextTier)
#undef ooSTRUCT


#define ooSTRUCT IntervalTier
oo_DEFINE_CLASS (IntervalTier, Function)

	oo_COLLECTION_OF (SortedSetOfDoubleOf, intervals, TextInterval, 0)

	#if oo_DECLARING
		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
		void v_shiftX (double xfrom, double xto)
			override;
		void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto)
			override;
	#endif

oo_END_CLASS (IntervalTier)
#undef ooSTRUCT


#define ooSTRUCT TextGrid
oo_DEFINE_CLASS (TextGrid, Function)

	oo_OBJECT (FunctionList, 0, tiers)   // TextTier and IntervalTier objects

	#if oo_DECLARING
		void v_info ()
			override;
		void v_repair ()
			override;
		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
		void v_shiftX (double xfrom, double xto)
			override;
		void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto)
			override;

		IntervalTier intervalTier_cast (int32 tierNumber) {
			return static_cast <IntervalTier> (our tiers -> at [tierNumber]);
		}
		TextTier textTier_cast (int32 tierNumber) {
			return static_cast <TextTier> (our tiers -> at [tierNumber]);
		}
	#endif

oo_END_CLASS (TextGrid)
#undef ooSTRUCT


/* End of file TextGrid_def.h */
