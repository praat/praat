a# = zero# (3)
a#[1]=3
a#[2]=2
a#[3]=4
output# = softmax# (a#)
writeInfoLine: output#[1], " ", output#[2], " ", output#[3]

a## = {{ 3, 2, 4 }, { 5, 7, 6 }}
output## = softmaxPerRow## (a##)
appendInfoLine: output##
