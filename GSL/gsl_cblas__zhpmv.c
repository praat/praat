#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_zhpmv (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const int N, const void *alpha, const void *Ap, const void *X,
             const int incX, const void *beta, void *Y, const int incY)
{
#define BASE double
#include "gsl_cblas__source_hpmv.h"
#undef BASE
}
