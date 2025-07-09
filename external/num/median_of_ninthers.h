#ifndef _median_of_ninthers_h_
#define _median_of_ninthers_h_
/* Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://boost.org/LICENSE_1_0.txt)
 */

#include "melder_assert.h"
#include "median_common.h"

template <class T>
size_t partitionImpl(T* beg, size_t length);

template <class T>
void adaptiveQuickselect(T* beg, size_t n, size_t length);

/**
Median of minima
*/
template <class T>
size_t medianOfMinima(T*const r, const size_t n, const size_t length)
{
	Melder_assert (length >= 2);
	Melder_assert (n * 4 <= length);
	Melder_assert (n > 0);
	const size_t subset = n * 2,
		computeMinOver = (length - subset) / subset;
	Melder_assert (computeMinOver > 0);
	for (size_t i = 0, j = subset; i < subset; ++i)
	{
		const auto limit = j + computeMinOver;
		size_t minIndex = j;
		while (++j < limit)
			if (r[j] < r[minIndex])
				minIndex = j;
		if (r[minIndex] < r[i])
			std::swap (r[i], r[minIndex]);
		Melder_assert (j < length || i + 1 == subset);
	}
	adaptiveQuickselect(r, n, subset);
	return expandPartition(r, 0, n, subset, length);
}

/**
Median of maxima
*/
template <class T>
size_t medianOfMaxima(T*const r, const size_t n, const size_t length)
{
	Melder_assert (length >= 2);
	Melder_assert (n * 4 >= length * 3 && n < length);
	const size_t subset = (length - n) * 2,
		subsetStart = length - subset,
		computeMaxOver = subsetStart / subset;
	Melder_assert (computeMaxOver > 0);
	for (size_t i = subsetStart, j = i - subset * computeMaxOver; i < length; ++i)
	{
		const auto limit = j + computeMaxOver;
		size_t maxIndex = j;
		while (++j < limit)
			if (r[j] > r[maxIndex])
				maxIndex = j;
		if (r[maxIndex] > r[i])
			std::swap (r[i], r[maxIndex]);
		Melder_assert (j != 0 || i + 1 == length);
	}
	adaptiveQuickselect(r + subsetStart, length - n, subset);
	return expandPartition(r, subsetStart, n, length, length);
}

/**
Partitions r[0 .. length] using a pivot of its own choosing. Attempts to pick a
pivot that approximates the median. Returns the position of the pivot.
*/
template <class T>
size_t medianOfNinthers(T*const r, const size_t length)
{
	Melder_assert (length >= 12);
	const auto frac =
		length <= 1024 ? length / 12 :
		length <= 128 * 1024 ? length / 64
		: length / 1024;
	auto pivot = frac / 2;
	const auto lo = length / 2 - pivot, hi = lo + frac;
	Melder_assert (lo >= frac * 4);
	Melder_assert (length - hi >= frac * 4);
	Melder_assert (lo / 2 >= pivot);
	const auto gap = (length - 9 * frac) / 4;
	auto a = lo - 4 * frac - gap, b = hi + gap;
	for (size_t i = lo; i < hi; ++i, a += 3, b += 3)
	{
		ninther(r, a, i - frac, b, a + 1, i, b + 1, a + 2, i + frac, b + 2);
	}

	adaptiveQuickselect(r + lo, pivot, frac);
	return expandPartition(r, lo, lo + pivot, hi, length);
}

/**

Quickselect driver for medianOfNinthers, medianOfMinima, and medianOfMaxima.
Dispathes to each depending on the relationship between n (the sought order
statistics) and length.

*/
template <class T>
void adaptiveQuickselect(T* r, size_t n, size_t length)
{
	trace (n, U" ", length);
	Melder_assert (n < length);
	for (;;)
	{
		// Decide strategy for partitioning
		if (n == 0)
		{
			// That would be the max
			auto pivot = n;
			for (++n; n < length; ++n)
				if (r[n] < r[pivot]) pivot = n;
			std::swap (r[0], r[pivot]);
			return;
		}
		if (n + 1 == length)
		{
			// That would be the min
			auto pivot = 0;
			for (n = 1; n < length; ++n)
				if (r[pivot] < r[n]) pivot = n;
			std::swap (r[pivot], r[length - 1]);
			return;
		}
		Melder_assert (n < length);
		size_t pivot;
		if (length <= 16)
			pivot = pivotPartition(r, n, length) - r;
		else if (n * 6 <= length)
			pivot = medianOfMinima(r, n, length);
		else if (n * 6 >= length * 5)
			pivot = medianOfMaxima(r, n, length);
		else
			pivot = medianOfNinthers(r, length);

		// See how the pivot fares
		if (pivot == n)
		{
			return;
		}
		if (pivot > n)
		{
			length = pivot;
		}
		else
		{
			++pivot;
			r += pivot;
			length -= pivot;
			n -= pivot;
		}
	}
}

#endif /* _median_of_ninthers_h_ */
