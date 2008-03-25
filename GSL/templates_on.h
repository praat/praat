/* If BASE is undefined we use function names like gsl_name()
   and assume that we are using doubles.

   If BASE is defined we used function names like gsl_BASE_name()
   and use BASE as the base datatype      */

#if   defined(BASE_GSL_COMPLEX_LONG)
#define BASE gsl_complex_long_double
#define SHORT complex_long_double
#define SHORT_REAL long_double
#define ATOMIC long double
#define USES_LONGDOUBLE 1
#define MULTIPLICITY 2
#define FP 1
#define IN_FORMAT "%Lg"
#define OUT_FORMAT "%Lg"
#define ATOMIC_IO ATOMIC
#define ZERO {{0.0L,0.0L}}
#define ONE {{1.0L,0.0L}}
#define BASE_EPSILON GSL_DBL_EPSILON

#elif defined(BASE_GSL_COMPLEX)
#define BASE gsl_complex
#define SHORT complex
#define SHORT_REAL
#define ATOMIC double
#define MULTIPLICITY 2
#define FP 1
#define IN_FORMAT "%lg"
#define OUT_FORMAT "%g"
#define ATOMIC_IO ATOMIC
#define ZERO {{0.0,0.0}}
#define ONE {{1.0,0.0}}
#define BASE_EPSILON GSL_DBL_EPSILON

#elif defined(BASE_GSL_COMPLEX_FLOAT)
#define BASE gsl_complex_float
#define SHORT complex_float
#define SHORT_REAL float
#define ATOMIC float
#define MULTIPLICITY 2
#define FP 1
#define IN_FORMAT "%g"
#define OUT_FORMAT "%g"
#define ATOMIC_IO ATOMIC
#define ZERO {{0.0F,0.0F}}
#define ONE {{1.0F,0.0F}}
#define BASE_EPSILON GSL_FLT_EPSILON

#elif defined(BASE_LONG_DOUBLE)
#define BASE long double
#define SHORT long_double
#define ATOMIC long double
#define USES_LONGDOUBLE 1
#define MULTIPLICITY 1
#define FP 1
#define IN_FORMAT "%Lg"
#define OUT_FORMAT "%Lg"
#define ATOMIC_IO ATOMIC
#define ZERO 0.0L
#define ONE 1.0L
#define BASE_EPSILON GSL_DBL_EPSILON

#elif defined(BASE_DOUBLE)
#define BASE double
#define SHORT
#define ATOMIC double
#define MULTIPLICITY 1
#define FP 1
#define IN_FORMAT "%lg"
#define OUT_FORMAT "%g"
#define ATOMIC_IO ATOMIC
#define ZERO 0.0
#define ONE 1.0
#define BASE_EPSILON GSL_DBL_EPSILON

#elif defined(BASE_FLOAT)
#define BASE float
#define SHORT float
#define ATOMIC float
#define MULTIPLICITY 1
#define FP 1
#define IN_FORMAT "%g"
#define OUT_FORMAT "%g"
#define ATOMIC_IO ATOMIC
#define ZERO 0.0F
#define ONE 1.0F
#define BASE_EPSILON GSL_FLT_EPSILON

#elif defined(BASE_ULONG)
#define BASE unsigned long
#define SHORT ulong
#define ATOMIC unsigned long
#define MULTIPLICITY 1
#define IN_FORMAT "%lu"
#define OUT_FORMAT "%lu"
#define ATOMIC_IO ATOMIC
#define ZERO 0UL
#define ONE 1UL
#define UNSIGNED 1

#elif defined(BASE_LONG)
#define BASE long
#define SHORT long
#define ATOMIC long
#define MULTIPLICITY 1
#define IN_FORMAT "%ld"
#define OUT_FORMAT "%ld"
#define ATOMIC_IO ATOMIC
#define ZERO 0L
#define ONE 1L

#elif defined(BASE_UINT)
#define BASE unsigned int
#define SHORT uint
#define ATOMIC unsigned int
#define MULTIPLICITY 1
#define IN_FORMAT "%u"
#define OUT_FORMAT "%u"
#define ATOMIC_IO ATOMIC
#define ZERO 0U
#define ONE 1U
#define UNSIGNED 1

#elif defined(BASE_INT)
#define BASE int
#define SHORT int
#define ATOMIC int
#define MULTIPLICITY 1
#define IN_FORMAT "%d"
#define OUT_FORMAT "%d"
#define ATOMIC_IO ATOMIC
#define ZERO 0
#define ONE 1

#elif defined(BASE_USHORT)
#define BASE unsigned short
#define SHORT ushort
#define ATOMIC unsigned short
#define MULTIPLICITY 1
#define IN_FORMAT "%hu"
#define OUT_FORMAT "%hu"
#define ATOMIC_IO ATOMIC
#define ZERO 0U
#define ONE 1U
#define UNSIGNED 1

