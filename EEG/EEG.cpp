/* EEG.cpp
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

#include "EEG.h"
#include "Sound_and_Spectrum.h"

#include "oo_DESTROY.h"
#include "EEG_def.h"
#include "oo_COPY.h"
#include "EEG_def.h"
#include "oo_EQUAL.h"
#include "EEG_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "EEG_def.h"
#include "oo_WRITE_TEXT.h"
#include "EEG_def.h"
#include "oo_READ_TEXT.h"
#include "EEG_def.h"
#include "oo_WRITE_BINARY.h"
#include "EEG_def.h"
#include "oo_READ_BINARY.h"
#include "EEG_def.h"
#include "oo_DESCRIPTION.h"
#include "EEG_def.h"

Thing_implement (EEG, Function, 0);

void structEEG :: v_shiftX (double xfrom, double xto) {
	EEG_Parent :: v_shiftX (xfrom, xto);
	if (d_sound    != NULL)  Function_shiftXTo (d_sound,    xfrom, xto);
	if (d_textgrid != NULL)  Function_shiftXTo (d_textgrid, xfrom, xto);
}

void structEEG :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	EEG_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	if (d_sound    != NULL)  d_sound    -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	if (d_textgrid != NULL)  d_textgrid -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
}

EEG EEG_create (double tmin, double tmax) {
	try {
		autoEEG me = Thing_new (EEG);
		my xmin = tmin;
		my xmax = tmax;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("EEG object not created.");
	}
}

long structEEG :: f_getChannelNumber (const wchar *channelName) {
	for (long ichan = 1; ichan <= d_numberOfChannels; ichan ++) {
		if (Melder_wcsequ (d_channelNames [ichan], channelName)) {
			return ichan;
		}
	}
	return 0;
}

EEG EEG_readFromBdfFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		char buffer [81];
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		fread (buffer, 1, 80, f); buffer [80] = '\0';
		//Melder_casual ("Local subject identification: \"%s\"", buffer);
		fread (buffer, 1, 80, f); buffer [80] = '\0';
		//Melder_casual ("Local recording identification: \"%s\"", buffer);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		//Melder_casual ("Start date of recording: \"%s\"", buffer);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		//Melder_casual ("Start time of recording: \"%s\"", buffer);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		long numberOfBytesInHeaderRecord = atol (buffer);
		//Melder_casual ("Number of bytes in header record: %ld", numberOfBytesInHeaderRecord);
		fread (buffer, 1, 44, f); buffer [44] = '\0';
		//Melder_casual ("Version of data format: \"%s\"", buffer);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		long numberOfDataRecords = strtol (buffer, NULL, 10);
		//Melder_casual ("Number of data records: %ld", numberOfDataRecords);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		double durationOfDataRecord = atof (buffer);
		//Melder_casual ("Duration of a data record: \"%f\"", durationOfDataRecord);
		fread (buffer, 1, 4, f); buffer [4] = '\0';
		long numberOfChannels = atol (buffer);
		//Melder_casual ("Number of channels in data record: %ld", numberOfChannels);
		if (numberOfBytesInHeaderRecord != (numberOfChannels + 1) * 256)
			Melder_throw ("Number of bytes in header record (", numberOfBytesInHeaderRecord,
				") doesn't match number of channels (", numberOfChannels, ").");
		autostringvector channelNames (1, numberOfChannels);
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			fread (buffer, 1, 16, f); buffer [16] = '\0';   // labels of the channels
			/*
			 * Strip all final spaces.
			 */
			for (int i = 15; i >= 0; i --) {
				if (buffer [i] == ' ') {
					buffer [i] = '\0';
				} else {
					break;
				}
			}
			channelNames [ichannel] = Melder_wcsdup (Melder_peekUtf8ToWcs (buffer));
		}
		double samplingFrequency = NUMundefined;
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			fread (buffer, 1, 80, f); buffer [80] = '\0';   // transducer type
		}
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			fread (buffer, 1, 8, f); buffer [8] = '\0';   // physical dimension of channels
		}
		autoNUMvector <double> physicalMinimum (1, numberOfChannels);
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			fread (buffer, 1, 8, f); buffer [8] = '\0';
			physicalMinimum [ichannel] = atof (buffer);
		}
		autoNUMvector <double> physicalMaximum (1, numberOfChannels);
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			fread (buffer, 1, 8, f); buffer [8] = '\0';
			physicalMaximum [ichannel] = atof (buffer);
		}
		autoNUMvector <double> digitalMinimum (1, numberOfChannels);
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			fread (buffer, 1, 8, f); buffer [8] = '\0';
			digitalMinimum [ichannel] = atof (buffer);
		}
		autoNUMvector <double> digitalMaximum (1, numberOfChannels);
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			fread (buffer, 1, 8, f); buffer [8] = '\0';
			digitalMaximum [ichannel] = atof (buffer);
		}
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			fread (buffer, 1, 80, f); buffer [80] = '\0';   // prefiltering
		}
		long numberOfSamplesPerDataRecord = 0;
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			fread (buffer, 1, 8, f); buffer [8] = '\0';   // number of samples in each data record
			long numberOfSamplesInThisDataRecord = atol (buffer);
			if (samplingFrequency == NUMundefined) {
				numberOfSamplesPerDataRecord = numberOfSamplesInThisDataRecord;
				samplingFrequency = numberOfSamplesInThisDataRecord / durationOfDataRecord;
			}
			if (numberOfSamplesInThisDataRecord / durationOfDataRecord != samplingFrequency)
				Melder_throw (L"Number of samples per data record in channel ", channel,
					" (", numberOfSamplesInThisDataRecord,
					") doesn't match sampling frequency of channel 1 (", samplingFrequency, ").");
		}
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			fread (buffer, 1, 32, f); buffer [32] = '\0';   // reserved
		}
		double duration = numberOfDataRecords * durationOfDataRecord;
		autoSound me = Sound_createSimple (numberOfChannels, duration, samplingFrequency);
		for (long record = 1; record <= numberOfDataRecords; record ++) {
			for (long channel = 1; channel <= numberOfChannels; channel ++) {
				double factor = channel == numberOfChannels ? 1.0 : physicalMinimum [channel] / digitalMinimum [channel];
				if (channel < numberOfChannels - 8) factor /= 1000000.0;
				for (long i = 1; i <= numberOfSamplesPerDataRecord; i ++) {
					long sample = i + (record - 1) * numberOfSamplesPerDataRecord;
					Melder_assert (sample <= my nx);
					my z [channel] [sample] = bingeti3LE (f) * factor; therror
				}
			}
		}
		autoTextGrid thee = TextGrid_create (0, duration, L"S1 S2 S3 S4 S5 S6 S7 S8", L"");
		for (int bit = 1; bit <= 8; bit ++) {
			unsigned long bitValue = 1 << (bit - 1);
			IntervalTier tier = (IntervalTier) thy tiers -> item [bit];
			for (long i = 1; i <= my nx; i ++) {
				unsigned long previousValue = i == 1 ? 0 : (long) my z [numberOfChannels] [i - 1];
				unsigned long thisValue = (long) my z [numberOfChannels] [i];
				if ((thisValue & bitValue) != (previousValue & bitValue)) {
					double time = i == 1 ? 0.0 : my x1 + (i - 1.5) * my dx;
					if (time != 0.0)
						TextGrid_insertBoundary (thee.peek(), bit, time);
					if ((thisValue & bitValue) != 0)
						TextGrid_setIntervalText (thee.peek(), bit, tier -> intervals -> size, L"1");
				}
			}
		}
		f.close (file);
		autoEEG him = EEG_create (0, duration);
		his d_numberOfChannels = numberOfChannels;
		his d_channelNames = channelNames.transfer();
		his d_sound = me.transfer();
		his d_textgrid = thee.transfer();
		if (numberOfChannels == 80) {
			his f_setChannelName (1, L"Fp1");
			his f_setChannelName (2, L"AF7");
			his f_setChannelName (3, L"AF3");
			his f_setChannelName (4, L"F1");
			his f_setChannelName (5, L"F3");
			his f_setChannelName (6, L"F5");
			his f_setChannelName (7, L"F7");
			his f_setChannelName (8, L"FT7");
			his f_setChannelName (9, L"FC5");
			his f_setChannelName (10, L"FC3");
			his f_setChannelName (11, L"FC1");
			his f_setChannelName (12, L"C1");
			his f_setChannelName (13, L"C3");
			his f_setChannelName (14, L"C5");
			his f_setChannelName (15, L"T7");
			his f_setChannelName (16, L"TP7");
			his f_setChannelName (17, L"CP5");
			his f_setChannelName (18, L"CP3");
			his f_setChannelName (19, L"CP1");
			his f_setChannelName (20, L"P1");
			his f_setChannelName (21, L"P3");
			his f_setChannelName (22, L"P5");
			his f_setChannelName (23, L"P7");
			his f_setChannelName (24, L"P9");
			his f_setChannelName (25, L"PO7");
			his f_setChannelName (26, L"PO3");
			his f_setChannelName (27, L"O1");
			his f_setChannelName (28, L"Iz");
			his f_setChannelName (29, L"Oz");
			his f_setChannelName (30, L"POz");
			his f_setChannelName (31, L"Pz");
			his f_setChannelName (32, L"CPz");
			his f_setChannelName (33, L"Fpz");
			his f_setChannelName (34, L"Fp2");
			his f_setChannelName (35, L"AF8");
			his f_setChannelName (36, L"AF4");
			his f_setChannelName (37, L"AFz");
			his f_setChannelName (38, L"Fz");
			his f_setChannelName (39, L"F2");
			his f_setChannelName (40, L"F4");
			his f_setChannelName (41, L"F6");
			his f_setChannelName (42, L"F8");
			his f_setChannelName (43, L"FT8");
			his f_setChannelName (44, L"FC6");
			his f_setChannelName (45, L"FC4");
			his f_setChannelName (46, L"FC2");
			his f_setChannelName (47, L"FCz");
			his f_setChannelName (48, L"Cz");
			his f_setChannelName (49, L"C2");
			his f_setChannelName (50, L"C4");
			his f_setChannelName (51, L"C6");
			his f_setChannelName (52, L"T8");
			his f_setChannelName (53, L"TP8");
			his f_setChannelName (54, L"CP6");
			his f_setChannelName (55, L"CP4");
			his f_setChannelName (56, L"CP2");
			his f_setChannelName (57, L"P2");
			his f_setChannelName (58, L"P4");
			his f_setChannelName (59, L"P6");
			his f_setChannelName (60, L"P8");
			his f_setChannelName (61, L"P10");
			his f_setChannelName (62, L"PO8");
			his f_setChannelName (63, L"PO4");
			his f_setChannelName (64, L"O2");
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("BDF file not read.");
	}
}

