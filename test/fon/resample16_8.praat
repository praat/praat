Erase all
Times
12
depth = 200

sweep = Create Sound from formula: "sweep", 1, 0, 10, 16000,
	... ~ sin (2 * pi * 400 * x^2)
To Spectrogram: 0.05, 8000, 0.002, 20, "Gaussian"
Select outer viewport: 0, 6, 0, 3
Paint: 0, 0, 0, 8000, 100, "yes", 90, 0, 0, "yes"
Remove
selectObject: sweep
sweep_8k = Resample: 8000, 50
To Spectrogram: 0.05, 8000, 0.002, 20, "Gaussian"
Select outer viewport: 0, 6, 3, 6
Paint: 0, 0, 0, 8000, 100, "yes", 90, 0, 0, "yes"
Remove

cutoff = 3800

filter_mat = Create Matrix: "filter1", -depth / 16000, depth / 16000,
... depth*2, 1 / 16000, (-depth+0.5) / 16000,
... 1, 1, 1, 1, 1, ~ if x = 0 then 1 else sin (2*pi*x*cutoff) / (2*pi*x*cutoff)
... * (0.5 + 0.5 * cos (pi * x*16000 / depth)) fi
sum = Get sum
Formula: ~ self / sum
filter = To Sound

selectObject: sweep, filter
sweep_low = Convolve: "sum", "zero"

mooi = Create Sound from formula: "mooi", 1, 0, 10, 16000/2, ~ object [sweep_low, col*2]
To Spectrogram: 0.05, 8000, 0.002, 20, "Gaussian"
Select outer viewport: 0, 6, 6, 9
Paint: 0, 0, 0, 8000, 100, "yes", 90, 0, 0, "yes"
Remove

#
# Write to Info window in base-0 C format.
#
writeInfo: "static double filter_2 [", depth*2, "] = { "
for i to depth*2
	value = object [filter, i]
	appendInfo: if abs (value) < 1e-12 then 0 else fixed$ (value, 12) fi, ", "
endfor
appendInfoLine: "};"

removeObject: filter_mat, filter, sweep_low, sweep, sweep_8k, mooi
