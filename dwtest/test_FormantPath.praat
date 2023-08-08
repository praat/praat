# test_FormantPath.praat
# djmw 20230801

appendInfoLine: "test_FormantPath.praat"
@testReadAndWriteVersions
appendInfoLine: "test_FormantPath.praat OK"

procedure testReadAndWriteVersions
	appendInfoLine: tab$, "Reading and writing old version"
	formantPath = Read from file: "sineWithNoise_version0.FormantPath"
	randomName$ = "kanweg" + string$ (randomInteger (100000, 200000)) + ".FormantPath"
	Save as binary file: randomName$
	formantPath2 = Read from file: randomName$
	deleteFile (randomName$)
	removeObject: formantPath, formantPath2
	appendInfoLine: tab$, "Reading and writing old version OK"
endproc

