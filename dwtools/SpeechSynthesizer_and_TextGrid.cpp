/* SpeechSynthesizer_and_TextGrid.cpp
 *
 * Copyright (C) 2011-2017 David Weenink
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

// prototypes
static void IntervalTier_splitInterval (IntervalTier me, double time, const char32 *leftLabel, integer interval, double precision);
static autoIntervalTier IntervalTier_IntervalTier_cutPartsMatchingLabel (IntervalTier me, IntervalTier thee, const char32 *label, double precision);
static autoIntervalTier IntervalTiers_patch_noBoundaries (IntervalTier me, IntervalTier thee, const char32 *patchLabel, double precision);
static autoTable IntervalTiers_to_Table_textAlignmentment (IntervalTier target, IntervalTier source, EditCostsTable costs);

autoSound SpeechSynthesizer_TextInterval_to_Sound (SpeechSynthesizer me, TextInterval thee, autoTextGrid *p_tg)
{
	try {
		Melder_require (thy text && thy text[0] != U'\0', U"TextInterval should not be empty.");
		
		autoSound him = SpeechSynthesizer_to_Sound (me, thy text, p_tg, nullptr);
		return him;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from TextInterval.");
	}
}

autoSound SpeechSynthesizer_TextGrid_to_Sound (SpeechSynthesizer me, TextGrid thee, integer tierNumber, integer iinterval, autoTextGrid *p_tg) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (thee, tierNumber);
		IntervalTier intervalTier = (IntervalTier) thy tiers->at [tierNumber];
		Melder_require (intervalTier -> classInfo == classIntervalTier, 
			U"Tier ", tierNumber, U" is not an interval tier.");
		Melder_require (iinterval > 0 && iinterval <= intervalTier -> intervals.size, 
			U"Interval ", iinterval, U" does not exist on tier ", tierNumber, U".");
		return SpeechSynthesizer_TextInterval_to_Sound (me, intervalTier -> intervals.at [iinterval], p_tg);
	} catch (MelderError) {
		Melder_throw (U"Sound not created from textGrid.");
	}
}

#if 0
static double TextGrid_getStartTimeOfFirstOccurence (TextGrid thee, integer tierNumber, const char32 *label) {
	TextGrid_checkSpecifiedTierNumberWithinRange (thee, tierNumber);
	IntervalTier intervalTier = (IntervalTier) thy tiers->at [tierNumber];
	if (intervalTier -> classInfo != classIntervalTier) {
		Melder_throw (U"Tier ", tierNumber, U" is not an interval tier.");
	}
	double start = undefined;
	for (integer iint = 1; iint <= intervalTier -> intervals.size; iint ++) {
		TextInterval ti = intervalTier -> intervals.at [iint];
		if (Melder_cmp (ti -> text, label) == 0) {
			start = ti -> xmin;
			break;
		}
	}
	return start;
}

static double TextGrid_getEndTimeOfLastOccurence (TextGrid thee, integer tierNumber, const char32 *label) {
	TextGrid_checkSpecifiedTierNumberWithinRange (thee, tierNumber);
	IntervalTier intervalTier = (IntervalTier) thy tiers->at [tierNumber];
	if (intervalTier -> classInfo != classIntervalTier) {
		Melder_throw (U"Tier ", tierNumber, U" is not an interval tier.");
	}
	double end = undefined;
	for (integer iint = intervalTier -> intervals.size; iint > 0; iint --) {
		TextInterval ti = intervalTier -> intervals.at [iint];
		if (Melder_equ (ti -> text, label)) {
			end = ti -> xmax;
			break;
		}
	}
	return end;
}
#endif

static void IntervalTier_getLabelInfo (IntervalTier me, const char32 *label, double *labelDurations, integer *numberOfOccurences) {
    *labelDurations = 0;
    *numberOfOccurences = 0;
    for (integer i = 1; i <= my intervals.size; i ++) {
        TextInterval ti = my intervals.at [i];
        if (Melder_equ (ti -> text, label)) {
            *labelDurations += ti -> xmax - ti -> xmin;
            (*numberOfOccurences)++;
        }
    }
}

#define TIMES_ARE_CLOSE(x,y) (fabs((x)-(y)) < precision)
void IntervalTier_splitInterval (IntervalTier me, double time, const char32 *leftLabel, integer interval, double precision) {
    try {
		Melder_assert (interval > 0);
        TextInterval ti = nullptr;
		integer index = 0; 
        for (integer i = interval; i <= my intervals.size; i ++) {
            ti = my intervals.at [i];
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
        my intervals. addItem_move (newInterval.move());
    } catch (MelderError) {
        Melder_throw (U"Boundary not inserted.");
    }

}

static autoTextTier TextTier_IntervalTier_cutPartsMatchingLabel (TextTier me, IntervalTier thee, const char32 *label, double precision) {
    try {
        if (my xmin != thy xmin || my xmax != thy xmax) {
            Melder_throw (U"Domains should be equal.");
        }
        integer myIndex = 1;
		double timeCut = 0.0;
        autoTextTier him = TextTier_create (0.0, my xmax - my xmin);
        for (integer j = 1; j <= thy intervals.size; j ++) {
            TextInterval cut = thy intervals.at [j];
            if (Melder_equ (cut -> text, label)) {
                timeCut += cut -> xmax - cut -> xmin;
            } else {
                 while (myIndex <= my points.size) {
                    TextPoint tp = my points.at [myIndex];
                    if (tp -> number < cut -> xmin - precision) {
                        // point is left of cut
                        myIndex++;
                    } else if (tp -> number < cut -> xmax + precision) {
                        // point is in (no)cut
                        double time = tp -> number - my xmin - timeCut;
                        TextTier_addPoint (him.get(), time, tp -> mark);
                        myIndex++;
                    } else {
                        break;
                    }
                 }
            }
        }
        his xmax -= timeCut;
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": parts not cut.");
    }
}

// Cut parts from me marked by labels in thee
autoIntervalTier IntervalTier_IntervalTier_cutPartsMatchingLabel (IntervalTier me, IntervalTier thee, const char32 *label, double precision) {
    try {
        if (my xmin != thy xmin || my xmax != thy xmax) {
            Melder_throw (U"Domains should be equal.");
        }
        autoNUMvector<double> durations (1, my intervals.size);
        for (integer i = 1; i <= my intervals.size; i ++) {
            TextInterval ti = my intervals.at [i];
            durations[i] = ti -> xmax - ti -> xmin;
        }
        integer myInterval = 1;
        for (integer j = 1; j <= thy intervals.size; j ++) {
            TextInterval cut = thy intervals.at [j];
            if (Melder_equ (cut -> text, label)) { // trim
                while (myInterval <= my intervals.size) {
                    TextInterval ti = my intervals.at [myInterval];
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
        for (integer i = 1; i <= my intervals.size; i ++) {
            if (durations[i] < precision) {
                durations[i] = 0;
            }
            totalDuration += durations[i];
        }
        autoIntervalTier him = IntervalTier_create (0, totalDuration);
        double time = 0; integer hisInterval = 1;
        for (integer i = 1; i <= my intervals.size; i ++) {
            if (durations[i] <= 0) continue;
            TextInterval ti = my intervals.at [i];
            time += durations[i];
            if (fabs (time - totalDuration) > precision) {
                IntervalTier_splitInterval (him.get(), time, ti -> text, hisInterval, precision);
                hisInterval++;
            } else { // last interval
                TextInterval histi = his intervals.at [hisInterval];
                TextInterval_setText (histi, ti -> text);
            }
        }
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": parts not cut.");
    }
}

autoTextGrid TextGrid_IntervalTier_cutPartsMatchingLabel (TextGrid me, IntervalTier thee, const char32 *label, double precision) {
    try {
        if (my xmin != thy xmin || my xmax != thy xmax) {
            Melder_throw (U"Domains should be equal.");
        }
        double cutDurations = 0;
        for (integer i = 1; i <= thy intervals.size; i ++) {
            TextInterval cut = thy intervals.at [i];
            if (Melder_equ (cut -> text, label)) {
                cutDurations += cut -> xmax - cut -> xmin;
            }
        }
        if (cutDurations <= precision) { // Nothing to patch
            return Data_copy (me);
        }
        autoTextGrid him = TextGrid_createWithoutTiers (0, thy xmax - thy xmin - cutDurations);
        for (integer itier = 1; itier <= my tiers->size; itier ++) {
            Function anyTier = my tiers->at [itier];
            if (anyTier -> classInfo == classIntervalTier) {
                autoIntervalTier newTier = IntervalTier_IntervalTier_cutPartsMatchingLabel ((IntervalTier) anyTier, thee, label, precision);
                his tiers -> addItem_move (newTier.move());
            } else {
                autoTextTier newTier = TextTier_IntervalTier_cutPartsMatchingLabel ((TextTier) anyTier, thee, label, precision);
                his tiers -> addItem_move (newTier.move());
            }
        }
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": no parts cut.");
    }
}

// Patch thy intervals that match patchLabel into my intervals
// The resulting IntervalTier has thy xmin as starting time and thy xmax as end time
autoIntervalTier IntervalTiers_patch_noBoundaries (IntervalTier me, IntervalTier thee, const char32 *patchLabel, double precision) {
    try {
		autoNUMvector <double> durations ((integer) 0, my intervals.size + 1);
		for (integer i = 1; i <= my intervals.size; i ++) {
			TextInterval myti = my intervals.at [i];
			durations [i] = myti -> xmax - myti -> xmin;
		}
		integer myInterval = 1;
		double xShift = thy xmin - my xmin;
        for (integer j = 1; j <= thy intervals.size; j ++) {
            TextInterval patch = thy intervals.at [j];
            if (Melder_equ (patch -> text, patchLabel)) {
				if (j == 1) {
					xShift += durations[0] = patch -> xmax - patch -> xmin;
				} else if (j == thy intervals.size) {
					durations [my intervals.size + 1] = patch -> xmax - patch -> xmin;
				} else {
					while (myInterval <= my intervals.size) {
						TextInterval ti = my intervals.at [myInterval];
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
				while (myInterval <= my intervals.size) {
					TextInterval ti = my intervals.at [myInterval];
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
		integer hisInterval = 1;
		if (durations [0] > 0) {
			IntervalTier_splitInterval (him.get(), time , U"", hisInterval, precision);
			hisInterval++;
		}
		for (integer i = 1; i <= my intervals.size; i ++) {
			TextInterval ti = my intervals.at [i];
			time += durations [i];
			IntervalTier_splitInterval (him.get(), time, ti -> text, hisInterval, precision);
			hisInterval++;
		}
		if (durations [my intervals.size + 1] > 0) {
			time += durations [my intervals.size + 1];
			IntervalTier_splitInterval (him.get(), time , U"", hisInterval, precision);
		}
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": not patched.");
    }
}

#if 0
static autoIntervalTier IntervalTiers_patch (IntervalTier me, IntervalTier thee, const char32 *patchLabel, double precision) {
    try {
        autoIntervalTier him = IntervalTier_create (thy xmin, thy xmax);
        integer myInterval = 1, hisInterval = 1;
        double xmax = thy xmin;
        for (integer i = 1; i <= thy intervals.size; i ++) {
            TextInterval myti, ti = thy intervals.at [i];
            if (Melder_equ (ti -> text, patchLabel)) {
                bool splitInterval = false; double endtime, split = 0;
                if (i > 0) {
                    while (myInterval <= my intervals.size) {
                        myti = my intervals.at [myInterval];
                        endtime = xmax + myti -> xmax - myti -> xmin;
                        if (endtime <= ti -> xmin + precision) {
                            xmax = endtime;
                            IntervalTier_splitInterval (him.get(), xmax, myti -> text, hisInterval, precision);
                            hisInterval++;
                        } else {
                            if (xmax < ti -> xmin - precision) { // split interval ???
                                splitInterval = true;
                                xmax = ti -> xmin;
                                split = endtime - xmax;
                                IntervalTier_splitInterval (him.get(), xmax, myti -> text, hisInterval, precision);
                                hisInterval ++; myInterval++;
                            }
                            break;
                        }
                        myInterval++;
                    }
                }
                xmax += ti -> xmax - ti -> xmin;
                IntervalTier_splitInterval (him.get(), xmax, U"", hisInterval, precision);
                hisInterval++;
                if (splitInterval) {
                    xmax += split;
                    IntervalTier_splitInterval (him.get(), xmax, myti -> text, hisInterval, precision);
                    hisInterval ++;
                }
            } else if (i == thy intervals.size) { // copy remaining if last interval doesn't match
                while (myInterval <= my intervals.size) {
                    myti = my intervals.at [myInterval];
                    xmax += myti -> xmax - myti -> xmin;
                    IntervalTier_splitInterval (him.get(), xmax, myti -> text, hisInterval, precision);
                    hisInterval++;
                    myInterval++;
                }
            }
        }
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": not patched.");
    }
}
#endif

static autoTextTier TextTier_IntervalTier_patch (TextTier me, IntervalTier thee, const char32 *patchLabel, double precision) {
    try {
        integer myIndex = 1;
        autoTextTier him = TextTier_create (thy xmin, thy xmax);
        double xShift = thy xmin - my xmin;
        for (integer i = 1; i <= thy intervals.size; i ++) {
            TextInterval ti = thy intervals.at [i];
            if (Melder_equ (ti -> text, patchLabel)) {
                if (i > 1) {
                    while (myIndex <= my points.size) {
                        TextPoint tp = my points.at [myIndex];
                        double time = tp -> number + xShift;
                        if (time < ti -> xmin + precision) {
                            autoTextPoint newPoint = TextPoint_create (time, tp -> mark);
                            his points. addItem_move (newPoint.move());
                        } else {
                            break;
                        }
                        myIndex++;
                    }
                }
                xShift += ti -> xmax - ti -> xmin;
           } else if (i == thy intervals.size) {
                while (myIndex <= my points.size) {
                    TextPoint tp = my points.at [myIndex];
                    double time = tp -> number + xShift;
                    if (time < ti -> xmin + precision) {
                        autoTextPoint newPoint = TextPoint_create (time, tp -> mark);
                        his points. addItem_move (newPoint.move());
                    }
                    myIndex++;
                }
            }
        }
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": cannot patch TextTier.");
    }
}

autoTextGrid TextGrid_IntervalTier_patch (TextGrid me, IntervalTier thee, const char32 *patchLabel, double precision) {
    try {
        double patchDurations;
        integer numberOfPatches;
        IntervalTier_getLabelInfo (thee, patchLabel, &patchDurations, &numberOfPatches);
        if (patchDurations <= 0 || my xmax - my xmin >= thy xmax - thy xmin ) { // Nothing to patch
            return Data_copy (me);
        }
        autoTextGrid him = TextGrid_createWithoutTiers (thy xmin, thy xmax);
        for (integer itier = 1; itier <= my tiers->size; itier ++) {
            Function anyTier = my tiers->at [itier];
            if (anyTier -> classInfo == classIntervalTier) {
//                autoIntervalTier ait = IntervalTiers_patch ((IntervalTier) anyTier, thee, patchLabel, precision);
                autoIntervalTier newTier = IntervalTiers_patch_noBoundaries ((IntervalTier) anyTier, thee, patchLabel, precision);
                his tiers -> addItem_move (newTier.move());
            } else {
                autoTextTier newTier = TextTier_IntervalTier_patch ((TextTier) anyTier, thee, patchLabel, precision);
                his tiers -> addItem_move (newTier.move());
            }
        }
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": not patched.");
    }
}

// We assume that the Sound and the SpeechSynthesizer have the same samplingFrequency
autoTextGrid SpeechSynthesizer_Sound_TextInterval_align (SpeechSynthesizer me, Sound thee, TextInterval him, double silenceThreshold, double minSilenceDuration, double minSoundingDuration) {
	try {
		Melder_require (thy xmin == his xmin && thy xmax == his xmax,
			U"Domains of Sound and TextGrid should be equal.");
		Melder_require (fabs (1.0 / thy dx - my d_samplingFrequency) < 1e-9, 
			U"The sampling frequencies of the SpeechSynthesizer and the Sound should be equal.");

		integer numberOfTokens = Melder_countTokens (his text);
		Melder_require (numberOfTokens > 0, U"The interval should have text.");
		
		// Remove silent intervals from start and end of sounds
		double minPitch = 200.0, timeStep = 0.005, precision = thy dx;
		double t1_thee, t2_thee;
		autoSound s_thee = Sound_trimSilencesAtStartAndEnd (thee, 0.0, minPitch, timeStep, silenceThreshold, minSilenceDuration, minSoundingDuration, & t1_thee, & t2_thee);
		double s_thee_duration = s_thee -> xmax - s_thee -> xmin;
		bool hasSilence_thee = fabs (t1_thee - thy xmin) > precision || fabs (t2_thee - thy xmax) > precision;

		if (my d_estimateSpeechRate) {
			// estimate speaking rate with the number of words per minute from the text
			double wordsPerMinute_rawTokens = 60.0 * numberOfTokens / s_thee_duration;
			// compensation for long words: 5 characters / word
			double wordsPerMinute_rawText = 60.0 * (str32len (his text) / 5.0) / s_thee_duration;
			my d_wordsPerMinute = Melder_ifloor (0.5 * (wordsPerMinute_rawTokens + wordsPerMinute_rawText));
		}
		autoTextGrid tg2;
		autoSound synth = SpeechSynthesizer_TextInterval_to_Sound (me, him, & tg2);
		autoTextGrid silentTextGrid;
		/*
		 * For the synthesizer the silence threshold has to be < -30 dB, otherwise fricatives will not
		 * be found as sounding! This is ok since silences are almost at zero amplitudes for synthesized sounds.
		 * We also have to decrease the minimum silence and minimum sounding duration to catch, for example,
		 * the final plosive "t" from the synthesized sound "text"
		 *
		 */
		double synth_silenceThreshold = -40.0, synth_minSilenceDuration = 0.05, synth_minSoundingDuration = 0.05;
		double t1_synth, t2_synth;
		autoSound synth_trimmed = Sound_trimSilencesAtStartAndEnd (synth.get(), 0.0, minPitch, timeStep,
			synth_silenceThreshold, synth_minSilenceDuration, synth_minSoundingDuration, & t1_synth, & t2_synth);
		double synth_trimmed_duration = synth_trimmed -> xmax - synth_trimmed -> xmin;
		bool hasSilence_synth = fabs (t1_synth - synth -> xmin) > precision || fabs (t2_synth - synth -> xmax) > precision;
		if (hasSilence_synth) {
			silentTextGrid = TextGrid_extractPart (tg2.get(), t1_synth, t2_synth, true);
		}
		double analysisWidth = 0.02, dt = 0.005, band = 0.0;
		// compare the durations of the two sounds to get an indication of the slope constraint of the DTW
		double slope = s_thee_duration / synth_trimmed_duration;
		slope = (slope > 1.0 ? slope : 1.0 / slope);
        int constraint = (slope < 1.5 ? 4 : slope < 2.0 ? 3 : slope < 3.0 ? 2 : 1);
		//autoMFCC m1 = Sound_to_MFCC ((hasSilence_thee ? s_thee.get() : thee),
		//	numberOfCoefficients, analysisWidth, dt, f1_mel, fmax_mel, df_mel);
		//autoMFCC m2 = Sound_to_MFCC ((hasSilence_synth ? synth_trimmed.get() : synth.get()),
		//	numberOfCoefficients, analysisWidth, dt, f1_mel, fmax_mel, df_mel);
		//double wc = 1, wle = 0, wr = 0, wer = 0, dtr = 0;
		//int matchStart = 1, matchEnd = 1, constraint = 4; // no 1/3 1/2 2/3
		//autoDTW dtw = CCs_to_DTW (m1.get(), m2.get(), wc, wle, wr, wer, dtr, matchStart, matchEnd, constraint);
        autoDTW dtw = Sounds_to_DTW ((hasSilence_thee ? s_thee.get() : thee), (hasSilence_synth ? synth_trimmed.get() : synth.get()),
			analysisWidth, dt, band, constraint);
		autoTextGrid result = DTW_TextGrid_to_TextGrid (dtw.get(), (hasSilence_synth ? silentTextGrid.get() : tg2.get()), precision);
		if (hasSilence_thee) {
			if (t1_thee > thy xmin) {
				TextGrid_setEarlierStartTime (result.get(), thy xmin, U"", U"");
			}
			if (result -> xmax > thy xmax) { // one sample or so out of sync
				result -> xmax = thy xmax;
				for (integer itier = 1; itier <= 4; itier	++) {
					IntervalTier tier = result -> intervalTier_cast (itier);
					tier -> xmax = thy xmax;
					TextInterval textInterval = tier -> intervals.at [tier -> intervals.size];
					textInterval -> xmax = thy xmax;
				}
			} else {	
				if (t2_thee < thy xmax + thy dx) {
					TextGrid_setLaterEndTime (result.get(), thy xmax, U"", U"");
				}
			}
		}
		return result;
	} catch (MelderError) {
		Melder_throw (U"Sound and TextInterval not aligned.");
	}
}
/*
typedef struct structAlignmentOfSoundAndTextStruct {
	double windowLength, timeStep; // analysis
	double f1_mel, fmax_mel, df_mel; // MelFilter
	integer numberOfMFCCCoefficients; // MFCC
	double dtw_cepstralWeight, dtw_logEnergyWeight; // MFCC -> DTW
	double dtw_regressionWeight, dtw_regressionlogEnergyWeight;
	double dtw_regressionWindowLength;
	double dtw_sakoeChibaBand, dtw_constraint;
	double silenceThreshold, minSilenceDuration, minSoundingDuration, trimDuration; // silence detection
	integer language, voicevariant, pitchAdjustment, pitchRange, wordsPerMinute; // synthesizer
	bool interpretPhonemeCodes, ipa, set_wordsPerMinute;
	double wordgap; // synthesizer
} *SpeechSynthesizer_alignmentStruct;*/

