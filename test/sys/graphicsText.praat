# graphicsText.praat
# Paul Boersma 2020-11-11

Erase all
Select outer viewport: 0, 9, 0, 9
Axes: 0, 100, 50, 100
Times
12

width = Text width (mm): "Hello world"
writeInfoLine: "width ", width, " mm"

Text: 0, "left", 99, "half", "Getting graphical text right is not a trivial task:"
Text: 0, "left", 98, "half", "many types of glitches are possible."
Text: 0, "left", 96, "half", "Code might be misaligned as in: $$< > <> <= = >=$, you see?"
Courier
text$ = "Code might be misaligned as in: $$< > <> <= = >=$, you see?"
Text: 0, "left", 95, "half", text$
width = Text width (mm): text$
appendInfoLine: "width ", width, " mm"
width = Text width (world coordinates): text$
appendInfoLine: "width ", width
Text: width, "left", 94, "half", "... and here it continues:"
Text: 0, "left", 93, "half", "a / b, a/b, a / b, a/b, a / b, a/b, a / b, a /b, a/ b"
Times
Text: 0, "left", 92, "half", "a / b, a/b, a / b"
Courier
Text: 0, "left", 90, "half", "The space might be too small in: Now %%you% try."
Times
Text: 0, "left", 89, "half", "The space might be too small in: Now %%you% try."

Text: 0, "left", 88, "half", "A lot of normal text might cause ##misalignment# misalignment misalignment of the f in: (%%false%)"
Text: 0, "left", 87, "half", "(%%false%) now at the beginning of the line"

Save as 600-dpi PNG file: "kanweg.png"
Save as EPS file: "kanweg.eps"

width = Text width (mm): "Hello world"
assert width > 18.0 and width < 24.0   ; 'width'
appendInfoLine: "width ", width, " mm"
appendInfoLine: "OK"
