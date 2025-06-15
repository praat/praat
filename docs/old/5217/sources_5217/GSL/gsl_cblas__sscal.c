#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_sscal (const int N, const float alpha, float *X, const int incX)
{
#define BASE float
#include "gsl_cblas__source_scal_r.h"
#undef BASE
}
