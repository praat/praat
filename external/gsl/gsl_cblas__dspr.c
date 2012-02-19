#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dspr (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
            const int N, const double alpha, const double *X, const int incX,
            double *Ap)
{
#define BASE double
#include "gsl_cblas__source_spr.h"
#undef BASE
}
