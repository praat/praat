#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_vector.h"

#include "gsl_vector__view.h"

#define BASE_GSL_COMPLEX_LONG
#include "templates_on.h"
#include "gsl_vector__reim_source.c"
#include "templates_off.h"
#undef  BASE_GSL_COMPLEX_LONG

#define BASE_GSL_COMPLEX
#include "templates_on.h"
#include "gsl_vector__reim_source.c"
#include "templates_off.h"
#undef  BASE_GSL_COMPLEX

#define BASE_GSL_COMPLEX_FLOAT
#include "templates_on.h"
#include "gsl_vector__reim_source.c"
#include "templates_off.h"
#undef  BASE_GSL_COMPLEX_FLOAT

#define USE_QUALIFIER
#define QUALIFIER const

#define BASE_GSL_COMPLEX_LONG
#include "templates_on.h"
#include "gsl_vector__reim_source.c"
#include "templates_off.h"
#undef  BASE_GSL_COMPLEX_LONG

#define BASE_GSL_COMPLEX
#include "templates_on.h"
#include "gsl_vector__reim_source.c"
#include "templates_off.h"
#undef  BASE_GSL_COMPLEX

#define BASE_GSL_COMPLEX_FLOAT
#include "templates_on.h"
#include "gsl_vector__reim_source.c"
#include "templates_off.h"
#undef  BASE_GSL_COMPLEX_FLOAT
