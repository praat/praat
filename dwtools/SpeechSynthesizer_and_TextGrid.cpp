/* SpeechSynthesizer_and_TextGrid.cpp
 *
 * Copyright (C) 2011-2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
	djmw 20111214
*/

#include "DTW.h"
#include "Sounds_to_DTW.h"
#include "Sound_extensions.h"
#include "SpeechSynthesizer_and_TextGrid.h"
#include "CCs_to_DTW.h"
#include "DTW_and_TextGrid.h"
#include "NUMmachar.h"

Sound SpeechSynthesizer_and_TextInterval_to_Sound (SpeechSynthesizer me, TextInterval thee, bool isPhonemeTier, TextGrid *tg)
{
	try {
		if (thy text == NULL || thy text[0] == '\0') {
			Melder_throw ("No text in TextInterval.");
		}
		autoMelderString text;
		bool interpretPhonemeCodes = false;
		if (isPhonemeTier) {
			interpretPhonemeCodes = true;
			MelderString_append (&text, L"[[ ", thy text, L" ]]");
		}
		autoSound him = SpeechSynthesizer_to_Sound_special (me, (isPhonemeTier ? text.string : thy text),
			my d_wordgap, my d_pitchAdjustment, my d_pitchRange, my d_wordsPerMinute, my d_interpretSSML,
			interpretPhonemeCodes, my d_ipa, tg, NULL);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not created from TextInterval.");
	}
}

Sound SpeechSynthesizer_and_TextGrid_to_Sound (SpeechSynthesizer me, TextGrid thee, long tierNumber,
	long iinterval, bool isPhonemeTier, TextGrid *tg) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (thee, tierNumber);
		IntervalTier intervalTier = (IntervalTier) thy tiers -> item [tierNumber];
		if (intervalTier -> classInfo != classIntervalTier) {
			Melder_throw ("Tier ", tierNumber, " is not an interval tier.");
		}
		if (iinterval < 1 || iinterval > intervalTier -> intervals -> size) {
			Melder_throw ("Interval ", iinterval, " does not exist on tier ", tierNumber, ".");
		}
		return SpeechSynthesizer_and_TextInterval_to_Sound (me, (TextInterval) intervalTier -> intervals -> item[iinterval], isPhonemeTier, tg);
	} catch (MelderError) {
		Melder_throw ("Sound not created from textGrid.");
	}
}

static double TextGrid_getStartTimeOfFirstOccurence (TextGrid thee, long tierNumber, wchar_t *label) {
	TextGrid_checkSpecifiedTierNumberWithinRange (thee, tierNumber);
	IntervalTier intervalTier = (IntervalTier) thy tiers -> item [tierNumber];
	if (intervalTier -> classInfo != classIntervalTier) {
			Melder_throw ("Tier ", tierNumber, " is not an interval tier.");
	}
	double start = NUMundefined;
	for (long iint = 1; iint <= intervalTier -> intervals -> size; iint++) {
		TextInterval ti = (TextInterval) intervalTier -> intervals -> item[iint];
		if (Melder_wcscmp (ti -> text, label) == 0) {
			start = ti -> xmin;
			break;
		}
	}
	return start;
}

static double TextGrid_getEndTimeOfLastOccurence (TextGrid thee, long tierNumber, wchar_t *label) {
	TextGrid_checkSpecifiedTierNumberWithinRange (thee, tierNumber);
	IntervalTier intervalTier = (IntervalTier) thy tiers -> item [tierNumber];
	if (intervalTier -> classInfo != classIntervalTier) {
			Melder_throw ("Tier ", tierNumber, " is not an interval tier.");
	}
	double end = NUMundefined;
	for (long iint = intervalTier -> intervals -> size; iint > 0; iint--) {
		TextInterval ti = (TextInterval) intervalTier -> intervals -> item[iint];
		if (Melder_wcscmp (ti -> text, label) == 0) {
			end = ti -> xmax;
			break;
		}
	}
	return end;
}

