/* tensor.cpp
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

#include "tensor.h"
#include "NUM2.h"

void numvec :: _initAt (integer givenSize, bool zero) {
	Melder_assert (givenSize >= 0);
	try {
		our at = ( givenSize == 0 ? nullptr : NUMvector <double> (1, givenSize, zero) );
	} catch (MelderError) {
		Melder_throw (U"Numeric vector not created.");
	}
}

void numvec :: _freeAt () noexcept {
	if (our at) NUMvector_free (our at, 1);
}

void nummat :: _initAt (integer givenNrow, integer givenNcol, bool zero) {
	Melder_assert (givenNrow >= 0);
	Melder_assert (givenNcol >= 0);
	try {
		our at = ( givenNrow > 0 && givenNcol > 0 ? NUMmatrix <double> (1, givenNrow, 1, givenNcol, zero) : nullptr );
	} catch (MelderError) {
		Melder_throw (U"Numeric matrix not created.");
	}
}

void nummat :: _freeAt () noexcept {
	if (our at) NUMmatrix_free (our at, 1, 1);
}

/*
	Recursive ("pairwise") addition preserves precision.
	Therefore, don't delete the parentheses!
*/

#define tensor_ADD_1  tensor_TERM (1)

#define tensor_ADD_2  tensor_TERM (1) + tensor_TERM (2)

#define tensor_ADD_3  tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)

// 4 = (2) + (2)
#define tensor_ADD_4  (tensor_TERM (1) + tensor_TERM (2)) + \
                      (tensor_TERM (3) + tensor_TERM (4))

// 5 = (3) + (2)
#define tensor_ADD_5  (tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                      (tensor_TERM (4) + tensor_TERM (5))

// 6 = (3) + (3)
#define tensor_ADD_6  (tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                      (tensor_TERM (4) + tensor_TERM (5) + tensor_TERM (6))

// 7 = (4) + (3) = ((2) + (2)) + (3)
#define tensor_ADD_7  ((tensor_TERM (1) + tensor_TERM (2)) + \
                       (tensor_TERM (3) + tensor_TERM (4))) + \
                      (tensor_TERM (5) + tensor_TERM (6) + tensor_TERM (7))

// 8 = (4) + (4) = ((2) + (2)) + ((2) + (2))
#define tensor_ADD_8  ((tensor_TERM (1) + tensor_TERM (2)) + \
                       (tensor_TERM (3) + tensor_TERM (4))) + \
                      ((tensor_TERM (5) + tensor_TERM (6)) + \
                       (tensor_TERM (7) + tensor_TERM (8)))

// 9 = (5) + (4) = ((3) + (2)) + ((2) + (2))
#define tensor_ADD_9  ((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                       (tensor_TERM (4) + tensor_TERM (5))) + \
                      ((tensor_TERM (6) + tensor_TERM (7)) + \
                       (tensor_TERM (8) + tensor_TERM (9)))

// 10 = (5) + (5) = ((3) + (2)) + ((3) + (2))
#define tensor_ADD_10  ((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                        (tensor_TERM (4) + tensor_TERM (5))) + \
                       ((tensor_TERM (6) + tensor_TERM (7) + tensor_TERM (8)) + \
                        (tensor_TERM (9) + tensor_TERM (10)))

// 11 = (6) + (5) = ((3) + (3)) + ((3) + (2))
#define tensor_ADD_11  ((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                        (tensor_TERM (4) + tensor_TERM (5) + tensor_TERM (6))) + \
                       ((tensor_TERM (7) + tensor_TERM (8) + tensor_TERM (9)) + \
                        (tensor_TERM (10) + tensor_TERM (11)))

// 12 = (6) + (6) = ((3) + (3)) + ((3) + (3))
#define tensor_ADD_12  ((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                        (tensor_TERM (4) + tensor_TERM (5) + tensor_TERM (6))) + \
                       ((tensor_TERM (7) + tensor_TERM (8) + tensor_TERM (9)) + \
                        (tensor_TERM (10) + tensor_TERM (11) + tensor_TERM (12)))

// 13 = (7) + (6) = (((2) + (2)) + (3)) + ((3) + (3))
#define tensor_ADD_13  (((tensor_TERM (1) + tensor_TERM (2)) + \
                         (tensor_TERM (3) + tensor_TERM (4))) + \
                        (tensor_TERM (5) + tensor_TERM (6) + tensor_TERM (7))) + \
                       ((tensor_TERM (8) + tensor_TERM (9) + tensor_TERM (10)) + \
                        (tensor_TERM (11) + tensor_TERM (12) + tensor_TERM (13)))

// 14 = (7) + (7) = (((2) + (2)) + (3)) + (((2) + (2)) + (3))
#define tensor_ADD_14  (((tensor_TERM (1) + tensor_TERM (2)) + \
                         (tensor_TERM (3) + tensor_TERM (4))) + \
                        (tensor_TERM (5) + tensor_TERM (6) + tensor_TERM (7))) + \
                       (((tensor_TERM (8) + tensor_TERM (9)) + \
                         (tensor_TERM (10) + tensor_TERM (11))) + \
                        (tensor_TERM (12) + tensor_TERM (13) + tensor_TERM (14)))

// 15 = (8) + (7) = (((2) + (2)) + ((2) + (2))) + (((2) + (2)) + (3))
#define tensor_ADD_15  (((tensor_TERM (1) + tensor_TERM (2)) + \
                         (tensor_TERM (3) + tensor_TERM (4))) + \
                        ((tensor_TERM (5) + tensor_TERM (6)) + \
                         (tensor_TERM (7) + tensor_TERM (8)))) + \
                       (((tensor_TERM (9) + tensor_TERM (10)) + \
                         (tensor_TERM (11) + tensor_TERM (12))) + \
                        (tensor_TERM (13) + tensor_TERM (14) + tensor_TERM (15)))

// 16 = (8) + (8) = (((2) + (2)) + ((2) + (2))) + (((2) + (2)) + ((2) + (2)))
#define tensor_ADD_16  (((tensor_TERM (1) + tensor_TERM (2)) + \
                         (tensor_TERM (3) + tensor_TERM (4))) + \
                        ((tensor_TERM (5) + tensor_TERM (6)) + \
                         (tensor_TERM (7) + tensor_TERM (8)))) + \
                       (((tensor_TERM (9) + tensor_TERM (10)) + \
                         (tensor_TERM (11) + tensor_TERM (12))) + \
                        ((tensor_TERM (13) + tensor_TERM (14)) + \
                         (tensor_TERM (15) + tensor_TERM (16))))

