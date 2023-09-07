/* spit.c */
/* Paul Boersma, 7 September 2023 */

/* This program spits out its arguments. */
/* It is meant to debug `runProcess()` in Praat. */

#include <stdio.h>

int main (int argc, char **argv) {
	for (int i = 0; i < argc; i ++)
		printf ("arg [%d] = <<%s>>\n", i, argv [i]);
}
