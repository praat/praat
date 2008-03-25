#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_sgemv (const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA,
             const int M, const int N, const float alpha, const float *A,
             const int lda, const float *X, const int incX, const float beta,
             float *Y, const int incY)
{
#define BASE float
#include "gsl_cblas__source_gemv_r.h"
#undef BASE
}
