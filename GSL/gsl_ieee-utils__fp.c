#include "gsl__config.h"

#if HAVE_GNUSPARC_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-gnusparc.c"
#elif HAVE_GNUM68K_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-gnum68k.c"
#elif HAVE_GNUPPC_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-gnuppc.c"
#elif HAVE_GNUX86_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-gnux86.c"
#elif HAVE_HPUX11_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-hpux11.c"
#elif HAVE_HPUX_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-hpux.c"
#elif HAVE_SUNOS4_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-sunos4.c"
#elif HAVE_SOLARIS_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-solaris.c"
#elif HAVE_IRIX_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-irix.c"
#elif HAVE_AIX_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-aix.c"
#elif HAVE_TRU64_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-tru64.c"
#elif HAVE_FREEBSD_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-freebsd.c"
#elif HAVE_OS2EMX_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-os2emx.c"
#elif HAVE_NETBSD_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-netbsd.c"
#elif HAVE_OPENBSD_IEEE_INTERFACE
#include "gsl_ieee-utils__fp-openbsd.c"
/* Try to handle universal binaries */
#elif HAVE_DARWIN_IEEE_INTERFACE
# if defined(__i386__)
#  include "gsl_ieee-utils__fp-darwin86.c"
#else
#  include "gsl_ieee-utils__fp-darwin.c"
# endif
#elif HAVE_DARWIN86_IEEE_INTERFACE
# if defined(__ppc__)
#  include "gsl_ieee-utils__fp-darwin.c"
# else
#  include "gsl_ieee-utils__fp-darwin86.c"
#endif
#elif HAVE_DECL_FEENABLEEXCEPT || HAVE_DECL_FESETTRAPENABLE
#include "gsl_ieee-utils__fp-gnuc99.c"
#else
#include "gsl_ieee-utils__fp-unknown.c"
#endif
