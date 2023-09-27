/* EMArawData.cpp
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
#include "EMArawData.h"
#include "melder.h"

#include "oo_DESTROY.h"
#include "EMArawData_def.h"
#include "oo_COPY.h"
#include "EMArawData_def.h"
#include "oo_EQUAL.h"
#include "EMArawData_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "EMArawData_def.h"
#include "oo_WRITE_TEXT.h"
#include "EMArawData_def.h"
#include "oo_WRITE_BINARY.h"
#include "EMArawData_def.h"
#include "oo_READ_TEXT.h"
#include "EMArawData_def.h"
#include "oo_READ_BINARY.h"
#include "EMArawData_def.h"
#include "oo_DESCRIPTION.h"
#include "EMArawData_def.h"

Thing_implement (EMArawData, Sampled, 0);

void structEMArawData :: v1_info () {
	structDaata :: v1_info ();
}

autoEMArawData EMArawData_readFromCarstensEMA50xAmpFile (MelderFile file) {
	try {
		integer version, headerSize, numberOfSensors, samplingFrequency, numberOfBytesInLines1234;
		CarstensEMA_processHeader (file, & version, & headerSize, & numberOfSensors, & samplingFrequency, & numberOfBytesInLines1234);
		(void) MelderFile_open (file);
		/*
			We have to read the sensor calibration data which are located in the header part.
			1. Read the complete header and find the start of the calibration part
		*/
		autostring8 header (headerSize, false);
		size_t nread = fread (& header [0], 1, headerSize, file -> filePointer);
		Melder_require (nread == headerSize,
			U"Header size does not match its specification.");
		header [headerSize - 1] = 0; // just in case
		char *pCalf = & header [0] + numberOfBytesInLines1234;
		pCalf = strstr (pCalf, "Calf_Channel_");
		Melder_require (pCalf,
			U"The calibration part is missing.");
		/*
			2. Create the EMArawData object
		*/
		const integer numberOfBytes = MelderFile_length (file);
		const integer numberOfAmpBytes = numberOfBytes - headerSize;
		constexpr integer numberOfBytesPerValue = 4;
		constexpr integer numberOfTransmitters = 9;
		const integer frameSize = numberOfSensors * numberOfTransmitters * numberOfBytesPerValue;
		const integer numberOfFrames = numberOfAmpBytes / frameSize;
		const double dt = 1.0 / samplingFrequency;
		const double duration = numberOfFrames * dt;
		
		autoEMArawData me = EMArawData_create (0.0, duration, numberOfSensors, numberOfTransmitters, numberOfFrames, dt, 0.5 * dt);
		/*
			3. Get the calibration matrix (numberOfSensors x numberOfTransmitters)
		
		*/
		constexpr integer ncharCalf = 13;
		char *headerEnd = & header [0] + headerSize - 1;
		for (integer isensor = 1; isensor <= numberOfSensors; isensor ++) {
			Melder_require (strnequ (pCalf, "Calf_Channel_", ncharCalf),
				U"We expected 'Calf_Channel_'.");
			pCalf += ncharCalf; // start of channel number
			char *qCalf = pCalf;
			while (*qCalf ++ != '=' && qCalf < headerEnd);
			*(--qCalf) = '\0';
			const integer isensorf = atoi (pCalf);
			Melder_require (isensorf == isensor - 1,
				U"Wrong sensor number ", isensorf, U")." );
			pCalf = qCalf + 3; // skip "=[ "
			for (integer itrans = 1; itrans <= numberOfTransmitters; itrans ++) {
				qCalf = pCalf;
				while (*qCalf ++ != ' ' && qCalf < headerEnd);
				*(--qCalf) = '\0';
				my sensorCalibrations [isensor] [itrans] = atof (pCalf);
				pCalf = qCalf + 1;
			}
			pCalf += 2; // "]\n"
		}
		/*
			4. Get the amplitude data.
		*/
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			EMArawData_Frame emaRawDataFrame = & my emaRawDataFrames [iframe];
			emaRawDataFrame -> numberOfSensors = numberOfSensors;
			emaRawDataFrame -> transmitterFrames = newvectorzero<structEMATransmitter_Frame> (numberOfSensors);
			for (integer isensor = 1; isensor <= numberOfSensors; isensor ++) {
				EMATransmitter_Frame transmitterFrame = & (emaRawDataFrame -> transmitterFrames [isensor]);
				transmitterFrame -> numberOfTransmitters = numberOfTransmitters;
				transmitterFrame -> amplitudes = zero_VEC (numberOfTransmitters);
				for (integer itrans = 1; itrans <= numberOfTransmitters; itrans ++)
					transmitterFrame -> amplitudes [itrans] = bingetr32LE (file -> filePointer);
			}
		}
		
		MelderFile_close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Carstens EMArawData50x amp file not read.");
	}
}




void EMArawData_init (EMArawData me, double tmin, double tmax, integer numberOfSensors, integer numberOfTransmitters, integer numberOfFrames, double dt, double t1) {
	Sampled_init (me, tmin, tmax, numberOfFrames, dt, t1);
	my numberOfSensors = numberOfSensors;
	my emaRawDataFrames = newvectorzero <structEMArawData_Frame> (numberOfFrames);
	my sensorCalibrations = zero_MAT (numberOfSensors, numberOfTransmitters);
}

autoEMArawData EMArawData_create (double tmin, double tmax, integer numberOfSensors, integer numberOfTransmitters, integer numberOfFrames, double dt, double t1) {
	autoEMArawData me = Thing_new (EMArawData);
	my numberOfSensors = numberOfSensors;
	my numberOfTransmitters = numberOfTransmitters;
	EMArawData_init (me.get(), tmin, tmax, numberOfSensors, numberOfTransmitters, numberOfFrames, dt, t1);
	return me;
}

/* End of file EMArawData.cpp */

