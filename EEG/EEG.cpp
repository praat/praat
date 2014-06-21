/* EEG.cpp
 *
 * Copyright (C) 2011-2012,2013,2014 Paul Boersma
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
	MelderInfo_writeLine (L"   Start time: ", Melder_double (our xmin), L" seconds");
	MelderInfo_writeLine (L"   End time: ", Melder_double (our xmax), L" seconds");
	MelderInfo_writeLine (L"   Total duration: ", Melder_double (our xmax - our xmin), L" seconds");
	if (our sound != NULL) {
		MelderInfo_writeLine (L"Time sampling of the signal:");
		MelderInfo_writeLine (L"   Number of samples: ", Melder_integer (our sound -> nx));
		MelderInfo_writeLine (L"   Sampling period: ", Melder_double (our sound -> dx), L" seconds");
		MelderInfo_writeLine (L"   Sampling frequency: ", Melder_single (1.0 / our sound -> dx), L" Hz");
		MelderInfo_writeLine (L"   First sample centred at: ", Melder_double (our sound -> x1), L" seconds");
	}
	MelderInfo_writeLine (L"Number of cap electrodes: ", Melder_integer (EEG_getNumberOfCapElectrodes (this)));
	MelderInfo_writeLine (L"Number of external electrodes: ", Melder_integer (EEG_getNumberOfExternalElectrodes (this)));
	MelderInfo_writeLine (L"Number of extra sensors: ", Melder_integer (EEG_getNumberOfExtraSensors (this)));
}

void structEEG :: v_shiftX (double xfrom, double xto) {
	EEG_Parent :: v_shiftX (xfrom, xto);
	if (our sound    != NULL)  Function_shiftXTo (our sound,    xfrom, xto);
	if (our textgrid != NULL)  Function_shiftXTo (our textgrid, xfrom, xto);
}

void structEEG :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	EEG_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	if (our sound    != NULL)  our sound    -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	if (our textgrid != NULL)  our textgrid -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
}

void EEG_init (EEG me, double tmin, double tmax) {
	my xmin = tmin;
	my xmax = tmax;
}

EEG EEG_create (double tmin, double tmax) {
	try {
		autoEEG me = Thing_new (EEG);
		EEG_init (me.peek(), tmin, tmax);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("EEG object not created.");
	}
}

long EEG_getChannelNumber (EEG me, const wchar_t *channelName) {
	for (long ichan = 1; ichan <= my numberOfChannels; ichan ++) {
		if (Melder_wcsequ (my channelNames [ichan], channelName)) {
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
		bool is24bit = buffer [0] == (char) 255;
		fread (buffer, 1, 80, f); buffer [80] = '\0';
		trace ("Local subject identification: \"%s\"", buffer);
		fread (buffer, 1, 80, f); buffer [80] = '\0';
		trace ("Local recording identification: \"%s\"", buffer);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		trace ("Start date of recording: \"%s\"", buffer);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		trace ("Start time of recording: \"%s\"", buffer);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		long numberOfBytesInHeaderRecord = atol (buffer);
		trace ("Number of bytes in header record: %ld", numberOfBytesInHeaderRecord);
		fread (buffer, 1, 44, f); buffer [44] = '\0';
		trace ("Version of data format: \"%s\"", buffer);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		long numberOfDataRecords = strtol (buffer, NULL, 10);
		trace ("Number of data records: %ld", numberOfDataRecords);
		fread (buffer, 1, 8, f); buffer [8] = '\0';
		double durationOfDataRecord = atof (buffer);
		trace ("Duration of a data record: \"%f\"", durationOfDataRecord);
		fread (buffer, 1, 4, f); buffer [4] = '\0';
		long numberOfChannels = atol (buffer);
		trace ("Number of channels in data record: %ld", numberOfChannels);
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
			trace ("Channel <<%ls>>", channelNames [ichannel]);
		}
		bool hasLetters = wcsequ (channelNames [numberOfChannels], L"EDF Annotations");
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
		his numberOfChannels = numberOfChannels;
		autoSound me = Sound_createSimple (numberOfChannels, duration, samplingFrequency);
		Melder_assert (my nx == numberOfSamplesPerDataRecord * numberOfDataRecords);
		autoNUMvector <unsigned char> dataBuffer (0L, 3 * numberOfSamplesPerDataRecord - 1);
		for (long record = 1; record <= numberOfDataRecords; record ++) {
			for (long channel = 1; channel <= numberOfChannels; channel ++) {
				double factor = channel == numberOfChannels ? 1.0 : physicalMinimum [channel] / digitalMinimum [channel];
				if (channel < numberOfChannels - EEG_getNumberOfExtraSensors (him.peek())) factor /= 1000000.0;
				if (is24bit) {
					fread (& dataBuffer [0], 3, numberOfSamplesPerDataRecord, f);
					unsigned char *p = & dataBuffer [0];
					for (long i = 1; i <= numberOfSamplesPerDataRecord; i ++) {
						long sample = i + (record - 1) * numberOfSamplesPerDataRecord;
						Melder_assert (sample <= my nx);
						uint8_t lowByte = *p ++, midByte = *p ++, highByte = *p ++;
						uint32_t externalValue = ((uint32_t) highByte << 16) | ((uint32_t) midByte << 8) | (uint32_t) lowByte;
						if ((highByte & 128) != 0)   // is the 24-bit sign bit on?
							externalValue |= 0xFF000000;   // extend negative sign to 32 bits
						my z [channel] [sample] = (int32_t) externalValue * factor;
					}
				} else {
					fread (& dataBuffer [0], 2, numberOfSamplesPerDataRecord, f);
					unsigned char *p = & dataBuffer [0];
					for (long i = 1; i <= numberOfSamplesPerDataRecord; i ++) {
						long sample = i + (record - 1) * numberOfSamplesPerDataRecord;
						Melder_assert (sample <= my nx);
						uint8_t lowByte = *p ++, highByte = *p ++;
						uint16_t externalValue = ((uint16_t) highByte << 8) | (uint16_t) lowByte;
						my z [channel] [sample] = (int16_t) externalValue * factor;
					}
				}
			}
		}
		int numberOfStatusBits = 8;
		for (long i = 1; i <= my nx; i ++) {
			unsigned long value = (long) my z [numberOfChannels] [i];
			if (value & 0x0000FF00) {
				numberOfStatusBits = 16;
			}
		}
		autoTextGrid thee;
		if (hasLetters) {
			thee.reset (TextGrid_create (0, duration, L"Mark Trigger", L"Mark Trigger"));
			autoMelderString letters;
			double time = NUMundefined;
			for (long i = 1; i <= my nx; i ++) {
				unsigned long value = (long) my z [numberOfChannels] [i];
				for (int byte = 1; byte <= numberOfStatusBits / 8; byte ++) {
					unsigned long mask = byte == 1 ? 0x000000ff : 0x0000ff00;
					wchar_t kar = byte == 1 ? (value & mask) : (value & mask) >> 8;
					if (kar != '\0' && kar != 20) {
						MelderString_appendCharacter (& letters, kar);
					} else if (letters. string [0] != '\0') {
						if (letters. string [0] == '+') {
							if (NUMdefined (time)) {
								try {
									TextGrid_insertPoint (thee.peek(), 1, time, L"");
								} catch (MelderError) {
									Melder_throw ("Did not insert empty mark (", letters. string, ") on Mark tier.");
								}
								time = NUMundefined;   // defensive
							}
							time = Melder_atof (& letters. string [1]);
							MelderString_empty (& letters);
						} else {
							if (! NUMdefined (time)) {
								Melder_throw ("Undefined time for label at sample ", i, ".");
							}
							try {
								if (Melder_wcsnequ (letters. string, L"Trigger-", 8)) {
									try {
										TextGrid_insertPoint (thee.peek(), 2, time, & letters. string [8]);
									} catch (MelderError) {
										Melder_clearError ();
										trace ("Duplicate trigger at %f seconds: %ls", time, & letters. string [8]);
									}
								} else {
									TextGrid_insertPoint (thee.peek(), 1, time, & letters. string [0]);
								}
							} catch (MelderError) {
								Melder_throw ("Did not insert mark (", letters. string, ") on Trigger tier.");
							}
							time = NUMundefined;   // crucial
							MelderString_empty (& letters);
						}
					}
				}
			}
			if (NUMdefined (time)) {
				TextGrid_insertPoint (thee.peek(), 1, time, L"");
				time = NUMundefined;   // defensive
			}
		} else {
			thee.reset (TextGrid_create (0, duration,
				numberOfStatusBits == 8 ? L"S1 S2 S3 S4 S5 S6 S7 S8" : L"S1 S2 S3 S4 S5 S6 S7 S8 S9 S10 S11 S12 S13 S14 S15 S16", L""));
			for (int bit = 1; bit <= numberOfStatusBits; bit ++) {
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
		}
		f.close (file);
		his channelNames = channelNames.transfer();
		his sound = me.transfer();
		his textgrid = thee.transfer();
		if (EEG_getNumberOfCapElectrodes (him.peek()) == 32) {
			EEG_setChannelName (him.peek(), 1, L"Fp1");
			EEG_setChannelName (him.peek(), 2, L"AF3");
			EEG_setChannelName (him.peek(), 3, L"F7");
			EEG_setChannelName (him.peek(), 4, L"F3");
			EEG_setChannelName (him.peek(), 5, L"FC1");
			EEG_setChannelName (him.peek(), 6, L"FC5");
			EEG_setChannelName (him.peek(), 7, L"T7");
			EEG_setChannelName (him.peek(), 8, L"C3");
			EEG_setChannelName (him.peek(), 9, L"CP1");
			EEG_setChannelName (him.peek(), 10, L"CP5");
			EEG_setChannelName (him.peek(), 11, L"P7");
			EEG_setChannelName (him.peek(), 12, L"P3");
			EEG_setChannelName (him.peek(), 13, L"Pz");
			EEG_setChannelName (him.peek(), 14, L"PO3");
			EEG_setChannelName (him.peek(), 15, L"O1");
			EEG_setChannelName (him.peek(), 16, L"Oz");
			EEG_setChannelName (him.peek(), 17, L"O2");
			EEG_setChannelName (him.peek(), 18, L"PO4");
			EEG_setChannelName (him.peek(), 19, L"P4");
			EEG_setChannelName (him.peek(), 20, L"P8");
			EEG_setChannelName (him.peek(), 21, L"CP6");
			EEG_setChannelName (him.peek(), 22, L"CP2");
			EEG_setChannelName (him.peek(), 23, L"C4");
			EEG_setChannelName (him.peek(), 24, L"T8");
			EEG_setChannelName (him.peek(), 25, L"FC6");
			EEG_setChannelName (him.peek(), 26, L"FC2");
			EEG_setChannelName (him.peek(), 27, L"F4");
			EEG_setChannelName (him.peek(), 28, L"F8");
			EEG_setChannelName (him.peek(), 29, L"AF4");
			EEG_setChannelName (him.peek(), 30, L"Fp2");
			EEG_setChannelName (him.peek(), 31, L"Fz");
			EEG_setChannelName (him.peek(), 32, L"Cz");
		} else if (EEG_getNumberOfCapElectrodes (him.peek()) == 64) {
			EEG_setChannelName (him.peek(), 1, L"Fp1");
			EEG_setChannelName (him.peek(), 2, L"AF7");
			EEG_setChannelName (him.peek(), 3, L"AF3");
			EEG_setChannelName (him.peek(), 4, L"F1");
			EEG_setChannelName (him.peek(), 5, L"F3");
			EEG_setChannelName (him.peek(), 6, L"F5");
			EEG_setChannelName (him.peek(), 7, L"F7");
			EEG_setChannelName (him.peek(), 8, L"FT7");
			EEG_setChannelName (him.peek(), 9, L"FC5");
			EEG_setChannelName (him.peek(), 10, L"FC3");
			EEG_setChannelName (him.peek(), 11, L"FC1");
			EEG_setChannelName (him.peek(), 12, L"C1");
			EEG_setChannelName (him.peek(), 13, L"C3");
			EEG_setChannelName (him.peek(), 14, L"C5");
			EEG_setChannelName (him.peek(), 15, L"T7");
			EEG_setChannelName (him.peek(), 16, L"TP7");
			EEG_setChannelName (him.peek(), 17, L"CP5");
			EEG_setChannelName (him.peek(), 18, L"CP3");
			EEG_setChannelName (him.peek(), 19, L"CP1");
			EEG_setChannelName (him.peek(), 20, L"P1");
			EEG_setChannelName (him.peek(), 21, L"P3");
			EEG_setChannelName (him.peek(), 22, L"P5");
			EEG_setChannelName (him.peek(), 23, L"P7");
			EEG_setChannelName (him.peek(), 24, L"P9");
			EEG_setChannelName (him.peek(), 25, L"PO7");
			EEG_setChannelName (him.peek(), 26, L"PO3");
			EEG_setChannelName (him.peek(), 27, L"O1");
			EEG_setChannelName (him.peek(), 28, L"Iz");
			EEG_setChannelName (him.peek(), 29, L"Oz");
			EEG_setChannelName (him.peek(), 30, L"POz");
			EEG_setChannelName (him.peek(), 31, L"Pz");
			EEG_setChannelName (him.peek(), 32, L"CPz");
			EEG_setChannelName (him.peek(), 33, L"Fpz");
			EEG_setChannelName (him.peek(), 34, L"Fp2");
			EEG_setChannelName (him.peek(), 35, L"AF8");
			EEG_setChannelName (him.peek(), 36, L"AF4");
			EEG_setChannelName (him.peek(), 37, L"AFz");
			EEG_setChannelName (him.peek(), 38, L"Fz");
			EEG_setChannelName (him.peek(), 39, L"F2");
			EEG_setChannelName (him.peek(), 40, L"F4");
			EEG_setChannelName (him.peek(), 41, L"F6");
			EEG_setChannelName (him.peek(), 42, L"F8");
			EEG_setChannelName (him.peek(), 43, L"FT8");
			EEG_setChannelName (him.peek(), 44, L"FC6");
			EEG_setChannelName (him.peek(), 45, L"FC4");
			EEG_setChannelName (him.peek(), 46, L"FC2");
			EEG_setChannelName (him.peek(), 47, L"FCz");
			EEG_setChannelName (him.peek(), 48, L"Cz");
			EEG_setChannelName (him.peek(), 49, L"C2");
			EEG_setChannelName (him.peek(), 50, L"C4");
			EEG_setChannelName (him.peek(), 51, L"C6");
			EEG_setChannelName (him.peek(), 52, L"T8");
			EEG_setChannelName (him.peek(), 53, L"TP8");
			EEG_setChannelName (him.peek(), 54, L"CP6");
			EEG_setChannelName (him.peek(), 55, L"CP4");
			EEG_setChannelName (him.peek(), 56, L"CP2");
			EEG_setChannelName (him.peek(), 57, L"P2");
			EEG_setChannelName (him.peek(), 58, L"P4");
			EEG_setChannelName (him.peek(), 59, L"P6");
			EEG_setChannelName (him.peek(), 60, L"P8");
			EEG_setChannelName (him.peek(), 61, L"P10");
			EEG_setChannelName (him.peek(), 62, L"PO8");
			EEG_setChannelName (him.peek(), 63, L"PO4");
			EEG_setChannelName (him.peek(), 64, L"O2");
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

void EEG_detrend (EEG me) {
	for (long ichan = 1; ichan <= my numberOfChannels - EEG_getNumberOfExtraSensors (me); ichan ++) {
		detrend (my sound -> z [ichan], my sound -> nx);
	}
}

void EEG_filter (EEG me, double lowFrequency, double lowWidth, double highFrequency, double highWidth, bool doNotch50Hz) {
	try {
/*
	long nsampFFT = 1;
	while (nsampFFT < my sound -> nx)
		nsampFFT *= 2;
	autoNUMfft_Table fftTable;
	NUMfft_Table_init (& fftTable, nsampFFT);
*/
		for (long ichan = 1; ichan <= my numberOfChannels - EEG_getNumberOfExtraSensors (me); ichan ++) {
			autoSound channel = Sound_extractChannel (my sound, ichan);
			autoSpectrum spec = Sound_to_Spectrum (channel.peek(), TRUE);
			Spectrum_passHannBand (spec.peek(), lowFrequency, 0.0, lowWidth);
			Spectrum_passHannBand (spec.peek(), 0.0, highFrequency, highWidth);
			if (doNotch50Hz) {
				Spectrum_stopHannBand (spec.peek(), 48.0, 52.0, 1.0);
			}
			autoSound him = Spectrum_to_Sound (spec.peek());
			NUMvector_copyElements (his z [1], my sound -> z [ichan], 1, my sound -> nx);
		}
	} catch (MelderError) {
		Melder_throw (me, ": not filtered.");
	}
}

