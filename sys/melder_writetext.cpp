/* melder_writetext.cpp
 *
 * Copyright (C) 2007-2011,2015 Paul Boersma
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
 * pb 2007/06/02 utf8 <-> wcs
 * pb 2007/06/14 separated from melder_strings.c and melder_alloc.c
 * pb 2007/06/16 text encoding preferences
 * pb 2007/08/12 prefs in wchar_t
 * pb 2007/09/04 Melder_malloc rather than malloc in Melder_wcsToAscii (had caused an error in counting memory leaks)
 * pb 2007/10/06 Melder_peekWcsToCfstring
 * pb 2007/12/09 made MelderFile_writeCharacter compatible with the ISO Latin-1 preference
 * pb 2007/12/09 made MelderFile_readText ignore null bytes
 * pb 2008/11/05 split off from melder_encodings.c
 * pb 2010/03/09 support Unicode values above 0xFFFF
 * pb 2011/04/05 C++
 */

#include "melder.h"
#include "Preferences.h"
#include "UnicodeData.h"
#include "abcio.h"

#if defined (macintosh)
	#include "macport_on.h"
    #if useCarbon
        #include <Carbon/Carbon.h>
    #endif
	#include "macport_off.h"
#endif

static void Melder_fwriteUnicodeAsUtf8 (char32_t unicode, FILE *f) {
	if (unicode <= 0x0000007F) {
		#ifdef _WIN32
			if (unicode == '\n') fputc (13, f);
		#endif
		fputc ((int) unicode, f);   // because fputc wants an int instead of an uint8_t (guarded conversion)
	} else if (unicode <= 0x000007FF) {
		fputc (0xC0 | (unicode >> 6), f);
		fputc (0x80 | (unicode & 0x0000003F), f);
	} else if (unicode <= 0x0000FFFF) {
		fputc (0xE0 | (unicode >> 12), f);
		fputc (0x80 | ((unicode >> 6) & 0x0000003F), f);
		fputc (0x80 | (unicode & 0x0000003F), f);
	} else {
		fputc (0xF0 | (unicode >> 18), f);
		fputc (0x80 | ((unicode >> 12) & 0x0000003F), f);
		fputc (0x80 | ((unicode >> 6) & 0x0000003F), f);
		fputc (0x80 | (unicode & 0x0000003F), f);
	}
}

void Melder_fwriteWcsAsUtf8 (const wchar_t *ptr, size_t n, FILE *f) {
	/*
	 * Precondition:
	 *    the string's encoding is either UTF-32 or UTF-16.
	 * Failure:
	 *    if the precondition does not hold, we don't crash,
	 *    but the characters that are written may be incorrect.
	 */
	for (size_t i = 0; i < n; i ++) {
		if (sizeof (wchar_t) == 4) {
			/*
			 * We are likely to be on Macintosh or Linux.
			 * We assume that the string's encoding is UTF-32.
			 */
			Melder_fwriteUnicodeAsUtf8 ((char32_t) ptr [i], f);   // safe conversion, because sign bit is zero
		} else if (sizeof (wchar_t) == 2) {
			/*
			 * We are likely to be on Windows.
			 * We assume that the string's encoding is UTF-16;
			 * if it turns out to be otherwise, we write question marks.
			 */
			char32_t kar1 = (char32_t) (char16_t) ptr [i];   // don't extend sign
			if (kar1 < 0x00D800) {
				Melder_fwriteUnicodeAsUtf8 (kar1, f);   // a character from the Basic Multilingual Plane
			} else if (kar1 < 0x00DC00) {
				/*
				 * We detected a surrogate code point
				 * and will therefore translate two UTF-16 words into one Unicode supplementary character.
				 */
				char32_t kar2 = (char32_t) (char16_t) ptr [++ i];   // don't extend sign
				if (kar2 == 0) {   // string exhausted?
					// Melder_fatal ("Detected a bare (final) high surrogate in UTF-16.");
					Melder_fwriteUnicodeAsUtf8 (UNICODE_REPLACEMENT_CHARACTER, f);
					return;
				}
				if (kar2 >= 0x00DC00 && kar2 <= 0x00DFFF) {
					Melder_fwriteUnicodeAsUtf8 (0x010000 + ((kar1 - 0x00D800) << 10) + (kar2 - 0x00DC00), f);
				} else {
					// Melder_fatal ("Detected a bare high surrogate in UTF-16.");
					Melder_fwriteUnicodeAsUtf8 (UNICODE_REPLACEMENT_CHARACTER, f);
					i --;   // try to interpret the next character in the normal way
				}
			} else if (kar1 < 0x00E000) {
				// Melder_fatal ("Detected a bare low surrogate in UTF-16.");
				Melder_fwriteUnicodeAsUtf8 (UNICODE_REPLACEMENT_CHARACTER, f);
			} else if (kar1 <= 0x00FFFF) {
				Melder_fwriteUnicodeAsUtf8 (kar1, f);   // a character from the Basic Multilingual Plane
			} else {
				Melder_fatal ("Melder_fwriteWcsAsUtf8: unsigned short greater than 0xFFFF: should not occur.");
			}
		} else {
			Melder_fatal ("Melder_fwriteWcsAsUtf8: unsupported size of wide character.");
		}
	}
}

