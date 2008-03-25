/* err/strerror.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "gsl__config.h"
#include "gsl_errno.h"

const char *
gsl_strerror (const int gsl_errno)
{
  switch (gsl_errno)
    {
    case GSL_SUCCESS:
      return "success" ;
    case GSL_FAILURE:
      return "failure" ;
    case GSL_CONTINUE:
      return "the iteration has not converged yet";
    case GSL_EDOM:
      return "input domain error" ;
    case GSL_ERANGE:
      return "output range error" ;
    case GSL_EFAULT:
      return "invalid pointer" ;
    case GSL_EINVAL:
      return "invalid argument supplied by user" ;
    case GSL_EFAILED:
      return "generic failure" ;
    case GSL_EFACTOR:
      return "factorization failed" ;
    case GSL_ESANITY:
      return "sanity check failed - shouldn't happen" ;
    case GSL_ENOMEM:
      return "malloc failed" ;
    case GSL_EBADFUNC:
      return "problem with user-supplied function";
    case GSL_ERUNAWAY:
      return "iterative process is out of control";
    case GSL_EMAXITER:
      return "exceeded max number of iterations" ;
    case GSL_EZERODIV:
      return "tried to divide by zero" ;
    case GSL_EBADTOL:
      return "specified tolerance is invalid or theoretically unattainable" ;
    case GSL_ETOL:
      return "failed to reach the specified tolerance" ;
    case GSL_EUNDRFLW:
      return "underflow" ;
    case GSL_EOVRFLW:
      return "overflow" ;
    case GSL_ELOSS:
      return "loss of accuracy" ;
    case GSL_EROUND:
      return "roundoff error" ;
    case GSL_EBADLEN:
      return "matrix/vector sizes are not conformant" ;
    case GSL_ENOTSQR:
      return "matrix not square" ;
    case GSL_ESING:
      return "singularity or extremely bad function behavior detected" ;
    case GSL_EDIVERGE:
      return "integral or series is divergent" ;
    case GSL_EUNSUP:
      return "the required feature is not supported by this hardware platform";
    case GSL_EUNIMPL:
      return "the requested feature is not (yet) implemented";
    case GSL_ECACHE:
      return "cache limit exceeded";
    case GSL_ETABLE:
      return "table limit exceeded";
    case GSL_ENOPROG:
      return "iteration is not making progress towards solution";
    case GSL_ENOPROGJ:
      return "jacobian evaluations are not improving the solution";
    case GSL_ETOLF:
      return "cannot reach the specified tolerance in F";
    case GSL_ETOLX:
      return "cannot reach the specified tolerance in X";
    case GSL_ETOLG:
      return "cannot reach the specified tolerance in gradient";
    case GSL_EOF:
      return "end of file";
    default:
      return "unknown error code" ;
    }
}
