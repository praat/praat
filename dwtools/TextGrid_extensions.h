#ifndef _TextGrid_extensions_h_
#define _TextGrid_extensions_h_
/* TextGrid_extensions.h
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20020516 GPL header
 djmw 20120418 Latest modification
*/

#include "TextGrid.h"

TextGrid TextGrid_readFromTIMITLabelFile (MelderFile file, int phnFile);
/*
	Read TIMIT label file with the following structure:
		samplenumber1 samplenumber2 label1
		samplenumber3 samplenumber4 label2
		...
		samplenumber2n-1 samplenumber2n labeln

	The first tier of TextGrid will contain the TIMIT labels.
	If phnFile != 0, the second tier will contain the translation of the
	TIMIT labels into IPA labels.
	For the translation from sample number to time a default sampling
	frequency of 16000 Hz is assumed.
*/

Any TextGrid_TIMITLabelFileRecognizer (int nread, const char *header, MelderFile file);
/*
	There are two types of TIMIT label files. One with phonetic labels, these
	files have '.phn' as file extension. The other contains word labels and has
	'.wrd' as extension. Since these extensions are only valid on the CDROM we can
	not use them for filetype recognition. Both TIMIT label files do not have a
	self-describing format. For filetype recognition we make use of the fact that
	both files are text files and always have three items on each line: two numbers
	followed by a string. The numbers increase in a monotone way.
	The recognizer only checks the first two lines and it tests whether
		0 <= number[1] < number[2] <= number[3] < number[4]
	(A number of .wrd files do not obey the monotonocity constraint for
	 number[4] and number[5] !)
	The decision whether it is a .phn or .wrd file is:
		.phn if string[1] == 'h#' AND string[2] is a TIMIT phonetic label
		.wrd if (string[1] == 'h#' AND string[2] is a valid word) OR
			string[1] and string[2] are both valid words.
		A valid word is a string with contains the lowercase characters [a-z] and ['].
*/

TextGrid TextGrids_merge (TextGrid grid1, TextGrid grid2);
/*
	Merge two textGrids.
	The new domain will be:
	[min(grid1->xmin, grid2->xmin), max(grid1->xmax, grid2->xmax)].
	This implies that for the resulting TextGrid each interval tier will have
	one or two extra intervals if the domains of the two TextGrids are not equal,
*/

void TextGrid_extendTime (TextGrid me, double delta_time, int position);
/*
	Extend the begin-time (delta_time<0) or end-time (delta_time>0).
	For Point-tiers only the domain will be extended.
	Interval tiers will have a new (empty) interval at the start or the end.
*/

void TextGrid_setTierName (TextGrid me, long itier, const wchar_t *newName);

void TextTier_changeLabels (I, long from, long to, const wchar_t *search, const wchar_t *replace, int use_regexp, long *nmatches, long *nstringmatches);

void IntervalTier_changeLabels (I, long from, long to, const wchar_t *search, const wchar_t *replace, int use_regexp, long *nmatches, long *nstringmatches);

void IntervalTier_removeBoundariesBetweenIdenticallyLabeledIntervals (IntervalTier me, const wchar_t *label);

void IntervalTier_cutIntervalsOnLabelMatch (IntervalTier me, const wchar_t *label);
void IntervalTier_cutIntervals_minimumDuration (IntervalTier me, const wchar_t *label, double minimumDuration);

void TextGrid_changeLabels (TextGrid me, int tier, long from, long to, const wchar_t *search, const wchar_t *replace, int use_regexp, long *nmatches, long *nstringmatches);

/* Set the start/end time to a smaller/larger value.
 * If mark is NULL, only times are changed
 * If mark != NULL mark the previous start/end time
 *    For a TextTier this involves adding a point with the marker
 *    For an IntervalTier this involves adding a new interval
 */
void IntervalTier_setLaterEndTime (IntervalTier me, double xmax, const wchar_t *mark);
void IntervalTier_setEarlierStartTime (IntervalTier me, double xmin, const wchar_t *mark);

void IntervalTier_moveBoundary (IntervalTier me, long interval, bool atStart, double newTime);
void TextTier_setLaterEndTime (TextTier me, double xmax, const wchar_t *mark);
void TextTier_setEarlierStartTime (TextTier me, double xmin, const wchar_t *mark);
void TextGrid_setEarlierStartTime (TextGrid me, double xmin, const wchar_t *imark, const wchar_t *pmark);
void TextGrid_setLaterEndTime (TextGrid me, double xmax, const wchar_t *imark, const wchar_t *pmark);


// Precondition: if (preserveTimes) { my xmax <= thy xmin }
// Postcondition: my xmin preserved
void IntervalTiers_append_inline (IntervalTier me, IntervalTier thee, bool preserveTimes);
void TextTiers_append_inline (TextTier me, TextTier thee, bool preserveTimes);
void TextGrids_append_inline (TextGrid me, TextGrid thee, bool preserveTimes);

// Postcondition:
TextGrid TextGrids_to_TextGrid_appendContinuous (Collection me, bool preserveTimes);

#endif /* _TextGrid_extensions_h_ */
