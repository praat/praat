#ifndef _melder_atof_h_
#define _melder_atof_h_
/* melder_atof.h
 *
 * Copyright (C) 1992-2018,2021 Paul Boersma
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

/**
	Return true if the string contains no more than a floating-point number
	optionally surrounded by whitespace.
*/
bool Melder_isStringNumeric (conststring32 string);

/**
	Turn a string into a floating-point number.
	Examples:
		"3.14e-3" -> 3.14e-3
		"15.6%" -> 0.156
		"fghfghj" -> undefined
*/
double Melder_a8tof (conststring8 string);
double Melder_atof (conststring32 string);

/*
	Turn a string into an integer.
*/
int64 Melder_atoi (conststring32 string);

/* End of file melder_atof.h */
#endif
