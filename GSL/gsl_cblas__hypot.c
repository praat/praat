#include <math.h>

static double xhypot (const double x, const double y);

static double xhypot (const double x, const double y)
{
  double xabs = fabs(x) ;
  double yabs = fabs(y) ;
  double min, max;

  if (xabs < yabs) {
    min = xabs ;
    max = yabs ;
  } else {
    min = yabs ;
    max = xabs ;
  }

  if (min == 0) 
    {
      return max ;
    }

  {
    double u = min / max ;
    return max * sqrt (1 + u * u) ;
  }
}
