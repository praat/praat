/* TextGrid_files.cpp
 *
 * Copyright (C) 1992-2025 Paul Boersma
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

#include "TextGrid.h"

autoTextTier TextTier_readFromXwaves (MelderFile file) {
	try {
		conststring8 line;

		autoTextTier me = TextTier_create (0, 100);
		autoMelderFile mfile = MelderFile_open (file);

		/*
		 * Search for a line that starts with '#'.
		 */
		for (;;) {
			line = MelderFile_readLine8 (file);
			if (! line)
				Melder_throw (U"Missing '#' line.");
			if (line [0] == '#')
				break;
		}

		/*
		 * Read a mark from every line.
		 */
		for (;;) {
			line = MelderFile_readLine8 (file);
			if (! line)
				break;   // normal end-of-file
			double time;
			integer colour;
			char mark [300];
			if (sscanf (line, "%lf%td%299s", & time, & colour, mark) < 3)   // BUG: semantic buffer overflow
				Melder_throw (U"Line too short: \"", Melder_peek8to32 (line), U"\".");
			TextTier_addPoint (me.get(), time, Melder_peek8to32 (mark));
		}

		/*
		 * Fix domain.
		 */
		if (my points.size > 0) {
			TextPoint point = my points.at [1];
			if (point -> number < 0.0)
				my xmin = point -> number - 1.0;
			point = my points.at [my points.size];
			my xmax = point -> number + 1.0;
		}
		mfile.close ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextTier not read from Xwaves file.");
	}
}

static void MelderReadText_skipHorizontalWhiteSpace (MelderReadText me) {
	for (;;) {
		char32 kar = MelderReadText_getChar (me);
		if (kar == U'\0')
			return;
		if (Melder_isHorizontalSpace (kar))
			continue;
		MelderReadText_ungetChar (me);
		return;
	}
}

static int64 MelderReadText_readInteger (MelderReadText me) {
	char32 kar = MelderReadText_getChar (me);
	Melder_require (kar != U'\0',
		U"Looking for an integer, but found the end of the text.");
	Melder_require (! Melder_isVerticalSpace (kar),
		U"Looking for an integer, but found the end of the line.");
	const bool hasSign = ( kar == U'-' || kar == U'+' );
	Melder_require (hasSign || Melder_isAsciiDecimalNumber (kar),
		U"Looking for an integer, but found “", MelderReadText_readLine (me), U"”.");
	constexpr integer MAXIMUM_NUMBER_OF_DIGITS = 40;
	char buffer [1 + MAXIMUM_NUMBER_OF_DIGITS + 1];   // include room for leading sign and trailing null
	/* mutable increment */ integer ipos = 0;
	buffer [ipos ++] = (char) (char8) kar;
	if (hasSign) {
		char32 shouldBeDigit = MelderReadText_getChar (me);
		Melder_require (Melder_isAsciiDecimalNumber (shouldBeDigit),
			U"Looking for a digit after “", kar, U"”, but found “", MelderReadText_readLine (me), U"”.");
		buffer [ipos ++] = (char) (char8) shouldBeDigit;
	}
	for (;;) {
		char32 mayBeDigit = MelderReadText_getChar (me);
		if (Melder_isAsciiDecimalNumber (mayBeDigit)) {
			Melder_require (ipos < hasSign + MAXIMUM_NUMBER_OF_DIGITS,
				U"Looking for a normal-sized integer, but found more than ", MAXIMUM_NUMBER_OF_DIGITS, U" digits.");
			buffer [ipos ++] = (char) (char8) mayBeDigit;
		} else {
			MelderReadText_ungetChar (me);
			break;
		}
	}
	buffer [ipos] = '\0';
	return strtoll (buffer, nullptr, 10);
}

