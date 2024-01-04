# meson.build of the external library "portaudio"
# David Weenink, 4 January 2024

sources = '''
	pa_unix_hostapis.c pa_unix_util.c pa_linux_alsa.c pa_jack.c
	pa_ringbuffer.c pa_win_hostapis.c pa_win_util.c pa_win_wmme.c
	pa_win_waveformat.c pa_front.c pa_debugprint.c pa_cpuload.c
	pa_allocation.c pa_process.c pa_converters.c pa_dither.c
	pa_stream.c'''.split()

libportaudio = static_library (
	'portaudio',
	sources : sources,
	include_directories : melder_inc
)

libportaudio_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libportaudio
)
