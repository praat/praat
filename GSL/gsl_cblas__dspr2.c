#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dspr2 (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const int N, const double alpha, const double *X, const int incX,
             const double *Y, const int incY, double *Ap)
{
#define BASE double
#include "gsl_cblas__source_spr2.h"
#undef BASE
}
