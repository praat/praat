# Sound_drawParts_test.praat
# djmw 20091023 

Erase all

# maximally steep
Create Sound from formula... s Mono 0 1 44100 20*((x mod 0.1) - 0.05)
Select outer viewport... 0 6 0 3
Blue
Draw parts... 0 0.4 -1 1 n Curve 10 self>0.5
Black
Draw parts... 0 0.4 -1 1 n Curve 10 self<=0.5
One mark left... 0.5 y y y

# corner cases
Create Sound from formula... s Mono 0 0.0003 10000 0
Select outer viewport... 0 6 3 6
Blue
Formula... if col=1 then -1 else if col=2 then 1 else -1 fi fi
Draw parts... 0 0 -1 1 n Curve 10 self>0.5
Black
Draw parts... 0 0 -1 1 n Curve 10 self<=0.5
One mark left... 0.5 y y y

Formula... if col=1 then 1 else if col=2 then -1 else 1 fi fi
Blue
Draw parts... 0 0 -1 1 n Curve 10 self>0.5

Black
Draw parts... 0 0 -1 1 n Curve 10 self<=0.5
One mark left... 0.5 y y y

# random, selection on amplitudes
Select outer viewport... 0 6 6 9
Create Sound from formula... s Mono 0 0.01 10000 randomUniform(-1,1)
Blue
Draw parts... 0 0 -1 1 n Curve 10 self>0.5
Black
Draw parts... 0 0 -1 1 n Curve 10 self<=0.5
One mark left... 0.5 y y y

# complementary selections in amplitude and time domain
Select outer viewport... 0 6 9 12
Blue
Draw parts... 0 0 -1 1 n Curve 10 (self>0.5 and x mod 0.001 < 0.0005)
Black
Draw parts... 0 0 -1 1 n Curve 10 not (self>0.5 and x mod 0.001 < 0.0005)
One mark left... 0.5 y y y
Marks bottom every... 1 0.0005 n y y
Marks bottom every... 1 0.001 y y y