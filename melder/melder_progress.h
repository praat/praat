#ifndef _melder_progress_h_
#define _melder_progress_h_
/* melder_progress.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

	Melder_progress (double progress, messageArgs...);

		Function:
			Show the progress of a time-consuming process.
		Arguments:
			Any of 'args' may be null.
		Batch behaviour:
			Does nothing, always returns 1.
		Interactive behaviour:
			Shows the progress of a time-consuming process:
			- if 'progress' <= 0.0, show a window with text and a Cancel button, and return 1;
			- if 0.0 < 'progress' < 1.0, show text and a partially filled progress bar,
			  and return 0 if user interrupts, else return 1;
			- if 'progress' >= 1, hide the window.
		Usage:
			- call with 'progress' = 0.0 before the process starts:
				  (void) Melder_progress (0.0, U"Starting work...");
			- at every turn in your loop, call with 'progress' between 0.0 and 1.0:
				  Melder_progress (i / (n + 1.0), U"Working on part ", i, U" out of ", n, U"...");
			  an exception is thrown if the user clicks Cancel; if you don't want that, catch it:
				  try {
					  Melder_progress (i / (n + 1.0), U"Working on part ", i, U" out of ", n, U"...");
				  } catch (MelderError) {
					  Melder_clearError ();
					  break;
				  }
			- after the process has finished, call with 'progress' = 1.0:
				  (void) Melder_progress (1.0);
			- the first and third steps can be automated by autoMelderProgress:
				  autoMelderProgress progress (U"Starting work...");

	void* Melder_monitor (double progress, messageArgs...);

		Function:
			Show the progress of a time-consuming process.
		Arguments:
			Any of 'args' may be null.
		Batch behaviour:
			Does nothing, returns null if 'progress' <= 0.0 and a non-null pointer otherwise.
		Interactive behaviour:
			Shows the progress of a time-consuming process:
			- if 'progress' <= 0.0, show a window with text and a Cancel button and
			  room for a square drawing, and return a Graphics;
			- if 0.0 < 'progress' < 1.0, show text and a partially filled progress bar,
			  and return nullptr if user interrupts, else return a non-null pointer;
			- if 'progress' >= 1, hide the window.
		Usage:
			- call with 'progress' = 0.0 before the process starts.
			- assign the return value to a Graphics:
				  Graphics graphics = Melder_monitor (0.0, U"Starting work...");
			- at every turn of your loop, draw something in the Graphics:
				  if (graphics) {   // always check; might be batch
					  Graphics_clearWs (graphics);   // only if you redraw all every time
					  Graphics_polyline (graphics, ...);
					  Graphics_text (graphics, ...);
				  }
			- immediately after this in your loop, call with 'progress' between 0.0 and 1.0:
				  Melder_monitor (i / (n + 1.0), U"Working on part ", i, U" out of ", n, U"...");
			  an exception is thrown if the user clicks Cancel; if you don't want that, catch it:
				  try {
					  Melder_monitor (i / (n + 1.0), U"Working on part ", i, U" out of ", n, U"...");
				  } catch (MelderError) {
					  Melder_clearError ();
					  break;
				  }
			- after the process has finished, call with 'progress' = 1.0:
				  (void) Melder_monitor (1.0, nullptr);
			- showing and hiding can be automated by autoMelderMonitor:
				  autoMelderMonitor monitor ("Starting work...");
				  if (monitor.graphics()) {   // always check; might be batch
					  Graphics_clearWs (monitor.graphics());   // only if you redraw all every time
					  Graphics_polyline (monitor.graphics(), ...);
					  Graphics_text (monitor.graphics(), ...);
				  }
*/

namespace MelderProgress {
	extern int _depth;
	using ProgressProc = void (*) (double progress, conststring32 message);
	using MonitorProc = void * (*) (double progress, conststring32 message);
	extern ProgressProc _p_progressProc;
	extern MonitorProc _p_monitorProc;
	void _doProgress (double progress, conststring32 message);
	void * _doMonitor (double progress, conststring32 message);
	extern MelderString _buffer;
}

void Melder_progressOff ();
void Melder_progressOn ();

inline static void Melder_progress (double progress) {
	MelderProgress::_doProgress (progress, U"");
}
template <typename... Args>
void Melder_progress (double progress, const MelderArg& first, Args... rest) {
	MelderString_copy (& MelderProgress::_buffer, first, rest...);
	MelderProgress::_doProgress (progress, MelderProgress::_buffer.string);
}
class autoMelderProgress {
public:
	autoMelderProgress (conststring32 message) {
		Melder_progress (0.0, message);
	}
	~autoMelderProgress () {
		Melder_progress (1.0);
	}
};

inline static void * Melder_monitor (double progress) {
	return MelderProgress::_doMonitor (progress, U"");
}
template <typename... Args>
void * Melder_monitor (double progress, const MelderArg& first, Args... rest) {
	MelderString_copy (& MelderProgress::_buffer, first, rest...);
	return MelderProgress::_doMonitor (progress, MelderProgress::_buffer.string);
}

typedef class structGraphics *Graphics;
class autoMelderMonitor {
	Graphics _graphics;
public:
	autoMelderMonitor (conststring32 message) {
		_graphics = (Graphics) Melder_monitor (0.0, message);
	}
	~autoMelderMonitor () {
		Melder_monitor (1.0);
	}
	Graphics graphics () { return _graphics; }
};

struct autoMelderProgressOff {
	autoMelderProgressOff () { Melder_progressOff (); }
	~autoMelderProgressOff () { Melder_progressOn (); }
};

/* End of file melder_progress.h */
#endif
