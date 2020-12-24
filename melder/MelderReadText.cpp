/* melder_readtext.cpp
 *
 * Copyright (C) 2008,2010-2012,2014-2020 Paul Boersma
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

#include "melder.h"
#include "../kar/UnicodeData.h"

char32 MelderReadText_getChar (MelderReadText me) {
	if (my string32) {
		if (* my readPointer32 == U'\0')
			return U'\0';
		return * my readPointer32 ++;
	} else {
		if (* my readPointer8 == '\0') return U'\0';
		if (my input8Encoding == kMelder_textInputEncoding::UTF8) {
			char32 kar1 = (char32) (char8) * my readPointer8 ++;
			if (kar1 <= 0x00'007F) {
				return kar1;
			} else if (kar1 <= 0x00'00DF) {
				char32 kar2 = (char32) (char8) * my readPointer8 ++;
				return ((kar1 & 0x00'001F) << 6) | (kar2 & 0x00'003F);
			} else if (kar1 <= 0x00'00EF) {
				char32 kar2 = (char32) (char8) * my readPointer8 ++;
				char32 kar3 = (char32) (char8) * my readPointer8 ++;
				return ((kar1 & 0x00'000F) << 12) | ((kar2 & 0x00'003F) << 6) | (kar3 & 0x00'003F);
			} else if (kar1 <= 0x00'00F4) {
				char32 kar2 = (char32) (char8) * my readPointer8 ++;
				char32 kar3 = (char32) (char8) * my readPointer8 ++;
				char32 kar4 = (char32) (char8) * my readPointer8 ++;
				return ((kar1 & 0x00'0007) << 18) | ((kar2 & 0x00'003F) << 12) | ((kar3 & 0x00'003F) << 6) | (kar4 & 0x00'003F);
			} else {
				return UNICODE_REPLACEMENT_CHARACTER;
			}
		} else if (my input8Encoding == kMelder_textInputEncoding::MACROMAN) {
			return Melder_decodeMacRoman [(char8) * my readPointer8 ++];
		} else if (my input8Encoding == kMelder_textInputEncoding::WINDOWS_LATIN1) {
			return Melder_decodeWindowsLatin1 [(char8) * my readPointer8 ++];
		} else {
			/* Unknown encoding. */
			return (char32) (char8) * my readPointer8 ++;
		}
	}
}

mutablestring32 MelderReadText_readLine (MelderReadText me) {
	if (my string32) {
		Melder_assert (my readPointer32);
		Melder_assert (! my readPointer8);
		if (*my readPointer32 == U'\0')   // tried to read past end of file
			return nullptr;
		char32 *result = my readPointer32;
		char32 *newline = str32chr (result, U'\n');
		if (newline) {
			*newline = U'\0';
			my readPointer32 = newline + 1;
		} else {
			my readPointer32 += str32len (result);
		}
		return result;
	} else {
		Melder_assert (my string8);
		Melder_assert (! my readPointer32);
		Melder_assert (my readPointer8);
		if (*my readPointer8 == '\0')   // tried to read past end of file
			return nullptr;
		char *result8 = my readPointer8;
		char *newline = strchr (result8, '\n');
		if (newline) {
			*newline = '\0';
			my readPointer8 = newline + 1;
		} else {
			my readPointer8 += strlen (result8);
		}
		static char32 *text32 = nullptr;
		static int64 size = 0;
		int64 sizeNeeded = (int64) strlen (result8) + 1;
		if (sizeNeeded > size) {
			Melder_free (text32);
			text32 = Melder_malloc_f (char32, sizeNeeded + 100);
			size = sizeNeeded + 100;
		}
		Melder_8to32_inplace (result8, text32, my input8Encoding);
		return text32;
	}
}

int64 MelderReadText_getNumberOfLines (MelderReadText me) {
	int64 n = 0;
	if (my string32) {
		char32 *p = & my string32 [0];
		for (; *p != U'\0'; p ++) {
			if (*p == U'\n')
				n ++;
		}
		if (p - & my string32 [0] > 1 && p [-1] != U'\n')
			n ++;
	} else {
		char *p = & my string8 [0];
		for (; *p != '\0'; p ++) {
			if (*p == '\n')
				n ++;
		}
		if (p - & my string8 [0] > 1 && p [-1] != '\n')
			n ++;
	}
	return n;
}

conststring32 MelderReadText_getLineNumber (MelderReadText me) {
	int64 result = 1;
	if (my string32) {
		char32 *p = & my string32 [0];
		while (my readPointer32 - p > 0) {
			if (*p == U'\0' || *p == U'\n')
				result ++;
			p ++;
		}
	} else {
		char *p = & my string8 [0];
		while (my readPointer8 - p > 0) {
			if (*p == '\0' || *p == '\n')
				result ++;
			p ++;
		}
	}
	return Melder_integer (result);
}

autoMelderReadText MelderReadText_createFromFile (MelderFile file) {
	autoMelderReadText me = std::make_unique <structMelderReadText> ();
	my string32 = MelderFile_readText (file, & my string8);
	if (my string32) {
		my readPointer32 = & my string32 [0];
	} else {
		Melder_assert (my string8);
		my readPointer8 = & my string8 [0];
		my input8Encoding = Melder_getInputEncoding ();
		if (my input8Encoding == kMelder_textInputEncoding::UTF8 ||
			my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1 ||
			my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1 ||
			my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_MACROMAN)
		{
			if (Melder_str8IsValidUtf8 (my string8.get())) {
				my input8Encoding = kMelder_textInputEncoding::UTF8;
			} else if (my input8Encoding == kMelder_textInputEncoding::UTF8) {
				Melder_throw (U"Text is not valid UTF-8; please try a different text input encoding.");
			} else if (my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1) {
				my input8Encoding = kMelder_textInputEncoding::ISO_LATIN1;
			} else if (my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1) {
				my input8Encoding = kMelder_textInputEncoding::WINDOWS_LATIN1;
			} else if (my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_MACROMAN) {
				my input8Encoding = kMelder_textInputEncoding::MACROMAN;
			}
		}
	}
	return me;
}

/* End of file melder_readtext.cpp */
