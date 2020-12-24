#ifndef _melder_debug_h_
#define _melder_debug_h_
/* melder_debug.h
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

/*
	SYNOPSIS

	trace (arg1, arg2, arg3...);

	extern int Melder_debug;
*/

extern int Melder_debug;


void Melder_tracingToFile (MelderFile file);
void Melder_setTracing (bool tracing);
extern bool Melder_isTracing;

namespace MelderTrace {
	extern structMelderFile _file;
	FILE * _open (conststring8 sourceCodeFileName, int lineNumber, conststring8 functionName);
	void _close (FILE *f);
	conststring8  _peek32to8  (conststring32 string);
	conststring16 _peek32to16 (conststring32 string);
}

inline void _recursiveTemplate_Melder_trace (FILE *f, const MelderArg& arg) {
	if (arg._arg)
		fprintf (f, "%s", MelderTrace::_peek32to8 (arg. _arg));
}
template <typename... Args>
void _recursiveTemplate_Melder_trace (FILE *f, const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_trace (f, first);
	_recursiveTemplate_Melder_trace (f, rest...);
}

template <typename... Args>
void Melder_trace (conststring8 sourceCodeFileName, int lineNumber, conststring8 functionName, const MelderArg& first, Args... rest) {
	if (! Melder_isTracing || MelderFile_isNull (& MelderTrace::_file))
		return;
	FILE *f = MelderTrace::_open (sourceCodeFileName, lineNumber, functionName);
	_recursiveTemplate_Melder_trace (f, first, rest...);
	MelderTrace::_close (f);
}

#ifdef NDEBUG
	#define trace(...)   ((void) 0)
#else
	#define trace(...)   (! Melder_isTracing ? (void) 0 : Melder_trace (__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#endif

/* End of file melder_debug.h */
#endif
