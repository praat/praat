#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dgbmv (const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA,
             const int M, const int N, const int KL, const int KU,
             const double alpha, const double *A, const int lda,
             const double *X, const int incX, const double beta, double *Y,
             const int incY)
{
#define BASE double
#include "gsl_cblas__source_gbmv_r.h"
#undef BASE
}
