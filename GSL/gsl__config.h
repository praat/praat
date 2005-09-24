/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you have the acosh function.  */
#define HAVE_ACOSH 1

/* Define if you have the asinh function.  */
#define HAVE_ASINH 1

/* Define if you have the atanh function.  */
#define HAVE_ATANH 1

/* Define if you have the clock function.  */
#define HAVE_CLOCK 1

/* Define if you have the expm1 function.  */
#if defined(linux)
  #define HAVE_EXPM1 1
#else
  #undef HAVE_EXPM1
#endif

/* Define if you have the finite function.  */
#if defined(linux)
  #define HAVE_FINITE 1
#else
  #undef HAVE_FINITE
#endif

/* Define if you have the frexp function.  */
#define HAVE_FREXP 1

/* Define if you have the hypot function.  */
#if defined(linux)
  #define HAVE_HYPOT 1
#else
  #undef HAVE_HYPOT
#endif

/* Define if you have the isfinite function.  */
/* #undef HAVE_ISFINITE */

/* Define if you have the isinf function.  */
#if defined(linux) || defined (macintosh) || defined (_WIN32)
  #define HAVE_ISINF 1
#else
  #undef HAVE_ISINF
#endif

/* Define if you have the isnan function.  */
#if defined(linux) || defined (macintosh) || defined (_WIN32)
  #define HAVE_ISNAN 1
#else
  #undef HAVE_ISNAN
#endif

/* Define if you have the ldexp function.  */
#define HAVE_LDEXP 1

/* Define if you have the log1p function.  */
#if defined(linux)
  #define HAVE_LOG1P 1
#else
  #undef HAVE_LOG1P
#endif

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the strdup function.  */
#if defined(linux)
  #define HAVE_STRDUP 1
#else
  #undef HAVE_STRDUP
#endif

/* Define if you have the strtol function.  */
#if defined(linux)
  #define HAVE_STRTOL 1
#else
  #undef HAVE_STRTOL
#endif

/* Define if you have the strtoul function.  */
#if defined(linux)
  #define HAVE_STRTOUL 1
#else
  #undef HAVE_STRTOUL
#endif

/* Define if you have the <dlfcn.h> header file.  */
#if defined(linux)
  #define HAVE_DLFCN_H 1
#else
  #undef HAVE_DLFCN_H
#endif

/* Name of package */
#define PACKAGE "gsl"

/* Version number of package */
#define VERSION "1.3"

/* Define if you have inline */
#undef HAVE_INLINE
#ifdef sgi
   #define inline
#endif


/* Define if you need to hide the static definitions of inline functions */
/* #undef HIDE_INLINE_STATIC */

/* Define if you have the ansi CLOCKS_PER_SEC clock rate */
#define HAVE_CLOCKS_PER_SEC 1

/* Defined if configure has guessed a missing ansi CLOCKS_PER_SEC clock rate */
/* #undef HAVE_GUESSED_CLOCKS_PER_SEC */

/* Use configure's best guess for CLOCKS_PER_SEC if it is unknown */
#ifndef HAVE_CLOCKS_PER_SEC
#define CLOCKS_PER_SEC HAVE_GUESSED_CLOCKS_PER_SEC
#endif

/* Defined if you have ansi EXIT_SUCCESS and EXIT_FAILURE in stdlib.h */
#define HAVE_EXIT_SUCCESS_AND_FAILURE 1

/* Use 0 and 1 for EXIT_SUCCESS and EXIT_FAILURE if we don't have them */
#ifndef HAVE_EXIT_SUCCESS_AND_FAILURE
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

