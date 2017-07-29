#ifndef _tensor_h_
#define _tensor_h_
/* tensor.h
 *
 * Copyright (C) 2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "NUM.h"

/*
	Initialization (tested in praat.cpp):
		numvec x;                  // does not initialize x
		numvec x { };              // initializes x.at to nullptr and x.size to 0
		numvec x { 100, false };   // initializes x to 100 uninitialized values
		numvec x { 100, true };    // initializes x to 100 zeroes
		NUMvector<double> a [100];
		numvec x { a, 100 };       // initializes x to 100 values from a base-1 array

		autonumvec y;                  // initializes y.at to nullptr and y.size to 0
		autonumvec y { 100, false };   // initializes y to 100 uninitialized values, having ownership
		autonumvec y { 100, true };    // initializes y to 100 zeroes, having ownership
		autonumvec y { x };            // initializes y to the content of x, taking ownership (explicit, so not "y = x")
		numvec z = y.transfer();       // releases ownership, x.at becoming nullptr
		}                              // end of scope destroys x.at if not nullptr

	To return a numvec from a function, transfer ownership like this:
		numvec foo () {
			autonumvec x { 100, false };
			... // fill in the 100 values
			return x.transfer();
		}
*/

struct autonumvec;   // forward declaration, needed in the declaration of numvec

struct numvec {
	double *at;
	long size;
	numvec () = default;   // for use in a union
	numvec (double *givenAt, long givenSize): at (givenAt), size (givenSize) { }
	numvec (long givenSize, bool zero) {
		our _initAt (givenSize, zero);
		our size = givenSize;
	}
	numvec (const numvec& other) = default;
	numvec (const autonumvec& other) = delete;
	numvec& operator= (const numvec&) = default;
	numvec& operator= (const autonumvec&) = delete;
	double& operator[] (long i) {
		return our at [i];
	}
	void reset () noexcept {
		if (our at) {
			our _freeAt ();
			our at = nullptr;
		}
		our size = 0;
	}
protected:
	void _initAt (long givenSize, bool zero);
	void _freeAt () noexcept;
};

#define empty_numvec  numvec { nullptr, 0 }

struct autonumvec: numvec {
	autonumvec (): numvec (nullptr, 0) { }
	autonumvec (long givenSize, bool zero): numvec (givenSize, zero) { }
	explicit autonumvec (numvec x): numvec (x.at, x.size) { }   // explicit because unusual
	autonumvec (const autonumvec&) = delete;   // disable copy constructor...
	autonumvec (autonumvec&& other) noexcept : numvec { other.get() } {   // ...and enable move constructor
		other.at = nullptr;   // disown source
	}
	~autonumvec () {
		if (our at) our _freeAt ();
	}
	autonumvec& operator= (const autonumvec&) = delete;   // disable copy assignment...
	autonumvec& operator= (autonumvec&& other) noexcept {   // ...and enable move assignment
		if (other.at != our at) {
			if (our at) our _freeAt ();
			our at = other.at;
			other.at = nullptr;   // disown source
			our size = other.size;
		}
		return *this;
	}
	autonumvec&& move () noexcept { return static_cast <autonumvec&&> (*this); }
	numvec get () { return { our at, our size }; }
	numvec releaseToAmbiguousOwner () {
		double *oldAt = our at;
		our at = nullptr;
		return { oldAt, our size };
	}
	void reset (long newSize, bool zero) {
		numvec :: reset ();   // exception guarantee: leave this in a reasonable state...
		our _initAt (newSize, zero);   // ...in case this throws
		our size = newSize;
	}
};

struct autonummat;   // forward declaration, needed in the declaration of nummat

struct nummat {
	double **at;
	long nrow, ncol;
	nummat () = default;   // for use in a union
	nummat (double **givenAt, long givenNrow, long givenNcol): at (givenAt), nrow (givenNrow), ncol (givenNcol) { }
	nummat (long givenNrow, long givenNcol, bool zero) {
		our _initAt (givenNrow, givenNcol, zero);
		our nrow = givenNrow;
		our ncol = givenNcol;
	}
	nummat (const nummat& other) = default;
	nummat (const autonummat& other) = delete;
	nummat& operator= (const nummat&) = default;
	nummat& operator= (const autonummat&) = delete;
	double *& operator[] (long i) {
		return our at [i];
	}
	void reset () noexcept {
		if (our at) {
			our _freeAt ();
			our at = nullptr;
		}
		our nrow = 0;
		our ncol = 0;
	}
protected:
	void _initAt (long givenNrow, long givenNcol, bool zero);
	void _freeAt () noexcept;
};

#define empty_nummat  nummat { nullptr, 0, 0 }

struct autonummat : nummat {
	autonummat () : nummat { nullptr, 0, 0 } { }
	autonummat (double **givenAt, long givenNrow, long givenNcol): nummat (givenAt, givenNrow, givenNcol) { }
	autonummat (long givenNrow, long givenNcol, bool zero): nummat { givenNrow, givenNcol, zero } { }
	explicit autonummat (nummat x): nummat (x.at, x.nrow, x.ncol) { }   // explicit because unusual
	autonummat (const autonummat&) = delete;   // disable copy constructor...
	autonummat (autonummat&& other) noexcept : nummat { other.get() } {   // ...and enable move constructor
		other.at = nullptr;   // disown source
	}
	~autonummat () {
		if (our at) our _freeAt ();
	}
	autonummat& operator= (const autonummat&) = delete;   // disable copy assignment...
	autonummat& operator= (autonummat&& other) noexcept {   // ...and enable move assignment
		if (other.at != our at) {
			if (our at) our _freeAt ();
			our at = other.at;
			other.at = nullptr;   // disown source
			our nrow = other.nrow;
			our ncol = other.ncol;
		}
		return *this;
	}
	autonummat&& move () noexcept { return static_cast <autonummat&&> (*this); }
	nummat get () { return { our at, our nrow, our ncol }; }
	nummat releaseToAmbiguousOwner () {
		double **oldAt = our at;
		our at = nullptr;
		return { oldAt, our nrow, our ncol };
	}
	void reset (long newNrow, long newNcol, bool zero) {
		nummat :: reset ();
		our _initAt (newNrow, newNcol, zero);
		our nrow = newNrow;
		our ncol = newNcol;
	}
};

inline static double sqrt_scalar (double x) {
	#if defined (_WIN32)
		if (x < 0.0) return NUMundefined;
	#endif
	return sqrt (x);
}

inline static double sum_scalar (numvec x) {
	double sum = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		sum += x [i];
	}
	return sum;
}

inline static double mean_scalar (numvec x) {
	if (x.size == 0) return NUMundefined;
	double sum = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		sum += x [i];
	}
	return sum / x.size;
}

double stdev_scalar (numvec x) noexcept;
double center_scalar (numvec x) noexcept;

inline static double inner_scalar (numvec x, numvec y) {
	if (x.size != y.size) return NUMundefined;
	double result = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		result += x [i] * y [i];
	}
	return result;
}

autonumvec copy_numvec (numvec x);

inline static autonumvec add_numvec (numvec x, numvec y) {
	//if (x.size != y.size) return autonumvec { nullptr, 0, 0 };
	autonumvec result (x.size, false);
	for (long i = 1; i <= x.size; i ++) {
		result [i] = x [i] + y [i];
	}
	return result;
}

autonummat copy_nummat (nummat x);
autonummat outer_nummat (numvec x, numvec y);

/* End of file tensor.h */
#endif
