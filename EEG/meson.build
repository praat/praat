# meson.build of the library "EEG"
# David Weenink, 4 January 2024

sources = '''
	EEG.cpp EEGArea.cpp EEGAnalysisArea.cpp
	EEGWindow.cpp ERPTier.cpp ERP.cpp ERPArea.cpp ERPWindow.cpp
	praat_EEG.cpp manual_EEG.cpp'''.split()

libEEG = static_library (
	'EEG',
	sources : sources,
	dependencies : gtk_dep,
	include_directories : [dwsys_inc, dwtools_inc, fon_inc, foned_inc, kar_inc, melder_inc, stat_inc, sys_inc]
)

libEEG_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libEEG
)