static void detrend (double *a, long numberOfSamples) {
	double firstValue = a [1], lastValue = a [numberOfSamples];
	a [1] = a [numberOfSamples] = 0.0;
	for (long isamp = 2; isamp < numberOfSamples; isamp ++) {
		a [isamp] -= ((isamp - 1.0) * lastValue + (numberOfSamples - isamp) * firstValue) / (numberOfSamples - 1);
	}
}

void structEEG :: f_detrend () {
	for (long ichan = 1; ichan <= d_numberOfChannels - 8; ichan ++) {
		detrend (d_sound -> z [ichan], d_sound -> nx);
	}
}

void structEEG :: f_filter (double lowFrequency, double lowWidth, double highFrequency, double highWidth, bool doNotch50Hz) {
	try {
/*
	long nsampFFT = 1;
	while (nsampFFT < d_sound -> nx)
		nsampFFT *= 2;
	autoNUMfft_Table fftTable;
	NUMfft_Table_init (& fftTable, nsampFFT); therror
*/
		for (long ichan = 1; ichan <= d_numberOfChannels - 8; ichan ++) {
			autoSound channel = Sound_extractChannel (d_sound, ichan);
			autoSpectrum spec = Sound_to_Spectrum (channel.peek(), TRUE);
			Spectrum_passHannBand (spec.peek(), lowFrequency, 0.0, lowWidth);
			Spectrum_passHannBand (spec.peek(), 0.0, highFrequency, highWidth);
			if (doNotch50Hz) {
				Spectrum_stopHannBand (spec.peek(), 48.0, 52.0, 1.0);
			}
			autoSound him = Spectrum_to_Sound (spec.peek());
			NUMdvector_copyElements (his z [1], d_sound -> z [ichan], 1, d_sound -> nx);
		}
	} catch (MelderError) {
		Melder_throw (this, ": not filtered.");
	}
}