// 17 = (9) + (8) = (((3) + (2)) + ((2) + (2))) + (((2) + (2)) + ((2) + (2)))
#define tensor_ADD_17  (((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                         (tensor_TERM (4) + tensor_TERM (5))) + \
                        ((tensor_TERM (6) + tensor_TERM (7)) + \
                         (tensor_TERM (8) + tensor_TERM (9)))) + \
                       (((tensor_TERM (10) + tensor_TERM (11)) + \
                         (tensor_TERM (12) + tensor_TERM (13))) + \
                        ((tensor_TERM (14) + tensor_TERM (15)) + \
                         (tensor_TERM (16) + tensor_TERM (17))))

// 18 = (9) + (9) = (((3) + (2)) + ((2) + (2))) + (((3) + (2)) + ((2) + (2)))
#define tensor_ADD_18  (((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                         (tensor_TERM (4) + tensor_TERM (5))) + \
                        ((tensor_TERM (6) + tensor_TERM (7)) + \
                         (tensor_TERM (8) + tensor_TERM (9)))) + \
                       (((tensor_TERM (10) + tensor_TERM (11) + tensor_TERM (12)) + \
                         (tensor_TERM (13) + tensor_TERM (14))) + \
                        ((tensor_TERM (15) + tensor_TERM (16)) + \
                         (tensor_TERM (17) + tensor_TERM (18))))

// 19 = (10) + (9) = (((3) + (2)) + ((3) + (2))) + (((3) + (2)) + ((2) + (2)))
#define tensor_ADD_19  (((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                         (tensor_TERM (4) + tensor_TERM (5))) + \
                        ((tensor_TERM (6) + tensor_TERM (7) + tensor_TERM (8)) + \
                         (tensor_TERM (9) + tensor_TERM (10)))) + \
                       (((tensor_TERM (11) + tensor_TERM (12) + tensor_TERM (13)) + \
                         (tensor_TERM (14) + tensor_TERM (15))) + \
                        ((tensor_TERM (16) + tensor_TERM (17)) + \
                         (tensor_TERM (18) + tensor_TERM (19))))

// 20 = (10) + (10) = (((3) + (2)) + ((3) + (2))) + (((3) + (2)) + ((3) + (2)))
#define tensor_ADD_20  (((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                         (tensor_TERM (4) + tensor_TERM (5))) + \
                        ((tensor_TERM (6) + tensor_TERM (7) + tensor_TERM (8)) + \
                         (tensor_TERM (9) + tensor_TERM (10)))) + \
                       (((tensor_TERM (11) + tensor_TERM (12) + tensor_TERM (13)) + \
                         (tensor_TERM (14) + tensor_TERM (15))) + \
                        ((tensor_TERM (16) + tensor_TERM (17) + tensor_TERM (18)) + \
                         (tensor_TERM (19) + tensor_TERM (20))))

// 21 = (11) + (10) = (((3) + (3)) + ((3) + (2))) + (((3) + (2)) + ((3) + (2)))
#define tensor_ADD_21  (((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                         (tensor_TERM (4) + tensor_TERM (5) + tensor_TERM (6))) + \
                        ((tensor_TERM (7) + tensor_TERM (8) + tensor_TERM (9)) + \
                         (tensor_TERM (10) + tensor_TERM (11)))) + \
                       (((tensor_TERM (12) + tensor_TERM (13) + tensor_TERM (14)) + \
                         (tensor_TERM (15) + tensor_TERM (16))) + \
                        ((tensor_TERM (17) + tensor_TERM (18) + tensor_TERM (19)) + \
                         (tensor_TERM (20) + tensor_TERM (21))))

// 22 = (11) + (11) = (((3) + (3)) + ((3) + (2))) + (((3) + (3)) + ((3) + (2)))
#define tensor_ADD_22  (((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                         (tensor_TERM (4) + tensor_TERM (5) + tensor_TERM (6))) + \
                        ((tensor_TERM (7) + tensor_TERM (8) + tensor_TERM (9)) + \
                         (tensor_TERM (10) + tensor_TERM (11)))) + \
                       (((tensor_TERM (12) + tensor_TERM (13) + tensor_TERM (14)) + \
                         (tensor_TERM (15) + tensor_TERM (16) + tensor_TERM (17))) + \
                        ((tensor_TERM (18) + tensor_TERM (19) + tensor_TERM (20)) + \
                         (tensor_TERM (21) + tensor_TERM (22))))

// 23 = (12) + (11) = (((3) + (3)) + ((3) + (3))) + (((3) + (3)) + ((3) + (2)))
#define tensor_ADD_23  (((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                         (tensor_TERM (4) + tensor_TERM (5) + tensor_TERM (6))) + \
                        ((tensor_TERM (7) + tensor_TERM (8) + tensor_TERM (9)) + \
                         (tensor_TERM (10) + tensor_TERM (11) + tensor_TERM (12)))) + \
                       (((tensor_TERM (13) + tensor_TERM (14) + tensor_TERM (15)) + \
                         (tensor_TERM (16) + tensor_TERM (17) + tensor_TERM (18))) + \
                        ((tensor_TERM (19) + tensor_TERM (20) + tensor_TERM (21)) + \
                         (tensor_TERM (22) + tensor_TERM (23))))

// 24 = (12) + (12) = (((3) + (3)) + ((3) + (3))) + (((3) + (3)) + ((3) + (3)))
#define tensor_ADD_24  (((tensor_TERM (1) + tensor_TERM (2) + tensor_TERM (3)) + \
                         (tensor_TERM (4) + tensor_TERM (5) + tensor_TERM (6))) + \
                        ((tensor_TERM (7) + tensor_TERM (8) + tensor_TERM (9)) + \
                         (tensor_TERM (10) + tensor_TERM (11) + tensor_TERM (12)))) + \
                       (((tensor_TERM (13) + tensor_TERM (14) + tensor_TERM (15)) + \
                         (tensor_TERM (16) + tensor_TERM (17) + tensor_TERM (18))) + \
                        ((tensor_TERM (19) + tensor_TERM (20) + tensor_TERM (21)) + \
                         (tensor_TERM (22) + tensor_TERM (23) + tensor_TERM (24))))

// 25 = (13) + (12) = ((((2) + (2)) + (3)) + ((3) + (3))) + (((3) + (3)) + ((3) + (3)))
#define tensor_ADD_25  ((((tensor_TERM (1) + tensor_TERM (2)) + \
                          (tensor_TERM (3) + tensor_TERM (4))) + \
                         (tensor_TERM (5) + tensor_TERM (6) + tensor_TERM (7))) + \
                        ((tensor_TERM (8) + tensor_TERM (9) + tensor_TERM (10)) + \
                         (tensor_TERM (11) + tensor_TERM (12) + tensor_TERM (13)))) + \
                       (((tensor_TERM (14) + tensor_TERM (15) + tensor_TERM (16)) + \
                         (tensor_TERM (17) + tensor_TERM (18) + tensor_TERM (19))) + \
                        ((tensor_TERM (20) + tensor_TERM (21) + tensor_TERM (22)) + \
                         (tensor_TERM (23) + tensor_TERM (24) + tensor_TERM (25))))