static double MelderReadText_readReal (MelderReadText me) {
	char32 kar = MelderReadText_getChar (me);
	Melder_require (kar != U'\0',
		U"Looking for a real number, but found the end of the text.");
	Melder_require (! Melder_isVerticalSpace (kar),
		U"Looking for a real number, but found the end of the line.");
	const bool hasSign = ( kar == U'-' || kar == U'+' );
	Melder_require (hasSign || Melder_isAsciiDecimalNumber (kar),
		U"Looking for a real number, but found “", MelderReadText_readLine (me), U"”.");
	constexpr integer MAXIMUM_NUMBER_OF_CHARACTERS = 100;
	char buffer [1 + MAXIMUM_NUMBER_OF_CHARACTERS + 1];   // include room for leading sign and trailing null
	/* mutable increment */ integer ipos = 0;
	buffer [ipos ++] = (char) (char8) kar;
	if (hasSign) {
		char32 shouldBeDigit = MelderReadText_getChar (me);
		Melder_require (Melder_isAsciiDecimalNumber (shouldBeDigit),
			U"Looking for a digit after “", kar, U"”, but found “", MelderReadText_readLine (me), U"”.");
		buffer [ipos ++] = (char) (char8) shouldBeDigit;
	}
	for (;;) {
		char32 numberCharacter = MelderReadText_getChar (me);
		if (Melder_isAsciiDecimalNumber (numberCharacter) || numberCharacter == U'+' || numberCharacter == U'-' ||
			numberCharacter == U'e' || numberCharacter == U'E' || numberCharacter == U'.'
		) {
			Melder_require (ipos < hasSign + MAXIMUM_NUMBER_OF_CHARACTERS,
				U"Looking for a normal-sized real number, but found more than ", MAXIMUM_NUMBER_OF_CHARACTERS, U" digits.");
			buffer [ipos ++] = (char) (char8) numberCharacter;
		} else {
			MelderReadText_ungetChar (me);
			break;
		}
	}
	buffer [ipos] = '\0';
	return Melder_a8tof (buffer);
}

autoTextGrid TextGrid_readFromEspsLabelFile (MelderFile file) {
	TRACE
	try {
		autoMelderReadText text = MelderReadText_createFromFile (file);   // going to be UTF-8-compatible
		/* mutable increment */ integer lineNumber = 0;

		/*
			Cycle through all lines until encountering a line that starts with '#'.
		*/
		integer numberOfTiers = 1;
		char32 separator = U';';
		for (;;) {
			const conststring32 line = MelderReadText_readLine (text.get());
			if (! line)
				Melder_throw (U"Missing '#' line.");
			if (line [0] == '#')
				break;
			++ lineNumber;
			trace (U"Line ", lineNumber, U": <", line, U">");
			constexpr char32 tag_nfields [] = U"nfields ";
			constexpr integer tag_nfields_length = Melder_length (tag_nfields);
			if (Melder_startsWith (line, tag_nfields))
				numberOfTiers = Melder_atoi (line + tag_nfields_length);
			constexpr char32 tag_separator [] = U"separator ";
			if (Melder_startsWith (line, tag_separator))
				separator = line [Melder_length (tag_separator)];
		}
		Melder_require (numberOfTiers >= 1,
			U"The number of tiers has to be at least 1, but the file states that it should be ", numberOfTiers, U".");
		Melder_require (numberOfTiers <= 1'000'000'000,
			U"The number of tiers has to be at most 1,000,000,000, but the file states that it should be ", numberOfTiers, U".");

		/* mutable preliminary */ double tmin = 0.0, tmax = 100.0;
		autoTextGrid me = TextGrid_createWithoutTiers (tmin, tmax);
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			/*
				Dummy name.
			*/
			autoIntervalTier tier = IntervalTier_create (tmin, tmax);
			(void) tier -> intervals. subtractItem_move (1);
			Thing_setName (tier.get(), Melder_integer (itier));
			my tiers -> addItem_move (tier.move());
		}

		autoMelderString label;
		/* mutable step */ double startingTime = 0.0, endTime = undefined;
		for (;;) {
			const mutablestring32 line = MelderReadText_readLine (text.get());
			if (! line)
				break;
			++ lineNumber;
			trace (U"Line ", lineNumber, U": <", line, U">");
			autoMelderReadText lineText = MelderReadText_createFromText (Melder_dup (line));   // a bit costly
			MelderReadText_skipHorizontalWhiteSpace (lineText.get());
			endTime = MelderReadText_readReal (lineText.get());
			trace (U"end time: ", endTime);
			MelderReadText_skipHorizontalWhiteSpace (lineText.get());
			const integer colour = MelderReadText_readInteger (lineText.get());
			trace (U"colour: ", colour);
			const char32 shouldBeHorizontalSpace = MelderReadText_getChar (lineText.get());
			Melder_require (Melder_isHorizontalSpace (shouldBeHorizontalSpace),
				U"There should be a space after the colour number in line ", lineNumber, U".");
			trace (U"starting time: ", startingTime, U"; end time: ", endTime);
			for (integer itier = 1; itier <= numberOfTiers; itier ++) {
				MelderString_empty (& label);
				for (;;) {
					char32 kar = MelderReadText_getChar (lineText.get());
					if (kar == separator || kar == U'\0')
						break;
					MelderString_appendCharacter (& label, kar);
				}
				IntervalTier tier = static_cast <IntervalTier> (my tiers->at [itier]);
				autoTextInterval interval = TextInterval_create (startingTime, endTime, label.string);
				tier -> intervals. addItem_move (interval.move());
			}
			startingTime = endTime;
		}
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			IntervalTier tier = static_cast <IntervalTier> (my tiers->at [itier]);
			tier -> xmax = endTime;
		}
		my xmax = endTime;
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from file ", file, U".");
	}
}

