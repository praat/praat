/* evaluate a function discarding the status value in a modifiable way */

#define EVAL_RESULT(fn) \
   gsl_sf_result result; \
   int status = fn; \
   if (status != GSL_SUCCESS) { \
     GSL_ERROR_VAL(#fn, status, result.val); \
   } ; \
   return result.val;

#define EVAL_DOUBLE(fn) \
   int status = fn; \
   if (status != GSL_SUCCESS) { \
     GSL_ERROR_VAL(#fn, status, result); \
   } ; \
   return result;

