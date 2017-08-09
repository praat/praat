/* TextGrid_extensions.cpp
 *
 * Copyright (C) 1993-2016 David Weenink
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
 djmw 20020702 GPL header
 djmw 20020702 +TextGrid_extendTime
 djmw 20051215 Corrected a bug in TextGrid_readFromTIMITLabelFile that caused a crash when the first number in
 	a file was not 0 (in that case an empty interval was added as the first element in the tier).
 djmw 20060517 Added (TextTier|IntervalTier|TextGrid)_changeLabels.
 djmw 20060712 TextGrid_readFromTIMITLabelFile: don't set first boundary to zero for .wrd files.
 djmw 20060921 Added IntervalTier_cutIntervalOnLabelMatch, IntervalTier_cutIntervals_minimumDuration
 djmw 20061113 Reassign item in list after a deletion.
 djmw 20061116 Added IntervalTier_cutInterval to correct a bug in IntervalTier_removeBoundary...
 djmw 20071008 Removed two unused variables.
 djmw 20071202 Melder_warning<n>
*/

#include <ctype.h>
#include "TextGrid_extensions.h"
#include "NUM2.h"

struct TIMIT_key {
	const char *timitLabel, *ipaLabel;
} TIMIT_toIpaTable[] = {
	{"", ""},
	/* Vowels */
	{"iy", "i"},			/* beet: bcl b IY tcl t */
	{"ih", "\\ic"}, 		/* bit: bcl b IH tcl t */
	{"eh", "\\ep"}, 		/* bet: bcl b EH tcl t */
	{"ey", "e"},  			/* bait: bcl b EY tcl t */
	{"ae", "\\ae"},  		/* bat: bcl b AE tcl t */
	{"aa", "\\as"}, 		/* bott: bcl b AA tcl t */
	{"aw", "a\\hs"},  		/* bout: bcl b AW tcl t */
	{"ay", "a\\ic"},  		/* bite: bcl b AY tcl t */
	{"ah", "\\vt"}, 		/* but: bcl b AH tcl t */
	{"ao", "\\ct"},  		/* bought: bcl b AO tcl t */
	{"oy", "\\ct\\ic"},  	/* boy: bcl b OY */
	{"ow", "o"}, 			/* boat: bcl b OW tcl t */
	{"uh", "\\hs"}, 		/* book: bcl b UH tcl t */
	{"uw", "u"},  			/* boot: bcl b UW tcl t */
	/* fronted allophone of uw (alveolair contexts) */
	{"ux", "\\u\""}, 		/* toot: tcl t UX tcl t */
	{"er", "\\er\\hr"},  	/* bird: bcl b ER dcl d */
	{"ax", "\\sw"}, 		/* about: AX bcl b aw tcl t */
	{"ix", "\\i-"}, 		/* debit: dcl d eh bcl b IX tcl t */
	{"axr", "\\sr"}, 		/* butter: bcl ah dx AXR */
	/* devoiced schwa, very short */
	{"ax-h", "\\sw\\ov"}, 	/* suspect: s AX-H s pcl p eh kcl k tcl t */
	/* Semivowels and glides */
	{"l", "l"},				/* lay:	L ey */
	{"r", "r"},				/* ray:	R ey */
	{"w", "w"},				/* way:	w ey */
	{"y", "j"},				/* yacht: Y aa tcl t */
	{"hh", "h" },			/* hay: HH ey*/
	/* voiced allophone of h */
	{"hv", "\\hh"},			/* ahead: ax HV eh dcl d */
	{"el", "l\\|v"},		/* bottle: bcl b aa tcl t EL */
	/* Nasals */
	{"m", "m"},				/* mom:	M aa M */
	{"n", "n"},				/* noon: N uw N*/
	{"ng", "\\ng"},			/* sing: s ih NG */
	{"em", "m\\|v"},		/* bottom: b aa tcl t EM */
	{"en", "n\\|v"},		/* button:	b ah q EN */
	{"eng", "\\ng\\|v"},	/* washington: w aa sh ENG tcl t ax n */
	/* nasal flap */
	{"nx", "n^\\fh"},		/* winner: wih NX axr */
	/* Fricatives */
	{"s", "s"},				/* sea: S iy */
	{"sh", "\\sh"},			/* she: SH iy */
	{"z", "z"},				/* zone: Z ow n */
	{"zh", "\\zh"},			/* azure: ae ZH er */
	{"f", "f"},				/* fin: F ih n */
	{"th", "\\te"},			/* thin: TH ih n */
	{"v", "v"},				/* van: v ae n */
	{"dh", "\\dh"},			/* then: DH en */
	/* Affricates */
	{"jh", "d\\zh"},		/* joke: DCL JH ow kcl k */
	{"ch", "t\\sh"},		/* choke TCL CH ow kcl k */
	/* Stops */
	{"b", "b"},				/* bee: BCL B iy */
	{"d", "d"},				/* day: DCL D ey */
	{"g", "g"},				/* gay: GCL G ey */
	{"p", "p"},				/* pea: PCL P iy */
	{"t", "t"},				/* tea: TCL T iy */
	{"k", "k"},				/* key: KCL K iy */
	/* 20140315: Added silences before the burst */
	{"bcl", ""},
	{"dcl", ""},
	{"gcl", ""},
	{"pcl", ""},
	{"tcl", ""},
	{"kcl", ""},
	/* flap */
	{"dx", "\\fh"},			/* muddy: m ah DX iy & dirty: dcl d er DX iy */
	/* glottal stop */
	{"q", "?"},
	/* Others */
	{"pau", ""},	/* pause */
	{"epi", ""},	/* epenthetic silence */
	{"h#", ""}, 	/* marks start and end piece of sentence */
	/* the following markers only occur in the dictionary */
	{"1", "1"},		/* primary stress marker */
	{"2", "2"}		/* secondary stress marker */
};

