echo Sound files...
procedure test .type$ .extension$ .duration
	printline Testing '.type$' files...
	for numberOfChannels from 1 to 8
		print 'numberOfChannels' channels:
		sound = Create Sound from formula... sound numberOfChannels 0 .duration/numberOfChannels 44100 1/4 * sin(2*pi*377*x) + randomGauss(0,0.05)
		Formula... round (self * 32768) / 32768
		energy1 = Get energy in air
		Write to '.type$' file... kanweg.'.extension$'
		stopwatch
		sound2 = Read from file... kanweg.'.extension$'
		t = stopwatch
		energy2 = Get energy in air
		assert "'energy1:11'" = "'energy2:11'"
		plus sound
		Remove
		deleteFile ("kanweg." + .extension$)
		printline  't:4' seconds
	endfor
endproc
call test WAV wav 3
call test WAV wav 3
call test AIFF aiff 3
call test AIFC aifc 3
call test Next/Sun au 3
call test NIST nist 3
call test FLAC flac 3
call test WAV wav 30

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