#elif defined(BASE_SHORT)
#define BASE short
#define SHORT short
#define ATOMIC short
#define MULTIPLICITY 1
#define IN_FORMAT "%hd"
#define OUT_FORMAT "%hd"
#define ATOMIC_IO ATOMIC
#define ZERO 0
#define ONE 1

#elif defined(BASE_UCHAR)
#define BASE unsigned char
#define SHORT uchar
#define ATOMIC unsigned char
#define MULTIPLICITY 1
#define IN_FORMAT "%u"
#define OUT_FORMAT "%u"
#define ATOMIC_IO unsigned int
#define ZERO 0U
#define ONE 1U
#define UNSIGNED 1

#elif defined(BASE_CHAR)
#define BASE char
#define SHORT char
#define ATOMIC char
#define MULTIPLICITY 1
#define IN_FORMAT "%d"
#define OUT_FORMAT "%d"
#define ATOMIC_IO int
#define ZERO 0
#define ONE 1
#ifdef __CHAR_UNSIGNED__
#define UNSIGNED 1
#endif

#else
#error unknown BASE_ directive in source.h
#endif

#define CONCAT2x(a,b) a ## _ ## b 
#define CONCAT2(a,b) CONCAT2x(a,b)
#define CONCAT3x(a,b,c) a ## _ ## b ## _ ## c
#define CONCAT3(a,b,c) CONCAT3x(a,b,c)
#define CONCAT4x(a,b,c,d) a ## _ ## b ## _ ## c ## _ ## d
#define CONCAT4(a,b,c,d) CONCAT4x(a,b,c,d)

#ifndef USE_QUALIFIER
#define QUALIFIER
#endif

#ifdef USE_QUALIFIER
#if defined(BASE_DOUBLE)
#define FUNCTION(dir,name) CONCAT3(dir,QUALIFIER,name)
#define TYPE(dir) dir
#define VIEW(dir,name) CONCAT2(dir,name)
#define QUALIFIED_TYPE(dir) QUALIFIER dir
#define QUALIFIED_VIEW(dir,name) CONCAT3(dir,QUALIFIER,name)
#else
#define FUNCTION(a,c) CONCAT4(a,SHORT,QUALIFIER,c)
#define TYPE(dir) CONCAT2(dir,SHORT)
#define VIEW(dir,name) CONCAT3(dir,SHORT,name)
#define QUALIFIED_TYPE(dir) QUALIFIER CONCAT2(dir,SHORT)
#define QUALIFIED_VIEW(dir,name) CONCAT4(dir,SHORT,QUALIFIER,name)
#endif
#if defined(BASE_GSL_COMPLEX)
#define REAL_TYPE(dir) dir
#define REAL_VIEW(dir,name) CONCAT2(dir,name)
#define QUALIFIED_REAL_TYPE(dir) QUALIFIER dir
#define QUALIFIED_REAL_VIEW(dir,name) CONCAT3(dir,QUALIFIER,name)
#else
#define REAL_TYPE(dir) CONCAT2(dir,SHORT_REAL)
#define REAL_VIEW(dir,name) CONCAT3(dir,SHORT_REAL,name)
#define QUALIFIED_REAL_TYPE(dir) QUALIFIER CONCAT2(dir,SHORT_REAL)
#define QUALIFIED_REAL_VIEW(dir,name) CONCAT4(dir,SHORT_REAL,QUALIFIER,name)
#endif
#else
#if defined(BASE_DOUBLE)
#define FUNCTION(dir,name) CONCAT2(dir,name)
#define TYPE(dir) dir
#define VIEW(dir,name) CONCAT2(dir,name)
#define QUALIFIED_TYPE(dir) TYPE(dir)
#define QUALIFIED_VIEW(dir,name) CONCAT2(dir,name)
#else
#define FUNCTION(a,c) CONCAT3(a,SHORT,c)
#define TYPE(dir) CONCAT2(dir,SHORT)
#define VIEW(dir,name) CONCAT3(dir,SHORT,name)
#define QUALIFIED_TYPE(dir) TYPE(dir)
#define QUALIFIED_VIEW(dir,name) CONCAT3(dir,SHORT,name)
#endif
#if defined(BASE_GSL_COMPLEX)
#define REAL_TYPE(dir) dir
#define REAL_VIEW(dir,name) CONCAT2(dir,name)
#define QUALIFIED_REAL_TYPE(dir) dir
#define QUALIFIED_REAL_VIEW(dir,name) CONCAT2(dir,name)
#else
#define REAL_TYPE(dir) CONCAT2(dir,SHORT_REAL)
#define REAL_VIEW(dir,name) CONCAT3(dir,SHORT_REAL,name)
#define QUALIFIED_REAL_TYPE(dir) CONCAT2(dir,SHORT_REAL)
#define QUALIFIED_REAL_VIEW(dir,name) CONCAT3(dir,SHORT_REAL,name)
#endif
#endif

#define STRING(x) #x
#define EXPAND(x) STRING(x)
#define NAME(x) EXPAND(TYPE(x))
