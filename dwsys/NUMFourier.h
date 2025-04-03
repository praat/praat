#ifndef _NUMFourier_h_
#define _NUMFourier_h_
/* NUMFourier.h
 *
 * Copyright (C) 2025 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "melder.h"
#include "Data.h"

#include "NUMFourierTable_def.h"

autoNUMFourierTable NUMFourierTable_create (integer n);

void NUMfft_forward (NUMFourierTable table, VEC data);
/*
	Function:
		Calculates the Fourier Transform of a set of n real-valued data points.
		Replaces this data in array data [1...n] by the positive frequency half
		of its complex Fourier Transform, with a minus sign in the exponent.
	Preconditions:
		data != NULL;
		table must have been created with NUMFourierTable_create
	Postconditions:
		data [1] contains real valued first component (Direct Current)
		data [2..n-1] even index : real part; odd index: imaginary part of DFT.
		data [n] contains real valued last component (Nyquist frequency)

	Output parameters:

	data  r(1) = the sum from i=1 to i=n of r(i)

		If l =(int) (n+1)/2

		then for k = 2,...,l

			r(2*k-2) = the sum from i = 1 to i = n of r(i)*cos((k-1)*(i-1)*2*pi/n)

			r(2*k-1) = the sum from i = 1 to i = n of -r(i)*sin((k-1)*(i-1)*2*pi/n)

		if n is even

			 r(n) = the sum from i = 1 to i = n of (-1)**(i-1)*r(i)

		i.e., the ordering of the output array will be for n even
			r(1),(r(2),i(2)),(r(3),i(3)),...,(r(l-1),i(l-1)),r(l).
		Or ...., (r(l),i(l)) for n uneven.

 *****  note
	this transform is unnormalized since a call of NUMfft_forward
	followed by a call of NUMfft_backward will multiply the input sequence by n.
*/

void NUMfft_backward (NUMFourierTable table, VEC data);
/*
	Function:
		Calculates the inverse transform of a complex array if it is the transform of real data.
		(Result in this case should be multiplied by 1/n.)
	Preconditions:
		n is an integer power of 2.
		data [1] contains real valued first component (Direct Current)
		data [2..n-1] even index : real part; odd index: imaginary part of DFT.
		data [n] contains real valued last component (Nyquist frequency)

		table must have been created with NUMFourierTable_create

	Output parameters

	data	 for n even and for i = 1,...,n

			 r(i) = r(1)+(-1)**(i-1)*r(n)

				plus the sum from k=2 to k=n/2 of

				2.0*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n) -2.0*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)

		for n odd and for i = 1,...,n

			 r(i) = r(1) plus the sum from k=2 to k=(n+1)/2 of

				2.0*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n) -2.0*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)

 *****  note
	this transform is unnormalized since a call of NUMfft_forward
	followed by a call of NUMfft_backward will multiply the input
	sequence by n.
*/


/**** Compatibility with NR fft's */

void NUMforwardRealFastFourierTransform (VEC data);
/*
	Function:
		Calculates the Fourier Transform of a set of n real-valued data points.
		Replaces this data in array data [1...n] by the positive frequency half
		of its complex Fourier Transform, with a minus sign in the exponent.
	Preconditions:
		n is an integer power of 2.
		data != NULL;
	Postconditions:
		data [1] contains real valued first component (Direct Current)
		data [2] contains real valued last component (Nyquist frequency)
		data [3..n] odd index : real part; even index: imaginary part of DFT.
*/

void NUMreverseRealFastFourierTransform (VEC data);
/*
	Function:
		Calculates the inverse transform of a complex array if it is the transform of real data.
		(Result in this case should be multiplied by 1/n.)
	Preconditions:
		n is an integer power of 2.
		data [1] contains real valued first component (Direct Current)
		data [2] contains real valued last component (Nyquist frequency)
		data [3..n] odd index : real part; even index: imaginary part of DFT.
*/

void NUMrealft (VEC data, integer direction);

#endif /* _NUMFourier_h_ */
