#ifndef _STRVEC_sorting_h_
#define _STRVEC_sorting_h_
/* STRVEC_sorting.cpp
 *
 * Copyright (C) 2022 David Weenink
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

#include "Index.h"
#include "NUM2.h"
#include "Permutation.h"
#include "Strings_.h"

/*
	Sort strings that may have numeric substrings:
	string    := <alphaPart> | <numPart>
	alphaPart := <alphas> | <alphas><numPart>
	numPart   := <nums> | <nums><alphaPart>

		Natural Order uses the following rule to sort strings:

		1. The space character sorts before numbers; numbers sort before non-numbers.
			' 5' < '5' < 'a'
		2. Numbers sort in numerical order; leading zeroes and spaces on numbers are ignored, except as a tie-breaker
			for numbers that have the same numerical value.
			' 5' < ' 5 ' < '5' < '05' < '005'
		3. Non-numbers sort in the asciibetical order ('Z' before 'a').
			'A' < 'B' ... < "Z' < 'a' ... 'z' 
	
	<alphaPart> sorts the <alpha> alphabetical, <numPart> sorts the <nums> numerical:
	0a, 1a, 11a, 2a, 11a -> 0a, 1a, 2a, 11a, 11a
	a, 00a, 0a -> 0a, 00a, a, 
	00a1, 00a2, 00a11, 0b0
*/

typedef struct structSTRVECIndexer *STRVECIndexer;

typedef struct structStringDatum *stringDatum;
typedef struct structStringDatum StringDatum;

using StringDataVec = vector<StringDatum>;
using constStringDataVec = constvector <StringDatum>;
using autoStringDataVec = autovector <StringDatum>;

inline void INTVECindex_inout (INTVEC index, StringDataVec const& v);


struct structStringDatum {
	
	struct structNumDatum { // for easy saving and restoring number data
		integer numPosStart; // start of num part in alpha, else 0 if no num part
		integer numPosDot; // numPosStart < numPosDot < numPosEnd if (breakAtDecimalPoint==true and there is a dot)
		integer numPosEnd; // >= numPosStart if numPosStart > 0 else undefined
		integer numberOfLeadingZeros;
		integer numberOfLeadingSpaces;
		char32 save0;
	};
	typedef struct structNumDatum NumDatum;

	mutablestring32 alpha; // either <alphaPart> or <numPart>
	integer length; // strlen (alpha)
	integer alphaPosStart;
	char32 save0; // efficiency: instead of copying we mask part of alpha by putting a '\0'
	bool breakAtDecimalPoint;
	NumDatum number;
	
private:
	
	integer compareLeadings (stringDatum y) {
		return (number.numberOfLeadingZeros < y ->  number.numberOfLeadingZeros ? - 1 : 
				number.numberOfLeadingZeros > y -> number.numberOfLeadingZeros ? 1 :
				number.numberOfLeadingSpaces > y -> number.numberOfLeadingSpaces ? - 1 :
				number.numberOfLeadingSpaces < y -> number.numberOfLeadingSpaces ? 1 :
				0 );
	}
	
	integer compareNumPart (stringDatum y) {
		const integer xlength = ( breakAtDecimalPoint ? number.numPosEnd - number.numPosStart :
			number.numPosEnd - number.numPosDot );
		const integer ylength = ( y -> breakAtDecimalPoint ? y -> number.numPosEnd - y -> number.numPosStart :
			y -> number.numPosEnd - y -> number.numPosDot );
		if (xlength < ylength)
			return - 1;
		else if (xlength > ylength)
			return 1;
		/*
			the digitstrings have equal lengths
			we could simply convert both to a number and compare the size
			or do a character by character <num> comparison
			*/
		char32 *px = alpha + number.numPosStart - 1;
		char32 *py = y -> alpha + y -> number.numPosStart - 1;
		for ( ; px < alpha + number.numPosEnd; px ++, py ++) {
			if (*px < *py)
				return - 1;
			else if (*px > *py)
				return 1;
		}
		if (! breakAtDecimalPoint && y -> breakAtDecimalPoint)
			return -1;
		else if (breakAtDecimalPoint && ! y -> breakAtDecimalPoint)
			return 1;
		for ( ; px < alpha + number.numPosEnd || py < y -> alpha + y -> number.numPosEnd; px ++, py ++) {
			if (*px < *py)
				return - 1;
			else if (*px > *py)
				return 1;
		}
		/*
			both numbers are equal
			compare the alphaPart
		*/
		bool xNotAtEnd = setAlphaPosStartAfterNumPosEnd ();
		bool yNotAtEnd = y -> setAlphaPosStartAfterNumPosEnd ();
		if (xNotAtEnd && ! yNotAtEnd)
			return 1;
		else if (! xNotAtEnd && yNotAtEnd)
			return - 1;
		else if (! xNotAtEnd && ! yNotAtEnd)
			return compareLeadings (y);
		// both are not at the end 
		integer alphaPart = compareAlphaPart (y);
		if (alphaPart < 0)
			return -1;
		else if (alphaPart > 0)
			return 1;
		/*
			<alphaPart>'s are also equal,
			let the leading zeros and spaces decide
		*/	
		return compareLeadings (y);
	}
	
