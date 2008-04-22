Praats/glpk/READ_ME.TXT
Paul Boersma, March 31, 2008
This file describes the adaptations to the glpk 4.28 sources
that were needed to make them compatible with Praat.

Because of a name clash with the C library in CodeWarrior
("error" is a member of the FILE structure),
I replaced the "error" macro with "mpl_error" in all glpmpl* files.
