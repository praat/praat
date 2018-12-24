# test_robustStatistics.praat
#djmw 20181127

appendInfoLine: "test_robustStatistics.praat"


@mad
@location_scale

procedure mad
	.answer# ={0,0,1.4826, 1.4826, 1.4826, 2.2239, 2.9652,2.9652,2.9652,3.7065}
	for .irow  from 3 to 10 
		.tab = Create Table with column names: "tabel", .irow, "c1"
		Formula: "c1", "row"
		.mad = Get median absolute deviation: "c1"
		assert abs (.mad - .answer#[.irow]) < 0.0001
		;appendInfoLine: .irow, " ", .mad
		removeObject: .tab
	endfor
endproc
		
procedure location_scale
	.chem# ={2.20, 2.20, 2.4, 2.4, 2.5, 2.7, 2.8, 2.9, 3.03, 3.03, 3.10, 3.37, 3.4, 3.4, 3.4, 3.5, 3.6, 3.7, 3.7, 3.7, 3.7,3.77, 5.28, 28.95}
	.tab = Create Table with column names: "tabel", 24, "chem"
	for .irow to 24
		Set numeric value: .irow, "chem", .chem# [.irow]
	endfor
	.report$ = Report robust statistics: "chem", 1.5, 1e-8, 30
	appendInfoLine: .report$
endproc

appendInfoLine: "test_robustStatistics.praat OK"
