# Sound_drawWhere_test.praat
# djmw 20091031,20100912, 20110524

Erase all

appendInfoLine: "Sound_drawWhere_test.praat"
# maximally steep
appendInfoLine: tab$, "Top: sawtooth in blue with parts above 0.5 in red + red line"
s = Create Sound from formula: "s", 1, 0, 1, 44100, "20*((x mod 0.1) - 0.05)"
ymin = -1.1
ymax = 1.1
Select outer viewport: 0, 6, 0, 3
Colour: "Red"
Draw where: 0, 0.4, ymin, ymax, "no", "Curve", "self>0.5"
Colour: "Blue"
Draw where: 0, 0.4, ymin, ymax, "no", "Curve", "self<=0.5"
One mark left: 0.5, "yes", "yes", "yes", ""
Draw inner box
Text top: "no", "sawtooth in blue with parts above 0.5 in red"
removeObject: s

# corner cases
appendInfoLine: tab$, "Second figure, interpolation between sample points:  three sample points, parts above 0.5 in red"
s = Create Sound from formula: "s", 1, 0, 0.0003, 10000, "0"
Select outer viewport: 0, 6, 3, 6
Colour: "Red"
Formula: "if col=2 then -1 else 1 fi"
Draw where: 0, 0, ymin, ymax, "no", "Curve", "self>0.5"
Colour: "Blue"
Draw where: 0, 0, ymin, ymax, "no", "Curve", "self<=0.5"
One mark left: 0.5, "yes", "yes", "yes", ""

Formula... if col=2 then 1 else -1 fi
Colour: "Red"
Draw where: 0, 0, ymin, ymax, "no", "Curve", "self>0.5"
Colour: "Blue"
Draw where: 0, 0, ymin, ymax, "no", "Curve", "self<=0.5"
One mark left: 0.5, "yes", "yes", "yes", ""

# Compatibility with Draw... : only draw in red
Formula: "0.6"
Colour: "Red"
Draw where: 0, 0, ymin, ymax, "no", "Curve", "1"
Colour: "Blue"
Draw where: 0, 0, ymin, ymax, "no", "Curve", "0"
Draw inner box
Text top: "no",  "(2x) three sample points, parts above 0.5 in red + red line"
removeObject: s

appendInfoLine: tab$, "Random amplitudes: parts > 0.5 in red"
Select outer viewport: 0, 6, 6, 9
s = Create Sound from formula: "s", 1, 0, 0.01, 10000, "randomUniform(-1,1)"
Colour: "Red"
Draw where: 0, 0, ymin, ymax, "no", "Curve", "self>0.5"
Colour: "Blue"
Draw where: 0, 0, ymin, ymax, "no", "Curve", "self<=0.5"
One mark left: 0.5, "yes", "yes", "yes", ""
Draw inner box
Text top: "no", "Random amplitudes: parts > 0.5 in red"

appendInfoLine: tab$, "Random amplitudes: parts > 0.5 in red only in the first half of every 0.001 s"
Select outer viewport: 0, 6, 9, 12
Colour: "Red"
Draw where: 0.001, 0.01, ymin, ymax, "no", "Curve", "(self>0.5 and x mod 0.001 < 0.0005)"
Colour: "Blue"
Draw where: 0.001, 0.01, ymin, ymax, "no", "Curve", "not (self>0.5 and x mod 0.001 < 0.0005)"
One mark left: 0.5, "yes", "yes", "yes", ""
Marks bottom every: 1, 0.0005, "no", "yes", "yes"
Marks bottom every: 1, 0.001, "yes", "yes", "yes"
Draw inner box
Text top: "no", "Random amplitudes: parts > 0.5 red in the first half of every 0.001 s"
removeObject: s

Colour: "Black"
appendInfoLine: "Sound_drawWhere_test.praat End"

