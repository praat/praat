# graphicsText.praat
# Paul Boersma 2021-10-27

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
Text: 0, "left", 90, "half", "The space might be too small in: Now %%you% try. and/or"
Times
Text: 0, "left", 89, "half", "The space might be too small in: Now %%you% try. and/or"

Text: 0, "left", 88, "half", "A lot of normal text might cause ##misalignment# misalignment misalignment of the f in: (%%false%)"
Text: 0, "left", 87, "half", "(%%false%) now at the beginning of the line"

Text: 0, "left", 85, "half", "n\|v b\0v o\Tv o\T^ o\T( o\T) e\-v o\+v o\:v o\~v d\Nv d\Uv u\nv e\3v u\cv///"
Text: 0, "left", 83, "half", "\gf\0^ \ef\'^ \ef\`^ \ef\-^ \ef\~^ \ef\v^ \ef\^^ o\:^ e\x^ \ef\N^ k\lip t\lis a\_ub///"
Text: 0, "left", 81, "half", "t\^h b\^H t\^g k\^w t\^Y a\^? t\^9 t\^l t\^n p\^m k\^N t\^s k\^x p\^f t\^y///"
Text: 0, "left", 79, "half", "\ts \tS///"

Save as 600-dpi PNG file: "kanweg.png"
Save as EPS file: "kanweg.eps"



width = Text width (mm): "Hello world"
assert width > 18.0 and width < 26.0   ; 'width'
appendInfoLine: "width ", width, " mm"
appendInfoLine: "OK"
