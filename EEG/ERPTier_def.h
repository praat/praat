/* ERPTier_def.h
 *
 * Copyright (C) 2011 Paul Boersma
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

	oo_OBJECT (Sound, 2, d_erp)

oo_END_CLASS (ERPPoint)
#undef ooSTRUCT


#define ooSTRUCT ERPTier
oo_DEFINE_CLASS (ERPTier, Function)

	oo_COLLECTION (SortedSetOfDouble, d_events, ERPPoint, 0)

	oo_LONG (d_numberOfChannels)
	oo_STRING_VECTOR (d_channelNames, d_numberOfChannels)

	#if oo_DECLARING
		// functions:
		public:
			void f_init (double tmin, double tmax);
			long f_getChannelNumber (const wchar_t *channelName);
			void f_checkEventNumber (long eventNumber) {
				if (eventNumber < 1)
					Melder_throw ("The specified event number is ", eventNumber, " but should have been positive.");
				if (eventNumber > d_events -> size)
					Melder_throw ("The specified event number (", eventNumber, ") exceeds the number of events (", d_events -> size, ").");
			}
			ERPPoint f_peekEvent (long i) { return static_cast <ERPPoint> (d_events -> item [i]); }
			double f_getMean (long pointNumber, long channelNumber, double tmin, double tmax);
			double f_getMean (long pointNumber, const wchar_t *channelName, double tmin, double tmax);
			void f_subtractBaseline (double tmin, double tmax);
			void f_rejectArtefacts (double threshold);
			ERP f_extractERP (long pointNumber);
			ERP f_toERP_mean ();
			ERPTier f_extractEventsWhereColumn_number (Table table, long columnNumber, int which_Melder_NUMBER, double criterion);
			ERPTier f_extractEventsWhereColumn_string (Table table, long columnNumber, int which_Melder_STRING, const wchar_t *criterion);
		// overridden methods:
		protected:
			virtual int v_domainQuantity () { return MelderQuantity_TIME_SECONDS; }
			virtual void v_shiftX (double xfrom, double xto);
			virtual void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto);
	#endif

oo_END_CLASS (ERPTier)
#undef ooSTRUCT


/* End of file ERPTier_def.h */
