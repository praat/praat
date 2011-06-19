# test_Permutation.praat
# djmw 20050710, 20070820, 20100525, 20100819, 20110418

printline test_Permutation

call test_600_12
call rotate
call sequence
call swap
call invertp

printline test_Permutation OK

procedure rotate
	printline ..Rotate
	for .i to 20
		.p = Create Permutation... test .i y
	
		print    '.i'
		for .k to .i
			select .p
			.pk = Rotate... 0 0 .k
			.pkk = Get value... .k+1
			if .pkk > 0
				assert .pkk = 1 ; i='.i', k='.k'
			endif
			.pki =  Rotate... 0 0 -.k
			for .l to .k
				.pkil = Get value... .l
				assert .pkil = .l
			endfor
			plus .pk
			Remove
			print .
		endfor
		printline ..o.k.
		select .p
		Remove
	endfor
endproc

procedure invertp
	printline ..Invert
	for .i to 20
		.p = Create Permutation... test .i y
		.pin = Invert
		plus .p
		.pm = Multiply
		for .k to .i
			.pmk = Get value... .k
			assert .pmk = .k ; i='.i', k='.k'
		endfor
		print .
		plus .pin
		plus .p
		Remove
	endfor
	printline ..o.k.
endproc

procedure test_600_12
	printline ..test_600_12
	.p = Create Permutation... 600 600 y
	.pin = Interleave... 0 0 12 1
	.pininv = Invert
	plus .pin
	.pm = Multiply
	for .l to 600
		.pkil = Get value... .l
		assert .pkil = .l; l='.l'
	endfor
	plus .pin
	plus .pininv
	plus .p
	Remove
	printline ..o.k.
endproc  

procedure sequence
	printline ..Sequences 
	.n = 4
	.np = 4*3*2 - 1
	Create Permutation... ps .n y
	for .j to .np 
		for .i to .j
			Next
		endfor
		for .i to .j
			Previous
		endfor
		for .i to .n
			.val = Get value... .i
			assert .val = .i
		endfor
		print .
	endfor
	Remove
	printline ..o.k. 
endproc
 
procedure swap
	print ..Swap positions
	.p = Create Permutation... id 10 y
	for .i to 10
		for .j to 10
			select .p
			.ps = Copy... swap
			Swap positions... .i .j
			for .k to 10
				select .p
				.pk = Get value... .k
				select .ps
				.psk = Get value... .k
				if .k <> .i and .k <> .j
					assert .pk = .psk
				endif
			endfor
			select .ps
			Remove
			print .
		endfor
	endfor
	select .p
	Remove
	printline ..o.k.
endproc
