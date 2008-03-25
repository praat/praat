#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_stbsv (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
             const int N, const int K, const float *A, const int lda,
             float *X, const int incX)
{
#define BASE float
#include "gsl_cblas__source_tbsv_r.h"
#undef BASE
}
