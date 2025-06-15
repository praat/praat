/* Polynomial.c
 *
 * Copyright (C) 1993-2008 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020813 GPL header
 djmw 20030619 Added SVD_compute before SVD_solve
 djmw 20060510 Polynomial_to_Roots: changed behaviour. All roots found are now saved.
 	In previous version a NULL pointer was returned. New error messages.
 djmw 20061021 printf expects %ld for 'long int'
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071201 Melder_warning<n>
 djmw 20080122 float -> double
*/

#include "Polynomial.h"
#include "SVD.h"
#include "NUMclapack.h"
#include "TableOfReal_extensions.h"
#include "NUMmachar.h"

#include "oo_DESTROY.h"
#include "Polynomial_def.h"
#include "oo_COPY.h"
#include "Polynomial_def.h"
#include "oo_EQUAL.h"
#include "Polynomial_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Polynomial_def.h"
#include "oo_WRITE_TEXT.h"
#include "Polynomial_def.h"
#include "oo_WRITE_BINARY.h"
#include "Polynomial_def.h"
#include "oo_READ_TEXT.h"
#include "Polynomial_def.h"
#include "oo_READ_BINARY.h"
#include "Polynomial_def.h"
#include "oo_DESCRIPTION.h"
#include "Polynomial_def.h"

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

extern machar_Table NUMfpp;

/* Evaluate polynomial and derivative jointly
	c[1..n] -> degree n-1 !!
*/
static void Polynomial_evaluate2 (I, double x, double *f, double *df)
{
	iam (Polynomial);
	long double p = my coefficients[my numberOfCoefficients], dp = 0, xc = x;
	long i;

	for (i = my numberOfCoefficients - 1; i > 0; i--)
	{
		dp = dp * xc + p;
		 p =  p * xc + my coefficients[i];
	}
	*f = p; *df = dp;
}

/* Get value and derivative */
static void Polynomial_evaluate2_z (I, dcomplex *z, dcomplex *p, dcomplex *dp)
{
	iam (Polynomial);
	long double pr = my coefficients[my numberOfCoefficients], pi = 0;
	long double dpr = 0, dpi = 0, x = z -> re, y = z -> im, tr;
	long i;

	for (i = my numberOfCoefficients - 1; i > 0; i--)
	{
		tr   = dpr;
		dpr  =  dpr * x -  dpi * y + pr;
		dpi  =   tr * y +  dpi * x + pi;
		tr   = pr;
		pr   =   pr * x -   pi * y + my coefficients[i];
		pi   =   tr * y +   pi * x;
	}
	 p -> re =   pr;  p -> im =  pi;
	dp -> re =  dpr; dp -> im = dpi;
}



/*
	void polynomial_divide (double *u, long m, double *v, long n, double *q, double *r);
	
	Purpose:
		Find the quotient q(x) and the remainder r(x) polynomials that result from the division of
		the polynomial u(x) = u[1] + u[2]*x^1 + u[3]*x^2 + ... + u[m]*x^(m-1) by the
		polynomial v(x) = v[1] + v[2]*x^1 + v[3]*x^2 + ... + v[n]*x^(n-1), such that
			u(x) = v(x)*q(x) + r(x).
			
	The arrays u, v, q and r have to be dimensioned as u[1...m], v[1..n], q[1...m] and r[1...m],
	respectively.
	On return, the q[1..m-n] and r[1..n-1] contain the quotient and the remainder
	polynomial coefficients, repectively.
	See Knuth, The Art of Computer Programming, Volume 2: Seminumerical algorithms,
	Third edition, Section 4.6.1 Algorithm D (the algorithm as described has been modified
	to prevent overwriting of the u-polynomial).
*/
static void polynomial_divide (double *u, long m, double *v, long n, double *q, double *r)
{
	long j, k;

	/*
		Copy u[1..m] into r[1..n] to prevent overwriting of u.
		Put the q coefficients to zero for cases n > m.
	*/
	for (k = 1; k <= m; k++)
	{
		r[k] = u[k];
		q[k] = 0;
	}

	for (k = m - n + 1; k > 0; k--) /* D1 */
	{
		q[k] = r[n+k-1] / v[n];  /* D2 with u -> r*/
		for (j = n + k - 1; j >= k; j--)
		{
			r[j] -= q[k] * v[j-k+1];
		}
    }
}

static int Polynomial_polish_realroot (I, double *x, long maxit)
{
	iam (Polynomial);
	double dx, xbest = *x, pmin = 1e38, dp, p, fabsp;
	long i;

	for (i = 1; i <= maxit; i++)
	{
		Polynomial_evaluate2 (me, *x, &p, &dp);
		fabsp = fabs (p);
        if (fabsp > pmin)
        {
			/*
				We stop because the approximation gets worse.
				Return previous (best) value for x.
			*/
			*x = xbest; return 1;
        }
        pmin = fabsp; xbest = *x;
		if (fabs (dp) == 0) return 1;
        dx = p / dp; /* Newton -Raphson */
        *x -= dx;
   }
   return 0; /* Maximum number of iterations exceeded. */
}

static int Polynomial_polish_complexroot_nr (I, dcomplex *z, long maxit)
{
	iam (Polynomial);
	dcomplex dz, zbest = *z, p, dp;
	double pmin = 1e38, fabsp;
	long i;

	for (i = 1; i <= maxit; i++)
	{
		Polynomial_evaluate2_z (me, z, &p, &dp);
		fabsp = dcomplex_abs (p);
        if (fabsp > pmin)
        {
			/*
				We stop because the approximation gets worse.
				Return previous (best) value for z.
			*/
			*z = zbest; return 1;
        }
        pmin = fabsp; zbest = *z;
		if (dcomplex_abs (dp) == 0) return 1;
        dz = dcomplex_div (p , dp); /* Newton -Raphson */
        *z = dcomplex_sub (*z, dz);
    }
	return 0; /* Maximum number of iterations exceeded. */
}

/*
	Symbolic evaluation of polynomial coefficients.
	Recurrence: P[n] = (a[n] * x + b[n]) P[n-1] + c[n] P[n-2],
		where P[n] is any orthogonal polynomial of degree n.
	P[n] is an array of coefficients p[k] representing: p[1] + p[2] x + ... p[n+1] x^n.
	Preconditions:
		degree > 1;
		pnm1 : polynomial of degree n - 1
		pnm2 : polynomial of degree n - 2
*/
static void NUMpolynomial_recurrence (double *pn, long degree, double a,
	double b, double c, double *pnm1, double *pnm2)
{
	long i;
	
	Melder_assert (degree > 1);
	
	pn[1] = b * pnm1[1] + c * pnm2[1];
	for (i=2; i <= degree-1; i++)
	{
		pn[i] = a * pnm1[i-1] + b * pnm1[i] + c * pnm2[i];
	}
	pn[degree] = a * pnm1[degree-1] + b * pnm1[degree];
	pn[degree+1] = a * pnm1[degree];
}


/* frozen[1..ma] */
static int svdcvm (double **v, long mfit, long ma, int *frozen, double *w,
	double **cvm)
{
	long i, j, k;
	double sum, t, *wti;

	if (! (wti = NUMdvector (1, mfit))) return 0;
	for (i=1; i <= mfit; i++)
	{
		if (w[i]) wti[i] = 1.0 / (w[i] * w[i]);
	}
	for (i=1; i <= mfit; i++)
	{
		for (j=1; j <= i; j++)
		{
			for (sum=0, k=1; k <= mfit; k++) sum+= v[i][k] * v[j][k] * wti[k];
			cvm[j][i] = cvm[i][j] = sum;
		}
	}
	
	for (i=mfit+1; i <= ma; i++)
	{
		for (j=1; j <= i; j++) cvm[j][i] = cvm[i][j] = 0;
	}
	
	k = mfit;
	for (j=ma; j > 0; j--)
	{
		if (! frozen || ! frozen[i])
		{
			for (i=1; i <= ma; i++)
			{
				t = cvm[i][k]; cvm[i][k] = cvm[i][j]; cvm[i][j] = t;
			}	
			for (i=1; i <= ma; i++)
			{
				t = cvm[k][i]; cvm[k][i] = cvm[j][i]; cvm[j][i] = t;
			}	
			k--;
		}
	}
	NUMdvector_free (wti, 1);
	return 1;
}

/********* FunctionTerms *********************************************/

static double classFunctionTerms_evaluate (I, double x)
{
	(void) void_me; (void) x;
	return NUMundefined;
}

static void classFunctionTerms_evaluate_z (I, dcomplex *z, dcomplex *p)
{
	(void) void_me; (void) z;
	p -> re = p -> im = NUMundefined;
}

static void classFunctionTerms_evaluateTerms (I, double x, double terms[])
{
	iam (FunctionTerms); 
	long i;
	(void) x;
	for (i = 1; i <= my numberOfCoefficients; i++)
	{
		terms[i] = NUMundefined;
	}
}

static long classFunctionTerms_getDegree (I)
{
	iam (FunctionTerms); 
	return my numberOfCoefficients - 1;
}

