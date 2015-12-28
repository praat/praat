#ifndef _EEG_extensions_h_
#define _EEG_extensions_h_
/* EEG_extensions.h
 *
 * Copyright (C) 2012, 2015 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20120926 Initial
*/


#include "ICA.h"
#include "EEG.h"

autoCrossCorrelationTable EEG_to_CrossCorrelationTable (EEG me, double startTime, double endTime, double lagTime, const char32 *channelRanges);

autoCovariance EEG_to_Covariance (EEG me, double startTime, double endTime, const char32 *channelRanges);

autoCrossCorrelationTableList EEG_to_CrossCorrelationTableList (EEG me, double startTime, double endTime, double lagTime, long ncovars, const char32 *channelRanges);

autoPCA EEG_to_PCA (EEG me, double startTime, double endTime, const char32 *channelRanges, int fromCorrelation);

autoEEG EEG_and_PCA_to_EEG_whiten (EEG me, PCA thee, long numberOfComponents);

autoEEG EEG_and_PCA_to_EEG_principalComponents (EEG me, PCA thee, long numberOfComponents);

autoEEG EEG_to_EEG_bss (EEG me, double startTime, double endTime, long ncovars, double lagTime, const char32 *channelRanges, int whiteningMethod, int diagonalizerMethod, long maxNumberOfIterations, double tol);

autoSound EEG_to_Sound_frequencyShifted (EEG me, long channel, double frequencyShift, double samplingFrequency, double maxAmp);

autoSound EEG_to_Sound_modulated (EEG me, double baseFrequency, double channelBandWidth, const char32 *channelRanges);

#endif /* _EEG_extensions_h_ */
