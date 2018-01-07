/* EEG_extensions.cpp
 *
 * Copyright (C) 2012-2017 David Weenink, 2015,2017 Paul Boersma
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


#include "ICA.h"
#include "EEG_extensions.h"
#include "NUM2.h"
#include "Sound_and_PCA.h"
#include "Sound_extensions.h"
#include "Spectrum_extensions.h"
#include "Sound_and_MixingMatrix.h"
#include "Sound_and_Spectrum.h"

static autoEEG EEG_copyWithoutSound (EEG me) {
	try {
 		autoEEG thee = EEG_create (my xmin, my xmax);
		thy numberOfChannels = my numberOfChannels;
		thy textgrid = Data_copy (my textgrid.get());
		autostring32vector channelNames (1, my numberOfChannels);
		for (integer i = 1; i <= my numberOfChannels; i ++) {
			channelNames [i] = Melder_dup (my channelNames [i]);
		}
		thy channelNames = channelNames.transfer();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not copied.");
	}
}

static integer *EEG_channelNames_to_channelNumbers (EEG me, char32 **channelNames, integer numberOfChannelNames) {
	try {
		autoNUMvector<integer> channelNumbers (1, numberOfChannelNames);
		for (integer i = 1; i <= numberOfChannelNames; i ++) {
			for (integer j = 1; j <= my numberOfChannels; j ++) {
				if (Melder_equ (channelNames [i], my channelNames [j])) {
					channelNumbers [i] = j;
				}
			}
			if (channelNumbers [i] == 0) {
				Melder_throw (U"Channel name \"", channelNames [i], U"\" not found.");
			}
		}
		return channelNumbers.transfer();
	} catch (MelderError) {
		Melder_throw (me, U": channelNames not found.");
	}
}

static void EEG_setChannelNames_selected (EEG me, const char32 *precursor, integer *channelNumbers, integer numberOfChannels) {
	autoMelderString name;
	const char32 *zero = U"0";
	for (integer i = 1; i <= numberOfChannels; i ++) {
		MelderString_copy (&name, precursor);
		if (my numberOfChannels > 100) {
			if (i < 10) {
				MelderString_append (& name, zero);
			}
			if (i < 100) {
				MelderString_append (& name, zero);
			}
		} else if (i < 10) {
			MelderString_append (& name, zero);
		}
		MelderString_append (& name, i);
		EEG_setChannelName (me, channelNumbers [i], name.string);
	}
}

autoCrossCorrelationTable EEG_to_CrossCorrelationTable (EEG me, double startTime, double endTime, double lagStep, const char32 *channelRanges)
{
	try {
		// autowindow
		if (startTime == endTime) {
			startTime = my xmin; endTime = my xmax;
		}
		// don't allow times outside domain
		if (startTime < my xmin) {
			startTime = my xmin;
		}
		if (endTime > my xmax) {
			endTime = my xmax;
		}
		autoEEG thee = EEG_extractPart (me, startTime, endTime, true);
		integer numberOfChannels;
		autoNUMvector <integer> channels (NUMstring_getElementsOfRanges (channelRanges, thy numberOfChannels, & numberOfChannels, nullptr, U"channel", true), 1);
		autoSound soundPart = Sound_copyChannelRanges (thy sound.get(), channelRanges);
		autoCrossCorrelationTable him = Sound_to_CrossCorrelationTable (soundPart.get(), startTime, endTime, lagStep);
		// assign channel names
		for (integer i = 1; i <= numberOfChannels; i ++) {
			integer ichannel = channels [i];
			char32 *label = my channelNames [ichannel];
			TableOfReal_setRowLabel (him.get(), i, label);
			TableOfReal_setColumnLabel (him.get(), i, label);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no CrossCorrelationTable calculated.");
	}
}

autoCovariance EEG_to_Covariance (EEG me, double startTime, double endTime, const char32 *channelRanges)
{
	try {
		double lagStep = 0.0;
		autoCrossCorrelationTable thee = EEG_to_CrossCorrelationTable (me, startTime, endTime, lagStep, channelRanges);
        autoCovariance him = Thing_new (Covariance);
        thy structCrossCorrelationTable :: v_copy (him.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance calculated.");
	}
}

autoCrossCorrelationTableList EEG_to_CrossCorrelationTableList (EEG me, double startTime, double endTime, double lagStep, integer ncovars, const char32 *channelRanges) {
	try {
		// autowindow
		if (startTime == endTime) {
			startTime = my xmin; endTime = my xmax;
		}
		// don't allow times outside domain
		if (startTime < my xmin) {
			startTime = my xmin;
		}
		if (endTime > my xmax) {
			endTime = my xmax;
		}
		autoEEG thee = EEG_extractPart (me, startTime, endTime, true);
		integer numberOfChannels;
		autoNUMvector <integer> channels (NUMstring_getElementsOfRanges (channelRanges, thy numberOfChannels, & numberOfChannels, nullptr, U"channel", true), 1);
		autoSound soundPart = Sound_copyChannelRanges (thy sound.get(), channelRanges);
		autoCrossCorrelationTableList him = Sound_to_CrossCorrelationTableList (soundPart.get(), startTime, endTime, lagStep, ncovars);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no CrossCorrelationTables calculated.");
	}
}

autoPCA EEG_to_PCA (EEG me, double startTime, double endTime, const char32 *channelRanges, int fromCorrelation) {
	try {
		autoCovariance cov = EEG_to_Covariance (me, startTime, endTime, channelRanges);
		autoPCA him;
		if (fromCorrelation) {
			autoCorrelation cor = SSCP_to_Correlation (cov.get());
			him = SSCP_to_PCA (cor.get());
		} else {
			him = SSCP_to_PCA (cov.get());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no PCA calculated.");
	}
}

autoEEG EEG_PCA_to_EEG_whiten (EEG me, PCA thee, integer numberOfComponents) {
	try {
		if (numberOfComponents <= 0 || numberOfComponents > thy numberOfEigenvalues) {
			numberOfComponents = thy numberOfEigenvalues;
		}
		numberOfComponents = ( numberOfComponents > my numberOfChannels ? my numberOfChannels : numberOfComponents );

		autoNUMvector <integer> channelNumbers (EEG_channelNames_to_channelNumbers (me, thy labels, thy dimension), 1);

		autoEEG him = Data_copy (me);
		autoSound white = Sound_PCA_whitenSelectedChannels (my sound.get(), thee, numberOfComponents, channelNumbers.peek(), thy dimension);
		for (integer i = 1; i <= thy dimension; i ++) {
			integer ichannel = channelNumbers [i];
			NUMvector_copyElements<double> (white -> z [i], his sound -> z [ichannel], 1, his sound -> nx);
		}
		EEG_setChannelNames_selected (him.get(), U"wh", channelNumbers.peek(), thy dimension);
		return him;
	} catch(MelderError) {
		Melder_throw (me, U": not whitened with ", thee);
	}
}

autoEEG EEG_PCA_to_EEG_principalComponents (EEG me, PCA thee, integer numberOfComponents) {
	try {
		if (numberOfComponents <= 0 || numberOfComponents > thy numberOfEigenvalues) {
			numberOfComponents = thy numberOfEigenvalues;
		}
		numberOfComponents = numberOfComponents > my numberOfChannels ? my numberOfChannels : numberOfComponents;

		autoNUMvector <integer> channelNumbers (EEG_channelNames_to_channelNumbers (me, thy labels, thy dimension), 1);
		autoEEG him = Data_copy (me);
		autoSound pc = Sound_PCA_to_Sound_pc_selectedChannels (my sound.get(), thee, numberOfComponents, channelNumbers.peek(), thy dimension);
		for (integer i = 1; i <= thy dimension; i ++) {
			integer ichannel = channelNumbers [i];
			NUMvector_copyElements<double> (pc -> z [i], his sound -> z [ichannel], 1, his sound -> nx);
		}
		EEG_setChannelNames_selected (him.get(), U"pc", channelNumbers.peek(), thy dimension);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not projected.");
	}
}

autoEEG EEG_to_EEG_bss (EEG me, double startTime, double endTime, integer ncovars, double lagStep, const char32 *channelRanges, int whiteningMethod, int diagonalizerMethod, integer maxNumberOfIterations, double tol) {
	try {
		// autowindow
		if (startTime == endTime) {
			startTime = my xmin; endTime = my xmax;
		}
		// don't allow times outside domain
		if (startTime < my xmin) {
			startTime = my xmin;
		}
		if (endTime > my xmax) {
			endTime = my xmax;
		}
		integer numberOfChannels;
		autoNUMvector <integer> channelNumbers (NUMstring_getElementsOfRanges (channelRanges, my numberOfChannels, & numberOfChannels, nullptr, U"channel", true), 1);
		autoEEG thee = EEG_extractPart (me, startTime, endTime, true);
		if (whiteningMethod != 0) {
			bool fromCorrelation = ( whiteningMethod == 2 );
			autoPCA pca = EEG_to_PCA (thee.get(), thy xmin, thy xmax, channelRanges, fromCorrelation);
			autoEEG white = EEG_PCA_to_EEG_whiten (thee.get(), pca.get(), 0);
			thee = white.move();
		}
		autoMixingMatrix mm = Sound_to_MixingMatrix (thy sound.get(), startTime, endTime, ncovars, lagStep, maxNumberOfIterations, tol, diagonalizerMethod);

		autoEEG him = EEG_copyWithoutSound (me);
		his sound = Sound_MixingMatrix_unmix (my sound.get(), mm.get());
		EEG_setChannelNames_selected (him.get(), U"ic", channelNumbers.peek(), numberOfChannels);

		// Calculate the cross-correlations between eye-channels and the ic's

		return him;

	} catch (MelderError) {
		Melder_throw (me, U": no independent components determined.");
	}
}

autoSound EEG_to_Sound_modulated (EEG me, double baseFrequency, double channelBandwidth, const char32 *channelRanges) {
	try {
		integer numberOfChannels;
		autoNUMvector <integer> channelNumbers (NUMstring_getElementsOfRanges (channelRanges, my numberOfChannels, & numberOfChannels, nullptr, U"channel", true), 1);
		double maxFreq = baseFrequency + my numberOfChannels * channelBandwidth;
		double samplingFrequency = 2.0 * maxFreq;
		samplingFrequency = samplingFrequency < 44100.0 ? 44100.0 : samplingFrequency;
		autoSound thee = Sound_createSimple (1, my xmax - my xmin, samplingFrequency);
		for (integer i = 1; i <= numberOfChannels; i ++) {
			integer ichannel = channelNumbers [i];
			double fbase = baseFrequency;// + (ichannel - 1) * channelBandwidth;
			autoSound si = Sound_extractChannel (my sound.get(), ichannel);
			autoSpectrum spi = Sound_to_Spectrum (si.get(), 1);
			Spectrum_passHannBand (spi.get(), 0.5, channelBandwidth - 0.5, 0.5);
			autoSpectrum spi_shifted = Spectrum_shiftFrequencies (spi.get(), fbase, samplingFrequency / 2.0, 30);
			autoSound resampled = Spectrum_to_Sound (spi_shifted.get());
			integer nx = resampled -> nx < thy nx ? resampled -> nx : thy nx;
			for (integer j = 1; j <= nx; j ++) {
				thy z [1] [j] += resampled -> z [1] [j];
			}
		}
		Vector_scale (thee.get(), 0.99);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no playable sound created.");
	}
}

autoSound EEG_to_Sound_frequencyShifted (EEG me, integer channel, double frequencyShift, double samplingFrequency, double maxAmp) {
	try {
		autoSound si = Sound_extractChannel (my sound.get(), channel);
		autoSpectrum spi = Sound_to_Spectrum (si.get(), 1);
		autoSpectrum spi_shifted = Spectrum_shiftFrequencies (spi.get(), frequencyShift, samplingFrequency / 2.0, 30);
		autoSound thee = Spectrum_to_Sound (spi_shifted.get());
		if (maxAmp > 0) {
			Vector_scale (thee.get(), maxAmp);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": channel not converted to sound.");
	}
}

/* End of file EEG_extensions.cpp */
