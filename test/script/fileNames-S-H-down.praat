writeInfoLine: "filenames$#"

files$# = fileNames$# ("down/*")
appendInfoLine: files$#

files$# = fileNames$# ("down")
appendInfoLine: files$#

files$# = fileNames$# ("down/*.txt")
appendInfoLine: files$#
assert files$# = { "down-S-H1.txt", "down-S-H2.txt", "down-S-H3.txt" }

files$# = fileNames$# ("down/*.TXT")
appendInfoLine: files$#
assert files$# = { "down-S-H4.TXT", "down-S-H5.TXT" }

files$# = fileNames_caseInsensitive$# ("down/*.TXT")
appendInfoLine: files$#
assert files$# = { "down-S-H1.txt", "down-S-H2.txt", "down-S-H3.txt",
... "down-S-H4.TXT", "down-S-H5.TXT" }

files$# = fileNames_caseInsensitive$# ("down/*.tXt")
appendInfoLine: files$#
assert files$# = { "down-S-H1.txt", "down-S-H2.txt", "down-S-H3.txt",
... "down-S-H4.TXT", "down-S-H5.TXT" }

appendInfoLine: "OK"
