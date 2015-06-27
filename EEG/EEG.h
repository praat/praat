#ifndef _EEG_h_
#define _EEG_h_
/* EEG.h
 *
 * Copyright (C) 2011-2012,2014,2015 Paul Boersma
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

#include "Sound.h"
#include "TextGrid.h"
#include "../dwtools/ICA.h"

#include "EEG_def.h"
oo_CLASS_CREATE (EEG, Function);

EEG EEG_create (double tmin, double tmax);

EEG EEG_readFromBdfFile (MelderFile file);

EEG EEGs_concatenate (Collection me);

void EEG_init (EEG me, double tmin, double tmax);
long EEG_getChannelNumber (EEG me, const char32 *channelName);
void EEG_setChannelName (EEG me, long channelNumber, const char32 *a_name);
static inline long EEG_getNumberOfCapElectrodes (EEG me) {
	return (my numberOfChannels - 1) & ~ 15L;   // BUG
}
static inline long EEG_getNumberOfExtraSensors (EEG me) {
	return my numberOfChannels == 1 ? 0 : my numberOfChannels & 1 ? 1 : 8;   // BUG
}
static inline long EEG_getNumberOfExternalElectrodes (EEG me) {
	return my numberOfChannels - EEG_getNumberOfCapElectrodes (me) - EEG_getNumberOfExtraSensors (me);
}
void EEG_setExternalElectrodeNames (EEG me, const char32 *nameExg1, const char32 *nameExg2, const char32 *nameExg3, const char32 *nameExg4,
	const char32 *nameExg5, const char32 *nameExg6, const char32 *nameExg7, const char32 *nameExg8);
void EEG_detrend (EEG me);
void EEG_filter (EEG me, double lowFrequency, double lowWidth, double highFrequency, double highWidth, bool doNotch50Hz);
void EEG_subtractReference (EEG me, const char32 *channelNumber1, const char32 *channelNumber2);
void EEG_subtractMeanChannel (EEG me, long fromChannel, long toChannel);
void EEG_setChannelToZero (EEG me, long channelNumber);
void EEG_setChannelToZero (EEG me, const char32 *channelName);
void EEG_removeTriggers (EEG me, int which_Melder_STRING, const char32 *criterion);
EEG EEG_extractChannel (EEG me, long channelNumber);
EEG EEG_extractChannel (EEG me, const char32 *channelName);
static inline Sound EEG_extractSound (EEG me) { return Data_copy (my sound); }
static inline TextGrid EEG_extractTextGrid (EEG me) { return Data_copy (my textgrid); }
EEG EEG_extractPart (EEG me, double tmin, double tmax, bool preserveTimes);
void EEG_replaceTextGrid (EEG me, TextGrid textgrid);
MixingMatrix EEG_to_MixingMatrix (EEG me, long maxNumberOfIterations, double tol, int method);

/* End of file EEG.h */
#endif