void MelderFile_writeText (MelderFile file, const wchar_t *text, enum kMelder_textOutputEncoding outputEncoding) {
	autofile f = Melder_fopen (file, "wb");
	if (outputEncoding == kMelder_textOutputEncoding_UTF8) {
		Melder_fwriteWcsAsUtf8 (text, wcslen (text), f);
	} else if ((outputEncoding == kMelder_textOutputEncoding_ASCII_THEN_UTF16 && Melder_isValidAscii (text)) ||
		(outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ISO_LATIN1)))
	{
		#ifdef _WIN32
			#define flockfile(f)  (void) 0
			#define funlockfile(f)  (void) 0
			#define putc_unlocked  putc
		#endif
		flockfile (f);
		size_t n = wcslen (text);
		for (size_t i = 0; i < n; i ++) {
			char32_t kar = sizeof (wchar_t) == 2 ?
				(char16_t) text [i] :   // don't extend sign
				(char32_t) text [i];
			#ifdef _WIN32
				if (kar == '\n') putc_unlocked (13, f);
			#endif
			putc_unlocked (kar, f);
		}
		funlockfile (f);
	} else {
		binputu2 (0xFEFF, f);   // Byte Order Mark
		size_t n = wcslen (text);
		for (size_t i = 0; i < n; i ++) {
			if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
				char16_t kar = (char16_t) text [i];
				#ifdef _WIN32
					if (kar == '\n') binputu2 (13, f);
				#endif
				binputu2 (kar, f);
			} else {   // wchar_t is UTF-32.
				char32_t kar = (char32_t) text [i];
				#ifdef _WIN32
					if (kar == '\n') binputu2 (13, f);
				#endif
				if (kar <= 0x00FFFF) {
					binputu2 ((char16_t) kar, f);   // guarded conversion down
				} else if (kar <= 0x10FFFF) {
					kar -= 0x010000;
					binputu2 (0xD800 | (uint16_t) (kar >> 10), f);
					binputu2 (0xDC00 | (uint16_t) ((char16_t) kar & 0x3ff), f);
				} else {
					binputu2 (UNICODE_REPLACEMENT_CHARACTER, f);
				}
			}
		}
	}
	f.close (file);
}

