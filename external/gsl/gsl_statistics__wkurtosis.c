#include "gsl__config.h"
#include <math.h>
#include "gsl_statistics.h"

#define BASE_LONG_DOUBLE
#include "templates_on.h"
#include "gsl_statistics__wkurtosis_source.c"
#include "templates_off.h"
#undef  BASE_LONG_DOUBLE

#define BASE_DOUBLE
#include "templates_on.h"
#include "gsl_statistics__wkurtosis_source.c"
#include "templates_off.h"
#undef  BASE_DOUBLE

#define BASE_FLOAT
#include "templates_on.h"
#include "gsl_statistics__wkurtosis_source.c"
#include "templates_off.h"
#undef  BASE_FLOAT
