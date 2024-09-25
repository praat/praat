# meson.build of the external library "espeak"
# David Weenink, 5 September 2024

sources = '''
	case.cpp categories.cpp common.cpp compiledict.cpp
	dictionary.cpp
	encoding.cpp error.cpp espeak_api.cpp
	intonation.cpp klatt.cpp langopts.cpp mnemonics.cpp
	numbers.cpp phonemelist.cpp proplist.cpp readclause.cpp
	setlengths.cpp soundicon.cpp speech.cpp
	ssml.cpp synthdata.cpp synthesize.cpp
	tr_languages.cpp translate.cpp translateword.cpp voices.cpp wavegen.cpp
	espeak_praat_FileInMemorySet_addLanguages.cpp espeak_praat_FileInMemorySet_addVoices.cpp
	espeak_praat_FileInMemorySet_addRussianDict.cpp espeak_praat_FileInMemorySet_addFaroeseDict.cpp
	espeak_praat_FileInMemorySet_addOtherDicts.cpp espeak_praat_FileInMemorySet_addPhon.cpp
	espeak_praat.cpp'''.split()

libespeak = static_library (
	'espeak',
	sources : sources,
	include_directories : [dwtools_inc, dwsys_inc, kar_inc, melder_inc, sys_inc, stat_inc]
)

libespeak_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libespeak
)