void MelderFile_appendText (MelderFile file, const wchar_t *text) {
	autofile f1;
	try {
		f1.reset (Melder_fopen (file, "rb"));
	} catch (MelderError) {
		Melder_clearError ();   // it's OK if the file didn't exist yet...
		MelderFile_writeText (file, text, Melder_getOutputEncoding ());   // because then we just "write"
		return;
	}
	/*
	 * The file already exists and is open. Determine its type.
	 */
	int firstByte = fgetc (f1), secondByte = fgetc (f1);
	f1.close (file);
	int type = 0;
	if (firstByte == 0xfe && secondByte == 0xff) {
		type = 1;   // big-endian 16-bit
	} else if (firstByte == 0xff && secondByte == 0xfe) {
		type = 2;   // little-endian 16-bit
	}
	if (type == 0) {
		int outputEncoding = Melder_getOutputEncoding ();
		if (outputEncoding == kMelder_textOutputEncoding_UTF8) {   // TODO: read as file's encoding
			autofile f2 = Melder_fopen (file, "ab");
			Melder_fwriteWcsAsUtf8 (text, wcslen (text), f2);
			f2.close (file);
		} else if ((outputEncoding == kMelder_textOutputEncoding_ASCII_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ASCII))
		    || (outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ISO_LATIN1)))
		{
			/*
			 * Append ASCII or ISOLatin1 text to ASCII or ISOLatin1 file.
			 */
			autofile f2 = Melder_fopen (file, "ab");
			size_t n = wcslen (text);
			for (size_t i = 0; i < n; i ++) {
				uint32_t kar = sizeof (wchar_t) == 2 ?
					(uint16_t) text [i] :   // don't extend sign
					(uint32_t) text [i];
				#ifdef _WIN32
					if (kar == '\n') fputc (13, f2);
				#endif
				fputc ((uint8_t) kar, f2);
			}
			f2.close (file);
		} else {
			/*
			 * Convert to wide character file.
			 */
			autostring oldText = MelderFile_readText (file);
			autofile f2 = Melder_fopen (file, "wb");
			binputu2 (0xfeff, f2);
			size_t n = wcslen (oldText.peek());
			for (size_t i = 0; i < n; i ++) {
				if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
					char16_t kar = (char16_t) oldText [i];   // reinterpret sign bit
					#ifdef _WIN32
						if (kar == '\n') binputu2 (13, f2);
					#endif
					binputu2 (kar, f2);
				} else {   // wchar_t is UTF-32.
					char32_t kar = (char32_t) oldText [i];   // sign bit is always 0
					#ifdef _WIN32
						if (kar == '\n') binputu2 (13, f2);
					#endif
					if (kar <= 0x0000FFFF) {
						binputu2 ((uint16_t) kar, f2);   // guarded conversion down
					} else if (kar <= 0x0010FFFF) {
						kar -= 0x00010000;
						binputu2 (0xD800 | (uint16_t) (kar >> 10), f2);
						binputu2 (0xDC00 | (uint16_t) ((uint16_t) kar & 0x03ff), f2);
					} else {
						binputu2 (UNICODE_REPLACEMENT_CHARACTER, f2);
					}
				}
			}
			n = wcslen (text);
			for (unsigned long i = 0; i < n; i ++) {
				if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
					char16_t kar = (char16_t) text [i];   // reinterpret sign bit
					#ifdef _WIN32
						if (kar == '\n') binputu2 (13, f2);
					#endif
					binputu2 (kar, f2);
				} else {   // wchar_t is UTF-32.
					char32_t kar = (char32_t) text [i];   // sign bit is always 0
					#ifdef _WIN32
						if (kar == '\n') binputu2 (13, f2);
					#endif
					if (kar <= 0x00FFFF) {
						binputu2 ((char16_t) kar, f2);   // guarded conversion down
					} else if (kar <= 0x10FFFF) {
						kar -= 0x010000;
						binputu2 (0xD800 | (char16_t) (kar >> 10), f2);
						binputu2 (0xDC00 | (char16_t) ((char16_t) kar & 0x03ff), f2);
					} else {
						binputu2 (UNICODE_REPLACEMENT_CHARACTER, f2);
					}
				}
			}
			f2.close (file);
		}
	} else {
		autofile f2 = Melder_fopen (file, "ab");
		unsigned long n = wcslen (text);
		for (unsigned long i = 0; i < n; i ++) {
			if (type == 1) {
				if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
					uint16_t kar = (uint16_t) text [i];   // reinterpret sign bit
					#ifdef _WIN32
						if (kar == '\n') binputu2 (13, f2);
					#endif
					binputu2 (kar, f2);
				} else {   // wchar_t is UTF-32
					char32_t kar = (char32_t) text [i];   // sign bit is always 0
					#ifdef _WIN32
						if (kar == '\n') binputu2 (13, f2);
					#endif
					if (kar <= 0x00FFFF) {
						binputu2 ((char16_t) kar, f2);   // guarded conversion down
					} else if (kar <= 0x10FFFF) {
						kar -= 0x010000;
						binputu2 (0xD800 | (uint16_t) (kar >> 10), f2);
						binputu2 (0xDC00 | (uint16_t) ((uint16_t) kar & 0x03ff), f2);
					} else {
						binputu2 (UNICODE_REPLACEMENT_CHARACTER, f2);
					}
				}
			} else {
				if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
					uint16_t kar = (uint16_t) text [i];   // reinterpret sign bit
					#ifdef _WIN32
						if (kar == '\n') binputu2LE (13, f2);
					#endif
					binputu2LE (kar, f2);
				} else {   // wchar_t is UTF-32
					char32_t kar = (char32_t) text [i];   // sign bit is always 0
					#ifdef _WIN32
						if (kar == '\n') binputu2LE (13, f2);
					#endif
					if (kar <= 0x00FFFF) {
						binputu2LE ((char16_t) kar, f2);   // guarded conversion down
					} else if (kar <= 0x10FFFF) {
						kar -= 0x010000;
						binputu2LE (0xD800 | (uint16_t) (kar >> 10), f2);
						binputu2LE (0xDC00 | (uint16_t) ((uint16_t) kar & 0x3ff), f2);
					} else {
						binputu2LE (UNICODE_REPLACEMENT_CHARACTER, f2);
					}
				}
			}
		}
		f2.close (file);
	}
}

