#include "f2c.h"

double d_sign(double *a, double *b)
{
double x;
x = (*a >= 0 ? *a : - *a);
return( *b >= 0 ? x : -x);
}

