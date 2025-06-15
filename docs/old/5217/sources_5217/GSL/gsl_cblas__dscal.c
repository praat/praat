#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dscal (const int N, const double alpha, double *X, const int incX)
{
#define BASE double
#include "gsl_cblas__source_scal_r.h"
#undef BASE
}
