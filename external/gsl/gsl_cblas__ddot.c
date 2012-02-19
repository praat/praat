#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

double
cblas_ddot (const int N, const double *X, const int incX, const double *Y,
            const int incY)
{
#define INIT_VAL  0.0
#define ACC_TYPE  double
#define BASE double
#include "gsl_cblas__source_dot_r.h"
#undef ACC_TYPE
#undef BASE
#undef INIT_VAL
}
