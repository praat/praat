/* EEG_def.h
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


#define ooSTRUCT EEG
oo_DEFINE_CLASS (EEG, Function)

	oo_LONG (d_numberOfChannels)
	oo_STRING_VECTOR (d_channelNames, d_numberOfChannels)
	oo_OBJECT (Sound, 2, d_sound)
	oo_OBJECT (TextGrid, 0, d_textgrid)

	#if oo_DECLARING
		// functions:
		public:
			void f_init (double tmin, double tmax);
			long f_getChannelNumber (const wchar_t *channelName);
			void f_setChannelName (long channelNumber, const wchar_t *a_name);
			long f_getNumberOfCapElectrodes () { return (d_numberOfChannels - 1) & ~ 15L; }   // BUG
			long f_getNumberOfExternalElectrodes () { return d_numberOfChannels - f_getNumberOfCapElectrodes () - f_getNumberOfExtraSensors (); }
			long f_getNumberOfExtraSensors () { return d_numberOfChannels & 1 ? 1 : 8; }   // BUG
			void f_setExternalElectrodeNames (const wchar_t *nameExg1, const wchar_t *nameExg2, const wchar_t *nameExg3, const wchar_t *nameExg4,
				const wchar_t *nameExg5, const wchar_t *nameExg6, const wchar_t *nameExg7, const wchar_t *nameExg8);
			void f_detrend ();
			void f_filter (double lowFrequency, double lowWidth, double highFrequency, double highWidth, bool doNotch50Hz);
			void f_subtractReference (const wchar_t *channelNumber1, const wchar_t *channelNumber2);
			void f_subtractMeanChannel (long fromChannel, long toChannel);
			void f_setChannelToZero (long channelNumber);
			void f_setChannelToZero (const wchar_t *channelName);
			EEG f_extractChannel (long channelNumber);
			EEG f_extractChannel (const wchar_t *channelName);
			Sound f_extractSound () { return Data_copy (d_sound); }
			TextGrid f_extractTextGrid () { return Data_copy (d_textgrid); }
			EEG f_extractPart (double tmin, double tmax, bool preserveTimes);
			void f_replaceTextGrid (TextGrid textgrid);
		// overridden methods:
		protected:
			virtual void v_info ();
			virtual int v_domainQuantity () { return MelderQuantity_TIME_SECONDS; }
			virtual void v_shiftX (double xfrom, double xto);
			virtual void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto);
	#endif

oo_END_CLASS (EEG)
#undef ooSTRUCT

/* End of file EEG_def.h */