static void defaultGetExtrema (I, double x1, double x2, double *xmin, 
	double *ymin, double *xmax, double *ymax)
{
	iam (FunctionTerms);
	long i, numberOfPoints = 1000; double x = x1, dx, y;
	
	/*
		Melder_warning1 (L"defaultGetExtrema: extrema calculated by sampling the interval");
	*/
	
	dx = (x2 - x1) / (numberOfPoints - 1);
	*xmin = *xmax = x; *ymin = *ymax = our evaluate (me, x);
	for (i=2; i <= numberOfPoints; i++) 
	{
		x += dx; y = our evaluate (me, x); 
		if (y > *ymax)
		{
			*ymax = y; *xmax = x;
		}
		else if (y < *ymin)
		{
			*ymin = y; *xmin = x;
		}
	}
}

class_methods (FunctionTerms, Function)
	class_method_local (FunctionTerms, destroy)
	class_method_local (FunctionTerms, equal)
	class_method_local (FunctionTerms, canWriteAsEncoding)
	class_method_local (FunctionTerms, copy)
	class_method_local (FunctionTerms, readText)
	class_method_local (FunctionTerms, readBinary)
	class_method_local (FunctionTerms, writeText)
	class_method_local (FunctionTerms, writeBinary)
	class_method_local (FunctionTerms, description)
	class_method_local (FunctionTerms, evaluate)
	class_method_local (FunctionTerms, evaluate_z)
	class_method_local (FunctionTerms, evaluateTerms)
	class_method_local (FunctionTerms, getDegree)
	us -> getExtrema = defaultGetExtrema;
class_methods_end

int FunctionTerms_init (I, double xmin, double xmax, long numberOfCoefficients)
{
	iam (FunctionTerms);
	
	if ((my coefficients = NUMdvector (1, numberOfCoefficients)) == NULL)
		return 0;
	my numberOfCoefficients = numberOfCoefficients;
	my xmin = xmin; my xmax = xmax;
	return 1;
}

FunctionTerms FunctionTerms_create (double xmin, double xmax, 
	long numberOfCoefficients)
{
	FunctionTerms me = new (FunctionTerms);
	
	if (me == NULL) return NULL;
	if (! FunctionTerms_init (me, xmin, xmax, numberOfCoefficients))
		forget (me);
	return me;
}

int FunctionTerms_initFromString (I, double xmin, double xmax, wchar_t *s,
	int allowTrailingZeros)
{
	iam (FunctionTerms); long numberOfCoefficients; double *numbers = NULL;
	
	if ((numbers = NUMstring_to_numbers (s, &numberOfCoefficients)) == NULL)
		return 0;

	/*
		Skip trailing zeros
	*/

	if (! allowTrailingZeros)
	{
		while (numbers[numberOfCoefficients] == 0 && numberOfCoefficients > 1)
		{
			 numberOfCoefficients--;
		}
	}
	
	if (FunctionTerms_init (me, xmin, xmax, numberOfCoefficients))
	{
		NUMdvector_copyElements (numbers, my coefficients, 1,
			 numberOfCoefficients);
	}
	NUMdvector_free (numbers, 1);
	return ! Melder_hasError ();
}

long FunctionTerms_getDegree (I)
{
	iam (FunctionTerms); return our getDegree (me);
}

void FunctionTerms_setDomain (I, double xmin, double xmax)
{
	iam (FunctionTerms);
	my xmin = xmin; my xmax = xmax;
}

double FunctionTerms_evaluate (I, double x)
{
	iam (FunctionTerms);
	return our evaluate (me, x);
}

void FunctionTerms_evaluate_z (I, dcomplex *z, dcomplex *p)
{
	iam (FunctionTerms);
	our evaluate_z (me, z, p);
}

void FunctionTerms_evaluateTerms (I, double x, double terms[])
{
	iam (FunctionTerms);
	our evaluateTerms (me, x, terms);
}

void FunctionTerms_getExtrema (I, double x1, double x2, double *xmin, 
	double *ymin, double *xmax, double *ymax)
{
	iam (FunctionTerms);
	if (x2 <= x1)
	{
		x1 = my xmin; x2 = my xmax;
	}
	our getExtrema (me, x1, x2, xmin, ymin, xmax, ymax);
}

double FunctionTerms_getMinimum (I, double x1, double x2)
{
	iam (FunctionTerms);
	double xmin, xmax, ymin, ymax;
	FunctionTerms_getExtrema (me, x1, x2, &xmin, &ymin, &xmax, &ymax);
	return ymin;
}

double FunctionTerms_getXOfMinimum (I, double x1, double x2)
{
	iam (FunctionTerms);
	double xmin, xmax, ymin, ymax;
	FunctionTerms_getExtrema (me, x1, x2, &xmin, &ymin, &xmax, &ymax);
	return xmin;
}

double FunctionTerms_getMaximum (I, double x1, double x2)
{
	iam (FunctionTerms);
	double xmin, xmax, ymin, ymax;
	FunctionTerms_getExtrema (me, x1, x2, &xmin, &ymin, &xmax, &ymax);
	return ymax;
}

double FunctionTerms_getXOfMaximum (I, double x1, double x2)
{
	iam (FunctionTerms);
	double xmin, xmax, ymin, ymax;
	FunctionTerms_getExtrema (me, x1, x2, &xmin, &ymin, &xmax, &ymax);
	return xmax;
}

static void Graphics_polyline_clipTopBottom (Graphics g, double *x, double *y,
	long numberOfPoints, double ymin, double ymax)
{
	double xb, xe, yb, ye, x1, x2, y1, y2;
	long i, index = 0;
	
	if (numberOfPoints < 2) return;
	
	xb = x1 = x[0]; yb = y1 = y[0];
	for (i = 1; i < numberOfPoints; i++)
	{
		x2 = x[i]; y2 = y[i];
		
		if (! ((y1 > ymax && y2 > ymax) || (y1 < ymin && y2 < ymin)))
		{
			double dxy = (x2 - x1) / (y1 - y2);
			double xcros_max = x1 - (ymax - y1) * dxy;
			double xcros_min = x1 - (ymin - y1) * dxy;
			if (y1 > ymax && y2 < ymax)
			{
				/*
					Line enters from above: start new segment.
					Save start values.
				*/
				xb = x[i-1]; yb = y[i-1]; index = i - 1;
				y[i-1] = ymax; x[i-1] = xcros_max;
			}
			if (y1 > ymin && y2 < ymin)
			{
				/*
					Line leaves at bottom: draw segment.
					Save end values and restore them
					Origin of arrays for Graphics_polyline are at element 0 !!!
				*/
				xe = x[i]; ye = y[i];
				y[i] = ymin; x[i] = xcros_min;
				
				Graphics_polyline (g, i - index + 1, x + index, y + index);
				
				x[index] = xb; y[index] = yb; x[i] = xe; y[i] = ye;
			}
			if (y1 < ymin && y2 > ymin)
			{
				/*
					Line enters from below: start new segment.
					Save start values
				*/
				xb = x[i-1]; yb = y[i-1]; index = i - 1;
				y[i-1] = ymin; x[i-1] = xcros_min;
			}
			if (y1 < ymax && y2 > ymax)
			{
				/*
					Line leaves at top: draw segment.
					Save and restore
				*/
				xe = x[i]; ye = y[i];
				y[i] = ymax; x[i] =  xcros_max;

				Graphics_polyline (g, i - index + 1, x + index, y + index);
				
				x[index] = xb; y[index] = yb; x[i] = xe; y[i] = ye;
			}
		}
		else index = i;
		y1 = y2; x1 = x2;
	}
	if (index < numberOfPoints - 1)
	{
		Graphics_polyline (g, numberOfPoints - index, x + index, y + index);
		x[index] = xb; y[index] = yb;
	}
}

void FunctionTerms_draw (I, Graphics g, double xmin, double xmax, double ymin,
	double ymax, int extrapolate, int garnish)
{
	iam (FunctionTerms);
	double dx, fxmin, fxmax, x1, x2;
	double *x = NULL, *y = NULL;
	long i, numberOfPoints = 1000;
		
	if (((y = NUMdvector (1, numberOfPoints +1)) == NULL) ||
		((x = NUMdvector (1, numberOfPoints +1)) == NULL)) goto end;

	if (xmax <= xmin)
	{
		xmin = my xmin; xmax = my xmax;
	}
	
	fxmin = xmin; fxmax = xmax;
	if (! extrapolate)
	{
		if (xmax < my xmin || xmin > my xmax) return;
		if (xmin < my xmin) fxmin = my xmin;
		if (xmax > my xmax) fxmax = my xmax;
	}
	
	if (ymax <= ymin)
	{
		FunctionTerms_getExtrema (me, fxmin, fxmax, &x1, &ymin, &x2, &ymax);
	}
	
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	/*
		Draw only the parts within [fxmin, fxmax] X [ymin, ymax].
	*/
	
	dx = (fxmax - fxmin) / (numberOfPoints - 1);	
	for (i=1; i <= numberOfPoints; i++)
	{
		x[i] = fxmin + (i - 1.0) * dx;
		y[i] = FunctionTerms_evaluate (me, x[i]);
	}
	Graphics_polyline_clipTopBottom (g, x+1, y+1, numberOfPoints, ymin, ymax);	
	Graphics_unsetInner (g);
	
	if (garnish)
	{
		Graphics_drawInnerBox (g);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
	}
end:
	NUMdvector_free (y, 1); NUMdvector_free (x, 1);
}

