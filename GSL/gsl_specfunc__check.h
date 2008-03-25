/* check for underflow */

#define CHECK_UNDERFLOW(r) if (fabs((r)->val) < GSL_DBL_MIN) GSL_ERROR("underflow", GSL_EUNDRFLW);