// We assume that the Sound and the SpeechSynthesizer have the same samplingFrequency, say 16000 or lower
TextGrid SpeechSynthesizer_and_Sound_and_TextInterval_align (SpeechSynthesizer me, Sound thee, TextInterval him, double silenceThreshold, double minSilenceDuration, double minSoundingDuration) {
	try {
		if (thy xmin != his xmin || thy xmax != his xmax) {
			Melder_throw ("Domains of Sound and TextGrid must be equal.");
		}
		if (fabs (1.0 / thy dx - my d_samplingFrequency) > NUMfpp -> eps) {
			Melder_throw ("The sampling frequencies of the SpeechSynthesizer and the Sound must be equal.");
		}
		TextGrid tg2 = 0;
		autoSound s2 = SpeechSynthesizer_and_TextInterval_to_Sound (me, him, false, &tg2);
		autoTextGrid atg2 = tg2, s_atg2;
		// Remove silent intervals from start and end of sounds
		double minPitch = 200, timeStep = 0.005;
		bool atStart = true, atEnd = true;
		double t1_thee, t2_thee;
		autoSound s_thee = Sound_trimSilences (thee, atStart, atEnd, minPitch, timeStep,
			silenceThreshold, minSilenceDuration, minSoundingDuration, &t1_thee, &t2_thee);
		bool hasSilence_thee = t1_thee > thy xmin || t2_thee < thy xmax;
		/*
		 * For the synthesizer the silence threshold has to be < -30 dB, otherwise fricatives will not
		 * be found as sounding! This is ok since silences are almost at zero amplitudes
		 * We also have to decrease the minimum silence and minimum sounding duration to catch, for example,
		 * the final plosive "t" from the word "text"
		 *
		 */
		double s2_silenceThreshold = -40, s2_minSilenceDuration = 0.05, s2_minSoundingDuration = 0.05;
		double t1_s2, t2_s2;
		autoSound s_s2 = Sound_trimSilences (s2.peek(), atStart, atEnd, minPitch, timeStep,
			s2_silenceThreshold, s2_minSilenceDuration, s2_minSoundingDuration, &t1_s2, &t2_s2);
		bool hasSilence_s2 = t1_s2 > s2 -> xmin || t2_s2 < s2 -> xmax;
		if (hasSilence_s2) {
			s_atg2.reset (TextGrid_extractPart (atg2.peek(), t1_s2, t2_s2, true));
		}
		double analysisWidth = 0.02, dt = 0.005, band = 0.0;
        int constraint = 4;
		//autoMFCC m1 = Sound_to_MFCC ((hasSilence_thee ? s_thee.peek() : thee),
		//	numberOfCoefficients, analysisWidth, dt, f1_mel, fmax_mel, df_mel);
		//autoMFCC m2 = Sound_to_MFCC ((hasSilence_s2 ? s_s2.peek() : s2.peek()),
		//	numberOfCoefficients, analysisWidth, dt, f1_mel, fmax_mel, df_mel);
		//double wc = 1, wle = 0, wr = 0, wer = 0, dtr = 0;
		//int matchStart = 1, matchEnd = 1, constraint = 4; // no 1/3 1/2 2/3
		//autoDTW dtw = CCs_to_DTW (m1.peek(), m2.peek(), wc, wle, wr, wer, dtr, matchStart, matchEnd, constraint);
        autoDTW dtw = Sounds_to_DTW ((hasSilence_thee ? s_thee.peek() : thee), (hasSilence_s2 ? s_s2.peek() : s2.peek()), analysisWidth, dt, band, constraint);
		autoTextGrid result = DTW_and_TextGrid_to_TextGrid (dtw.peek(),
			(hasSilence_s2 ? s_atg2.peek() : atg2.peek()));
		if (hasSilence_thee) {
			if (t1_thee > thy xmin) {
				TextGrid_setEarlierStartTime (result.peek(), thy xmin, L"", L"");
			}
			if (t2_thee < thy xmax) {
				TextGrid_setLaterEndTime (result.peek(), thy xmax, L"", L"");
			}
		}
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound and TextInterval not aligned.");
	}
}

TextGrid SpeechSynthesizer_and_Sound_and_IntervalTier_align (SpeechSynthesizer me, Sound thee, IntervalTier him, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration) {
	try {
		if (istart < 1 || iend < istart || iend > his intervals -> size) {
			Melder_throw ("Not avalid interval range.");
		}
		autoCollection textgrids = Ordered_create ();
		TextInterval tb = (TextInterval) his intervals -> item[istart];
		TextInterval te = (TextInterval) his intervals -> item[iend];
		autoTextGrid result = TextGrid_create (tb -> xmin, te -> xmax, L"sentence clause word phoneme", L"");
		for (long iint = istart; iint <= iend; iint ++) {
			TextInterval ti = (TextInterval) his intervals -> item[iint];
			if (ti -> text != NULL && wcslen (ti -> text) > 0) {
				autoSound sound = Sound_extractPart (thee, ti -> xmin, ti -> xmax,  kSound_windowShape_RECTANGULAR, 1, true);
				autoTextGrid atg = SpeechSynthesizer_and_Sound_and_TextInterval_align (me, sound.peek(), ti, silenceThreshold, minSilenceDuration, minSoundingDuration);
				Collection_addItem (textgrids.peek(), atg.transfer());
			}
		}
		autoTextGrid aligned = TextGrids_to_TextGrid_appendContinuous (textgrids.peek(), true);
		return aligned.transfer();
	} catch (MelderError) {
		Melder_throw ("No aligned TextGrid created.");
	}
}

TextGrid SpeechSynthesizer_and_Sound_and_TextGrid_align (SpeechSynthesizer me, Sound thee, TextGrid him, long tierNumber, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (him, tierNumber);
		Function iTier = (Function) his tiers -> item [tierNumber];
		if (iTier -> classInfo != classIntervalTier) {
			Melder_throw ("Tier is not an IntervalTier.");
		}
		autoTextGrid tg = SpeechSynthesizer_and_Sound_and_IntervalTier_align (me, thee, (IntervalTier) iTier, istart, iend, silenceThreshold, minSilenceDuration, minSoundingDuration);
		return tg.transfer();
	} catch (MelderError) {
		Melder_throw ("");
	}
}

// End of file TextGrid_and_SpeechSynthesizer.cpp