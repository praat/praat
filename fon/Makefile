# Makefile of the library "fon"
# Paul Boersma, March 18, 2007

include ../makefile.defs
CFLAGS = -I ../audio -I ../kar -I ../sys -I ../dwsys -I ../stat -I ../dwtools -I ../LPC -I ../fon

OBJECTS = Transition.o Distributions_and_Transition.o \
   Function.o Sampled.o Matrix.o Vector.o Polygon.o PointProcess.o \
   Matrix_and_PointProcess.o Matrix_and_Polygon.o AnyTier.o RealTier.o \
   Sound.o LongSound.o Sound_files.o Sound_audio.o PointProcess_and_Sound.o ParamCurve.o \
   Pitch.o Harmonicity.o Intensity.o Matrix_and_Pitch.o Sound_to_Pitch.o \
   Sound_to_Intensity.o Sound_to_Harmonicity.o Sound_to_Harmonicity_GNE.o Sound_to_PointProcess.o \
   Pitch_to_PointProcess.o Pitch_to_Sound.o Pitch_Intensity.o \
   PitchTier.o Pitch_to_PitchTier.o PitchTier_to_PointProcess.o PitchTier_to_Sound.o Manipulation.o \
   Pitch_AnyTier_to_PitchTier.o IntensityTier.o DurationTier.o AmplitudeTier.o \
   Spectrum.o Ltas.o Spectrogram.o SpectrumTier.o Ltas_to_SpectrumTier.o \
   Formant.o Image.o Sound_to_Formant.o Sound_and_Spectrogram.o \
   Sound_and_Spectrum.o Spectrum_and_Spectrogram.o Spectrum_to_Formant.o \
   FormantTier.o TextGrid.o Label.o \
   Excitation.o Cochleagram.o Cochleagram_and_Excitation.o Excitation_to_Formant.o \
   Sound_to_Cochleagram.o Spectrum_to_Excitation.o \
   VocalTract.o VocalTract_to_Spectrum.o \
   SoundRecorder.o Sound_enhance.o VoiceAnalysis.o \
   FunctionEditor.o PitchEditor.o SoundEditor.o SpectrumEditor.o SpectrogramEditor.o \
   PointEditor.o \
   FunctionEditor_Sound.o FunctionEditor_SoundAnalysis.o \
   RealTierEditor.o PitchTierEditor.o IntensityTierEditor.o \
   DurationTierEditor.o AmplitudeTierEditor.o \
   ManipulationEditor.o TextGridEditor.o \
   WordList.o SpellingChecker.o \
   FujisakiPitch.o \
   ExperimentMFC.o RunnerMFC.o manual_Exp.o praat_Exp.o \
   OTGrammar.o OTGrammarEditor.o manual_OT.o praat_OT.o OTMulti.o OTMultiEditor.o \
   OTGrammar_ex_metrics.o OTGrammar_ex_NoCoda.o OTGrammar_ex_NPA.o OTGrammar_ex_tongueRoot.o \
   manual_Picture.o manual_Manual.o manual_Script.o \
   manual_soundFiles.o manual_tutorials.o manual_references.o \
   manual_programming.o manual_Fon.o manual_voice.o Praat_tests.o \
   manual_glossary.o manual_Sampling.o manual_exampleSound.o \
   manual_sound.o manual_pitch.o manual_spectrum.o manual_formant.o manual_annotation.o \
   praat_Sound_init.o praat_TextGrid_init.o praat_Fon.o

all: libfon.a
libfon.a: $(OBJECTS)
	touch libfon.a
	rm libfon.a
	ar cq libfon.a $(OBJECTS)
	$(RANLIB) libfon.a

$(OBJECTS): *.h ../audio/*.h ../kar/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h ../dwtools/*.h ../LPC/*.h
