#ifndef _EEG_extensions_h_
#define _EEG_extensions_h_
/* EEG_extensions.h
 *
 * Copyright (C) 2012-2017 David Weenink, 2018 Paul Boersma
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

#include "Covariance.h"
#include "ICA.h"
#include "EEG.h"

autoCrossCorrelationTable EEG_to_CrossCorrelationTable (EEG me,
	double startTime, double endTime, double lagTime,
	conststring32 channelRanges);

autoCovariance EEG_to_Covariance (EEG me, double startTime, double endTime, conststring32 channelRanges);

autoCrossCorrelationTableList EEG_to_CrossCorrelationTableList (EEG me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep,
	conststring32 channelRanges);

autoPCA EEG_to_PCA (EEG me, double startTime, double endTime, conststring32 channelRanges, int fromCorrelation);

autoEEG EEG_PCA_to_EEG_whiten (EEG me, PCA thee, integer numberOfComponents);

autoEEG EEG_PCA_to_EEG_principalComponents (EEG me, PCA thee, integer numberOfComponents);

void EEG_to_EEG_bss (EEG me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep,
	conststring32 channelRanges,
	int whiteningMethod, int diagonalizerMethod, integer maxNumberOfIterations, double tol,
	autoEEG *p_resultingEEG, autoMixingMatrix *p_resultingMixingMatrix);

autoSound EEG_to_Sound_frequencyShifted (EEG me, integer channel, double frequencyShift, double samplingFrequency, double maxAmp);

autoSound EEG_to_Sound_modulated (EEG me, double baseFrequency, double channelBandWidth, conststring32 channelRanges);

/* End of file EEG_extensions.h */
#endif