void FunctionTerms_drawBasisFunction (I, Graphics g, long index, double xmin,
	double xmax, double ymin, double ymax, int extrapolate, int garnish)
{
	iam (FunctionTerms);
	FunctionTerms thee;
	long i;
	
	if (index < 1 || index > my numberOfCoefficients) return;
	if ((thee = Data_copy (me)) == NULL) return;
	
	for (i=1; i < index; i++) thy coefficients[i] = 0;
	for (i=index+1; i <= my numberOfCoefficients; i++) thy coefficients[i] = 0;
	thy coefficients[index] = 1;
	
	FunctionTerms_draw (thee, g, xmin, xmax, ymin, ymax, extrapolate, garnish);
	
	forget (thee);
}

int FunctionTerms_setCoefficient (I, long index, double value)
{
	iam (FunctionTerms);
	if (index < 1 || index > my numberOfCoefficients)
	{
		return Melder_error3 (L"FunctionTerms_setCoefficient: index out of "
			"range [1, ", Melder_integer (my numberOfCoefficients), L"].");
	}
	if (index == my numberOfCoefficients && value == 0)
	{
		return Melder_error1 (L"FunctionTerms_setCoefficient: you cannot remove "
			"the highest degree term.");
	}
	my coefficients[index] = value;
	return 1;
}

/********** Polynomial ***********************************************/

static double classPolynomial_evaluate (I, double x)
{
	iam (Polynomial); long i; 
	long double p = my coefficients[my numberOfCoefficients], xi = x;
	
	for (i = my numberOfCoefficients - 1; i > 0; i--)
	{
		p = p * xi + my coefficients[i];
	}
	return p;
}

static void classPolynomial_evaluate_z (I, dcomplex *z, dcomplex *p)
{
	iam (Polynomial); long i;
	long double pr, pi, p_r, x = z -> re, y = z -> im;

	pr = my coefficients[my numberOfCoefficients];
	pi = 0;
	for (i = my numberOfCoefficients - 1; i > 0; i--)
	{
		p_r = pr;
		pr =  pr * x - pi * y + my coefficients[i];
		pi = p_r * y + pi * x;
	}
	p -> re = pr; p -> im = pi;
}

static void classPolynomial_evaluateTerms (I, double x, double terms[])
{
	iam (Polynomial); long i;
	terms[1] = 1;
	for (i=2; i <= my numberOfCoefficients; i++)
	{
		terms[i] = terms[i-1] * x;
	}
}

static void classPolynomial_getExtrema (I, double x1, double x2, double *xmin,
	double *ymin, double *xmax, double *ymax)
{
	iam (Polynomial); Polynomial d = NULL; Roots r = NULL;
	long i, degree = my numberOfCoefficients - 1;
	
	*xmin = x1; *ymin = our evaluate (me, x1);
	*xmax = x2; *ymax = our evaluate (me, x2);
	if (*ymin > *ymax)
	{
		/* Swap */
		double t = *ymin; *ymin = *ymax; *ymax = t;
		t = *xmin; *xmin = *xmax; *xmax = t;
	}
	
	if (degree < 2) return;
	
	if (! (d = Polynomial_getDerivative (me)) ||
		! (r = Polynomial_to_Roots (d)))
	{
		defaultGetExtrema (me, x1, x2, xmin, ymin, xmax, ymax);
		goto end;
	}
	
	for (i=1; i <= degree - 1; i++)
	{
		double x = (r -> v[i]).re;
		if (x > x1 && x < x2)
		{
			double y = our evaluate (me, x);
			if (y > *ymax)
			{
				*ymax = y; *xmax = x;
			}
			else if (y < *ymin)
			{
				*ymin = y; *xmin = x;
			}
		}
	}
end:
	forget (d); forget (r);
}

class_methods (Polynomial, FunctionTerms)
	class_method_local (Polynomial, evaluate)
	class_method_local (Polynomial, evaluate_z)
	class_method_local (Polynomial, evaluateTerms)
	class_method_local (Polynomial, getExtrema)
class_methods_end

Polynomial Polynomial_create (double xmin, double xmax, long degree)
{
	Polynomial me = new (Polynomial);
	
	if (me == NULL) return NULL;
	if (! FunctionTerms_init (me, xmin, xmax, degree + 1)) forget (me);
	return me;
}

Polynomial Polynomial_createFromString (double xmin, double xmax, wchar_t *s)
{
	Polynomial me = new (Polynomial);
	
	if (me == NULL) return NULL;
	if (! FunctionTerms_initFromString (me, xmin, xmax, s, 0)) forget (me);
	return me;
}

void Polynomial_scaleCoefficients_monic (Polynomial me)
{
	long i; double cn = my coefficients[my numberOfCoefficients];
	
	if (cn == 1 || my numberOfCoefficients <= 1) return;
	
	for (i=1; i < my numberOfCoefficients; i++)
	{
			my coefficients[i] /= cn;
	}
	my coefficients[my numberOfCoefficients] = 1;
}

