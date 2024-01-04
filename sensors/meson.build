# meson.build of the library "sensors"
# David Weenink, 4 January 2024

sources = '''
	Electroglottogram.cpp EMA.cpp EMArawData.cpp
	praat_sensors_init.cpp'''.split()

libsensors = static_library (
	'sensors',
	sources : sources,
	dependencies : gtk_dep,
	include_directories : [dwtools_inc, clapack_inc, gsl_inc, fon_inc, kar_inc, melder_inc, stat_inc, sys_inc]
)

libsensors_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libsensors
)

