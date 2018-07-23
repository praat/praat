# test/fon/TextGrid.praat
# Paul Boersma, 19 November 2013

do ("Create TextGrid...", 0.0, 3.0, "A B C", "")
#Create TextGrid: 0.0, 3.0, "A B C", ""

i = Get interval at time... 2 -1
assert i = 0
i = Get interval at time... 2 0
assert i = 1
i = Get interval at time... 2 1
assert i = 1
i = Get interval at time... 2 2
assert i = 1
i = Get interval at time... 2 3
assert i = 1
i = Get interval at time... 2 4
assert i = 0

i = Get low interval at time... 2 -1
assert i = 0
i = Get low interval at time... 2 0
assert i = 0
i = Get low interval at time... 2 1
assert i = 1
i = Get low interval at time... 2 2
assert i = 1
i = Get low interval at time... 2 3
assert i = 1
i = Get low interval at time... 2 4
assert i = 0

i = Get high interval at time... 2 -1
assert i = 0
i = Get high interval at time... 2 0
assert i = 1
i = Get high interval at time... 2 1
assert i = 1
i = Get high interval at time... 2 2
assert i = 1
i = Get high interval at time... 2 3
assert i = 0
i = Get high interval at time... 2 4
assert i = 0

i = Get interval edge from time... 2 -1
assert i = 0
i = Get interval edge from time... 2 0
assert i = 1
i = Get interval edge from time... 2 1
assert i = 0
i = Get interval edge from time... 2 2
assert i = 0
i = Get interval edge from time... 2 3
assert i = 1
i = Get interval edge from time... 2 4
assert i = 0

i = Get interval boundary from time... 2 -1
assert i = 0
i = Get interval boundary from time... 2 0
assert i = 0
i = Get interval boundary from time... 2 1
assert i = 0
i = Get interval boundary from time... 2 2
assert i = 0
i = Get interval boundary from time... 2 3
assert i = 0
i = Get interval boundary from time... 2 4
assert i = 0

Insert boundary: 2, 1.0
Insert boundary: 2, 2.0

i = Get interval at time... 2 -1
assert i = 0
i = Get interval at time... 2 0
assert i = 1
i = Get interval at time... 2 1
assert i = 2
i = Get interval at time... 2 2
assert i = 3
i = Get interval at time... 2 3
assert i = 3
i = Get interval at time... 2 4
assert i = 0

i = Get low interval at time... 2 -1
assert i = 0
i = Get low interval at time... 2 0
assert i = 0
i = Get low interval at time... 2 1
assert i = 1
i = Get low interval at time... 2 2
assert i = 2
i = Get low interval at time... 2 3
assert i = 3
i = Get low interval at time... 2 4
assert i = 0

i = Get high interval at time... 2 -1
assert i = 0
i = Get high interval at time... 2 0
assert i = 1
i = Get high interval at time... 2 1
assert i = 2
i = Get high interval at time... 2 2
assert i = 3
i = Get high interval at time... 2 3
assert i = 0
i = Get high interval at time... 2 4
assert i = 0

i = Get interval edge from time... 2 -1
assert i = 0
i = Get interval edge from time... 2 0
assert i = 1
i = Get interval edge from time... 2 1
assert i = 2
i = Get interval edge from time... 2 2
assert i = 3
i = Get interval edge from time... 2 3
assert i = 3
i = Get interval edge from time... 2 4
assert i = 0

i = Get interval boundary from time... 2 -1
assert i = 0
i = Get interval boundary from time... 2 0
assert i = 0
i = Get interval boundary from time... 2 1
assert i = 2
i = Get interval boundary from time... 2 2
assert i = 3
i = Get interval boundary from time... 2 3
assert i = 0
i = Get interval boundary from time... 2 4
assert i = 0

Remove
