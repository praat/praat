time = 0;0.015
demo Times
demo 12
demo Select inner viewport: 0, 100, 0, 100
demo Axes: 0, 1, 0, 1
n = 100
for i to n
	demo Erase all
	demoShow()
	demo Paint rectangle: "red", (i-1)/n, i/n, 0.4, 0.6
	sleep (time)
	demoShow()
	sleep (0.3)
endfor
