# build system for praat
# David Weenink 4 January 2024

project('praat', 'cpp', 'c',
	default_options : ['cpp_std=c++17', 'c_std=gnu99']
)

dwsys_inc = include_directories ('dwsys')
dwtools_inc = include_directories ('dwtools')
EEG_inc = include_directories ('EEG')
fon_inc = include_directories ('fon')
foned_inc = include_directories ('foned')
gram_inc = include_directories ('gram')
kar_inc = include_directories ('kar')
LPC_inc = include_directories ('LPC')
melder_inc = include_directories ('melder')
sensors_inc = include_directories ('sensors')
stat_inc = include_directories ('stat')
sys_inc = include_directories ('sys')

clapack_inc = include_directories ('external/clapack')
espeak_inc = include_directories ('external/espeak')
flac_inc = include_directories ('external/flac')
gsl_inc = include_directories ('external/gsl')
mp3_inc = include_directories ('external/mp3')
opusfile_inc = include_directories ('external/opusfile')
portaudio_inc = include_directories ('external/portaudio')
vorbis_inc = include_directories ('external/vorbis')

threads_dep = dependency('threads')

gtk_dep = dependency('gtk+-3.0')

system_libs = ['-lX11', '-lm', '-lpulse', '-lasound']

add_global_arguments ('''-DUNIX -Dlinux -DALSA -DHAVE_PULSEAUDIO -D_FILE_OFFSET_BITS=64 
	-Wreturn-type -Wunused -Wunused-parameter -Wuninitialized -g0 -ggdb'''.split(), language: ['cpp', 'c'])

praat_dirs = ['artsynth', 'dwsys', 'dwtools', 'EEG', 'FFNet', 'fon', 'foned', 'gram', 'kar', 'LPC', 'melder', 'sensors', 'stat', 'sys']
praat_external_dirs = ['clapack', 'espeak', 'flac', 'gsl', 'glpk', 'lame', 'mp3', 'opusfile', 'portaudio', 'vorbis']

foreach dir : praat_dirs
	subdir (dir)
endforeach
foreach dir : praat_external_dirs
	subdir ('external' / dir)
endforeach

praat_libs_dep = [libartsynth_dep, libdwsys_dep, libdwtools_dep, libEEG_dep, libFFNet_dep, libfon_dep, libfoned_dep, libgram_dep, libkar_dep, libLPC_dep, libmelder_dep, libsensors_dep, libstat_dep, libsys_dep]
praat_external_libs_dep = [libclapack_dep, libespeak_dep, libflac_dep, libgsl_dep, libglpk_dep, liblame_dep, libmp3_dep, libopusfile_dep, libportaudio_dep, libvorbis_dep]

subdir ('main')
