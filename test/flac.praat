echo Endian...
procedure do
	Read from file... test.wav
	wavEnergy = Get energy in air
	Remove

	Read from file... test.flac
	flacEnergy = Get energy in air
	Remove

	assert "'wavEnergy:11'" = "'flacEnergy:11'"
endproc
printline Optimized:
Debug... 0
call do
printline Portable:
Debug... 18
call do
Debug... 0

printline OK
