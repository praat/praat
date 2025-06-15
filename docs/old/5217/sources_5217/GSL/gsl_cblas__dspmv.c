#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dspmv (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const int N, const double alpha, const double *Ap,
             const double *X, const int incX, const double beta, double *Y,
             const int incY)
{
#define BASE double
#include "gsl_cblas__source_spmv.h"
#undef BASE
}
