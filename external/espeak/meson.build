# meson.build of the external library "espeak"
# David Weenink, 6 January 2024

sources = '''
	case.cpp categories.cpp
	create_espeak_ng_FileInMemoryManager.cpp
	create_espeak_ng_FileInMemorySet.cpp create_espeak_ng_FileInMemorySet__ru.cpp
	espeak_io.cpp
	common.cpp compiledata.cpp compiledict.cpp
	dictionary.cpp
	encoding.cpp error.cpp espeak_api.cpp
	intonation.cpp klatt.cpp langopts.cpp mnemonics.cpp
	numbers.cpp phonemelist.cpp proplist.cpp readclause.cpp
	setlengths.cpp soundicon.cpp speech.cpp
	ssml.cpp synthdata.cpp synthesize.cpp
	synth_mbrola.cpp tr_languages.cpp translate.cpp translateword.cpp voices.cpp wavegen.cpp'''.split()

libespeak = static_library (
	'espeak',
	sources : sources,
	include_directories : [dwtools_inc, dwsys_inc, kar_inc, melder_inc, sys_inc, stat_inc]
)

libespeak_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libespeak
)