/*
	Transform the polynomial as if the domain were [xmin, xmax].
	Some polynomials (Legendre) are defined on the domain [-1,1]. The domain
	for x may be extended to [xmin, xmax] by a transformation such as
		x' = (2 * x - (xmin + xmax)) / (xmax - xmin)   -1 < x' < x.
	This procedure transforms x' back to x.
*/
Polynomial Polynomial_scaleX (Polynomial me, double xmin, double xmax)
{
	Polynomial thee = NULL; long j, n;
	double a, b, c = 0, *buf = NULL, *pn, *pnm1, *pnm2;
	
	Melder_assert (xmin < xmax);
		
	if (! (thee = Polynomial_create (xmin, xmax, my numberOfCoefficients - 1)))
		return NULL;
		
	thy coefficients[1] = my coefficients[1];
	if (my numberOfCoefficients == 1) return thee;

	/*
		x = a x + b
		Constraints:
		my xmin = a xmin + b;    a = (my xmin - my xmax) / (xmin - xmax);
		my xmax = a xmax + b;    b = my xmin - a * xmin
	*/
		
	a = (my xmin - my xmax) / (xmin - xmax); 
	b = my xmin - a * xmin;
	thy coefficients[2] = my coefficients[2] * a;
	thy coefficients[1] += my coefficients[2] * b;
	if (my numberOfCoefficients == 2) return thee;

	if (! (buf = NUMdvector (1, 3 * my numberOfCoefficients))) goto end;
		
	pn = buf;
	pnm1 = pn   + my numberOfCoefficients;
	pnm2 = pnm1 + my numberOfCoefficients;
	
	/*
		Start the recursion: P[1] = a x + b; P[0] = 1; 
	*/
	
	pnm1[2] = a; pnm1[1] = b; pnm2[1] = 1;
	for (n=2; n <= my numberOfCoefficients - 1; n++)
	{
		double *t1 = pnm1, *t2 = pnm2;
		NUMpolynomial_recurrence (pn, n, a, b, c, pnm1, pnm2);	
		if (my coefficients[n+1] != 0)
		{
			for (j=1; j <= n+1; j++)
			{
				thy coefficients[j] += my coefficients[n+1] * pn[j];
			}
		}
		pnm1 = pn;
		pnm2 = t1;
		pn = t2;
	}
end:
	NUMdvector_free (buf, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

double Polynomial_evaluate (I, double x)
{
	iam (Polynomial);
	return our evaluate (me, x);
}

void Polynomial_evaluate_z (Polynomial me, dcomplex *z, dcomplex *p)
{
	our evaluate_z (me, z, p);
}

static void Polynomial_evaluate_z_cart (Polynomial me, double r, double phi,
	double *re, double *im)
{
	double arg, rn = 1; long i;

	*re = my coefficients[1]; *im = 0;
	if (r == 0) return;
	for (i=2; i <= my numberOfCoefficients; i++)
	{
		rn *= r; arg = (i - 1) * phi;
		*re += my coefficients[i] * rn * cos (arg);
		*im += my coefficients[i] * rn * sin (arg);
	}
}


Polynomial Polynomial_getDerivative (Polynomial me)
{
	Polynomial thee; long i;
	
	if (my numberOfCoefficients == 1)
	{
		return Polynomial_create (my xmin, my xmax, 0);
	}
	
	if ((thee = Polynomial_create (my xmin, my xmax, my numberOfCoefficients
		- 2)) == NULL) return NULL;
		
	for (i=1; i <= thy numberOfCoefficients; i++) 
	{
		thy coefficients[i] = i * my coefficients[i+1];
	}
	return thee;
}

Polynomial Polynomial_getPrimitive (Polynomial me)
{
	Polynomial thee; long i;
	
	if ((thee = Polynomial_create (my xmin, my xmax, my numberOfCoefficients)))
		return NULL;
	
	for (i=1; i <= my numberOfCoefficients; i++) 
	{
		thy coefficients[i+1] = my coefficients[i] / i;
	}
	return thee;
}

double Polynomial_getArea (Polynomial me, double xmin, double xmax)
{
	Polynomial p; double area;
	
	if (xmax >= xmin)
	{
		xmin = my xmin; xmax = my xmax;
	}
	if ((p = Polynomial_getPrimitive (me)) == NULL) return NUMundefined;
	area = FunctionTerms_evaluate (p, xmax) - FunctionTerms_evaluate (p, xmin);
	forget (p);
	return area;
}

Polynomial Polynomials_multiply (Polynomial me, Polynomial thee)
{
	Polynomial him;
	long i, j, k, n1 = my numberOfCoefficients, n2 = thy numberOfCoefficients;
	double xmin, xmax;
	
	if (my xmax <= thy xmin || my xmin >= thy xmax) return Melder_errorp
		("Polynomial2_multiply: domains do not overlap.");
	xmin = my xmin > thy xmin ? my xmin : thy xmin;
	xmax = my xmax < thy xmax ? my xmax : thy xmax;
	if (! (him = Polynomial_create (xmin, xmax, n1 + n2 - 2))) return NULL;
	for (i=1; i <= n1; i++)
	{
		for (j=1; j <= n2; j++)
		{
			k = i + j - 1;
			his coefficients [k] += my coefficients[i] * thy coefficients[j];
		}
	}
	return him;
}

int Polynomials_divide (Polynomial me, Polynomial thee, Polynomial *q, Polynomial *r)
{
	double *qc, *rc;
	long degree, m = my numberOfCoefficients, n = thy numberOfCoefficients;

	if (*q == NULL && *r == NULL) return 1;
	qc = NUMdvector (1, m);
	rc = NUMdvector (1, m);
	if (qc == NULL || rc == NULL) goto end;
    polynomial_divide (my coefficients, m, thy coefficients, n, qc, rc);
	if (*q != NULL)
	{
		degree = MAX (m - n, 0);
		*q = Polynomial_create (my xmin, my xmax, degree);
		if (*q == NULL) goto end;
		if (degree >= 0) NUMdvector_copyElements (qc, (*q) -> coefficients, 1, degree+1);
	}
	if (*r != NULL)
	{
		degree = n - 2;
		if (m >= n) degree --;
		if (degree < 0) degree = 0;
		while (degree > 1 && rc[degree] == 0) degree--;
		*r = Polynomial_create (my xmin, my xmax, degree);
		if (*r == NULL) goto end;
		NUMdvector_copyElements (rc, (*r) -> coefficients, 1, degree+1);
	}	
end:
	NUMdvector_free (qc, 1);
	NUMdvector_free (rc, 1);
	if (Melder_hasError ())
	{
		forget (*q); forget (*r);
		return 0;
	}
	return 1;
}


/******** LegendreSeries ********************************************/

static double classLegendreSeries_evaluate (I, double x)
{
	iam (LegendreSeries); long i;
	double pi, pim1, pim2 = 1, p = my coefficients[1];
	
	/* 
		Transform x from domain [xmin, xmax] to domain [-1, 1]
	*/
	
	if (x < my xmin || x > my xmax) return NUMundefined;
	
	pim1 = x = (2 * x - my xmin - my xmax) / (my xmax - my xmin);
		
	if (my numberOfCoefficients > 1)
	{
		double twox = 2 * x, f1, f2 = x, d = 1.0;
		p += my coefficients[2] * pim1;
		for (i=3; i <= my numberOfCoefficients; i++)
		{
			f1 = d++; f2 += twox;
			pi = (f2 * pim1 - f1 * pim2) / d;
			p += my coefficients[i] * pi;
			pim2 = pim1; pim1 = pi;
		}
	}
	return p;
}

static void classLegendreSeries_evaluateTerms (I, double x, double terms[])
{
	iam (LegendreSeries); long i;
	
	if (x < my xmin || x > my xmax)
	{
		for (i=1; i <= my numberOfCoefficients; i++) terms[i] = NUMundefined;
		return;
	}
	
	/* 
		Transform x from domain [xmin, xmax] to domain [-1, 1]
	*/
	
	x = (2 * x - my xmin - my xmax) / (my xmax - my xmin);
	
	terms[1] = 1; 
	if (my numberOfCoefficients > 1)
	{
		double twox = 2 * x, f1, f2 = x, d = 1.0;
		terms[2] = x;
		for (i=3; i <= my numberOfCoefficients; i++)
		{
			f1 = d++; f2 += twox;
			terms[i] = (f2 * terms[i-1] - f1 * terms[i-2]) / d;
		}
	}
}

static void classLegendreSeries_getExtrema (I, double x1, double x2, 
	double *xmin, double *ymin, double *xmax, double *ymax)
{
	iam (LegendreSeries);
	Polynomial p = LegendreSeries_to_Polynomial (me);
	
	if (p)
	{
		FunctionTerms_getExtrema (p, x1, x2, xmin, ymin, xmax, ymax);
		forget (p);
	}
	else defaultGetExtrema (me, x1, x2, xmin, ymin, xmax, ymax);
}

class_methods (LegendreSeries, FunctionTerms)
	class_method_local (LegendreSeries, evaluate)
	class_method_local (LegendreSeries, evaluateTerms)
	class_method_local (LegendreSeries, getExtrema)
class_methods_end

LegendreSeries LegendreSeries_create (double xmin, double xmax, long numberOfPolynomials)
{
	LegendreSeries me = new (LegendreSeries);
	
	if (me == NULL) return NULL;
	if (! FunctionTerms_init (me, xmin, xmax, numberOfPolynomials)) forget (me);
	return me;
}

LegendreSeries LegendreSeries_createFromString (double xmin, double xmax,
	wchar_t *s)
{
	LegendreSeries me = new (LegendreSeries);
	
	if (me == NULL) return NULL;
	if (! FunctionTerms_initFromString (me, xmin, xmax, s, 0)) forget (me);
	return me;
}

LegendreSeries LegendreSeries_getDerivative (LegendreSeries me)
{
	LegendreSeries thee; long k, n, n2;
	
	if ((thee = LegendreSeries_create (my xmin, my xmax,
		my numberOfCoefficients - 1)) == NULL) return NULL;
	
	for (n=1; n <= my numberOfCoefficients - 1; n++)
	{
		/*
			P[n]'(x) = Sum (k=0..nonNegative, (2n - 4k - 1) P[n-2k-1](x))
		*/
		
		n2 = n - 1;
		for (k=0; n2 >= 0; k++, n2 -= 2) 
		{
			thy coefficients [n2 + 1] += (2 * n - 4 * k - 1) 
				* my coefficients[n + 1];
		}
	}
	return thee;
}

Polynomial LegendreSeries_to_Polynomial (LegendreSeries me)
{
	Polynomial thee = NULL, s = NULL, t; long j, n;
	double *buf = NULL, b = 0, *pn, *pnm1, *pnm2, xmin = -1, xmax = 1;

	if ((thee = Polynomial_create (xmin, xmax, my numberOfCoefficients - 1))
		== NULL) return NULL;	

	thy coefficients[1] = my coefficients[1]; /* * p[1] */
	if (my numberOfCoefficients == 1) return thee;

	thy coefficients[2] = my coefficients[2]; /* * p[2] */
	if (my numberOfCoefficients > 2)
	{
		if (! (buf = NUMdvector (1, 3 * my numberOfCoefficients))) goto end;
		
		pn = buf;
		pnm1 = pn   + my numberOfCoefficients;
		pnm2 = pnm1 + my numberOfCoefficients;
	
		/*
			Start the recursion: P[1] = x; P[0] = 1; 
		*/
	
		pnm1[2] = 1; pnm2[1] = 1; 
		for (n=2; n <= my numberOfCoefficients - 1; n++)
		{
			double a = (2 * n - 1.0) / n, c = -(n - 1.0) / n;
			double *t1 = pnm1, *t2 = pnm2;
			NUMpolynomial_recurrence (pn, n, a, b, c, pnm1, pnm2);	
			if (my coefficients[n+1] != 0)
			{
				for (j=1; j <= n+1; j++)
				{
					thy coefficients[j] += my coefficients[n+1] * pn[j];
				}
			}
			pnm1 = pn; pnm2 = t1; pn = t2;
		}
	}
	if (my xmin != xmin || my xmax != xmax)
	{
		if (! (s = Polynomial_scaleX (thee, my xmin, my xmax))) goto end;
		t = thee; thee = s; s = t; forget (s);
	}
end:
	NUMdvector_free (buf, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

/********* Roots ****************************************************/

class_methods (Roots, ComplexVector)
class_methods_end

Roots Roots_create (long numberOfRoots)
{
	Roots me = new (Roots);
	
	if (me == NULL) return me;
	if (! ComplexVector_init (me, 1, numberOfRoots)) forget (me);
	return me;
}

void Roots_fixIntoUnitCircle (Roots me)
{
	dcomplex z10 = dcomplex_create (1, 0);
    long i;
    
	for (i = my min; i <= my max; i++)
	{
		if (dcomplex_abs (my v[i]) > 1.0)
		{
			my v[i] = dcomplex_div (z10, dcomplex_conjugate (my v[i]));
		}
	}
}

static void NUMdcvector_extrema_re (dcomplex v[], long lo, long hi,
	double *min, double *max)
{
	long i;
	*min = *max = v[lo].re;
	for (i=lo+1; i <= hi; i++)
	{
		if (v[i].re < *min) *min = v[i].re;
		else if (v[i].re > *max) *max = v[i].re;
	}
}

static void NUMdcvector_extrema_im (dcomplex v[], long lo, long hi,
	double *min, double *max)
{
	long i;
	*min = *max = v[lo].im;
	for (i=lo+1; i <= hi; i++)
	{
		if (v[i].im < *min) *min = v[i].im;
		else if (v[i].im > *max) *max = v[i].im;
	}
}

void Roots_draw (Roots me, Graphics g, double rmin, double rmax, double imin,
	double imax, wchar_t *symbol, int fontSize, int garnish)
{
	long i; int oldFontSize = Graphics_inqFontSize (g);
	double eps = 1e-6, denum;
	
	if (rmax <= rmin)
	{
		NUMdcvector_extrema_re (my v, 1, my max, &rmin, &rmax);
	}
	denum = fabs (rmax) > fabs (rmin) ? fabs(rmax) : fabs (rmin);
	if (denum == 0) denum = 1;
	if (fabs((rmax - rmin) / denum) < eps)
	{
		rmin -= 1; rmax += 1; 
	}
	if (imax <= imin)
	{
		NUMdcvector_extrema_im (my v, 1, my max, &imin, &imax);
	}
	denum = fabs (imax) > fabs (imin) ? fabs(imax) : fabs (imin);
	if (denum == 0) denum = 1;
	if (fabs((imax - imin) / denum) < eps)
	{
		imin -= 1; imax += 1;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, rmin, rmax, imin, imax);
	Graphics_setFontSize (g, fontSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (i=1; i <= my max; i++)
	{
		double re = my v[i].re, im = my v[i].im;
		if (re >= rmin && re <= rmax && im >= imin && im <= imax)
		{
			Graphics_text (g, re, im, symbol);
		}
	}
	Graphics_setFontSize (g, oldFontSize);
	Graphics_unsetInner (g);
	if (garnish)
	{
    	Graphics_drawInnerBox (g);
 		if (rmin * rmax < 0) Graphics_markLeft (g, 0, 1, 1, 1, L"0");
 		if (imin * imax < 0) Graphics_markBottom (g, 0, 1, 1, 1, L"0");
    	Graphics_marksLeft (g, 2, 1, 1, 0);
 		Graphics_textLeft (g, 1, L"Imaginary part");
    	Graphics_marksBottom (g, 2, 1, 1, 0);
 		Graphics_textBottom (g, 1, L"Real part");
	}
}

Roots Polynomial_to_Roots (Polynomial me)
{
	Roots thee = NULL;
	char job = 'E', compz = 'N';
	double *hes = NULL, *wr, *wi, *work = NULL, *z = NULL, wt[1];
    long i, np1 = my numberOfCoefficients, n = np1 - 1, n2 = n * n, nrootsfound = n, ioffset = 0;
	long ihi = n, ilo = 1, ldh = n, ldz = n, lwork = -1, info;

	if (n < 1) return Melder_errorp1 (L"Cannot find roots of a constant function.");

	/*
    	Allocate storage for Hessenberg matrix (n * n) plus real and imaginary
     	parts of eigenvalues wr[1..n] and wi[1..n].
	*/
	hes = NUMdvector (1, n2 + n + n);
	if (hes == NULL) return NULL;
	wr = &hes[n2];
	wi = &hes[n2 + n];

	/*
    	Fill the upper Hessenberg matrix (storage is Fortran)
     	C: [i][j] -> Fortran: (j-1)*n + i
	*/

	for (i = 1; i <= n; i++)
	{
		hes[(i-1)*n+1] = -(my coefficients[np1-i] / my coefficients[np1]);
		if (i < n) hes[(i-1)*n+1+i] = 1;
	}

	/*
		Find out the working storage needed
	*/

	(void) NUMlapack_dhseqr (&job, &compz, &n, &ilo, &ihi, &hes[1], &ldh,
		&wr[1], &wi[1], z, &ldz, wt, &lwork, &info);
	if (info != 0)
	{
		if (info < 0) (void) Melder_error3 (L"Programming error. Argument ", Melder_integer (info), L" in NUMlapack_dhseqr has illegal value.");
		else { (void) Melder_error1 (L"Cannot occur here."); }
		goto end;
	}
	lwork = wt[0];
	work = NUMdvector (1, lwork);
	if (work == NULL) goto end;

	/*
		Find eigenvalues.
	*/

	(void) NUMlapack_dhseqr (&job, &compz, &n, &ilo, &ihi, &hes[1], &ldh,
		&wr[1], &wi[1], z, &ldz, &work[1], &lwork, &info);
	nrootsfound = n;
	ioffset = 0;
	if (info > 0)
	{
		/* if INFO = i, NUMlapack_dhseqr failed to compute all of the eigenvalues. Elements i+1:n of
			WR and WI contain those eigenvalues which have been successfully computed */
		nrootsfound -= info;
		if (nrootsfound < 1) goto end;
		Melder_warning3 (L"Calculated only ", Melder_integer (nrootsfound), L" roots.");
		ioffset = info;
	}
	else if (info < 0)
	{
		(void) Melder_error3 (L"Programming error. Argument ", Melder_integer (info), L" in NUMlapack_dhseqr has illegal value.");
		goto end;
	}
	
	thee = Roots_create (nrootsfound);
	if (thee != NULL)
	{
		for (i = 1; i <= nrootsfound; i++)
		{
			(thy v[i]).re = wr[ioffset + i];
			(thy v[i]).im = wi[ioffset + i];
		}
	}
	
	Roots_and_Polynomial_polish (thee, me);
	
end:
	NUMdvector_free (work, 1);
	NUMdvector_free (hes, 1);
	return thee;
}

void Roots_sort (Roots me)
{
	(void) me;
}

/*
	Precondition: complex roots occur in pairs (a,bi), (a,-bi) with b>0
*/
void Roots_and_Polynomial_polish (Roots me, Polynomial thee)
{
	long i = my min, maxit = 80;
	while (i <= my max)
	{
		double im = my v[i].im, re = my v[i].re;
		if (im != 0)
		{
			(void) Polynomial_polish_complexroot_nr (thee, & my v[i], maxit);
			if (i < my max && im == -my v[i+1].im && re == my v[i+1].re)
			{
				my v[i+1].re = my v[i].re; my v[i+1].im = -my v[i].im;
				i++;
			}	
		}
		else
		{
			(void) Polynomial_polish_realroot (thee, &(my v[i].re), maxit);
		}
		i++;
	}
}

Polynomial Roots_to_Polynomial (Roots me)
{
	(void) me;
	return Melder_errorp1 (L"Not implemented yet");
}

int Roots_setRoot (Roots me, long index, double re, double im)
{
	if (index < my min || index > my max)
	{
		return Melder_error3 (L"Roots_setRoot: index must be in interval [1, ", 
			Melder_integer (my max), L"].");
	}
	my v[index].re = re;
	my v[index].im = im;
	return 1;
}

dcomplex Roots_evaluate_z (Roots me, dcomplex z)
{
	dcomplex t, result = {1, 0};
	long i; 
	
	for (i=my min; i <= my max; i++)
	{
		t = dcomplex_sub (z, my v[i]); 
		result = dcomplex_mul (result, t);
	}
	return result;
}
 
Spectrum Roots_to_Spectrum (Roots me, double nyquistFrequency,
	long numberOfFrequencies, double radius)
{
	Spectrum thee = NULL;
	dcomplex z, s;
	double phi;
	long i;
	
	if (numberOfFrequencies < 2)
	{
		return Melder_errorp1 (L"Roots_to_Spectrum: numberOfFrequencies must "
			"be greater or equal 2.");
	}
		
	if ((thee = Spectrum_create (nyquistFrequency, numberOfFrequencies))
		== NULL) return NULL;

	phi = NUMpi / (numberOfFrequencies - 1);
	
	for (i=1; i <= numberOfFrequencies; i++)
	{
		z.re = radius * cos ((i - 1) * phi);
		z.im = radius * sin ((i - 1) * phi);
		s = Roots_evaluate_z (me, z);
		thy z[1][i] = s.re; thy z[2][i] = s.im;
	}
	return thee;
}

long Roots_getNumberOfRoots (Roots me)
{
	return my max;
}

dcomplex Roots_getRoot (Roots me, long index)
{
	dcomplex root;
	
	if (index >= 1 && index <= my max)
	{
		root = my v[index];
	}
	else
	{
		(void) Melder_error1 (L"Roots_getRoot: root index out of range.");
		root.re = root.im = NUMundefined;
	}
	return root;
}

/* Can be speeded up by doing a FFT */
Spectrum Polynomial_to_Spectrum (Polynomial me, double nyquistFrequency,
	long numberOfFrequencies, double radius)
{
	Spectrum thee = NULL; double phi, re, im; long i;

	if (numberOfFrequencies < 2)
	{
		return Melder_errorp1 (L"Polynomial_to_Spectrum: numberOfFrequencies "
			"must be greater or equal 2.");
	}
	if ((thee = Spectrum_create (nyquistFrequency, numberOfFrequencies))
		== NULL) return NULL;

	phi = NUMpi / (numberOfFrequencies - 1);
	
	for (i = 1; i <= numberOfFrequencies; i++)
	{
		Polynomial_evaluate_z_cart (me, radius, (i - 1) * phi, &re, &im);
		thy z[1][i] = re; thy z[2][i] = im;
	}	
	return thee;
}

/****** ChebyshevSeries ******************************************/

/*
	p(x) = sum (k=1..numberOfCoefficients, c[k]*T[k](x')) - c[1] / 2;
	Numerical evaluation via Clenshaw's recurrence equation (NRC: 5.8.11)
	d[m+1] = d[m] = 0;
	d[j] = 2 x' d[j+1] - d[j+2] + c[j];
	p(x) = d[0] = x' d[1] - d[2] + c[0] / 2;
	x' = (2 * x - xmin - xmax) / (xmax - xmin)
*/
static double classChebyshevSeries_evaluate (I, double x)
{
	iam (ChebyshevSeries); double d1 = 0, d2 = 0, tmp, x2; long i; 
	
	if (x < my xmin || x > my xmax) return NUMundefined;
	
	if (my numberOfCoefficients > 1)
	{
		/* 
			Transform x from domain [xmin, xmax] to domain [-1, 1]
		*/
		
		x = (2 * x - my xmin - my xmax) / (my xmax - my xmin);
		x2 = 2 * x; 
						
		for (i = my numberOfCoefficients; i > 1; i--)
		{
			tmp = d1;
			d1 = x2 * d1 - d2 + my coefficients[i];
			d2 = tmp;
		}
	}
	return x * d1 - d2 + my coefficients[1];
}

/*
	T[n](x) = 2*x*T[n-1] - T[n-2](x)  n >= 2
*/
static void classChebyshevSeries_evaluateTerms (I, double x, double *terms)
{
	iam (ChebyshevSeries); long i;
	
	if (x < my xmin || x > my xmax)
	{
		for (i=1; i <= my numberOfCoefficients; i++) terms[i] = NUMundefined;
		return;
	}
	terms[1] = 1;
	if (my numberOfCoefficients > 1)
	{
		/* 
			Transform x from domain [xmin, xmax] to domain [-1, 1]
		*/
		
		terms[2] = x = (2 * x - my xmin - my xmax) / (my xmax - my xmin);
						
		for (i=3; i <= my numberOfCoefficients; i++)
		{
			terms[i] = 2 * x * terms[i-1] - terms[i-2];
		}
	}
}

static void classChebyshevSeries_getExtrema (I, double x1, double x2, 
	double *xmin, double *ymin, double *xmax, double *ymax)
{
	iam (ChebyshevSeries);
	Polynomial p = ChebyshevSeries_to_Polynomial (me);
	 
	if (p)
	{
		FunctionTerms_getExtrema (p, x1, x2, xmin, ymin, xmax, ymax);
		forget (p);
	}
	else
	{
		defaultGetExtrema (me, x1, x2, xmin, ymin, xmax, ymax);
	}
}

class_methods (ChebyshevSeries, FunctionTerms)
	class_method_local (ChebyshevSeries, evaluate)
	class_method_local (ChebyshevSeries, evaluateTerms)
	class_method_local (ChebyshevSeries, getExtrema)
class_methods_end

ChebyshevSeries ChebyshevSeries_create (double xmin, double xmax, 
	long numberOfPolynomials)
{
	ChebyshevSeries me = new (ChebyshevSeries);
	
	if (me == NULL) return NULL;
	if (! FunctionTerms_init (me, xmin, xmax, numberOfPolynomials)) forget (me);
	return me;
}

ChebyshevSeries ChebyshevSeries_createFromString (double xmin, double xmax,
	wchar_t *s)
{
	ChebyshevSeries me = new (ChebyshevSeries);
	
	if (me == NULL) return NULL;
	if (! FunctionTerms_initFromString (me, xmin, xmax, s, 0)) forget (me);
	return me;
}

Polynomial ChebyshevSeries_to_Polynomial (ChebyshevSeries me)
{
	Polynomial thee = NULL, s = NULL, t;
	long j, n;
	double *buf = NULL, a = 2, b = 0, c = -1, *pn, *pnm1, *pnm2;
	double xmin = -1, xmax = 1;

	if ((thee = Polynomial_create (xmin, xmax, my numberOfCoefficients - 1))
		== NULL) return NULL;
	
	thy coefficients[1] = my coefficients[1] /* * p[1] */;
	if (my numberOfCoefficients == 1) return thee;

	thy coefficients[2] = my coefficients[2];
	if (my numberOfCoefficients > 2)
	{
		if (! (buf = NUMdvector (1, 3 * my numberOfCoefficients))) goto end;
		
		pn = buf;
		pnm1 = pn   + my numberOfCoefficients;
		pnm2 = pnm1 + my numberOfCoefficients;
	
		/*
			Start the recursion: T[1] = x; T[0] = 1; 
		*/
	
		pnm1[2] = 1; pnm2[1] = 1;
		for (n=2; n <= my numberOfCoefficients - 1; n++)
		{
			double *t1 = pnm1, *t2 = pnm2;
			NUMpolynomial_recurrence (pn, n, a, b, c, pnm1, pnm2);	
			if (my coefficients[n+1] != 0)
			{
				for (j=1; j <= n+1; j++)
				{
					thy coefficients[j] += my coefficients[n+1] * pn[j];
				}
			}
			pnm1 = pn;
			pnm2 = t1;
			pn = t2;
		}
	}
	if (my xmin != xmin || my xmax != xmax)
	{
		if ((s = Polynomial_scaleX (thee, my xmin, my xmax)) == NULL) goto end;
		t = thee; thee = s; s = t;
		forget (s);
	}
end:
	NUMdvector_free (buf, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}


int FunctionTerms_and_RealTier_fit (I, thou, int *freeze, double tol,
	int ic, Covariance *c)
{
	iam (FunctionTerms);
	thouart (RealTier);
	FunctionTerms frozen = NULL;
	SVD svd = NULL;
	long i, j, k, numberOfData = thy points -> size;
	long numberOfParameters = my numberOfCoefficients;
	long numberOfFreeParameters = numberOfParameters;
	double *terms = NULL, *y_residual = NULL, *p = NULL, sigma;
	
	if (numberOfData < 2)
	{
		return Melder_error1 (L"FunctionTerms_and_RealTier_fit: not enough "
			"data points.");
	}
	
	if (((frozen = Data_copy (me)) == NULL) ||
		((terms = NUMdvector (1, my numberOfCoefficients)) == NULL) ||
		((p = NUMdvector (1, numberOfParameters)) == NULL) ||
		((y_residual = NUMdvector (1, numberOfData)) == NULL) ||
		(ic && ((*c = Covariance_create (numberOfParameters)) == NULL)))
			goto end;

	for (k=1, j=1; j <= my numberOfCoefficients; j++)
	{
		if (freeze && freeze[j])
		{
			numberOfFreeParameters--;
		}
		else
		{
			p[k] = my coefficients[j]; k++;
			frozen -> coefficients[j] = 0;
		}
	}
	
	if (numberOfFreeParameters == 0) goto end;

	if (! (svd = SVD_create (numberOfData, numberOfFreeParameters)))
		 goto end;
	
	sigma = RealTier_getStandardDeviation_points (thee, my xmin, my xmax);
	
	if (sigma == NUMundefined)
	{
		return Melder_error1 (L"FunctionTerms_and_RealTier_fit: not enough "
			"data points in fit interval.");
	}

	for (i=1; i <= numberOfData; i++)
	{
		/*
			Only 'residual variance' must be explained by the model
			Evaluate only with the frozen parameters
		*/
		RealPoint point = thy points -> item [i];
		double x = point -> time, y = point -> value, y_frozen, **u = svd -> u;
		
		y_frozen = numberOfFreeParameters == numberOfParameters ? 0 :
			FunctionTerms_evaluate (frozen, x);
				
		y_residual[i] = (y - y_frozen) / sigma;
		
		/*
			Data matrix
		*/
		
		FunctionTerms_evaluateTerms (me, x, terms);
		
		for (k=0, j=1; j <= my numberOfCoefficients; j++)
		{
			if (! freeze || ! freeze[j])
			{
				k++; u[i][k] = terms[j] / sigma;
			}
		}
	}

	/*
		SVD and evaluation of the singular values
	*/
	
	if (tol > 0) SVD_setTolerance (svd, tol);
	
	if (! SVD_compute (svd) ||
		! SVD_solve (svd, y_residual, p)) goto end;

	/*
		Put fitted values at correct position
	*/
	
	for (k=1, j=1; j <= my numberOfCoefficients; j++)
	{
		if (! freeze || ! freeze[j])
		{
			my coefficients[j] = p[k++];
		}
	}
	
	if (ic) svdcvm (svd -> v, numberOfFreeParameters, numberOfParameters,
		freeze, svd -> d, (*c) -> data);
end:
	forget (frozen); forget (svd);
	NUMdvector_free (terms, 1);
	NUMdvector_free (y_residual, 1);
	if (Melder_hasError ())
	{
		if (ic) forget (*c); return 0;
	}
	return 1;
}


Polynomial RealTier_to_Polynomial (I, long degree, double tol, int ic,
	Covariance *cvm)
{
	iam (RealTier);
	Polynomial thee = NULL;
	
	if ((thee = Polynomial_create (my xmin, my xmax, degree)) ||
		! FunctionTerms_and_RealTier_fit (thee, me, NULL, tol, ic, cvm))
			forget (thee);
	return thee;
}

LegendreSeries RealTier_to_LegendreSeries (I, long degree, double tol, int ic,
	Covariance *cvm)
{
	iam (RealTier);
	LegendreSeries thee = NULL;
	
	if (((thee = LegendreSeries_create (degree, my xmin, my xmax)) == NULL) ||
		! FunctionTerms_and_RealTier_fit (thee, me, NULL, tol, ic, cvm))
			forget (thee);
	return thee;
}

ChebyshevSeries RealTier_to_ChebyshevSeries (I, long degree, double tol, int ic,
	Covariance *cvm)
{
	iam (RealTier);
	ChebyshevSeries thee = NULL;
	
	if (((thee = ChebyshevSeries_create (degree, my xmin, my xmax)) == NULL) ||
		! FunctionTerms_and_RealTier_fit (thee, me, NULL, tol, ic, cvm))
			forget (thee);
	return thee;
}

/******* Splines *************************************************/

/*
	Functions for calculating an mspline and an ispline. These functions should replace
	the functions in NUM2.c. Before we can do that we first have to adapt the spline-
	dependencies in MDS.c.
	
	Formally nKnots == order + numberOfInteriorKnots + order.
	We forget about the multiple knots at start and end.
	Our point-sequece xmin < interiorKont[1] < ... < interiorKnot[n] < xmax.
	nKnots is now numberOfinteriorKnots + 2.
*/
static double NUMmspline2 (double points[], long numberOfPoints, long order,
	long index, double x)
{
	long numberOfSplines = numberOfPoints + order - 2;
	long index_b, index_e, j, k, k1, k2;
	double m[Spline_MAXIMUM_DEGREE + 2]; 
			
	Melder_assert (numberOfPoints > 2 && order > 0 && index > 0);
	
	if (index > numberOfSplines) return NUMundefined;
	
	/*
		Find the range/interval where x is located. 
		M-splines of order k have degree k-1. 
		M-splines are zero outside interval [ knot[i], knot[i+order] ).
		First and last 'order' knots are equal, i.e.,
		knot[1] = ... = knot[order] && knot[nKnots-order+1] = ... knot[nKnots].
	*/
	
	index_b = index - order + 1; index_b = MAX (index_b, 1);
	if (x < points[index_b]) return 0;
	
	index_e = index_b + MIN (index, order);
	index_e = MIN (numberOfPoints, index_e);
	if (x > points[index_e]) return 0;
			
	/*
		Calculate M[i](x|1,t) according to eq.2.
	*/

	for (k=1; k <= order; k++)
	{
		k1 = index - order + k; k2 = k1 + 1;
		m[k] = 0;
		if (k1 > 0 && k2 <= numberOfPoints && x >= points[k1] && x < points[k2])
		{
			m[k] = 1 / (points[k2] - points[k1]);
		}
	}
	/*
		Iterate to get M[i](x|k,t)
	*/
	
	for (k=2; k <= order; k++)
	{
		for (j=1; j <= order - k + 1; j++)
		{
			k1 = index - order + j; k2 = k1 + k;
			if (k2 > 1 && k1 < 1)
			{
				k1 = 1;
			}
			else if (k2 > numberOfPoints && k1 < numberOfPoints)
			{
				k2 = numberOfPoints;
			}
			if (k1 > 0 && k2 <= numberOfPoints)
			{
				double p1 = points[k1], p2 = points[k2];
				m[j] = k * ((x - p1) * m[j] + (p2 - x) * m[j+1]) / 
					((k - 1) * (p2 - p1));
			}
		}
	}
	return m[1];
}

static double NUMispline2 (double points[], long numberOfPoints, long order,
	long index, double x)
{
	long index_b, index_e, j, m, orderp1 = order + 1;
	double y = 0;
			
	Melder_assert (numberOfPoints > 2 && order > 0 && index > 0);
	
	index_b = index - order + 1;
	index_b = MAX (index_b, 1);
	
	if (x < points[index_b]) return 0;
	
	index_e = index_b + MIN (index, order);
	index_e = MIN (numberOfPoints, index_e);
	
	if (x > points[index_e]) return 1;
	
	for (j = index_e - 1; j >= index_b; j--)
	{
		if (x > points[j]) break;
	}
			
	/*
		Equation 5 in Ramsay's article contains some errors!!!
		1. the interval selection must be 'j-k <= i <= j' instead of
			'j-k+1 <= i <= j'
		2. the summation index m starts at 'i+1' instead of 'i'
	*/
	
	for (m=index+1; m <= j+order; m++)
	{
		long km = m - order, kmp = km + orderp1;
		km = MAX (km, 1);
		kmp = MIN (kmp, numberOfPoints);
		y += (points[kmp] - points[km]) *
			NUMmspline2 (points, numberOfPoints, orderp1, m, x); 
	}
	return y /= orderp1;
}

static double classSpline_evaluate (I, double x)
{
	(void) void_me; (void) x;
	return 0;
}

static long classSpline_getDegree (I)
{
	iam (Spline);
	return my degree;
}

static long classSpline_getOrder (I)
{
	iam (Spline); 
	return my degree + 1;
}

class_methods (Spline, FunctionTerms)
	class_method_local (Spline, destroy)
	class_method_local (Spline, equal)
	class_method_local (Spline, canWriteAsEncoding)
	class_method_local (Spline, copy)
	class_method_local (Spline, readText)
	class_method_local (Spline, readBinary)
	class_method_local (Spline, writeText)
	class_method_local (Spline, writeBinary)
	class_method_local (Spline, description)
	class_method_local (Spline, evaluate)
	class_method_local (Spline, getDegree)
	class_method_local (Spline, getOrder)
class_methods_end

/* Precondition: FunctionTerms part inited + degree */
static int Spline_initKnotsFromString (I, long degree, wchar_t *interiorKnots)
{
	iam (Spline); double *numbers = NULL;
	long i, n, numberOfInteriorKnots, order;
	
	if (degree > Spline_MAXIMUM_DEGREE)
	{
		return Melder_error1 (L"Spline_init: degree must be <= 20.");
	}
		 
	if ((numbers = NUMstring_to_numbers (interiorKnots, &numberOfInteriorKnots))
		==NULL) goto end;
	
	if (numberOfInteriorKnots > 0)
	{
		NUMsort_d (numberOfInteriorKnots, numbers);
		if (numbers[1] <= my xmin || numbers[numberOfInteriorKnots] > my xmax)
		{
			(void) Melder_error1 (L"Spline_initKnotsfromString: knots must be inside domain.");
		}
	}
	
	my degree = degree;
	order = Spline_getOrder (me); /* depends on spline type !! */
	n = numberOfInteriorKnots + order;
	
	if (my numberOfCoefficients != n)
	{
		(void) Melder_error2 (L"MSpline_createFromStrings: numberOfCoefficients"
			" must equal ", Melder_integer (n));
		goto end;
	}
	
	my numberOfKnots = numberOfInteriorKnots + 2;
	
	if (! (my knots = NUMdvector (1, my numberOfKnots))) goto end;
	
	for (i=1; i <= numberOfInteriorKnots; i++)
	{
		my knots[i+1] = numbers[i];
	}
	my knots[1] = my xmin;
	my knots[my numberOfKnots] = my xmax;
end:
	NUMdvector_free (numbers, 1);
	return ! Melder_hasError ();
}

int Spline_init (I, double xmin, double xmax, long degree,
	long numberOfCoefficients, long numberOfKnots)
{
	iam (Spline);
	
	if (degree > Spline_MAXIMUM_DEGREE)
	{
		return Melder_error1 (L"Spline_init: degree must be <= 20.");
	}	 
	
	if (! FunctionTerms_init (me, xmin, xmax, numberOfCoefficients)) return 0;
	
	if ((my knots = NUMdvector (1, numberOfKnots)) == NULL) return 0;
	
	my degree = degree;
	my numberOfKnots = numberOfKnots;
	my knots[1] = xmin;
	my knots[numberOfKnots] = xmax;
	
	return 1;
}

void Spline_drawKnots (I, Graphics g, double xmin, double xmax, double ymin, 
	double ymax, int garnish)
{
	iam (Spline);
	long i, order = Spline_getOrder (me);
	double x1, x2;
	wchar_t ts[20] = L"";
	
	if (xmax <= xmin)
	{
		xmin = my xmin; xmax = my xmax;
	}
	
	if (xmax < my xmin || xmin > my xmax) return;
	
	if (ymax <= ymin)
	{
		FunctionTerms_getExtrema (me, xmin, xmax, &x1, &ymin, &x2, &ymax);
	}
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	
	if (my knots[1] >= xmin && my knots[1] <= xmax)
	{
		if (garnish)
		{
    		if (order == 1) swprintf (ts, 20, L"t__1_");
			else if (order == 2) swprintf (ts, 20, L"{t__1_, t__2_}");
			else swprintf (ts, 20, L"{t__1_..t__%ld_}", order);
		}
		Graphics_markTop (g, my knots[1], 0, 1, 1, ts);
	}
	for (i=2; i <= my numberOfKnots - 1; i++)
	{
		if (my knots[i] >= xmin && my knots[i] <= xmax)
		{
			if (garnish) swprintf (ts, 20, L"t__%ld_", i + order - 1);
			Graphics_markTop (g, my knots[i], 0, 1, 1, ts); 
		}
	}
	if (my knots[my numberOfKnots] >= xmin &&
		my knots[my numberOfKnots] <= xmax)
	{
		if (garnish)
		{
			long numberOfKnots = my numberOfKnots + 2 * (order - 1);
    		if (order == 1)
			{
				swprintf (ts, 20, L"t__%ld_", numberOfKnots);
			}
			else if (order == 2)
			{
				swprintf (ts, 20, L"{t__%d_, t__%ld_}", numberOfKnots - 1,
					numberOfKnots);
			}
			else
			{
				swprintf (ts, 20, L"{t__%d_..t__%ld_}", numberOfKnots - order + 1,
					numberOfKnots);
			}
		}
		Graphics_markTop (g, my knots[my numberOfKnots], 0, 1, 1, ts);
	}
}

long Spline_getOrder (I)
{
	iam (Spline);
	return our getOrder (me);
}

Spline Spline_scaleX (I, double xmin, double xmax)
{
	iam (Spline);
	Spline thee;
	double a, b;
	long i; 
	
	Melder_assert (xmin < xmax);
		
	if ((thee = Data_copy (me)) == NULL) return NULL;
		
	thy xmin = xmin; thy xmax = xmax;

	/*
		x = a x + b
		Constraints:
		my xmin = a xmin + b;    a = (my xmin - my xmax) / (xmin - xmax);
		my xmax = a xmax + b;    b = my xmin - a * xmin
	*/
	
	a = (xmin - xmax) / (my xmin - my xmax);
	b = xmin - a * my xmin;
	for (i=1; i <= my numberOfKnots; i++)
	{
		thy knots[i] = a * my knots[i] + b;
	}
	return thee;
}	

/********* MSplines ************************************************/

static double classMSpline_evaluate (I, double x)
{
	iam (MSpline); long i; double result = 0;
	
	if (x < my xmin || x > my xmax) return 0;
	
	for (i=1; i <= my numberOfCoefficients; i++)
	{
		if (my coefficients[i] != 0) result += my coefficients[i] *
			NUMmspline2 (my knots, my numberOfKnots, my degree + 1, i, x);
	}
	return result;
}

static void classMSpline_evaluateTerms (I, double x, double *terms)
{
	iam (MSpline); long i;
	
	if (x < my xmin || x > my xmax) return;
	
	for (i=1; i <= my numberOfCoefficients; i++)
	{
		terms[i] = NUMmspline2 (my knots, my numberOfKnots,
			my degree + 1, i, x);
	}
}

class_methods (MSpline, Spline)
	class_method_local (MSpline, evaluate)
	class_method_local (MSpline, evaluateTerms)
class_methods_end

MSpline MSpline_create (double xmin, double xmax, long degree, long numberOfInteriorKnots)
{
	MSpline me = new (MSpline);
	long numberOfCoefficients = numberOfInteriorKnots + degree + 1;
	long numberOfKnots = numberOfCoefficients + degree + 1;
	
	if (me == NULL) return NULL;
	if (! Spline_init (me, xmin, xmax, degree, numberOfCoefficients,
		numberOfKnots)) forget (me);
	return me;
}

MSpline MSpline_createFromStrings (double xmin, double xmax, long degree,
	wchar_t *coef, wchar_t *interiorKnots)
{
	MSpline me = new (MSpline);
	
	if (degree > Spline_MAXIMUM_DEGREE)
	{
		return Melder_errorp1 (L"MSpline_createFromStrings: degree must be <= 20.");
	}	 
		
	if ((me == NULL) ||
		! FunctionTerms_initFromString (me, xmin, xmax, coef, 1) ||
		! Spline_initKnotsFromString (me, degree, interiorKnots)) forget (me);
	
	return me;
}

/******** ISplines ************************************************/

static double classISpline_evaluate (I, double x)
{
	iam (ISpline); long i; double result = 0;
	
	if (x < my xmin || x > my xmax) return 0;
		
	for (i=1; i <= my numberOfCoefficients; i++)
	{
		if (my coefficients[i] != 0) result += my coefficients[i] *
			NUMispline2 (my knots, my numberOfKnots, my degree, i, x);
	}
	
	return result;
}

static void classISpline_evaluateTerms (I, double x, double *terms)
{
	iam (ISpline); long i;
		
	for (i=1; i <= my numberOfCoefficients; i++)
	{
		terms[i] = NUMispline2 (my knots, my numberOfKnots, my degree, i, x);
	}
}

static long classISpline_getOrder (I) { iam (ISpline); return my degree; }

class_methods (ISpline, Spline)
	class_method_local (ISpline, evaluate)
	class_method_local (ISpline, evaluateTerms)
	class_method_local (ISpline, getOrder)
class_methods_end

ISpline ISpline_create (double xmin, double xmax, long degree,
	long numberOfInteriorKnots)
{
	ISpline me = new (ISpline);
	long numberOfCoefficients = numberOfInteriorKnots + degree;
	long numberOfKnots = numberOfCoefficients + degree;
	
	if (me == NULL) return NULL;
	if (! Spline_init (me, xmin, xmax, degree, numberOfCoefficients,
		numberOfKnots)) forget (me);
	return me;
}

ISpline ISpline_createFromStrings (double xmin, double xmax, long degree,
	wchar_t *coef, wchar_t *interiorKnots)
{
	ISpline me = new (ISpline);
	
	if (degree > Spline_MAXIMUM_DEGREE)
	{
		return Melder_errorp1 (L"ISpline_createFromStrings: degree must be <= 20.");	 
	}
	if ((me == NULL) ||
		! FunctionTerms_initFromString (me, xmin, xmax, coef, 1) ||
		! Spline_initKnotsFromString (me, degree, interiorKnots)) forget (me);
	
	return me;
}

/*

#define RationalFunction_members Function_members \
	Polynomial num, denum;
#define RationalFunction_methods Function_methods
class_create (RationalFunction, Function)	

RationalFunction RationalFunction_create (double xmin, double xmax,
	long degree_num, long degree_denum)
{
	RationalFunction me = new (RationalFunction);
	if (! me || ! (my num = Polynomial_create (xmin, xmax, degree_num)) ||
		!  (my denum = Polynomial_create (xmin, xmax, degree_denum))) forget (me);
	return me;
}

RationalFunction RationalFunction_createFromString (I, double xmin, double xmax,
	char *num, char *denum)
{
	RationalFunction me = new (RationalFunction); long i;
	
	if (! (my num = Polynomial_createFromString (xmin, xmax, num)) ||
		! (my denum = Polynomial_createFromString (xmin, xmax, denum))) forget (me);
	if (my denum -> v[1] != 1 && my denum -> v[1] != 0)
	{
		double q0 = my denum -> v[1];
		for (i=1; 1 <= my num ->numberOfCoefficients; i++) my num -> v[i] /= q0;
		for (i=1; 1 <= my denum ->numberOfCoefficients; i++) my denum -> v[i] /= q0;
	}
	return me;
}

// divide out common roots
RationalFunction RationalFunction_simplify (RationalFunction me)
{
	Roots num = NULL, denum = NULL; RationalFunction thee = NULL;
	if (! (num = Polynomial_to_Roots (my num)) ||
		! (denum = Polynomial_to_Roots (my denum))) goto end;
	
}

*/

#undef MAX
#undef MIN

/* end of file Polynomial.c */
