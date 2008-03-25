#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_cher (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
            const int N, const float alpha, const void *X, const int incX,
            void *A, const int lda)
{
#define BASE float
#include "gsl_cblas__source_her.h"
#undef BASE
}
