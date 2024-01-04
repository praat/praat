# meson.build of the external library "lame"
# David Weenink, 4 January 2024

sources = '''
	bitstream.c encoder.c fft.c gain_analysis.c mpglib_interface.c
	id3tag.c lame.c
	newmdct.c psymodel.c quantize.c quantize_pvt.c set_get.c vbrquantize.c
	reservoir.c tables.c takehiro.c util.c VbrTag.c
	version.c presets.c vector_xmm_quantize_sub.c'''.split()

liblame = static_library (
	'lame',
	sources : sources,
	include_directories : melder_inc
)

liblame_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: liblame
)