void structEEG :: f_setChannelName (long channelNumber, const wchar *a_name) {
	autostring l_name = Melder_wcsdup (a_name);
	Melder_free (d_channelNames [channelNumber]);
	d_channelNames [channelNumber] = l_name.transfer();
}

void structEEG :: f_setExternalElectrodeNames (const wchar *nameExg1, const wchar *nameExg2, const wchar *nameExg3, const wchar *nameExg4,
	const wchar *nameExg5, const wchar *nameExg6, const wchar *nameExg7, const wchar *nameExg8)
{
	f_setChannelName (d_numberOfChannels - 15, nameExg1);
	f_setChannelName (d_numberOfChannels - 14, nameExg2);
	f_setChannelName (d_numberOfChannels - 13, nameExg3);
	f_setChannelName (d_numberOfChannels - 12, nameExg4);
	f_setChannelName (d_numberOfChannels - 11, nameExg5);
	f_setChannelName (d_numberOfChannels - 10, nameExg6);
	f_setChannelName (d_numberOfChannels -  9, nameExg7);
	f_setChannelName (d_numberOfChannels -  8, nameExg8);
}

void structEEG :: f_subtractReference (const wchar *channelNumber1_text, const wchar *channelNumber2_text) {
	long channelNumber1 = f_getChannelNumber (channelNumber1_text);
	if (channelNumber1 == 0)
		Melder_throw (this, ": no channel named \"", channelNumber1_text, "\".");
	long channelNumber2 = f_getChannelNumber (channelNumber2_text);
	if (channelNumber2 == 0 && channelNumber2_text [0] != '\0')
		Melder_throw (this, ": no channel named \"", channelNumber2_text, "\".");
	for (long isamp = 1; isamp <= d_sound -> nx; isamp ++) {
		double referenceValue = channelNumber2 == 0 ? d_sound -> z [channelNumber1] [isamp] :
			0.5 * (d_sound -> z [channelNumber1] [isamp] + d_sound -> z [channelNumber2] [isamp]);
		for (long ichan = 1; ichan <= d_numberOfChannels - 8; ichan ++) {
			d_sound -> z [ichan] [isamp] -= referenceValue;
		}
	}
}

