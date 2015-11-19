# textioSpeed.praat
# Paul Boersma, 2015-11-18

echo Text I/O speed:

for i to 33
	sound'i' = Create Sound from formula... sound'i' Mono 0 10 44100 i
endfor
select sound1
for i from 2 to 33
	plus sound'i'
endfor
print writing:
stopwatch
Write to text file... kanweg.Collection
t = stopwatch
printline  't:3' seconds
Remove
print reading:
stopwatch
Read from file... kanweg.Collection
t = stopwatch
printline  't:3' seconds
assert numberOfSelected () = 33
minus Sound sound30
Remove
select Sound sound30
mean = Get mean... All 0 0
assert mean = 30 ;   'mean'
Remove
deleteFile ("kanweg.Collection")
printline OK
