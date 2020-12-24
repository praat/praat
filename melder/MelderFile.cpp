/* MelderFile.cpp
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

extern "C" int  FLAC__stream_encoder_finish (FLAC__StreamEncoder *);
extern "C" void FLAC__stream_encoder_delete (FLAC__StreamEncoder *);

/*
 * Functions for wrapping the file pointers.
 */

MelderFile MelderFile_open (MelderFile me) {
	my filePointer = Melder_fopen (me, "rb");
	my openForReading = true;
	return me;
}

char * MelderFile_readLine8 (MelderFile me) {
	if (! my filePointer)
		return nullptr;
	if (feof (my filePointer))
		return nullptr;
	static char *buffer;
	static integer capacity;
	if (! buffer)
		buffer = Melder_malloc (char, capacity = 100);
	integer i = 0;
	for (; true; i ++) {
		if (i >= capacity)
			buffer = (char *) Melder_realloc (buffer, capacity *= 2);
		int c = fgetc (my filePointer);
		if (feof (my filePointer))
			break;
		if (c == '\n') {
			c = fgetc (my filePointer);
			if (feof (my filePointer))
				break;   // ignore last empty line (Unix)
			ungetc (c, my filePointer);
			break;   // Unix line separator
		}
		if (c == '\r') {
			c = fgetc (my filePointer);
			if (feof (my filePointer))
				break;   // ignore last empty line (Macintosh)
			if (c == '\n') {
				c = fgetc (my filePointer);
				if (feof (my filePointer))
					break;   // ignore last empty line (Windows)
				ungetc (c, my filePointer);
				break;   // Windows line separator
			}
			ungetc (c, my filePointer);
			break;   // Macintosh line separator
		}
		buffer [i] = c;
	}
	buffer [i] = '\0';
	return buffer;
}

MelderFile MelderFile_create (MelderFile me) {
	my filePointer = Melder_fopen (me, "wb");
	my openForWriting = true;   // a bit superfluous (will have been set by Melder_fopen)
	return me;
}

