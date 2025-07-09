/* median_of_ninthers.cpp
 *
 * Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://boost.org/LICENSE_1_0.txt)
 */

#include "median_of_ninthers.h"

template <class T>
static void quickselect(T* beg, T* mid, T* end) {
	if (beg == end || mid >= end) return;
	Melder_assert (beg <= mid && mid < end);
	adaptiveQuickselect(beg, mid - beg, end - beg);
}

void (*computeSelection)(double*, double*, double*)
	= &quickselect<double>;