static autoTextGrid SpeechSynthesizer_Sound_TextInterval_align2 (SpeechSynthesizer me, Sound thee, TextInterval him, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration) {
    try {
		Melder_require (thy xmin == his xmin && thy xmax == his xmax,
			U"Domains of Sound and TextGrid should be equal.");
		Melder_require (fabs (1.0 / thy dx - my d_samplingFrequency) < 1e-9, 
			U"The sampling frequencies of the SpeechSynthesizer and the Sound should be equal.");

        const char32 *trimLabel = U"trim";
        // 1. trim the silences of the sound
        /*
         * For the synthesizer the silence threshold has to be < -30 dB, otherwise fricatives will not
         * be found as sounding! This is ok since silences are almost at zero amplitudes
         * We also have to decrease the minimum silence and minimum sounding duration to catch, for example,
         * the final plosive "t" from the word "text"
         *
         */
        double minPitch = 200, timeStep = 0.005, precision = thy dx;
        autoTextGrid thee_trimmer;
        autoSound thee_trimmed = Sound_trimSilences (thee, trimDuration, false, minPitch, timeStep, silenceThreshold,  minSilenceDuration, minSoundingDuration, &thee_trimmer, trimLabel);

        // 2. synthesize the sound from the TextInterval

        autoTextGrid tg_syn;
        autoSound synth = SpeechSynthesizer_TextInterval_to_Sound (me, him, &tg_syn);

        // 3. There should be no silences in the synthesized sound except at the start and finish.
		//    Set the wordwarp parameter to a small value like 0.001 s.

        // 4. Get DTW from the two sounds
		
        double analysisWidth = 0.02, dt = 0.005, band = 0.0;
        int constraint = 4;
        autoDTW dtw = Sounds_to_DTW (thee_trimmed.get(), synth.get(), analysisWidth, dt, band, constraint);

        // 6. Warp the synthesis TextGrid
        // first make domains equal, otherwsise the warper protests

        autoTextGrid warp = DTW_TextGrid_to_TextGrid (dtw.get(), tg_syn.get(), precision);

        // 7. Patch the trimmed intervals back into the warped TextGrid
		
        autoTextGrid result = TextGrid_IntervalTier_patch (warp.get(), (IntervalTier) thee_trimmer -> tiers->at [1], U"trim", 2 * thy dx);

        return result;
    } catch (MelderError) {
        Melder_throw (thee, U": sound and TextInterval not aligned.");
    }
}

