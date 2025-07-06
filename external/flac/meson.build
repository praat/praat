# meson.build of the external library "flac"
# David Weenink, 23 June 2025

sources = '''
	flac_bitmath.c flac_bitreader.c
	flac_bitwriter.c flac_cpu.c
	flac_crc.c flac_fixed.c
	flac_float.c flac_format.c
	flac_lpc.c flac_md5.c
	flac_memory.c flac_metadata_iterators.c
	flac_metadata_object.c flac_stream_decoder.c
	flac_stream_encoder.c flac_stream_encoder_framing.c
	flac_window.c flac_share_win_utf8_io.c'''.split()

libflac = static_library (
	'flac',
	sources : sources,
	include_directories : melder_inc
)

libflac_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libflac
)
