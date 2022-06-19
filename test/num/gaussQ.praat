echo gaussQ
values# = { 1e-100, 1e-40, 1e-20, 1e-10, 1e-6, 1e-5, 0.0001, 0.001, 0.01, 0.05, 0.10, 0.25, 0.50,
... 0.75, 0.90, 0.95, 0.99, 0.999, 0.9999, 0.99999, 0.999999, 1-1e-10 }
for i to size (values#)
	assert abs (gaussQ (invGaussQ (values# [i])) - values# [i]) < 1e-15
	assert abs (gaussQ (invGaussQ (values# [i])) - values# [i]) < 1e-13 * values# [i]
endfor
printline OK