autoTextGrid SpeechSynthesizer_Sound_IntervalTier_align (SpeechSynthesizer me, Sound thee, IntervalTier him, integer istart, integer iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration) {
    try {
		Melder_require (istart > 0 && istart <= iend && iend <= his intervals.size,
			U"Not a valid interval range.");
        OrderedOf<structTextGrid> textgrids;
        TextInterval tb = his intervals.at [istart];
        TextInterval te = his intervals.at [iend];
        autoTextGrid result = TextGrid_create (tb -> xmin, te -> xmax, U"sentence clause word phoneme", U"");
        for (integer iint = istart; iint <= iend; iint ++) {
            TextInterval ti = his intervals.at [iint];
            if (ti -> text && str32len (ti -> text) > 0) {
                autoSound sound = Sound_extractPart (thee, ti -> xmin, ti -> xmax,  kSound_windowShape::RECTANGULAR, 1, true);
                autoTextGrid grid = SpeechSynthesizer_Sound_TextInterval_align (me, sound.get(), ti, silenceThreshold, minSilenceDuration, minSoundingDuration);
                textgrids. addItem_move (grid.move());
            }
        }
        Melder_require (textgrids.size > 0, U"Nothing could be aligned. Was your IntervalTier empty?");

        autoTextGrid aligned = TextGrids_to_TextGrid_appendContinuous (& textgrids, true);
        return aligned;
    } catch (MelderError) {
        Melder_throw (U"No aligned TextGrid created.");
    }
}

