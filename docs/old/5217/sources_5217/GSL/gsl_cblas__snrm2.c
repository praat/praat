#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

float
cblas_snrm2 (const int N, const float *X, const int incX)
{
#define BASE float
#include "gsl_cblas__source_nrm2_r.h"
#undef BASE
}
