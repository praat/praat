# test_Permutation.praat
# djmw 20050710, 20070820, 20100525, 20100819, 20110418

appendInfoLine: "test_Permutation.praat"

@test_600_12
@rotate
@sequence
@swap
@invertp
@multiply
@jump

appendInfoLine: "test_Permutation OK"

procedure multiply: 
	appendInfo: tab$, "Multiply"
	.numberOfPermutations = 8
	.numberOfElements = 5
	for .i to .numberOfPermutations / 2
		.p [2*.i -1] = Create Permutation: "p", .numberOfElements, "no"
		.p [2*.i] = Invert
	endfor
	selectObject: .p [1]
	for .i from 2 to .numberOfPermutations
		plusObject: .p [.i]
	endfor
	.p [.numberOfPermutations + 1] = Multiply
	for .j to .numberOfElements
		.val = Get value: .j
		assert .val = .j; p ['.j']='.val'
		appendInfo: "."
	endfor
	for .i to .numberOfPermutations + 1
		removeObject: .p [.i]
	endfor
	appendInfoLine:  ".o.k."
endproc

procedure rotate
	appendInfoLine: tab$, "Rotate"
	for .i to 20
		.p = Create Permutation: "test", .i,  "yes"
	
		appendInfo: tab$, .i
		for .k to .i
			selectObject: .p
			.pk = Rotate: 0, 0, .k
			.pkk = Get value: .k+1
			if .pkk > 0
				assert .pkk = 1 ; i='.i', k='.k'
			endif
			.pki =  Rotate: 0, 0, -.k
			for .l to .k
				.pkil = Get value: .l
				assert .pkil = .l
			endfor
			removeObject: .pk, .pki
			appendInfo: "."
		endfor
		appendInfoLine: " o.k."
		removeObject: .p
	endfor
endproc

procedure invertp
	appendInfo: tab$, "Invert"
	for .i to 20
		.p = Create Permutation: "test", .i, "yes"
		.pin = Invert
		plusObject: .p
		.pm = Multiply
		for .k to .i
			.pmk = Get value: .k
			assert .pmk = .k ; i='.i', k='.k'
		endfor
		appendInfo: "."
		removeObject: .pin, .p, .pm
	endfor
	appendInfoLine: "o.k."
endproc

procedure test_600_12
	appendInfo: tab$, "test_600_12"
	.p = Create Permutation: "600", 600, "yes"
	.pin = Interleave: 0, 0, 12, 1
	.pininv = Invert
	plusObject: .pin
	.pm = Multiply
	for .l to 600
		.pkil = Get value: .l
		assert .pkil = .l; l='.l'
	endfor
	removeObject: .pin, .pininv, .p, .pm
	appendInfoLine: "o.k."
endproc  

procedure sequence
	appendInfo: tab$, "Sequences" 
	.n = 4
	.np = 4*3*2 - 1
	p = Create Permutation: "ps", .n, "yes"
	for .j to .np 
		for .i to .j
			Next
		endfor
		for .i to .j
			Previous
		endfor
		for .i to .n
			.val = Get value: .i
			assert .val = .i
		endfor
		appendInfo: "."
	endfor
	removeObject: p
	appendInfoLine: "o.k." 
endproc
 
procedure swap
	appendInfo: tab$, "Swap positions"
	.p = Create Permutation: "id", 10, "yes"
	for .i to 10
		for .j to 10
			selectObject: .p
			.ps = Copy: "swap"
			Swap positions: .i, .j
			for .k to 10
				selectObject: .p
				.pk = Get value: .k
				selectObject: .ps
				.psk = Get value: .k
				if .k <> .i and .k <> .j
					assert .pk = .psk
				endif
			endfor
			removeObject: .ps
			appendInfo: "."
		endfor
	endfor
	removeObject: .p
	appendInfoLine: "o.k."
endproc

procedure jump
	appendInfo: tab$, "Jump"
	.p1 = Create Permutation: "id", 10, "yes"
	Jump: 3, 1
	for .i to 10
		.p [.i] = Get value: .i
	endfor
	assert .p [1] = 1
	assert .p [2] = 4
	assert .p [3] = 7
	assert .p [4] = 10
	assert .p [5] = 2
	assert .p [6] = 5
	assert .p [7] = 8
	assert .p [8] = 3
	assert .p [9] = 6
	assert .p [10] = 9
	appendInfo: "." 
	.p2 = Create Permutation: "id", 10, "yes"
	Jump: 3, 2
	for .i to 10
		.p [.i] = Get value: .i
	endfor
	assert .p [1] = 2
	assert .p [2] = 5
	assert .p [3] = 8
	assert .p [4] = 3
	assert .p [5] = 6
	assert .p [6] = 9
	assert .p [7] = 1
	assert .p [8] = 4
	assert .p [9] = 7
	assert .p [10] = 10
	removeObject: .p1, .p2
	appendInfoLine: ".o.k." 
endproc
