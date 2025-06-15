# regexp_textgrid_test.praat
# djmw 20081119

Create Sound from formula... sineWithNoise Mono 0 1 44100  1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
To TextGrid... hhhhh 
select TextGrid sineWithNoise
for i to 5
  Insert boundary... 1 0.1*i
  Set interval text... 1 i+1 'i'
endfor