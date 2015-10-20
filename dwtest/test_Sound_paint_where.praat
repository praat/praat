# test_Sound_paint_where.praat
# djmw 20110524

Erase all

s1 = Create Sound from formula... s Mono 0 1 10000 0.5*sin (2*pi*5*x)
s2 = Create Sound from formula... 1odx Mono 0 20 100 1/x
s3 = Create Sound from formula... line Mono 0 1 10000 x/2

Select outer viewport... 0 6 0 3
select s1
Paint where... Red 0 0 -1 1 -1 y 1
Paint where... Green 0 0 -1 1  1 n 1
Text top... n Paint where: green above, red below the sin of 5 Hz

Select outer viewport... 0 6 3 6
Paint where... Red 0 0 -1 1 0 n self>0
Paint where... Green 0 0 -1 1 0 y self<0
Text top... n Paint where: green is positive area , red is negative area of sin of 5 Hz

select s2
Select outer viewport... 0 6 6 9
Draw... 0 20 0 1.5 y Curve
Paint where... Grey 0 20 0 1.5 0 y (x>=1 and x<2) or (x>=4 and x<8)
One mark bottom... 1 y y n
One mark bottom... 2 y y n
One mark bottom... 4 y y n
One mark bottom... 8 y y n
Text top... n Paint where: grey intervals (1,2)  and (4,8)
Remove

select s3
plus s1
Select outer viewport... 0 6 9 12
Paint enclosed... Grey 0 0 -1 1 yes
Text top... n Paint enclosed: between sine and straight line
removeObject: s1, s3