void MelderFile_write (MelderFile file, conststring32 string) {
	if (! file -> filePointer)
		return;
	if (! string)
		return;
	int64 length = str32len (string);
	FILE *f = file -> filePointer;
	if (file -> outputEncoding == kMelder_textOutputEncoding_ASCII || file -> outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1) {
		for (int64 i = 0; i < length; i ++) {
			char kar = (char) (char8) string [i];   // truncate
			if (kar == '\n' && file -> requiresCRLF)
				putc (13, f);
			putc (kar, f);
		}
	} else if (file -> outputEncoding == (unsigned long) kMelder_textOutputEncoding::UTF8) {
		for (int64 i = 0; i < length; i ++) {
			char32 kar = string [i];
			if (kar <= 0x00'007F) {
				if (kar == U'\n' && file -> requiresCRLF)
					putc (13, f);
				putc ((int) kar, f);   // guarded conversion down
			} else if (kar <= 0x00'07FF) {
				putc (0xC0 | (kar >> 6), f);
				putc (0x80 | (kar & 0x00'003F), f);
			} else if (kar <= 0x00'FFFF) {
				putc (0xE0 | (kar >> 12), f);
				putc (0x80 | ((kar >> 6) & 0x00'003F), f);
				putc (0x80 | (kar & 0x00'003F), f);
			} else {
				putc (0xF0 | (kar >> 18), f);
				putc (0x80 | ((kar >> 12) & 0x00'003F), f);
				putc (0x80 | ((kar >> 6) & 0x00'003F), f);
				putc (0x80 | (kar & 0x00'003F), f);
			}
		}
	} else {
		for (int64 i = 0; i < length; i ++) {
			char32 kar = string [i];
			if (kar == U'\n' && file -> requiresCRLF)
				binputu16 (13, f);
			if (kar <= 0x00'FFFF) {
				binputu16 ((char16) kar, f);
			} else if (kar <= 0x10'FFFF) {
				kar -= 0x01'0000;
				binputu16 (0xD800 | (char16) (kar >> 10), f);
				binputu16 (0xDC00 | (char16) ((char16) kar & 0x03ff), f);
			} else {
				binputu16 (UNICODE_REPLACEMENT_CHARACTER, f);
			}
		}
	}
}

void MelderFile_writeCharacter (MelderFile file, char32 kar) {
	FILE *f = file -> filePointer;
	if (! f)
		return;
	if (file -> outputEncoding == kMelder_textOutputEncoding_ASCII || file -> outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1) {
		if (kar == U'\n' && file -> requiresCRLF)
			putc (13, f);
		putc ((int) kar, f);
	} else if (file -> outputEncoding == (unsigned long) kMelder_textOutputEncoding::UTF8) {
		if (kar <= 0x00'007F) {
			if (kar == U'\n' && file -> requiresCRLF)
				putc (13, f);
			putc ((int) kar, f);   // guarded conversion down
		} else if (kar <= 0x00'07FF) {
			putc (0xC0 | (kar >> 6), f);
			putc (0x80 | (kar & 0x00'003F), f);
		} else if (kar <= 0x00'FFFF) {
			putc (0xE0 | (kar >> 12), f);
			putc (0x80 | ((kar >> 6) & 0x00'003F), f);
			putc (0x80 | (kar & 0x00'003F), f);
		} else {
			putc (0xF0 | (kar >> 18), f);
			putc (0x80 | ((kar >> 12) & 0x00'003F), f);
			putc (0x80 | ((kar >> 6) & 0x00'003F), f);
			putc (0x80 | (kar & 0x00'003F), f);
		}
	} else {
		if (kar == U'\n' && file -> requiresCRLF)
			binputu16 (13, f);
		if (kar <= 0x00'FFFF) {
			binputu16 ((uint16) kar, f);
		} else if (kar <= 0x10'FFFF) {
			kar -= 0x01'0000;
			binputu16 (0xD800 | (uint16) (kar >> 10), f);
			binputu16 (0xDC00 | (uint16) ((uint16) kar & 0x00'03ff), f);
		} else {
			binputu16 (UNICODE_REPLACEMENT_CHARACTER, f);
		}
	}
}

void MelderFile_seek (MelderFile me, integer position, int direction) {
	if (! my filePointer)
		return;
	if (fseek (my filePointer, position, direction)) {
		fclose (my filePointer);
		my filePointer = nullptr;
		Melder_throw (U"Cannot seek in file ", me, U".");
	}
}

integer MelderFile_tell (MelderFile me) {
	if (! my filePointer)
		return 0;
	integer result = ftell (my filePointer);
	if (result == -1) {
		fclose (my filePointer);
		my filePointer = nullptr;
		Melder_throw (U"Cannot tell in file ", me, U".");
	}
	return result;
}

void MelderFile_rewind (MelderFile me) {
	if (! my filePointer)
		return;
	rewind (my filePointer);
}

static void _MelderFile_close (MelderFile me, bool mayThrow) {
	if (my outputEncoding == kMelder_textOutputEncoding_FLAC) {
		if (my flacEncoder) {
			FLAC__stream_encoder_finish (my flacEncoder);   // This already calls fclose! BUG: we cannot get any error messages out.
			FLAC__stream_encoder_delete (my flacEncoder);
		}
	} else if (my filePointer) {
		if (mayThrow)
			Melder_fclose (me, my filePointer);
		else
			fclose (my filePointer);
	}
	/* Set everything to zero, except paths (they stay around for error messages and the like). */
	my filePointer = nullptr;
	my openForWriting = my openForReading = false;
	my indent = 0;
	my flacEncoder = nullptr;
}
void MelderFile_close (MelderFile me) {
	_MelderFile_close (me, true);
}
void MelderFile_close_nothrow (MelderFile me) {
	_MelderFile_close (me, false);
}

/* End of file MelderFile.cpp */
