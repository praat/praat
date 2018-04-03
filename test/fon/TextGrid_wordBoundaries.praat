# TextGrid_wordBoundaries.praat

Read from file: "wordBoundaries.TextGrid"

n = Count intervals where: 1, "is equal to", "hallo"
assert n = 0

n = Count intervals where: 1, "is equal to", "This is a test for word boundaries"
assert n = 1

n = Count intervals where: 1, "contains the word", "hallo"
assert n = 0

n = Count intervals where: 1, "contains the word", "This is a test for word boundaries"
assert n = 1

n = Count intervals where: 1, "contains the word", "test"
assert n = 1

n = Count intervals where: 1, "contains the word", "tes"
assert n = 0

n = Count intervals where: 1, "contains a word starting with", "test"
assert n = 1

n = Count intervals where: 1, "contains a word starting with", "tes"
assert n = 1

n = Count intervals where: 1, "contains a word ending with", "est"
assert n = 1

n = Count intervals where: 1, "contains a word ending with", "test"
assert n = 1

n = Count intervals where: 1, "contains the word", "This"
assert n = 1

n = Count intervals where: 1, "contains the word", "This is"
assert n = 1

n = Count intervals where: 1, "contains the word", "This i"
assert n = 0

n = Count intervals where: 1, "contains the word", "boundaries"
assert n = 1

n = Count intervals where: 1, "contains the word", "oundaries"
assert n = 0

n = Count intervals where: 1, "contains the word", "d boundaries"
assert n = 0

n = Count intervals where: 1, "contains the word", "word boundaries"
assert n = 1

Remove