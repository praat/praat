s=Create Sound from formula... sineWithNoise Mono 0 1 44100  1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
lpc=To LPC (covariance)...     16 0.025 0.005 50
select s
plus lpc
ttt=To LPC (robust)... 0.025 50 1.5 5 1e-06 no
#rrrr=selected ("LPC")
select s
#plus rrrr
plus ttt