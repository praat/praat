/* TextGrid_extensions.cpp
 *
 * Copyright (C) 1993-2019 David Weenink, Paul Boersma 2019
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

#include "TextGrid_extensions.h"
#include "NUM2.h"

const struct TIMIT_key {
	const char *timitLabel, *ipaLabel;
} TIMIT_toIpaTable[] = {
	{"", ""},
	/* Vowels */
	{"iy", "i"},			/* beet: bcl b IY tcl t */
	{"ih", "\\ic"}, 		/* bit: bcl b IH tcl t */
	/* 20190704 wgmichener "\\ep" -> "\\ef" */
	{"eh", "\\ef"}, 		/* bet: bcl b EH tcl t */
	{"ey", "e"},  			/* bait: bcl b EY tcl t */
	{"ae", "\\ae"},  		/* bat: bcl b AE tcl t */
	{"aa", "\\as"}, 		/* bott: bcl b AA tcl t */
	{"aw", "a\\hs"},  		/* bout: bcl b AW tcl t */
	{"ay", "a\\ic"},  		/* bite: bcl b AY tcl t */
	{"ah", "\\vt"}, 		/* but: bcl b AH tcl t */
	{"ao", "\\ct"},  		/* bought: bcl b AO tcl t */
	{"oy", "\\ct\\ic"},		/* boy: bcl b OY */
	{"ow", "o"}, 			/* boat: bcl b OW tcl t */
	{"uh", "\\hs"}, 		/* book: bcl b UH tcl t */
	{"uw", "u"},  			/* boot: bcl b UW tcl t */
	/* fronted allophone of uw (alveolar contexts) */
	/* 20190704 wgmichener "\\u\"" -> "\\u\\:^" */
	{"ux", "\\u\\:^"}, 		/* toot: tcl t UX tcl t */
	{"er", "\\er\\hr"},		/* bird: bcl b ER dcl d */
	{"ax", "\\sw"}, 		/* about: AX bcl b aw tcl t */
	{"ix", "\\i-"}, 		/* debit: dcl d eh bcl b IX tcl t */
	{"axr", "\\sr"}, 		/* butter: bcl ah dx AXR */
	/* devoiced schwa, very short */
	/* 20190704 wgmichener "\\sw\\ov"" -> "\\sw\\0v" */
	{"ax-h", "\\sw\\0v"}, 	/* suspect: s AX-H s pcl p eh kcl k tcl t */
	/* Semivowels and glides */
	{"l", "l"},				/* lay:	L ey */
	/* 20190704 wgmichener "r" -> "\\rt" */
	{"r", "\\rt"},			/* ray:	R ey */
	{"w", "w"},				/* way:	w ey */
	{"y", "j"},				/* yacht: Y aa tcl t */
	{"hh", "h" },		/* hay: HH ey*/
	/* voiced allophone of h */
	/* 20190704 wgmichener "\\hv" -> "\\h^" */
	{"hv", "\\h^"},			/* ahead: ax HV eh dcl d */
	{"el", "l\\|v"},		/* bottle: bcl b aa tcl t EL */
	/* Nasals */
	{"m", "m"},				/* mom:	M aa M */
	{"n", "n"},				/* noon: N uw N*/
	{"ng", "\\ng"},			/* sing: s ih NG */
	{"em", "m\\|v"},		/* bottom: b aa tcl t EM */
	{"en", "n\\|v"},		/* button:	b ah q EN */
	{"eng", "\\ng\\|v"},	/* washington: w aa sh ENG tcl t ax n */
	/* nasal flap */
	/* 20190704 wgmichener "n^\\fh" -> "\\fh\\~^" */
	{"nx", "\\fh\\~^"},		/* winner: wih NX axr */
	/* Fricatives */
	{"s", "s"},				/* sea: S iy */
	{"sh", "\\sh"},			/* she: SH iy */
	{"z", "z"},				/* zone: Z ow n */
	{"zh", "\\zh"},			/* azure: ae ZH er */
	{"f", "f"},				/* fin: F ih n */
	/* 20190704 wgmichener "\\te" -> "\\tf" */
	{"th", "\\tf"},			/* thin: TH ih n */
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
	/* 20190704 wgmichener "" -> "x\\cn" */
	{"bcl", "b\\cn"},
	{"dcl", "d\\cn"},
	{"gcl", "g\\cn"},
	{"pcl", "p\\cn"},
	{"tcl", "t\\cn"},
	{"kcl", "k\\cn"},
	/* flap */
	{"dx", "\\fh"},			/* muddy: m ah DX iy & dirty: dcl d er DX iy */
	/* glottal stop */
	/* 20190704 wgmichener "?" -> "\\?g" */
	{"q", "\\?g"},
	/* Others */
	{"pau", ""},	/* pause */
	{"epi", ""},	/* epenthetic silence */
	{"h#", ""}, 	/* marks start and end piece of sentence */
	/* the following markers only occur in the dictionary */
	/* 20190704 wgmichener "1" -> "\\'1" */
	{"1", "\\'1"},		/* primary stress marker */
	/* 20190704 wgmichener "2" -> "\\'2" */
	{"2", "\\'2"}		/* secondary stress marker */
};

