/* Author: G. Jungman
 */
#include "gsl__config.h"
#include <stdlib.h>
#include <string.h>
#include "gsl_errno.h"
#include "gsl_qrng.h"


gsl_qrng *
gsl_qrng_alloc (const gsl_qrng_type * T, unsigned int dimension)
{

  gsl_qrng * q = (gsl_qrng *) malloc (sizeof (gsl_qrng));

  if (q == 0)
    {
      GSL_ERROR_VAL ("allocation failed for qrng struct",
                        GSL_ENOMEM, 0);
    };

  q->dimension = dimension;
  q->state_size = T->state_size(dimension);
  q->state = malloc (q->state_size);

  if (q->state == 0)
    {
      free (q);
      GSL_ERROR_VAL ("allocation failed for qrng state",
                        GSL_ENOMEM, 0);
    };

  q->type = T;

  T->init_state(q->state, q->dimension);

  return q;
}

void
gsl_qrng_init (gsl_qrng * q)
{
  (q->type->init_state) (q->state, q->dimension);
}

int
gsl_qrng_memcpy (gsl_qrng * dest, const gsl_qrng * src)
{
  if (dest->type != src->type)
    {
      GSL_ERROR ("generators must be of the same type", GSL_EINVAL);
    }

  dest->dimension = src->dimension;
  dest->state_size = src->state_size;
  memcpy (dest->state, src->state, src->state_size);

  return GSL_SUCCESS;
}


gsl_qrng *
gsl_qrng_clone (const gsl_qrng * q)
{
  gsl_qrng * r = (gsl_qrng *) malloc (sizeof (gsl_qrng));

  if (r == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for rng struct",
                        GSL_ENOMEM, 0);
    };

  r->dimension = q->dimension;
  r->state_size = q->state_size;
  r->state = malloc (r->state_size);

  if (r->state == 0)
    {
      free (r);
      GSL_ERROR_VAL ("failed to allocate space for rng state",
                        GSL_ENOMEM, 0);
    };

  r->type = q->type;

  memcpy (r->state, q->state, q->state_size);

  return r;
}

#ifndef HIDE_INLINE_STATIC
int
gsl_qrng_get (const gsl_qrng * q, double x[])
{
  return (q->type->get) (q->state, q->dimension, x);
}
#endif

const char *
gsl_qrng_name (const gsl_qrng * q)
{
  return q->type->name;
}


size_t
gsl_qrng_size (const gsl_qrng * q)
{
  return q->state_size;
}


void *
gsl_qrng_state (const gsl_qrng * q)
{
  return q->state;
}


void
gsl_qrng_free (gsl_qrng * q)
{
  if(q != 0) {
    if(q->state != 0) free (q->state);
    free (q);
  }
}
