#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_drotm (const int N, double *X, const int incX, double *Y,
             const int incY, const double *P)
{
#define BASE double
#include "gsl_cblas__source_rotm.h"
#undef BASE
}