	integer compareAlphaPart (stringDatum y) {
		const char32 xalpha = alpha [alphaPosStart - 1];
		const char32 yalpha = y -> alpha [y -> alphaPosStart - 1];
		if (xalpha <  yalpha)
			return - 1;
		else if (xalpha  > yalpha)
			return 1;
		/*
			compare the <alphas> of the <alphaPart>
		*/
		setNumber ();
		y -> setNumber ();
		maskNumTrailer ();
		y -> maskNumTrailer ();
		char32 *px = alpha + alphaPosStart - 1;
		char32 *py = y -> alpha + y -> alphaPosStart - 1;
		integer cmp = str32cmp (px, py);
		maskNumTrailer_undo ();
		y -> maskNumTrailer_undo ();
		if (cmp < 0)
			return -1;
		else if (cmp > 0)
			return 1;
		// <alpha>'s are equal
		if (number.numPosStart > 0 && ! (y -> number.numPosStart > 0))
			return 1;
		else if (! (number.numPosStart > 0) && y -> number.numPosStart > 0)
			return - 1;
		else if (! (number.numPosStart > 0) && ! (y -> number.numPosStart > 0))
			return 0;
		return compareNumPart (y);
	}
	
	/*
		We use a mask, i.e. a '\0' at positions in the string 'alpha' to mask the
		the <alpaPart> of the <numPart> or the <numPart> of the <alpahPart>.
		The unmask is used to undo the mask.
	*/
	
	void maskNumTrailer () {
		if (number.numPosStart > 0) {
			integer posSave = number.numPosStart - number.numberOfLeadingZeros - number.numberOfLeadingSpaces - 1;
			save0 = alpha [posSave];
			alpha [posSave] = U'\0';
		}
	}
		
	void maskNumTrailer_undo () {
		if (number.numPosStart > 0)
			alpha [number.numPosStart - number.numberOfLeadingZeros - number.numberOfLeadingSpaces - 1] = save0;
	}
	
	void maskAlphaTrailer () { //  save the char32 after number part and replace by U'\0'
		if (number.numPosStart > 0 && number.numPosEnd < length) {  
			save0 = alpha [number.numPosEnd]; // save position after last digit
			alpha [number.numPosEnd] = U'\0'; // mark number part only
		}
	}

	void maskAlphaTrailer_undo () { // undo the masking!
		if (number.numPosStart > 0 && number.numPosEnd < length)
			alpha [number.numPosEnd] = save0; 
		}
	
	/*
		Find the first and last index of the first numeric part in a string.
		The allowed numeric part is either <d> or <d>.<d>, where <d> a sequence of one or more digits
		Examples with breakAtDecimalPoint
					false 		true
		s1d 		-> 2,2		2,2
		2.222		-> 1,5		1,1
		abc123		-> 4,6		4,6
		a.3			-> 3,3		3,3
		a..3		-> 4,4		4,4
		a..3.		-> 4,4		4,4
		b33.33.4	-> 2,6		2,3
	*/
	void setNumPosStart () {
		const char32 *p = alpha + alphaPosStart - 1;
		while (*p != U'\0' && (*p < U'0' || *p > U'9'))
			p ++;
		if (*p != U'\0')
			number.numPosStart = 1 + p - alpha;
		else
			number.numPosStart = 0;
	}
		
