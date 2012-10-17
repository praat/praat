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

//TODO: SpeechSynthesizer crashes on long input strings
#include "DTW.h"
#include "Sounds_to_DTW.h"
#include "Sound_extensions.h"
#include "SpeechSynthesizer_and_TextGrid.h"
#include "CCs_to_DTW.h"
#include "DTW_and_TextGrid.h"
#include "NUMmachar.h"

// prototypes
void IntervalTier_splitInterval (IntervalTier me, double time, const wchar_t *leftLabel, long interval, double precision);
IntervalTier IntervalTier_and_IntervalTier_cutPartsMatchingLabel (IntervalTier me, IntervalTier thee, const wchar_t *label, double precision);
IntervalTier IntervalTiers_patch_noBoundaries (IntervalTier me, IntervalTier thee, const wchar_t *patchLabel, double precision);
TextGrid SpeechSynthesizer_and_Sound_and_IntervalTier_align2 (SpeechSynthesizer me, Sound thee, IntervalTier him, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration);
Table IntervalTiers_to_Table_textAlignmentment (IntervalTier target, IntervalTier source, EditCostsTable costs);

Sound SpeechSynthesizer_and_TextInterval_to_Sound (SpeechSynthesizer me, TextInterval thee, TextGrid *tg)
{
	try {
		if (thy text == NULL || thy text[0] == '\0') {
			Melder_throw ("No text in TextInterval.");
		}
		autoSound him = SpeechSynthesizer_to_Sound (me, thy text, tg, NULL);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not created from TextInterval.");
	}
}

Sound SpeechSynthesizer_and_TextGrid_to_Sound (SpeechSynthesizer me, TextGrid thee, long tierNumber, long iinterval, TextGrid *tg) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (thee, tierNumber);
		IntervalTier intervalTier = (IntervalTier) thy tiers -> item [tierNumber];
		if (intervalTier -> classInfo != classIntervalTier) {
			Melder_throw ("Tier ", tierNumber, " is not an interval tier.");
		}
		if (iinterval < 1 || iinterval > intervalTier -> intervals -> size) {
			Melder_throw ("Interval ", iinterval, " does not exist on tier ", tierNumber, ".");
		}
		return SpeechSynthesizer_and_TextInterval_to_Sound (me, (TextInterval) intervalTier -> intervals -> item[iinterval], tg);
	} catch (MelderError) {
		Melder_throw ("Sound not created from textGrid.");
	}
}

