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
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			fread (buffer, 1, 16, f); buffer [16] = '\0';   // labels of the channels
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
		his d_sound = me.transfer();
		his d_textgrid = thee.transfer();
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("BDF file not read.");
	}
}

/* End of file EEG.cpp */