void EEG_setChannelName (EEG me, long channelNumber, const wchar_t *a_name) {
	autostring l_name = Melder_wcsdup (a_name);
	Melder_free (my channelNames [channelNumber]);
	my channelNames [channelNumber] = l_name.transfer();
}

void EEG_setExternalElectrodeNames (EEG me,
	const wchar_t *nameExg1, const wchar_t *nameExg2, const wchar_t *nameExg3, const wchar_t *nameExg4,
	const wchar_t *nameExg5, const wchar_t *nameExg6, const wchar_t *nameExg7, const wchar_t *nameExg8)
{
	if (EEG_getNumberOfExternalElectrodes (me) != 8)
		Melder_throw (L"There aren't 8 external electrodes.");
	const long firstExternalElectrode = EEG_getNumberOfCapElectrodes (me) + 1;
	EEG_setChannelName (me, firstExternalElectrode, nameExg1);
	EEG_setChannelName (me, firstExternalElectrode + 1, nameExg2);
	EEG_setChannelName (me, firstExternalElectrode + 2, nameExg3);
	EEG_setChannelName (me, firstExternalElectrode + 3, nameExg4);
	EEG_setChannelName (me, firstExternalElectrode + 4, nameExg5);
	EEG_setChannelName (me, firstExternalElectrode + 5, nameExg6);
	EEG_setChannelName (me, firstExternalElectrode + 6, nameExg7);
	EEG_setChannelName (me, firstExternalElectrode + 7, nameExg8);
}

