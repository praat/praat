# test_angle_between_planes
# djmw 20041021, 20070820 

printline ----- test_angle_between_planes -----
for i to 200
  hoek = randomInteger (0,90)
  nrow = randomInteger (10,1000)
  call test hoek nrow 3
endfor
printline ----- succeeded:  test_angle_between_planes -----

procedure test degrees nrow ncol
  Create TableOfReal... t nrow ncol
  idt1 = selected ("TableOfReal")
  Formula... if col=1 then randomGauss(0,10) else self endif
  Formula... if col=2 then randomGauss(0,5) else self endif
  Formula... if col=3 then randomGauss(0,2.5) else self endif
 # Centering is very important
  Centre columns
  To Configuration
  c = selected ("Configuration")
  Rotate (pc)
  To TableOfReal
  idt2 = selected ("TableOfReal")
  select c
 # The rotation is in the pc-coordinates frame
  Rotate... 1 3 degrees
  To TableOfReal
  idt3 = selected ("TableOfReal")
  plus idt2
  To PCA
  pc1 = selected("PCA", 1)
  pc2 = selected("PCA", 2)
  degrees_found = Get angle between pc1-pc2 planes
  assert abs(degrees-degrees_found) < 2e-6; 'nrow' 'degrees' 'degrees_found'
  printline 'nrow' 'degrees' 'degrees_found'
  plus idt1
  plus idt2
  plus idt3
  plus pc1
  plus pc2
  plus c
  Remove
endproc


