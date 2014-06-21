/* ERPTier_def.h
 *
 * Copyright (C) 2011,2014 Paul Boersma
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


#define ooSTRUCT ERPPoint
oo_DEFINE_CLASS (ERPPoint, AnyPoint)

	oo_OBJECT (Sound, 2, erp)

oo_END_CLASS (ERPPoint)
#undef ooSTRUCT


#define ooSTRUCT ERPTier
oo_DEFINE_CLASS (ERPTier, Function)

	oo_COLLECTION (SortedSetOfDouble, events, ERPPoint, 0)

	oo_LONG (numberOfChannels)
	oo_STRING_VECTOR (channelNames, numberOfChannels)

	#if oo_DECLARING
		// functions:
		public:
			ERPPoint event (long i) { return static_cast <ERPPoint> (our events -> item [i]); }
		// overridden methods:
		protected:
			virtual int v_domainQuantity () { return MelderQuantity_TIME_SECONDS; }
			virtual void v_shiftX (double xfrom, double xto);
			virtual void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto);
	#endif

oo_END_CLASS (ERPTier)
#undef ooSTRUCT


/* End of file ERPTier_def.h */
