echo Endian...
procedure do
	Read from file... test.wav
	energyInAir = Get energy in air
	assert "'energyInAir:11'" = "0.00008397361"

	Read from file... test.Sound
	energyInAir2 = Get energy in air
	Remove
	select Sound test
	assert energyInAir2 = energyInAir

	Write to WAV file... test2.wav
	Remove
	Read from file... test2.wav
	energyInAir2 = Get energy in air
	deleteFile ("test2.wav")
	assert energyInAir2 = energyInAir

	Write to AIFF file... test2.aiff
	Remove
	Read from file... test2.aiff
	energyInAir2 = Get energy in air
	deleteFile ("test2.aiff")
	assert energyInAir2 = energyInAir

	Write to FLAC file... test2.flac
	Remove
	Read from file... test2.flac
	energyInAir2 = Get energy in air
	deleteFile ("test2.flac")
	assert energyInAir2 = energyInAir

	Write to binary file... test2.bin
	Remove
	Read from file... test2.bin
	energyInAir2 = Get energy in air
	deleteFile ("test2.bin")
	assert energyInAir2 = energyInAir

	Write to Sesam file... test2.sdf
	Remove
	Read from file... test2.sdf
	deleteFile ("test2.sdf")
	energyInAir2 = Get energy in air
	Write to Sesam file... test3.sdf
	Remove
	Read from file... test3.sdf
	deleteFile ("test3.sdf")
	energyInAir3 = Get energy in air
	assert energyInAir3 = energyInAir2

	Remove

	Create Sound... test 0 100 22050 0.1 * randomGauss (0, 1)
	for i to 2
		stopwatch
		Write to WAV file... kanweg.Sound
		t1 = stopwatch
		stopwatch
		Write to AIFF file... kanweg.Sound
		t2 = stopwatch
		stopwatch
		Write to binary file... kanweg.Sound
		t3 = stopwatch
		stopwatch
		Write to FLAC file... kanweg.Sound
		t4 = stopwatch
		printline 't1:2' 't2:2' 't3:2' 't4:2'
	endfor
	Remove

	Create Sound... test 0 1 22050 exp(-x*1000)
	Write to binary file... kanweg.Sound
	Read from file... kanweg.Sound
	Formula... (self = Sound_test []) - 1
	energyInAir = Get energy in air
	assert energyInAir = 0
	plus Sound test
	Remove

	Create Sound... test 0 1 22050 exp(x*1000)
	Write to binary file... kanweg.Sound
	Read from file... kanweg.Sound
	Formula... (self = Sound_test []) - 1
	energyInAir = Get energy in air
	assert energyInAir = 0
	plus Sound test
	Remove
	deleteFile ("kanweg.Sound")

	Create TableOfReal... test 1 22050
	Formula... exp(-col/22.05)
	Write to binary file... kanweg.TableOfReal
	Read from file... kanweg.TableOfReal
	Formula... (self = TableOfReal_test []) - 1
	norm = Get table norm
	assert norm = 0
	plus TableOfReal test
	Remove

	Create TableOfReal... test 1 22050
	Formula... exp(col/22.05)
	Write to binary file... kanweg.TableOfReal
	Read from file... kanweg.TableOfReal
	Formula... (self = TableOfReal_test []) - 1
	norm = Get table norm
	assert norm = 0
	plus TableOfReal test
	Remove
	deleteFile ("kanweg.TableOfReal")
endproc
printline Optimized:
Debug... no 0
call do
printline Portable:
Debug... no 18
call do
Debug... no 0

printline OK
