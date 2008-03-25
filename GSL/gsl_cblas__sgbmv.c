#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_sgbmv (const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA,
             const int M, const int N, const int KL, const int KU,
             const float alpha, const float *A, const int lda, const float *X,
             const int incX, const float beta, float *Y, const int incY)
{
#define BASE float
#include "gsl_cblas__source_gbmv_r.h"
#undef BASE
}