// 26 = (13) + (13) = ((((2) + (2)) + (3)) + ((3) + (3))) + ((((2) + (2)) + (3)) + ((3) + (3)))
#define tensor_ADD_26  ((((tensor_TERM (1) + tensor_TERM (2)) + \
                          (tensor_TERM (3) + tensor_TERM (4))) + \
                         (tensor_TERM (5) + tensor_TERM (6) + tensor_TERM (7))) + \
                        ((tensor_TERM (8) + tensor_TERM (9) + tensor_TERM (10)) + \
                         (tensor_TERM (11) + tensor_TERM (12) + tensor_TERM (13)))) + \
                       ((((tensor_TERM (14) + tensor_TERM (15)) + \
                          (tensor_TERM (16) + tensor_TERM (17))) + \
                         (tensor_TERM (18) + tensor_TERM (19) + tensor_TERM (20))) + \
                        ((tensor_TERM (21) + tensor_TERM (22) + tensor_TERM (23)) + \
                         (tensor_TERM (24) + tensor_TERM (25) + tensor_TERM (26))))

// 27 = (14) + (13) = ((((2) + (2)) + (3)) + (((2) + (2)) + (3))) + ((((2) + (2)) + (3)) + ((3) + (3)))
#define tensor_ADD_27  ((((tensor_TERM (1) + tensor_TERM (2)) + \
                          (tensor_TERM (3) + tensor_TERM (4))) + \
                         (tensor_TERM (5) + tensor_TERM (6) + tensor_TERM (7))) + \
                        (((tensor_TERM (8) + tensor_TERM (9)) + \
                          (tensor_TERM (10) + tensor_TERM (11))) + \
                         (tensor_TERM (12) + tensor_TERM (13) + tensor_TERM (14)))) + \
                       ((((tensor_TERM (15) + tensor_TERM (16)) + \
                          (tensor_TERM (17) + tensor_TERM (18))) + \
                         (tensor_TERM (19) + tensor_TERM (20) + tensor_TERM (21))) + \
                        ((tensor_TERM (22) + tensor_TERM (23) + tensor_TERM (24)) + \
                         (tensor_TERM (25) + tensor_TERM (26) + tensor_TERM (27))))

// 28 = (14) + (14) = ((((2) + (2)) + (3)) + (((2) + (2)) + (3))) + ((((2) + (2)) + (3)) + (((2) + (2)) + (3)))
#define tensor_ADD_28  ((((tensor_TERM (1) + tensor_TERM (2)) + \
                          (tensor_TERM (3) + tensor_TERM (4))) + \
                         (tensor_TERM (5) + tensor_TERM (6) + tensor_TERM (7))) + \
                        (((tensor_TERM (8) + tensor_TERM (9)) + \
                          (tensor_TERM (10) + tensor_TERM (11))) + \
                         (tensor_TERM (12) + tensor_TERM (13) + tensor_TERM (14)))) + \
                       ((((tensor_TERM (15) + tensor_TERM (16)) + \
                          (tensor_TERM (17) + tensor_TERM (18))) + \
                         (tensor_TERM (19) + tensor_TERM (20) + tensor_TERM (21))) + \
                        (((tensor_TERM (22) + tensor_TERM (23)) + \
                          (tensor_TERM (24) + tensor_TERM (25))) + \
                         (tensor_TERM (26) + tensor_TERM (27) + tensor_TERM (28))))

// 29 = (15) + (14) = ((((2) + (2)) + ((2) + (2))) + (((2) + (2)) + (3))) + ((((2) + (2)) + (3)) + (((2) + (2)) + (3)))
#define tensor_ADD_29  ((((tensor_TERM (1) + tensor_TERM (2)) + \
                          (tensor_TERM (3) + tensor_TERM (4))) + \
                         ((tensor_TERM (5) + tensor_TERM (6)) + \
                          (tensor_TERM (7) + tensor_TERM (8)))) + \
                        (((tensor_TERM (9) + tensor_TERM (10)) + \
                          (tensor_TERM (11) + tensor_TERM (12))) + \
                         (tensor_TERM (13) + tensor_TERM (14) + tensor_TERM (15)))) + \
                       ((((tensor_TERM (16) + tensor_TERM (17)) + \
                          (tensor_TERM (18) + tensor_TERM (19))) + \
                         (tensor_TERM (20) + tensor_TERM (21) + tensor_TERM (22))) + \
                        (((tensor_TERM (23) + tensor_TERM (24)) + \
                          (tensor_TERM (25) + tensor_TERM (26))) + \
                         (tensor_TERM (27) + tensor_TERM (28) + tensor_TERM (29))))

// 30 = (15) + (15) = ((((2) + (2)) + ((2) + (2))) + (((2) + (2)) + (3))) + ((((2) + (2)) + ((2) + (2))) + (((2) + (2)) + (3)))
#define tensor_ADD_30  ((((tensor_TERM (1) + tensor_TERM (2)) + \
                          (tensor_TERM (3) + tensor_TERM (4))) + \
                         ((tensor_TERM (5) + tensor_TERM (6)) + \
                          (tensor_TERM (7) + tensor_TERM (8)))) + \
                        (((tensor_TERM (9) + tensor_TERM (10)) + \
                          (tensor_TERM (11) + tensor_TERM (12))) + \
                         (tensor_TERM (13) + tensor_TERM (14) + tensor_TERM (15)))) + \
                       ((((tensor_TERM (16) + tensor_TERM (17)) + \
                          (tensor_TERM (18) + tensor_TERM (19))) + \
                         ((tensor_TERM (20) + tensor_TERM (21)) + \
                          (tensor_TERM (22) + tensor_TERM (23)))) + \
                        (((tensor_TERM (24) + tensor_TERM (25)) + \
                          (tensor_TERM (26) + tensor_TERM (27))) + \
                         (tensor_TERM (28) + tensor_TERM (29) + tensor_TERM (30))))

// 31 = (16) + (15) = ((((2) + (2)) + ((2) + (2))) + (((2) + (2)) + ((2) + (2)))) + ((((2) + (2)) + ((2) + (2))) + (((2) + (2)) + (3)))
#define tensor_ADD_31  ((((tensor_TERM (1) + tensor_TERM (2)) + \
                          (tensor_TERM (3) + tensor_TERM (4))) + \
                         ((tensor_TERM (5) + tensor_TERM (6)) + \
                          (tensor_TERM (7) + tensor_TERM (8)))) + \
                        (((tensor_TERM (9) + tensor_TERM (10)) + \
                          (tensor_TERM (11) + tensor_TERM (12))) + \
                         ((tensor_TERM (13) + tensor_TERM (14)) + \
                          (tensor_TERM (15) + tensor_TERM (16))))) + \
                       ((((tensor_TERM (17) + tensor_TERM (18)) + \
                          (tensor_TERM (19) + tensor_TERM (20))) + \
                         ((tensor_TERM (21) + tensor_TERM (22)) + \
                          (tensor_TERM (23) + tensor_TERM (24)))) + \
                        (((tensor_TERM (25) + tensor_TERM (26)) + \
                          (tensor_TERM (27) + tensor_TERM (28))) + \
                         (tensor_TERM (29) + tensor_TERM (30) + tensor_TERM (31))))

