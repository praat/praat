# meson.build for library "num".
# David Weenink 30 April 2025

sources = '''
	median_of_ninthers.cpp'''.split()

libnum = static_library (
	'num',
	sources : sources,
	include_directories : [dwsys_inc, sys_inc, melder_inc, kar_inc]
)

libnum_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libnum
)


