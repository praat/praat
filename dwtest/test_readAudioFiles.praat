# test_readAudioFiles.praat
# djmw 20210208

# ogg vorbis
appendInfoLine: "test_readAudioFiles.praat"
appendInfoLine: tab$, "Read vorbis audio from ogg file"
vorbis  = Read from file: "Example_vorbis_audio.ogg"
appendInfoLine: tab$, "Read opus audio from ogg file "
opus = Read from file: "Example_opus_audio.ogg"
removeObject: vorbis, opus
appendInfoLine: "test_readAudioFiles.praat OK"
