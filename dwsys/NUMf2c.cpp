/* NUMf2c.c */

#include <ctype.h>
#include "NUMf2c.h"
//#include "melder.h"

double d_sign (double *a, double *b) {
	double x;
	x = (*a >= 0.0 ? *a : - *a);
	return (*b >= 0.0 ? x : -x);
}

integer lsame_ (const char *ca, const char *cb) {
	int a = * (unsigned char *) ca;
	int b = * (unsigned char *) cb;
	return tolower (a) == tolower (b);
}

double pow_di (double *ap, integer *bp) {
	double pow, x;
	integer n;
	uinteger u;

	pow = 1;
	x = *ap;
	n = *bp;

	if (n != 0) {
		if (n < 0) {
			n = -n;
			x = 1.0 / x;
		}
		for (u = n; ;) {
			if (u & 01) {
				pow *= x;
			}
			if (u >>= 1) {
				x *= x;
			} else {
				break;
			}
		}
	}
	return (pow);
}

void s_cat (char *lp, const char *rpp[], integer rnp[], integer *np, integer ll) {
	integer i, nc;
	char *rp;
	integer n = *np;
#ifndef NO_OVERWRITE
	integer L, m;
	char *lp0, *lp1;

	lp0 = 0;
	lp1 = lp;
	L = ll;
	i = 0;
	while (i < n) {
		rp = (char *) rpp[i];
		m = rnp[i++];
		if (rp >= lp1 || rp + m <= lp) {
			if ( (L -= m) <= 0) {
				n = i;
				break;
			}
			lp1 += m;
			continue;
		}
		lp0 = lp;
		lp = lp1 = Melder_malloc_f (char, L = ll);
		break;
	}
	lp1 = lp;
#endif /* NO_OVERWRITE */
	for (i = 0 ; i < n ; ++i) {
		nc = ll;
		if (rnp[i] < nc) {
			nc = rnp[i];
		}
		ll -= nc;
		rp = (char *) rpp[i];
		while (--nc >= 0) {
			*lp++ = *rp++;
		}
	}
	while (--ll >= 0) {
		*lp++ = ' ';
	}
#ifndef NO_OVERWRITE
	if (lp0) {
		memcpy (lp0, lp1, L);
		free (lp1);
	}
#endif
}


/* compare two strings */
integer s_cmp (const char *a0, const char *b0, integer la, integer lb) {
	register unsigned char *a, *aend, *b, *bend;
	a = (unsigned char *) a0;
	b = (unsigned char *) b0;
	aend = a + la;
	bend = b + lb;

	if (la <= lb) {
		while (a < aend)
			if (*a != *b) {
				return (*a - *b);
			} else {
				++a;
				++b;
			}

		while (b < bend)
			if (*b != ' ') {
				return (' ' - *b);
			} else	{
				++b;
			}
	}

	else {
		while (b < bend)
			if (*a == *b) {
				++a;
				++b;
			} else {
				return (*a - *b);
			}
		while (a < aend)
			if (*a != ' ') {
				return (*a - ' ');
			} else	{
				++a;
			}
	}
	return (0);
}

void s_copy (register char *a, register char *b, integer la, integer lb) {
	register char *aend, *bend;

	aend = a + la;

	if (la <= lb)
#ifndef NO_OVERWRITE
		if (a <= b || a >= b + la)
#endif
			while (a < aend) {
				*a++ = *b++;
			}
#ifndef NO_OVERWRITE
		else
			for (b += la; a < aend;) {
				*--aend = *--b;
			}
#endif

	else {
		bend = b + lb;
#ifndef NO_OVERWRITE
		if (a <= b || a >= bend)
#endif
			while (b < bend) {
				*a++ = *b++;
			}
#ifndef NO_OVERWRITE
		else {
			a += lb;
			while (b < bend) {
				*--a = *--bend;
			}
			a += lb;
		}
#endif
		while (a < aend) {
			*a++ = ' ';
		}
	}
}

/* End of file NUMf2c.c */
