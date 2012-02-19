static inline int
cheb_eval_mode_e(const cheb_series * cs,
                 const double x,
                 gsl_mode_t mode,
                 gsl_sf_result * result)
{
  int j;
  double d  = 0.0;
  double dd = 0.0;

  double y  = (2.*x - cs->a - cs->b) / (cs->b - cs->a);
  double y2 = 2.0 * y;

  int eval_order;

  if(GSL_MODE_PREC(mode) == GSL_PREC_DOUBLE)
    eval_order = cs->order;
  else
    eval_order = cs->order_sp;

  for(j = eval_order; j>=1; j--) {
    double temp = d;
    d = y2*d - dd + cs->c[j];
    dd = temp;
  }

  result->val = y*d - dd + 0.5 * cs->c[0];
  result->err = GSL_DBL_EPSILON * fabs(result->val) + fabs(cs->c[eval_order]);
  return GSL_SUCCESS;
}
