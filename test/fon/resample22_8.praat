Erase all
Times
12
depth = 200

sweep = Create Sound from formula: "sweep", 1, 0, 10, 22050,
	... ~ sin (2 * pi * 500 * x^2)
To Spectrogram: 0.05, 10000, 0.002, 20, "Gaussian"
Select outer viewport: 0, 6, 0, 3
Paint: 0, 0, 0, 10000, 100, "yes", 90, 0, 0, "yes"
Remove
selectObject: sweep
sweep_8k = Resample: 8000, depth
To Spectrogram: 0.05, 6000, 0.002, 20, "Gaussian"
Select outer viewport: 0, 6, 3, 6
Paint: 0, 0, 0, 6000, 100, "yes", 90, 0, 0, "yes"
Remove

cutoff = 3600

for ifilter from 0 to 3
	filter_mat [ifilter] = Create Matrix: "filter", -depth / 22050, depth / 22050,
	... depth*2, 1 / 22050, (-depth+0.875-0.25*ifilter) / 22050,
	... 1, 1, 1, 1, 1, ~ if x = 0 then 1 else sin (2*pi*x*cutoff) / (2*pi*x*cutoff)
	... * (0.5 + 0.5 * cos (pi * x*22050 / depth)) fi
	sum = Get sum
	Formula: ~ self / sum
	filter [ifilter] = To Sound
	plusObject: sweep
	sweep_low [ifilter] = Convolve: "sum", "zero"
endfor

mooi = Create Sound from formula: "mooi", 1, 0, 10, 22050/2.75,
	... ~ object [sweep_low [col mod 4], (col*11+(col mod 4))/4]   ; 3 6 9 11 14 17 20 22...
To Spectrogram: 0.05, 6000, 0.002, 20, "Gaussian"
Select outer viewport: 0, 6, 6, 9
Paint: 0, 0, 0, 6000, 100, "yes", 90, 0, 0, "yes"
Remove

;exit

#
# Write to Info window in base-0 C format.
#
writeInfoLine: "static double filter_11_4 [4] [", depth*2, "] = {"
for ifilter from 0 to 3
	appendInfo: tab$, "{ "
	for i to depth*2
		value = object [filter [ifilter], i]
		appendInfo: if abs (value) < 1e-12 then 0 else fixed$ (value, 12) fi, ", "
	endfor
	appendInfoLine: "},"
endfor
appendInfoLine: "};"

for ifilter from 0 to 3
	removeObject: filter_mat [ifilter], filter [ifilter], sweep_low [ifilter]
endfor
removeObject: sweep, sweep_8k, mooi
