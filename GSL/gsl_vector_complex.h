#ifndef __GSL_VECTOR_COMPLEX_H__
#define __GSL_VECTOR_COMPLEX_H__

#define  GSL_VECTOR_REAL(z, i)  ((z)->data[2*(i)*(z)->stride])
#define  GSL_VECTOR_IMAG(z, i)  ((z)->data[2*(i)*(z)->stride + 1])

#if GSL_RANGE_CHECK
#define GSL_VECTOR_COMPLEX(zv, i) (((i) >= (zv)->size ? (gsl_error ("index out of range", __FILE__, __LINE__, GSL_EINVAL), 0):0 , *GSL_COMPLEX_AT((zv),(i))))
#else
#define GSL_VECTOR_COMPLEX(zv, i) (GSL_COMPLEX_AT((zv),(i)))
#endif

#define GSL_COMPLEX_AT(zv,i) ((gsl_complex*)&((zv)->data[2*(i)*(zv)->stride]))
#define GSL_COMPLEX_FLOAT_AT(zv,i) ((gsl_complex_float*)&((zv)->data[2*(i)*(zv)->stride]))
#define GSL_COMPLEX_LONG_DOUBLE_AT(zv,i) ((gsl_complex_long_double*)&((zv)->data[2*(i)*(zv)->stride]))

#endif /* __GSL_VECTOR_COMPLEX_H__ */
