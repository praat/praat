/* TextGrid_def.h
 *
 * Copyright (C) 1992-2011,2014,2015 Paul Boersma
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#define ooSTRUCT TextPoint
oo_DEFINE_CLASS (TextPoint, AnyPoint)

	oo_STRING (mark)

	/* 'after' was a temporary attribute (19970211-19970307). */
	#if oo_READING_TEXT
		if (Thing_version == 1) texgetw2 (a_text);
	#elif oo_READING_BINARY
		if (Thing_version == 1) bingetw2 (f);
	#endif

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
oo_DEFINE_CLASS (TextTier, Function)

	oo_COLLECTION (SortedSetOfDouble, points, TextPoint, 0)

	#if oo_DECLARING
		long numberOfPoints () // accessor
			{ return our points -> size; }
		TextPoint point (long i) // accessor
			{ return static_cast <TextPoint> (our points -> item [i]); }

		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
		void v_shiftX (double xfrom, double xto)
			override;
		void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto)
			override;
	#endif

oo_END_CLASS (TextTier)
#undef ooSTRUCT


#define ooSTRUCT IntervalTier
oo_DEFINE_CLASS (IntervalTier, Function)

	oo_COLLECTION (SortedSetOfDouble, intervals, TextInterval, 0)

	#if oo_DECLARING
		long numberOfIntervals () // accessor
			{ return our intervals -> size; }
		TextInterval interval (long i) // accessor
			{ return static_cast <TextInterval> (our intervals -> item [i]); }
		TextInterval operator[] (long i) { return static_cast <TextInterval> (our intervals -> item [i]); }
		//template <class T> T& operator[] (long i) { return (T) (our intervals -> item [i]); }
		TextInterval* intervalss () { return (TextInterval *) (our intervals -> item); }

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

	oo_OBJECT (Ordered, 0, tiers)   // TextTier and IntervalTier objects

	#if oo_DECLARING
		long numberOfTiers () // accessor
			{ return our tiers -> size; }
		Function tier (long i) // accessor
			{ return static_cast <Function> (our tiers -> item [i]); }

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
	#endif

oo_END_CLASS (TextGrid)
#undef ooSTRUCT


/* End of file TextGrid_def.h */
