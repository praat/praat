# Makefile of the library "dwtools"
# David Weenink and Paul Boersma
# 2 June 2020

include ../makefile.defs

CPPFLAGS = -I . -I ../kar -I ../melder -I ../LPC -I ../fon -I ../sys -I ../stat -I ../dwsys -I ../external/portaudio -I ../external/espeak -I ../external/clapack -I ../EEG -I ../external/vorbis -I ../external/opusfile

OBJECTS = ActivationList.o AffineTransform.o \
	Categories.o CategoriesEditor.o \
	Categories_and_Strings.o CCA.o CCA_and_Correlation.o \
	Correlation.o Covariance.o CC.o CCs_to_DTW.o \
	ClassificationTable.o Confusion.o \
	ComplexSpectrogram.o Configuration.o ContingencyTable.o \
	Configuration_AffineTransform.o \
	Configuration_and_Procrustes.o DataModeler.o Distance.o \
	DTW.o DTW_and_TextGrid.o \
	Discriminant.o  Discriminant_PatternList_Categories.o \
	Electroglottogram.o EditDistanceTable.o EEG_extensions.o \
	Eigen_and_Matrix.o Eigen_and_Procrustes.o \
	Eigen_and_TableOfReal.o\
	Eigen_and_SSCP.o Excitations.o \
	espeakdata_FileInMemory.o \
	FilterBank.o FormantGrid_extensions.o \
	GaussianMixture.o  \
	HMM.o \
	ICA.o Intensity_extensions.o IntervalTierNavigator.o \
	LFCC.o LongSound_extensions.o \
	KlattGrid.o KlattGridEditors.o KlattTable.o \
	Ltas_extensions.o \
	MDSVec.o MFCC.o \
	manual_DataModeler.o manual_dwtools.o manual_BSS.o manual_HMM.o \
	manual_KlattGrid.o manual_MDS.o manual_Permutation.o \
	Minimizers.o MixingMatrix.o \
	Matrix_and_NMF.o Matrix_extensions.o \
	Matrix_Categories.o MDS.o \
	NavigationContext.o \
	OptimalCeilingTier.o OptimalCeilingTierEditor.o \
	PatternList.o PCA.o \
	Pitch_extensions.o Polynomial_to_Spectrum.o \
	Polygon_extensions.o Procrustes.o \
	Proximity.o Proximity_and_Distance.o \
	Resonator.o Roots_to_Spectrum.o \
	Sound_and_MixingMatrix.o \
	Sound_and_Spectrogram_extensions.o Sound_and_PCA.o Sound_extensions.o \
	Sound_to_MFCC.o Sounds_to_DTW.o \
	Sound_to_Pitch2.o Sound_to_SPINET.o SPINET.o SPINET_to_Pitch.o \
	Spectrogram_extensions.o Spectrum_extensions.o SSCP.o Strings_extensions.o \
	SpeechSynthesizer.o SpeechSynthesizer_and_TextGrid.o \
	Table_and_Strings.o Table_extensions.o TableOfReal_and_SVD.o \
	TableOfReal_extensions.o \
	TableOfReal_and_Discriminant.o TableOfReal_and_Permutation.o \
	TextGrid_and_DurationTier.o TextGrid_and_PitchTier.o TextGrid_extensions.o \
	TextGridNavigator.o TextGridView.o VowelEditor.o \
	praat_MDS_init.o praat_BSS_init.o praat_HMM_init.o \
	praat_KlattGrid_init.o praat_DataModeler_init.o praat_David_init.o

.PHONY: all clean

all: libdwtools.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libdwtools.a

libdwtools.a: $(OBJECTS)
	touch libdwtools.a
	rm libdwtools.a
	$(AR) cq libdwtools.a $(OBJECTS)
	$(RANLIB) libdwtools.a

$(OBJECTS): *.h ../kar/*.h ../melder/*.h ../sys/*.h ../fon/*.h ../dwsys/*.h ../stat/*.h ../LPC/*.h ../external/espeak/*.h

