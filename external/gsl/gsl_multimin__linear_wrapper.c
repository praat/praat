typedef struct
{
  gsl_function_fdf fdf_linear;
  gsl_multimin_function_fdf *fdf;
  /* fixed values */
  const gsl_vector *x;
  const gsl_vector *g;
  const gsl_vector *p;

  /* cached values, for x(alpha) = x + alpha * p */
  double f_alpha;
  double df_alpha;
  gsl_vector *x_alpha;
  gsl_vector *g_alpha;

  /* cache "keys" */
  double f_cache_key;
  double df_cache_key;
  double x_cache_key;
  double g_cache_key;
}
wrapper_t;

static void
moveto (double alpha, wrapper_t * w)
{
  if (alpha == w->x_cache_key)  /* using previously cached position */
    {
      return;
    }

  /* set x_alpha = x + alpha * p */

  gsl_vector_memcpy (w->x_alpha, w->x);
  gsl_blas_daxpy (alpha, w->p, w->x_alpha);

  w->x_cache_key = alpha;
}

static double
slope (wrapper_t * w)           /* compute gradient . direction */
{
  double df;
  gsl_blas_ddot (w->g_alpha, w->p, &df);
  return df;
}

static double
wrap_f (double alpha, void *params)
{
  wrapper_t *w = (wrapper_t *) params;
  if (alpha == w->f_cache_key)  /* using previously cached f(alpha) */
    {
      return w->f_alpha;
    }

  moveto (alpha, w);

  w->f_alpha = GSL_MULTIMIN_FN_EVAL_F (w->fdf, w->x_alpha);
  w->f_cache_key = alpha;

  return w->f_alpha;
}

static double
wrap_df (double alpha, void *params)
{
  wrapper_t *w = (wrapper_t *) params;
  if (alpha == w->df_cache_key) /* using previously cached df(alpha) */
    {
      return w->df_alpha;
    }

  moveto (alpha, w);

  if (alpha != w->g_cache_key) 
    {
      GSL_MULTIMIN_FN_EVAL_DF (w->fdf, w->x_alpha, w->g_alpha);
      w->g_cache_key = alpha;
    }

  w->df_alpha = slope (w);
  w->df_cache_key = alpha;

  return w->df_alpha;
}

static void
wrap_fdf (double alpha, void *params, double *f, double *df)
{
  wrapper_t *w = (wrapper_t *) params;

  /* Check for previously cached values */

  if (alpha == w->f_cache_key && alpha == w->df_cache_key)
    {
      *f = w->f_alpha;
      *df = w->df_alpha;
      return;
    }

  if (alpha == w->f_cache_key || alpha == w->df_cache_key)
    {
      *f = wrap_f (alpha, params);
      *df = wrap_df (alpha, params);
      return;
    }

  moveto (alpha, w);
  GSL_MULTIMIN_FN_EVAL_F_DF (w->fdf, w->x_alpha, &w->f_alpha, w->g_alpha);
  w->f_cache_key = alpha;
  w->g_cache_key = alpha;

  w->df_alpha = slope (w);
  w->df_cache_key = alpha;

  *f = w->f_alpha;
  *df = w->df_alpha;
}

static void
prepare_wrapper (wrapper_t * w, gsl_multimin_function_fdf * fdf,
                 const gsl_vector * x, double f, const gsl_vector *g, 
                 const gsl_vector * p,
                 gsl_vector * x_alpha, gsl_vector *g_alpha)
{
  w->fdf_linear.f = &wrap_f;
  w->fdf_linear.df = &wrap_df;
  w->fdf_linear.fdf = &wrap_fdf;
  w->fdf_linear.params = (void *)w;  /* pointer to "self" */

  w->fdf = fdf;

  w->x = x;
  w->g = g;
  w->p = p;

  w->x_alpha = x_alpha;
  w->g_alpha = g_alpha;

  gsl_vector_memcpy(w->x_alpha, w->x); 
  w->x_cache_key = 0.0;
  
  w->f_alpha = f;
  w->f_cache_key = 0.0;
  
  gsl_vector_memcpy(w->g_alpha, w->g);
  w->g_cache_key = 0.0;

  w->df_alpha = slope(w);
  w->df_cache_key = 0.0;
}

static void
update_position (wrapper_t * w, double alpha, gsl_vector *x, double *f, gsl_vector *g)
{
  /* ensure that everything is fully cached */
  { double f_alpha, df_alpha; wrap_fdf (alpha, w, &f_alpha, &df_alpha); } ;

  *f = w->f_alpha;
  gsl_vector_memcpy(x, w->x_alpha);
  gsl_vector_memcpy(g, w->g_alpha);
}  

static void
change_direction (wrapper_t * w)
{
  /* Convert the cache values from the end of the current minimisation
     to those needed for the start of the next minimisation, alpha=0 */

  /* The new x_alpha for alpha=0 is the current position */
  gsl_vector_memcpy (w->x_alpha, w->x);
  w->x_cache_key = 0.0;

  /* The function value does not change */
  w->f_cache_key = 0.0;

  /* The new g_alpha for alpha=0 is the current gradient at the endpoint */
  gsl_vector_memcpy (w->g_alpha, w->g);
  w->g_cache_key = 0.0;

  /* Calculate the slope along the new direction vector, p */
  w->df_alpha = slope (w);
  w->df_cache_key = 0.0;
}
