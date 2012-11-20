/* ERP_def.h
 *
 * Copyright (C) 2011-2012 Paul Boersma
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


#define ooSTRUCT ERP
oo_DEFINE_CLASS (ERP, Sound)

	oo_STRING_VECTOR (d_channelNames, ny)

	#if oo_DECLARING
		// functions:
		public:
			long f_getChannelNumber (const wchar_t *channelName);
			void f_draw (Graphics graphics, long channelNumber, double tmin, double tmax, double vmin, double vmax, bool garnish);
			void f_draw (Graphics graphics, const wchar_t *channelName, double tmin, double tmax, double vmin, double vmax, bool garnish);
			void f_drawScalp (Graphics graphics, double tmin, double tmax, double vmin, double vmax, bool garnish);
			Table f_tabulate (bool includeSampleNumbers, bool includeTime, int timeDecimals, int voltageDecimals, int units);
			Sound f_downToSound ();
	#endif

oo_END_CLASS (ERP)
#undef ooSTRUCT


/* End of file ERP_def.h */
