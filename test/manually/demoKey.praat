demo Select outer viewport: 0, 100, 0, 100
demo Axes: 0, 100, 0, 100
demo Erase all
demo Font size: 15
demo Text: 50, "centre", 80, "half",
... "Type a key."
demo Black
while demoWaitForInput ( )
	if demoKeyPressed ( )
		car$ = demoKey$ ( )
		demo Erase all
		demo Text: 50, "centre", 80, "half",
		... "Type a key."
		if praatVersion < 6000
			demo Text: 50, "centre", 40, "half", "Key pressed: (" + car$ + ")"
		elsif praatVersion < 6100
			demo Text: 50, "centre", 40, "half", "Key pressed: " + string$ (unicode (car$), 8) + " (" + car$ + ")"
		else
			demo Text: 50, "centre", 40, "half", "Key pressed: 0x" + hexadecimal$ (unicode (car$), 8) + " (" + car$ + ")"
		endif
		demo Text: 50, "centre", 35, "half", "Shift key pressed: " + if demoShiftKeyPressed() then "yes" else "no" fi
		demo Text: 50, "centre", 30, "half", "Command key pressed: " + if demoCommandKeyPressed() then "yes" else "no" fi
		demo Text: 50, "centre", 25, "half", "Option key pressed: " + if demoOptionKeyPressed() then "yes" else "no" fi
	endif
endwhile
