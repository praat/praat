/* EMA.cpp
 *
 * Copyright (C) 2023 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */


#include "EMA.h"
#include "melder.h"

#include "oo_DESTROY.h"
#include "EMA_def.h"
#include "oo_COPY.h"
#include "EMA_def.h"
#include "oo_EQUAL.h"
#include "EMA_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "EMA_def.h"
#include "oo_WRITE_TEXT.h"
#include "EMA_def.h"
#include "oo_WRITE_BINARY.h"
#include "EMA_def.h"
#include "oo_READ_TEXT.h"
#include "EMA_def.h"
#include "oo_READ_BINARY.h"
#include "EMA_def.h"
#include "oo_DESCRIPTION.h"
#include "EMA_def.h"

Thing_implement (EMA, Function, 0);

void structEMA :: v1_info () {
	structDaata :: v1_info ();
}

static void processLines12 (char *lines12, integer nbytes, integer *out_version, integer *out_headerSizeBytes) {
	try {
		/*
			First line is 15 chars long incl. \n: 'AG50xDATA_V00y\n'
			where y is '1', '2' or '3'
		*/
		const integer nchar1 = 15, nchar2 = 9;
		const integer length12 = nchar1 + nchar2;
		Melder_assert (nbytes >= length12);
		if (! strnequ (lines12, "AG50xDATA_V00", nchar1 - 2))
			Melder_throw (U"Not a Carstens AG50x file.");
		const integer version = lines12 [nchar1 - 2] - '0';
		Melder_require (version > 0 && version < 4,
			U"Unknown version number.");
		/*
			Second line is 9 bytes long incl. \n: the header size as a character string
		*/
		const integer lastIndex = length12 - 1;
		const char save = lines12 [lastIndex];
		lines12 [lastIndex] = '\0'; // temporary
		const integer headerSizeBytes = atoi (& lines12 [nchar1]);
		lines12 [lastIndex] = save; // restore
		if (out_version)
			*out_version = version;
		if (out_headerSizeBytes)
			*out_headerSizeBytes = headerSizeBytes;
	} catch (MelderError) {
		Melder_throw (U"lines 1 and 2 are not OK.");
	}
}

static char *processLines34 (char *buffer, integer bufferSize, integer *out_numberOfSensors, integer *out_samplingFrequencyHz) {
	try {
		/*
			Third line should start with the 17 characters 'NumberOfChannels=', followed by the number of channels/sensors (8, 16, or 24)
			followed by a newline
		*/
		char *pbuf = buffer, *pend = pbuf + bufferSize;
		const integer nchar3 = 17, nchar4 = 20;
		Melder_require (strnequ (pbuf, "NumberOfChannels=", nchar3),
			U"We expected 'NumberOfChannels='.");
		pbuf += nchar3;
		char *qbuf = pbuf;
		while (*qbuf ++ != '\n' && qbuf < pend);
		char save = *qbuf;
		*qbuf = '\0';
		const integer numberOfSensors = atoi (pbuf);
		*qbuf = save;
		Melder_require (numberOfSensors <= 24,
			U"The number of sensors (", numberOfSensors, U") should not exceed 24.");		
		/*
			Fourth line should start with the 20 characters 'SamplingFrequencyHz=' followed by a number < 10000 and a newline
		*/
		pbuf = qbuf; // skip the '\n'
		Melder_require (strnequ (pbuf, "SamplingFrequencyHz=", nchar4),
			U"We expected 'SamplingFrequencyHz='.");
		pbuf += nchar4;
		qbuf = pbuf;
		while (*qbuf ++ != '\n' && qbuf < pend);
		save = *qbuf;
		*qbuf = '\0';
		const integer samplingFrequency = atoi (pbuf);
		*qbuf = save;
		if (out_numberOfSensors)
			*out_numberOfSensors = numberOfSensors;
		if (out_samplingFrequencyHz)
			*out_samplingFrequencyHz = samplingFrequency;
		return qbuf;
	} catch (MelderError) {
		Melder_throw (U"lines 1 and 2 are not OK.");
	}
}

autoEMA EMA_readFromCarstensEMA50xPosFile (MelderFile file) {
	try {
		integer version, headerSizeBytes;
		autofile f = Melder_fopen (file, "rb");
		const integer nchar12 = 24;
		char lines12 [nchar12];
		(void) fread (lines12, 1, nchar12, f);
		processLines12 (lines12, nchar12, & version, & headerSizeBytes);
		const integer bufferSize = headerSizeBytes - nchar12;
		char buffer [bufferSize + 1];
		buffer [bufferSize] = 0; // just in case
		integer numberOfSensors, samplingFrequency;
		(void) fread (buffer, 1, bufferSize, f);
		(void) processLines34 (buffer, bufferSize, & numberOfSensors, & samplingFrequency);
		/*
			The data section
		*/
		const integer numberOfBytes = MelderFile_length (file);
		constexpr integer numberOfChannelsPerSensor = 7;
		constexpr integer numberOfBytesPerValue = 4;
		const integer sampleSizeBytes = numberOfSensors * numberOfChannelsPerSensor * numberOfBytesPerValue;
		const integer numberOfPosBytes = numberOfBytes - headerSizeBytes;
		const integer numberOfSamples = numberOfPosBytes / sampleSizeBytes;
		const double dt = 1.0 / samplingFrequency;
		const double duration = numberOfSamples * dt;
		autoEMA me = EMA_create (0.0, duration, numberOfSensors, numberOfChannelsPerSensor, numberOfSamples, dt, 0.5 * dt);
		
		for (integer isample = 1; isample <= numberOfSamples; isample ++)
			for (integer isensor = 1; isensor <= numberOfSensors; isensor ++) {
				Sensor sensor = my sensors.at [isensor];
				for (integer ichannel = 1; ichannel <= numberOfChannelsPerSensor; ichannel ++)
					sensor -> z [ichannel] [isample] = bingetr32LE (f);
			}
		
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Carstens EMA50x pos file not read.");
	}
}