/* Define one of these if you have a known IEEE arithmetic interface */
/* #undef HAVE_SPARCLINUX_IEEE_INTERFACE */
/* #undef HAVE_M68KLINUX_IEEE_INTERFACE */
/* #undef HAVE_PPCLINUX_IEEE_INTERFACE */
#define HAVE_X86LINUX_IEEE_INTERFACE 1
/* #undef HAVE_SUNOS4_IEEE_INTERFACE */
/* #undef HAVE_SOLARIS_IEEE_INTERFACE */
/* #undef HAVE_HPUX11_IEEE_INTERFACE */
/* #undef HAVE_HPUX_IEEE_INTERFACE */
/* #undef HAVE_TRU64_IEEE_INTERFACE */
/* #undef HAVE_IRIX_IEEE_INTERFACE */
/* #undef HAVE_AIX_IEEE_INTERFACE */
/* #undef HAVE_FREEBSD_IEEE_INTERFACE */
/* #undef HAVE_OS2EMX_IEEE_INTERFACE */
/* #undef HAVE_NETBSD_IEEE_INTERFACE */
/* #undef HAVE_OPENBSD_IEEE_INTERFACE */
/* #undef HAVE_DARWIN_IEEE_INTERFACE */

/* Define this if we need to include /usr/include/float.h explicitly
   in order to get FP_RND_RN and related macros.  This is known to be
   a problem on some Compaq Tru64 unix systems when compiled with GCC. */
/* #undef FIND_FP_RND_IN_USR_INCLUDE_FLOAT_H */

/* Define a rounding function which moves extended precision values
   out of registers and rounds them to double-precision. This should
   be used *sparingly*, in places where it is necessary to keep
   double-precision rounding for critical expressions while running in
   extended precision. For example, the following code should ensure
   exact equality, even when extended precision registers are in use,

      double q = GSL_COERCE_DBL(3.0/7.0) ;
      if (q == GSL_COERCE_DBL(3.0/7.0)) { ... } ;

   It carries a penalty even when the program is running in double
   precision mode unless you compile a separate version of the
   library with HAVE_EXTENDED_PRECISION_REGISTERS turned off. */

#define HAVE_EXTENDED_PRECISION_REGISTERS 1

#ifdef HAVE_EXTENDED_PRECISION_REGISTERS
#define GSL_COERCE_DBL(x) (gsl_coerce_double(x))
#else
#define GSL_COERCE_DBL(x) (x)
#endif

/* Define this if printf can handle %Lf for long double */
#define HAVE_PRINTF_LONGDOUBLE 1

/* Define this is IEEE comparisons work correctly (e.g. NaN != NaN) */
#define HAVE_IEEE_COMPARISONS 1

/* Define this is IEEE denormalized numbers are available */
#define HAVE_IEEE_DENORMALS 1

/* Substitute gsl functions for missing system functions */

#ifndef HAVE_HYPOT
#define hypot gsl_hypot
#endif

#ifndef HAVE_LOG1P
#define log1p gsl_log1p
#endif

#ifndef HAVE_EXPM1
#define expm1 gsl_expm1
#endif

#ifndef HAVE_ACOSH
#define acosh gsl_acosh
#endif

#ifndef HAVE_ASINH
#define asinh gsl_asinh
#endif

#ifndef HAVE_ATANH
#define atanh gsl_atanh
#endif

#ifndef HAVE_ISINF
#define isinf gsl_isinf
#endif

#ifndef HAVE_ISNAN
#define isnan gsl_isnan
#endif

#ifndef HAVE_LDEXP
#define ldexp gsl_ldexp
#endif

#ifndef HAVE_FREXP
#define frexp gsl_frexp
#endif


#ifndef HAVE_FINITE
#ifdef HAVE_ISFINITE
#define finite isfinite
#else
#define finite gsl_finite
#endif
#endif

#ifdef __GNUC__
#define DISCARD_POINTER(p) do { ; } while(p ? 0 : 0);
#else
#define DISCARD_POINTER(p) /* ignoring discarded pointer */
#endif

#ifndef GSL_RANGE_CHECK_ON
#define GSL_RANGE_CHECK_OFF  /* turn off range checking by default */
#endif
