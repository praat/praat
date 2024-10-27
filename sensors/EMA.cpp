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

Thing_implement (EMA, Sampled, 0);

void structEMA :: v1_info () {
	structDaata :: v1_info ();
}

void CarstensEMA_processHeader (MelderFile file, integer *out_version, integer *out_headerSizeBytes, 
	integer *out_numberOfSensors, integer *out_samplingFrequencyHz, integer *out_numberOfBytesInLines1234)
{
	try {
		file -> filePointer = Melder_fopen (file, "r");
		file -> openForReading = true;
		integer numberOfBytesRead = 0;
		/*
			First line is 15 chars long incl. \n: 'AG50xDATA_V00y\n'
			where y is '1', '2' or '3'
		*/
		constexpr integer nchar1 = 14;
		char *line1 = MelderFile_readLine8 (file);
		const integer length1 = strlen (line1);
		Melder_require (length1 == nchar1,
			U"Line 1 should have ", nchar1, U" characters and not ", length1, U".");
		numberOfBytesRead += nchar1 + 1;   // + 15: end of line is always '\n'
		if (! strnequ (line1, "AG50xDATA_V00", nchar1 - 1))
			Melder_throw (U"Not a Carstens AG50x file.");
		const integer version = line1 [nchar1 - 1] - '0';
		Melder_require (version > 0 && version < 4,
			U"Unknown version number.");
		/*
			Second line is 9 bytes long incl. \n: the header size as a character string
		*/
		constexpr integer nchar2 = 8;
		char *line2 = MelderFile_readLine8 (file);
		const integer length2 = strlen (line2);
		Melder_require (length2 == nchar2,
			U"Line 2 should have ", nchar2, U" characters and not ", length2, U".");
		numberOfBytesRead += nchar2 + 1;   // + 9: end of line is always '\n'
		const integer headerSizeBytes = atoi (line2);
		/*
			Third line should start with the 17 characters 'NumberOfChannels=', followed by the number of channels/sensors
			(8, 16, or 24) followed by a newline
		*/
		constexpr integer nchar3min = 17;
		char *line3 = MelderFile_readLine8 (file);
		const integer nchar3 = strlen (line3);
		Melder_require (nchar3min < nchar3,
			U"Line 3 should at least have ", nchar3min, U" characters and not ", nchar3, U".");
		Melder_require (strnequ (line3, "NumberOfChannels=", nchar3min),
			U"We expected 'NumberOfChannels='.");
		const integer numberOfSensors = atoi (& line3 [nchar3min]);
		Melder_require (numberOfSensors <= 24,
			U"The number of sensors (", numberOfSensors, U") should not exceed 24.");
		numberOfBytesRead += nchar3 + 1;   // + 19/20: end of line is always '\n'
		/*
			Fourth line should start with the 20 characters 'SamplingFrequencyHz=' followed by a number < 10000 and a newline
		*/
		constexpr integer nchar4min = 20;
		char *line4 = MelderFile_readLine8 (file);
		const integer nchar4 = strlen (line4);
		Melder_require (nchar4min < nchar4,
			U"Line 4 should at least have ", nchar4min, U" characters and not ", nchar4, U".");
		Melder_require (strnequ (line4, "SamplingFrequencyHz=", nchar4min),
			U"We expected 'SamplingFrequencyHz='.");
		const integer samplingFrequency = atoi (& line4 [nchar4min]);
		numberOfBytesRead += nchar4 + 1;   // + 22/23/24: 
		MelderFile_close (file);
		if (out_version)
			*out_version = version;
		if (out_headerSizeBytes)
			*out_headerSizeBytes = headerSizeBytes;
		if (out_numberOfSensors)
			*out_numberOfSensors = numberOfSensors;
		if (out_samplingFrequencyHz)
			*out_samplingFrequencyHz = samplingFrequency;
		if (out_numberOfBytesInLines1234)
			*out_numberOfBytesInLines1234 = numberOfBytesRead;
	} catch (MelderError) {
		Melder_throw (U"Cannot read the common header part.");
	}
}


autoEMA EMA_readFromCarstensEMA50xPosFile (MelderFile file) {
	try {
		integer version, headerSizeBytes, numberOfSensors, samplingFrequency, numberOfBytesInLines1234;
		CarstensEMA_processHeader (file, & version, & headerSizeBytes, & numberOfSensors, & samplingFrequency, & numberOfBytesInLines1234);
		(void) MelderFile_open (file);
		/*
			Skip the header: we have all the info needed to read the position data.
		*/
		MelderFile_seek (file, headerSizeBytes, 0);
		
		/*
			The data section
		*/
		const integer numberOfBytes = MelderFile_length (file);
		constexpr integer numberOfChannelsPerSensor = 7;
		constexpr integer numberOfBytesPerValue = 4;
		const integer frameSizeBytes = numberOfSensors * numberOfChannelsPerSensor * numberOfBytesPerValue;
		const integer numberOfPosBytes = numberOfBytes - headerSizeBytes;
		const integer numberOfFrames = numberOfPosBytes / frameSizeBytes;
		const double dt = 1.0 / samplingFrequency;
		const double duration = numberOfFrames * dt;
		autoEMA me = EMA_create (0.0, duration, numberOfSensors, numberOfFrames, dt, 0.5 * dt);
		
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			EMA_Frame emaFrame = & my emaFrames [iframe];
			emaFrame -> numberOfSensors = numberOfSensors;
			emaFrame -> sensorFrames = newvectorzero<structEMAsensor_Frame> (numberOfSensors);
			for (integer isensor = 1; isensor <= numberOfSensors; isensor ++) {
				EMAsensor_Frame sensorFrame = & (emaFrame -> sensorFrames [isensor]);
				sensorFrame -> x = bingetr32LE (file -> filePointer);
				sensorFrame -> y = bingetr32LE (file -> filePointer);
				sensorFrame -> z = bingetr32LE (file -> filePointer);
				sensorFrame -> phi = bingetr32LE (file -> filePointer);
				sensorFrame -> theta = bingetr32LE (file -> filePointer);
				(void) bingetr32LE (file -> filePointer); // rms
				(void) bingetr32LE (file -> filePointer); // extra
			}
		}
		MelderFile_close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Carstens EMA50x pos file not read.");
	}
}

void EMA_init (EMA me, double tmin, double tmax, integer numberOfSensors, integer numberOfFrames, double dt, double t1) {
	Sampled_init (me, tmin, tmax, numberOfFrames, dt, t1);
	my numberOfSensors = numberOfSensors;
	my emaFrames = newvectorzero <structEMA_Frame> (numberOfFrames);
}

autoEMA EMA_create (double tmin, double tmax, integer numberOfSensors, integer numberOfFrames, double dt, double t1) {
	autoEMA me = Thing_new (EMA);
	EMA_init (me.get(), tmin, tmax, numberOfSensors, numberOfFrames, dt, t1);
	return me;
}

/* End of file EMA.cpp */

