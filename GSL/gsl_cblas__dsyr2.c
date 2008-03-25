#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dsyr2 (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const int N, const double alpha, const double *X, const int incX,
             const double *Y, const int incY, double *A, const int lda)
{
#define BASE double
#include "gsl_cblas__source_syr2.h"
#undef BASE
}
