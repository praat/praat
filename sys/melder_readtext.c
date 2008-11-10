/* melder_readtext.c
 *
 * Copyright (C) 2008 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2008/11/04 split off from melder_strings.c and melder_encodings.c
 */

#include "melder.h"
#include "UnicodeData.h"
#include "abcio.h"
#define my  me ->

struct structMelderReadText {
	bool valid;
	wchar_t *stringW, *readPointerW;
	char *string8, *readPointer8;
	unsigned long input8Encoding;
};

bool MelderReadText_isValid (MelderReadText me) {
	return my valid;
}

wchar_t MelderReadText_getChar (MelderReadText me) {
	if (my stringW != NULL) {
		if (* my readPointerW == '\0') return 0;
		return * my readPointerW ++;
	} else {
		if (* my readPointer8 == '\0') return 0;
		if (my input8Encoding == kMelder_textInputEncoding_UTF8) {
			unsigned long kar = * (unsigned char *) my readPointer8 ++;
			if (kar <= 0x7F) {
				return kar;
			} else if (kar <= 0xDF) {
				unsigned long kar2 = * (unsigned char *) my readPointer8 ++;
				return ((kar & 0x1F) << 6) | (kar2 & 0x3F);
			} else if (kar <= 0xEF) {
				unsigned long kar2 = * (unsigned char *) my readPointer8 ++;
				unsigned long kar3 = * (unsigned char *) my readPointer8 ++;
				return ((kar & 0x0F) << 12) | ((kar2 & 0x3F) << 6) | (kar3 & 0x3F);
			} else if (kar <= 0xF4) {
				unsigned long kar2 = * (unsigned char *) my readPointer8 ++;
				unsigned long kar3 = * (unsigned char *) my readPointer8 ++;
				unsigned long kar4 = * (unsigned char *) my readPointer8 ++;
				return ((kar & 0x07) << 18) | ((kar2 & 0x3F) << 12) | ((kar3 & 0x3F) << 6) | (kar4 & 0x3F);   // BUG: should be UTF-16 on Windows
			}
		} else if (my input8Encoding == kMelder_textInputEncoding_MACROMAN) {
			return Melder_decodeMacRoman [* (unsigned char *) my readPointer8 ++];
		} else if (my input8Encoding == kMelder_textInputEncoding_WINDOWS_LATIN1) {
			return Melder_decodeWindowsLatin1 [* (unsigned char *) my readPointer8 ++];
		}
		return * (unsigned char *) my readPointer8 ++;
	}
}

wchar_t * MelderReadText_readLine (MelderReadText me) {
	if (! my valid) return NULL;
	if (my stringW != NULL) {
		Melder_assert (my readPointerW != NULL);
		Melder_assert (my readPointer8 == NULL);
		if (*my readPointerW == '\0') {   // Tried to read past end of file.
			my valid = false;
			return NULL;
		}
		wchar_t *result = my readPointerW;
		wchar_t *newline = wcschr (result, '\n');
		if (newline != NULL) {
			*newline = '\0';
			my readPointerW = newline + 1;
		} else {
			my readPointerW += wcslen (result);
		}
		return result;
	} else {
		Melder_assert (my string8 != NULL);
		Melder_assert (my readPointerW == NULL);
		Melder_assert (my readPointer8 != NULL);
		if (*my readPointer8 == '\0') {   // Tried to read past end of file.
			my valid = false;
			return NULL;
		}
		char *result8 = my readPointer8;
		char *newline = strchr (result8, '\n');
		if (newline != NULL) {
			*newline = '\0';
			my readPointer8 = newline + 1;
		} else {
			my readPointer8 += strlen (result8);
		}
		static wchar_t *textW = NULL;
		static size_t size = 0;
		size_t sizeNeeded = strlen (result8) + 1;
		if (sizeNeeded > size) {
			Melder_free (textW);
			textW = Melder_malloc (wchar_t, sizeNeeded + 100);
			size = sizeNeeded + 100;
		}
		Melder_8bitToWcs_inline (result8, textW, my input8Encoding);
		return textW;
	}
}

