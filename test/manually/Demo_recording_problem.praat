	# Initialize
    demo Erase all

	demo Select inner viewport... 5 95 5 95
	demo Axes... 0 100 0 100

	# Recording text
	demo Font size: 24
	demo Colour: "Black"
	demo Text: 50, "Centre", 50, "Bottom", "Recording started: You should see a red disk during recording"
    demoShow()
    
	# Recording light
    demo Paint circle: "Red", 5, 95, 2
    demoShow()
    
	.sound = Record Sound (fixed time): "Microphone", 0.99, 0.5, "44100", 5
	Remove
	
	# Clear text
	demo Paint rectangle: "White", 0, 100, 45, 60

 	# Finish light
	demo Paint circle: "Blue", 10, 95, 2

	# Finish text
	demo Font size: 24
	demo Colour: "Black"
	demo Text: 50, "Centre", 50, "Bottom", "Recording stopped: Now you should see a red and a blue disk"
	demo Text: 50, "Centre", 45, "Bottom", "(Click anywhere in the window or hit any key to erase and exit)"

	# Wait and exit
	demoWaitForInput()
	demo Erase all
	demoShow()
	exit