#define TIMIT_NLABELS (sizeof TIMIT_toIpaTable / sizeof TIMIT_toIpaTable[1] - 1)
static const char *TIMIT_DELIMITER = "h#";

static const char *timitLabelToIpaLabel (const char timitLabel[]) {
	for (unsigned int i = 1; i <= TIMIT_NLABELS; i++)
		if (!strcmp (TIMIT_toIpaTable[i].timitLabel, timitLabel)) {
			return TIMIT_toIpaTable[i].ipaLabel;
		}
	return timitLabel;
}

static int isTimitPhoneticLabel (const char label[]) {
	for (unsigned int i = 1; i <= TIMIT_NLABELS; i++)
		if (! strcmp (TIMIT_toIpaTable[i].timitLabel, label)) {
			return 1;
		}
	return 0;
}

static int isTimitWord (const char label[]) {
	const char *p = label;
	for (; *p; p++) if (isupper (*p) && *p != '\'') {
			return 0;
		}
	return 1;
}

autoDaata TextGrid_TIMITLabelFileRecognizer (int nread, const char *header, MelderFile file) {
	char hkruis[3] = "h#", label1[512], label2[512];
	int length, phnFile = 0;
	long it[5]; 
	if (nread < 12 || sscanf (header, "%ld%ld%511s%n\n", &it[1], &it[2], label1, &length) != 3 ||
		it[1] < 0 || it[2] <= it[1] || sscanf (&header[length], "%ld%ld%511s\n", &it[3], &it[4], label2) != 3 || it[4] <= it[3]) {
		// 20120512 djmw removed the extra "it[3] < it[2]" check, because otherwise train/dr7/mdlm0/si1864.wrd cannot be read
		return autoDaata ();
	}
	if (! strcmp (label1, hkruis)) {
		if (isTimitPhoneticLabel (label2)) {
			phnFile = 1;
		} else if (! isTimitWord (label2)) {
			return autoDaata ();
		}
	} else if (! isTimitWord (label1) || ! isTimitWord (label2)) {
		return autoDaata ();
	}
	autoTextGrid thee = TextGrid_readFromTIMITLabelFile (file, phnFile);
	return thee.move();
}

