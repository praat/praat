/* TextGrid_extensions.cpp
 *
 * Copyright (C) 1993-2013 David Weenink
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

Any TextGrid_TIMITLabelFileRecognizer (int nread, const char *header, MelderFile file) {
	long it[5]; int length, phnFile = 0; char hkruis[3] = "h#", label1[512], label2[512];
	if (nread < 12 ||
	        sscanf (header, "%ld%ld%s%n\n", &it[1], &it[2], label1, &length) != 3 ||
	        it[1] < 0 || it[2] <= it[1] ||
	        sscanf (&header[length], "%ld%ld%s\n", &it[3], &it[4], label2) != 3 ||
	        // 20120512 djmw removed the extra "it[3] < it[2]" check, because otherwise train/dr7/mdlm0/si1864.wrd cannot be read
	        it[4] <= it[3]) {
		return 0;
	}
	if (! strcmp (label1, hkruis)) {
		if (isTimitPhoneticLabel (label2)) {
			phnFile = 1;
		} else if (! isTimitWord (label2)) {
			return 0;
		}
	} else if (! isTimitWord (label1) || ! isTimitWord (label2)) {
		return 0;
	}
	return TextGrid_readFromTIMITLabelFile (file, phnFile);
}

static void IntervalTier_add (IntervalTier me, double xmin, double xmax, const wchar_t *label) {
	long i = IntervalTier_timeToIndex (me, xmin); // xmin is in interval i
	if (i < 1) {
		Melder_throw ("Index too low.");
	}

	autoTextInterval newti = TextInterval_create (xmin, xmax, label);
	TextInterval interval = (TextInterval) my intervals -> item[i];
	double xmaxi = interval -> xmax;
	if (xmax > xmaxi) {
		Melder_throw ("Don't know what to do");    // Don't know what to do
	}
	if (xmin == interval -> xmin) {
		if (xmax == interval -> xmax) { // interval already present
			TextInterval_setText (interval, label);
			return;
		}
		// split interval
		interval -> xmin = xmax;
		Collection_addItem (my intervals, newti.transfer());
		return;
	}
	interval -> xmax = xmin;
	Collection_addItem (my intervals, newti.transfer());
	// extra interval when xmax's are not the same
	if (xmax < xmaxi) {
		autoTextInterval newti2 = TextInterval_create (xmax, xmaxi, interval -> text);
		Collection_addItem (my intervals, newti2.transfer());
	}
}

TextGrid TextGrid_readFromTIMITLabelFile (MelderFile file, int phnFile) {
	try {
		double dt = 1.0 / 16000; /* 1 / (TIMIT samplingFrequency) */
		double xmax = dt;
		autofile f = Melder_fopen (file, "r");

		// Ending time will only be known after all labels have been read.
		// We start with a sufficiently long duration (one hour) and correct this later.

		autoTextGrid me = TextGrid_create (0, 3600, L"wrd", 0);
		IntervalTier timit = (IntervalTier) my tiers -> item[1];
		long linesRead = 0;
		char line[200], label[200];
		while (fgets (line, 199, f)) {
			long it1, it2;
			linesRead++;
			if (sscanf (line, "%ld%ld%s", &it1, &it2, label) != 3) {
				Melder_throw ("Incorrect number of items.");
			}
			if (it1 < 0 || it2 <= it1) {
				Melder_throw (L"Incorrect time at line ", linesRead);
			}
			xmax = it2 * dt;
			double xmin = it1 * dt;
			long ni = timit -> intervals -> size - 1;
			if (ni < 1) {
				ni = 1;
				// Some files do not start with a first line "0 <number2> h#".
				// Instead they start with "<number1> <number2> h#", where number1 > 0.
				// We override number1 with 0. */

				if (xmin > 0 && phnFile) {
					xmin = 0;
				}
			}
			TextInterval interval = (TextInterval) timit -> intervals -> item[ni];
			if (xmin < interval -> xmax && linesRead > 1) {
				xmin = interval -> xmax;
				Melder_warning (L"File \"", MelderFile_messageName (file), L"\": Start time set to previous end "
				                 "time for label at line ", Melder_integer (linesRead), L".");
			}
			// standard: new TextInterval
			const char *labelstring = (strncmp (label, "h#", 2) ? label : TIMIT_DELIMITER);
			IntervalTier_add (timit, xmin, xmax, Melder_peekUtf8ToWcs (labelstring));
		}

		// Now correct the end times, based on last read interval.
		// (end time was set to large value!)

		if (timit -> intervals -> size < 2) {
			Melder_throw ("Empty TextGrid");
		}
		Collection_removeItem (timit -> intervals, timit -> intervals -> size);
		TextInterval interval = (TextInterval) timit -> intervals -> item[timit -> intervals -> size];
		timit -> xmax = interval -> xmax;
		my xmax = xmax;
		if (phnFile) { // Create tier 2 with IPA symbols
			autoIntervalTier ipa = Data_copy (timit);
			Thing_setName (ipa.peek(), L"ipa");
			// First change the data in ipa
			for (long i = 1; i <= ipa -> intervals -> size; i++) {
				interval = (TextInterval) timit -> intervals -> item[i];

				TextInterval_setText ( (TextInterval) ipa -> intervals -> item[i],
				                       Melder_peekUtf8ToWcs (timitLabelToIpaLabel (Melder_peekWcsToUtf8 (interval -> text))));
			}
			Collection_addItem (my tiers, ipa.transfer()); // Then: add to collection
			Thing_setName (timit, L"phn");  // rename wrd
		}
		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid not read from file ", file, ".");
	}
}

