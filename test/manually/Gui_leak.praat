for i to 1000
	writeInfoLine: i
	Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
	View & Edit
	Remove
endfor
pause 58473589
