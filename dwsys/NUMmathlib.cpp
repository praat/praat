/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1998       Ross Ihaka
 *  Copyright (C) 2000--2007 The R Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *  SYNOPSIS
 *
 *    #include <Rmath.h>
 *    double ptukey(q, rr, cc, df, lower_tail, log_p);
 *
 *  DESCRIPTION
 *
 *    Computes the probability that the maximum of rr studentized
 *    ranges, each based on cc means and with df degrees of freedom
 *    for the standard error, is less than q.
 *
 *    The algorithm is based on that of the reference.
 *
 *  REFERENCE
 *
 *    Copenhaver, Margaret Diponzio & Holland, Burt S.
 *    Multiple comparisons of simple effects in
 *    the two-way analysis of variance with fixed effects.
 *    Journal of Statistical Computation and Simulation,
 *    Vol.30, pp.1-15, 1988.
 */

#include "NUM2.h"

#define R_Q_P01_boundaries(p, _LEFT_, _RIGHT_)		\
    if (log_p) {					\
		if (p > 0.0)					\
	    	return undefined;				\
		if (p == 0.0) /* upper bound*/			\
	    	return lower_tail ? _RIGHT_ : _LEFT_;	\
		if (isundef (p))	/* cannot occur*/		\
	    	return lower_tail ? _LEFT_ : _RIGHT_;	\
    } else { /* !log_p */					\
		if (p < 0.0 || p > 1.0)				\
			return undefined;				\
		if (p == 0.0)					\
			return lower_tail ? _LEFT_ : _RIGHT_;	\
		if (p == 1.0)					\
	    	return lower_tail ? _RIGHT_ : _LEFT_;	\
    }

#define R_D_Lval(p)	(lower_tail ? (p) : (0.5 - (p) + 0.5))
#define R_DT_qIv(p)	(log_p ? (lower_tail ? exp(p) : - expm1(p)) : R_D_Lval(p))

#define R_D__0	(log_p ? undefined : 0.0)		/* 0 */
#define R_D__1	(log_p ? 0.0 : 1.0)			/* 1 */
#define R_DT_0	(lower_tail ? R_D__0 : R_D__1)		/* 0 */
#define R_DT_1	(lower_tail ? R_D__1 : R_D__0)		/* 1 */
#define R_D_val(x)	(log_p	? log(x) : (x))
#define R_D_Clog(p)	(log_p	? log1p(-(p)) : (0.5 - (p) + 0.5)) /* [log](1-p) */
#define R_DT_val(x)	(lower_tail ? R_D_val(x)  : R_D_Clog(x))