autoIntervalTier IntervalTier_readFromXwaves (MelderFile file) {
	try {
		char *line;
		double lastTime = 0.0;

		autoIntervalTier me = IntervalTier_create (0, 100);
		autoMelderFile mfile = MelderFile_open (file);

		/*
			Search for a line that starts with '#'.
		*/
		for (;;) {
			line = MelderFile_readLine8 (file);
			if (! line)
				Melder_throw (U"Missing '#' line.");
			if (line [0] == '#')
				break;
		}

		/*
			Read a mark from every line.
		*/
		for (;;) {
			double time;
			integer colour;
			integer numberOfElements;
			char mark [300];

			line = MelderFile_readLine8 (file);
			if (! line)
				break;   // normal end-of-file
			numberOfElements = sscanf (line, "%lf%td%199s", & time, & colour, mark);
			if (numberOfElements == 0)
				break;   // an empty line, hopefully at the end
			if (numberOfElements == 1)
				Melder_throw (U"Line too short: \"", Melder_peek8to32 (line), U"\".");
			if (numberOfElements == 2)
				mark [0] = '\0';
			if (lastTime == 0.0) {
				TextInterval interval = my intervals.at [1];
				interval -> xmax = time;
				TextInterval_setText (interval, Melder_peek8to32 (mark));
			} else {
				IntervalTier_addInterval_raw (me.get(), lastTime, time, Melder_peek8to32 (mark));
			}
			lastTime = time;
		}

		/*
			Fix domain.
		*/
		if (lastTime > 0.0) {
			TextInterval lastInterval = my intervals.at [my intervals.size];
			my xmax = lastInterval -> xmax = lastTime;
		}

		mfile.close ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"IntervalTier not read from file ", file, U".");
	}
}

void IntervalTier_writeToXwaves (IntervalTier me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "w");
		fprintf (f, "separator ;\nnfields 1\n#\n");
		for (integer iinterval = 1; iinterval <= my intervals.size; iinterval ++) {
			TextInterval interval = my intervals.at [iinterval];
			fprintf (f, "\t%.6f 26\t%s\n", interval -> xmax, Melder_peek32to8 (interval -> text.get()));
		}
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, U": not written to Xwaves file ", file, U".");
	}
}

