# recursiveRunScript.praat
# Paul Boersma 2020-03-20, 2023-06-08

# After you click Apply 20 times,
# there should be a neat error message,
# rather than a crash.

beginPause: "Recursive runScript()"
	comment: "Click Apply or OK 20 times, and see the message."
clicked = endPause: "Cancel", "Apply", "OK", 3, 1
 
if clicked == 1
	exitScript()
endif
 
if clicked == 2 or clicked = 3
	runScript: "recursiveRunScript.praat"
endif
