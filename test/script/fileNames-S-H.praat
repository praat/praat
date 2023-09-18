writeInfoLine: "filenames$#"

files$# = fileNames$# ("*.txt")
assert files$# = { "fileNames-S-H1.txt", "fileNames-S-H2.txt", "fileNames-S-H3.txt" }

files$# = fileNames$# ("*.TXT")
assert files$# = { "fileNames-S-H4.TXT", "fileNames-S-H5.TXT" }

files$# = fileNames_caseInsensitive$# ("*.TXT")
assert files$# = { "fileNames-S-H1.txt", "fileNames-S-H2.txt", "fileNames-S-H3.txt",
... "fileNames-S-H4.TXT", "fileNames-S-H5.TXT" }

files$# = fileNames_caseInsensitive$# ("*.tXt")
assert files$# = { "fileNames-S-H1.txt", "fileNames-S-H2.txt", "fileNames-S-H3.txt",
... "fileNames-S-H4.TXT", "fileNames-S-H5.TXT" }

appendInfoLine: "OK"
