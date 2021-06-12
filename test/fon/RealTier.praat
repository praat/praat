# RealTier.praat
# Paul Boersma 2021-06-12

writeInfoLine: "RealTier"

table = Read Table from tab-separated file: "RealTierTable.txt"
To RealTier: "Time", "Value", 0, undefined
Remove

matrix = Read Matrix from raw text file: "RealTierMatrix.txt"
To RealTier: 1, 2, 0, undefined
Remove

removeObject: table, matrix

appendInfoLine: "OK"
