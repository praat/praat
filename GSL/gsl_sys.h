/* sys/gsl_sys.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman, Brian Gough
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __GSL_SYS_H__
#define __GSL_SYS_H__



double gsl_log1p (const double x);
double gsl_expm1 (const double x);
double gsl_hypot (const double x, const double y);
double gsl_acosh (const double x);
double gsl_asinh (const double x);
double gsl_atanh (const double x);

int gsl_isnan (const double x);
int gsl_isinf (const double x);
int gsl_finite (const double x);

double gsl_nan (void);
double gsl_posinf (void);
double gsl_neginf (void);
double gsl_fdiv (const double x, const double y);

double gsl_coerce_double (const double x);
float gsl_coerce_float (const float x);
long double gsl_coerce_long_double (const long double x);

double gsl_ldexp(const double x, const int e);
double gsl_frexp(const double x, int * e);

int gsl_fcmp (const double x1, const double x2, const double epsilon);


#endif /* __GSL_SYS_H__ */