void EEG_subtractReference (EEG me, const wchar_t *channelNumber1_text, const wchar_t *channelNumber2_text) {
	long channelNumber1 = EEG_getChannelNumber (me, channelNumber1_text);
	if (channelNumber1 == 0)
		Melder_throw (me, ": no channel named \"", channelNumber1_text, "\".");
	long channelNumber2 = EEG_getChannelNumber (me, channelNumber2_text);
	if (channelNumber2 == 0 && channelNumber2_text [0] != '\0')
		Melder_throw (me, ": no channel named \"", channelNumber2_text, "\".");
	const long numberOfElectrodeChannels = my numberOfChannels - EEG_getNumberOfExtraSensors (me);
	for (long isamp = 1; isamp <= my sound -> nx; isamp ++) {
		double referenceValue = channelNumber2 == 0 ? my sound -> z [channelNumber1] [isamp] :
			0.5 * (my sound -> z [channelNumber1] [isamp] + my sound -> z [channelNumber2] [isamp]);
		for (long ichan = 1; ichan <= numberOfElectrodeChannels; ichan ++) {
			my sound -> z [ichan] [isamp] -= referenceValue;
		}
	}
}

void EEG_subtractMeanChannel (EEG me, long fromChannel, long toChannel) {
	if (fromChannel < 1 || fromChannel > my numberOfChannels)
		Melder_throw ("No channel ", fromChannel, ".");
	if (toChannel < 1 || toChannel > my numberOfChannels)
		Melder_throw ("No channel ", toChannel, ".");
	if (fromChannel > toChannel)
		Melder_throw ("Channel range cannot run from ", fromChannel, " to ", toChannel, ". Please reverse.");
	const long numberOfElectrodeChannels = my numberOfChannels - EEG_getNumberOfExtraSensors (me);
	for (long isamp = 1; isamp <= my sound -> nx; isamp ++) {
		double referenceValue = 0.0;
		for (long ichan = fromChannel; ichan <= toChannel; ichan ++) {
			referenceValue += my sound -> z [ichan] [isamp];
		}
		referenceValue /= (toChannel - fromChannel + 1);
		for (long ichan = 1; ichan <= numberOfElectrodeChannels; ichan ++) {
			my sound -> z [ichan] [isamp] -= referenceValue;
		}
	}
}