static void sgmlToPraat (char *text) {
	char *sgml = text, *praat = text;
	for (;;) {
		if (*sgml == '\0')
			break;
		if (*sgml == '&') {
			static struct { const char *sgml, *praat; } translations [] = {
				{ "auml", "\\a\"" }, { "euml", "\\e\"" }, { "iuml", "\\i\"" },
				{ "ouml", "\\o\"" }, { "ouml", "\\o\"" },
				{ "Auml", "\\A\"" }, { "Euml", "\\E\"" }, { "Iuml", "\\I\"" },
				{ "Ouml", "\\O\"" }, { "Uuml", "\\U\"" },
				{ "aacute", "\\a'" }, { "eacute", "\\e'" }, { "iacute", "\\i'" },
				{ "oacute", "\\o'" }, { "oacute", "\\o'" },
				{ "Aacute", "\\A'" }, { "Eacute", "\\E'" }, { "Iacute", "\\I'" },
				{ "Oacute", "\\O'" }, { "Uacute", "\\U'" },
				{ "agrave", "\\a`" }, { "egrave", "\\e`" }, { "igrave", "\\i`" },
				{ "ograve", "\\o`" }, { "ograve", "\\o`" },
				{ "Agrave", "\\A`" }, { "Egrave", "\\E`" }, { "Igrave", "\\I`" },
				{ "Ograve", "\\O`" }, { "Ugrave", "\\U`" },
				{ "acirc", "\\a^" }, { "ecirc", "\\e^" }, { "icirc", "\\i^" },
				{ "ocirc", "\\o^" }, { "ocirc", "\\o^" },
				{ "Acirc", "\\A^" }, { "Ecirc", "\\E^" }, { "Icirc", "\\I^" },
				{ "Ocirc", "\\O^" }, { "Ucirc", "\\U^" },
				{ nullptr, nullptr } };
			char sgmlCode [201];
			int i = 0;
			++ sgml;
			for (i = 0; i < 200; i ++) {
				char sgmlChar = sgml [i];
				if (sgmlChar == ';') {
					if (i == 0)
						Melder_throw (U"Empty SGML code.");
					sgml += i + 1;
					break;
				}
				sgmlCode [i] = sgmlChar;
			}
			if (i >= 200)
				Melder_throw (U"Unfinished SGML code.");
			sgmlCode [i] = '\0';
			for (i = 0; translations [i]. sgml; i ++) {
				if (strequ (sgmlCode, translations [i]. sgml)) {
					memcpy (praat, translations [i]. praat, strlen (translations [i]. praat));
					praat += strlen (translations [i]. praat);
					break;
				}
			}
			if (! translations [i]. sgml)
				Melder_throw (U"Unknown SGML code &", Melder_peek8to32 (sgmlCode), U";.");
		} else {
			* praat ++ = * sgml ++;
		}
	}
	*praat = '\0';
}

autoTextGrid TextGrid_readFromChronologicalTextFile (MelderFile file) {
	try {
		int formatVersion = 0;
		autoMelderReadText text = MelderReadText_createFromFile (file);
		autostring32 tag = texgetw16 (text.get());
		if (! str32equ (tag.get(), U"Praat chronological TextGrid text file"))
			Melder_throw (U"This is not a chronological TextGrid text file.");
		autoTextGrid me = Thing_new (TextGrid);
		my structFunction :: v1_readText (text.get(), formatVersion);
		my tiers = FunctionList_create ();
		integer numberOfTiers = texgeti32 (text.get());
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			autostring32 klas = texgetw16 (text.get());
			if (str32equ (klas.get(), U"IntervalTier")) {
				autoIntervalTier tier = Thing_new (IntervalTier);
				tier -> name = texgetw16 (text.get());
				tier -> structFunction :: v1_readText (text.get(), formatVersion);
				my tiers -> addItem_move (tier.move());
			} else if (str32equ (klas.get(), U"TextTier")) {
				autoTextTier tier = Thing_new (TextTier);
				tier -> name = texgetw16 (text.get());
				tier -> structFunction :: v1_readText (text.get(), formatVersion);
				my tiers -> addItem_move (tier.move());
			} else {
				Melder_throw (U"Unknown tier class \"", klas.get(), U"\".");
			}
		}
		for (;;) {
			integer tierNumber;
			try {
				tierNumber = texgeti32 (text.get());
			} catch (MelderError) {
				if (str32str (Melder_getError (), U"Early end of text")) {
					Melder_clearError ();
					break;
				} else {
					throw;
				}
			}
			Function anyTier = TextGrid_checkSpecifiedTierNumberWithinRange (me.get(), tierNumber);
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = static_cast <IntervalTier> (anyTier);
				autoTextInterval interval = Thing_new (TextInterval);
				interval -> v1_readText (text.get(), formatVersion);
				tier -> intervals. addItem_move (interval.move());   // not earlier: sorting depends on contents of interval
			} else {
				TextTier tier = static_cast <TextTier> (anyTier);
				autoTextPoint point = Thing_new (TextPoint);
				point -> v1_readText (text.get(), formatVersion);
				tier -> points. addItem_move (point.move());   // not earlier: sorting depends on contents of point
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from chronological text file ", file, U".");
	}
}

