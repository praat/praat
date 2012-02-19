#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_ssymm (const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
             const enum CBLAS_UPLO Uplo, const int M, const int N,
             const float alpha, const float *A, const int lda, const float *B,
             const int ldb, const float beta, float *C, const int ldc)
{
#define BASE float
#include "gsl_cblas__source_symm_r.h"
#undef BASE
}
