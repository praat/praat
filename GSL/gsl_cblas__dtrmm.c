#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dtrmm (const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
             const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
             const enum CBLAS_DIAG Diag, const int M, const int N,
             const double alpha, const double *A, const int lda, double *B,
             const int ldb)
{
#define BASE double
#include "gsl_cblas__source_trmm_r.h"
#undef BASE
}
