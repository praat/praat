/* vector/vector.c
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
#include "gsl_vector.h"

/* turn off range checking at runtime if zero */
int gsl_check_range = 1;

#define BASE_GSL_COMPLEX_LONG
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_GSL_COMPLEX_LONG

#define BASE_GSL_COMPLEX
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_GSL_COMPLEX

#define BASE_GSL_COMPLEX_FLOAT
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_GSL_COMPLEX_FLOAT

#define BASE_LONG_DOUBLE
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_LONG_DOUBLE

#define BASE_DOUBLE
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_DOUBLE

#define BASE_FLOAT
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_FLOAT

#define BASE_ULONG
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_ULONG

#define BASE_LONG
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_LONG

#define BASE_UINT
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_UINT

#define BASE_INT
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_INT

#define BASE_USHORT
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_USHORT

#define BASE_SHORT
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_SHORT

#define BASE_UCHAR
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_UCHAR

#define BASE_CHAR
#include "templates_on.h"
#include "gsl_vector__vector_source.c"
#include "templates_off.h"
#undef  BASE_CHAR
