echo Sound files...
procedure test .type$ .extension$ .duration
	printline Testing '.type$' files...
	for numberOfChannels from 1 to 8
		print 'numberOfChannels' channels:
		sound = Create Sound from formula... sound numberOfChannels 0 .duration/numberOfChannels 44100 1/4 * sin(2*pi*377*x) + randomGauss(0,0.05)
		Formula: ~ round (self * 32768) / 32768
		energy1 = Get energy in air
		Save as '.type$' file... kanweg.'.extension$'
		stopwatch
		sound2 = Read from file... kanweg.'.extension$'
		t = stopwatch
		energy2 = Get energy in air
		assert "'energy1:12'" = "'energy2:12'"   ; 'numberOfChannels'
		plus sound
		Remove
		deleteFile ("kanweg." + .extension$)
		printline  't:4' seconds
	endfor
endproc
procedure test24 .type$ .extension$ .duration
	printline Testing 24-bit '.type$' files...
	for numberOfChannels from 1 to 8
		print 'numberOfChannels' channels:
		sound = Create Sound from formula... sound numberOfChannels 0 .duration/numberOfChannels 44100 1/4 * sin(2*pi*377*x) + randomGauss(0,0.05)
		Formula: ~ round (self * 32768*256) / (32768*256)
		energy1 = Get energy in air
		Save as 24-bit '.type$' file... kanweg.'.extension$'
		stopwatch
		sound2 = Read from file... kanweg.'.extension$'
		t = stopwatch
		energy2 = Get energy in air
		assert "'energy1:12'" = "'energy2:12'"
		plus sound
		Remove
		deleteFile ("kanweg." + .extension$)
		printline  't:4' seconds
	endfor
endproc
procedure test32 .type$ .extension$ .duration
	printline Testing 32-bit '.type$' files...
	for numberOfChannels from 1 to 8
		print 'numberOfChannels' channels:
		sound = Create Sound from formula... sound numberOfChannels 0 .duration/numberOfChannels 44100 1/4 * sin(2*pi*377*x) + randomGauss(0,0.05)
		Formula: ~ round (self * 32768*65536) / (32768*65536)
		energy1 = Get energy in air
		Save as 32-bit '.type$' file... kanweg.'.extension$'
		stopwatch
		sound2 = Read from file... kanweg.'.extension$'
		t = stopwatch
		energy2 = Get energy in air
		assert "'energy1:12'" = "'energy2:12'"   ; '.type$' '.extension$' '.duration'
		plus sound
		Remove
		deleteFile ("kanweg." + .extension$)
		printline  't:4' seconds
	endfor
endproc
Debug... no 18
call test WAV wav 3
Debug... no 0
call test WAV wav 3
Debug... no 0
call test AIFF aiff 3
call test AIFC aifc 3
call test Next/Sun au 3
call test NIST nist 3
@test: "Kay sound", "nsp", 3
call test FLAC flac 3
Debug... no 18
call test WAV wav 30
Debug... no 0
call test WAV wav 30
call test24 WAV wav 3
Debug... no 18
call test32 WAV wav 3
Debug... no 0
call test32 WAV wav 3

procedure do
	Read from file... test.wav
	wavEnergy = Get energy in air
	Remove

	Read from file... test.flac
	flacEnergy = Get energy in air
	Remove

	assert "'wavEnergy:12'" = "'flacEnergy:12'"
endproc
printline Optimized:
Debug... no 0
call do
printline Portable:
Debug... no 18
call do
Debug... no 0

printline OK
