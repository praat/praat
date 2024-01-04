# meson.build of the library "gram"
# David Weenink, 3 January 2024

sources = '''
	CubeGrid.cpp CubeGridArea.cpp CubeGridEditor.cpp
	Layer.cpp manual_gram.cpp Net.cpp Network.cpp
	NoulliGrid.cpp NoulliGridArea.cpp NoulliGridEditor.cpp
	OTGrammar.cpp OTGrammarEditor.cpp OTMulti.cpp OTMultiEditor.cpp
	OTGrammar_ex_metrics.cpp OTGrammar_ex_NoCoda.cpp OTGrammar_ex_NPA.cpp 
	OTGrammar_ex_tongueRoot.cpp OTMulti_ex_metrics.cpp
	praat_gram.cpp'''.split()

libgram = static_library (
	'gram',
	sources : sources,
	dependencies : gtk_dep,
	include_directories : [dwsys_inc, dwtools_inc, fon_inc, foned_inc, kar_inc, melder_inc, stat_inc, sys_inc]
)

libgram_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libgram
)
