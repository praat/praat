#include "gsl__config.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gsl_errno.h"
#include "gsl_complex.h"

#include "gsl_fft_complex.h"
#include "gsl_fft_complex_float.h"

#define BASE_DOUBLE
#include "templates_on.h"
#include "gsl_fft__bitreverse.c"
#include "templates_off.h"
#undef  BASE_DOUBLE

#define BASE_FLOAT
#include "templates_on.h"
#include "gsl_fft__bitreverse.c"
#include "templates_off.h"
#undef  BASE_FLOAT

#include "gsl_fft__factorize.c"

#define BASE_DOUBLE
#include "templates_on.h"
#include "gsl_fft__c_init.c"
#include "gsl_fft__c_main.c"
#include "gsl_fft__c_pass_2.c"
#include "gsl_fft__c_pass_3.c"
#include "gsl_fft__c_pass_4.c"
#include "gsl_fft__c_pass_5.c"
#include "gsl_fft__c_pass_6.c"
#include "gsl_fft__c_pass_7.c"
#include "gsl_fft__c_pass_n.c"
#include "gsl_fft__c_radix2.c"
#include "templates_off.h"
#undef  BASE_DOUBLE

#define BASE_FLOAT
#include "templates_on.h"
#include "gsl_fft__c_init.c"
#include "gsl_fft__c_main.c"
#include "gsl_fft__c_pass_2.c"
#include "gsl_fft__c_pass_3.c"
#include "gsl_fft__c_pass_4.c"
#include "gsl_fft__c_pass_5.c"
#include "gsl_fft__c_pass_6.c"
#include "gsl_fft__c_pass_7.c"
#include "gsl_fft__c_pass_n.c"
#include "gsl_fft__c_radix2.c"
#include "templates_off.h"
#undef  BASE_FLOAT

#include "gsl_fft_halfcomplex.h"
#include "gsl_fft_halfcomplex_float.h"

#define BASE_DOUBLE
#include "templates_on.h"
#include "gsl_fft__hc_init.c"
#include "gsl_fft__hc_main.c"
#include "gsl_fft__hc_pass_2.c"
#include "gsl_fft__hc_pass_3.c"
#include "gsl_fft__hc_pass_4.c"
#include "gsl_fft__hc_pass_5.c"
#include "gsl_fft__hc_pass_n.c"
#include "gsl_fft__hc_radix2.c"
#include "gsl_fft__hc_unpack.c"
#include "templates_off.h"
#undef  BASE_DOUBLE

#define BASE_FLOAT
#include "templates_on.h"
#include "gsl_fft__hc_init.c"
#include "gsl_fft__hc_main.c"
#include "gsl_fft__hc_pass_2.c"
#include "gsl_fft__hc_pass_3.c"
#include "gsl_fft__hc_pass_4.c"
#include "gsl_fft__hc_pass_5.c"
#include "gsl_fft__hc_pass_n.c"
#include "gsl_fft__hc_radix2.c"
#include "gsl_fft__hc_unpack.c"
#include "templates_off.h"
#undef  BASE_FLOAT

#include "gsl_fft_real.h"
#include "gsl_fft_real_float.h"

#define BASE_DOUBLE
#include "templates_on.h"
#include "gsl_fft__real_init.c"
#include "gsl_fft__real_main.c"
#include "gsl_fft__real_pass_2.c"
#include "gsl_fft__real_pass_3.c"
#include "gsl_fft__real_pass_4.c"
#include "gsl_fft__real_pass_5.c"
#include "gsl_fft__real_pass_n.c"
#include "gsl_fft__real_radix2.c"
#include "gsl_fft__real_unpack.c"
#include "templates_off.h"
#undef  BASE_DOUBLE

#define BASE_FLOAT
#include "templates_on.h"
#include "gsl_fft__real_init.c"
#include "gsl_fft__real_main.c"
#include "gsl_fft__real_pass_2.c"
#include "gsl_fft__real_pass_3.c"
#include "gsl_fft__real_pass_4.c"
#include "gsl_fft__real_pass_5.c"
#include "gsl_fft__real_pass_n.c"
#include "gsl_fft__real_radix2.c"
#include "gsl_fft__real_unpack.c"
#include "templates_off.h"
#undef  BASE_FLOAT