void EEG_setChannelToZero (EEG me, long channelNumber) {
	try {
		if (channelNumber < 1 || channelNumber > my numberOfChannels)
			Melder_throw ("No channel ", channelNumber, ".");
		long numberOfSamples = my sound -> nx;
		double *channel = my sound -> z [channelNumber];
		for (long isample = 1; isample <= numberOfSamples; isample ++) {
			channel [isample] = 0.0;
		}
	} catch (MelderError) {
		Melder_throw (me, ": channel ", channelNumber, " not set to zero.");
	}
}

void EEG_setChannelToZero (EEG me, const wchar_t *channelName) {
	try {
		long channelNumber = EEG_getChannelNumber (me, channelName);
		if (channelNumber == 0)
			Melder_throw ("No channel named \"", channelName, "\".");
		EEG_setChannelToZero (me, channelNumber);
	} catch (MelderError) {
		Melder_throw (me, ": channel ", channelName, " not set to zero.");
	}
}

void EEG_removeTriggers (EEG me, int which_Melder_STRING, const wchar_t *criterion) {
	try {
		if (my textgrid -> numberOfTiers () < 2 || ! Melder_wcsequ (my textgrid -> tier (2) -> name, L"Trigger"))
			Melder_throw (me, " does not have a Trigger channel.");
		my textgrid -> removePoints (2, which_Melder_STRING, criterion);
	} catch (MelderError) {
		Melder_throw (me, ": triggers not removed.");
	}
}

