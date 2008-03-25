#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_sger (const enum CBLAS_ORDER order, const int M, const int N,
            const float alpha, const float *X, const int incX, const float *Y,
            const int incY, float *A, const int lda)
{
#define BASE float
#include "gsl_cblas__source_ger.h"
#undef BASE
}
