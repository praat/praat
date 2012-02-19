/* CDF_ERROR: call the error handler, and return a NAN. */

#define CDF_ERROR(reason, gsl_errno) GSL_ERROR_VAL(reason, gsl_errno, GSL_NAN)