// 32 = (16) + (16) = ((((2) + (2)) + ((2) + (2))) + (((2) + (2)) + ((2) + (2)))) + ((((2) + (2)) + ((2) + (2))) + (((2) + (2)) + ((2) + (2))))
#define tensor_ADD_32  ((((tensor_TERM (1) + tensor_TERM (2)) + \
                          (tensor_TERM (3) + tensor_TERM (4))) + \
                         ((tensor_TERM (5) + tensor_TERM (6)) + \
                          (tensor_TERM (7) + tensor_TERM (8)))) + \
                        (((tensor_TERM (9) + tensor_TERM (10)) + \
                          (tensor_TERM (11) + tensor_TERM (12))) + \
                         ((tensor_TERM (13) + tensor_TERM (14)) + \
                          (tensor_TERM (15) + tensor_TERM (16))))) + \
                       ((((tensor_TERM (17) + tensor_TERM (18)) + \
                          (tensor_TERM (19) + tensor_TERM (20))) + \
                         ((tensor_TERM (21) + tensor_TERM (22)) + \
                          (tensor_TERM (23) + tensor_TERM (24)))) + \
                        (((tensor_TERM (25) + tensor_TERM (26)) + \
                          (tensor_TERM (27) + tensor_TERM (28))) + \
                         ((tensor_TERM (29) + tensor_TERM (30)) + \
                          (tensor_TERM (31) + tensor_TERM (32)))))

