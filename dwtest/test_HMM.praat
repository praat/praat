# test_HMM.praat

appendInfoLine: "test_HMM"
@mm
@hmms_test_multiple_os


# two state not hidden model
procedure mm
	.mm1 = Create simple HMM: "m1", "no", "1 2", ""
	Set transition probabilities: 1, "0.8 0.2"
	Set transition probabilities: 2, "0.4 0.6"

	# check sum of p leaving
	for .it to 10
 		.psum = 0
		for .istate to 2
			.lnp = Get p (time, state): .it , .istate
			.p = exp(.lnp)
			.psum += .p
		endfor
 		assert (.psum>0.999 and .psum<1.001); '.it' '.psum'
	endfor
	.os = To HMMObservationSequence: 0, 2000
	.mm2 = To HMM: 0, "no"
	plusObject: .os
	Learn: 0.001, 0.00000001, "no"

	selectObject: .mm2
  	# check sum of p leaving
	for .it to 10
		.psum = 0
		for .istate to 2
			.p = Get p (time, state): .it, .istate
 			.p = exp(.p)
			.psum += .p
		endfor
		assert (.psum>0.999 and .psum<1.001); '.it' '.psum'
	endfor

	removeObject: .mm1, .mm2, .os
endproc

procedure hmms_test_multiple_os
	.hmm = Create simple HMM: "hmm1", "no", "1 2 3", ""
	Set transition probabilities: 1, "0.5 0.1 0.4"
	Set transition probabilities: 2, "0.3 0.5 0.2"
	Set transition probabilities: 3, "0.4 0.1 0.5"
	.os = To HMMObservationSequence: 0, 20000
	.s = To Strings
	.s1 = Extract part: 1, 10001
	.os1 = To HMMObservationSequence
	selectObject: .s
	.s2 = Extract part: 10002, 20000
	.os2 = To HMMObservationSequence

	.hmm2 = Create simple HMM: "hmm3", "no", "1 2 3", ""
	selectObject: .hmm2, .os1, .os2
	Learn: 0001, 0.00000001, "no"
	removeObject: .s, .s1, .s2, .hmm2, .os, .os1, .os2, .hmm
endproc

appendInfoLine: "test_HMM OK"