static void IntervalTier_add (IntervalTier me, double xmin, double xmax, const char32 *label) {
	long i = IntervalTier_timeToIndex (me, xmin); // xmin is in interval i
	if (i < 1) {
		Melder_throw (U"Index too low.");
	}

	autoTextInterval newti = TextInterval_create (xmin, xmax, label);
	TextInterval interval = my intervals.at [i];
	double xmaxi = interval -> xmax;
	if (xmax > xmaxi) {
		Melder_throw (U"Don't know what to do");    // Don't know what to do
	}
	if (xmin == interval -> xmin) {
		if (xmax == interval -> xmax) { // interval already present
			TextInterval_setText (interval, label);
			return;
		}
		// split interval
		interval -> xmin = xmax;
		my intervals. addItem_move (newti.move());
		return;
	}
	interval -> xmax = xmin;
	my intervals. addItem_move (newti.move());
	// extra interval when xmax's are not the same
	if (xmax < xmaxi) {
		autoTextInterval newti2 = TextInterval_create (xmax, xmaxi, interval -> text);
		my intervals. addItem_move (newti2.move());
	}
}

autoTextGrid TextGrid_readFromTIMITLabelFile (MelderFile file, int phnFile) {
	try {
		double dt = 1.0 / 16000; /* 1 / (TIMIT samplingFrequency) */
		double xmax = dt;
		autofile f = Melder_fopen (file, "r");

		// Ending time will only be known after all labels have been read.
		// We start with a sufficiently long duration (one hour) and correct this later.

		autoTextGrid me = TextGrid_create (0.0, 3600.0, U"wrd", 0);
		IntervalTier timit = (IntervalTier) my tiers->at [1];
		long linesRead = 0;
		char line[200], label[200];
		while (fgets (line, 199, f)) {
			long it1, it2;
			linesRead++;
			if (sscanf (line, "%ld%ld%199s", &it1, &it2, label) != 3) {
				Melder_throw (U"Incorrect number of items.");
			}
			if (it1 < 0 || it2 <= it1) {
				Melder_throw (U"Incorrect time at line ", linesRead);
			}
			xmax = it2 * dt;
			double xmin = it1 * dt;
			long ni = timit -> intervals.size - 1;
			if (ni < 1) {
				ni = 1;
				// Some files do not start with a first line "0 <number2> h#".
				// Instead they start with "<number1> <number2> h#", where number1 > 0.
				// We override number1 with 0. */

				if (xmin > 0.0 && phnFile) {
					xmin = 0.0;
				}
			}
			TextInterval interval = timit -> intervals.at [ni];
			if (xmin < interval -> xmax && linesRead > 1) {
				xmin = interval -> xmax;
				Melder_warning (U"File \"", MelderFile_messageName (file), U"\": Start time set to previous end "
				                 U"time for label at line ", linesRead, U".");
			}
			// standard: new TextInterval
			const char *labelstring = (strncmp (label, "h#", 2) ? label : TIMIT_DELIMITER);
			IntervalTier_add (timit, xmin, xmax, Melder_peek8to32 (labelstring));
		}

		// Now correct the end times, based on last read interval.
		// (end time was set to large value!)

		if (timit -> intervals.size < 2) {
			Melder_throw (U"Empty TextGrid");
		}
		timit -> intervals. removeItem (timit -> intervals.size);
		TextInterval interval = timit -> intervals.at [timit -> intervals.size];
		timit -> xmax = interval -> xmax;
		my xmax = xmax;
		if (phnFile) { // Create tier 2 with IPA symbols
			autoIntervalTier ipa = Data_copy (timit);
			Thing_setName (ipa.get(), U"ipa");
			// First change the data in ipa
			for (long i = 1; i <= ipa -> intervals.size; i ++) {
				interval = timit -> intervals.at [i];

				TextInterval_setText (ipa -> intervals.at [i],
					Melder_peek8to32 (timitLabelToIpaLabel (Melder_peek32to8 (interval -> text))));
			}
			my tiers -> addItem_move (ipa.move()); // Then: add to collection
			Thing_setName (timit, U"phn");  // rename wrd
		}
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from file ", file, U".");
	}
}

