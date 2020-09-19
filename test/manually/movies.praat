# test/manually/movies.praat
# Paul Boersma 2020-09-19

## Matrix movies.

matrix = Create simple Matrix: "xy", 100, 100, ~ x*y
pauseScript: "watch a line with a slope that rises in time."
Play movie
Remove

## Articulatory synthesis.

speaker = Create Speaker: "speaker", "female", "2"
artword = Create Artword: "oko", 1.0
Set target: 0.0, 0.8, "Lungs"
Set target: 0.3, 0.3, "Lungs"
Set target: 1.0, 0.0, "Lungs"
Set target: 0.0, 0.5, "Interarytenoid"
Set target: 1.0, 0.5, "Interarytenoid"
Set target: 0.5, 0.9, "Styloglossus"

selectObject: speaker, artword
pauseScript: "Watch a vocal tract saying [oko]."
Play movie

pauseScript: "Watch the vocal folds move while the sound is computed."
sound = To Sound: 22050, 25, 0,0,0,0,0,0,0,0,0

selectObject: speaker, artword, sound
pauseScript: "Watch and hear a vocal tract saying [oko]."
Play movie
Remove

## OTGrammar.

grammar = Create place assimilation grammar
distribution = Create place assimilation distribution
selectObject: grammar, distribution
pauseScript: "Watch the constraint rankings evolve."
Learn: 2.0, "symmetric all", 1.0, 1e6, 0.1, 4, 0.1, "yes", 1
Remove

## Minimizers.