TextGrid TextGrids_merge (TextGrid me, TextGrid thee) {
	try {
		int at_end = 0, at_start = 1;

		autoTextGrid g1 = Data_copy (me);
		autoTextGrid g2 = Data_copy (thee);

		// The new TextGrid will have the domain
		// [min(g1->xmin, g2->xmin), max(g1->xmax, g2->xmax)]

		double extra_time_end = fabs (g2 -> xmax - g1 -> xmax);
		double extra_time_start = fabs (g2 -> xmin - g1 -> xmin);

		if (g1 -> xmin > g2 -> xmin) {
			TextGrid_extendTime (g1.peek(), extra_time_start, at_start);
		}
		if (g1 -> xmax < g2 -> xmax) {
			TextGrid_extendTime (g1.peek(), extra_time_end, at_end);
		}
		if (g2 -> xmin > g1 -> xmin) {
			TextGrid_extendTime (g2.peek(), extra_time_start, at_start);
		}
		if (g2 -> xmax < g1 -> xmax) {
			TextGrid_extendTime (g2.peek(), extra_time_end, at_end);
		}

		for (long i = 1; i <= g2 -> tiers -> size; i++) {
			autoFunction tier = Data_copy ( (Function) g2 -> tiers -> item [i]);
			Collection_addItem (g1 -> tiers, tier.transfer());
		}
		return g1.transfer();
	} catch (MelderError) {
		Melder_throw (me, " & ", thee, ": not merged.");
	}
}