void sum_mean_scalar (numvec x, real *p_sum, real *p_mean) noexcept {
	if (x.size <= 4) {
		switch (x.size) {
			case 0: {
				if (p_sum) *p_sum = 0.0;
				if (p_mean) *p_mean = undefined;
			} break; case 1: {
				if (p_sum) *p_sum = x [1];
				if (p_mean) *p_mean = x [1];
			} break; case 2: {
				real80 sum = (real80) x [1] + (real80) x [2];
				if (p_sum) *p_sum = (real) sum;
				if (p_mean) *p_mean = real (0.5 * sum);
			} break; case 3: {
				real80 sum = (real80) x [1] + (real80) x [2] + (real80) x [3];
				if (p_sum) *p_sum = (real) sum;
				if (p_mean) *p_mean = real (sum / 3.0);
			} break; case 4: {
				real80 sum = ((real80) x [1] + (real80) x [2]) + ((real80) x [3] + (real80) x [4]);
				if (p_sum) *p_sum = (real) sum;
				if (p_mean) *p_mean = real (0.25 * sum);
			} break; default: {
				if (p_sum) *p_sum = undefined;
				if (p_mean) *p_mean = undefined;
			}
		}
		return;
	}
	if (Melder_debug != 0) {
		if (Melder_debug == 48) {
			/*
				Naive implementation in real64.
			*/
			real sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = sum;
			real mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = mean;
			return;
		}
		if (Melder_debug == 49) {
			/*
				Naive implementation in real80.
			*/
			real80 sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += (real80) x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = (real) sum;
			real80 mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = (real) mean;
			return;
		}
		if (Melder_debug == 50) {
			/*
				First-element offset corrects for large DC components.
			*/
			real80 offset = (real80) x [1];   // x.size != 0 -> offset in R
			real80 sumOfDifferences = 0.0;   // sumOfDifferences in R (invariant)
			for (integer i = 2; i <= x.size; i ++) {
				sumOfDifferences += (real80) x [i] - offset;   // sumOfDifferences before in R, x [i] in R, offset in R -> sumOfDifferences after in R
			}
			if (p_sum) {
				real80 sum = sumOfDifferences + offset * x.size;
				*p_sum = (real) sum;
			}
			real80 mean = offset + sumOfDifferences / x.size;   // offset in R, sumOfDifferences in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = (real) mean;
			return;
		}
		if (Melder_debug == 51) {
			/*
				Chan, Golub & LeVeque's pairwise algorithm.
			*/
			#define REAL  real80
			//real offset = x [1];
			const real offset = 0.0;
			long terms [65];
			REAL suma [65];
			terms [1] = 0;
			int top = 2;
			long n2 = x.size / 2;
			for (long i = 1; i <= n2; i ++) {
				/*
					Compute the sum of the next two data points.
					Put this sum on top of the stack.
				*/
				long start = 2 * i - 1;
				suma [top] = (REAL) (x [start] - offset) + REAL (x [start + 1] - offset);
				terms [top] = 2;
				while (terms [top] == terms [top - 1]) {
					top --;
					terms [top] *= 2;
					suma [top] += suma [top + 1];
				}
				top ++;
			}
			top --;
			if (x.size & 1) {
				/*
					x.size is odd. Put the last point on the stack.
				*/
				top ++;
				suma [top] = (REAL) (x [x.size] - offset);
			}
			REAL sum = suma [top];
			/*
				If the remaining stack contains more than one element, x.size is not a power of 2.
				Add all the elements.
			*/
			for (long i = top - 1; i >= 2; i --) {
				sum += suma [i];
			}
			REAL mean = offset + sum / x.size;
			if (p_sum) {
				sum += offset * x.size;
				*p_sum = (real) sum;
			}
			if (p_mean) *p_mean = (real) mean;
			#undef REAL
			return;
		}
		if (Melder_debug == 52) {
			/*
				Pairwise algorithm with base case 8.
				
				For an explanation see the base case 32 case.
			*/
			constexpr integer baseCasePower = 3;
			constexpr integer baseCaseSize = 1 << baseCasePower;
			integer remainder = x.size % baseCaseSize;
			real80 sum;
			real *y = x.at;
			#define tensor_TERM(i)  (real80) y [i]
			switch (remainder) {
				case 0: sum = 0.0; break;
				case 1: sum = tensor_ADD_1; break;
				case 2: sum = tensor_ADD_2; break;
				case 3: sum = tensor_ADD_3; break;
				case 4: sum = tensor_ADD_4; break;
				case 5: sum = tensor_ADD_5; break;
				case 6: sum = tensor_ADD_6; break;
				case 7: sum = tensor_ADD_7; break;
				default: sum = undefined;
			}
			integer numberOfBaseCases = x.size / baseCaseSize;
			if (numberOfBaseCases != 0) {
				constexpr integer highestIndex = 63 - baseCasePower;
				integer numbersOfTerms [1 + highestIndex];
				real80 partialSums [1 + highestIndex];
				numbersOfTerms [0] = 0;
				integer stackPointer = 0;
				y += remainder;
				for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
					partialSums [++ stackPointer] = tensor_ADD_8;
					numbersOfTerms [stackPointer] = baseCaseSize;
					while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
						numbersOfTerms [-- stackPointer] *= 2;
						partialSums [stackPointer] += partialSums [stackPointer + 1];
					}
					y += baseCaseSize;
				}
				for (integer i = stackPointer; i > 0; i --) {
					sum += partialSums [i];
				}
			}
			#undef tensor_TERM
			if (p_sum) *p_sum = (real) sum;
			if (p_mean) {
				real80 mean = sum / x.size;
				*p_mean = (real) mean;
			}
			return;
		}
		if (Melder_debug == 53) {
			/*
				Pairwise algorithm with base case 16.
				
				For an explanation see the base case 32 case.
			*/
			constexpr integer baseCasePower = 4;
			constexpr integer baseCaseSize = 1 << baseCasePower;
			integer remainder = x.size % baseCaseSize;
			real80 sum;
			real *y = x.at;
			#define tensor_TERM(i)  (real80) y [i]
			switch (remainder) {
				case 0: sum = 0.0; break;
				case 1: sum = tensor_ADD_1; break;
				case 2: sum = tensor_ADD_2; break;
				case 3: sum = tensor_ADD_3; break;
				case 4: sum = tensor_ADD_4; break;
				case 5: sum = tensor_ADD_5; break;
				case 6: sum = tensor_ADD_6; break;
				case 7: sum = tensor_ADD_7; break;
				case 8: sum = tensor_ADD_8; break;
				case 9: sum = tensor_ADD_9; break;
				case 10: sum = tensor_ADD_10; break;
				case 11: sum = tensor_ADD_11; break;
				case 12: sum = tensor_ADD_12; break;
				case 13: sum = tensor_ADD_13; break;
				case 14: sum = tensor_ADD_14; break;
				case 15: sum = tensor_ADD_15; break;
				default: sum = undefined;
			}
			integer numberOfBaseCases = x.size / baseCaseSize;
			if (numberOfBaseCases != 0) {
				constexpr integer highestIndex = 63 - baseCasePower;
				integer numbersOfTerms [1 + highestIndex];
				real80 partialSums [1 + highestIndex];
				numbersOfTerms [0] = 0;
				integer stackPointer = 0;
				y += remainder;
				for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
					partialSums [++ stackPointer] = tensor_ADD_16;
					numbersOfTerms [stackPointer] = baseCaseSize;
					while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
						numbersOfTerms [-- stackPointer] *= 2;
						partialSums [stackPointer] += partialSums [stackPointer + 1];
					}
					y += baseCaseSize;
				}
				for (integer i = stackPointer; i > 0; i --) {
					sum += partialSums [i];
				}
			}
			#undef tensor_TERM
			if (p_sum) *p_sum = (real) sum;
			if (p_mean) {
				real80 mean = sum / x.size;
				*p_mean = (real) mean;
			}
			return;
		}
	}
	/*
		Our standard: pairwise algorithm with base case size 32 (if baseCasePower is 5).

		If you want to change the base case size, do the following three things:
		1. Change the `constexpr integer baseCasePower = 5` assignment (e.g. to 6).
		2. Change the number of cases in the switch statement (e.g. up to case 63).
		3. Change the `partialSums [++ stackPointer] = tensor_ADD_32` assignment (e.g. to tensor_ADD_64).
	*/
	constexpr integer baseCasePower = 5;
	constexpr integer baseCaseSize = 1 << baseCasePower;
	integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
	real80 sum;
	real *y = x.at;
	#define tensor_TERM(i)  (real80) y [i]
	switch (remainder) {
		case 0: sum = 0.0; break;
		case 1: sum = tensor_ADD_1; break;
		case 2: sum = tensor_ADD_2; break;
		case 3: sum = tensor_ADD_3; break;
		case 4: sum = tensor_ADD_4; break;
		case 5: sum = tensor_ADD_5; break;
		case 6: sum = tensor_ADD_6; break;
		case 7: sum = tensor_ADD_7; break;
		case 8: sum = tensor_ADD_8; break;
		case 9: sum = tensor_ADD_9; break;
		case 10: sum = tensor_ADD_10; break;
		case 11: sum = tensor_ADD_11; break;
		case 12: sum = tensor_ADD_12; break;
		case 13: sum = tensor_ADD_13; break;
		case 14: sum = tensor_ADD_14; break;
		case 15: sum = tensor_ADD_15; break;
		case 16: sum = tensor_ADD_16; break;
		case 17: sum = tensor_ADD_17; break;
		case 18: sum = tensor_ADD_18; break;
		case 19: sum = tensor_ADD_19; break;
		case 20: sum = tensor_ADD_20; break;
		case 21: sum = tensor_ADD_21; break;
		case 22: sum = tensor_ADD_22; break;
		case 23: sum = tensor_ADD_23; break;
		case 24: sum = tensor_ADD_24; break;
		case 25: sum = tensor_ADD_25; break;
		case 26: sum = tensor_ADD_26; break;
		case 27: sum = tensor_ADD_27; break;
		case 28: sum = tensor_ADD_28; break;
		case 29: sum = tensor_ADD_29; break;
		case 30: sum = tensor_ADD_30; break;
		case 31: sum = tensor_ADD_31; break;
		default: sum = undefined;   // will not occur unless somebody copied too few cases here (programming error)
	}
	if (numberOfBaseCases != 0) {
		/*
			The value of numbersOfTerms [0] stays at 0, to denote the bottom of the stack.
			The maximum value of numbersOfTerms [1] should be 2^62, because x.size can be at most 2^63-1 (if sizeof integer is 64).
			The maximum value of numbersOfTerms [2] should then be 2^61.
			The maximum value of numbersOfTerms [3] should be 2^60.
			...
			The maximum value of numbersOfTerms [58] should be 2^5, which is the granularity with which base case sums are put on the stack.
			The maximum value of numbersOfTerms [59] should also be 2^5, because this can be the situation just before collapsing the top of the stack.
			However, if the whole stack is filled up like this, the actual number of terms is already 2^63. Therefore, we need one element less.
			So the highest index of numbersOfTerms [] should be 58.
		*/
		constexpr integer highestIndex = 63 - baseCasePower;
		integer numbersOfTerms [1 + highestIndex];
		real80 partialSums [1 + highestIndex];
		numbersOfTerms [0] = 0;   // the constant zero at the bottom of the stack
		integer stackPointer = 0;
		y += remainder;
		for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
			/*
				Compute the sum of the next 32 data points.
				Put this sum on top of the stack.
			*/
			partialSums [++ stackPointer] = tensor_ADD_32;
			numbersOfTerms [stackPointer] = baseCaseSize;
			while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
				numbersOfTerms [-- stackPointer] *= 2;
				partialSums [stackPointer] += partialSums [stackPointer + 1];
			}
			y += baseCaseSize;
		}
		/*
			Add all the elements of the stack, starting at the top (small sums to big sums).
		*/
		for (integer i = stackPointer; i > 0; i --) {
			sum += partialSums [i];
		}
	}
	#undef tensor_TERM
	if (p_sum) *p_sum = (real) sum;
	if (p_mean) {
		real80 mean = sum / x.size;   // it helps a bit to perform this division while still in real80
		*p_mean = (real) mean;
	}
}

