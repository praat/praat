#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_poly.h"

/* Find a minimum in x=[0,1] of the interpolating quadratic through
 * (0,f0) (1,f1) with derivative fp0 at x=0.  The interpolating
 * polynomial is q(x) = f0 + fp0 * z + (f1-f0-fp0) * z^2
 */

static double
interp_quad (double f0, double fp0, double f1, double zl, double zh)
{
  double fl = f0 + zl*(fp0 + zl*(f1 - f0 -fp0));
  double fh = f0 + zh*(fp0 + zh*(f1 - f0 -fp0));
  double c = 2 * (f1 - f0 - fp0);       /* curvature */

  double zmin = zl, fmin = fl;

  if (fh < fmin) { zmin = zh; fmin = fh; } 

  if (c > 0)  /* positive curvature required for a minimum */
    {
      double z = -fp0 / c;      /* location of minimum */
      if (z > zl && z < zh) {
        double f = f0 + z*(fp0 + z*(f1 - f0 -fp0));
        if (f < fmin) { zmin = z; fmin = f; };
      }
    }

  return zmin;
}

/* Find a minimum in x=[0,1] of the interpolating cubic through
 * (0,f0) (1,f1) with derivatives fp0 at x=0 and fp1 at x=1.
 *
 * The interpolating polynomial is:
 *
 * c(x) = f0 + fp0 * z + eta * z^2 + xi * z^3
 *
 * where eta=3*(f1-f0)-2*fp0-fp1, xi=fp0+fp1-2*(f1-f0). 
 */

static double
cubic (double c0, double c1, double c2, double c3, double z)
{
  return c0 + z * (c1 + z * (c2 + z * c3));
}

static void
check_extremum (double c0, double c1, double c2, double c3, double z,
                double *zmin, double *fmin)
{
  /* could make an early return by testing curvature >0 for minimum */

  double y = cubic (c0, c1, c2, c3, z);

  if (y < *fmin)  
    {
      *zmin = z;  /* accepted new point*/
      *fmin = y;
    }
}

static double
interp_cubic (double f0, double fp0, double f1, double fp1, double zl, double zh)
{
  double eta = 3 * (f1 - f0) - 2 * fp0 - fp1;
  double xi = fp0 + fp1 - 2 * (f1 - f0);
  double c0 = f0, c1 = fp0, c2 = eta, c3 = xi;
  double zmin, fmin;
  double z0, z1;

  zmin = zl; fmin = cubic(c0, c1, c2, c3, zl);
  check_extremum (c0, c1, c2, c3, zh, &zmin, &fmin);

  {
    int n = gsl_poly_solve_quadratic (3 * c3, 2 * c2, c1, &z0, &z1);
    
    if (n == 2)  /* found 2 roots */
      {
        if (z0 > zl && z0 < zh) 
          check_extremum (c0, c1, c2, c3, z0, &zmin, &fmin);
        if (z1 > zl && z1 < zh) 
          check_extremum (c0, c1, c2, c3, z1, &zmin, &fmin);
      }
    else if (n == 1)  /* found 1 root */
      {
        if (z0 > zl && z0 < zh) 
          check_extremum (c0, c1, c2, c3, z0, &zmin, &fmin);
      }
  }

  return zmin;
}


static double
interpolate (double a, double fa, double fpa,
             double b, double fb, double fpb, double xmin, double xmax,
             int order)
{
  /* Map [a,b] to [0,1] */
  double z, alpha, zmin, zmax;

  zmin = (xmin - a) / (b - a);
  zmax = (xmax - a) / (b - a);

  if (zmin > zmax)
    {
      double tmp = zmin;
      zmin = zmax;
      zmax = tmp;
    };
  
  if (order > 2 && GSL_IS_REAL(fpb)) {
    z = interp_cubic (fa, fpa * (b - a), fb, fpb * (b - a), zmin, zmax);
  } else {
    z = interp_quad (fa, fpa * (b - a), fb, zmin, zmax);
  }

  alpha = a + z * (b - a);

  return alpha;
}

