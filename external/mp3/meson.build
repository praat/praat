# meson.build of the external library "mp3"
# David Weenink, 4 January 2024

sources = '''
	mp3.cpp mad_bit.c mad_decoder.c
	mad_fixed.c mad_frame.c mad_huffman.c
	mad_layer12.c mad_layer3.c mad_stream.c
	mad_synth.c mad_timer.c mad_version.c'''.split()

libmp3 = static_library (
	'mp3',
	sources : sources,
	include_directories : melder_inc
)

libmp3_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libmp3
)
