writeInfoLine: "Table: Rows to columns"

orig = Create formant table (Peterson & Barney 1952)
new = nowarn Rows to columns: "Type Sex Speaker", "IPA", "F0 F1 F2 F3"

numberOfRows = Get number of rows
assert numberOfRows = 76
numberOfColumns = Get number of columns
assert numberOfColumns = 43

removeObject: orig, new

appendInfoLine: "OK"
