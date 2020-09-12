assert extractNumber ("Number: 234", "Number:") = 234
assert extractNumber ("Number: 234", "Number: ") = 234

assert extractNumber ("Number: 1/2", "Number: ") = 0.5
assert extractNumber ("Number: 1 / 2", "Number: ") = 1

#
# Funny stuff.
#
assert extractNumber ("Number: 1X/2U", "Number: ") = 0.5
assert extractNumber ("Number: 1X/U", "Number: ") = undefined

assert extractNumber ("Number: X/U", "Number: ") = undefined

assert extractNumber ("Number: X/2U", "Number: ") = undefined

assert extractNumber ("Number: X", "Number: ") = undefined