EEG EEG_extractChannel (EEG me, long channelNumber) {
	try {
		if (channelNumber < 1 || channelNumber > my numberOfChannels)
			Melder_throw ("No channel ", channelNumber, ".");
		autoEEG thee = EEG_create (my xmin, my xmax);
		thy numberOfChannels = 1;
		thy channelNames = NUMvector <wchar_t *> (1, 1);
		thy channelNames [1] = Melder_wcsdup (my channelNames [1]);
		thy sound = Sound_extractChannel (my sound, channelNumber);
		thy textgrid = Data_copy (my textgrid);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": channel ", channelNumber, " not extracted.");
	}
}

EEG EEG_extractChannel (EEG me, const wchar_t *channelName) {
	try {
		long channelNumber = EEG_getChannelNumber (me, channelName);
		if (channelNumber == 0)
			Melder_throw ("No channel named \"", channelName, "\".");
		return EEG_extractChannel (me, channelNumber);
	} catch (MelderError) {
		Melder_throw (me, ": channel ", channelName, " not extracted.");
	}
}

EEG EEGs_concatenate (Collection me) {
	try {
		if (my size < 1)
			Melder_throw ("Cannot concatenate zero EEG objects.");
		EEG first = (EEG) my item [1];
		long numberOfChannels = first -> numberOfChannels;
		wchar_t **channelNames = first -> channelNames;
		for (long ieeg = 2; ieeg <= my size; ieeg ++) {
			EEG other = (EEG) my item [ieeg];
			if (other -> numberOfChannels != numberOfChannels)
				Melder_throw ("The number of channels of ", other, " does not match the number of channels of ", first, ".");
			for (long ichan = 1; ichan <= numberOfChannels; ichan ++) {
				if (! Melder_wcsequ (other -> channelNames [ichan], channelNames [ichan]))
					Melder_throw ("Channel ", ichan, " has a different name in ", other, " (", other -> channelNames [ichan], ") than in ", first, " (", channelNames [ichan], ").");
			}
		}
		autoOrdered soundCollection = Ordered_create ();
		Collection_dontOwnItems (soundCollection.peek());
		autoOrdered textgridCollection = Ordered_create ();
		Collection_dontOwnItems (textgridCollection.peek());
		for (long ieeg = 1; ieeg <= my size; ieeg ++) {
			EEG eeg = (EEG) my item [ieeg];
			Collection_addItem (soundCollection.peek(), eeg -> sound);
			Collection_addItem (textgridCollection.peek(), eeg -> textgrid);
		}
		autoEEG thee = Thing_new (EEG);
		thy numberOfChannels = numberOfChannels;
		thy channelNames = NUMvector <wchar_t *> (1, numberOfChannels);
		for (long ichan = 1; ichan <= numberOfChannels; ichan ++) {
			thy channelNames [ichan] = Melder_wcsdup (channelNames [ichan]);
		}
		thy sound = Sounds_concatenate_e (soundCollection.peek(), 0.0);
		thy textgrid = TextGrids_concatenate (textgridCollection.peek());
		thy xmin = thy textgrid -> xmin;
		thy xmax = thy textgrid -> xmax;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrids not concatenated.");
	}
}