	void setNumPosEndAndDot () {
		if (alphaPosStart < 0)
			return;
		const char32 *pstart = alpha + number.numPosStart - 1;
		const char32 *decimalPoint = 0, *p = pstart;
		while (*p != U'\0') {
			if (*p == U'.') {
				if (decimalPoint) { // second occurence: stop
					p --;
					break;
				} else {
					if (breakAtDecimalPoint)
						break;
					else
						decimalPoint = p;
				}
			} else if (*p < U'0' || *p > U'9') {
				break;
			}
			p ++;
		}
		if (p - decimalPoint == 1) // no decimal point at end, belongs to alpha!
			p --;
		number.numPosDot = ( (breakAtDecimalPoint || decimalPoint) ? 0 : number.numPosStart + decimalPoint - pstart);
		const integer relPos = p - pstart - 1;
		number.numPosEnd = number.numPosStart + relPos;
	}
		
	void setNumber () {
		setNumPosStart ();
		if (number.numPosStart > 0) {
			setNumPosEndAndDot ();
			char32 *pstart = alpha + number.numPosStart - 1;
			char32 *p = pstart;
			while (*p == U'0')
				p ++;
			if (p == alpha + number.numPosEnd) // keep one zero if there are only leading zeros!
				p --;
			number.numberOfLeadingZeros = p - pstart;
			p = pstart - 1;
			while (p >= alpha && *p == U' ')
					p --;
			number.numberOfLeadingSpaces = pstart - 1 - p;
			number.numPosStart += number.numberOfLeadingZeros;
		}
	}
		
	bool setAlphaPosStartAfterNumPosEnd () {
		bool notAtEnd = false;
		if (number.numPosStart == 0)
			alphaPosStart = length + 1; // at \0
		else if (number.numPosEnd <= length) {
			alphaPosStart = number.numPosEnd + 1;
			if (alphaPosStart <= length) {
				setNumber ();
				notAtEnd = true;
			}
		}
		return notAtEnd;
	}

public:
	
	integer compare (stringDatum y) {
		char32 *px = alpha + alphaPosStart - 1;
		char32 *py = y -> alpha  + y -> alphaPosStart - 1;
		// ' ' < '1..9' < 
		if (*px == U'\0' && *py != U'\0')
			return - 1;
		else if (*px != U'\0' && *py == U'\0')
			return 1;
		else if (*px == U'\0' && *py == U'\0')
			return 0;
		while (*px == U' ')
			px ++;
		while (*py == U' ')
			py ++;
		bool xIsNum = ( *px >= U'0' && *px <= U'9' );
		bool yIsNum = ( *py >= U'0' && *py <= U'9' );
		if (xIsNum && ! yIsNum)
			return - 1;
		else if (! xIsNum && yIsNum)
			return 1;
		NumDatum xsave = number;
		NumDatum ysave = y -> number;
		integer cmp = ( (xIsNum && yIsNum) ? compareNumPart (y) : compareAlphaPart (y) );
		number = xsave;
		y -> number = ysave;
		alphaPosStart = 1;
		y -> alphaPosStart = 1;
		return cmp;
	}
			
	void init (conststring32 string, bool breakAtTheDecimalPoint) {
		breakAtDecimalPoint = breakAtTheDecimalPoint;
		alphaPosStart = 1;
		length = str32len (string);
		alpha = (char32 *)_Melder_calloc (length + 1, sizeof (char32));
		str32cpy (alpha, string); // adds the U'\0'
		setNumber();
	}
};

struct structSTRVECIndexer {
private:
	autoStringDataVec stringsInfoDatavector;
	bool breakAtDecimalPoint = true;
	
	autoINTVEC strvecIndex; // index of the strvec
	autoSTRVEC strvecClasses;  // the unique items in the STRVEC
	autoPermutation strvecPermutation; // keeps track of the individual item position.
	autoPermutation classesSorting; // keeps track of the sorting of 'strvecClasses'
	
	void init (constSTRVEC const& v,  bool breakAtDecimalPoint) {
		stringsInfoDatavector = newvectorraw <StringDatum> (v.size);
		for (integer i = 1; i <= v.size; i ++) {
			stringsInfoDatavector [i].init (v [i], breakAtDecimalPoint);
		}
	}
		
