# TextGrid_wordBoundaries.praat

Read from file: "wordBoundaries.TextGrid"

n = Count intervals where: 1, "is equal to", "hallo"
assert n = 0

n = Count intervals where: 1, "is equal to", "This is a test for word boundaries"
assert n = 1

n = Count intervals where: 1, "contains a word equal to", "hallo"
assert n = 0

n = Count intervals where: 1, "contains a word equal to", "This is a test for word boundaries"
assert n = 1

n = Count intervals where: 1, "contains a word equal to", "test"
assert n = 1

n = Count intervals where: 1, "contains a word equal to", "tes"
assert n = 0

n = Count intervals where: 1, "contains a word starting with", "test"
assert n = 1

n = Count intervals where: 1, "contains a word starting with", "tes"
assert n = 1

n = Count intervals where: 1, "contains a word ending with", "est"
assert n = 1

n = Count intervals where: 1, "contains a word ending with", "test"
assert n = 1

n = Count intervals where: 1, "contains a word equal to", "This"
assert n = 1

n = Count intervals where: 1, "contains a word equal to", "This is"
assert n = 1

n = Count intervals where: 1, "contains a word equal to", "This i"
assert n = 0

n = Count intervals where: 1, "contains a word equal to", "boundaries"
assert n = 1

n = Count intervals where: 1, "contains a word equal to", "oundaries"
assert n = 0

n = Count intervals where: 1, "contains a word equal to", "ord boundaries"
assert n = 0

n = Count intervals where: 1, "contains a word equal to", "d boundaries"
assert n = 0

n = Count intervals where: 1, "contains a word equal to", "word boundaries"
assert n = 1

Remove