static double wprob(double w, double rr, double cc)
{
/*  wprob() :

	This function calculates probability integral of Hartley's
	form of the range.

	w     = value of range
	rr    = no. of rows or groups
	cc    = no. of columns or treatments
	ir    = error flag = 1 if pr_w probability > 1
	pr_w = returned probability integral from (0, w)

	program will not terminate if ir is raised.

	bb = upper limit of legendre integration
	iMax = maximum acceptable value of integral
	nleg = order of legendre quadrature
	ihalf = int ((nleg + 1) / 2)
	wlar = value of range above which wincr1 intervals are used to
	       calculate second part of integral,
	       else wincr2 intervals are used.
	C1, C2, C3 = values which are used as cutoffs for terminating
	or modifying a calculation.

	M_1_SQRT_2PI = 1 / sqrt(2 * pi);  from abramowitz & stegun, p. 3.
	M_SQRT2 = sqrt(2)
	xleg = legendre 12-point nodes
	aleg = legendre 12-point coefficients
 */
#define nleg	12
#define ihalf	6

    /* looks like this is suboptimal for double precision.
       (see how C1-C3 are used) <MM>
    */
    /* const double iMax  = 1.; not used if = 1*/
    constexpr static double C1 = -30.;
    constexpr static double C2 = -50.;
    constexpr static double C3 = 60.;
    constexpr static double bb   = 8.;
    constexpr static double wlar = 3.;
    constexpr static double wincr1 = 2.;
    constexpr static double wincr2 = 3.;
    constexpr static double xleg [ihalf] = {
		0.981560634246719250690549090149,
		0.904117256370474856678465866119,
		0.769902674194304687036893833213,
		0.587317954286617447296702418941,
		0.367831498998180193752691536644,
		0.125233408511468915472441369464
    };
    constexpr static double aleg[ihalf] = {
		0.047175336386511827194615961485,
		0.106939325995318430960254718194,
		0.160078328543346226334652529543,
		0.203167426723065921749064455810,
		0.233492536538354808760849898925,
		0.249147045813402785000562436043
    };
    double pr_w, rinsum, xx;
    long double blb, bub, einsum, elsum;

    const double qsqz = w * 0.5;

    /* if w >= 16 then the integral lower bound (occurs for c=20) */
    /* is 0.99999999999995 so return a value of 1. */

    if (qsqz >= bb)
		return 1.0;

    /* find (f(w/2) - 1) ^ cc */
    /* (first term in integral of hartley's form). */
	// djwm: pnorm(q,m,s,1,0) = NUMgaussP((q-m)/s)
    // pr_w = 2 * pnorm(qsqz, 0.,1., 1,0) - 1.; /* erf(qsqz / M_SQRT2) */
    pr_w = 2 * NUMgaussP (qsqz) - 1.0;
    /* if pr_w ^ cc < 2e-22 then set pr_w = 0 */
    if (pr_w >= exp(C2 / cc))
		pr_w = pow(pr_w, cc);
    else
		pr_w = 0.0;

    /* if w is large then the second component of the */
    /* integral is small, so fewer intervals are needed. */

	const double wincr = ( w > wlar ? wincr1 : wincr2 );

    /* find the integral of second term of hartley's form */
    /* for the integral of the range for equal-length */
    /* intervals using legendre quadrature.  limits of */
    /* integration are from (w/2, 8).  two or three */
    /* equal-length intervals are used. */

    /* blb and bub are lower and upper limits of integration. */

    blb = qsqz;
    const double binc = (bb - qsqz) / wincr;
    bub = blb + binc;
    einsum = 0.0;

    /* integrate over each interval */

    const double cc1 = cc - 1.0;
    for (double wi = 1.0; wi <= wincr; wi ++) {
		elsum = 0.0;
		const double a = double (0.5 * (bub + blb));

		/* legendre quadrature with order = nleg */

		const double b = double (0.5 * (bub - blb));

		for (integer jj = 1; jj <= nleg; jj ++) {
			integer j;
			if (ihalf < jj) {
				j = (nleg - jj) + 1;
				xx = xleg [j-1];
			} else {
				j = jj;
				xx = -xleg [j-1];
			}
			const double c = b * xx;
			const double ac = a + c;

			/* if exp(-qexpo/2) < 9e-14, */
			/* then doesn't contribute to integral */

			const double qexpo = ac * ac;
			if (qexpo > C3)
				break;

			const double pplus = 2 * NUMgaussP (ac); // djmw: 2 * pnorm(ac, 0., 1., 1,0);
			const double pminus= 2 * NUMgaussP (ac - w); // djmw: 2 * pnorm(ac, w,  1., 1,0);

			/* if rinsum ^ (cc-1) < 9e-14, */
			/* then doesn't contribute to integral */

			rinsum = pplus * 0.5 - pminus * 0.5;
			if (rinsum >= exp (C1 / cc1)) {
				rinsum = aleg[j-1] * exp(-(0.5 * qexpo)) * pow(rinsum, cc1);
				elsum += rinsum;
			}
		}
		elsum *= 2.0 * b * cc * NUM1_sqrt2pi;
		einsum += elsum;
		blb = bub;
		bub += binc;
    }

    /* if pr_w ^ rr < 9e-14, then return 0 */
    pr_w += (double) einsum;
    if (pr_w <= exp(C1 / rr))
		return 0.0;

    pr_w = pow(pr_w, rr);
    if (pr_w >= 1.0)   // 1 was iMax was eps
		return 1.0;
    return pr_w;
} /* wprob() */

