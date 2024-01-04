# meson.build of the library "FFNet"
# David Weenink, 3 January 2024

sources = '''
	FFNet.cpp FFNet_Eigen.cpp FFNet_Matrix.cpp FFNet_PatternList.cpp
	FFNet_ActivationList_Categories.cpp FFNet_PatternList_ActivationList.cpp
	FFNet_PatternList_Categories.cpp
	praat_FFNet_init.cpp manual_FFNet.cpp'''.split()

libFFNet = static_library (
	'FFNet',
	sources : sources,
	dependencies : gtk_dep,
	include_directories : [dwsys_inc, dwtools_inc, fon_inc, foned_inc, gram_inc, kar_inc, melder_inc, stat_inc, sys_inc]
)

libFFNet_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libFFNet
)
