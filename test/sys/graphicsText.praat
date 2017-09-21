# graphicsText.praat
# Paul Boersma 2017-09-19

Erase all
Select outer viewport: 0, 6, 0, 9
Axes: 0, 100, 50, 100
Times
12
Text: 0, "left", 99, "half", "Getting graphical text right is not a trivial task:"
Text: 0, "left", 98, "half", "many types of glitches are possible."
Text: 0, "left", 95, "half", "Code might be misaligned as in: $$< > <> <= = >=$, you see?"
Courier
text$ = "Code might be misaligned as in: $$< > <> <= = >=$, you see?"
Text: 0, "left", 94, "half", text$
width = Text width (world coordinates): text$
Text: width, "left", 94, "half", "... and here it continues"
Text: 0, "left", 93, "half", "a / b, a/b, a / b, a/b, a / b, a/b, a / b, a /b, a/ b"
Times
Text: 0, "left", 92, "half", "a / b, a/b, a / b"
Courier
Text: 0, "left", 90, "half", "The space might be too small in: Now %%you% try."
Times
Text: 0, "left", 89, "half", "The space might be too small in: Now %%you% try."

Text: 0, "left", 88, "half", "A lot of normal text might cause ##misalignment# misalignment misalignment of the f in: (%%false%)"
Text: 0, "left", 87, "half", "(%%false%) now at the beginning of the line"

