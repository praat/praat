#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dger (const enum CBLAS_ORDER order, const int M, const int N,
            const double alpha, const double *X, const int incX,
            const double *Y, const int incY, double *A, const int lda)
{
#define BASE double
#include "gsl_cblas__source_ger.h"
#undef BASE
}
