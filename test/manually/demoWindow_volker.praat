### START SCRIPT ###

demo Select inner viewport: 0, 100, 0, 100
demo Axes: 0, 100, 0, 100			

# Present red screen:
label START
demo Erase all
demo Paint rectangle: "Maroon", 0, 100, 0, 100
demoShow()

# Delay the red screen by 0.5 sec:
Create Sound from formula... delay 1 0 0.5 1000 0
Play
Remove

# Present green screen with buttons:
demo Erase all
demo Paint rectangle: "Green", 0, 100, 0, 100
demo Black
demo Paint rounded rectangle... silver 40 60 5 10 3
demo Text special... 50 centre 7.5 half Helvetica 20 0 repeat
demo Paint rounded rectangle... silver 93 98 2 8 3
demo Text special... 95.5 centre 4.5 half Helvetica 15 0 EXIT
demoShow()

# Get user input:
while demoWaitForInput ()
	if demoClickedIn (40, 60, 5, 10)
		goto START
	elsif demoClickedIn (93, 98, 2, 8)
		goto END
	endif
endwhile

label END
demo Erase all

