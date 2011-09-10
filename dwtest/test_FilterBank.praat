# test_Filterbank.praat
# djmw 20110907

eps = 1.6e-16
printline test_FilterBank

starttime = 0
endtime = 1
fs = 44100
s = Create Sound from formula... sineWithNoise 1 starttime endtime fs 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)

call test_FormantFilter s
call test_BarkFilter s
call test_MelFilter s

select s
Remove
printline test_FilterBank OK

procedure test_FormantFilter .s
	printline 'tab$' test FormantFilter
	bf_first = 100
	fdistance = 50
	timestep = 0.005
	select .s
	.ff = To FormantFilter... 0.015 timestep bf_first fdistance 0 1.1 75 600

	call queries .ff
	call modifies .ff
	call commons .ff
	select .ff
	Remove
endproc

procedure test_BarkFilter .s
	printline 'tab$' test BarkFilter
	select .s
	bf_first = 1
	fdistance = 1
	.bf = To BarkFilter... 0.015 timestep 1 1 0

	call queries .bf
	call modifies .bf
	call commons .bf
	select .bf
	Remove
endproc

procedure test_MelFilter .s
	printline 'tab$' test MelFilter
	select .s
	bf_first = 100
	fdistance = 100
	.mf = To MelFilter... 0.015 timestep bf_first fdistance 0

	call queries .mf
	call modifies .mf
	call commons .mf
	select .mf
	.mfcc = To MFCC... 12
	Remove
	select .mf
	Remove
endproc

procedure commons .bf
	select .bf
	.in = To Intensity
	Remove
	select .bf
	.mat = To Matrix
	Remove
endproc

procedure queries .fb
	select .fb
	.starttime = Get start time
	.endtime = Get end time
	.dur = Get total duration
	assert abs( .dur -(.endtime-.starttime)) <= eps
	assert .starttime = starttime
	assert .endtime = endtime
	.timestep = Get time step
	assert .timestep = timestep; '.timestep' 'timestep'
	.bf_first = Get lowest frequency
	assert .bf_first >= 0; '.bf_first' 'bf_first'
	.bf_last = Get highest frequency
	assert .bf_last <= fs/2
	.nf = Get number of frequencies
	assert .nf >= 1
	.fdistance = Get frequency distance
	assert .fdistance=fdistance; '.fdistance' 'fdistance'
	.f_from_row[1] = Get frequency from row... 1
	for .i  from 2 to .nf
		.f_from_row[.i] = Get frequency from row... .i
		.fi =  .f_from_row[.i]
		.fim1 =   .f_from_row[.i-1]
		assert .f_from_row[.i] >.f_from_row[.i-1]; '.fi' >.'fim1'
	endfor
	for .i to .nf
		.time = randomUniform (.starttime, endtime)
		.vali = Get value in cell... .time .i
	endfor
endproc

procedure modifies .fb
	select .fb
	Equalize intensities... 80
endproc

