#ifndef _melder_readtext_h_
#define _melder_readtext_h_
/* melder_readtext.h
 *
 * Copyright (C) 1992-2019 Paul Boersma
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

struct structMelderReadText {
	autostring32 string32;
	char32 *readPointer32;
	autostring8 string8;
	char *readPointer8;
	kMelder_textInputEncoding input8Encoding;
	structMelderReadText () : readPointer32 (nullptr), readPointer8 (nullptr) {
		/*
			Check that C++ default initialization has worked.
		*/
		Melder_assert (! our string32);
		Melder_assert (! our string8);
	}
};
typedef struct structMelderReadText *MelderReadText;

#if 1
	using autoMelderReadText = std::unique_ptr<structMelderReadText>;
#else
struct autoMelderReadText {
	MelderReadText text;
	autoMelderReadText () {
		our text = new structMelderReadText;
	}
	~ autoMelderReadText () {
		delete (our text);
	}
	MelderReadText operator-> () const {   // as r-value
		return our text;
	}
	MelderReadText get () const {
		return our text;
	}
	autoMelderReadText (const autoMelderReadText&) = delete;   // disable copy constructor
	autoMelderReadText (autoMelderReadText&& other) noexcept {   // enable move constructor
		our text = other.text;
		other.text = nullptr;
	}
	autoMelderReadText& operator= (const autoMelderReadText&) = delete;   // disable copy assignment
	autoMelderReadText& operator= (autoMelderReadText&& other) noexcept {   // enable move assignment
		if (& other != this) {
			delete (our text);
			our text = other.text;
			other.text = nullptr;
		}
		return *this;
	}
	autoMelderReadText&& move () noexcept { return static_cast <autoMelderReadText&&> (*this); }
	explicit operator bool () const noexcept { return !! our text; }
};
#endif

autoMelderReadText MelderReadText_createFromFile (MelderFile file);
char32 MelderReadText_getChar (MelderReadText text);
mutablestring32 MelderReadText_readLine (MelderReadText text);
int64 MelderReadText_getNumberOfLines (MelderReadText me);
conststring32 MelderReadText_getLineNumber (MelderReadText text);

/* End of file melder_readText.h */
#endif
