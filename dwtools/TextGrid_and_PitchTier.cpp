/* TextGrid_and_PitchTier.cpp
 *
 * Copyright (C) 2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Interpreter.h"
#include "NUM2.h"
#include "Strings_extensions.h"
#include "TextGrid_and_PitchTier.h"
#include "Thing.h"


#define TIME_OFFSET_AS_FRACTION_FROM_START 1
#define TIME_OFFSET_AS_PERCENTAGE_FROM_START 2
#define TIME_OFFSET_AS_SECONDS_FROM_START 3

#define PITCH_VALUE_AS_FREQUENCY 1
#define PITCH_VALUE_AS_FRACTION 2
#define PITCH_VALUE_AS_PERCENTAGE 3
#define PITCH_VALUE_AS_START_AND_SLOPES 4
#define PITCH_VALUE_AS_SLOPES_AND_END 5
#define PITCH_VALUE_AS_MUSIC_NOTE 6
#define PITCH_VALUE_AS_SEMITONES 7

#define PITCH_UNIT_HERTZ 1

#define PITCH_ANCHOR_IS_NOT_USED 1
#define PITCH_ANCHOR_IS_CURRENT 2
#define PITCH_ANCHOR_IS_START 3
#define PITCH_ANCHOR_IS_END 4
#define PITCH_ANCHOR_IS_MEAN_OF_CURVE 5
#define PITCH_ANCHOR_IS_MEAN_OF_POINTS 6
#define PITCH_ANCHOR_IS_MAXIMUM 7
#define PITCH_ANCHOR_IS_MINIMUM 8

static real RealTier_getMinimumValue_interval (RealTier me, real tmin, real tmax) {
	integer imin, imax;
	(void) AnyTier_getWindowPoints ((AnyTier) me, tmin, tmax, & imin, & imax);
	real result = undefined;
	for (long i = imin; i <= imax; i ++) {
		RealPoint point = my points.at [i];
		if (isundef (result) || point -> value < result) {
			result = point -> value;
		}
	}
	return result;
}

static real RealTier_getMaximumValue_interval (RealTier me, real tmin, real tmax) {
	integer imin, imax;
	(void) AnyTier_getWindowPoints ((AnyTier) me, tmin, tmax, & imin, & imax);
	real result = undefined;
	for (long i = imin; i <= imax; i ++) {
		RealPoint point = my points.at [i];
		if (isundef (result) || point -> value > result) {
			result = point -> value;
		}
	}
	return result;
}

static autoPitchTier PitchTier_createFromPoints (double xmin, double xmax, double *times, double *pitches, long numberOfTimes) {
	try {
		autoPitchTier me = PitchTier_create (xmin, xmax);
		for (long i = 1; i <= numberOfTimes; i++) {
			RealTier_addPoint (me.get(), times[i], pitches [i]);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"No PitchTier created from points.");
	} 
}

static double * getTimesFromString (double tmin, double tmax, const char32 *times_string, int time_offset, long *numberOfTimes) {
	autoNUMvector<double> times (NUMstring_to_numbers (times_string, numberOfTimes), 1);
	// translate the "times" to real time
	for (long i = 1; i <= *numberOfTimes; i++) {
		if (time_offset == TIME_OFFSET_AS_FRACTION_FROM_START) {
			times [i] = tmin + times [i] * (tmax - tmin);
		} else if (time_offset == TIME_OFFSET_AS_PERCENTAGE_FROM_START) {
			times [i] = tmin + times [i] * (tmax - tmin) * 0.01;
		} else if (time_offset == TIME_OFFSET_AS_SECONDS_FROM_START) {
			times [i] = tmin + times [i];
		} else {
			// we should not be here
		}
	}
	return times.transfer();
}

// a1, a#1, b1,b#1, ... g#1, a2, b2, a#1,b#2, a a# b c c# d d# e f f# g g#
static double note_to_frequency (const char32 *token, double a4) {
	double base = a4 / 8.0;
	long octave, index;
	const char32 note = *token++, char2 = *token++;
	if (note == U'a' || note == U'A') {
		index = 1;
	} else if (note == U'b' || note == U'B') {
		index = 3;
	} else if (note == U'c' || note == U'C') {
		index = 4;
	} else if (note == U'd' || note == U'D') {
		index = 6;
	} else if (note == U'e' || note == U'E') {
		index = 8;
	} else if (note == U'f' || note == U'F') {
		index = 9;
	} else if (note == U'g' || note == U'G') {
		index = 10;
	} else {
		return undefined;
	}
	char32 char3;
	if (char2 == U'#') {
		index ++;
		char3 = *token++;
	} else {
		char3 = char2;
	}

	if (char3 >= U'0' && char3 <= U'9') {
		octave = char3 - U'0';
	} else {
		return undefined;
	}
	double frequency = base * pow (2, octave - 1.0 + (index - 1.0) / 12.0);
	return frequency;
}

static autoPitchTier PitchTier_extractModifiedInterval (PitchTier me, double tmin, double tmax, const char32 *times_string, int time_offset, const char32 *pitches_string, int pitch_unit, int pitch_as, int pitchAnchor_status) {
	(void) pitch_unit;
	try {
		if (tmin >= tmax) {
			tmin = my xmin; tmax = my xmax;
		}
		
		if (((pitch_as == PITCH_VALUE_AS_FRACTION) || (pitch_as == PITCH_VALUE_AS_PERCENTAGE)) && 
			pitchAnchor_status == PITCH_ANCHOR_IS_NOT_USED) {
			Melder_throw (U"You need to specify an anchor value to calculate ", (pitch_as == PITCH_VALUE_AS_FRACTION ? U"fractions" : U"percentages"), U".");
		}
		
		long numberOfTimes;
		autoNUMvector<double> times (getTimesFromString (tmin, tmax, times_string, time_offset, & numberOfTimes), 1);
		
		autoStrings items = Strings_createAsTokens (pitches_string, U" ");
		long numberOfPitches = items -> numberOfStrings;
		if (numberOfTimes != numberOfPitches) {
			Melder_throw (U"The number of items in the times and the pitches string have to be equal.");
		}
		autoNUMvector<double> pitchesraw (1, numberOfPitches);
		for (long i = 1; i <= numberOfPitches; i++) {
			const char32 *token = items -> strings [i];
			if (pitch_as == PITCH_VALUE_AS_MUSIC_NOTE) {
				pitchesraw [i] = note_to_frequency (token, 440.0);
			} else {
				Interpreter_numericExpression (0, token, & pitchesraw [i]);
			}
		}
		
		// now we have the real times and we can sort them tohether with the pitches
		
		autoNUMvector<double> pitches (1, numberOfTimes);
		NUMvector_copyElements<double> (pitchesraw.peek(), pitches.peek(), 1, numberOfPitches);
		NUMsort2<double, double> (numberOfTimes, times.peek(), pitches.peek());
		double pitchAnchor, pitch;
		for (long i = 1; i <= numberOfTimes; i++) {
			long index = pitch_as != PITCH_VALUE_AS_SLOPES_AND_END ? i : numberOfTimes - i + 1;
			double time = times [index];
			if (pitchAnchor_status == PITCH_ANCHOR_IS_NOT_USED) {
				pitchAnchor = undefined;
			} else if (pitchAnchor_status == PITCH_ANCHOR_IS_CURRENT) {
				pitchAnchor = RealTier_getValueAtTime (me, time);
			} else if (pitchAnchor_status == PITCH_ANCHOR_IS_START) {
				pitchAnchor = i == 1 ? RealTier_getValueAtTime (me, tmin) : pitchAnchor;
			} else if (pitchAnchor_status == PITCH_ANCHOR_IS_END) {
				pitchAnchor = i == 1 ? RealTier_getValueAtTime (me, tmax) : pitchAnchor;
			} else if (pitchAnchor_status == PITCH_ANCHOR_IS_MEAN_OF_CURVE) {
				pitchAnchor = i == 1 ? RealTier_getMean_curve (me, tmin, tmax) : pitchAnchor;
			} else if (pitchAnchor_status == PITCH_ANCHOR_IS_MEAN_OF_POINTS) {
				pitchAnchor = i == 1 ? RealTier_getMean_points (me, tmin, tmax) : pitchAnchor;
			} else if (pitchAnchor_status == PITCH_ANCHOR_IS_MAXIMUM) {
				pitchAnchor = i == 1 ? RealTier_getMaximumValue_interval (me, tmin, tmax) : pitchAnchor;
			} else if (pitchAnchor_status == PITCH_ANCHOR_IS_MINIMUM) {
				pitchAnchor = i == 1 ? RealTier_getMinimumValue_interval (me, tmin, tmax) : pitchAnchor;
			} else {
				// we should not be here
			}
			
			if (isundef (pitchAnchor) && (pitchAnchor_status != PITCH_ANCHOR_IS_NOT_USED)) {
				Melder_throw (U"The pitch anchor value is undefined because the PitchTier is empty.");
			}
			// How to interpret the "pitch" value
			
			if (pitch_as == PITCH_VALUE_AS_FREQUENCY) {
				pitch = pitches [i];
			} else if (pitch_as == PITCH_VALUE_AS_FRACTION) {
				pitch = pitchAnchor * (1.0 + pitches [i]);
			} else if (pitch_as == PITCH_VALUE_AS_PERCENTAGE) {
				pitch = pitchAnchor * (1.0 + pitches [i] * 0.01);
			} else if (pitch_as == PITCH_VALUE_AS_START_AND_SLOPES) {
				if (i == 1) {
					pitch = pitchAnchor;
				} else {
					pitch += (times [i] - times [i - 1]) * pitches [i];
				}
			} else if (pitch_as == PITCH_VALUE_AS_SLOPES_AND_END) {
				if (i == 1) {
					pitch = pitchAnchor;
				} else {
					pitch -= (times [index + 1] - times [index]) * pitches [index];
				}
			} else if (pitch_as == PITCH_VALUE_AS_MUSIC_NOTE) {
				pitch = pitches [i];
			} else if (pitch_as == PITCH_VALUE_AS_SEMITONES) {
				pitch = NUMsemitonesToHertz (pitches [i]);
			} else {
				// we should not be here
			}
			pitches [index] = pitch;
		}
		
		// remove old points
		autoPitchTier thee = PitchTier_createFromPoints (times [1], times [numberOfTimes], times.peek(), pitches.peek(), numberOfTimes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no modified PitchTier created.");
	}
}

static void PitchTiers_replacePoints (PitchTier me, PitchTier thee) {
	AnyTier_removePointsBetween ((AnyTier) me, thy xmin, thy xmax);
	for (long i = 1; i <= thy points.size; i++) {
		RealPoint pp = thy points.at [i];
		RealTier_addPoint (me, pp -> number, pp -> value);
	}
}

void PitchTier_modifyInterval (PitchTier me, double tmin, double tmax, const char32 *times_string, int time_offset, const char32 *pitches_string, int pitch_unit, int pitch_as, int pitchAnchor_status) {
	try {
		autoPitchTier thee = PitchTier_extractModifiedInterval (me, tmin, tmax, times_string, time_offset, pitches_string, pitch_unit, pitch_as, pitchAnchor_status);
		PitchTiers_replacePoints (me, thee.get());
	} catch (MelderError) {
		Melder_throw (me, U": interval modification not completed.");
	}
}


autoPitchTier IntervalTier_and_PitchTier_to_PitchTier (IntervalTier me, PitchTier thee, const char32 *times_string, int time_offset, const char32 *pitches_string, int pitch_unit, int pitch_as, int pitchAnchor_status, int which_Melder_STRING, const char32 *criterion) {
	try {
		autoPitchTier him = Data_copy (thee);
		for (long i = 1; i <= my intervals.size; i++) {
			TextInterval segment = my intervals.at [i];
			if (Melder_stringMatchesCriterion (segment -> text, which_Melder_STRING, criterion)) {
				double xmin = segment -> xmin, xmax = segment -> xmax;
				autoPitchTier modified = PitchTier_extractModifiedInterval (thee, xmin, xmax, times_string, time_offset, pitches_string, pitch_unit, pitch_as, pitchAnchor_status);
				PitchTiers_replacePoints (him.get(), modified.get());
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create PitchTier.");
	}
}

autoPitchTier TextGrid_and_PitchTier_to_PitchTier (TextGrid me, PitchTier thee, long tierNumber, const char32 *times_string, int time_offset, const char32 *pitches_string, int pitch_unit, int pitch_as, int pitchAnchor_status, int which_Melder_STRING, const char32 *criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		return IntervalTier_and_PitchTier_to_PitchTier (tier, thee, times_string, time_offset, pitches_string, pitch_unit, pitch_as, pitchAnchor_status, which_Melder_STRING, criterion);
	} catch (MelderError) {
		Melder_throw (me, U": cannot create PitchTier.");
	}
}

// We specify pitches as tone levels (1 - numberOfToneLevels). These levels are relative to the pitch range of a speaker.
// (normally in Mandarin Chinese they count 5 levels). 
autoPitchTier PitchTier_extractModifiedInterval_toneLevels (PitchTier me, double tmin, double tmax, double fmin, double fmax, long numberOfToneLevels, const char32 *times_string, int time_offset, const char32 *pitches_string) {
	try {
		if (tmin >= tmax) {
			tmin = my xmin; tmax = my xmax;
		}
		if (fmin >= fmax) {
			Melder_throw (U"The lowest frequency must be lower than the highest frequency.");
		}
		long numberOfTimes, numberOfPitches;
		autoNUMvector<double> times (getTimesFromString (tmin, tmax, times_string, time_offset, & numberOfTimes), 1);
		autoNUMvector<double> pitches (NUMstring_to_numbers (pitches_string, & numberOfPitches), 1);
		if (numberOfTimes != numberOfPitches) {
			Melder_throw (U"The number of items in the times and the pitches string have to be equal.");
		}
		double scale = log10 (fmax / fmin) / numberOfToneLevels;
		for (long i = 1; i <= numberOfPitches; i ++) {
			pitches [i] = fmin * pow (10.0, scale * pitches [i]);
		}
		NUMsort2<double, double> (numberOfTimes, times.peek(), pitches.peek());
		autoPitchTier thee = PitchTier_createFromPoints (times [1], times [numberOfTimes], times.peek(), pitches.peek(), numberOfTimes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": interval modification not succeeded.");
	}
}

void PitchTier_modifyInterval_toneLevels (PitchTier me, double tmin, double tmax, double fmin, double fmax, long numberOfToneLevels, const char32 *times_string, int time_offset, const char32 *pitches_string) {
	try {
		autoPitchTier thee = PitchTier_extractModifiedInterval_toneLevels (me, tmin, tmax, fmin, fmax, numberOfToneLevels, times_string, time_offset, pitches_string);
		PitchTiers_replacePoints (me, thee.get());
	} catch (MelderError) {
		Melder_throw (me, U": interval modification as tone levels not succeeded.");
	}	
}

/* End of file TextGrid_and_PitchTier.cpp */
