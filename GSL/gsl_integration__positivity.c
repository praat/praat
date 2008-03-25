/* Compare the integral of f(x) with the integral of |f(x)|
   to determine if f(x) covers both positive and negative values */

static inline int
test_positivity (double result, double resabs);

static inline int
test_positivity (double result, double resabs)
{
  int status = (fabs (result) >= (1 - 50 * GSL_DBL_EPSILON) * resabs);

  return status;
}
