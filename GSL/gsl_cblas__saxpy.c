#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_saxpy (const int N, const float alpha, const float *X, const int incX,
             float *Y, const int incY)
{
#define BASE float
#include "gsl_cblas__source_axpy_r.h"
#undef BASE
}
