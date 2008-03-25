#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_zgbmv (const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA,
             const int M, const int N, const int KL, const int KU,
             const void *alpha, const void *A, const int lda, const void *X,
             const int incX, const void *beta, void *Y, const int incY)
{
#define BASE double
#include "gsl_cblas__source_gbmv_c.h"
#undef BASE
}
