#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dtbmv (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
             const int N, const int K, const double *A, const int lda,
             double *X, const int incX)
{
#define BASE double
#include "gsl_cblas__source_tbmv_r.h"
#undef BASE
}
