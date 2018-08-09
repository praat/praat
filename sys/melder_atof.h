#ifndef _melder_atof_h_
#define _melder_atof_h_
// melder_atof.h
// Copyright 1992-2018 Paul Boersma
// This code is distributed under GPL 3 or later (see ../LICENSE.TXT).

/// Return true if the string contains no more than a floating-point number
/// optionally surrounded by whitespace.
bool Melder_isStringNumeric (conststring32 string) noexcept;

// Turn a string into a floating-point number. Examples:
//		"3.14e-3" -> 3.14e-3
//		"15.6%" -> 0.156
//		"fghfghj" -> undefined
double Melder_a8tof (conststring8 string) noexcept;
double Melder_atof (conststring32 string) noexcept;

// Turn a string into an integer.
int64 Melder_atoi (conststring32 string) noexcept;

// End of file melder_atof.h
#endif