void structEEG :: f_setChannelToZero (long channelNumber) {
	try {
		if (channelNumber < 1 || channelNumber > d_numberOfChannels)
			Melder_throw ("No channel ", channelNumber, ".");
		long numberOfSamples = d_sound -> nx;
		double *channel = d_sound -> z [channelNumber];
		for (long isample = 1; isample <= numberOfSamples; isample ++) {
			channel [isample] = 0.0;
		}
	} catch (MelderError) {
		Melder_throw (this, ": channel ", channelNumber, " not set to zero.");
	}
}

void structEEG :: f_setChannelToZero (const wchar *channelName) {
	try {
		long channelNumber = f_getChannelNumber (channelName);
		if (channelNumber == 0)
			Melder_throw ("No channel named \"", channelName, "\".");
		f_setChannelToZero (channelNumber);
	} catch (MelderError) {
		Melder_throw (this, ": channel ", channelName, " not set to zero.");
	}
}

EEG structEEG :: f_extractChannel (long channelNumber) {
	try {
		if (channelNumber < 1 || channelNumber > d_numberOfChannels)
			Melder_throw ("No channel ", channelNumber, ".");
		autoEEG thee = EEG_create (xmin, xmax);
		thee -> d_numberOfChannels = 1;
		thee -> d_channelNames = NUMvector <wchar *> (1, 1);
		thee -> d_channelNames [1] = Melder_wcsdup (d_channelNames [1]);
		thee -> d_sound = Sound_extractChannel (d_sound, channelNumber);
		thee -> d_textgrid = Data_copy (d_textgrid);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": channel ", channelNumber, " not extracted.");
	}
}

EEG structEEG :: f_extractChannel (const wchar *channelName) {
	try {
		long channelNumber = f_getChannelNumber (channelName);
		if (channelNumber == 0)
			Melder_throw ("No channel named \"", channelName, "\".");
		return f_extractChannel (channelNumber);
	} catch (MelderError) {
		Melder_throw (this, ": channel ", channelName, " not extracted.");
	}
}

/* End of file EEG.cpp */
