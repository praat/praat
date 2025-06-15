#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_srot (const int N, float *X, const int incX, float *Y, const int incY,
            const float c, const float s)
{
#define BASE float
#include "gsl_cblas__source_rot.h"
#undef BASE
}
