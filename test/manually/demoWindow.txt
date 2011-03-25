#! Praat script guentherGjajaDemo.praat
# Paul Boersma, 6 May 2009

demo Erase all
demo Black
demo Line width... 1
demo Times
demo 12
demo Select inner viewport... 0 100 0 100
demo Axes... 0 100 0 100
demo Text... 50 centre 50 half Please give this window the right size, then click to start.
demoWaitForInput ( )
# After resizing, the viewport has to be set again.
demo Select inner viewport... 0 100 0 100
demo Erase all
demo Draw line... 0 0 30 70
demoWaitForInput ( )
demo Erase all
demo Red
demo Draw line... 0 0 7 3
demo Text... 50 centre 50 half Click in the sound to stop.
duration = 3
Create Sound from formula... sine Mono 0 duration 1000 sin(2*pi*20*x)
demo Select outer viewport... 0 40 0 40
demo Draw... 0 0 0 0 yes curve
while 1
	demoWaitForInput ( )
	if demoClicked ( )
		# We're gonna show that demoX and demoY can look at the whole window or into a part.
		demo Select inner viewport... 0 100 0 100
		demo Axes... 0 100 0 100
		x1 = demoX ( )
		y1 = demoY ( )
		clickedInTheSound = 0
		if x1 > 0 and x1 < 40 and y1 > 0 and y1 < 40
			demo Select outer viewport... 0 40 0 40
			demo Axes... 0 duration -1 1
			x2 = demoX ( )
			y2 = demoY ( )
			clickedInTheSound = x2 > 0 and x2 < duration and y2 > -1 and y2 < 1
		endif
		if clickedInTheSound
			echo Hit the sound!
			printline 'x2:6' 'y2:6'
			goto end
		else
			echo 'x1:3' 'y1:3'
		endif
	elsif demoKeyPressed ( )
		key$ = demoKey$ ( )
		printline Key <<'key$'>>
	endif
endwhile
label end
demo Erase all
demo Select inner viewport... 0 100 0 100
demo Axes... 0 100 0 100
demo Text... 50 centre 50 half You have finished.
