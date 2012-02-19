#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_drot (const int N, double *X, const int incX, double *Y, const int incY,
            const double c, const double s)
{
#define BASE double
#include "gsl_cblas__source_rot.h"
#undef BASE
}