static void writeQuotedString (MelderFile file, conststring32 string) {
	MelderFile_writeCharacter (file, U'\"');
	if (string) {
		char32 kar;
		while ((kar = *string ++) != U'\0') {
			MelderFile_writeCharacter (file, kar);
			if (kar == '\"')
				MelderFile_writeCharacter (file, kar);
		}
	}   // BUG
	MelderFile_writeCharacter (file, U'\"');
}

void TextGrid_writeToChronologicalTextFile (TextGrid me, MelderFile file) {
	try {
		Data_createTextFile (me, file, false);
		autoMelderFile mfile = file;
		/*
			The "elements" (intervals and points) are sorted primarily by time and secondarily by tier.
		*/
		double sortingTime = -1e308;
		integer sortingTier = 0;
		file -> verbose = false;
		texindent (file);
		MelderFile_write (file, U"\"Praat chronological TextGrid text file\"\n", my xmin, U" ", my xmax,
			U"   ! Time domain.\n", my tiers->size, U"   ! Number of tiers.");
		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			Function anyTier = my tiers->at [itier];
			MelderFile_write (file, U"\n");
			writeQuotedString (file, Thing_className (anyTier));
			MelderFile_write (file, U" ");
			writeQuotedString (file, anyTier -> name.get());
			MelderFile_write (file, U" ", anyTier -> xmin, U" ", anyTier -> xmax);
		}
		for (;;) {
			double firstRemainingTime = +1e308;
			integer firstRemainingTier = 2000000000, firstRemainingElement = 0;
			for (integer itier = 1; itier <= my tiers->size; itier ++) {
				Function anyTier = my tiers->at [itier];
				if (anyTier -> classInfo == classIntervalTier) {
					IntervalTier tier = static_cast <IntervalTier> (anyTier);
					for (integer ielement = 1; ielement <= tier -> intervals.size; ielement ++) {
						TextInterval interval = tier -> intervals.at [ielement];
						if ((interval -> xmin > sortingTime ||   // sort primarily by time
							 (interval -> xmin == sortingTime && itier > sortingTier)) &&   // sort secondarily by tier number
							(interval -> xmin < firstRemainingTime ||   // sort primarily by time
							 (interval -> xmin == firstRemainingTime && itier < firstRemainingTier)))   // sort secondarily by tier number
						{
							firstRemainingTime = interval -> xmin;
							firstRemainingTier = itier;
							firstRemainingElement = ielement;
						}
					}
				} else {
					TextTier tier = static_cast <TextTier> (anyTier);
					for (integer ielement = 1; ielement <= tier -> points.size; ielement ++) {
						TextPoint point = tier -> points.at [ielement];
						if ((point -> number > sortingTime ||   // sort primarily by time
							 (point -> number == sortingTime && itier > sortingTier)) &&   // sort secondarily by tier number
							(point -> number < firstRemainingTime ||   // sort primarily by time
							 (point -> number == firstRemainingTime && itier < firstRemainingTier)))   // sort secondarily by tier number
						{
							firstRemainingTime = point -> number;
							firstRemainingTier = itier;
							firstRemainingElement = ielement;
						}
					}
				}
			}
			if (firstRemainingElement == 0) {
				break;
			} else {
				Function anyTier = my tiers->at [firstRemainingTier];
				if (anyTier -> classInfo == classIntervalTier) {
					IntervalTier tier = static_cast <IntervalTier> (anyTier);
					TextInterval interval = tier -> intervals.at [firstRemainingElement];
					if (tier -> name) MelderFile_write (file, U"\n\n! ", tier -> name.get(), U":");
					MelderFile_write (file, U"\n", firstRemainingTier, U" ", interval -> xmin, U" ", interval -> xmax);
					texputw32 (file, interval -> text.get(), U"", 0,0,0,0,0);
				} else {
					TextTier tier = static_cast <TextTier> (anyTier);
					TextPoint point = tier -> points.at [firstRemainingElement];
					if (tier -> name) MelderFile_write (file, U"\n\n! ", tier -> name.get(), U":");
					MelderFile_write (file, U"\n", firstRemainingTier, U" ", point -> number, U" ");
					texputw32 (file, point -> mark.get(), U"", 0,0,0,0,0);
				}
				sortingTime = firstRemainingTime;
				sortingTier = firstRemainingTier;
			}
		}
		texexdent (file);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, U": not written to chronological text file ", file, U".");
	}
}

