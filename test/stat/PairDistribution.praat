writeInfoLine: "PairDistribution"

Read from file: "test.PairDistribution"

numberOfPairs = Get number of pairs
assert numberOfPairs = 117

string1$ = Get string1: 100
assert string1$ = "|cvc.cv:.cv.cv|"
string2$ = Get string2: 100
assert string2$ = "[cvc1.cv:.cv2.cv] \-> /(cV1C.cVV).(cV2.cV)/"

#
# On 3 July 2018, the following checked that std::swap()
# swapped two autostring32 objects correctly:
#

Swap inputs and outputs

string1$ = Get string1: 100
assert string1$ = "[cvc1.cv:.cv2.cv] \-> /(cV1C.cVV).(cV2.cV)/"
string2$ = Get string2: 100
assert string2$ = "|cvc.cv:.cv.cv|"

#

Remove

appendInfoLine: "OK"