void IntervalTier_setLaterEndTime (IntervalTier me, double xmax, const wchar_t *mark) {
	try {
		if (xmax <= my xmax) return; // nothing to be done
		TextInterval ti = (TextInterval) my intervals -> item[my intervals -> size];
		Melder_assert (xmax > ti -> xmax);
		if (mark != NULL) {
			autoTextInterval interval = TextInterval_create (ti -> xmax, xmax, mark);
			Collection_addItem (my intervals, interval.transfer());
		} else {
			// extend last interval
			ti -> xmax = xmax;
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (L"Larger end time of IntervalTier not set.");
	}
}

void IntervalTier_setEarlierStartTime (IntervalTier me, double xmin, const wchar_t *mark) {
	try {
		if (xmin >= my xmin) return; // nothing to be done
		TextInterval ti = (TextInterval) my intervals -> item[1];
		Melder_assert (xmin < ti -> xmin);
		if (mark != NULL) {
			autoTextInterval interval = TextInterval_create (xmin, ti -> xmin, mark);
			Collection_addItem (my intervals, interval.transfer());
		} else {
			// extend first interval
			ti -> xmin = xmin;
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (L"Earlier start time of IntervalTier not set.");
	}
}

void IntervalTier_moveBoundary (IntervalTier me, long iint, bool atStart, double newTime) {
    try {
        if (iint < 1 or iint > my intervals -> size) {
            Melder_throw ("Interval out of range.");
        }
        if ((iint == 1 && atStart) or ((iint == my intervals -> size && not atStart))) {
            Melder_throw ("Cannot change the domain.");
        }
        TextInterval interval = (TextInterval) my intervals -> item[iint];
        if (atStart) {
            TextInterval pinterval = (TextInterval) my intervals -> item[iint-1];
            if (newTime <= pinterval -> xmin) {
                Melder_throw ("Cannot move past the start of previous interval.");
            }
            pinterval -> xmax = interval -> xmin = newTime;
        } else {
            TextInterval ninterval = (TextInterval) my intervals -> item[iint+1];
            if (newTime >= ninterval -> xmax) {
                Melder_throw ("Cannot move past the end of next interval.");
            }
            ninterval -> xmin = interval -> xmax = newTime;
        }
    } catch (MelderError) {
        Melder_throw (me, ": boundary not moved.");
    }
}


void TextTier_setLaterEndTime (TextTier me, double xmax, const wchar_t *mark) {
	try {
		if (xmax <= my xmax) return; // nothing to be done
		if (mark != NULL) {
			autoTextPoint textpoint = TextPoint_create (my xmax, mark);
			Collection_addItem (my points, textpoint.transfer());
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (L"Larger end time of TextTier not set.");
	}
}

void TextTier_setEarlierStartTime (TextTier me, double xmin, const wchar_t *mark) {
	try {
		if (xmin >= my xmin) return; // nothing to be done
		if (mark != NULL) {
			autoTextPoint textpoint = TextPoint_create (my xmin, mark);
			Collection_addItem (my points, textpoint.transfer());
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (L"Earlier start time of TextTier not set.");
	}
}

void TextGrid_setEarlierStartTime (TextGrid me, double xmin, const wchar_t *imark, const wchar_t *pmark) {
	try {
		if (xmin >= my xmin) return;
		for (long tierNumber = 1 ; tierNumber <= my tiers -> size; tierNumber++) {
			Function tier = (Function) my tiers -> item [tierNumber];
			if (tier -> classInfo == classIntervalTier) {
				IntervalTier_setEarlierStartTime ((IntervalTier) tier, xmin, imark);

			} else {
				TextTier_setEarlierStartTime ((TextTier) tier, xmin, pmark);
			}
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (L"Earlier start time of TextGrid not set.");
	}
}

void TextGrid_setLaterEndTime (TextGrid me, double xmax, const wchar_t *imark, const wchar_t *pmark) {
	try {
		if (xmax <= my xmax) return;
		for (long tierNumber =1 ; tierNumber <= my tiers -> size; tierNumber++) {
			Function tier = (Function) my tiers -> item [tierNumber];
			if (tier -> classInfo == classIntervalTier) {
				IntervalTier_setLaterEndTime ((IntervalTier) tier, xmax, imark);

			} else {
				TextTier_setLaterEndTime ((TextTier) tier, xmax, pmark);
			}
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (L"Larger end time of TextGrid not set.");
	}
}

void TextGrid_extendTime (TextGrid me, double extra_time, int position) {
	autoTextGrid thee = 0;
	try {
		double xmax = my xmax, xmin = my xmin;
		int at_end = position == 0;

		if (extra_time == 0) {
			return;
		}
		extra_time = fabs (extra_time); // Just in case...
		thee.reset (Data_copy (me));

		if (at_end) {
			xmax += extra_time;
		} else {
			xmin -= extra_time;
		}

		for (long i = 1; i <= my tiers -> size; i++) {
			Function anyTier = (Function) my tiers -> item [i];
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
				autoTextInterval interval = TextInterval_create (tmin, tmax, L"");
				Collection_addItem (tier -> intervals, interval.transfer());
			}
		}
		my xmin = xmin;
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (me, ": time not extended.");
	}
}

void TextGrid_setTierName (TextGrid me, long itier, const wchar_t *newName) {
	try {
		long ntiers = my tiers -> size;

		if (itier < 1 || itier > ntiers) Melder_throw ("Tier number (", itier, ") should not be "
			        "larger than the number of tiers (", ntiers, L").");
		Thing_setName ( (Thing) my tiers -> item [itier], newName);
	} catch (MelderError) {
		Melder_throw (me, ": tier name not set.");
	}
}

static void IntervalTier_cutInterval (IntervalTier me, long index, int extend_option) {
	long size_pre = my intervals -> size;

	/* There always must be at least one interval */
	if (size_pre == 1 || index > size_pre || index < 1) {
		return;
	}

	TextInterval ti = (TextInterval) my intervals -> item[index];
	double xmin = ti -> xmin;
	double xmax = ti -> xmax;
	Collection_removeItem (my intervals, index);
	if (index == 1) { // Change xmin of the new first interval.
		ti = (TextInterval) my intervals -> item[index];
		ti -> xmin = xmin;
	} else if (index == size_pre) { // Change xmax of the new last interval.
		ti = (TextInterval) my intervals -> item[my intervals -> size];
		ti -> xmax = xmax;
	} else {
		if (extend_option == 0) { // extend earlier interval to the right
			ti = (TextInterval) my intervals -> item[index - 1];
			ti -> xmax = xmax;
		} else { // extend next interval to the left
			ti = (TextInterval) my intervals -> item[index];
			ti -> xmin = xmin;
		}
	}
}

void IntervalTier_removeBoundariesBetweenIdenticallyLabeledIntervals (IntervalTier me, const wchar_t *label) {
    try {
        for (long iint = my intervals -> size; iint > 1; iint--) {
            TextInterval ti = (TextInterval) my intervals -> item[iint];
            if (Melder_wcsequ (ti -> text, label)) {
                TextInterval tim1 = (TextInterval) my intervals -> item[iint - 1];
                if (Melder_wcsequ (tim1 -> text, label)) {
                    Melder_free (tim1 -> text);
                    IntervalTier_removeLeftBoundary (me, iint);
                }
            }
        }
    } catch (MelderError) {
        Melder_throw (me, ": boundaries not removed.");
    }
}

void IntervalTier_cutIntervals_minimumDuration (IntervalTier me, const wchar_t *label, double minimumDuration) {
	long i = 1;
	while (i <= my intervals -> size) {
		TextInterval ti = (TextInterval) my intervals -> item[i];
		if ( (label == 0 || (ti -> text != 0 && wcsequ (ti -> text, label))) &&
		        ti -> xmax - ti -> xmin < minimumDuration) {
			IntervalTier_cutInterval (me, i, 0);
		} else {
			i++;
		}
	}
}

void IntervalTier_cutIntervalsOnLabelMatch (IntervalTier me, const wchar_t *label) {
	long i = 1;
	while (i < my intervals -> size) {
		TextInterval ti = (TextInterval) my intervals -> item[i];
		TextInterval tip1 = (TextInterval) my intervals -> item[i + 1];
		if ( (label == 0 || (ti -> text != 0 && wcsequ (ti -> text, label))) &&
		        (Melder_wcscmp (ti -> text, tip1 -> text) == 0)) {

			IntervalTier_cutInterval (me, i, 1);
		} else {
			i++;
		}
	}
}

void IntervalTier_changeLabels (I, long from, long to, const wchar_t *search, const wchar_t *replace, int use_regexp, long *nmatches, long *nstringmatches) {
	iam (IntervalTier);
	try {
		if (from == 0) {
			from = 1;
		}
		if (to == 0) {
			to = my intervals -> size;
		}
		if (from > to || from < 1 || to > my intervals -> size) {
			Melder_throw ("Incorrect specification of where to act.");
		}
		if (use_regexp && wcslen (search) == 0) Melder_throw ("The regex search string cannot be empty.\n"
			        "You may search for an empty string with the expression \"^$\"");

		long nlabels = to - from + 1;
		autoNUMvector<wchar_t *> labels (1, nlabels);

		for (long i = from; i <= to; i++) {
			TextInterval interval = (TextInterval) my intervals -> item[i];
			labels[i - from + 1] = interval -> text;   // Shallow copy.
		}
		autostringvector newlabels (strs_replace (labels.peek(), 1, nlabels, search, replace, 0, nmatches, nstringmatches, use_regexp), 1, nlabels);

		for (long i = from; i <= to; i++) {
			TextInterval interval = (TextInterval) my intervals -> item[i];
			Melder_free (interval -> text);
			interval -> text = newlabels[i - from + 1];   // Transfer of ownership.
			newlabels[i - from + 1] = 0;
		}
	} catch (MelderError) {
		Melder_throw (me, ": labels not changed.");
	}
}

void TextTier_changeLabels (I, long from, long to, const wchar_t *search, const wchar_t *replace, int use_regexp, long *nmatches, long *nstringmatches) {
	iam (TextTier);
	try {
		if (from == 0) {
			from = 1;
		}
		if (to == 0) {
			to = my points -> size;
		}
		if (from > to || from < 1 || to > my points -> size) {
			Melder_throw ("Incorrect specification of where to act.");
		}
		if (use_regexp && wcslen (search) == 0) Melder_throw ("The regex search string cannot be empty.\n"
			        "You may search for an empty string with the expression \"^$\"");

		long nmarks = to - from + 1;
		autoNUMvector<wchar_t *> marks (1, nmarks);

		for (long i = from; i <= to; i++) {
			TextPoint point = (TextPoint) my points -> item[i];
			marks[i - from + 1] = point -> mark;   // Shallow copy.
		}
		autostringvector newmarks (strs_replace (marks.peek(), 1, nmarks, search, replace, 0, nmatches, nstringmatches, use_regexp), 1, nmarks);

		for (long i = from; i <= to; i++) {
			TextPoint point = (TextPoint) my points -> item[i];
			Melder_free (point -> mark);
			point -> mark = newmarks[i - from + 1];   // Transfer of ownership.
			newmarks[i - from + 1] = 0;
		}
	} catch (MelderError) {
		Melder_throw (me, ": no labels changed.");
	}
}

void TextGrid_changeLabels (TextGrid me, int tier, long from, long to, const wchar_t *search, const wchar_t *replace, int use_regexp, long *nmatches, long *nstringmatches) {
	try {
		long ntiers = my tiers -> size;

		if (tier < 1 || tier > ntiers) Melder_throw ("The tier number (", tier, ") should not be "
			        "larger than the number of tiers (", ntiers, ").");
		if (use_regexp && wcslen (search) == 0) Melder_throw ("The regex search string cannot be empty.\n"
			        "You may search for an empty string with the expression \"^$\"");
		Data anyTier = (Data) my tiers -> item [tier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier_changeLabels (anyTier, from, to, search, replace, use_regexp, nmatches, nstringmatches);
		} else {
			TextTier_changeLabels (anyTier, from, to, search, replace, use_regexp, nmatches, nstringmatches);
		}
	} catch (MelderError) {
		Melder_throw (me, ": labels not changed");
	}
}

// Precondition: if (preserveTimes) { my xmax <= thy xmin }
// Postcondition: my xmin preserved
void IntervalTiers_append_inline (IntervalTier me, IntervalTier thee, bool preserveTimes) {
	try {
        double xmax_previous = my xmax, time_shift = my xmax - thy xmin;
		if (preserveTimes && my xmax < thy xmin) {
			autoTextInterval connection = TextInterval_create (my xmax, thy xmin, L"");
            xmax_previous = thy xmin;
			Collection_addItem (my intervals, connection.transfer());
		}
		for (long iint = 1; iint <= thy intervals -> size; iint++) {
			autoTextInterval ti = (TextInterval) Data_copy ((Data) thy intervals -> item[iint]);
			if (preserveTimes) {
				Collection_addItem (my intervals, ti.transfer());
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
					Collection_addItem (my intervals, ti.transfer());
					xmax_previous = xmax;
				}
				// else don't include interval
            }
		}
		my xmax = preserveTimes ? thy xmax : xmax_previous;
	} catch (MelderError) {
		Melder_throw ("IntervalTiers not appended.");
	}
}

// Precondition: if (preserveTimes) { my xmax <= thy xmin }
void TextTiers_append_inline (TextTier me, TextTier thee, bool preserveTimes) {
	try {
		for (long iint = 1; iint <= thy points -> size; iint++) {
			autoTextPoint tp = (TextPoint) Data_copy ((Data) thy points -> item[iint]);
			if (not preserveTimes) {
				tp -> number += my xmax - thy xmin;
			}
			Collection_addItem (my points, tp.transfer());
		}
		my xmax = preserveTimes ? thy xmax : my xmax + (thy xmax - thy xmin);
	} catch (MelderError) {
		Melder_throw ("TextTiers not appended.");
	}
}

void TextGrids_append_inline (TextGrid me, TextGrid thee, bool preserveTimes)
{
	try {
		if (my tiers -> size != thy tiers -> size) {
			Melder_throw ("The number of tiers must be equal.");
		}
		if (preserveTimes && thy xmin < my xmax) {
			Melder_throw ("The start time of the second TextGrid can't be earlier than the end time of the first one if you want to preserve times.");
		}
		// all tiers must have the same end time
		// last intervals must have the same end time
		double xmax = preserveTimes ? thy xmax : my xmax + (thy xmax - thy xmin);
		for (long itier = 1; itier <= my tiers -> size; itier++) {
			Function myTier = (Function) my tiers -> item[itier], thyTier = (Function) thy tiers -> item[itier];
			if (myTier -> classInfo == classIntervalTier && thyTier -> classInfo == classIntervalTier) {
                IntervalTier ti = (IntervalTier) myTier;
				IntervalTiers_append_inline (ti, (IntervalTier) thy tiers -> item[itier], preserveTimes);
                // make sure last interval has correct end tTime
                TextInterval last = (TextInterval) ti -> intervals -> item [ti -> intervals -> size];
                last -> xmax = xmax;
                Melder_assert (last -> xmax > last -> xmin);
			} else if (myTier -> classInfo == classTextTier && thyTier -> classInfo == classTextTier) {
                TextTier ti = (TextTier) myTier;
				TextTiers_append_inline (ti, (TextTier) thy tiers -> item [itier], preserveTimes);
                ti -> xmax = xmax;
			} else {
				Melder_throw ("Tier number ", Melder_integer (itier), " in the second TextGrid is of different type as the corresponding tier in the first TextGrid.");
			}
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw ("TextGrids not appended.");
	}
}

TextGrid TextGrids_to_TextGrid_appendContinuous (Collection me, bool preserveTimes) {
	try {
		if (my size == 1) {
			return (TextGrid) Data_copy ((Data) my item[1]);
		}
		autoTextGrid thee = (TextGrid) Data_copy ((Data) my item[1]);
		for (long igrid = 2; igrid <= my size; igrid++) {
			TextGrids_append_inline (thee.peek(), (TextGrid) my item[igrid], preserveTimes);
		}
		if (not preserveTimes) Function_shiftXBy ((Function) thee.peek(), -thy xmin);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("No aligned TextGrid created from Collection.");
	}
}

/* End of file TextGrid_extensions.cpp */
