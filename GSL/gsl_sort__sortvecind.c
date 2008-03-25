/*
 * Implement Heap sort -- direct and indirect sorting
 * Based on descriptions in Sedgewick "Algorithms in C"
 *
 * Copyright (C) 1999  Thomas Walter
 *
 * 18 February 2000: Modified for GSL by Brian Gough
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3, or (at your option) any
 * later version.
 *
 * This source is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "gsl__config.h"
#include "gsl_errno.h"
#include "gsl_vector.h"
#include "gsl_sort.h"
#include "gsl_sort_vector.h"

#define BASE_LONG_DOUBLE
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_LONG_DOUBLE

#define BASE_DOUBLE
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_DOUBLE

#define BASE_FLOAT
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_FLOAT

#define BASE_ULONG
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_ULONG

#define BASE_LONG
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_LONG

#define BASE_UINT
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_UINT

#define BASE_INT
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_INT

#define BASE_USHORT
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_USHORT

#define BASE_SHORT
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_SHORT

#define BASE_UCHAR
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_UCHAR

#define BASE_CHAR
#include "templates_on.h"
#include "gsl_sort__sortvecind_source.c"
#include "templates_off.h"
#undef  BASE_CHAR