void sum_mean_sumsq_variance_stdev_scalar (numvec x, real *p_sum, real *p_mean, real *p_sumsq, real *p_variance, real *p_stdev) noexcept {
	if (x.size < 2) {
		if (x.size <= 0) {
			if (p_sum) *p_sum = 0.0;
			if (p_mean) *p_mean = undefined;
			if (p_sumsq) *p_sumsq = undefined;
		} else {
			if (p_sum) *p_sum = x [1];
			if (p_mean) *p_mean = x [1];
			if (p_sumsq) *p_sumsq = 0.0;
		}
		if (p_variance) *p_variance = undefined;
		if (p_stdev) *p_stdev = undefined;
		return;
	}
	if (Melder_debug != 0) {
		if (Melder_debug == 48) {
			/*
				Naive implementation in real64.
			*/
			real sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = sum;
			real mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) return;
			real sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				real residual = x [i] - mean;   // x [i] in R, mean in R -> residual in R
				real squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
				sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
			}
			if (p_sumsq) *p_sumsq = sumOfSquaredResiduals;
			integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
			real meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
			if (p_variance) *p_variance = (real) meanSquaredResidual;
			if (p_stdev) {
				real rootMeanSquaredResidual = sqrt (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
				*p_stdev = rootMeanSquaredResidual;
			}
			return;
		}
		if (Melder_debug == 49) {
			/*
				Naive implementation in real80.
			*/
			real80 sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += (real80) x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = (real) sum;
			real80 mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = (real) mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) return;
			real80 sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				real80 residual = (real80) x [i] - mean;   // x [i] in R, mean in R -> residual in R
				real80 squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
				sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
			}
			if (p_sumsq) *p_sumsq = (real) sumOfSquaredResiduals;
			integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
			real80 meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
			if (p_variance) *p_variance = (real) meanSquaredResidual;
			if (p_stdev) {
				real80 rootMeanSquaredResidual = sqrtl (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
				*p_stdev = (real) rootMeanSquaredResidual;
			}
			return;
		}
		if (Melder_debug == 50) {
			/*
				First-element offset corrects for large DC components.
			*/
			real80 offset = (real80) x [1];   // x.size != 0 -> offset in R
			real80 sumOfDifferences = 0.0;   // sumOfDifferences in R (invariant)
			for (integer i = 2; i <= x.size; i ++) {
				sumOfDifferences += (real80) x [i] - offset;   // sumOfDifferences before in R, x [i] in R, offset in R -> sumOfDifferences after in R
			}
			if (p_sum) {
				real80 sum = sumOfDifferences + offset * x.size;
				*p_sum = (real) sum;
			}
			real80 mean = offset + sumOfDifferences / x.size;   // offset in R, sumOfDifferences in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = (real) mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) return;
			real80 sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				real80 residual = (real80) x [i] - mean;   // x [i] in R, mean in R -> residual in R
				real80 squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
				sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
			}
			if (p_sumsq) *p_sumsq = (real) sumOfSquaredResiduals;
			integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
			real80 meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
			if (p_variance) *p_variance = (real) meanSquaredResidual;
			if (p_stdev) {
				real80 rootMeanSquaredResidual = sqrtl (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
				*p_stdev = (real) rootMeanSquaredResidual;
			}
			return;
		}
		if (Melder_debug == 51) {
			/*
				Chan, Golub & LeVeque's pairwise algorithm.
			*/
			#define REAL  real80
			if (! p_sumsq && ! p_variance && ! p_stdev) {
				//real offset = x [1];
				const real offset = 0.0;
				long terms [65];
				REAL suma [65];
				terms [1] = 0;
				int top = 2;
				long n2 = x.size / 2;
				for (long i = 1; i <= n2; i ++) {
					/*
						Compute the sum of the next two data points.
						Put this sum on top of the stack.
					*/
					long start = 2 * i - 1;
					suma [top] = (REAL) (x [start] - offset) + REAL (x [start + 1] - offset);
					terms [top] = 2;
					while (terms [top] == terms [top - 1]) {
						top --;
						terms [top] *= 2;
						suma [top] += suma [top + 1];
					}
					top ++;
				}
				top --;
				if (x.size & 1) {
					/*
						x.size is odd. Put the last point on the stack.
					*/
					top ++;
					suma [top] = (REAL) (x [x.size] - offset);
				}
				REAL sum = suma [top];
				/*
					If the remaining stack contains more than one element, x.size is not a power of 2.
					Add all the elements.
				*/
				for (long i = top - 1; i >= 2; i --) {
					sum += suma [i];
				}
				REAL mean = offset + sum / x.size;
				if (p_sum) {
					sum += offset * x.size;
					*p_sum = (real) sum;
				}
				if (p_mean) *p_mean = (real) mean;
				return;
			}
			int64 terms [65];
			REAL suma [65], sa [65];
			terms [1] = 0;
			int top = 2;
			long n2 = x.size / 2;
			for (long i = 1; i <= n2; i ++) {
				suma [top] = x [2*i-1] + x [2*i];
				REAL diff = x [2*i] - x [2*i-1];
				sa [top] = diff * diff / 2.0;
				terms [top] = 2;
				while (terms [top] == terms [top - 1]) {
					top --;
					terms [top] *= 2;
					diff = suma [top] - suma [top + 1];
					sa [top] += sa [top + 1] + diff * diff / terms [top];
					suma [top] += suma [top + 1];
				}
				top ++;
			}
			top --;
			if (x.size & 1) {
				top ++;
				terms [top] = 1;
				suma [top] = x [x.size];
				sa [top] = 0.0;
			}
			long t = terms [top];
			REAL sum = suma [top];
			REAL sumOfSquaredResiduals = sa [top];
			for (long i = top - 1; i >= 2; i --) {
				REAL diff = terms [i] * sum / t - suma [i];
				sumOfSquaredResiduals += sa [i] + t * diff * diff / terms [i] / (terms [i] + t);
				sum += suma [i];
				t += terms [i];
			}
			REAL mean = sum / x.size;
			REAL variance = sumOfSquaredResiduals / (x.size - 1);
			if (p_sum) *p_sum = (real) sum;
			if (p_mean) *p_mean = (real) mean;
			if (p_sumsq) *p_sumsq = (real) sumOfSquaredResiduals;
			if (p_variance) *p_variance = (real) variance;
			if (p_stdev) *p_stdev = (real) sqrtl (variance);
			#undef REAL
			return;
		}
		if (Melder_debug == 52) {
			/*
				Pairwise algorithm with base case 8.
			*/
			#define REAL  real80
			//real offset = x [1];
			const real offset = 0.0;
			integer numbersOfTerms [1+60];
			REAL partialSums [1+60];
			numbersOfTerms [0] = 0;
			integer stackPointer = 0;
			integer n8 = x.size / 8, remainder = x.size % 8;
			for (integer ipart = 1; ipart <= n8; ipart ++) {
				/*
					Compute the sum of the next eight data points.
					Put this sum on top of the stack.
				*/
				real *y = & x [8 * (ipart - 1)];
				#define tensor_TERM(i)  REAL (y [i] - offset)
				partialSums [++ stackPointer] = tensor_ADD_8;
				#undef tensor_TERM
				numbersOfTerms [stackPointer] = 8;
				while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
					numbersOfTerms [-- stackPointer] *= 2;
					partialSums [stackPointer] += partialSums [stackPointer + 1];
				}
			}
			REAL sum = 0.0;
			if (remainder != 0) {
				real *y = & x [x.size - remainder];
				switch (remainder) {
					#define tensor_TERM(i)  REAL (y [i] - offset)
					case 1: sum = tensor_ADD_1; break;
					case 2: sum = tensor_ADD_2; break;
					case 3: sum = tensor_ADD_3; break;
					case 4: sum = tensor_ADD_4; break;
					case 5: sum = tensor_ADD_5; break;
					case 6: sum = tensor_ADD_6; break;
					case 7: sum = tensor_ADD_7; break;
					#undef tensor_TERM
				}
			}
			/*
				Add all the elements of the stack.
			*/
			for (integer i = stackPointer; i > 0; i --) {
				sum += partialSums [i];
			}
			REAL mean = offset + sum / x.size;
			if (p_sum) {
				sum += offset * x.size;
				*p_sum = (real) sum;
			}
			real mean64 = (real) mean;
			if (p_mean) *p_mean = mean64;
			if (! p_sumsq && ! p_variance && ! p_stdev) {
				return;
			}
			stackPointer = 0;
			for (integer ipart = 1; ipart <= n8; ipart ++) {
				real *y = & x [8 * (ipart - 1)];
				#define tensor_TERM(i)  REAL (y [i] - mean64) * REAL (y [i] - mean64)
				partialSums [++ stackPointer] = tensor_ADD_8;
				#undef tensor_TERM
				numbersOfTerms [stackPointer] = 16;
				while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
					numbersOfTerms [-- stackPointer] *= 2;
					partialSums [stackPointer] += partialSums [stackPointer + 1];
				}
			}
			REAL sumsq = 0.0;
			if (remainder != 0) {
				real *y = & x [x.size - remainder];
				switch (remainder) {
					#define tensor_TERM(i)  REAL (y [i] - mean64) * REAL (y [i] - mean64)
					case 1: sumsq = tensor_ADD_1; break;
					case 2: sumsq = tensor_ADD_2; break;
					case 3: sumsq = tensor_ADD_3; break;
					case 4: sumsq = tensor_ADD_4; break;
					case 5: sumsq = tensor_ADD_5; break;
					case 6: sumsq = tensor_ADD_6; break;
					case 7: sumsq = tensor_ADD_7; break;
					#undef tensor_TERM
				}
			}
			for (integer i = stackPointer; i > 0; i --) {
				sumsq += partialSums [i];
			}
			real variance = (real) sumsq / (x.size - 1);
			if (p_sumsq) *p_sumsq = (real) sumsq;
			if (p_variance) *p_variance = variance;
			if (p_stdev) *p_stdev = sqrt (variance);
			#undef REAL
			return;
		}
		if (Melder_debug == 53) {
			real mean;
			sum_mean_scalar (x, p_sum, & mean);   // compute the sum only if the user asks for it, but the mean always, because we need it here
			if (p_mean) *p_mean = mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) {
				return;
			}
			constexpr integer baseCasePower = 4;
			constexpr integer baseCaseSize = 1 << baseCasePower;
			integer remainder = x.size % baseCaseSize;
			real80 sumsq;
			real *y = x.at;
			switch (remainder) {
				#define tensor_TERM(i)  real80 (y [i] - mean) * real80 (y [i] - mean)
				case 0: sumsq = 0.0;
				case 1: sumsq = tensor_ADD_1; break;
				case 2: sumsq = tensor_ADD_2; break;
				case 3: sumsq = tensor_ADD_3; break;
				case 4: sumsq = tensor_ADD_4; break;
				case 5: sumsq = tensor_ADD_5; break;
				case 6: sumsq = tensor_ADD_6; break;
				case 7: sumsq = tensor_ADD_7; break;
				case 8: sumsq = tensor_ADD_8; break;
				case 9: sumsq = tensor_ADD_9; break;
				case 10: sumsq = tensor_ADD_10; break;
				case 11: sumsq = tensor_ADD_11; break;
				case 12: sumsq = tensor_ADD_12; break;
				case 13: sumsq = tensor_ADD_13; break;
				case 14: sumsq = tensor_ADD_14; break;
				case 15: sumsq = tensor_ADD_15; break;
				default: sumsq = undefined;
				#undef tensor_TERM
			}
			integer numberOfBaseCases = x.size / baseCaseSize;
			if (numberOfBaseCases != 0) {
				constexpr integer highestIndex = 63 - baseCasePower;
				integer numbersOfTerms [1 + highestIndex];
				real80 partialSums [1 + highestIndex];
				numbersOfTerms [0] = 0;
				integer stackPointer = 0;
				y += remainder;
				for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
					#define tensor_TERM(i)  real80 (y [i] - mean) * real80 (y [i] - mean)
					partialSums [++ stackPointer] = tensor_ADD_16;
					#undef tensor_TERM
					numbersOfTerms [stackPointer] = baseCaseSize;
					while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
						numbersOfTerms [-- stackPointer] *= 2;
						partialSums [stackPointer] += partialSums [stackPointer + 1];
					}
					y += baseCaseSize;
				}
				for (integer i = stackPointer; i > 0; i --) {
					sumsq += partialSums [i];
				}
			}
			real variance = (real) sumsq / (x.size - 1);
			if (p_sumsq) *p_sumsq = (real) sumsq;
			if (p_variance) *p_variance = variance;
			if (p_stdev) *p_stdev = sqrt (variance);
			return;
		}
	} else {
		/*
			Our standard: pairwise algorithm with base case 32.
		*/
		real mean;
		sum_mean_scalar (x, p_sum, & mean);   // compute the sum only if the user asks for it, but the mean always, because we need it here
		if (p_mean) *p_mean = mean;
		if (! p_sumsq && ! p_variance && ! p_stdev) {
			return;
		}
		constexpr integer baseCasePower = 5;
		constexpr integer baseCaseSize = 1 << baseCasePower;
		integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
		real80 sumsq;
		real *y = x.at;
		switch (remainder) {
			#define tensor_TERM(i)  real80 (y [i] - mean) * real80 (y [i] - mean)
			case 0: sumsq = 0.0;
			case 1: sumsq = tensor_ADD_1; break;
			case 2: sumsq = tensor_ADD_2; break;
			case 3: sumsq = tensor_ADD_3; break;
			case 4: sumsq = tensor_ADD_4; break;
			case 5: sumsq = tensor_ADD_5; break;
			case 6: sumsq = tensor_ADD_6; break;
			case 7: sumsq = tensor_ADD_7; break;
			case 8: sumsq = tensor_ADD_8; break;
			case 9: sumsq = tensor_ADD_9; break;
			case 10: sumsq = tensor_ADD_10; break;
			case 11: sumsq = tensor_ADD_11; break;
			case 12: sumsq = tensor_ADD_12; break;
			case 13: sumsq = tensor_ADD_13; break;
			case 14: sumsq = tensor_ADD_14; break;
			case 15: sumsq = tensor_ADD_15; break;
			case 16: sumsq = tensor_ADD_16; break;
			case 17: sumsq = tensor_ADD_17; break;
			case 18: sumsq = tensor_ADD_18; break;
			case 19: sumsq = tensor_ADD_19; break;
			case 20: sumsq = tensor_ADD_20; break;
			case 21: sumsq = tensor_ADD_21; break;
			case 22: sumsq = tensor_ADD_22; break;
			case 23: sumsq = tensor_ADD_23; break;
			case 24: sumsq = tensor_ADD_24; break;
			case 25: sumsq = tensor_ADD_25; break;
			case 26: sumsq = tensor_ADD_26; break;
			case 27: sumsq = tensor_ADD_27; break;
			case 28: sumsq = tensor_ADD_28; break;
			case 29: sumsq = tensor_ADD_29; break;
			case 30: sumsq = tensor_ADD_30; break;
			case 31: sumsq = tensor_ADD_31; break;
			default: sumsq = undefined;
			#undef tensor_TERM
		}
		if (numberOfBaseCases != 0) {
			constexpr integer highestIndex = 63 - baseCasePower;
			integer numbersOfTerms [1 + highestIndex];
			real80 partialSums [1 + highestIndex];
			numbersOfTerms [0] = 0;
			integer stackPointer = 0;
			y += remainder;
			for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
				#define tensor_TERM(i)  real80 (y [i] - mean) * real80 (y [i] - mean)
				partialSums [++ stackPointer] = tensor_ADD_32;
				#undef tensor_TERM
				numbersOfTerms [stackPointer] = baseCaseSize;
				while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
					numbersOfTerms [-- stackPointer] *= 2;
					partialSums [stackPointer] += partialSums [stackPointer + 1];
				}
				y += baseCaseSize;
			}
			for (integer i = stackPointer; i > 0; i --) {
				sumsq += partialSums [i];
			}
		}
		real variance = (real) sumsq / (x.size - 1);
		if (p_sumsq) *p_sumsq = (real) sumsq;
		if (p_variance) *p_variance = variance;
		if (p_stdev) *p_stdev = sqrt (variance);
	}
}