EEG EEG_extractPart (EEG me, double tmin, double tmax, bool preserveTimes) {
	try {
		autoEEG thee = Thing_new (EEG);
		thy numberOfChannels = my numberOfChannels;
		thy channelNames = NUMvector <wchar_t *> (1, my numberOfChannels);
		for (long ichan = 1; ichan <= my numberOfChannels; ichan ++) {
			thy channelNames [ichan] = Melder_wcsdup (my channelNames [ichan]);
		}
		thy sound = Sound_extractPart (my sound, tmin, tmax, kSound_windowShape_RECTANGULAR, 1.0, preserveTimes);
		thy textgrid = TextGrid_extractPart (my textgrid, tmin, tmax, preserveTimes);
		thy xmin = thy textgrid -> xmin;
		thy xmax = thy textgrid -> xmax;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": part not extracted.");
	}
}

void EEG_replaceTextGrid (EEG me, TextGrid textgrid) {
	try {
		autoTextGrid textgrid2 = Data_copy (textgrid);
		forget (my textgrid);
		my textgrid = textgrid2.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": TextGrid not replaced with ", textgrid, ".");
	}
}

MixingMatrix EEG_to_MixingMatrix (EEG me, long maxNumberOfIterations, double tol, int method) {
	try {
		autoCrossCorrelationTables tables = Sound_to_CrossCorrelationTables (my sound, 0.0, 0.0, 0.002, 1);
		autoMixingMatrix thee = MixingMatrix_create (my sound -> ny, my sound -> ny);
		for (long ichan = 1; ichan <= my numberOfChannels; ichan ++) {
			TableOfReal_setRowLabel (thee.peek(), ichan, my channelNames [ichan]);
			TableOfReal_setColumnLabel (thee.peek(), ichan, Melder_wcscat (L"ic", Melder_integer (ichan)));
		}
		MixingMatrix_and_CrossCorrelationTables_improveUnmixing (thee.peek(), tables.peek(), maxNumberOfIterations, tol, method);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MixingMatrix created.");
	}
}

/* End of file EEG.cpp */
