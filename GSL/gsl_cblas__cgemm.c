#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_cgemm (const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
             const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
             const int K, const void *alpha, const void *A, const int lda,
             const void *B, const int ldb, const void *beta, void *C,
             const int ldc)
{
#define BASE float
#include "gsl_cblas__source_gemm_c.h"
#undef BASE
}
