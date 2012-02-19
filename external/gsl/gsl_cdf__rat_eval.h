static double
rat_eval (const double a[], const size_t na,
          const double b[], const size_t nb, const double x)
{
  size_t i, j;
  double u, v, r;

  u = a[na - 1];

  for (i = na - 1; i > 0; i--)
    {
      u = x * u + a[i - 1];
    }

  v = b[nb - 1];

  for (j = nb - 1; j > 0; j--)
    {
      v = x * v + b[j - 1];
    }

  r = u / v;

  return r;
}
