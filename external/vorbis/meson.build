# meson.build of the external library "vorbis"
# David Weenink, 4 January 2024

sources = '''
	ogg_bitwise.cpp ogg_framing.cpp
	vorbis_block.cpp vorbis_bitrate.cpp vorbis_codebook.cpp vorbis_envelope.cpp vorbis_floor0.cpp
	vorbis_info.cpp vorbis_mdct.cpp vorbisfile.cpp vorbis_floor0.cpp vorbis_floor1.cpp vorbis_lpc.cpp vorbis_lsp.cpp
	vorbis_mapping0.cpp vorbis_psy.cpp vorbis_registry.cpp vorbis_res0.cpp vorbis_sharedbook.cpp vorbis_smallft.cpp
	vorbis_synthesis.cpp vorbis_window.cpp'''.split()

libvorbis = static_library (
	'vorbis',
	sources : sources,
	include_directories : melder_inc
)

libvorbis_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libvorbis
)
