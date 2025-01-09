# meson.build for 'main'
# David Weenink, 3 January 2025

executable ('praat', sources : ['main_Praat.cpp'],
	include_directories : [fon_inc, kar_inc, melder_inc, sys_inc],
	dependencies : [gtk_dep, threads_dep, praat_libs_dep, praat_external_libs_dep],
	link_args: system_libs
)
