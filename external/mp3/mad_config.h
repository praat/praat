/* config.h. */
/* Edited by Erez Volk for Praat. */
/* Luckily we assume C99, which makes life easier. */

/* Compile with Intel implementation for Win32, otherwise 64-bit */
#if defined (_WIN32) && defined (_MSC_VER) || defined (__GNUC__) && defined (__i386__) && ! defined (macintosh)
#  define FPM_INTEL
#else /* !_WIN32 */
#  define FPM_64BIT
#endif /* _WIN32 */

/* Define to enable diagnostic debugging support. */
/* #undef DEBUG */

/* Define to enable experimental code. */
/* #undef EXPERIMENTAL */

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the <errno.h> header file. */
/* #undef HAVE_ERRNO_H */

/* Define to 1 if you have the `fcntl' function. */
/* #undef HAVE_FCNTL */

/* Define to 1 if you have the <fcntl.h> header file. */
/* #undef HAVE_FCNTL_H */

/* Define to 1 if you have the `fork' function. */
/* #undef HAVE_FORK */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if your MIPS CPU supports a 2-operand MADD16 instruction. */
/* #undef HAVE_MADD16_ASM */

/* Define if your MIPS CPU supports a 2-operand MADD instruction. */
/* #undef HAVE_MADD_ASM */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `pipe' function. */
/* #undef HAVE_PIPE */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
/* #undef HAVE_SYS_WAIT_H */

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Define to 1 if you have the `waitpid' function. */
/* #undef HAVE_WAITPID */

/* Define to disable debugging assertions. */
/* #undef NDEBUG */

/* Define to optimize for accuracy over speed. */
/* #undef OPT_ACCURACY */

/* Define to optimize for speed over accuracy. */
/* #undef OPT_SPEED */

/* Define to enable a fast subband synthesis approximation optimization. */
/* #undef OPT_SSO */

/* Define to influence a strict interpretation of the ISO/IEC standards, even
   if this is in opposition with best accepted practices. */
/* #undef OPT_STRICT */

/* Name of package */
#define PACKAGE "libmad"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "support@underbit.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "MPEG Audio Decoder"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "MPEG Audio Decoder 0.15.1b"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libmad"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.15.1b"

/* Erez Volk: Let's hope these sizes are correct. */
/*            I've added a runtime test for them just in case. */
/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of a `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of a `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.15.1b"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
#ifdef _MSC_VER
#  define inline __inline
#endif /* _MSC_VER */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */
