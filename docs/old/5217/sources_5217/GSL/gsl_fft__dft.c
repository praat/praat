#include "gsl__config.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include "gsl_errno.h"
#include "gsl_complex.h"

#include "gsl_dft_complex.h"
#include "gsl_dft_complex_float.h"

#include "gsl_fft__complex_internal.h"

#define BASE_DOUBLE
#include "templates_on.h"
#include "gsl_fft__dft_source.c"
#include "templates_off.h"
#undef  BASE_DOUBLE

#define BASE_FLOAT
#include "templates_on.h"
#include "gsl_fft__dft_source.c"
#include "templates_off.h"
#undef  BASE_FLOAT

