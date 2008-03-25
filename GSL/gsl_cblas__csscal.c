#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_csscal (const int N, const float alpha, void *X, const int incX)
{
#define BASE float
#include "gsl_cblas__source_scal_c_s.h"
#undef BASE
}