static autoTextGrid SpeechSynthesizer_Sound_IntervalTier_align2 (SpeechSynthesizer me, Sound thee, IntervalTier him, integer istart, integer iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration) {
    try {
		Melder_require (istart > 0 && istart <= iend && iend <= his intervals.size,
			U"Not a valid interval range.");

        OrderedOf<structTextGrid> textgrids;
        TextInterval tb = his intervals.at [istart];
        TextInterval te = his intervals.at [iend];
        autoTextGrid result = TextGrid_create (tb -> xmin, te -> xmax, U"sentence clause word phoneme", U"");
        for (integer iint = istart; iint <= iend; iint ++) {
            TextInterval ti = his intervals.at [iint];
            if (ti -> text && str32len (ti -> text) > 0) {
                autoSound sound = Sound_extractPart (thee, ti -> xmin, ti -> xmax,  kSound_windowShape::RECTANGULAR, 1, true);
                autoTextGrid grid = SpeechSynthesizer_Sound_TextInterval_align2 (me, sound.get(), ti, silenceThreshold, minSilenceDuration, minSoundingDuration, trimDuration);
                textgrids. addItem_move (grid.move());
            }
        }
        Melder_require (textgrids.size > 0, U"Nothing could be aligned. Was your IntervalTier empty?");

        autoTextGrid aligned = TextGrids_to_TextGrid_appendContinuous (& textgrids, true);
        return aligned;
    } catch (MelderError) {
        Melder_throw (U"No aligned TextGrid created.");
    }
}

