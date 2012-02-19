#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_ccopy (const int N, const void *X, const int incX, void *Y,
             const int incY)
{
#define BASE float
#include "gsl_cblas__source_copy_c.h"
#undef BASE
}
