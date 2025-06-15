#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

#include "gsl_cblas__hypot.c"

void
cblas_ztrsm (const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
             const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
             const enum CBLAS_DIAG Diag, const int M, const int N,
             const void *alpha, const void *A, const int lda, void *B,
             const int ldb)
{
#define BASE double
#include "gsl_cblas__source_trsm_c.h"
#undef BASE
}