#define TIMIT_NLABELS (sizeof TIMIT_toIpaTable / sizeof TIMIT_toIpaTable[1] - 1)
static const char *TIMIT_DELIMITER = "h#";

static const char *timitLabelToIpaLabel (const char timitLabel[]) {
	for (integer i = 1; i <= TIMIT_NLABELS; i++)
		if (!strcmp (TIMIT_toIpaTable[i].timitLabel, timitLabel))
			return TIMIT_toIpaTable[i].ipaLabel;
	return timitLabel;
}

static bool isTimitPhoneticLabel (const char label[]) {
	for (integer i = 1; i <= TIMIT_NLABELS; i++)
		if (! strcmp (TIMIT_toIpaTable[i].timitLabel, label))
			return true;
	return false;
}

static bool isTimitWord (const char label[]) {
	const char *p = label;
	for (; *p; p++)
		if (Melder_isUpperCaseLetter (*p))
			return false;
	return true;
}

autoDaata TextGrid_TIMITLabelFileRecognizer (integer nread, const char *header, MelderFile file) {
	char hkruis[3] = "h#", label1[512], label2[512];
	int length;
	bool phnFile = false;
	long_not_integer it [5]; // because of %ld in sscanf we need an explicit long
	if (nread < 12 || sscanf (header, "%ld%ld%511s%n\n", & it [1], & it [2], label1, & length) != 3 ||
		it [1] < 0 || it [2] <= it [1] || sscanf (& header[length], "%ld%ld%511s\n", & it [3], & it [4], label2) != 3 ||
		it [4] <= it [3]) {
		/*
			20120512 djmw removed the extra "it [3] < it [2]" check, because otherwise train/dr7/mdlm0/si1864.wrd cannot be read
		*/
		return autoDaata ();
	}
	if (! strcmp (label1, hkruis)) {
		if (isTimitPhoneticLabel (label2))
			phnFile = true;
		else if (! isTimitWord (label2))
			return autoDaata ();
	} else if (! isTimitWord (label1) || ! isTimitWord (label2)) {
		return autoDaata ();
	}
	autoTextGrid thee = TextGrid_readFromTIMITLabelFile (file, phnFile);
	return thee.move();
}

static void IntervalTier_add (IntervalTier me, double xmin, double xmax, conststring32 label) {
	const integer i = IntervalTier_timeToIndex (me, xmin); // xmin is in interval i
	Melder_require (i > 0,
		U"Index too low.");
	autoTextInterval newti = TextInterval_create (xmin, xmax, label);
	const TextInterval interval = my intervals.at [i];
	const double xmaxi = interval -> xmax;
	Melder_require (xmax <= xmaxi,
		U"Don't know what to do");
	if (xmin == interval -> xmin) {
		if (xmax == interval -> xmax) { // interval already present
			TextInterval_setText (interval, label);
			return;
		}
		/*
			Split interval
		*/
		interval -> xmin = xmax;
		my intervals. addItem_move (newti.move());
		return;
	}
	interval -> xmax = xmin;
	my intervals. addItem_move (newti.move());
	/*
		Extra interval when xmax's are not the same
	*/
	if (xmax < xmaxi) {
		autoTextInterval newti2 = TextInterval_create (xmax, xmaxi, interval -> text.get());
		my intervals. addItem_move (newti2.move());
	}
}