autoTextGrid TextGrids_merge (TextGrid me, TextGrid thee) {
	try {
		int at_end = 0, at_start = 1;

		autoTextGrid g1 = Data_copy (me);
		autoTextGrid g2 = Data_copy (thee);

		// The new TextGrid will have the domain
		// [min(g1->xmin, g2->xmin), max(g1->xmax, g2->xmax)]

		double extra_time_end = fabs (g2 -> xmax - g1 -> xmax);
		double extra_time_start = fabs (g2 -> xmin - g1 -> xmin);

		if (g1 -> xmin > g2 -> xmin) {
			TextGrid_extendTime (g1.get(), extra_time_start, at_start);
		}
		if (g1 -> xmax < g2 -> xmax) {
			TextGrid_extendTime (g1.get(), extra_time_end, at_end);
		}
		if (g2 -> xmin > g1 -> xmin) {
			TextGrid_extendTime (g2.get(), extra_time_start, at_start);
		}
		if (g2 -> xmax < g1 -> xmax) {
			TextGrid_extendTime (g2.get(), extra_time_end, at_end);
		}

		for (long i = 1; i <= g2 -> tiers->size; i ++) {
			autoFunction tier = Data_copy (g2 -> tiers->at [i]);
			g1 -> tiers -> addItem_move (tier.move());
		}
		return g1;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": not merged.");
	}
}

void IntervalTier_setLaterEndTime (IntervalTier me, double xmax, const char32 *mark) {
	try {
		if (xmax <= my xmax) return; // nothing to be done
		Melder_assert (my intervals.size > 0);
		TextInterval ti = my intervals.at [my intervals.size];
		Melder_assert (xmax > ti -> xmax);
		if (mark) {
			autoTextInterval interval = TextInterval_create (ti -> xmax, xmax, mark);
			my intervals. addItem_move (interval.move());
		} else {
			// extend last interval
			ti -> xmax = xmax;
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"Larger end time of IntervalTier not set.");
	}
}

