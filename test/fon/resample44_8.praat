Erase all
Times
12
depth = 200

sweep = Create Sound from formula: "sweep", 1, 0, 10, 44100,
	... ~ sin (2 * pi * 1000 * x^2)
To Spectrogram: 0.05, 20000, 0.002, 20, "Gaussian"
Select outer viewport: 0, 6, 0, 3
Paint: 0, 0, 0, 20000, 100, "yes", 90, 0, 0, "yes"
Remove
selectObject: sweep
sweep_8k = Resample: 8000, 50
To Spectrogram: 0.05, 6000, 0.002, 20, "Gaussian"
Select outer viewport: 0, 6, 3, 6
Paint: 0, 0, 0, 6000, 100, "yes", 90, 0, 0, "yes"
Remove

cutoff = 3600

for ifilter from 0 to 1
	filter_mat [ifilter] = Create Matrix: "filter", -depth / 44100, depth / 44100,
	... depth*2, 1 / 44100, (-depth+0.75-0.5*ifilter) / 44100,
	... 1, 1, 1, 1, 1, ~ if x = 0 then 1 else sin (2*pi*x*cutoff) / (2*pi*x*cutoff)
	... * (0.5 + 0.5 * cos (pi * x*44100 / depth)) fi
	sum = Get sum
	Formula: ~ self / sum
	filter [ifilter] = To Sound
	;Multiply by window: "Hanning"
	plusObject: sweep
	sweep_low [ifilter] = Convolve: "sum", "zero"
endfor

mooi = Create Sound from formula: "mooi", 1, 0, 10, 44100/5.5,
	... ~ object [sweep_low [col mod 2], (col*11+(col mod 2))/2]
To Spectrogram: 0.05, 6000, 0.002, 20, "Gaussian"
Select outer viewport: 0, 6, 6, 9
Paint: 0, 0, 0, 6000, 100, "yes", 90, 0, 0, "yes"
Remove

;exit

#
# Write to Info window in base-0 C format.
#
writeInfoLine: "static double filter_11_2 [2] [", depth*2, "] = {"
for ifilter from 0 to 1
	appendInfo: tab$, "{ "
	for i to depth*2
		value = object [filter [ifilter], i]
		appendInfo: if abs (value) < 1e-12 then 0 else fixed$ (value, 12) fi, ", "
	endfor
	appendInfoLine: "},"
endfor
appendInfoLine: "};"

for ifilter from 0 to 1
	removeObject: filter_mat [ifilter], filter [ifilter], sweep_low [ifilter]
endfor
removeObject: sweep, sweep_8k, mooi