autoTextGrid TextGrid_readFromTIMITLabelFile (MelderFile file, bool phnFile) {
	try {
		const double dt = 1.0 / 16000.0; // TIMIT samplingFrequency)
		double xmax = dt;
		autofile f = Melder_fopen (file, "r");
		/*
			Ending time will only be known after all labels have been read.
			We start with a sufficiently long duration (one hour) and correct this later.
		*/
		autoTextGrid me = TextGrid_create (0.0, 3600.0, U"wrd", 0);
		const IntervalTier timit = (IntervalTier) my tiers->at [1];
		integer linesRead = 0;
		char line[200], label[200];
		while (fgets (line, 199, f)) {
			long_not_integer it1, it2; // because of %ld in sscanf we need an explicit long
			linesRead++;
			Melder_require (sscanf (line, "%ld%ld%199s", & it1, & it2, label) == 3,
				U"Incorrect number of items.");
			Melder_require (it1 >= 0 && it1 < it2,
				U"Incorrect time at line ", linesRead);
			
			xmax = it2 * dt;
			double xmin = it1 * dt;
			integer ni = timit -> intervals.size - 1;
			if (ni < 1) {
				ni = 1;
				/*
					Some files do not start with a first line "0 <number2> h#".
					Instead they start with "<number1> <number2> h#", where number1 > 0.
					We override number1 with 0.
				*/
				if (xmin > 0.0 && phnFile)
					xmin = 0.0;
			}
			const TextInterval interval = timit -> intervals.at [ni];
			if (xmin < interval -> xmax && linesRead > 1) {
				xmin = interval -> xmax;
				Melder_warning (U"File \"", MelderFile_messageName (file),
					U"\": Start time set to previous end time for label at line ", linesRead, U".");
			}
			/*
				Standard: new TextInterval
			*/
			const char *labelstring = (strncmp (label, "h#", 2) ? label : TIMIT_DELIMITER);
			IntervalTier_add (timit, xmin, xmax, Melder_peek8to32 (labelstring));
		}
		/*
			Now correct the end times, based on last read interval.
			(end time was set to large value!)
		*/
		Melder_require (timit -> intervals.size > 1,
			U"Empty TextGrid.");
		timit -> intervals. removeItem (timit -> intervals.size);
		TextInterval interval = timit -> intervals.at [timit -> intervals.size];
		timit -> xmax = interval -> xmax;
		my xmax = xmax;
		if (phnFile) { // Create tier 2 with IPA symbols
			autoIntervalTier ipa = Data_copy (timit);
			Thing_setName (ipa.get(), U"ipa");
			/*
				First change the data in ipa
			*/
			for (integer i = 1; i <= ipa -> intervals.size; i ++) {
				interval = timit -> intervals.at [i];
				TextInterval_setText (ipa -> intervals.at [i],
					Melder_peek8to32 (timitLabelToIpaLabel (Melder_peek32to8 (interval -> text.get()))));
			}
			my tiers -> addItem_move (ipa.move());
			Thing_setName (timit, U"phn");
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
		/*
			The new TextGrid will have the domain
			[min(g1->xmin, g2->xmin), max(g1->xmax, g2->xmax)]
		*/
		const double extra_time_end = fabs (g2 -> xmax - g1 -> xmax);
		const double extra_time_start = fabs (g2 -> xmin - g1 -> xmin);

		if (g1 -> xmin > g2 -> xmin)
			TextGrid_extendTime (g1.get(), extra_time_start, at_start);
		if (g1 -> xmax < g2 -> xmax)
			TextGrid_extendTime (g1.get(), extra_time_end, at_end);
		if (g2 -> xmin > g1 -> xmin)
			TextGrid_extendTime (g2.get(), extra_time_start, at_start);
		if (g2 -> xmax < g1 -> xmax)
			TextGrid_extendTime (g2.get(), extra_time_end, at_end);
		for (integer i = 1; i <= g2 -> tiers->size; i ++) {
			autoFunction tier = Data_copy (g2 -> tiers->at [i]);
			g1 -> tiers -> addItem_move (tier.move());
		}
		return g1;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": not merged.");
	}
}