static void _MelderFile_write (MelderFile file, const wchar_t *string) {
	if (string == NULL) return;
	size_t length = wcslen (string);
	FILE *f = file -> filePointer;
	if (file -> outputEncoding == kMelder_textOutputEncoding_ASCII || file -> outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1) {
		for (size_t i = 0; i < length; i ++) {
			char kar = string [i];   // truncate
			if (kar == '\n' && file -> requiresCRLF) putc (13, f);
			putc (kar, f);
		}
	} else if (file -> outputEncoding == kMelder_textOutputEncoding_UTF8) {
		for (size_t i = 0; i < length; i ++) {
			char32_t kar = sizeof (wchar_t) == 2 ?
				(char16_t) string [i] :   // don't extend sign
				(char32_t) string [i];
			if (kar <= 0x00007F) {
				if (kar == '\n' && file -> requiresCRLF) putc (13, f);
				putc ((int) kar, f);   // guarded conversion down
			} else if (kar <= 0x0007FF) {
				putc (0xC0 | (kar >> 6), f);
				putc (0x80 | (kar & 0x00003F), f);
			} else if (kar <= 0x00FFFF) {
				if (sizeof (wchar_t) == 2) {
					if ((kar & 0x00F800) == 0x00D800) {
						if (kar > 0x00DBFF)
							Melder_fatal ("Incorrect Unicode value (first surrogate member %lX).", kar);
						char32_t kar2 = (char32_t) (char16_t) string [++ i];   // don't extend sign
						if (kar2 < 0x00DC00 || kar2 > 0x00DFFF)
							Melder_fatal ("Incorrect Unicode value (second surrogate member %lX).", kar2);
						kar = (((kar & 0x0003FF) << 10) | (kar2 & 0x0003FF)) + 0x010000;   // decode UTF-16
						putc (0xF0 | (kar >> 18), f);
						putc (0x80 | ((kar >> 12) & 0x00003F), f);
						putc (0x80 | ((kar >> 6) & 0x00003F), f);
						putc (0x80 | (kar & 0x00003F), f);
					} else {
						putc (0xE0 | (kar >> 12), f);
						putc (0x80 | ((kar >> 6) & 0x00003F), f);
						putc (0x80 | (kar & 0x00003F), f);
					}
				} else {
					putc (0xE0 | (kar >> 12), f);
					putc (0x80 | ((kar >> 6) & 0x00003F), f);
					putc (0x80 | (kar & 0x00003F), f);
				}
			} else {
				putc (0xF0 | (kar >> 18), f);
				putc (0x80 | ((kar >> 12) & 0x00003F), f);
				putc (0x80 | ((kar >> 6) & 0x00003F), f);
				putc (0x80 | (kar & 0x00003F), f);
			}
		}
	} else {
		for (size_t i = 0; i < length; i ++) {
			char32_t kar = sizeof (wchar_t) == 2 ?
				(char16_t) string [i] :   // don't extend sign
				(char32_t) string [i];
			if (kar == '\n' && file -> requiresCRLF) binputu2 (13, f);
			if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
				binputu2 ((char16_t) kar, f);
			} else {   // wchar_t is UTF-32.
				if (kar <= 0x00FFFF) {
					binputu2 ((char16_t) kar, f);
				} else if (kar <= 0x10FFFF) {
					kar -= 0x010000;
					binputu2 (0xD800 | (char16_t) (kar >> 10), f);
					binputu2 (0xDC00 | (char16_t) ((char16_t) kar & 0x03ff), f);
				} else {
					binputu2 (UNICODE_REPLACEMENT_CHARACTER, f);
				}
			}
		}
	}
}