autoTextGrid TextGrid_readFromCgnSyntaxFile (MelderFile file) {
	try {
		autoTextGrid me = Thing_new (TextGrid);
		integer sentenceNumber = 0;
		double phraseBegin = 0.0, phraseEnd = 0.0;
		IntervalTier sentenceTier = nullptr, phraseTier = nullptr;
		TextInterval lastInterval = nullptr;
		static char phrase [1000];
		my tiers = FunctionList_create ();
		autoMelderFile mfile = MelderFile_open (file);
		char * const line1 = MelderFile_readLine8 (file);
		Melder_require (strequ (line1, "<?xml version=\"1.0\"?>"),
			U"This is not a CGN syntax file.");
		char * const line2 = MelderFile_readLine8 (file);
		Melder_require (strequ (line2, "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">"),
			U"This is not a CGN syntax file.");
		(void) MelderFile_readLine8 (file);   // ignore third line
		const integer startOfData = MelderFile_tell (file);
		/*
			Get duration.
		*/
		my xmin = 0.0;
		char arg1 [41], arg2 [41], arg3 [41], arg4 [41], arg5 [41], arg6 [41], arg7 [201];
		for (;;) {
			char * const line = MelderFile_readLine8 (file);
			if (! line)
				break;
			if (strnequ (line, "  <tau ref=\"", 12)) {
				Melder_require (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) == 7,
					U"Too few strings in tau line.");
				my xmax = atof (arg5 + 4);
			}
		}
		Melder_require (my xmax > 0.0,
			U"Duration (", my xmax, U" seconds) should be greater than zero.");
		/*
			Get number and names of tiers.
		*/
		MelderFile_seek (file, startOfData, SEEK_SET);
		for (;;) {
			char * const line = MelderFile_readLine8 (file);
			if (! line)
				break;
			if (strnequ (line, "  <tau ref=\"", 12)) {
				Melder_require (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) == 7,
					U"Too few strings in tau line.");
				const integer length_s = Melder8_length (arg3);
				Melder_require (length_s >= 5 && strnequ (arg3, "s=\"", 3),
					U"Missing speaker name.");
				arg3 [length_s - 1] = '\0';   // truncate at double quote
				char * const speakerName = arg3 + 3;   // truncate leading s="
				/*
					Does this speaker name occur in the tiers?
				*/
				/* mutable search */ integer speakerTier = 0;
				for (integer itier = 1; itier <= my tiers->size; itier ++) {
					IntervalTier tier = static_cast <IntervalTier> (my tiers->at [itier]);
					if (str32equ (tier -> name.get(), Melder_peek8to32 (speakerName))) {
						speakerTier = itier;
						break;
					}
				}
				if (speakerTier == 0) {
					/*
						Create two new tiers.
					*/
					autoIntervalTier newSentenceTier = Thing_new (IntervalTier);
					newSentenceTier -> xmin = 0.0;
					newSentenceTier -> xmax = my xmax;
					Thing_setName (newSentenceTier.get(), Melder_peek8to32 (speakerName));
					sentenceTier = (IntervalTier) my tiers -> addItem_move (newSentenceTier.move());
					autoIntervalTier newPhraseTier = Thing_new (IntervalTier);
					newPhraseTier -> xmin = 0.0;
					newPhraseTier -> xmax = my xmax;
					Thing_setName (newPhraseTier.get(), Melder_peek8to32 (speakerName));
					phraseTier = (IntervalTier) my tiers -> addItem_move (newPhraseTier.move());
				} else {
					sentenceTier = (IntervalTier) my tiers->at [speakerTier];
					phraseTier = (IntervalTier) my tiers->at [speakerTier + 1];
				}
				const double tb = atof (arg4 + 4), te = atof (arg5 + 4);
				Melder_require (te > tb,
					U"Zero duration for sentence.");
				/*
					We are going to add one or two intervals to the sentence tier.
				*/
				if (sentenceTier -> intervals.size > 0) {
					TextInterval latestInterval = sentenceTier -> intervals.at [sentenceTier -> intervals.size];
					if (tb > latestInterval -> xmax) {
						autoTextInterval interval = TextInterval_create (latestInterval -> xmax, tb, U"");
						sentenceTier -> intervals. addItem_move (interval.move());
					} else if (tb < latestInterval -> xmax) {
						Melder_throw (U"Overlap on tier not allowed.");
					}
				} else {
					if (tb > 0.0) {
						autoTextInterval interval = TextInterval_create (0.0, tb, U"");
						sentenceTier -> intervals. addItem_move (interval.move());
					} else if (tb < 0.0) {
						Melder_throw (U"Negative times not allowed.");
					}
				}
				autoTextInterval interval = TextInterval_create (tb, te, Melder_integer (++ sentenceNumber));
				sentenceTier -> intervals. addItem_move (interval.move());
			} else if (strnequ (line, "    <tw ref=\"", 13)) {
				Melder_require (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) == 7,
					U"Too few strings in tw line.");
				const integer length_tb = Melder8_length (arg3);
				Melder_require (length_tb >= 6 && strnequ (arg3, "tb=\"", 4),
					U"Missing tb.");
				const double tb = atof (arg3 + 4);
				const integer length_te = Melder8_length (arg4);
				Melder_require (length_te >= 6 && strnequ (arg4, "te=\"", 4),
					U"Missing te.");
				const double te = atof (arg4 + 4);
				Melder_require (te > tb,
					U"Zero duration for phrase.");
				if (tb == phraseBegin && te == phraseEnd) {
					/*
						Append a word.
					*/
					strcat (phrase, " ");
					const integer length_w = Melder8_length (arg7);
					Melder_require (length_w >= 6 && strnequ (arg7, "w=\"", 3),   // BUG? no words of length 1 allowed
						U"Missing word.");
					arg7 [length_w - 3] = '\0';   // truncate "/>
					strcat (phrase, arg7 + 3);
				} else {
					/*
						Begin a phrase.
					*/
					if (lastInterval) {
						sgmlToPraat (phrase);
						TextInterval_setText (lastInterval, Melder_peek8to32 (phrase));
					}
					phrase [0] = '\0';
					const integer length_w = Melder8_length (arg7);
					Melder_require (length_w >= 6 && strnequ (arg7, "w=\"", 3),   // BUG? no words of length 1 allowed
						U"Missing word.");
					arg7 [length_w - 3] = '\0';   // truncate "/>
					strcat (phrase, arg7 + 3);
					if (phraseTier -> intervals.size > 0) {
						TextInterval latestInterval = phraseTier -> intervals.at [phraseTier -> intervals.size];
						if (tb > latestInterval -> xmax) {
							autoTextInterval interval = TextInterval_create (latestInterval -> xmax, tb, U"");
							phraseTier -> intervals. addItem_move (interval.move());
						} else if (tb < latestInterval -> xmax) {
							Melder_throw (U"Overlap on tier not allowed.");
						}
					} else {
						if (tb > 0.0) {
							autoTextInterval interval = TextInterval_create (0.0, tb, U"");
							phraseTier -> intervals. addItem_move (interval.move());
						} else if (tb < 0.0) {
							Melder_throw (U"Negative times not allowed.");
						}
					}
					if (! phraseTier)
						Melder_throw (U"Phrase outside sentence.");
					autoTextInterval newLastInterval = TextInterval_create (tb, te, U"");
					lastInterval = newLastInterval.get();
					phraseTier -> intervals. addItem_move (newLastInterval.move());
					phraseBegin = tb;
					phraseEnd = te;
				}
			}
		}
		if (lastInterval) {
			sgmlToPraat (phrase);
			TextInterval_setText (lastInterval, Melder_peek8to32 (phrase));
		}
		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			IntervalTier tier = static_cast <IntervalTier> (my tiers->at [itier]);
			if (tier -> intervals.size > 0) {
				TextInterval latestInterval = tier -> intervals.at [tier -> intervals.size];
				if (my xmax > latestInterval -> xmax) {
					autoTextInterval interval = TextInterval_create (latestInterval -> xmax, my xmax, U"");
					tier -> intervals. addItem_move (interval.move());
				}
			} else {
				autoTextInterval interval = TextInterval_create (my xmin, my xmax, U"");
				tier -> intervals. addItem_move (interval.move());
			}
		}
		mfile.close ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from CGN syntax file ", file, U".");
	}
}