	/*
		Simplest way to get an index and the string classes, i.e. 
		the unique strings in the vector.
	*/
	void createAlphabeticalIndex (constSTRVEC const& strvec) {
		strvecIndex = raw_INTVEC (strvec.size);
		strvecPermutation = Permutation_create (strvec.size, true);
		INTVECindex_inout (strvecPermutation -> p.get(), strvec);
		integer iclass = 1;
		integer index = strvecPermutation -> p [1];
		strvecClasses.insert (iclass, strvec [index]);
		strvecIndex [index] = iclass;
		for (integer i = 2; i <= strvec.size; i ++) {
			index = strvecPermutation -> p [i];
			if (Melder_cmp (strvecClasses [iclass].get(), strvec [index]) != 0)
				strvecClasses.insert (++ iclass, strvec [index]);
			strvecIndex [index] = iclass;
		}
	}
	
 public:

	autoPermutation sortSimple (constSTRVEC const& strvec, kStrings_sorting sorting, bool breakAtDecimalPoint) {
		Melder_require (strvec.size > 0,
			U"There should be at least one element in your list.");
		autoPermutation p = Permutation_create (strvec.size, true);
		if (sorting == kStrings_sorting::ALPHABETICAL) {
			 INTVECindex_inout (p -> p.get(), strvec);
		} else if (sorting == kStrings_sorting::NATURAL) {
			init (strvec, breakAtDecimalPoint);
			INTVECindex_inout (p -> p.get(), stringsInfoDatavector.get());
		}
		return p;
	}
	
	void sortClasses (constSTRVEC const& strvec, kStrings_sorting sorting, bool breakAtTheDecimalPoint) {
		Melder_require (strvec.size > 0,
			U"There should be at least one element in your list.");
		breakAtDecimalPoint = breakAtTheDecimalPoint;
		createAlphabeticalIndex (strvec);
		if (sorting == kStrings_sorting::ALPHABETICAL) {
			 ;
		} else if (sorting == kStrings_sorting::NATURAL) {
			classesSorting = sortSimple (strvecClasses.get(), sorting, breakAtDecimalPoint); 
			Permutation_permuteSTRVEC_inout (classesSorting.get(), strvecClasses);
			for (integer i = 1; i <= strvec.size; i ++)
				strvecIndex [i] = Permutation_getIndexAtValue (classesSorting.get(), strvecIndex [i]);
		}
	 }
	 
	autoStringsIndex index (constSTRVEC const& strvec, kStrings_sorting sorting, bool breakAtTheDecimalPoint) {
		sortClasses (strvec, sorting, breakAtTheDecimalPoint);
		autoStringsIndex me = StringsIndex_create (strvec.size);
		for (integer i = 1; i <= strvecClasses.size; i ++) {
			conststring32 classi = strvecClasses [i].get();
			autoSimpleString ss = SimpleString_create (classi);
			my classes -> addItem_move (ss.move());
		}
		my classIndex.get()  <<= strvecIndex.get();
		return me;
	 }
};

/*
static void test_index () {
	const integer size = 1000;
	autoPermutation p = Permutation_create (size, true);
	autoSTRVEC s (size);
	char32 word [] { U"abc" };
	for (integer irep = 1; irep <= 100; irep ++) {
		for (integer i = 1; i <= size; i ++) {
			char32 word [] { U"abc" };
			word [0] = char32 (NUMrandomInteger (U'a', U'z'));
			word [1] = char32 (NUMrandomInteger (U'a', U'z'));
			s [i] = Melder_dup (word);
		}
		INTVECindex_inout (p -> p.get(), s.get());
		for (integer i = 2; i <= size; i ++) {
			conststring32 x = s [p -> p [i - 1]].get();
			conststring32 y = s [p -> p [i]].get();
			const integer cmp = Melder_cmp (x, y);
			if (cmp > 0)
				Melder_throw (x, U" > ", y);
		}
	}
}
*/
inline void INTVECindex_inout (INTVEC index, StringDataVec const& v) {
	std::stable_sort (index.begin(), index.end(), [& v] (integer ix, integer iy) 
	{ 
		StringDatum *vx = & v [ix], *vy = & v [iy];
		integer cmp = vx -> compare (vy);
		trace (U"compare: '", vx -> alpha, U"' ", (cmp < 0 ? U"<" : cmp == 0 ? U"==" : U">"), U" '", vy -> alpha, U"' ", ix, U" ", iy);
		return cmp < 0;
	});
}


autoPermutation Permutation_createFromSorting (constSTRVEC const& strvec, kStrings_sorting sorting, bool breakAtDecimalPoint); 

#endif /* _STRVEC_sorting_h_ */
