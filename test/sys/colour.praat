# colour.praat
# Paul Boersma 2019-11-05

Times
10
Erase all
Select outer viewport: 0, 6, 0, 9
Axes: 0, 100, 0, 100

#
# Old syntax.
#

y = 100

y -= 5
Green
Text... 50 right y half This is blue:
Paint rectangle... blue 50 100 y-2 y+2

y -= 5
Blue
Text... 50 right y half This is 0.25 grey:
Paint rectangle... 0.5-0.25 50 100 y-2 y+2

y -= 5
Yellow
Text... 50 right y half This is teal:
Paint rectangle... {0.25,0.1,0.2} 50 100 y-2 y+2

#
# New syntax.
#

y -= 2

y -= 5
Black
Text: 50, "right", y, "half", "This is red:"
Paint rectangle: "red", 50, 100, y-2, y+2

y -= 5
Blue
Text: 50, "right", y, "half", "This is 0.75 grey:"
Paint rectangle: 0.25+0.5, 50, 100, y-2, y+2

y -= 5
Maroon
Text: 50, "right", y, "half", "This is lime:"
Paint rectangle: "lime", 50, 100, y-2, y+2

y -= 5
Pink
Text: 50, "right", y, "half", "This is brown:"
Paint rectangle: "{ 0.1, 0.3, 0.2 }", 50, 100, y-2, y+2

y -= 5
Maroon
Text: 50, "right", y, "half", "This is lila:"
Paint rectangle: {0.3+0.4, 0.6, 0.8}, 50, 100, y-2, y+2

y -= 5
Navy
Text: 50, "right", y, "half", "This is 0.3 grey:"
Paint rectangle: {0.1+0.2}, 50, 100, y-2, y+2

y -= 5
Olive
Text: 50, "right", y, "half", "This is not drawn:"
asserterror Cannot compute a colour
Paint rectangle: "{ vxcvxcvbxvbcvb", 50, 100, y-2, y+2

y -= 5
Olive
Text: 50, "right", y, "half", "This is not drawn:"
asserterror Cannot compute a colour
Paint rectangle: "vxcvxcvbxvbcvb", 50, 100, y-2, y+2

y -= 5
Olive
Text: 50, "right", y, "half", "This is not drawn:"
asserterror Cannot compute a colour
Paint rectangle: "0.3", 50, 100, y-2, y+2