autoEMAamp EMAamp_readFromCarstensEMA50xAmpFile (MelderFile file) {
	try {
		integer version, headerSizeBytes;
		autofile f = Melder_fopen (file, "rb");
		constexpr integer nchar12 = 24;
		char lines12 [nchar12];
		(void) fread (lines12, 1, nchar12, f);
		processLines12 (lines12, nchar12, & version, & headerSizeBytes);
		const integer bufferSize = headerSizeBytes - nchar12;
		char buffer [bufferSize + 1];   // TODO: no VLA
		buffer [bufferSize] = 0; // just in case
		char *pend = buffer + bufferSize - 1;
		integer numberOfSensors, samplingFrequency;
		(void) fread (buffer, 1, bufferSize, f);
		char *pbuf = processLines34 (buffer, bufferSize, & numberOfSensors, & samplingFrequency);
		/*
			The calibration part
		*/
		constexpr integer numberOfTransmitters = 9;
		const integer ncharCalf = 13;
		pbuf = strstr (pbuf, "Calf_Channel_");
		Melder_require (pbuf,
			U"The calibration part is missing.");
		/*
			Create the EMAamp
		*/
		const integer numberOfBytes = MelderFile_length (file);
		const integer numberOfAmpBytes = numberOfBytes - headerSizeBytes;
		constexpr integer numberOfBytesPerValue = 4;
		const integer sampleSizeBytes = numberOfSensors * numberOfTransmitters * numberOfBytesPerValue;
		const integer numberOfSamples = numberOfAmpBytes / sampleSizeBytes;
		const double dt = 1.0 / samplingFrequency;
		const double duration = numberOfSamples * dt;
		
		autoEMAamp me = EMAamp_create (0.0, duration, numberOfSensors, numberOfTransmitters, numberOfSamples, dt, 0.5 * dt);

		for (integer isensor = 1; isensor <= numberOfSensors; isensor ++) {
			Melder_require (strnequ (pbuf, "Calf_Channel_", ncharCalf),
				U"We expected 'Calf_Channel_'.");
			pbuf += ncharCalf; // start of channel number
			char *qbuf = pbuf;
			while (*qbuf ++ != '=' && qbuf < pend);
			*(--qbuf) = '\0';
			const integer isensorf = atoi (pbuf);
			Melder_require (isensorf == isensor - 1,
				U"Wrong sensor number ", isensorf, U")." );
			pbuf = qbuf + 3; // skip "=[ "
			for (integer itrans = 1; itrans <= numberOfTransmitters; itrans ++) {
				qbuf = pbuf;
				while (*qbuf ++ != ' ' && qbuf < pend);
				*(--qbuf) = '\0';
				my sensorCalibrations [isensor] [itrans] = atof (pbuf);
				pbuf = qbuf + 1;
			}
			pbuf += 2; // "]\n"
		}
		
		/*
			The data section
		*/
		for (integer isample = 1; isample <= numberOfSamples; isample ++)
			for (integer isensor = 1; isensor <= numberOfSensors; isensor ++) {
				Sensor sensor = my sensors.at [isensor];
				for (integer ichannel = 1; ichannel <= numberOfTransmitters; ichannel ++)
					sensor -> z [ichannel] [isample] =  bingetr32LE (f);
			}
		
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Carstens EMA50x amp file not read.");
	}
}

void EMA_init (EMA me, double tmin, double tmax, integer numberOfSensors, integer numberOfChannelsPerSensor, integer numberOfSamples, double dt, double t1) {
	my xmin = tmin;
	my xmax = tmax;
	my numberOfSensors = numberOfSensors;
	for (integer isensor = 1; isensor <= numberOfSensors; isensor ++) {
		autoSensor sensor = Sensor_create (numberOfChannelsPerSensor, tmin, tmax, numberOfSamples, dt, t1);
		my sensors.addItem_move (sensor.move());
	}
}

autoEMA EMA_create (double tmin, double tmax, integer numberOfSensors, integer numberOfChannelsPerSensor, integer numberOfSamples, double dt, double t1) {
	autoEMA me = Thing_new (EMA);
	EMA_init (me.get(), tmin, tmax, numberOfSensors, numberOfChannelsPerSensor, numberOfSamples, dt, t1);
	return me;
}

Thing_implement (EMAamp, EMA, 0);
	
void structEMAamp :: v1_info () {
	structDaata :: v1_info ();
}

autoEMAamp EMAamp_create (double tmin, double tmax, integer numberOfSensors, integer numberOfTransmitters, integer numberOfSamples, double dt, double t1) {
	autoEMAamp me = Thing_new (EMAamp);
	EMA_init (me.get(), tmin, tmax, numberOfSensors, numberOfTransmitters, numberOfSamples, dt, t1);
	my numberOfTransmitters = numberOfTransmitters;
	my sensorCalibrations = zero_MAT (numberOfSensors, numberOfTransmitters);
	return me;
}

/* End of file EMA.cpp */

