# meson.build for library "num".
# David Weenink 3 January 2025

sources = '''
	median_of_ninthers.cpp'''.split()

libnum = static_library (
	'num',
	sources : sources,
	include_directories : [sys_inc, melder_inc, kar_inc]
)

libnum_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libnum
)


