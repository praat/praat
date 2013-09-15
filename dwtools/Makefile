# Makefile of the library "dwtools"
# David Weenink, 22 February 2010
# djmw 20130826 Latest modification

include ../makefile.defs

CPPFLAGS = -I ../num -I ../LPC -I ../fon -I ../sys -I ../stat -I ../dwsys -I ../external/portaudio -I ../external/espeak -I ../EEG -I ../kar

OBJECTS = Activation.o AffineTransform.o \
	Categories.o CategoriesEditor.o \
	Categories_and_Strings.o CCA.o CCA_and_Correlation.o \
	CC.o CCs_to_DTW.o \
	ClassificationTable.o Confusion.o \
	Configuration.o ContingencyTable.o \
	Configuration_AffineTransform.o \
	Configuration_and_Procrustes.o Distance.o \
	DTW.o DTW_and_TextGrid.o \
	Discriminant.o  Discriminant_Pattern_Categories.o \
	EditDistanceTable.o EEG_extensions.o \
	Eigen_and_Matrix.o Eigen_and_Procrustes.o \
	Eigen_and_TableOfReal.o\
	Eigen_and_SSCP.o Excitations.o \
	FilterBank.o FormantGrid_extensions.o \
	GaussianMixture.o  \
	HMM.o \
	ICA.o Intensity_extensions.o \
	LFCC.o LongSound_extensions.o \
	KlattGrid.o KlattGridEditors.o KlattTable.o \
	Ltas_extensions.o \
	MelFilter_and_MFCC.o MFCC.o \
	manual_dwtools.o manual_BSS.o manual_HMM.o \
	manual_KlattGrid.o manual_MDS.o manual_Permutation.o \
	Minimizers.o \
	Matrix_extensions.o \
	Matrix_Categories.o MDS.o \
	Pattern.o PCA.o \
	Pitch_extensions.o Polynomial.o \
	Polygon_extensions.o Procrustes.o \
	Proximity.o \
	Resonator.o \
	Sampled2.o \
	Sound_and_FilterBank.o Sound_and_PCA.o Sound_extensions.o \
	Sound_to_MFCC.o Sounds_to_DTW.o \
	Sound_to_Pitch2.o Sound_to_SPINET.o SPINET.o SPINET_to_Pitch.o \
	Spectrum_extensions.o SSCP.o Strings_extensions.o \
	SpeechSynthesizer.o SpeechSynthesizer_and_TextGrid.o\
	Table_extensions.o TableOfReal_and_SVD.o\
	TableOfReal_extensions.o \
	TableOfReal_and_Permutation.o \
	TextGrid_extensions.o \
	VowelEditor.o \
	praat_MDS_init.o praat_BSS_init.o praat_HMM_init.o praat_KlattGrid_init.o praat_David_init.o

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

$(OBJECTS): *.h ../num/NUM.h ../sys/*.h ../fon/*.h ../dwsys/*.h ../stat/*.h ../LPC/*.h ../external/espeak/*.h