void IntervalTier_setEarlierStartTime (IntervalTier me, double xmin, const char32 *mark) {
	try {
		if (xmin >= my xmin) {
			return;
		}
		Melder_assert (my intervals.size > 0);
		TextInterval ti = my intervals.at [1];
		Melder_assert (xmin < ti -> xmin);
		if (mark) {
			autoTextInterval interval = TextInterval_create (xmin, ti -> xmin, mark);
			my intervals. addItem_move (interval.move());
		} else {
			// extend first interval
			ti -> xmin = xmin;
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (U"Earlier start time of IntervalTier not set.");
	}
}

void IntervalTier_moveBoundary (IntervalTier me, long iint, bool atStart, double newTime) {
    try {
        if (iint < 1 or iint > my intervals.size) {
            Melder_throw (U"Interval out of range.");
        }
        if ((iint == 1 && atStart) or (iint == my intervals.size && ! atStart)) {
            Melder_throw (U"Cannot change the domain.");
        }
        TextInterval interval = my intervals.at [iint];
        if (atStart) {
            TextInterval pinterval = my intervals.at [iint-1];
            if (newTime <= pinterval -> xmin) {
                Melder_throw (U"Cannot move past the start of previous interval.");
            }
            pinterval -> xmax = interval -> xmin = newTime;
        } else {
            TextInterval ninterval = my intervals.at [iint+1];
            if (newTime >= ninterval -> xmax) {
                Melder_throw (U"Cannot move past the end of next interval.");
            }
            ninterval -> xmin = interval -> xmax = newTime;
        }
    } catch (MelderError) {
        Melder_throw (me, U": boundary not moved.");
    }
}


void TextTier_setLaterEndTime (TextTier me, double xmax, const char32 *mark) {
	try {
		if (xmax <= my xmax) {
			return;
		}
		if (mark) {
			autoTextPoint textpoint = TextPoint_create (my xmax, mark);
			my points. addItem_move (textpoint.move());
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"Larger end time of TextTier not set.");
	}
}

void TextTier_setEarlierStartTime (TextTier me, double xmin, const char32 *mark) {
	try {
		if (xmin >= my xmin) {
			return;
		}
		if (mark) {
			autoTextPoint textpoint = TextPoint_create (my xmin, mark);
			my points. addItem_move (textpoint.move());
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (U"Earlier start time of TextTier not set.");
	}
}

void TextGrid_setEarlierStartTime (TextGrid me, double xmin, const char32 *imark, const char32 *pmark) {
	try {
		if (xmin >= my xmin) {
			return;
		}
		for (long tierNumber = 1 ; tierNumber <= my tiers->size; tierNumber ++) {
			Function tier = my tiers->at [tierNumber];
			if (tier -> classInfo == classIntervalTier) {
				IntervalTier_setEarlierStartTime ((IntervalTier) tier, xmin, imark);
			} else {
				TextTier_setEarlierStartTime ((TextTier) tier, xmin, pmark);
			}
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (U"Earlier start time of TextGrid not set.");
	}
}

void TextGrid_setLaterEndTime (TextGrid me, double xmax, const char32 *imark, const char32 *pmark) {
	try {
		if (xmax <= my xmax) {
			return;
		}
		for (long tierNumber =1 ; tierNumber <= my tiers->size; tierNumber ++) {
			Function tier = my tiers->at [tierNumber];
			if (tier -> classInfo == classIntervalTier) {
				IntervalTier_setLaterEndTime ((IntervalTier) tier, xmax, imark);
			} else {
				TextTier_setLaterEndTime ((TextTier) tier, xmax, pmark);
			}
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"Larger end time of TextGrid not set.");
	}
}

void TextGrid_extendTime (TextGrid me, double extra_time, int position) {
	autoTextGrid thee;
	try {
		double xmax = my xmax, xmin = my xmin;
		bool at_end = ( position == 0 );

		if (extra_time == 0.0) {
			return;
		}
		extra_time = fabs (extra_time);   // just in case
		thee = Data_copy (me);

		if (at_end) {
			xmax += extra_time;
		} else {
			xmin -= extra_time;
		}

		for (long i = 1; i <= my tiers->size; i ++) {
			Function anyTier = my tiers->at [i];
			double tmin = anyTier -> xmin, tmax = anyTier -> xmax;

			if (at_end) {
				anyTier -> xmax = xmax;
				tmin = tmax;
				tmax = xmax;
			} else {
				anyTier -> xmin = xmin;
				tmax = tmin;
				tmin = xmin;
			}
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = (IntervalTier) anyTier;
				autoTextInterval interval = TextInterval_create (tmin, tmax, U"");
				tier -> intervals. addItem_move (interval.move());
			}
		}
		my xmin = xmin;
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (me, U": time not extended.");
	}
}

void TextGrid_setTierName (TextGrid me, long itier, const char32 *newName) {
	try {
		long ntiers = my tiers->size;
		if (itier < 1 || itier > ntiers) {
			Melder_throw (U"Tier number (", itier, U") should not be larger than the number of tiers (", ntiers, U").");
		}
		Thing_setName (my tiers->at [itier], newName);
	} catch (MelderError) {
		Melder_throw (me, U": tier name not set.");
	}
}

static void IntervalTier_cutInterval (IntervalTier me, long index, int extend_option) {
	long size_pre = my intervals.size;

	// There always must be at least one interval
	if (size_pre == 1 || index > size_pre || index < 1) {
		return;
	}

	TextInterval ti = my intervals.at [index];
	double xmin = ti -> xmin;
	double xmax = ti -> xmax;
	my intervals. removeItem (index);
	if (index == 1) { 
		// Change xmin of the new first interval.
		ti = my intervals.at [index];
		ti -> xmin = xmin;
	} else if (index == size_pre) { 
		// Change xmax of the new last interval.
		ti = my intervals.at [my intervals.size];
		ti -> xmax = xmax;
	} else {
		if (extend_option == 0) { 
			// extend earlier interval to the right
			ti = my intervals.at [index - 1];
			ti -> xmax = xmax;
		} else {
			// extend next interval to the left
			ti = my intervals.at [index];
			ti -> xmin = xmin;
		}
	}
}

void IntervalTier_removeBoundariesBetweenIdenticallyLabeledIntervals (IntervalTier me, const char32 *label) {
    try {
		for (long iinterval = my intervals.size; iinterval > 1; iinterval --) {
			TextInterval thisInterval = my intervals.at [iinterval];
			if (Melder_equ (thisInterval -> text, label)) {
				TextInterval previousInterval = my intervals.at [iinterval - 1];
				if (Melder_equ (previousInterval -> text, label)) {
					Melder_free (previousInterval -> text);
					IntervalTier_removeLeftBoundary (me, iinterval);
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": boundaries not removed.");
	}
}

void IntervalTier_cutIntervals_minimumDuration (IntervalTier me, const char32 *label, double minimumDuration) {
	long iinterval = 1;
	while (iinterval <= my intervals.size) {
		TextInterval interval = my intervals.at [iinterval];
		if ((! label || (interval -> text && str32equ (interval -> text, label))) &&
			interval -> xmax - interval -> xmin < minimumDuration)
		{
			IntervalTier_cutInterval (me, iinterval, 0);
		} else {
			iinterval ++;
		}
	}
}

void IntervalTier_cutIntervalsOnLabelMatch (IntervalTier me, const char32 *label) {
	long iinterval = 1;
	while (iinterval < my intervals.size) {
		TextInterval thisInterval = my intervals.at [iinterval];
		TextInterval nextInterval = my intervals.at [iinterval + 1];
		if ( (! label || (thisInterval -> text && str32equ (thisInterval -> text, label))) &&
			Melder_equ (thisInterval -> text, nextInterval -> text))
		{
			IntervalTier_cutInterval (me, iinterval, 1);
		} else {
			iinterval ++;
		}
	}
}

void IntervalTier_changeLabels (IntervalTier me, long from, long to, const char32 *search, const char32 *replace, int use_regexp, long *nmatches, long *nstringmatches) {
	try {
		if (from == 0) {
			from = 1;
		}
		if (to == 0) {
			to = my intervals.size;
		}
		if (from > to || from < 1 || to > my intervals.size) {
			Melder_throw (U"Incorrect specification of where to act.");
		}
		if (use_regexp && str32len (search) == 0) {
			Melder_throw (U"The regex search string cannot be empty.\nYou may search for an empty string with the expression \"^$\"");
		}

		long nlabels = to - from + 1;
		autoNUMvector<char32 *> labels (1, nlabels);

		for (long i = from; i <= to; i ++) {
			TextInterval interval = my intervals.at [i];
			labels[i - from + 1] = interval -> text;   // Shallow copy.
		}
		autostring32vector newlabels (strs_replace (labels.peek(), 1, nlabels, search, replace, 0, nmatches, nstringmatches, use_regexp), 1, nlabels);

		for (long i = from; i <= to; i ++) {
			TextInterval interval = my intervals.at [i];
			Melder_free (interval -> text);
			interval -> text = newlabels [i - from + 1];   // Transfer of ownership.
			newlabels [i - from + 1] = nullptr;
		}
	} catch (MelderError) {
		Melder_throw (me, U": labels not changed.");
	}
}

void TextTier_changeLabels (TextTier me, long from, long to, const char32 *search, const char32 *replace, int use_regexp, long *nmatches, long *nstringmatches) {
	try {
		if (from == 0) {
			from = 1;
		}
		if (to == 0) {
			to = my points.size;
		}
		if (from > to || from < 1 || to > my points.size) {
			Melder_throw (U"Incorrect specification of where to act.");
		}
		if (use_regexp && str32len (search) == 0) {
			Melder_throw (U"The regex search string cannot be empty.\nYou may search for an empty string with the expression \"^$\"");
		}
		long nmarks = to - from + 1;
		autoNUMvector<char32 *> marks (1, nmarks);   // a non-owning vector of strings

		for (long i = from; i <= to; i ++) {
			TextPoint point = my points.at [i];
			marks [i - from + 1] = point -> mark;   // reference copy
		}
		autostring32vector newMarks (strs_replace (marks.peek(), 1, nmarks, search, replace, 0, nmatches, nstringmatches, use_regexp), 1, nmarks);

		for (long i = from; i <= to; i ++) {
			TextPoint point = my points.at [i];
			Melder_free (point -> mark);   // this discards the original mark, and dangles its reference copy in `marks`, which will not be used
			point -> mark = newMarks [i - from + 1];   // move the new mark; this consists of a copy from A to B...
			newMarks [i - from + 1] = nullptr;   // ...followed by zeroing A
		}
	} catch (MelderError) {
		Melder_throw (me, U": no labels changed.");
	}
}

void TextGrid_changeLabels (TextGrid me, int tier, long from, long to, const char32 *search, const char32 *replace, int use_regexp, long *nmatches, long *nstringmatches) {
	try {
		long ntiers = my tiers->size;
		if (tier < 1 || tier > ntiers) {
			Melder_throw (U"The tier number (", tier, U") should not be larger than the number of tiers (", ntiers, U").");
		}
		if (use_regexp && str32len (search) == 0) {
			Melder_throw (U"The regex search string cannot be empty.\nYou may search for an empty string with the expression \"^$\"");
		}
		Function anyTier = my tiers->at [tier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier_changeLabels ((IntervalTier) anyTier, from, to, search, replace, use_regexp, nmatches, nstringmatches);
		} else {
			TextTier_changeLabels ((TextTier) anyTier, from, to, search, replace, use_regexp, nmatches, nstringmatches);
		}
	} catch (MelderError) {
		Melder_throw (me, U": labels not changed.");
	}
}

static void IntervalTier_checkStartAndEndTime (IntervalTier me) {
	Melder_assert (my intervals.size > 0);
	TextInterval ti = my intervals.at [1];
	if (my xmin != ti -> xmin) {
		Melder_throw (me, U": start time of first interval doesn't match start time of the tier.");
	}
	ti = my intervals.at [my intervals.size];
	if (my xmax != ti -> xmax) {
		Melder_throw (me, U": end time of last interval doesn't match end time of the tier.");
	}
}

// Precondition: if (preserveTimes) { my xmax <= thy xmin }
// Postcondition: my xmin preserved
void IntervalTiers_append_inline (IntervalTier me, IntervalTier thee, bool preserveTimes) {
	try {
		IntervalTier_checkStartAndEndTime (me); // start/end time of first/last interval should match with tier
		IntervalTier_checkStartAndEndTime (thee);
        double xmax_previous = my xmax, time_shift = my xmax - thy xmin;
		if (preserveTimes && my xmax < thy xmin) {
			autoTextInterval connection = TextInterval_create (my xmax, thy xmin, U"");
            xmax_previous = thy xmin;
			my intervals. addItem_move (connection.move());
		}
		for (long iint = 1; iint <= thy intervals.size; iint ++) {
			autoTextInterval ti = Data_copy (thy intervals.at [iint]);
			if (preserveTimes) {
				my intervals. addItem_move (ti.move());
			} else {
				/* the interval could be so short that if we test ti -> xmin < ti->xmax it might be true
				 * but after assigning ti->xmin = xmax_previous and ti->xmax += time_shift the test
				 * ti -> xmin < ti->xmax might be false!
				 * We want to make sure xmin and xmax are not register variables and therefore force double64 
				 * by using volatile variables.
		 		 */
				volatile double xmin = xmax_previous;
				volatile double xmax = ti -> xmax + time_shift;
				if (xmin < xmax) {
					ti -> xmin = xmin; ti -> xmax = xmax;
					my intervals. addItem_move (ti.move());
					xmax_previous = xmax;
				}
				// else don't include interval
            }
		}
		my xmax = preserveTimes ? thy xmax : xmax_previous;
	} catch (MelderError) {
		Melder_throw (U"IntervalTiers not appended.");
	}
}

// Precondition: if (preserveTimes) { my xmax <= thy xmin }
void TextTiers_append_inline (TextTier me, TextTier thee, bool preserveTimes) {
	try {
		for (long iint = 1; iint <= thy points.size; iint ++) {
			autoTextPoint tp = Data_copy (thy points.at [iint]);
			if (! preserveTimes) {
				tp -> number += my xmax - thy xmin;
			}
			my points. addItem_move (tp.move());
		}
		my xmax = preserveTimes ? thy xmax : my xmax + (thy xmax - thy xmin);
	} catch (MelderError) {
		Melder_throw (U"TextTiers not appended.");
	}
}

static void TextGrid_checkStartAndEndTimesOfTiers (TextGrid me) {
	for (long itier = 1; itier <= my tiers->size; itier ++) {
		Function tier = my tiers->at [itier];
		if (tier -> xmin != my xmin) {
			Melder_throw (me, U": the start time of tier ", itier, U" does not match the start time of its TextGrid.");
		} else if (tier -> xmax != my xmax) {
			Melder_throw (me, U": the end time of tier ", itier, U" does not match the end time of its TextGrid.");
		}
	}
}

void TextGrids_append_inline (TextGrid me, TextGrid thee, bool preserveTimes)
{
	try {
		if (my tiers->size != thy tiers->size) {
			Melder_throw (U"The numbers of tiers must be equal.");
		}
		if (preserveTimes && thy xmin < my xmax) {
			Melder_throw (U"The start time of the second TextGrid can't be earlier than the end time of the first one if you want to preserve times.");
		}
		
		TextGrid_checkStartAndEndTimesOfTiers (me); // all tiers must have the same start/end time as textgrid
		TextGrid_checkStartAndEndTimesOfTiers (thee);
		// last intervals must have the same end time
		double xmax = preserveTimes ? thy xmax : my xmax + (thy xmax - thy xmin);
		for (long itier = 1; itier <= my tiers->size; itier ++) {
			Function myTier = my tiers->at [itier], thyTier = thy tiers->at [itier];
			if (myTier -> classInfo == classIntervalTier && thyTier -> classInfo == classIntervalTier) {
				IntervalTier  myIntervalTier = static_cast <IntervalTier>  (myTier);
				IntervalTier thyIntervalTier = static_cast <IntervalTier> (thyTier);
				IntervalTiers_append_inline (myIntervalTier, thyIntervalTier, preserveTimes);
				/*
					Because of floating-point rounding errors, we explicitly make sure that
					both the xmax of the tier and the xmax of the last interval equal the xmax of the grid.
				*/
				myIntervalTier -> xmax = xmax;
                TextInterval lastInterval = myIntervalTier -> intervals.at [myIntervalTier -> intervals.size];
                lastInterval -> xmax = xmax;
                Melder_assert (lastInterval -> xmax > lastInterval -> xmin);
			} else if (myTier -> classInfo == classTextTier && thyTier -> classInfo == classTextTier) {
				TextTier  myTextTier = static_cast <TextTier>  (myTier);
				TextTier thyTextTier = static_cast <TextTier> (thyTier);
				TextTiers_append_inline (myTextTier, thyTextTier, preserveTimes);
                myTextTier -> xmax = xmax;
			} else {
				Melder_throw (U"Tier ", itier, U" in the second TextGrid is of a different type "
					"than tier ", itier, U" in the first TextGrid.");
			}
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"TextGrids not appended.");
	}
}

autoTextGrid TextGrids_to_TextGrid_appendContinuous (OrderedOf<structTextGrid>* me, bool preserveTimes) {
	try {
		Melder_assert (my size > 0);
		autoTextGrid thee = Data_copy (my at [1]);
		for (long igrid = 2; igrid <= my size; igrid ++) {
			TextGrids_append_inline (thee.get(), my at [igrid], preserveTimes);
		}
		if (! preserveTimes) {
			Function_shiftXBy (thee.get(), -thy xmin);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No aligned TextGrid created from Collection.");
	}
}

static void NUMshift (double *x, double dx) {   // TODO: make global
	*x += dx;
}

static autoIntervalTier IntervalTier_shiftBoundaries (IntervalTier me, double startTime, double shiftTime) {   // TODO: make global
	autoIntervalTier result;   // TODO: implement
	(void) me;   // TODO: use
	(void) startTime;   // TODO: use
	(void) shiftTime;   // TODO: use
	return result;
}

/* End of file TextGrid_extensions.cpp */
