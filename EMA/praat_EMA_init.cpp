/* praat_EMA_init.cpp
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "Sensor.h"
#include "EMA.h"

static autoDaata CarstensEMA50xV3PosFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread > 69 && strnequ (header, "AG50xDATA_V00", 13) &&
		(strstr (header, "NumberOfChannels=") != nullptr) &&
		(strstr (header, "SamplingFrequencyHz=") != nullptr) &&
		(strstr (header , "Calf_Channel_0") == nullptr))
		return EMA_readFromCarstensEMA50xPosFile (file);
	else
		return autoDaata ();
}

static autoDaata CarstensEMA50xV3AmpFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread > 69 && strnequ (header, "AG50xDATA_V00", 13) &&
		(strstr (header, "NumberOfChannels=") != nullptr) && 
		(strstr (header, "SamplingFrequencyHz=") != nullptr) &&
		(strstr (header, "Calf_Channel_0") != nullptr))
		return EMAamp_readFromCarstensEMA50xAmpFile (file);
	else
		return autoDaata ();
}

void praat_EMA_init ();

void praat_EMA_init () {
	Thing_recognizeClassesByName (classSensor, classEMA, classEMAamp, nullptr);

	Data_recognizeFileType (CarstensEMA50xV3PosFileRecognizer);
	Data_recognizeFileType (CarstensEMA50xV3AmpFileRecognizer);

}

/* End of file praat_EMA_init.cpp */
