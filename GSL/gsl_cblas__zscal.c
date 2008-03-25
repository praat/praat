#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_zscal (const int N, const void *alpha, void *X, const int incX)
{
#define BASE double
#include "gsl_cblas__source_scal_c.h"
#undef BASE
}
