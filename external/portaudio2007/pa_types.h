#ifndef PA_TYPES_H
#define PA_TYPES_H

/* 
 * Portable Audio I/O Library
 * integer type definitions
 *
 * Based on the Open Source API proposed by Ross Bencina
 * Copyright (c) 1999-2006 Ross Bencina, Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

/** @file
 @ingroup common_src

 @brief Definition of 16 and 32 bit integer types (PaInt16, PaInt32 etc)

 SIZEOF_SHORT, SIZEOF_INT and SIZEOF_LONG are set by the configure script
 when it is used. Otherwise we default to the common 32 bit values, if your
 platform doesn't use configure, and doesn't use the default values below
 you will need to explicitly define these symbols in your make file.

 A PA_VALIDATE_SIZES macro is provided to assert that the values set in this
 file are correct.

 Paul Boersma 2013: the sizes should not be handcoded if this header file is to be included in both 32-bit and 64-bit applications;
 instead, we use int32_t and the like from <stdint.h>.
*/

#include <stdint.h>

typedef int16_t PaInt16;
typedef uint16_t PaUint16;

typedef int32_t PaInt32;
typedef uint32_t PaUint32;


/* PA_VALIDATE_TYPE_SIZES compares the size of the integer types at runtime to
 ensure that PortAudio was configured correctly, and raises an assertion if
 they don't match the expected values. <assert.h> must be included in the
 context in which this macro is used.
*/
#define PA_VALIDATE_TYPE_SIZES \
    { \
        assert( "PortAudio: type sizes are not correct in pa_types.h" && sizeof( PaUint16 ) == 2 ); \
        assert( "PortAudio: type sizes are not correct in pa_types.h" && sizeof( PaInt16 ) == 2 ); \
        assert( "PortAudio: type sizes are not correct in pa_types.h" && sizeof( PaUint32 ) == 4 ); \
        assert( "PortAudio: type sizes are not correct in pa_types.h" && sizeof( PaInt32 ) == 4 ); \
    }


#endif /* PA_TYPES_H */
