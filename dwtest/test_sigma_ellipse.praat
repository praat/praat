# test_sigma_ellipse.praat
# djmw 20070412

Create TableOfReal... table 10000 2
tor = selected ("TableOfReal")
Formula... if col = 1 then randomGauss(0,1) else self fi
Formula... if col = 2 then randomGauss(0,2) else self fi
To SSCP... 0 0 0 0
sscp = selected ("SSCP")

Erase all

Select outer viewport... 0 6 0 12
select tor
Draw scatter plot... 1 2 0 0 -6 6 -12 12 12 no + yes

select sscp
Red
nsigma=3
call get_p 3
call get_p 4

procedure get_p .nsigma
  select sscp
  psigma = 1-exp(-.nsigma*.nsigma/2)
  printline nsigma='.nsigma', p='psigma'
  Red
  Draw sigma ellipse... .nsigma 1 2 -6 6 -12 12 no
  Green
  Draw confidence ellipse... psigma 1 2  -6 6 -12 12 no
  Black
endproc

removeObject: tor, sscp