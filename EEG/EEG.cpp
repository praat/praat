/* EEG.cpp
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

void structEEG :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Time domain:");
	MelderInfo_writeLine (L"   Start time: ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (L"   End time: ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"   Total duration: ", Melder_double (xmax - xmin), L" seconds");
	if (d_sound != NULL) {
		MelderInfo_writeLine (L"Time sampling of the signal:");
		MelderInfo_writeLine (L"   Number of samples: ", Melder_integer (d_sound -> nx));
		MelderInfo_writeLine (L"   Sampling period: ", Melder_double (d_sound -> dx), L" seconds");
		MelderInfo_writeLine (L"   Sampling frequency: ", Melder_single (1.0 / d_sound -> dx), L" Hz");
		MelderInfo_writeLine (L"   First sample centred at: ", Melder_double (d_sound -> x1), L" seconds");
	}
	MelderInfo_writeLine (L"Number of cap electrodes: ", Melder_integer (f_getNumberOfCapElectrodes ()));
	MelderInfo_writeLine (L"Number of external electrodes: ", Melder_integer (f_getNumberOfExternalElectrodes ()));
	MelderInfo_writeLine (L"Number of extra sensors: ", Melder_integer (f_getNumberOfExtraSensors ()));
}

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

long structEEG :: f_getChannelNumber (const wchar_t *channelName) {
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
		autoEEG him = EEG_create (0, duration);
		his d_numberOfChannels = numberOfChannels;
		autoSound me = Sound_createSimple (numberOfChannels, duration, samplingFrequency);
		for (long record = 1; record <= numberOfDataRecords; record ++) {
			for (long channel = 1; channel <= numberOfChannels; channel ++) {
				double factor = channel == numberOfChannels ? 1.0 : physicalMinimum [channel] / digitalMinimum [channel];
				if (channel < numberOfChannels - his f_getNumberOfExtraSensors ()) factor /= 1000000.0;
				for (long i = 1; i <= numberOfSamplesPerDataRecord; i ++) {
					long sample = i + (record - 1) * numberOfSamplesPerDataRecord;
					Melder_assert (sample <= my nx);
					my z [channel] [sample] = bingeti3LE (f) * factor;
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
		his d_channelNames = channelNames.transfer();
		his d_sound = me.transfer();
		his d_textgrid = thee.transfer();
		if (his f_getNumberOfCapElectrodes () == 32) {
			his f_setChannelName (1, L"Fp1");
			his f_setChannelName (2, L"AF3");
			his f_setChannelName (3, L"F7");
			his f_setChannelName (4, L"F3");
			his f_setChannelName (5, L"FC1");
			his f_setChannelName (6, L"FC5");
			his f_setChannelName (7, L"T7");
			his f_setChannelName (8, L"C3");
			his f_setChannelName (9, L"CP1");
			his f_setChannelName (10, L"CP5");
			his f_setChannelName (11, L"P7");
			his f_setChannelName (12, L"P3");
			his f_setChannelName (13, L"Pz");
			his f_setChannelName (14, L"PO3");
			his f_setChannelName (15, L"O1");
			his f_setChannelName (16, L"Oz");
			his f_setChannelName (17, L"O2");
			his f_setChannelName (18, L"PO4");
			his f_setChannelName (19, L"P4");
			his f_setChannelName (20, L"P8");
			his f_setChannelName (21, L"CP6");
			his f_setChannelName (22, L"CP2");
			his f_setChannelName (23, L"C4");
			his f_setChannelName (24, L"T8");
			his f_setChannelName (25, L"FC6");
			his f_setChannelName (26, L"FC2");
			his f_setChannelName (27, L"F4");
			his f_setChannelName (28, L"F8");
			his f_setChannelName (29, L"AF4");
			his f_setChannelName (30, L"Fp2");
			his f_setChannelName (31, L"Fz");
			his f_setChannelName (32, L"Cz");
		} else if (his f_getNumberOfCapElectrodes () == 64) {
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
	for (long ichan = 1; ichan <= d_numberOfChannels - f_getNumberOfExtraSensors (); ichan ++) {
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
	NUMfft_Table_init (& fftTable, nsampFFT);
*/
		for (long ichan = 1; ichan <= d_numberOfChannels - f_getNumberOfExtraSensors (); ichan ++) {
			autoSound channel = Sound_extractChannel (d_sound, ichan);
			autoSpectrum spec = Sound_to_Spectrum (channel.peek(), TRUE);
			Spectrum_passHannBand (spec.peek(), lowFrequency, 0.0, lowWidth);
			Spectrum_passHannBand (spec.peek(), 0.0, highFrequency, highWidth);
			if (doNotch50Hz) {
				Spectrum_stopHannBand (spec.peek(), 48.0, 52.0, 1.0);
			}
			autoSound him = Spectrum_to_Sound (spec.peek());
			NUMvector_copyElements (his z [1], d_sound -> z [ichan], 1, d_sound -> nx);
		}
	} catch (MelderError) {
		Melder_throw (this, ": not filtered.");
	}
}