void IntervalTier_setLaterEndTime (IntervalTier me, double xmax, conststring32 mark) {
	try {
		if (xmax <= my xmax)
			return; // nothing to be done
		Melder_assert (my intervals.size > 0);
		const TextInterval ti = my intervals.at [my intervals.size];
		/*
			The following assert signals that the IntervalTier is not correct:
			the xmax of the last inteval is not equal to the xmax of the IntervalTier.
		*/
		Melder_assert (xmax > ti -> xmax);
		if (mark) {
			autoTextInterval interval = TextInterval_create (ti -> xmax, xmax, mark);
			my intervals. addItem_move (interval.move());
		} else {
			/*
				Extend last interval
			*/
			ti -> xmax = xmax;
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"Larger end time of IntervalTier not set.");
	}
}

void IntervalTier_setEarlierStartTime (IntervalTier me, double xmin, conststring32 mark) {
	try {
		if (xmin >= my xmin)
			return;
		Melder_assert (my intervals.size > 0);
		const TextInterval ti = my intervals.at [1];
		Melder_assert (xmin < ti -> xmin);
		if (mark) {
			autoTextInterval interval = TextInterval_create (xmin, ti -> xmin, mark);
			my intervals. addItem_move (interval.move());
		} else {
			/*
				Extend first interval
			*/
			ti -> xmin = xmin;
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (U"Earlier start time of IntervalTier not set.");
	}
}

void IntervalTier_moveBoundary (IntervalTier me, integer iint, bool atStart, double newTime) {
    try {
		Melder_require (iint >= 1 && iint <= my intervals.size,
            U"The interval number is out of the valid range.");
		Melder_require (! ((iint == 1 && atStart) or (iint == my intervals.size && ! atStart)),
			U"Cannot change the domain.");
        TextInterval interval = my intervals.at [iint];
        if (atStart) {
            const TextInterval pinterval = my intervals.at [iint-1];
			Melder_require (newTime > pinterval -> xmin,
				U"Cannot move past the start of previous interval.");
            pinterval -> xmax = interval -> xmin = newTime;
        } else {
            const TextInterval ninterval = my intervals.at [iint+1];
			Melder_require (newTime < ninterval -> xmax,
				U"Cannot move past the end of next interval.");
            ninterval -> xmin = interval -> xmax = newTime;
        }
    } catch (MelderError) {
        Melder_throw (me, U": boundary not moved.");
    }
}


void TextTier_setLaterEndTime (TextTier me, double xmax, conststring32 mark) {
	try {
		if (xmax <= my xmax)
			return;
		if (mark) {
			autoTextPoint textpoint = TextPoint_create (my xmax, mark);
			my points. addItem_move (textpoint.move());
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"Larger end time of TextTier not set.");
	}
}

void TextTier_setEarlierStartTime (TextTier me, double xmin, conststring32 mark) {
	try {
		if (xmin >= my xmin)
			return;
		if (mark) {
			autoTextPoint textpoint = TextPoint_create (my xmin, mark);
			my points. addItem_move (textpoint.move());
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (U"Earlier start time of TextTier not set.");
	}
}

void TextGrid_setEarlierStartTime (TextGrid me, double xmin, conststring32 intervalMark, conststring32 pointMark) {
	try {
		if (xmin >= my xmin)
			return;
		for (integer tierNumber = 1 ; tierNumber <= my tiers->size; tierNumber ++) {
			const Function tier = my tiers->at [tierNumber];
			if (tier -> classInfo == classIntervalTier)
				IntervalTier_setEarlierStartTime ((IntervalTier) tier, xmin, intervalMark);
			else
				TextTier_setEarlierStartTime ((TextTier) tier, xmin, pointMark);
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (U"Earlier start time of TextGrid not set.");
	}
}

void TextGrid_setLaterEndTime (TextGrid me, double xmax, conststring32 intervalMark, conststring32 pointMark) {
	try {
		if (xmax <= my xmax)
			return;
		for (integer tierNumber = 1 ; tierNumber <= my tiers->size; tierNumber ++) {
			const Function tier = my tiers->at [tierNumber];
			if (tier -> classInfo == classIntervalTier)
				IntervalTier_setLaterEndTime ((IntervalTier) tier, xmax, intervalMark);
			else
				TextTier_setLaterEndTime ((TextTier) tier, xmax, pointMark);
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

		if (extra_time == 0.0)
			return;
		extra_time = fabs (extra_time);   // just in case
		thee = Data_copy (me);

		if (at_end)
			xmax += extra_time;
		else
			xmin -= extra_time;

		for (integer i = 1; i <= my tiers->size; i ++) {
			const Function anyTier = my tiers->at [i];
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
				const IntervalTier tier = (IntervalTier) anyTier;
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

void TextGrid_setTierName (TextGrid me, integer itier, conststring32 newName) {
	try {
		Melder_require (itier >= 1 && itier <= my tiers->size,
			U"The tier number (", itier, U") should not be larger than the number of tiers (", my tiers->size, U").");
		Thing_setName (my tiers->at [itier], newName);
	} catch (MelderError) {
		Melder_throw (me, U": tier name not set.");
	}
}

static void IntervalTier_cutInterval (IntervalTier me, integer index, int extend_option) {
	integer size_pre = my intervals.size;
	/*
		There always should be at least one interval
	 */
	if (size_pre == 1 || index > size_pre || index < 1)
		return;

	TextInterval ti = my intervals.at [index];
	const double xmin = ti -> xmin;
	const double xmax = ti -> xmax;
	my intervals. removeItem (index);
	if (index == 1) {
		/*
			Change xmin of the new first interval.
		 */
		ti = my intervals.at [index];
		ti -> xmin = xmin;
	} else if (index == size_pre) { 
		/*
			Change xmax of the new last interval.
		 */
		ti = my intervals.at [my intervals.size];
		ti -> xmax = xmax;
	} else {
		if (extend_option == 0) { 
			/*
				Extend earlier interval to the right
			 */
			ti = my intervals.at [index - 1];
			ti -> xmax = xmax;
		} else {
			/*
				Extend next interval to the left
			 */
			ti = my intervals.at [index];
			ti -> xmin = xmin;
		}
	}
}

void IntervalTier_removeBoundariesBetweenIdenticallyLabeledIntervals (IntervalTier me, conststring32 label) {
    try {
		for (integer iinterval = my intervals.size; iinterval > 1; iinterval --) {
			const TextInterval thisInterval = my intervals.at [iinterval];
			if (Melder_equ (thisInterval -> text.get(), label)) {
				const TextInterval previousInterval = my intervals.at [iinterval - 1];
				if (Melder_equ (previousInterval -> text.get(), label)) {
					previousInterval -> text. reset();
					IntervalTier_removeLeftBoundary (me, iinterval);
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": boundaries not removed.");
	}
}

void IntervalTier_cutIntervals_minimumDuration (IntervalTier me, conststring32 label, double minimumDuration) {
	integer iinterval = 1;
	while (iinterval <= my intervals.size) {
		const TextInterval interval = my intervals.at [iinterval];
		if ((! label || (interval -> text && str32equ (interval -> text.get(), label))) &&
			interval -> xmax - interval -> xmin < minimumDuration)
			IntervalTier_cutInterval (me, iinterval, 0);
		else
			iinterval ++;
	}
}

void IntervalTier_cutIntervalsOnLabelMatch (IntervalTier me, conststring32 label) {
	integer iinterval = 1;
	while (iinterval < my intervals.size) {
		const TextInterval thisInterval = my intervals.at [iinterval];
		const TextInterval nextInterval = my intervals.at [iinterval + 1];
		if ((! label || (thisInterval -> text && str32equ (thisInterval -> text.get(), label))) &&
			Melder_equ (thisInterval -> text.get(), nextInterval -> text.get()))
			IntervalTier_cutInterval (me, iinterval, 1);
		else
			iinterval ++;
	}
}

void IntervalTier_changeLabels (IntervalTier me, integer from, integer to,
	conststring32 search, conststring32 replace, bool use_regexp, integer *nmatches, integer *nstringmatches) {
	try {
		if (from == 0)
			from = 1;
		if (to == 0)
			to = my intervals.size;
		Melder_require (to >= from && from >= 1 && to <= my intervals.size,
			U"Incorrect specification of where to act.");
		Melder_require (! (use_regexp && search [0] == U'\0'),
			U"The regex search string cannot be empty.\nYou may search for an empty string with the expression \"^$\"");
		const integer offset = from - 1, nlabels = to - offset;
		autovector <conststring32> labels = newvectorzero <conststring32> (nlabels);
		for (integer i = from; i <= to; i ++) {
			const TextInterval interval = my intervals.at [i];
			labels [i - offset] = interval -> text.get();   // shallow copy
		}
		autoSTRVEC newLabels = string32vector_searchAndReplace (
			constSTRVEC (& labels [1], nlabels),
			//labels.get(),
			search, replace, 0, nmatches, nstringmatches, use_regexp
		);
		for (integer i = from; i <= to; i ++) {
			const TextInterval interval = my intervals.at [i];
			interval -> text = newLabels [i - offset].move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": labels not changed.");
	}
}

void TextTier_changeLabels (TextTier me, integer from, integer to,
	conststring32 search, conststring32 replace, bool use_regexp, integer *nmatches, integer *nstringmatches)
{
	try {
		if (from == 0)
			from = 1;
		if (to == 0)
			to = my points.size;
		Melder_require (to >= from && from >= 1 && to <= my points.size,
			U"Incorrect specification of where to act.");
		Melder_require (! (use_regexp && search [0] == U'\0'),
			U"The regex search string cannot be empty.\nTo search for an empty string, use the expression \"^$\" instead.");
		const integer offset = from - 1, nmarks = to - offset;
		autovector <conststring32> marks = newvectorzero <conststring32> (nmarks);   // a non-owning vector of strings
		for (integer i = from; i <= to; i ++) {
			TextPoint point = my points.at [i];
			marks [i - offset] = point -> mark.get();   // reference copy
		}
		autoSTRVEC newMarks = string32vector_searchAndReplace (
				constSTRVEC (& marks [1], nmarks),
				search, replace, 0, nmatches, nstringmatches, use_regexp);
		for (integer i = from; i <= to; i ++) {
			TextPoint point = my points.at [i];
			point -> mark = newMarks [i - offset].move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": no labels changed.");
	}
}

void TextGrid_changeLabels (TextGrid me, integer tier, integer from, integer to,
	conststring32 search, conststring32 replace, bool use_regexp, integer *nmatches, integer *nstringmatches)
{
	try {
		const integer ntiers = my tiers->size;
		Melder_require (tier > 0 && tier <= ntiers,
			U"The tier number (", tier, U") should not be larger than the number of tiers (", ntiers, U").");
		Melder_require (! (use_regexp && search [0] == U'\0'), 
			U"The regex search string should not be empty.\nTo search for an empty string, use the expression \"^$\"");

		const Function anyTier = my tiers->at [tier];
		if (anyTier -> classInfo == classIntervalTier)
			IntervalTier_changeLabels ((IntervalTier) anyTier, from, to, search, replace, use_regexp, nmatches, nstringmatches);
		else
			TextTier_changeLabels ((TextTier) anyTier, from, to, search, replace, use_regexp, nmatches, nstringmatches);
	} catch (MelderError) {
		Melder_throw (me, U": labels not changed.");
	}
}

static void IntervalTier_checkStartAndEndTime (IntervalTier me) {
	Melder_assert (my intervals.size > 0);
	const TextInterval first = my intervals.at [1];
	Melder_require (my xmin == first -> xmin,
		U": start time of first interval doesn't match start time of the tier.");
	const TextInterval last = my intervals.at [my intervals.size];
	Melder_require (my xmax == last -> xmax,
		U": end time of last interval doesn't match end time of the tier.");
}

// Precondition: if (preserveTimes) { my xmax <= thy xmin }
// Postcondition: my xmin preserved
void IntervalTiers_append_inplace (IntervalTier me, IntervalTier thee, bool preserveTimes) {
	try {
		IntervalTier_checkStartAndEndTime (me); // start/end time of first/last interval should match with tier
		IntervalTier_checkStartAndEndTime (thee);
		const double time_shift = my xmax - thy xmin;
        double xmax_previous = my xmax;
		if (preserveTimes && my xmax < thy xmin) {
			autoTextInterval connection = TextInterval_create (my xmax, thy xmin, U"");
            xmax_previous = thy xmin;
			my intervals. addItem_move (connection.move());
		}
		for (integer iint = 1; iint <= thy intervals.size; iint ++) {
			autoTextInterval ti = Data_copy (thy intervals.at [iint]);
			if (preserveTimes) {
				my intervals. addItem_move (ti.move());
			} else {
				/*
					The interval could be so short that if we test ti -> xmin < ti->xmax it might be true
					but after assigning ti->xmin = xmax_previous and ti->xmax += time_shift the test
					ti -> xmin < ti->xmax might be false!
					We want to make sure xmin and xmax are not register variables and therefore force
					double64 by using volatile variables.
		 		 */
				volatile double xmin = xmax_previous;
				volatile double xmax = ti -> xmax + time_shift;
				if (xmin < xmax) {
					ti -> xmin = xmin;
					ti -> xmax = xmax;
					my intervals. addItem_move (ti.move());
					xmax_previous = xmax;
				}
				/*
					Else don't include interval
				*/
            }
		}
		my xmax = preserveTimes ? thy xmax : xmax_previous;
	} catch (MelderError) {
		Melder_throw (U"IntervalTiers not appended.");
	}
}

// Precondition: if (preserveTimes) { my xmax <= thy xmin }
void TextTiers_append_inplace (TextTier me, TextTier thee, bool preserveTimes) {
	try {
		for (integer iint = 1; iint <= thy points.size; iint ++) {
			autoTextPoint tp = Data_copy (thy points.at [iint]);
			if (! preserveTimes)
				tp -> number += my xmax - thy xmin;
			my points. addItem_move (tp.move());
		}
		my xmax = preserveTimes ? thy xmax : my xmax + (thy xmax - thy xmin);
	} catch (MelderError) {
		Melder_throw (U"TextTiers not appended.");
	}
}

static void TextGrid_checkStartAndEndTimesOfTiers (TextGrid me) {
	for (integer itier = 1; itier <= my tiers->size; itier ++) {
		const Function tier = my tiers->at [itier];
		Melder_require (tier -> xmin == my xmin,
			U": the start time of tier ", itier, U" does not match the start time of its TextGrid.");
		Melder_require (tier -> xmax == my xmax,
			U": the end time of tier ", itier, U" does not match the end time of its TextGrid.");
	}
}

void TextGrids_append_inplace (TextGrid me, TextGrid thee, bool preserveTimes)
{
	try {
		Melder_require (my tiers->size == thy tiers->size,
			U"The numbers of tiers should be equal.");
		Melder_require (! (preserveTimes && thy xmin < my xmax),
			U"The start time of the second TextGrid can't be earlier than the end time of the first one if you want to preserve times.");
		TextGrid_checkStartAndEndTimesOfTiers (me); // all tiers must have the same start/end time as textgrid
		TextGrid_checkStartAndEndTimesOfTiers (thee);
		/*
			Last intervals must have the same end time
		*/
		const double xmax = preserveTimes ? thy xmax : my xmax + (thy xmax - thy xmin);
		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			const Function myTier = my tiers->at [itier], thyTier = thy tiers->at [itier];
			if (myTier -> classInfo == classIntervalTier && thyTier -> classInfo == classIntervalTier) {
				const IntervalTier  myIntervalTier = static_cast <IntervalTier>  (myTier);
				const IntervalTier thyIntervalTier = static_cast <IntervalTier> (thyTier);
				IntervalTiers_append_inplace (myIntervalTier, thyIntervalTier, preserveTimes);
				/*
					Because of floating-point rounding errors, we explicitly make sure that
					both the xmax of the tier and the xmax of the last interval equal the xmax of the grid.
				*/
				myIntervalTier -> xmax = xmax;
                const TextInterval lastInterval = myIntervalTier -> intervals.at [myIntervalTier -> intervals.size];
                lastInterval -> xmax = xmax;
                Melder_assert (lastInterval -> xmax > lastInterval -> xmin);
			} else if (myTier -> classInfo == classTextTier && thyTier -> classInfo == classTextTier) {
				const TextTier  myTextTier = static_cast <TextTier>  (myTier);
				const TextTier thyTextTier = static_cast <TextTier> (thyTier);
				TextTiers_append_inplace (myTextTier, thyTextTier, preserveTimes);
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
		for (integer igrid = 2; igrid <= my size; igrid ++)
			TextGrids_append_inplace (thee.get(), my at [igrid], preserveTimes);
		if (! preserveTimes)
			Function_shiftXBy (thee.get(), -thy xmin);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No aligned TextGrid created from Collection.");
	}
}

double TextGrid_getTotalDurationOfIntervalsWhere (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion) {
	try {
		longdouble totalDuration = 0.0;
		const IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			const TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text.get(), which, criterion, true))
				totalDuration += interval -> xmax - interval -> xmin;
		}
		return totalDuration;
	} catch (MelderError) {
		Melder_throw (me, U": interval durations not counted.");
	}
}

/* End of file TextGrid_extensions.cpp */
