/* Handling of packed complex types... not meant for client consumption.
 */
#ifndef COMPLEX_INTERNAL_H_
#define COMPLEX_INTERNAL_H_

#define VECTOR(a,stride,i) ((a)[(stride)*(i)])
#define REAL(a,stride,i) ((a)[2*(stride)*(i)])
#define IMAG(a,stride,i) ((a)[2*(stride)*(i)+1])

#define REAL0(a) ((a)[0])
#define IMAG0(a) ((a)[1])


#endif /* !COMPLEX_INTERNAL_H_ */
