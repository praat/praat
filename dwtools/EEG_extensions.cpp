/* EEG_extensions.cpp
 *
 * Copyright (C) 2012-2019 David Weenink, 2015,2017,2018 Paul Boersma
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

#include "Correlation.h"
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
		thy channelNames = newSTRVECcopy (my channelNames.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not copied.");
	}
}

static autoINTVEC EEG_channelNames_to_channelNumbers (EEG me, constSTRVEC const& channelNames) {
	try {
		autoINTVEC channelNumbers = zero_INTVEC (channelNames.size);
		for (integer i = 1; i <= channelNames.size; i ++) {
			for (integer j = 1; j <= my numberOfChannels; j ++)
				if (Melder_equ (channelNames [i], my channelNames [j].get()))
					channelNumbers [i] = j;
			Melder_require (channelNumbers [i] != 0,
				U"Channel name \"", channelNames [i], U"\" not found.");
		}
		return channelNumbers;
	} catch (MelderError) {
		Melder_throw (me, U": channelNames not found.");
	}
}

static void EEG_setChannelNames_selected (EEG me, conststring32 precursor, constINTVEC const& channelNumbers) {
	autoMelderString name;
	const conststring32 zero = U"0";
	for (integer i = 1; i <= channelNumbers.size; i ++) {
		MelderString_copy (& name, precursor);
		if (my numberOfChannels > 100) {
			if (i < 10)
				MelderString_append (& name, zero);
			if (i < 100)
				MelderString_append (& name, zero);
		} else if (i < 10) {
			MelderString_append (& name, zero);
		}
		MelderString_append (& name, i);
		EEG_setChannelName (me, channelNumbers [i], name.string);
	}
}

autoCrossCorrelationTable EEG_to_CrossCorrelationTable (EEG me, double startTime, double endTime, double lagStep, conststring32 channelRanges)
{
	try {
		// autowindow
		if (startTime == endTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		// don't allow times outside domain
		if (startTime < my xmin) {
			startTime = my xmin;
		}
		if (endTime > my xmax) {
			endTime = my xmax;
		}
		autoEEG thee = EEG_extractPart (me, startTime, endTime, true);
		autoINTVEC channels = NUMstring_getElementsOfRanges (channelRanges, thy numberOfChannels, U"channel", true);
		autoSound soundPart = Sound_copyChannelRanges (thy sound.get(), channelRanges);
		autoCrossCorrelationTable him = Sound_to_CrossCorrelationTable (soundPart.get(), startTime, endTime, lagStep);
		// assign channel names
		for (integer i = 1; i <= channels.size; i ++) {
			const integer ichannel = channels [i];
			const conststring32 label = my channelNames [ichannel].get();
			TableOfReal_setRowLabel (him.get(), i, label);
			TableOfReal_setColumnLabel (him.get(), i, label);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no CrossCorrelationTable calculated.");
	}
}

autoCovariance EEG_to_Covariance (EEG me, double startTime, double endTime, conststring32 channelRanges)
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

autoCrossCorrelationTableList EEG_to_CrossCorrelationTableList (EEG me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep, conststring32 channelRanges)
{
	try {
		// autowindow
		if (startTime == endTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		// don't allow times outside domain
		if (startTime < my xmin) {
			startTime = my xmin;
		}
		if (endTime > my xmax) {
			endTime = my xmax;
		}
		autoEEG thee = EEG_extractPart (me, startTime, endTime, true);
		autoINTVEC channels = NUMstring_getElementsOfRanges (channelRanges, thy numberOfChannels, U"channel", true);
		autoSound soundPart = Sound_copyChannelRanges (thy sound.get(), channelRanges);
		autoCrossCorrelationTableList him = Sound_to_CrossCorrelationTableList (soundPart.get(),
			startTime, endTime, numberOfCrossCorrelations, lagStep);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no CrossCorrelationTables calculated.");
	}
}

autoPCA EEG_to_PCA (EEG me, double startTime, double endTime, conststring32 channelRanges, int fromCorrelation) {
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
		if (numberOfComponents <= 0 || numberOfComponents > thy numberOfEigenvalues)
			numberOfComponents = thy numberOfEigenvalues;
		numberOfComponents = std::min (numberOfComponents, my numberOfChannels);

		Melder_assert (thy labels.size == thy dimension);
		autoINTVEC channelNumbers = EEG_channelNames_to_channelNumbers (me, thy labels.get());

		autoEEG him = Data_copy (me);
		autoSound white = Sound_PCA_whitenSelectedChannels (my sound.get(), thee, numberOfComponents, channelNumbers.get());
		for (integer i = 1; i <= channelNumbers.size; i ++)
			his sound -> z.row (channelNumbers [i]) <<= white -> z.row (i);

		EEG_setChannelNames_selected (him.get(), U"wh", channelNumbers.get());
		return him;
	} catch(MelderError) {
		Melder_throw (me, U": not whitened with ", thee);
	}
}

autoEEG EEG_PCA_to_EEG_principalComponents (EEG me, PCA thee, integer numberOfComponents) {
	try {
		if (numberOfComponents <= 0 || numberOfComponents > thy numberOfEigenvalues)
			numberOfComponents = thy numberOfEigenvalues;
		numberOfComponents = std::min (numberOfComponents, my numberOfChannels);

		Melder_assert (thy labels.size == thy dimension);
		autoINTVEC channelNumbers = EEG_channelNames_to_channelNumbers (me, thy labels.get());
		autoEEG him = Data_copy (me);
		autoSound pc = Sound_PCA_to_Sound_pc_selectedChannels (my sound.get(), thee, numberOfComponents, channelNumbers.get());
		for (integer i = 1; i <= channelNumbers.size; i ++)
			his sound -> z.row (channelNumbers [i]) <<= pc -> z.row (i);
		
		EEG_setChannelNames_selected (him.get(), U"pc", channelNumbers.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not projected.");
	}
}

void EEG_to_EEG_bss (EEG me, double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep, conststring32 channelRanges,
	int whiteningMethod, int diagonalizerMethod, integer maxNumberOfIterations, double tol,
	autoEEG *p_resultingEEG, autoMixingMatrix *p_resultingMixingMatrix)
{
	try {
		// autowindow
		if (startTime == endTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		// don't allow times outside domain
		if (startTime < my xmin)
			startTime = my xmin;

		if (endTime > my xmax)
			endTime = my xmax;

		autoINTVEC channelNumbers = NUMstring_getElementsOfRanges (channelRanges, my numberOfChannels, U"channel", true);
		autoEEG thee = EEG_extractPart (me, startTime, endTime, true);
		if (whiteningMethod != 0) {
			const bool fromCorrelation = ( whiteningMethod == 2 );
			autoPCA pca = EEG_to_PCA (thee.get(), thy xmin, thy xmax, channelRanges, fromCorrelation);
			autoEEG white = EEG_PCA_to_EEG_whiten (thee.get(), pca.get(), 0);
			thee = white.move();
		}
		autoMixingMatrix mm = Sound_to_MixingMatrix (thy sound.get(),
			startTime, endTime, numberOfCrossCorrelations, lagStep,
			maxNumberOfIterations, tol, diagonalizerMethod);

		autoEEG him = EEG_copyWithoutSound (me);
		his sound = Sound_MixingMatrix_unmix (my sound.get(), mm.get());
		EEG_setChannelNames_selected (him.get(), U"ic", channelNumbers.get());

		// Calculate the cross-correlations between eye-channels and the ic's

		if (p_resultingEEG)
			*p_resultingEEG = thee.move();
		if (p_resultingMixingMatrix)
			*p_resultingMixingMatrix = mm.move();
	} catch (MelderError) {
		Melder_throw (me, U": no independent components determined.");
	}
}

autoSound EEG_to_Sound_modulated (EEG me, double baseFrequency, double channelBandwidth, conststring32 channelRanges) {
	try {
		autoINTVEC channelNumbers = NUMstring_getElementsOfRanges (channelRanges, my numberOfChannels, U"channel", true);
		const double maxFreq = baseFrequency + my numberOfChannels * channelBandwidth;
		const double samplingFrequency = std::max (2.0 * maxFreq, 44100.0);
		autoSound thee = Sound_createSimple (1, my xmax - my xmin, samplingFrequency);
		for (integer i = 1; i <= channelNumbers.size; i ++) {
			const integer ichannel = channelNumbers [i];
			const double fbase = baseFrequency;// + (ichannel - 1) * channelBandwidth;
			autoSound si = Sound_extractChannel (my sound.get(), ichannel);
			autoSpectrum spi = Sound_to_Spectrum (si.get(), true);
			Spectrum_passHannBand (spi.get(), 0.5, channelBandwidth - 0.5, 0.5);
			autoSpectrum spi_shifted = Spectrum_shiftFrequencies (spi.get(), fbase, samplingFrequency / 2.0, 30);
			autoSound resampled = Spectrum_to_Sound (spi_shifted.get());
			const integer nx = std::min (resampled -> nx, thy nx);
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
		autoSpectrum spi = Sound_to_Spectrum (si.get(), true);
		autoSpectrum spi_shifted = Spectrum_shiftFrequencies (spi.get(), frequencyShift, samplingFrequency / 2.0, 30);
		autoSound thee = Spectrum_to_Sound (spi_shifted.get());
		if (maxAmp > 0)
			Vector_scale (thee.get(), maxAmp);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": channel not converted to sound.");
	}
}

/* End of file EEG_extensions.cpp */