static double ptukey(double q, double rr, double cc, double df, int lower_tail, int log_p)
{
/*  function ptukey() [was qprob() ]:

	q = value of studentized range
	rr = no. of rows or groups
	cc = no. of columns or treatments
	df = degrees of freedom of error term
	ir[0] = error flag = 1 if wprob probability > 1
	ir[1] = error flag = 1 if qprob probability > 1

	qprob = returned probability integral over [0, q]

	The program will not terminate if ir[0] or ir[1] are raised.

	All references in wprob to Abramowitz and Stegun
	are from the following reference:

	Abramowitz, Milton and Stegun, Irene A.
	Handbook of Mathematical Functions.
	New York:  Dover publications, Inc. (1970).

	All constants taken from this text are
	given to 25 significant digits.

	nlegq = order of legendre quadrature
	ihalfq = int ((nlegq + 1) / 2)
	eps = max. allowable value of integral
	eps1 & eps2 = values below which there is
		      no contribution to integral.

	d.f. <= dhaf:	integral is divided into ulen1 length intervals.  else
	d.f. <= dquar:	integral is divided into ulen2 length intervals.  else
	d.f. <= deigh:	integral is divided into ulen3 length intervals.  else
	d.f. <= dlarg:	integral is divided into ulen4 length intervals.

	d.f. > dlarg:	the range is used to calculate integral.

	M_LN2 = log(2)

	xlegq = legendre 16-point nodes
	alegq = legendre 16-point coefficients

	The coefficients and nodes for the legendre quadrature used in
	qprob and wprob were calculated using the algorithms found in:

	Stroud, A. H. and Secrest, D.
	Gaussian Quadrature Formulas.
	Englewood Cliffs,
	New Jersey:  Prentice-Hall, Inc, 1966.

	All values matched the tables (provided in same reference)
	to 30 significant digits.

	f(x) = .5 + erf(x / sqrt(2)) / 2      for x > 0

	f(x) = erfc( -x / sqrt(2)) / 2	      for x < 0

	where f(x) is standard normal c. d. f.

	if degrees of freedom large, approximate integral
	with range distribution.
 */
#define nlegq	16
#define ihalfq	8

/*  const double eps = 1.0; not used if = 1 */
    constexpr static double eps1 = -30.0;
    constexpr static double eps2 = 1.0e-14;
    constexpr static double dhaf  = 100.0;
    constexpr static double dquar = 800.0;
    constexpr static double deigh = 5000.0;
    constexpr static double dlarg = 25000.0;
    constexpr static double ulen1 = 1.0;
    constexpr static double ulen2 = 0.5;
    constexpr static double ulen3 = 0.25;
    constexpr static double ulen4 = 0.125;
    constexpr static double xlegq[ihalfq] = {
		0.989400934991649932596154173450,
		0.944575023073232576077988415535,
		0.865631202387831743880467897712,
		0.755404408355003033895101194847,
		0.617876244402643748446671764049,
		0.458016777657227386342419442984,
		0.281603550779258913230460501460,
		0.950125098376374401853193354250e-1
    };
    constexpr static double alegq[ihalfq] = {
		0.271524594117540948517805724560e-1,
		0.622535239386478928628438369944e-1,
		0.951585116824927848099251076022e-1,
		0.124628971255533872052476282192,
		0.149595988816576732081501730547,
		0.169156519395002538189312079030,
		0.182603415044923588866763667969,
		0.189450610455068496285396723208
    };

	if (isundef (q) || isundef (rr) || isundef (cc) || isundef (df))
		return undefined;

    if (q <= 0.0)
		return R_DT_0;

    /* df should be > 1 */
    /* there should be at least two values */

    if (df < 2 || rr < 1 || cc < 2) {
		return undefined;
	}

   // if (isundef (q) { return R_DT_1; }

    if (df > dlarg)
		return R_DT_val(wprob(q, rr, cc));

    /* calculate leading constant */

    const double f2 = df * 0.5;
    /* lgammafn(u) = log(gamma(u)) */
	double f2lf = ((f2 * log(df)) - (df * NUMln2)) - NUMlnGamma (f2); //lgammafn(f2);
	const double f21 = f2 - 1.0;

    /* integral is divided into unit, half-unit, quarter-unit, or */
    /* eighth-unit length intervals depending on the value of the */
    /* degrees of freedom. */

    const double ff4 = df * 0.25;
	double ulen;
    if	    (df <= dhaf)
		ulen = ulen1;
    else if (df <= dquar)
		ulen = ulen2;
    else if (df <= deigh)
		ulen = ulen3;
    else
		ulen = ulen4;

    f2lf += log (ulen);

    /* integrate over each subinterval */

    double otsum, ans = 0.0;

    for (integer i = 1; i <= 50; i++) {
		otsum = 0.0;

		/* legendre quadrature with order = nlegq */
		/* nodes (stored in xlegq) are symmetric around zero. */

		const double twa1 = (2 * i - 1) * ulen;

		for (integer jj = 1; jj <= nlegq; jj ++) {
			double t1;
			integer j;
			if (ihalfq < jj) {
				j = jj - ihalfq - 1;
				t1 = f2lf + f21 * log (twa1 + xlegq[j] * ulen) - (xlegq[j] * ulen + twa1) * ff4;
			} else {
				j = jj - 1;
				t1 = f2lf + f21 * log (twa1 - xlegq[j] * ulen) + (xlegq[j] * ulen - twa1) * ff4;
			}

			/* if exp(t1) < 9e-14, then doesn't contribute to integral */
			if (t1 >= eps1) {
				const double qsqz = ( ihalfq < jj ?
					q * sqrt ((xlegq[j] * ulen + twa1) * 0.5) :
					q * sqrt ((-(xlegq[j] * ulen) + twa1) * 0.5) );
				
				/* call wprob to find integral of range portion */

				const double wprb = wprob (qsqz, rr, cc);
				const double rotsum = wprb * alegq [j] * exp (t1);
				otsum += rotsum;
			}
			/* end legendre integral for interval i */
			/* L200: */
		}

		/* if integral for interval i < 1e-14, then stop.
		 * However, in order to avoid small area under left tail,
		 * at least  1 / ulen  intervals are calculated.
		 */
		if (i * ulen >= 1.0 && otsum <= eps2)
			break;

		/* end of interval i */
		/* L330: */

		ans += otsum;
    }

    if (otsum > eps2) { /* not converged */
		Melder_throw (U"Not converged");
    }
    if (ans > 1.)
	ans = 1.;
    return R_DT_val(ans);
}
/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1998 	     Ross Ihaka
 *  Copyright (C) 2000--2005 The R Core Team
 *  based in part on AS70 (C) 1974 Royal Statistical Society
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *  SYNOPSIS
 *
 *	double qtukey(p, rr, cc, df, lower_tail, log_p);
 *
 *  DESCRIPTION
 *
 *	Computes the quantiles of the maximum of rr studentized
 *	ranges, each based on cc means and with df degrees of freedom
 *	for the standard error, is less than q.
 *
 *	The algorithm is based on that of the reference.
 *
 *  REFERENCE
 *
 *	Copenhaver, Margaret Diponzio & Holland, Burt S.
 *	Multiple comparisons of simple effects in
 *	the two-way analysis of variance with fixed effects.
 *	Journal of Statistical Computation and Simulation,
 *	Vol.30, pp.1-15, 1988.
 */