real sumsq_scalar (numvec x) noexcept {
	real sumsq;
	sum_mean_sumsq_variance_stdev_scalar (x, nullptr, nullptr, & sumsq, nullptr, nullptr);
	return sumsq;
}

real variance_scalar (numvec x) noexcept {
	real variance;
	sum_mean_sumsq_variance_stdev_scalar (x, nullptr, nullptr, nullptr, & variance, nullptr);
	return variance;
}

real stdev_scalar (numvec x) noexcept {
	real stdev;
	sum_mean_sumsq_variance_stdev_scalar (x, nullptr, nullptr, nullptr, nullptr, & stdev);
	return stdev;
}

double center_scalar (numvec x) noexcept {
	double weightedSumOfIndexes = 0.0, sumOfWeights = 0.0;
	for (integer i = 1; i <= x.size; i ++) {
		weightedSumOfIndexes += i * x [i];
		sumOfWeights += x [i];
	}
	return weightedSumOfIndexes / sumOfWeights;
}

autonumvec copy_numvec (numvec x) {
	autonumvec result (x.size, false);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i];
	}
	return result;
}

autonummat copy_nummat (nummat x) {
	autonummat result (x.nrow, x.ncol, false);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol];
		}
	}
	return result;
}

autonummat outer_nummat (numvec x, numvec y) {
	autonummat result (x.size, y.size, false);
	for (integer irow = 1; irow <= x.size; irow ++) {
		for (integer icol = 1; icol <= y.size; icol ++) {
			result [irow] [icol] = x [irow] * y [icol];
		}
	}
	return result;
}