void structEEG :: f_setChannelName (long channelNumber, const wchar_t *a_name) {
	autostring l_name = Melder_wcsdup (a_name);
	Melder_free (d_channelNames [channelNumber]);
	d_channelNames [channelNumber] = l_name.transfer();
}

void structEEG :: f_setExternalElectrodeNames (const wchar_t *nameExg1, const wchar_t *nameExg2, const wchar_t *nameExg3, const wchar_t *nameExg4,
	const wchar_t *nameExg5, const wchar_t *nameExg6, const wchar_t *nameExg7, const wchar_t *nameExg8)
{
	if (f_getNumberOfExternalElectrodes () != 8)
		Melder_throw (L"There aren't 8 external electrodes.");
	const long firstExternalElectrode = f_getNumberOfCapElectrodes () + 1;
	f_setChannelName (firstExternalElectrode, nameExg1);
	f_setChannelName (firstExternalElectrode + 1, nameExg2);
	f_setChannelName (firstExternalElectrode + 2, nameExg3);
	f_setChannelName (firstExternalElectrode + 3, nameExg4);
	f_setChannelName (firstExternalElectrode + 4, nameExg5);
	f_setChannelName (firstExternalElectrode + 5, nameExg6);
	f_setChannelName (firstExternalElectrode + 6, nameExg7);
	f_setChannelName (firstExternalElectrode + 7, nameExg8);
}

void structEEG :: f_subtractReference (const wchar_t *channelNumber1_text, const wchar_t *channelNumber2_text) {
	long channelNumber1 = f_getChannelNumber (channelNumber1_text);
	if (channelNumber1 == 0)
		Melder_throw (this, ": no channel named \"", channelNumber1_text, "\".");
	long channelNumber2 = f_getChannelNumber (channelNumber2_text);
	if (channelNumber2 == 0 && channelNumber2_text [0] != '\0')
		Melder_throw (this, ": no channel named \"", channelNumber2_text, "\".");
	const long numberOfElectrodeChannels = d_numberOfChannels - f_getNumberOfExtraSensors ();
	for (long isamp = 1; isamp <= d_sound -> nx; isamp ++) {
		double referenceValue = channelNumber2 == 0 ? d_sound -> z [channelNumber1] [isamp] :
			0.5 * (d_sound -> z [channelNumber1] [isamp] + d_sound -> z [channelNumber2] [isamp]);
		for (long ichan = 1; ichan <= numberOfElectrodeChannels; ichan ++) {
			d_sound -> z [ichan] [isamp] -= referenceValue;
		}
	}
}

