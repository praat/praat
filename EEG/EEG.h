#ifndef _EEG_h_
#define _EEG_h_
/* EEG.h
 *
 * Copyright (C) 2011-2012,2014-2018 Paul Boersma
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

#include "Sound.h"
#include "TextGrid.h"
#include "../dwtools/ICA.h"

#include "EEG_def.h"

autoEEG EEG_create (double tmin, double tmax);

autoEEG EEG_readFromBdfFile (MelderFile file);

autoEEG EEGs_concatenate (OrderedOf<structEEG>* me);

void EEG_init (EEG me, double tmin, double tmax);
integer EEG_getChannelNumber (EEG me, conststring32 channelName);
void EEG_setChannelName (EEG me, integer channelNumber, conststring32 newName);
static inline integer EEG_getNumberOfCapElectrodes (EEG me) {
	return (my numberOfChannels - 1) & ~ 15L;   // BUG
}
static inline integer EEG_getNumberOfExtraSensors (EEG me) {
	return my numberOfChannels == 1 ? 0 : my numberOfChannels & 1 ? 1 : 8;   // BUG
}
static inline integer EEG_getNumberOfExternalElectrodes (EEG me) {
	return my numberOfChannels - EEG_getNumberOfCapElectrodes (me) - EEG_getNumberOfExtraSensors (me);
}
void EEG_setExternalElectrodeNames (EEG me, conststring32 nameExg1, conststring32 nameExg2, conststring32 nameExg3, conststring32 nameExg4,
	conststring32 nameExg5, conststring32 nameExg6, conststring32 nameExg7, conststring32 nameExg8);
void EEG_detrend (EEG me);
void EEG_filter (EEG me, double lowFrequency, double lowWidth, double highFrequency, double highWidth, bool doNotch50Hz);
void EEG_subtractReference (EEG me, conststring32 channelName1, conststring32 channelName2);
void EEG_subtractMeanChannel (EEG me, integer fromChannel, integer toChannel);
void EEG_setChannelToZero (EEG me, integer channelNumber);
void EEG_setChannelToZero (EEG me, conststring32 channelName);
void EEG_removeTriggers (EEG me, kMelder_string which, conststring32 criterion);
autoEEG EEG_extractChannel (EEG me, integer channelNumber);
autoEEG EEG_extractChannel (EEG me, conststring32 channelName);
autoEEG EEG_extractChannels (EEG me, constVECVU const& channelNumbers);
void EEG_removeChannel (EEG me, integer channelNumber);
void EEG_removeChannel (EEG me, conststring32 channelName);
static inline autoSound EEG_extractSound (EEG me) { return Data_copy (my sound.get()); }
static inline autoTextGrid EEG_extractTextGrid (EEG me) { return Data_copy (my textgrid.get()); }
autoEEG EEG_extractPart (EEG me, double tmin, double tmax, bool preserveTimes);
void EEG_replaceTextGrid (EEG me, TextGrid textgrid);

autoMixingMatrix EEG_to_MixingMatrix (EEG me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep,
	integer maxNumberOfIterations, double tol, int method);

autoEEG EEG_MixingMatrix_to_EEG_unmix (EEG me, MixingMatrix you);
autoEEG EEG_MixingMatrix_to_EEG_mix (EEG me, MixingMatrix you);

/* End of file EEG.h */
#endif