static double TextGrid_getStartTimeOfFirstOccurence (TextGrid thee, long tierNumber, const wchar_t *label) {
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

static double TextGrid_getEndTimeOfLastOccurence (TextGrid thee, long tierNumber, const wchar_t *label) {
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

static void IntervalTier_getLabelInfo (IntervalTier me, const wchar_t *label, double *labelDurations, long *numberOfOccurences) {
    *labelDurations = 0;
    *numberOfOccurences = 0;
    for (long i = 1; i <= my intervals -> size; i++) {
        TextInterval ti = (TextInterval) my intervals -> item[i];
        if (Melder_wcsequ (ti -> text, label)) {
            *labelDurations += ti -> xmax - ti -> xmin;
            (*numberOfOccurences)++;
        }
    }
}

#define TIMES_ARE_CLOSE(x,y) (fabs((x)-(y)) < precision)
void IntervalTier_splitInterval (IntervalTier me, double time, const wchar_t *leftLabel, long interval, double precision) {
    try {

        long index = 0; TextInterval ti = NULL;
        for (long i = interval; i <= my intervals -> size; i++) { // interval > 0
            ti = (TextInterval) my intervals -> item[i];
            if (time < ti -> xmax + precision && time > ti -> xmin - precision) {
                index = i; break;
            }
        }
        // if index == 0 then search left intervals??
        if (index == 0 || TIMES_ARE_CLOSE(time, ti -> xmin) || TIMES_ARE_CLOSE(time, ti -> xmax)) {
            return;
        }
        autoTextInterval newInterval = TextInterval_create (ti -> xmin, time, leftLabel);
        // Make start of current and begin of new interval equal
        ti -> xmin = time;
        Collection_addItem (my intervals, newInterval.transfer());
    } catch (MelderError) {
        Melder_throw ("Boundary not inserted.");
    }

}

TextTier TextTier_and_IntervalTier_cutPartsMatchingLabel (TextTier me, IntervalTier thee, const wchar_t *label, double precision) {
    try {
        if (my xmin != thy xmin || my xmax != thy xmax) {
            Melder_throw ("Domains must be equal.");
        }
        long myIndex = 1; double timeCut = 0;
        autoTextTier him = TextTier_create (0, my xmax - my xmin);
        for (long j = 1; j <= thy intervals -> size; j++) {
            TextInterval cut = (TextInterval) thy intervals -> item[j];
            if (Melder_wcsequ (cut -> text, label)) {
                timeCut += cut -> xmax - cut -> xmin;
            } else {
                 while (myIndex <= my points -> size) {
                    TextPoint tp = (TextPoint) my points -> item[myIndex];
                    if (tp -> number < cut -> xmin - precision) {
                        // point is left of cut
                        myIndex++;
                    } else if (tp -> number < cut -> xmax + precision) {
                        // point is in (no)cut
                        double time = tp -> number - my xmin - timeCut;
                        TextTier_addPoint (him.peek(), time, tp -> mark);
                        myIndex++;
                    } else {
                        break;
                    }
                 }
            }
        }
        his xmax -= timeCut;
        return him.transfer();
    } catch (MelderError) {
        Melder_throw (me, ": parts not cut.");
    }
}


// Cut parts from me marked by labels in thee
IntervalTier IntervalTier_and_IntervalTier_cutPartsMatchingLabel (IntervalTier me, IntervalTier thee, const wchar_t *label, double precision) {
    try {
        if (my xmin != thy xmin || my xmax != thy xmax) {
            Melder_throw ("Domains must be equal.");
        }
        autoNUMvector<double> durations (1, my intervals -> size);
        for (long i = 1; i <= my intervals -> size; i++) {
            TextInterval ti = (TextInterval) my intervals -> item[i];
            durations[i] = ti -> xmax - ti -> xmin;
        }
        long myInterval = 1;
        for (long j = 1; j <= thy intervals -> size; j++) {
            TextInterval cut = (TextInterval) thy intervals -> item[j];
            if (Melder_wcsequ (cut -> text, label)) { // trim
                while (myInterval <= my intervals -> size) {
                    TextInterval ti = (TextInterval) my intervals -> item[myInterval];
                    if (ti -> xmin > cut -> xmin - precision && ti -> xmax < cut -> xmax + precision) {
                        // 1. interval completely within cut
                        durations[myInterval] = 0;
                        myInterval++;
                    } else if (ti -> xmin < cut -> xmin + precision && cut -> xmin < ti -> xmax + precision) {
                        // 2. cut start is within interval
                        if (cut -> xmax > ti -> xmax - precision) {
                            // interval end is in cut, interval start before
                            durations[myInterval] -= ti -> xmax - cut -> xmin;
                            myInterval++;
                        } else {
                            // 3. cut completely within interval
                            durations[myInterval] -= cut -> xmax - cut -> xmin;
                            break;
                        }
                    } else if (cut -> xmax > ti -> xmin - precision && cut -> xmin < ti -> xmax + precision) {
                        // +1+2 : cut end is within interval, cut start before
                        durations[myInterval] -= cut -> xmax - ti -> xmin;
                        break;
                    } else if (ti -> xmax < cut -> xmin + precision) {
                        myInterval++;
                    }
                }
            }
        }
        double totalDuration = 0;
        for (long i = 1; i <= my intervals -> size; i++) {
            if (durations[i] < precision) {
                durations[i] = 0;
            }
            totalDuration += durations[i];
        }
        autoIntervalTier him = IntervalTier_create (0, totalDuration);
        double time = 0; long hisInterval = 1;
        for (long i = 1; i <= my intervals -> size; i++) {
            if (durations[i] <= 0) continue;
            TextInterval ti = (TextInterval) my intervals -> item[i];
            time += durations[i];
            if (fabs (time - totalDuration) > precision) {
                IntervalTier_splitInterval (him.peek(), time, ti -> text, hisInterval, precision);
                hisInterval++;
            } else { // last interval
                TextInterval histi = (TextInterval) his intervals -> item[hisInterval];
                TextInterval_setText (histi, ti -> text);
            }
        }
        return him.transfer();
    } catch (MelderError) {
        Melder_throw (me, ": parts not cut.");
    }
}

TextGrid TextGrid_and_IntervalTier_cutPartsMatchingLabel (TextGrid me, IntervalTier thee, const wchar_t *label, double precision) {
    try {
        if (my xmin != thy xmin || my xmax != thy xmax) {
            Melder_throw ("Domains must be equal.");
        }
        double cutDurations = 0;
        for (long i = 1; i <= thy intervals -> size; i++) {
            TextInterval cut = (TextInterval) thy intervals -> item[i];
            if (Melder_wcsequ (cut -> text, label)) {
                cutDurations += cut -> xmax - cut -> xmin;
            }
        }
        if (cutDurations <= precision) { // Nothing to patch
            return (TextGrid) Data_copy (me);
        }
        autoTextGrid him = TextGrid_createWithoutTiers (0, thy xmax - thy xmin - cutDurations);
        for (long itier = 1; itier <= my tiers -> size; itier++) {
            Function anyTier = (Function) my tiers -> item[itier];
            if (anyTier -> classInfo == classIntervalTier) {
                autoIntervalTier ait = IntervalTier_and_IntervalTier_cutPartsMatchingLabel ((IntervalTier) anyTier, thee, label, precision);
                Collection_addItem (his tiers, ait.transfer());
            } else {
                autoTextTier att = TextTier_and_IntervalTier_cutPartsMatchingLabel ((TextTier) anyTier, thee, label, precision);
                Collection_addItem (his tiers, att.transfer());
            }
        }
        return him.transfer();
    } catch (MelderError) {
        Melder_throw (me, ": no parts cut.");
    }
}

// Patch thy intervals that match patchLabel into my intervals
// The resulting IntervalTier has thy xmin as starting time and thy xmax as end time
IntervalTier IntervalTiers_patch_noBoundaries (IntervalTier me, IntervalTier thee, const wchar_t *patchLabel, double precision) {
    try {
		autoNUMvector<double> durations (0L, my intervals -> size + 1);
		for (long i = 1; i <= my intervals -> size; i++) {
			TextInterval myti = (TextInterval) my intervals -> item[i];
			durations[i] = myti -> xmax - myti -> xmin;
		}
		long myInterval = 1; double xShift = thy xmin - my xmin;
        for (long j = 1; j <= thy intervals -> size; j++) {
            TextInterval patch = (TextInterval) thy intervals -> item[j];
            if (Melder_wcsequ (patch -> text, patchLabel)) {
				if (j == 1) {
					xShift += durations[0] = patch -> xmax - patch -> xmin;
				} else if (j == thy intervals -> size) {
					durations[my intervals -> size + 1] = patch -> xmax - patch -> xmin;
				} else {
					while (myInterval <= my intervals -> size) {
						TextInterval ti = (TextInterval) my intervals -> item[myInterval];
						double tixmin = ti -> xmin + xShift;
						double tixmax = ti -> xmax + xShift;
						if ((patch -> xmin > tixmin - precision) && (patch -> xmin < tixmax + precision)) {
							durations[myInterval] += patch -> xmax - patch -> xmin;
							break;
						}
						myInterval++;
					}
				}
            } else {
				while (myInterval <= my intervals -> size) {
					TextInterval ti = (TextInterval) my intervals -> item[myInterval];
					double tixmax = ti -> xmax + xShift;
					if (tixmax < patch -> xmin + precision) {
						myInterval++;
					} else {
						break;
					}
				}
			}
        }
        autoIntervalTier him = IntervalTier_create (thy xmin, thy xmax);
        // first interval
		double time = thy xmin + durations[0];
		long hisInterval = 1;
		if (durations[0] > 0) {
			IntervalTier_splitInterval (him.peek(), time , L"", hisInterval, precision);
			hisInterval++;
		}
		for (long i = 1; i <= my intervals -> size; i++) {
			TextInterval ti = (TextInterval) my intervals -> item[i];
			time += durations[i];
			IntervalTier_splitInterval (him.peek(), time, ti -> text, hisInterval, precision);
			hisInterval++;
		}
		if (durations[my intervals -> size + 1] > 0) {
			time += durations[my intervals -> size + 1];
			IntervalTier_splitInterval (him.peek(), time , L"", hisInterval, precision);
		}
        return him.transfer();
    } catch (MelderError) {
        Melder_throw (me, ": not patched.");
    }
}

IntervalTier IntervalTiers_patch (IntervalTier me, IntervalTier thee, const wchar_t *patchLabel, double precision) {
    try {
        autoIntervalTier him = IntervalTier_create (thy xmin, thy xmax);
        long myInterval = 1, hisInterval = 1;
        double xmax = thy xmin;
        for (long i = 1; i <= thy intervals -> size; i++) {
            TextInterval myti, ti = (TextInterval) thy intervals -> item[i];
            if (Melder_wcsequ (ti -> text, patchLabel)) {
                bool splitInterval = false; double endtime, split = 0;
                if (i > 0) {
                    while (myInterval <= my intervals -> size) {
                        myti = (TextInterval) my intervals -> item[myInterval];
                        endtime = xmax + myti -> xmax - myti -> xmin;
                        if (endtime <= ti -> xmin + precision) {
                            xmax = endtime;
                            IntervalTier_splitInterval (him.peek(), xmax, myti -> text, hisInterval, precision);
                            hisInterval++;
                        } else {
                            if (xmax < ti -> xmin - precision) { // split interval ???
                                splitInterval = true;
                                xmax = ti -> xmin;
                                split = endtime - xmax;
                                IntervalTier_splitInterval (him.peek(), xmax, myti -> text, hisInterval, precision);
                                hisInterval ++; myInterval++;
                            }
                            break;
                        }
                        myInterval++;
                    }
                }
                xmax += ti -> xmax - ti -> xmin;
                IntervalTier_splitInterval (him.peek(), xmax, L"", hisInterval, precision);
                hisInterval++;
                if (splitInterval) {
                    xmax += split;
                    IntervalTier_splitInterval (him.peek(), xmax, myti -> text, hisInterval, precision);
                    hisInterval ++;
                }
            } else if (i == thy intervals -> size) { // copy remaining if last interval doesn't match
                while (myInterval <= my intervals -> size) {
                    myti = (TextInterval) my intervals -> item[myInterval];
                    xmax += myti -> xmax - myti -> xmin;
                    IntervalTier_splitInterval (him.peek(), xmax, myti -> text, hisInterval, precision);
                    hisInterval++;
                    myInterval++;
                }
            }
        }
        return him.transfer();
    } catch (MelderError) {
        Melder_throw (me, ": not patched.");
    }
}

TextTier TextTier_and_IntervalTier_patch (TextTier me, IntervalTier thee, const wchar_t *patchLabel, double precision) {
    try {
        long myIndex = 1;
        autoTextTier him = TextTier_create (thy xmin, thy xmax);
        double xShift = thy xmin - my xmin;
        for (long i = 1; i <= thy intervals -> size; i++) {
            TextInterval ti = (TextInterval) thy intervals -> item[i];
            if (Melder_wcsequ (ti -> text, patchLabel)) {
                if (i > 1) {
                    while (myIndex <= my points -> size) {
                        TextPoint tp = (TextPoint) my points -> item[myIndex];
                        double time = tp -> number + xShift;
                        if (time < ti -> xmin + precision) {
                            autoTextPoint atp = TextPoint_create (time, tp -> mark);
                            Collection_addItem (his points, atp.transfer());
                        } else {
                            break;
                        }
                        myIndex++;
                    }
                }
                xShift += ti -> xmax - ti -> xmin;
           } else if (i == thy intervals -> size) {
                while (myIndex <= my points -> size) {
                    TextPoint tp = (TextPoint) my points -> item[myIndex];
                    double time = tp -> number + xShift;
                    if (time < ti -> xmin + precision) {
                        autoTextPoint atp = TextPoint_create (time, tp -> mark);
                        Collection_addItem (his points, atp.transfer());
                    }
                    myIndex++;
                }
            }
        }
        return him.transfer();
    } catch (MelderError) {
        Melder_throw (me, ": cannot patch TextTier.");
    }
}

TextGrid TextGrid_and_IntervalTier_patch (TextGrid me, IntervalTier thee, const wchar_t *patchLabel, double precision) {
    try {
        double patchDurations;
        long numberOfPatches;
        IntervalTier_getLabelInfo (thee, patchLabel, &patchDurations, &numberOfPatches);
        if (patchDurations <= 0 || my xmax - my xmin >= thy xmax - thy xmin ) { // Nothing to patch
            return (TextGrid) Data_copy (me);
        }
        autoTextGrid him = TextGrid_createWithoutTiers (thy xmin, thy xmax);
        for (long itier = 1; itier <= my tiers -> size; itier++) {
            Function anyTier = (Function) my tiers -> item[itier];
            if (anyTier -> classInfo == classIntervalTier) {
//                autoIntervalTier ait = IntervalTiers_patch ((IntervalTier) anyTier, thee, patchLabel, precision);
                autoIntervalTier ait = IntervalTiers_patch_noBoundaries ((IntervalTier) anyTier, thee, patchLabel, precision);
                Collection_addItem (his tiers, ait.transfer());
            } else {
                autoTextTier att = TextTier_and_IntervalTier_patch ((TextTier) anyTier, thee, patchLabel, precision);
                Collection_addItem (his tiers, att.transfer());
            }
        }
        return him.transfer();
    } catch (MelderError) {
        Melder_throw (me, ": not patched.");
    }
}

// We assume that the Sound and the SpeechSynthesizer have the same samplingFrequency
// schakel waarschuwingen over stiltedetectie uit
TextGrid SpeechSynthesizer_and_Sound_and_TextInterval_align (SpeechSynthesizer me, Sound thee, TextInterval him, double silenceThreshold, double minSilenceDuration, double minSoundingDuration) {
	try {
		if (thy xmin != his xmin || thy xmax != his xmax) {
			Melder_throw ("Domains of Sound and TextGrid must be equal.");
		}
		if (fabs (1.0 / thy dx - my d_samplingFrequency) > NUMfpp -> eps) {
			Melder_throw ("The sampling frequencies of the SpeechSynthesizer and the Sound must be equal.");
		}
		long numberOfTokens = Melder_countTokens (his text);
		if (numberOfTokens == 0) {
			Melder_throw ("The interval has no text.");
		}
		// Remove silent intervals from start and end of sounds
		double minPitch = 200, timeStep = 0.005, precision = thy dx;
		double t1_thee, t2_thee;
		autoSound s_thee = Sound_trimSilencesAtStartAndEnd (thee, 0.0, minPitch, timeStep,
			silenceThreshold, minSilenceDuration, minSoundingDuration, &t1_thee, &t2_thee);
		double s_thee_duration = s_thee -> xmax - s_thee -> xmin;
		bool hasSilence_thee = fabs (t1_thee - thy xmin) > precision || fabs (t2_thee - thy xmax) > precision;

		if (my d_estimateWordsPerMinute) {
			// estimate speaking rate with the number of words per minute from the text
			double wordsPerMinute_rawTokens = 60.0 * numberOfTokens / s_thee_duration;
			// compensation for long words: 5 characters / word
			double wordsPerMinute_rawText = 60.0 * (wcslen (his text) / 5.0) / s_thee_duration;
			my d_wordsPerMinute =  0.5 * (wordsPerMinute_rawTokens + wordsPerMinute_rawText);
		}
		TextGrid tg2 = 0;
		autoSound s2 = SpeechSynthesizer_and_TextInterval_to_Sound (me, him, &tg2);
		autoTextGrid atg2 = tg2, s_atg2;
		/*
		 * For the synthesizer the silence threshold has to be < -30 dB, otherwise fricatives will not
		 * be found as sounding! This is ok since silences are almost at zero amplitudes
		 * We also have to decrease the minimum silence and minimum sounding duration to catch, for example,
		 * the final plosive "t" from the word "text"
		 *
		 */
		double s2_silenceThreshold = -40, s2_minSilenceDuration = 0.05, s2_minSoundingDuration = 0.05;
		double t1_s2, t2_s2;
		autoSound s_s2 = Sound_trimSilencesAtStartAndEnd (s2.peek(), 0.0, minPitch, timeStep,
			s2_silenceThreshold, s2_minSilenceDuration, s2_minSoundingDuration, &t1_s2, &t2_s2);
		double s_s2_duration = s_s2 -> xmax - s_s2 -> xmin;
		bool hasSilence_s2 = fabs (t1_s2 - s2 -> xmin) > precision || fabs (t2_s2 - s2 -> xmax) > precision;
		if (hasSilence_s2) {
			s_atg2.reset (TextGrid_extractPart (atg2.peek(), t1_s2, t2_s2, true));
		}
		double analysisWidth = 0.02, dt = 0.005, band = 0.0;
		// compare the durations of the two sounds to get an indication of the slope constraint of the DTW
		double slope = s_thee_duration / s_s2_duration;
		slope = slope > 1 ? slope : 1 / slope;
        int constraint = slope < 1.5 ? 4 : (slope < 2 ? 3 : (slope < 3 ? 2 : 1));
		//autoMFCC m1 = Sound_to_MFCC ((hasSilence_thee ? s_thee.peek() : thee),
		//	numberOfCoefficients, analysisWidth, dt, f1_mel, fmax_mel, df_mel);
		//autoMFCC m2 = Sound_to_MFCC ((hasSilence_s2 ? s_s2.peek() : s2.peek()),
		//	numberOfCoefficients, analysisWidth, dt, f1_mel, fmax_mel, df_mel);
		//double wc = 1, wle = 0, wr = 0, wer = 0, dtr = 0;
		//int matchStart = 1, matchEnd = 1, constraint = 4; // no 1/3 1/2 2/3
		//autoDTW dtw = CCs_to_DTW (m1.peek(), m2.peek(), wc, wle, wr, wer, dtr, matchStart, matchEnd, constraint);
        autoDTW dtw = Sounds_to_DTW ((hasSilence_thee ? s_thee.peek() : thee), (hasSilence_s2 ? s_s2.peek() : s2.peek()), analysisWidth, dt, band, constraint);
		autoTextGrid result = DTW_and_TextGrid_to_TextGrid (dtw.peek(),
			(hasSilence_s2 ? s_atg2.peek() : atg2.peek()), precision);
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
/*
typedef struct structAlignmentOfSoundAndTextStruct {
	double windowLength, timeStep; // analysis
	double f1_mel, fmax_mel, df_mel; // MelFilter
	long numberOfMFCCCoefficients; // MFCC
	double dtw_cepstralWeight, dtw_logEnergyWeight; // MFCC -> DTW
	double dtw_regressionWeight, dtw_regressionlogEnergyWeight;
	double dtw_regressionWindowLength;
	double dtw_sakoeChibaBand, dtw_constraint;
	double silenceThreshold, minSilenceDuration, minSoundingDuration, trimDuration; // silence detection
	long language, voicevariant, pitchAdjustment, pitchRange, wordsPerMinute; // synthesizer
	bool interpretPhonemeCodes, ipa, set_wordsPerMinute;
	double wordgap; // synthesizer
} *SpeechSynthesizer_alignmentStruct;*/

TextGrid SpeechSynthesizer_and_Sound_and_TextInterval_align2 (SpeechSynthesizer me, Sound thee, TextInterval him, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration);
TextGrid SpeechSynthesizer_and_Sound_and_TextInterval_align2 (SpeechSynthesizer me, Sound thee, TextInterval him, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration) {
    try {
        if (thy xmin != his xmin || thy xmax != his xmax) {
            Melder_throw ("Domains of Sound and TextGrid must be equal.");
        }
        if (fabs (1.0 / thy dx - my d_samplingFrequency) > NUMfpp -> eps) {
            Melder_throw ("The sampling frequencies of the SpeechSynthesizer and the Sound must be equal.");
        }
        const wchar_t *trimLabel = L"trim";
        // 1. trim the silences of the sound
        /*
         * For the synthesizer the silence threshold has to be < -30 dB, otherwise fricatives will not
         * be found as sounding! This is ok since silences are almost at zero amplitudes
         * We also have to decrease the minimum silence and minimum sounding duration to catch, for example,
         * the final plosive "t" from the word "text"
         *
         */
        TextGrid tg_tmp = 0;
        double minPitch = 200, timeStep = 0.005, precision = thy dx;
        autoSound thee_trimmed = Sound_trimSilences (thee, trimDuration, false, minPitch, timeStep, silenceThreshold,  minSilenceDuration, minSoundingDuration, &tg_tmp, trimLabel);
        autoTextGrid thee_trimmer = tg_tmp;

        // 2. synthesize the sound from the TextInterval

        autoSound synth = SpeechSynthesizer_and_TextInterval_to_Sound (me, him, &tg_tmp);
        autoTextGrid tg_syn = tg_tmp;

        // 3. There should be no silences in the synthesized sound except at the start and finish.
		//    Set the wordwap parameter to a small value like 0.001 s.

        // 4. Get DTW from the two sounds
        double analysisWidth = 0.02, dt = 0.005, band = 0.0;
        int constraint = 4;
        autoDTW dtw = Sounds_to_DTW (thee_trimmed.peek(), synth.peek(), analysisWidth, dt, band, constraint);

        // 6. Warp the synthesis TextGrid
        // first make domain equal, otherwsise the warper protests

        autoTextGrid warp = DTW_and_TextGrid_to_TextGrid (dtw.peek(), tg_syn.peek(), precision);

        // 7. Patch the trimmed intervals back into the warped TextGrid
        autoTextGrid result = TextGrid_and_IntervalTier_patch (warp.peek(), (IntervalTier) thee_trimmer -> tiers ->item[1], L"trim", 2 * thy dx);

        return result.transfer();
    } catch (MelderError) {
        Melder_throw (thee, ": sound and TextInterval not aligned.");
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
        if (textgrids -> size == 0) {
            Melder_throw ("Nothing could be aligned. Was your IntervalTier empty?");
        }
        autoTextGrid aligned = TextGrids_to_TextGrid_appendContinuous (textgrids.peek(), true);
        return aligned.transfer();
    } catch (MelderError) {
        Melder_throw ("No aligned TextGrid created.");
    }
}

TextGrid SpeechSynthesizer_and_Sound_and_IntervalTier_align2 (SpeechSynthesizer me, Sound thee, IntervalTier him, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration) {
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
                autoTextGrid atg = SpeechSynthesizer_and_Sound_and_TextInterval_align2 (me, sound.peek(), ti, silenceThreshold, minSilenceDuration, minSoundingDuration, trimDuration);
                Collection_addItem (textgrids.peek(), atg.transfer());
            }
        }
        if (textgrids -> size == 0) {
            Melder_throw ("Nothing could be aligned. Was your IntervalTier empty?");
        }
        autoTextGrid aligned = TextGrids_to_TextGrid_appendContinuous (textgrids.peek(), true);
        return aligned.transfer();
    } catch (MelderError) {
        Melder_throw ("No aligned TextGrid created.");
    }
}

TextGrid SpeechSynthesizer_and_Sound_and_TextGrid_align (SpeechSynthesizer me, Sound thee, TextGrid him, long tierNumber, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration) {
	try {//TODO: check not empty tier
		IntervalTier iTier = TextGrid_checkSpecifiedTierIsIntervalTier (him, tierNumber);
		autoTextGrid tg = SpeechSynthesizer_and_Sound_and_IntervalTier_align (me, thee, iTier, istart, iend, silenceThreshold, minSilenceDuration, minSoundingDuration);
		return tg.transfer();
	} catch (MelderError) {
		Melder_throw ("");
	}
}


TextGrid SpeechSynthesizer_and_Sound_and_TextGrid_align2 (SpeechSynthesizer me, Sound thee, TextGrid him, long tierNumber, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration) {
    try {//TODO: check not empty tier
    	IntervalTier iTier = TextGrid_checkSpecifiedTierIsIntervalTier (him, tierNumber);
        autoTextGrid tg = SpeechSynthesizer_and_Sound_and_IntervalTier_align2 (me, thee, iTier, istart, iend, silenceThreshold, minSilenceDuration, minSoundingDuration, trimDuration);
        return tg.transfer();
    } catch (MelderError) {
        Melder_throw ("");
    }
}

static Strings IntervalTier_to_Strings_withOriginData (IntervalTier me, long *from) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector<wchar_t *> (1, my intervals -> size);
		for (long i = 1; i <= my intervals -> size; i++) {
			TextInterval ti = (TextInterval) my intervals -> item[i];
			if (ti -> text != 0 && ti -> text[0] != '\0') {
				thy strings [++(thy numberOfStrings)] = Melder_wcsdup (ti -> text);
				from[thy numberOfStrings] = i;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Strings created.");
	}
}

Table IntervalTiers_to_Table_textAlignmentment (IntervalTier target, IntervalTier source, EditCostsTable costs) {
	try {
		long numberOfTargetIntervals = target -> intervals -> size;
		long numberOfSourceIntervals = source -> intervals -> size;
		autoNUMvector<long> targetOrigin (1, numberOfTargetIntervals);
		autoNUMvector<long> sourceOrigin (1, numberOfSourceIntervals);
		autoStrings targets = IntervalTier_to_Strings_withOriginData (target, targetOrigin.peek());
		autoStrings sources = IntervalTier_to_Strings_withOriginData (source, sourceOrigin.peek());
		autoEditDistanceTable edit = EditDistanceTable_create (targets.peek(), sources.peek());
		if (costs != 0) {
			EditDistanceTable_setEditCosts (edit.peek(), costs);
			EditDistanceTable_findPath (edit.peek(), NULL);
		}
		long pathLength = edit -> d_warpingPath -> d_pathLength;
		autoTable thee = Table_createWithColumnNames (pathLength - 1, L"targetInterval targetText targetStart targetEnd sourceInterval sourceText sourceStart sourceEnd operation");
		for (long i = 2; i <= pathLength; i++) {
			structPairOfInteger p = edit -> d_warpingPath -> d_path[i];
			structPairOfInteger p1 = edit -> d_warpingPath -> d_path[i - 1];
			double targetStart = NUMundefined, targetEnd =  NUMundefined;
			double sourceStart = NUMundefined, sourceEnd =  NUMundefined;
			const wchar_t * targetText = L"", *sourceText = L"";
			long targetInterval = p.y > 1 ? targetOrigin[p.y - 1] : 0;
			long sourceInterval = p.x > 1 ? sourceOrigin[p.x - 1] : 0;
			if (targetInterval > 0) {
				TextInterval ti = (TextInterval) target -> intervals -> item[targetInterval];
				targetStart = ti -> xmin;
				targetEnd =  ti -> xmax;
				targetText = ti -> text;
			}
			if (sourceInterval > 0) {
				TextInterval ti = (TextInterval) source -> intervals -> item[sourceInterval];
				sourceStart = ti -> xmin;
				sourceEnd =  ti -> xmax;
				sourceText = ti -> text;
			}
			long irow = i - 1;
			if (p.y == p1.y) { // deletion
				Table_setNumericValue (thee.peek(), irow, 1, 0);
				Table_setStringValue  (thee.peek(), irow, 2, L"");
				Table_setNumericValue (thee.peek(), irow, 3, NUMundefined);
				Table_setNumericValue (thee.peek(), irow, 4, NUMundefined);
				Table_setNumericValue (thee.peek(), irow, 5, sourceInterval);
				Table_setStringValue  (thee.peek(), irow, 6, sourceText);
				Table_setNumericValue (thee.peek(), irow, 7, sourceStart);
				Table_setNumericValue (thee.peek(), irow, 8, sourceEnd);
				Table_setStringValue  (thee.peek(), irow, 9, L"d");
			} else if (p.x == p1.x) { // insertion
				Table_setNumericValue (thee.peek(), irow, 1, targetInterval);
				Table_setStringValue  (thee.peek(), irow, 2, targetText);
				Table_setNumericValue (thee.peek(), irow, 3, targetStart);
				Table_setNumericValue (thee.peek(), irow, 4, targetEnd);
				Table_setNumericValue (thee.peek(), irow, 5, 0);
				Table_setStringValue  (thee.peek(), irow, 6, L"");
				Table_setNumericValue (thee.peek(), irow, 7, NUMundefined);
				Table_setNumericValue (thee.peek(), irow, 8, NUMundefined);
				Table_setStringValue  (thee.peek(), irow, 9,  L"i");
			} else { // substitution ?
				Table_setNumericValue (thee.peek(), irow, 1, targetInterval);
				Table_setStringValue  (thee.peek(), irow, 2, targetText);
				Table_setNumericValue (thee.peek(), irow, 3, targetStart);
				Table_setNumericValue (thee.peek(), irow, 4, targetEnd);
				Table_setNumericValue (thee.peek(), irow, 5, sourceInterval);
				Table_setStringValue  (thee.peek(), irow, 6, sourceText);
				Table_setNumericValue (thee.peek(), irow, 7, sourceStart);
				Table_setNumericValue (thee.peek(), irow, 8, sourceEnd);
				Table_setStringValue  (thee.peek(), irow, 9, Melder_wcsequ (targetText, sourceText) ? L" " : L"s");
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (target, " and ", source, " not aligned.");
	}
}

Table TextGrids_to_Table_textAlignmentment (TextGrid target, long ttier, TextGrid source, long stier, EditCostsTable costs) {
	try {
		IntervalTier targetTier = TextGrid_checkSpecifiedTierIsIntervalTier (target, ttier);
		IntervalTier sourceTier = TextGrid_checkSpecifiedTierIsIntervalTier (source, stier);
		return IntervalTiers_to_Table_textAlignmentment (targetTier, sourceTier, costs);
	} catch (MelderError) {
		Melder_throw (L"No text alignment table created from TextGrids ", target, " and ", source, L".");
	}
}

// End of file TextGrid_and_SpeechSynthesizer.cpp