long MelderReadText_getNumberOfLines (MelderReadText me) {
	long n = 0;
	if (my stringW != NULL) {
		wchar_t *p = & my stringW [0];
		for (; *p != '\0'; p ++) if (*p == '\n') n ++;
		if (p - my stringW > 1 && p [-1] != '\n') n ++;
	} else {
		char *p = & my string8 [0];
		for (; *p != '\0'; p ++) if (*p == '\n') n ++;
		if (p - my string8 > 1 && p [-1] != '\n') n ++;
	}
	return n;
}

const wchar_t * MelderReadText_getLineNumber (MelderReadText me) {
	long result = 1;
	if (my stringW != NULL) {
		wchar_t *p = my stringW;
		while (my readPointerW - p > 0) {
			if (*p == '\0' || *p == '\n') result ++;
			p ++;
		}
	} else {
		char *p = my string8;
		while (my readPointer8 - p > 0) {
			if (*p == '\0' || *p == '\n') result ++;
			p ++;
		}
		return Melder_integer (result);
	}
	return Melder_integer (result);
}

static wchar_t * _MelderFile_readText (MelderFile file, char **string8) {
	int type = 0;   // 8-bit
	wchar_t *text = NULL;
	FILE *f = Melder_fopen (file, "rb");
	if (f == NULL) return NULL;
 	fseek (f, 0, SEEK_END);
 	unsigned long length = ftell (f);
 	rewind (f);
	if (length >= 2) {
		int firstByte = fgetc (f), secondByte = fgetc (f);
		if (firstByte == 0xfe && secondByte == 0xff) {
			type = 1;   // big-endian 16-bit
		} else if (firstByte == 0xff && secondByte == 0xfe) {
			type = 2;   // little-endian 16-bit
		}
	}
	if (type == 0) {
		rewind (f);   // length and type already set correctly.
		char *text8bit = Melder_malloc (char, length + 1);
		if (! text8bit) { Melder_fclose (file, f); return NULL; }
		fread (text8bit, sizeof (char), length, f);
		if (! Melder_fclose (file, f)) {
			Melder_free (text8bit);
			Melder_error3 (L"Error reading file ", MelderFile_messageName (file), L".");
			return NULL;
		}
		text8bit [length] = '\0';
		/*
		 * Count and repair null bytes.
		 */
		if (length > 0) {
			long numberOfNullBytes = 0;
			for (char *p = text8bit + (length - 1); (long) (p - text8bit) >= 0; p --) {
				if (*p == '\0') {
					numberOfNullBytes += 1;
					/*
					 * Shift.
					 */
					for (char *q = p; (unsigned long) (q - text8bit) < length; q ++) {
						*q = q [1];
					}
				}
			}
			if (numberOfNullBytes > 0) {
				Melder_warning5 (L"Ignored ", Melder_integer (numberOfNullBytes), L" null bytes in text file ", MelderFile_messageName (file), L".");
			}
		}
		if (string8 != NULL) {
			*string8 = text8bit;
			(void) Melder_killReturns_inline (*string8);
			return NULL;
		} else {
			text = Melder_8bitToWcs (text8bit, 0);
			Melder_free (text8bit);
		}
	} else {
		length = length / 2 - 1;   // Byte Order Mark subtracted. Length = number of UTF-16 codes.
		text = Melder_malloc (wchar_t, length + 1);
		if (! text) { Melder_fclose (file, f); return NULL; }
		if (type == 1) {
			for (unsigned long i = 0; i < length; i ++) {
				unsigned short kar = bingetu2 (f);
				if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
					text [i] = kar;
				} else {   // wchar_t is UTF-32.
					unsigned long kar1 = kar;
					if (kar1 < 0xD800) {
						text [i] = kar1;
					} else if (kar1 < 0xDC00) {
						length --;
						unsigned long kar2 = bingetu2 (f);
						if (kar2 >= 0xDC00 && kar2 <= 0xDFFF) {
							text [i] = 0x10000 + ((kar1 - 0xD800) << 10) + (kar2 - 0xDC00);
						} else {
							text [i] = '?';
						}
					} else if (kar1 < 0xE000) {
						text [i] = '?';
					} else if (kar1 <= 0xFFFF) {
						text [i] = kar1;
					} else {
						Melder_fatal ("MelderFile_readText: unsigned short greater than 0xFFFF: should not occur.");
					}
				}
			}
		} else {
			for (unsigned long i = 0; i < length; i ++) {
				unsigned short kar = bingetu2LE (f);
				if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
					text [i] = kar;
				} else {   // wchar_t is UTF-32.
					unsigned long kar1 = kar;
					if (kar1 < 0xD800) {
						text [i] = kar1;
					} else if (kar1 < 0xDC00) {
						length --;
						unsigned long kar2 = bingetu2LE (f);
						if (kar2 >= 0xDC00 && kar2 <= 0xDFFF) {
							text [i] = 0x10000 + ((kar1 - 0xD800) << 10) + (kar2 - 0xDC00);
						} else {
							text [i] = '?';
						}
					} else if (kar1 < 0xE000) {
						text [i] = '?';
					} else if (kar1 <= 0xFFFF) {
						text [i] = kar1;
					} else {
						Melder_fatal ("MelderFile_readText_LE: unsigned short greater than 0xFFFF: should not occur.");
					}
				}
			}
		}
		if (! Melder_fclose (file, f)) {
			Melder_free (text);
			goto end;
		}
		text [length] = '\0';
		(void) Melder_killReturns_inlineW (text);
	}
