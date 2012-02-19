#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dsymv (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const int N, const double alpha, const double *A, const int lda,
             const double *X, const int incX, const double beta, double *Y,
             const int incY)
{
#define BASE double
#include "gsl_cblas__source_symv.h"
#undef BASE
}