autonummat peaks_nummat (numvec x, bool includeEdges, int interpolate, bool sortByHeight) {
	if (x.size < 2) {
		includeEdges = false;
	}
	integer numberOfPeaks = 0;
	for (integer i = 2; i < x.size; i ++) {
		if (x [i] > x [i - 1] && x [i] >= x [i + 1]) {
			numberOfPeaks ++;
		}
	}
	if (includeEdges) {
		if (x [1] > x [2]) numberOfPeaks ++;
		if (x [x.size] > x [x.size - 1]) numberOfPeaks ++;
	}
	autonummat result (2, numberOfPeaks, false);
	integer peakNumber = 0;
	if (includeEdges && x [1] > x [2]) {
		result [1] [++ peakNumber] = 1;
		result [2] [peakNumber] = x [1];
	}
	for (integer i = 2; i < x.size; i ++) {
		if (x [i] > x [i - 1] && x [i] >= x [i + 1]) {
			++ peakNumber;
			if (interpolate) {
				real dy = 0.5 * (x [i + 1] - x [i - 1]);
				real d2y = (x [i] - x [i - 1]) + (x [i] - x [i + 1]);
				Melder_assert (d2y > 0.0);
				result [1] [peakNumber] = (real) i + dy / d2y;
				result [2] [peakNumber] = x [i] + 0.5 * dy * (dy / d2y);
			} else {
				result [1] [peakNumber] = i;
				result [2] [peakNumber] = x [i];
			}
		}
	}
	if (includeEdges && x [x.size] > x [x.size - 1]) {
		result [1] [++ peakNumber] = x.size;
		result [2] [peakNumber] = x [x.size];
	}
	Melder_assert (peakNumber == numberOfPeaks);
	if (sortByHeight) {
		for (integer i = 1; i <= numberOfPeaks; i ++)
			result [2] [i] *= -1.0;
		NUMsort2 (result.ncol, result [2], result [1]);
		for (integer i = 1; i <= numberOfPeaks; i ++)
			result [2] [i] *= -1.0;
	}
	return result;
}

/* End of file tensor.cpp */
