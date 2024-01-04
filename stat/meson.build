# meson.build of the library "stat"
# David Weenink, 3 January 2024

sources = '''
	Distributions.cpp Distributions_and_Strings.cpp
	LogisticRegression.cpp manual_statistics.cpp PairDistribution.cpp
	praat_TableOfReal.cpp praat_Stat.cpp Regression.cpp
	Table.cpp TableEditor.cpp TableOfReal.cpp'''.split()

libstat = static_library (
	'stat',
	sources : sources,
	dependencies : gtk_dep,
	include_directories : [dwsys_inc, dwtools_inc, fon_inc, foned_inc, kar_inc, melder_inc, stat_inc, sys_inc]
)

libstat_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libstat
)