void structEEG :: f_subtractMeanChannel (long fromChannel, long toChannel) {
	if (fromChannel < 1 || fromChannel > d_numberOfChannels)
		Melder_throw ("No channel ", fromChannel, ".");
	if (toChannel < 1 || toChannel > d_numberOfChannels)
		Melder_throw ("No channel ", toChannel, ".");
	if (fromChannel > toChannel)
		Melder_throw ("Channel range cannot run from ", fromChannel, " to ", toChannel, ". Please reverse.");
	const long numberOfElectrodeChannels = d_numberOfChannels - f_getNumberOfExtraSensors ();
	for (long isamp = 1; isamp <= d_sound -> nx; isamp ++) {
		double referenceValue = 0.0;
		for (long ichan = fromChannel; ichan <= toChannel; ichan ++) {
			referenceValue += d_sound -> z [ichan] [isamp];
		}
		referenceValue /= (toChannel - fromChannel + 1);
		for (long ichan = 1; ichan <= numberOfElectrodeChannels; ichan ++) {
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

void structEEG :: f_setChannelToZero (const wchar_t *channelName) {
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
		thee -> d_channelNames = NUMvector <wchar_t *> (1, 1);
		thee -> d_channelNames [1] = Melder_wcsdup (d_channelNames [1]);
		thee -> d_sound = Sound_extractChannel (d_sound, channelNumber);
		thee -> d_textgrid = Data_copy (d_textgrid);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": channel ", channelNumber, " not extracted.");
	}
}

EEG structEEG :: f_extractChannel (const wchar_t *channelName) {
	try {
		long channelNumber = f_getChannelNumber (channelName);
		if (channelNumber == 0)
			Melder_throw ("No channel named \"", channelName, "\".");
		return f_extractChannel (channelNumber);
	} catch (MelderError) {
		Melder_throw (this, ": channel ", channelName, " not extracted.");
	}
}

EEG EEGs_concatenate (Collection me) {
	try {
		if (my size < 1)
			Melder_throw ("Cannot concatenate zero EEG objects.");
		EEG first = (EEG) my item [1];
		long numberOfChannels = first -> d_numberOfChannels;
		wchar_t **channelNames = first -> d_channelNames;
		for (long ieeg = 2; ieeg <= my size; ieeg ++) {
			EEG other = (EEG) my item [ieeg];
			if (other -> d_numberOfChannels != numberOfChannels)
				Melder_throw ("The number of channels of ", other, " does not match the number of channels of ", first, ".");
			for (long ichan = 1; ichan <= numberOfChannels; ichan ++) {
				if (! Melder_wcsequ (other -> d_channelNames [ichan], channelNames [ichan]))
					Melder_throw ("Channel ", ichan, " has a different name in ", other, " (", other -> d_channelNames [ichan], ") than in ", first, " (", channelNames [ichan], ").");
			}
		}
		autoOrdered soundCollection = Ordered_create ();
		Collection_dontOwnItems (soundCollection.peek());
		autoOrdered textgridCollection = Ordered_create ();
		Collection_dontOwnItems (textgridCollection.peek());
		for (long ieeg = 1; ieeg <= my size; ieeg ++) {
			EEG eeg = (EEG) my item [ieeg];
			Collection_addItem (soundCollection.peek(), eeg -> d_sound);
			Collection_addItem (textgridCollection.peek(), eeg -> d_textgrid);
		}
		autoEEG thee = Thing_new (EEG);
		thy d_numberOfChannels = numberOfChannels;
		thy d_channelNames = NUMvector <wchar_t *> (1, numberOfChannels);
		for (long ichan = 1; ichan <= numberOfChannels; ichan ++) {
			thy d_channelNames [ichan] = Melder_wcsdup (channelNames [ichan]);
		}
		thy d_sound = Sounds_concatenate_e (soundCollection.peek(), 0.0);
		thy d_textgrid = TextGrids_concatenate (textgridCollection.peek());
		thy xmin = thy d_textgrid -> xmin;
		thy xmax = thy d_textgrid -> xmax;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrids not concatenated.");
	}
}

EEG structEEG :: f_extractPart (double tmin, double tmax, bool preserveTimes) {
	try {
		autoEEG thee = Thing_new (EEG);
		thy d_numberOfChannels = d_numberOfChannels;
		thy d_channelNames = NUMvector <wchar_t *> (1, d_numberOfChannels);
		for (long ichan = 1; ichan <= d_numberOfChannels; ichan ++) {
			thy d_channelNames [ichan] = Melder_wcsdup (d_channelNames [ichan]);
		}
		thy d_sound = Sound_extractPart (d_sound, tmin, tmax, kSound_windowShape_RECTANGULAR, 1.0, preserveTimes);
		thy d_textgrid = TextGrid_extractPart (d_textgrid, tmin, tmax, preserveTimes);
		thy xmin = thy d_textgrid -> xmin;
		thy xmax = thy d_textgrid -> xmax;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": part not extracted.");
	}
}

void structEEG :: f_replaceTextGrid (TextGrid textgrid) {
	try {
		autoTextGrid textgrid2 = Data_copy (textgrid);
		forget (d_textgrid);
		d_textgrid = textgrid2.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": TextGrid not replaced with ", textgrid, ".");
	}
}

/* End of file EEG.cpp */