end:
	if (text == NULL)
		return Melder_errorp3 (L"Error reading file ", MelderFile_messageName (file), L".");
	return text;
}

wchar_t * MelderFile_readText (MelderFile file) {
	return _MelderFile_readText (file, NULL);
}

MelderReadText MelderReadText_createFromFile (MelderFile file) {
	MelderReadText me = Melder_calloc (struct structMelderReadText, 1); cherror
	my stringW = _MelderFile_readText (file, & my string8); cherror
	if (my stringW != NULL) {
		my readPointerW = & my stringW [0];
	} else {
		Melder_assert (my string8 != NULL);
		my readPointer8 = & my string8 [0];
		my input8Encoding = Melder_getInputEncoding ();
		if (my input8Encoding == kMelder_textInputEncoding_UTF8 ||
			my input8Encoding == kMelder_textInputEncoding_UTF8_THEN_ISO_LATIN1 ||
			my input8Encoding == kMelder_textInputEncoding_UTF8_THEN_WINDOWS_LATIN1 ||
			my input8Encoding == kMelder_textInputEncoding_UTF8_THEN_MACROMAN)
		{
			if (Melder_strIsValidUtf8 (my string8)) {
				my input8Encoding = kMelder_textInputEncoding_UTF8;
			} else if (my input8Encoding == kMelder_textInputEncoding_UTF8) {
				error1 (L"Text is not valid UTF-8; please try a different text input encoding.");
			} else if (my input8Encoding == kMelder_textInputEncoding_UTF8_THEN_ISO_LATIN1) {
				my input8Encoding = kMelder_textInputEncoding_ISO_LATIN1;
			} else if (my input8Encoding == kMelder_textInputEncoding_UTF8_THEN_WINDOWS_LATIN1) {
				my input8Encoding = kMelder_textInputEncoding_WINDOWS_LATIN1;
			} else if (my input8Encoding == kMelder_textInputEncoding_UTF8_THEN_MACROMAN) {
				my input8Encoding = kMelder_textInputEncoding_MACROMAN;
			}
		}
	}
	my valid = true;
end:
	iferror {
		MelderReadText_delete (me);
		me = NULL;
	}
	return me;
}

MelderReadText MelderReadText_createFromString (const wchar_t *string);

void MelderReadText_delete (MelderReadText me) {
	if (me == NULL) return;
	Melder_free (my stringW);
	Melder_free (my string8);
	Melder_free (me);
}

/* End of file melder_readtext.c */
