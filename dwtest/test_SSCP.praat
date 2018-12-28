# test_SSCP.praat
# djmw 20181218

appendInfoLine: tab$, "test_SSCP.praat "

@test_TableOfReal_to_SSCP_rowWeights

appendInfoLine: tab$, "test_SSCP.praat  OK"

procedure test_TableOfReal_to_SSCP_rowWeights
	.tor = Create TableOfReal: "table", 3 , 3
	Formula: "(col -1) * nrow + row"
	Formula: "if col = 3 then 1 else self endif"
	.sscp = To SSCP (row weights): 1, 3, 1, 2, 3
	for .i to 2
		for .j to 2
			assert object[.sscp, .i, .j] = 2; '.i', '.j'
		endfor
	endfor
	selectObject: .tor
	Formula: "if col=3 then row^2 else self fi"
	.sscp2 = To SSCP (row weights): 1, 3, 1, 2, 3
	for .i to 2
		for .j to 2
			assert object[.sscp2, .i, .j] = 10; '.i', '.j'
		endfor
	endfor
	removeObject: .tor, .sscp, .sscp2
endproc