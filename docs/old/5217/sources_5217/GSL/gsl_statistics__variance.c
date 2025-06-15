#include "gsl__config.h"
#include <math.h>
#include "gsl_statistics.h"

#define BASE_LONG_DOUBLE
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_LONG_DOUBLE

#define BASE_DOUBLE
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_DOUBLE

#define BASE_FLOAT
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_FLOAT

#define BASE_ULONG
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_ULONG

#define BASE_LONG
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_LONG

#define BASE_UINT
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_UINT

#define BASE_INT
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_INT

#define BASE_USHORT
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_USHORT

#define BASE_SHORT
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_SHORT

#define BASE_UCHAR
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_UCHAR

#define BASE_CHAR
#include "templates_on.h"
#include "gsl_statistics__variance_source.c"
#include "templates_off.h"
#undef  BASE_CHAR