autoTextGrid SpeechSynthesizer_Sound_TextGrid_align (SpeechSynthesizer me, Sound thee, TextGrid him, integer tierNumber, integer istart, integer iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration) {
	try {//TODO: check not empty tier
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (him, tierNumber);
		autoTextGrid grid = SpeechSynthesizer_Sound_IntervalTier_align (me, thee, tier, istart, iend, silenceThreshold, minSilenceDuration, minSoundingDuration);
		return grid;
	} catch (MelderError) {
		Melder_throw (me, U", ", thee, U", ", him, U": Cannot align.");
	}
}


autoTextGrid SpeechSynthesizer_Sound_TextGrid_align2 (SpeechSynthesizer me, Sound thee, TextGrid him, integer tierNumber, integer istart, integer iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration) {
    try {//TODO: check not empty tier
    	IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (him, tierNumber);
        autoTextGrid grid = SpeechSynthesizer_Sound_IntervalTier_align2 (me, thee, tier, istart, iend, silenceThreshold, minSilenceDuration, minSoundingDuration, trimDuration);
        return grid;
    } catch (MelderError) {
        Melder_throw (U"");
    }
}

static autoStrings IntervalTier_to_Strings_withOriginData (IntervalTier me, integer *from) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector<char32 *> (1, my intervals.size);
		for (integer i = 1; i <= my intervals.size; i ++) {
			TextInterval ti = my intervals.at [i];
			if (ti -> text && ti -> text [0] != U'\0') {
				thy strings [++ thy numberOfStrings] = Melder_dup (ti -> text);
				if (from) {
					from [thy numberOfStrings] = i;
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Strings created.");
	}
}

autoTable IntervalTiers_to_Table_textAlignmentment (IntervalTier target, IntervalTier source, EditCostsTable costs) {
	try {
		integer numberOfTargetIntervals = target -> intervals.size;
		integer numberOfSourceIntervals = source -> intervals.size;
		autoNUMvector<integer> targetOrigin (1, numberOfTargetIntervals);
		autoNUMvector<integer> sourceOrigin (1, numberOfSourceIntervals);
		autoStrings targets = IntervalTier_to_Strings_withOriginData (target, targetOrigin.peek());
		autoStrings sources = IntervalTier_to_Strings_withOriginData (source, sourceOrigin.peek());
		autoEditDistanceTable edit = EditDistanceTable_create (targets.get(), sources.get());
		if (costs != 0) {
			EditDistanceTable_setEditCosts (edit.get(), costs);
			EditDistanceTable_findPath (edit.get(), nullptr);
		}
		integer pathLength = edit -> warpingPath -> pathLength;
		autoTable thee = Table_createWithColumnNames (pathLength - 1, U"targetInterval targetText targetStart targetEnd sourceInterval sourceText sourceStart sourceEnd operation");
		for (integer i = 2; i <= pathLength; i++) {
			structPairOfInteger p = edit -> warpingPath -> path[i];
			structPairOfInteger p1 = edit -> warpingPath -> path[i - 1];
			double targetStart = undefined, targetEnd = undefined;
			double sourceStart = undefined, sourceEnd = undefined;
			const char32 * targetText = U"", *sourceText = U"";
			integer targetInterval = p.y > 1 ? targetOrigin[p.y - 1] : 0;
			integer sourceInterval = p.x > 1 ? sourceOrigin[p.x - 1] : 0;
			if (targetInterval > 0) {
				TextInterval ti = target -> intervals.at [targetInterval];
				targetStart = ti -> xmin;
				targetEnd =  ti -> xmax;
				targetText = ti -> text;
			}
			if (sourceInterval > 0) {
				TextInterval ti = source -> intervals.at [sourceInterval];
				sourceStart = ti -> xmin;
				sourceEnd =  ti -> xmax;
				sourceText = ti -> text;
			}
			integer irow = i - 1;
			if (p.y == p1.y) { // deletion
				Table_setNumericValue (thee.get(), irow, 1, 0);
				Table_setStringValue  (thee.get(), irow, 2, U"");
				Table_setNumericValue (thee.get(), irow, 3, undefined);
				Table_setNumericValue (thee.get(), irow, 4, undefined);
				Table_setNumericValue (thee.get(), irow, 5, sourceInterval);
				Table_setStringValue  (thee.get(), irow, 6, sourceText);
				Table_setNumericValue (thee.get(), irow, 7, sourceStart);
				Table_setNumericValue (thee.get(), irow, 8, sourceEnd);
				Table_setStringValue  (thee.get(), irow, 9, U"d");
			} else if (p.x == p1.x) { // insertion
				Table_setNumericValue (thee.get(), irow, 1, targetInterval);
				Table_setStringValue  (thee.get(), irow, 2, targetText);
				Table_setNumericValue (thee.get(), irow, 3, targetStart);
				Table_setNumericValue (thee.get(), irow, 4, targetEnd);
				Table_setNumericValue (thee.get(), irow, 5, 0);
				Table_setStringValue  (thee.get(), irow, 6, U"");
				Table_setNumericValue (thee.get(), irow, 7, undefined);
				Table_setNumericValue (thee.get(), irow, 8, undefined);
				Table_setStringValue  (thee.get(), irow, 9, U"i");
			} else { // substitution ?
				Table_setNumericValue (thee.get(), irow, 1, targetInterval);
				Table_setStringValue  (thee.get(), irow, 2, targetText);
				Table_setNumericValue (thee.get(), irow, 3, targetStart);
				Table_setNumericValue (thee.get(), irow, 4, targetEnd);
				Table_setNumericValue (thee.get(), irow, 5, sourceInterval);
				Table_setStringValue  (thee.get(), irow, 6, sourceText);
				Table_setNumericValue (thee.get(), irow, 7, sourceStart);
				Table_setNumericValue (thee.get(), irow, 8, sourceEnd);
				Table_setStringValue  (thee.get(), irow, 9, Melder_equ (targetText, sourceText) ? U" " : U"s");
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (target, U" and ", source, U" not aligned.");
	}
}

autoTable TextGrids_to_Table_textAlignmentment (TextGrid target, integer ttier, TextGrid source, integer stier, EditCostsTable costs) {
	try {
		IntervalTier targetTier = TextGrid_checkSpecifiedTierIsIntervalTier (target, ttier);
		IntervalTier sourceTier = TextGrid_checkSpecifiedTierIsIntervalTier (source, stier);
		return IntervalTiers_to_Table_textAlignmentment (targetTier, sourceTier, costs);
	} catch (MelderError) {
		Melder_throw (U"No text alignment table created from TextGrids ", target, U" and ", source, U".");
	}
}

// End of file TextGrid_and_SpeechSynthesizer.cpp
