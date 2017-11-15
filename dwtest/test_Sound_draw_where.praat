# Sound_drawWhere_test.praat
# djmw 20091031,20100912, 20110524

Erase all

printline Draw where... Begin tests
# maximally steep
printline   Top: sawtooth in blue with parts above 0.5 in red
s = Create Sound from formula... s Mono 0 1 44100 20*((x mod 0.1) - 0.05)
ymin = -1.1
ymax = 1.1
Select outer viewport... 0 6 0 3
Red
Draw where... 0 0.4 ymin ymax n Curve self>0.5
Blue
Draw where... 0 0.4 ymin ymax n Curve self<=0.5
One mark left... 0.5 y y y
Draw inner box
Text top... n sawtooth in blue with parts above 0.5 in red
Remove

# corner cases
printline   Second figure, interpolation between sample points:  three sample points, parts above 0.5 in red
s =Create Sound from formula... s Mono 0 0.0003 10000 0
Select outer viewport... 0 6 3 6
Red
Formula... if col=2 then -1 else 1 fi
Draw where... 0 0 ymin ymax n Curve self>0.5
Blue
Draw where... 0 0 ymin ymax n Curve self<=0.5
One mark left... 0.5 y y y

Formula... if col=2 then 1 else -1 fi
Red
Draw where... 0 0 ymin ymax n Curve self>0.5
Blue
Draw where... 0 0 ymin ymax n Curve self<=0.5
One mark left... 0.5 y y y

# Compatibility with Draw... : only draw in red
Formula... 0.6
Red
Draw where... 0 0 ymin ymax n Curve 1
Blue
Draw where... 0 0 ymin ymax n Curve 0
Draw inner box
Text top... n  (2x) three sample points, parts above 0.5 in red + red line
Remove


# random, selection on amplitudes
printline   Random amplitudes: parts > 0.5 in red
Select outer viewport... 0 6 6 9
s = Create Sound from formula... s Mono 0 0.01 10000 randomUniform(-1,1)
Red
Draw where... 0 0 ymin ymax n Curve self>0.5
Blue
Draw where... 0 0 ymin ymax n Curve self<=0.5
One mark left... 0.5 y y y
Draw inner box
Text top... n Random amplitudes: parts > 0.5 in red

# complementary selections in amplitude and time domain
printline   Random amplitudes: parts > 0.5 in red only in the first half of every 0.001 s
Select outer viewport... 0 6 9 12
Red
Draw where... 0.001 0.01 ymin ymax n Curve (self>0.5 and x mod 0.001 < 0.0005)
Blue
Draw where... 0.001 0.01 ymin ymax n Curve not (self>0.5 and x mod 0.001 < 0.0005)
One mark left... 0.5 y y y
Marks bottom every... 1 0.0005 n y y
Marks bottom every... 1 0.001 y y y
Draw inner box
Text top... n Random amplitudes: parts > 0.5 red in the first half of every 0.001 s
Remove
printline Draw where... End tests