/* recommended values from Fletcher are 
   rho = 0.01, sigma = 0.1, tau1 = 9, tau2 = 0.05, tau3 = 0.5 */

static int
minimize (gsl_function_fdf * fn, double rho, double sigma, 
          double tau1, double tau2, double tau3,
          int order, double alpha1, double *alpha_new)
{
  double f0, fp0, falpha, falpha_prev, fpalpha, fpalpha_prev, delta,
    alpha_next;
  double alpha = alpha1, alpha_prev = 0.0;
  double a, b, fa, fb, fpa, fpb;
  const size_t bracket_iters = 100, section_iters = 100;
  size_t i = 0;

  GSL_FN_FDF_EVAL_F_DF (fn, 0.0, &f0, &fp0);
  falpha_prev = f0;
  fpalpha_prev = fp0;

  /* Avoid uninitialized variables morning */
  a = 0.0; b = alpha;
  fa = f0; fb = 0.0;
  fpa = fp0; fpb = 0.0;

  /* Begin bracketing */  

  while (i++ < bracket_iters)
    {
      falpha = GSL_FN_FDF_EVAL_F (fn, alpha);

      /* Fletcher's rho test */

      if (falpha > f0 + alpha * rho * fp0 || falpha >= falpha_prev)
        {
          a = alpha_prev; fa = falpha_prev; fpa = fpalpha_prev;
          b = alpha; fb = falpha; fpb = GSL_NAN;
          break;                /* goto sectioning */
        }

      fpalpha = GSL_FN_FDF_EVAL_DF (fn, alpha);

      /* Fletcher's sigma test */

      if (fabs (fpalpha) <= -sigma * fp0)
        {
          *alpha_new = alpha;
          return GSL_SUCCESS;
        }

      if (fpalpha >= 0)
        {
          a = alpha; fa = falpha; fpa = fpalpha;
          b = alpha_prev; fb = falpha_prev; fpb = fpalpha_prev;
          break;                /* goto sectioning */
        }

      delta = alpha - alpha_prev;

      {
        double lower = alpha + delta;
        double upper = alpha + tau1 * delta;

        alpha_next = interpolate (alpha_prev, falpha_prev, fpalpha_prev,
                             alpha, falpha, fpalpha, lower, upper, order);

      }

      alpha_prev = alpha;
      falpha_prev = falpha;
      fpalpha_prev = fpalpha;
      alpha = alpha_next;
    }

  /*  Sectioning of bracket [a,b] */
  
  while (i++ < section_iters)
    {
      delta = b - a;
      
      {
        double lower = a + tau2 * delta;
        double upper = b - tau3 * delta;
        
        alpha = interpolate (a, fa, fpa, b, fb, fpb, lower, upper, order);
      }
      
      falpha = GSL_FN_FDF_EVAL_F (fn, alpha);
      
      if ((a-alpha)*fpa <= GSL_DBL_EPSILON) {
        /* roundoff prevents progress */
        return GSL_ENOPROG;
      };

      if (falpha > f0 + rho * alpha * fp0 || falpha >= fa)
        {
          /*  a_next = a; */
          b = alpha; fb = falpha; fpb = GSL_NAN;
        }
      else
        {
          fpalpha = GSL_FN_FDF_EVAL_DF (fn, alpha);
          
          if (fabs(fpalpha) <= -sigma * fp0)
            {
              *alpha_new = alpha;
              return GSL_SUCCESS;  /* terminate */
            }
          
          if ( ((b-a) >= 0 && fpalpha >= 0) || ((b-a) <=0 && fpalpha <= 0))
            {
              b = a; fb = fa; fpb = fpa;
              a = alpha; fa = falpha; fpa = fpalpha;
            }
          else
            {
              a = alpha; fa = falpha; fpa = fpalpha;
            }
        }
    }

  return GSL_SUCCESS;
}
