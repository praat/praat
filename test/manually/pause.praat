# Henning Reetz, bug report May 2021
# After running this script, one should be able to close the pause window

beginPause: "Invisible pause"
	comment: "Just hit 'return' on your keyboard"
		word: "Something", ""
clicked = endPause: "Stop", "Continue", 2, 1

printline 'clicked'
