# test_TableDrawWhere.praat
# djmw 20220612

appendInfoLine: "test_TableDrawWhere.praat"

table = Create formant table (Weenink 1985)
Erase all
Select outer viewport: 0, 3, 0, 3
Draw ellipses where: "F2", 0, 0, "F1", 0,0, "Vowel", 1.0, 12, "yes", "1"
Select outer viewport: 3, 6, 0, 3
Draw ellipses where: "F2", 600, 3000, "F1", 0, 0, "Vowel", 1.0, 12, "yes", "1"
Select outer viewport: 0, 3, 3, 6
Draw ellipses where: "F2", 0, 0, "F1", 1200, 250, "Vowel", 1.0, 12, "yes", "1"
Select outer viewport: 3, 6 , 3, 6
Draw ellipses where: "F2", 600, 3000, "F1", 1200, 250, "Vowel", 1.0, 12, "yes", "1"
Select outer viewport: 0, 3, 6, 9
Draw ellipses where: "F2", 600, 3000, "F1", 1200, 250, "Vowel", 1.0, 12, "yes",  ~ self$ ["Sex"] = "m"
Select outer viewport: 3, 6, 6, 9
Draw ellipses where: "F2", 600, 3000, "F1", 1200, 250, "Vowel", 1.0, 12, "yes",  ~ self$ ["Sex"] = "f"

removeObject: table
appendInfoLine: "test_TableDrawWhere.praat OK"