/* qinv() :
 *	this function finds percentage point of the studentized range
 *	which is used as initial estimate for the secant method.
 *	function is adapted from portion of algorithm as 70
 *	from applied statistics (1974) ,vol. 23, no. 1
 *	by odeh, r. e. and evans, j. o.
 *
 *	  p = percentage point
 *	  c = no. of columns or treatments
 *	  v = degrees of freedom
 *	  qinv = returned initial estimate
 *
 *	vmax is cutoff above which degrees of freedom
 *	is treated as infinity.
 */

static double qinv(double p, double c, double v)
{
    constexpr static double p0 = 0.322232421088;
    constexpr static double q0 = 0.993484626060e-01;
    constexpr static double p1 = -1.0;
    constexpr static double q1 = 0.588581570495;
    constexpr static double p2 = -0.342242088547;
    constexpr static double q2 = 0.531103462366;
    constexpr static double p3 = -0.204231210125;
    constexpr static double q3 = 0.103537752850;
    constexpr static double p4 = -0.453642210148e-04;
    constexpr static double q4 = 0.38560700634e-02;
    constexpr static double c1 = 0.8832;
    constexpr static double c2 = 0.2368;
    constexpr static double c3 = 1.214;
    constexpr static double c4 = 1.208;
    constexpr static double c5 = 1.4142;
    constexpr static double vmax = 120.0;

    const double ps = 0.5 - 0.5 * p;
    const double yi = sqrt (log (1.0 / (ps * ps)));
    double t = yi + (((( yi * p4 + p3) * yi + p2) * yi + p1) * yi + p0)
	   / (((( yi * q4 + q3) * yi + q2) * yi + q1) * yi + q0);
    if (v < vmax)
		t += (t * t * t + t) / v / 4.0;
    double q = c1 - c2 * t;
    if (v < vmax)
		q += -c3 / v + c4 * t / v;
    return t * (q * log (c - 1.0) + c5);
}

