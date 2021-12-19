#ifndef _melder_debug_h_
#define _melder_debug_h_
/* melder_debug.h
 *
 * Copyright (C) 1992-2018,2020,2021 Paul Boersma
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

inline integer Melder_debug = 0;

void Melder_tracingToFile (MelderFile file);
void Melder_setTracing (bool tracing);
inline bool Melder_isTracingGlobally = false;
inline bool Melder_isTracingLocally = false;

namespace MelderTrace {
	inline structMelderFile _file { };
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
	if (MelderFile_isNull (& MelderTrace::_file))
		return;
	FILE *f = MelderTrace::_open (sourceCodeFileName, lineNumber, functionName);
	_recursiveTemplate_Melder_trace (f, first, rest...);
	MelderTrace::_close (f);
}

#define TRACE  constexpr bool Melder_isTracingLocally = true;   // intentionally shadow a global variable; meant for temporary tracing of a function (to stderr)
#define TRACEF  constexpr bool Melder_isTracingGlobally = true;   // for temporary tracing of a function (to the tracing file, even if global tracing is off)

#ifdef NDEBUG
	#define trace(...)   ((void) 0)
#else
	#define trace(...)   ( \
		Melder_isTracingGlobally ? Melder_trace (__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__) : \
		Melder_isTracingLocally ? Melder_casual (U"" __FILE__ " ", __LINE__, U" ", Melder_peek8to32 (__FUNCTION__), U": ", __VA_ARGS__) : \
		(void) 0 \
	)
#endif

/* End of file melder_debug.h */
#endif