void MelderFile_writeCharacter (MelderFile file, wchar_t character) {
	FILE *f = file -> filePointer;
	char32_t kar = sizeof (wchar_t) == 2 ?
		(char16_t) character :   // don't extend sign
		(char32_t) character;
	if (file -> outputEncoding == kMelder_textOutputEncoding_ASCII || file -> outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1) {
		if (kar == '\n' && file -> requiresCRLF) putc (13, f);
		putc (kar, f);
	} else if (file -> outputEncoding == kMelder_textOutputEncoding_UTF8) {
		if (kar <= 0x00007F) {
			if (kar == '\n' && file -> requiresCRLF) putc (13, f);
			putc ((int) kar, f);   // guarded conversion down
		} else if (kar <= 0x0007FF) {
			putc (0xC0 | (kar >> 6), f);
			putc (0x80 | (kar & 0x00003F), f);
		} else if (kar <= 0x00FFFF) {
			if (sizeof (wchar_t) == 2) {
				if ((kar & 0x00F800) == 0x00D800) {
					static char32_t buffer;   // NOT REENTRANT
					if (kar >= 0x00D800 && kar <= 0x00DBFF) {
						buffer = kar;
					} else {
						kar = (((buffer & 0x3FF) << 10) | (kar & 0x3FF)) + 0x10000;   // decode UTF-16
						putc (0xF0 | (kar >> 18), f);
						putc (0x80 | ((kar >> 12) & 0x00003F), f);
						putc (0x80 | ((kar >> 6) & 0x00003F), f);
						putc (0x80 | (kar & 0x00003F), f);
					}
				} else {
					putc (0xE0 | (kar >> 12), f);
					putc (0x80 | ((kar >> 6) & 0x00003F), f);
					putc (0x80 | (kar & 0x00003F), f);
				}
			} else {
				putc (0xE0 | (kar >> 12), f);
				putc (0x80 | ((kar >> 6) & 0x00003F), f);
				putc (0x80 | (kar & 0x00003F), f);
			}
		} else {
			putc (0xF0 | (kar >> 18), f);
			putc (0x80 | ((kar >> 12) & 0x00003F), f);
			putc (0x80 | ((kar >> 6) & 0x00003F), f);
			putc (0x80 | (kar & 0x00003F), f);
		}
	} else {
		if (kar == '\n' && file -> requiresCRLF) binputu2 (13, f);
		if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
			binputu2 ((uint16_t) kar, f);
		} else {   // wchar_t is UTF-32.
			if (kar <= 0x0000FFFF) {
				binputu2 ((uint16_t) kar, f);
			} else if (kar <= 0x0010FFFF) {
				kar -= 0x00010000;
				binputu2 (0xD800 | (uint16_t) (kar >> 10), f);
				binputu2 (0xDC00 | (uint16_t) ((uint16_t) kar & 0x03ff), f);
			} else {
				binputu2 (UNICODE_REPLACEMENT_CHARACTER, f);
			}
		}
	}
}

void MelderFile_write (MelderFile file, const wchar_t *s1) {
	if (file -> filePointer == NULL) return;
	_MelderFile_write (file, s1);
}
void MelderFile_write (MelderFile file, const wchar_t *s1, const wchar_t *s2) {
	if (file -> filePointer == NULL) return;
	_MelderFile_write (file, s1);
	_MelderFile_write (file, s2);
}
void MelderFile_write (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	if (file -> filePointer == NULL) return;
	_MelderFile_write (file, s1);
	_MelderFile_write (file, s2);
	_MelderFile_write (file, s3);
}
void MelderFile_write (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	if (file -> filePointer == NULL) return;
	_MelderFile_write (file, s1);
	_MelderFile_write (file, s2);
	_MelderFile_write (file, s3);
	_MelderFile_write (file, s4);
}
void MelderFile_write (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5)
{
	if (file -> filePointer == NULL) return;
	_MelderFile_write (file, s1);
	_MelderFile_write (file, s2);
	_MelderFile_write (file, s3);
	_MelderFile_write (file, s4);
	_MelderFile_write (file, s5);
}
void MelderFile_write (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6)
{
	if (file -> filePointer == NULL) return;
	_MelderFile_write (file, s1);
	_MelderFile_write (file, s2);
	_MelderFile_write (file, s3);
	_MelderFile_write (file, s4);
	_MelderFile_write (file, s5);
	_MelderFile_write (file, s6);
}
void MelderFile_write (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7)
{
	if (file -> filePointer == NULL) return;
	_MelderFile_write (file, s1);
	_MelderFile_write (file, s2);
	_MelderFile_write (file, s3);
	_MelderFile_write (file, s4);
	_MelderFile_write (file, s5);
	_MelderFile_write (file, s6);
	_MelderFile_write (file, s7);
}
void MelderFile_write (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8)
{
	if (file -> filePointer == NULL) return;
	_MelderFile_write (file, s1);
	_MelderFile_write (file, s2);
	_MelderFile_write (file, s3);
	_MelderFile_write (file, s4);
	_MelderFile_write (file, s5);
	_MelderFile_write (file, s6);
	_MelderFile_write (file, s7);
	_MelderFile_write (file, s8);
}
void MelderFile_write (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9)
{
	if (file -> filePointer == NULL) return;
	_MelderFile_write (file, s1);
	_MelderFile_write (file, s2);
	_MelderFile_write (file, s3);
	_MelderFile_write (file, s4);
	_MelderFile_write (file, s5);
	_MelderFile_write (file, s6);
	_MelderFile_write (file, s7);
	_MelderFile_write (file, s8);
	_MelderFile_write (file, s9);
}

/* End of file melder_writetext.cpp */