const struct TIMIT_key {
	const char *timitLabel, *ipaLabel;
} TIMIT_toIpaTable [] = {
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

constexpr integer TIMIT_NLABELS = ((integer) sizeof TIMIT_toIpaTable / (integer) sizeof TIMIT_toIpaTable [1] - 1);
static const char *TIMIT_DELIMITER = "h#";

static const char *timitLabelToIpaLabel (const char timitLabel []) {
	for (integer i = 1; i <= TIMIT_NLABELS; i ++)
		if (! strcmp (TIMIT_toIpaTable [i].timitLabel, timitLabel))
			return TIMIT_toIpaTable [i].ipaLabel;
	return timitLabel;
}

static bool isTimitPhoneticLabel (const char label []) {
	for (integer i = 1; i <= TIMIT_NLABELS; i ++)
		if (! strcmp (TIMIT_toIpaTable [i].timitLabel, label))
			return true;
	return false;
}

static bool isTimitWord (const char label []) {
	const char *p = label;
	for (; *p; p++)
		if (Melder_isUpperCaseLetter (*p))
			return false;
	return true;
}

autoDaata TextGrid_TIMITLabelFileRecognizer (integer nread, const char *header, MelderFile file) {
	char hkruis [3] = "h#", label1 [512], label2 [512];
	int length;
	bool phnFile = false;
	integer it [5];
	if (nread < 12 || sscanf (header, "%td%td%511s%n\n", & it [1], & it [2], label1, & length) != 3 ||
		it [1] < 0 || it [2] <= it [1] || sscanf (& header [length], "%td%td%511s\n", & it [3], & it [4], label2) != 3 ||
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
		char line [200], label [200];
		while (fgets (line, 199, f)) {
			integer it1, it2;
			linesRead ++;
			Melder_require (sscanf (line, "%td%td%199s", & it1, & it2, label) == 3,
				U"Incorrect number of items.");
			if (it1 == it2) {
				Melder_warning (U"File \"", MelderFile_messageName (file), U"\": Label \"", Melder_peek8to32 (label),
					U"\" on line ", linesRead, U" was skipped because the start time and the end time were equal.");
				/*
					For the following .wrd files this occurs once per file:
					train/dr3/makr0/si1982
					train/dr3/mhjb0/sa2
					train/dr5/mmcc0/sx348
					train/dr6/meal0/sa2
					train/dr6/mesj0/si2039
					train/dr7/fmkc0/sx352
					train/dr7/mrem0/sx61
					train/dr7/mvrw0/sx315
					test/dr6/mjfc0/sx138
				*/
			} else {
				Melder_require (it1 >= 0 && it1 < it2,
					U"Incorrect time at line ", linesRead);
			}
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
				/*
					This warning occurs hundreds of time for the .wrd files
				*/
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

/* End of file TextGrid_files.cpp */
