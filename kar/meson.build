# meson.build of the library "kar"
# David Weenink, 3 January 2024

sources = 'ipaSerifRegularPS.cpp longchar.cpp wctype_portable.cpp'.split()

libkar = static_library (
	'kar',
	sources : sources,
	dependencies : gtk_dep,
	include_directories : melder_inc
)

libkar_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libkar
)