/*
 *  Copenhaver, Margaret Diponzio & Holland, Burt S.
 *  Multiple comparisons of simple effects in
 *  the two-way analysis of variance with fixed effects.
 *  Journal of Statistical Computation and Simulation,
 *  Vol.30, pp.1-15, 1988.
 *
 *  Uses the secant method to find critical values.
 *
 *  p = confidence level (1 - alpha)
 *  rr = no. of rows or groups
 *  cc = no. of columns or treatments
 *  df = degrees of freedom of error term
 *
 *  ir(1) = error flag = 1 if wprob probability > 1
 *  ir(2) = error flag = 1 if ptukey probability > 1
 *  ir(3) = error flag = 1 if convergence not reached in 50 iterations
 *		       = 2 if df < 2
 *
 *  qtukey = returned critical value
 *
 *  If the difference between successive iterates is less than eps,
 *  the search is terminated
 */


static double qtukey(double p, double rr, double cc, double df, int lower_tail, int log_p) {
    constexpr static double eps = 0.0001;
    constexpr int maxiter = 50;

	if (isundef (p) || isundef (rr) || isundef (cc) || isundef (df))
		return undefined;

    /* df should be > 1 ; there should be at least two values */
    if (df < 2.0 || rr < 1.0 || cc < 2.0)
		return undefined;

    //R_Q_P01_boundaries (p, 0.0, ML_POSINF);
	R_Q_P01_boundaries (p, 0.0, undefined);
    p = R_DT_qIv (p); /* lower_tail,non-log "p" */

    /* Initial value */

    double x0 = qinv (p, cc, df);

    /* Find prob(value < x0) */

    double valx0 = ptukey (x0, rr, cc, df, /*LOWER*/true, /*LOG_P*/false) - p;

    /* Find the second iterate and prob(value < x1). */
    /* If the first iterate has probability value */
    /* exceeding p then second iterate is 1 less than */
    /* first iterate; otherwise it is 1 greater. */
	double x1;
    if (valx0 > 0.0)
		x1 = ( x0 > 1.0 ? x0 - 1.0 : 0.0 ); // djmw: fmax2 (0.0, x0 - 1.0);
    else
		x1 = x0 + 1.0;
    double valx1 = ptukey (x1, rr, cc, df, /*LOWER*/true, /*LOG_P*/false) - p;

    /* Find new iterate */

    double ans = 0.0;
    for (integer iter = 1 ; iter < maxiter; iter ++) {
		ans = x1 - ((valx1 * (x1 - x0)) / (valx1 - valx0));
		valx0 = valx1;

		/* New iterate should be >= 0 */

		x0 = x1;
		if (ans < 0.0) {
			ans = 0.0;
			valx1 = -p;
		}
		/* Find prob(value < new iterate) */

		valx1 = ptukey (ans, rr, cc, df, /*LOWER*/true, /*LOG_P*/false) - p;
		x1 = ans;

		/* If the difference between two successive */
		/* iterates is less than eps, stop */

		const double xabs = fabs (x1 - x0);
		if (xabs < eps)
			return ans;
    }

    /* The process did not converge in 'maxiter' iterations */
	Melder_warning (U"Maximum number of iterations exceeded.");
    return ans;
}

double NUMinvTukeyQ (double p, double cc, double df, double rr) {
	return qtukey (p, rr, cc, df, 0, 0);
}

double NUMtukeyQ (double q, double cc, double df, double rr) {
	return ptukey (q, rr, cc, df, 0, 0);
}
