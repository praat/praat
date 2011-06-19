Erase all
Times
12
Viewport... 0 6 0 6

Create simple Matrix... hundred 2 100 randomUniform(0,100)
To Polygon
Salesperson... 1000
Draw closed... 0 0 0 0

Create simple Matrix... circle 2 100000
... if row=1 then cos(2*pi*col/100000) else sin(2*pi*col/100000) fi
To Polygon
Draw... 0 0 0 0
