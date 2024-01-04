# meson.build of the library "artsynth"
# David Weenink, 4 January 2024

sources = '''
	Speaker.cpp Articulation.cpp Artword.cpp
	Art_Speaker.cpp Art_Speaker_to_VocalTract.cpp Artword_Speaker.cpp Artword_Speaker_Sound.cpp
	Artword_Speaker_to_Sound.cpp Artword_to_Art.cpp
	Delta.cpp Speaker_to_Delta.cpp Art_Speaker_Delta.cpp
	ArtwordEditor.cpp praat_Artsynth.cpp manual_Artsynth.cpp'''.split()

libartsynth = static_library (
	'artsynth',
	sources : sources,
	dependencies: gtk_dep,
	include_directories : [fon_inc , kar_inc , melder_inc, stat_inc , sys_inc]
)
	
libartsynth_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libartsynth
)
