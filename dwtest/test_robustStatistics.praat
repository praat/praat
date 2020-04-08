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
	.tab = Create Table with column names: "tabel", 14, "data"
	Formula: "data", "row"
	Set numeric value: 14, "data", 20
	.report$ = Report robust statistics: "data", 1.5, 1e-8, 30
	.location = extractNumber (.report$, "Location:")
	.scale = extractNumber (.report$, "Scale:")
	;# compare with R: hubers(c(1:13, 20))
	assert abs(.location - 7.56108) < 0.00001
	assert abs(.scale - 4.86268) < 0.00001
	removeObject: .tab
endproc

appendInfoLine: "test_robustStatistics.praat OK"
