# meson.build of the library "fon"
# David Weenink & Paul Boersma, 18 May 2025

sources = '''
	Transition.cpp Distributions_and_Transition.cpp
	Function.cpp Sampled.cpp SampledXY.cpp Matrix.cpp Vector.cpp Polygon.cpp PointProcess.cpp
	Matrix_and_PointProcess.cpp Matrix_and_Polygon.cpp AnyTier.cpp RealTier.cpp
	Sound.cpp LongSound.cpp SoundSet.cpp Sound_files.cpp Sound_audio.cpp PointProcess_and_Sound.cpp Sound_PointProcess.cpp ParamCurve.cpp
	Pitch.cpp Harmonicity.cpp Intensity.cpp Matrix_and_Pitch.cpp Sound_to_Pitch.cpp
	Sound_to_Intensity.cpp Sound_to_Harmonicity.cpp Sound_to_Harmonicity_GNE.cpp Sound_to_PointProcess.cpp
	Pitch_to_PointProcess.cpp Pitch_to_Sound.cpp Pitch_Intensity.cpp
	PitchTier.cpp Pitch_to_PitchTier.cpp PitchTier_to_PointProcess.cpp PitchTier_to_Sound.cpp Manipulation.cpp
	Pitch_AnyTier_to_PitchTier.cpp IntensityTier.cpp DurationTier.cpp AmplitudeTier.cpp
	Spectrum.cpp Ltas.cpp Spectrogram.cpp SpectrumTier.cpp Ltas_to_SpectrumTier.cpp
	Formant.cpp Image.cpp Sound_to_Formant.cpp Sound_and_Spectrogram.cpp
	Sound_and_Spectrum.cpp Spectrum_and_Spectrogram.cpp Spectrum_to_Formant.cpp
	FormantTier.cpp TextGrid.cpp TextGrid_files.cpp TextGrid_Sound.cpp Label.cpp FormantGrid.cpp
	Excitation.cpp Cochleagram.cpp Cochleagram_and_Excitation.cpp Excitation_to_Formant.cpp
	Sound_to_Cochleagram.cpp Spectrum_to_Excitation.cpp
	VocalTract.cpp VocalTract_to_Spectrum.cpp
	Sound_enhance.cpp VoiceAnalysis.cpp H1minusH2Tier.cpp
	WordList.cpp SpellingChecker.cpp
	FujisakiPitch.cpp
	ExperimentMFC.cpp manual_ExperimentMFC.cpp praat_ExperimentMFC.cpp
	Photo.cpp Movie.cpp
	Corpus.cpp
	manual_Picture.cpp manual_Manual.cpp
	manual_commands.cpp manual_formulas.cpp manual_functions.cpp manual_scripting.cpp manual_demoWindow.cpp
	manual_whatsnew.cpp manual_soundFiles.cpp manual_licenses.cpp manual_tutorials.cpp manual_references.cpp
	manual_programming.cpp manual_Fon.cpp manual_voice.cpp Praat_tests.cpp
	manual_glossary.cpp manual_Sampling.cpp manual_exampleSound.cpp
	manual_sound.cpp manual_pitch.cpp manual_spectrum.cpp manual_formant.cpp
	manual_pointProcess.cpp manual_annotation.cpp
	praat_TimeFunction.cpp praat_TimeTier.cpp praat_TimeFrameSampled.cpp
	praat_Sound.cpp praat_Matrix.cpp praat_Tiers.cpp praat_TextGrid_init.cpp praat_uvafon_init.cpp'''.split()

libfon = static_library (
	'fon',
	sources : sources,
	dependencies : gtk_dep,
	include_directories : [dwsys_inc, dwtools_inc, espeak_inc, flac_inc, mp3_inc, portaudio_inc, fon_inc, foned_inc, gram_inc, kar_inc, LPC_inc, melder_inc, num_inc, stat_inc, sys_inc]
)

libfon_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libfon
)
