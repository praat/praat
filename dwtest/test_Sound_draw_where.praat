# test_Sound_draw_where.praat
# djmw 20091031,20100912, 20110524

appendInfoLine: "test_Sound_draw_where.praat"

Erase all

Select outer viewport: 0, 6, 0, 3
	@sawtooth
Select outer viewport: 0, 6, 3, 6
	@three_samples
Select outer viewport: 0, 6, 6, 9
	@three_samples_stereo
Select outer viewport: 0, 6, 9, 12
	@randomAmplitudes

procedure sawtooth
	appendInfoLine: "test_Sound_draw_where.praat"
	# maximally steep
	appendInfoLine: tab$, "Top: sawtooth in black with parts above 0.5 in red"
	s = Create Sound from formula: "s", 1, 0, 1, 44100, "20*((x mod 0.1) - 0.05)"
	ymin = -1.1
	ymax = 1.1
	Colour: "Red"
	Draw where: 0, 0.4, ymin, ymax, "no", "Curve", "self>0.5"
	Colour: "Black"
	Draw where: 0, 0.4, ymin, ymax, "no", "Curve", "self<=0.5"
	One mark left: 0.5, "yes", "yes", "yes", ""
	Draw inner box
	Text top: "no", "sawtooth in black with parts above 0.5 in red"
	removeObject: s
endproc

procedure three_samples
	appendInfoLine: tab$, "Interpolation between 3 sample points: parts above 0.5 in red"
	s = Create Sound from formula: "s", 1, 0, 0.0003, 10000, "0"
	Colour: "Red"
	Formula: "if col=2 then -1 else 1 fi"
	Draw where: 0, 0, ymin, ymax, "no", "Curve", "self>0.5"
	Colour: "Black"
	Draw where: 0, 0, ymin, ymax, "no", "Curve", "self<=0.5"
	One mark left: 0.5, "yes", "yes", "yes", ""

	Formula: "if col=2 then 1 else -1 fi"
	Colour: "Red"
	Draw where: 0, 0, ymin, ymax, "no", "Curve", "self>0.5"
	Colour: "Black"
	Draw where: 0, 0, ymin, ymax, "no", "Curve", "self<=0.5"
	Draw inner box
	Text top: "no", "Interpolation between 3 sample points: parts above 0.5 in red"
	removeObject: s
endproc

procedure three_samples_stereo
	appendInfoLine: tab$, "Stereo, interpolation between sample points:  three sample points, parts above 0.5 in Red"
	s = Create Sound from formula: "s", 2, 0, 0.0003, 10000, "0"
	Formula: "if row = 1 then if col=2 then -1 else 1 fi else self fi"
	Formula: "if row = 2 then if col=2 then 1 else -1 fi else self fi"
	Colour: "Red"
	Draw where: 0, 0, ymin, ymax, "no", "Curve", "self>0.5"
	Colour: "Black"
	Draw where: 0, 0, ymin, ymax, "yes", "Curve", "self<=0.5"
	Formula: "0.6"
	Colour: "Red"
	Draw where: 0, 0, ymin, ymax, "no", "Curve", "1"
	Colour: "Black"
	Draw where: 0, 0, ymin, ymax, "no", "Curve", "0"
	Text top: "no", "Parts above 0.5 in red. One line at 0.6"
	removeObject: s
endproc

procedure randomAmplitudes
	s = Create Sound from formula: "s", 1, 0, 0.01, 10000, "randomUniform (-1, 1)"
	appendInfoLine: tab$, "Random amplitudes: parts > 0.5 in red only in the first half of every 0.001 s"
	Select outer viewport: 0, 6, 9, 12
	Colour: "Red"
	Draw where: 0.001, 0.01, ymin, ymax, "no", "Curve", "(self>0.5 and x mod 0.001 < 0.0005)"
	Colour: "Black"
	Draw where: 0.001, 0.01, ymin, ymax, "no", "Curve", "not (self>0.5 and x mod 0.001 < 0.0005)"
	One mark left: 0.5, "yes", "yes", "yes", ""
	Marks bottom every: 1, 0.0005, "no", "yes", "yes"
	Marks bottom every: 1, 0.001, "yes", "yes", "yes"
	Draw inner box
	Text top: "no", "Random amplitudes: parts > 0.5 red in the first half of every 0.001 s"
	Colour: "Black"
	removeObject: s
endproc

appendInfoLine: "test_Sound_draw_where.praat End"

