/* melder_quantity.cpp
 *
 * Copyright (C) 2007-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2007/03/17 created
 * pb 2007/08/14 wchar_t
 * pb 2008/03/07 typo
 * pb 2011/04/05 C++
 */

#include "melder.h"

static const wchar_t * texts [1+MelderQuantity_NUMBER_OF_QUANTITIES] [4] = {
	{ L"", L"", L"", L"" },
	{ L"Time", L"Time (s)", L"seconds", L"s" },
	{ L"Frequency", L"Frequency (Hz)", L"Hertz", L"Hz" },
	{ L"Frequency", L"Frequency (Bark)", L"Bark", L"Bark" },
	{ L"Distance from glottis", L"Distance from glottis (m)", L"metres", L"m" },	
};

const wchar_t * MelderQuantity_getText (int quantity) { return texts [quantity] [0]; }
const wchar_t * MelderQuantity_getWithUnitText (int quantity) { return texts [quantity] [1]; }
const wchar_t * MelderQuantity_getLongUnitText (int quantity) { return texts [quantity] [2]; }
const wchar_t * MelderQuantity_getShortUnitText (int quantity) { return texts [quantity] [3]; }

/* End of file melder_quantity.cpp */
