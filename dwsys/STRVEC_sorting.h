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
			'5' < ' 5' < ' 5 ' < '05' < '005'
		3. Non-numbers sort in the asciibetical order ('Z' before 'a').
			'A' < 'B' ... < "Z' < 'a' ... 'z' 
	
	<alphaPart> sorts alphabetical, <numPart> sorts numerical:
	0a, 1a, 11a, 2a, 11a -> 0a, 1a, 2a, 11a, 11a
	a, 00a, 0a -> 0a, 00a, a, 
	00a1, 00a2, 00a11, 0b0
*/

class DigitstringNumber {
public:	
	integer numberOfLeadingZeros;
	integer numberOfLeadingSpaces;
	double value;

	integer compareLeadings (DigitstringNumber const& y) const {
		return (  
			numberOfLeadingZeros < y.numberOfLeadingZeros ? - 1 : numberOfLeadingZeros > y.numberOfLeadingZeros ? 1 :
			numberOfLeadingSpaces < y.numberOfLeadingSpaces ? - 1 : numberOfLeadingSpaces > y.numberOfLeadingSpaces ? 1 :
			0 );
	}
	integer compareValue (DigitstringNumber const& y) const {
		return ( 
			value < y.value ? - 1 : value > y.value ? 1 : 0 
		);
	}
};

typedef struct structSTRVECIndexer *STRVECIndexer;

typedef struct structStringDatum *stringInfo;
typedef struct structStringDatum StringDatum;

using StringInfoVec = vector<struct structStringDatum>;
using constStringInfoVec = constvector <struct structStringDatum>;
using autoStringInfoVec = autovector <struct structStringDatum>;

inline void INTVECindex_inout (INTVEC index, StringInfoVec const& v);

struct structStringDatum {
	mutablestring32 alpha; // either <alphaPart> or <numPart>
	integer length; // strlen (alpha)
	integer alphaPosStart; // start positions of alphaPart; alphaPosEnd is always fixed at the end U'\0'
	integer numPosStart; // start of num part in alpha, else 0 if no num part
	integer numPosDot; // numPosStart < numPosDot < numPosEnd if (breakAtDecimalPoint==true and there is a dot)
	integer numPosEnd; // >= numPosStart if numPosStart > 0 else undefined
	DigitstringNumber number;	// alpha [numPosStart-1..numPosEnd-1] as a number
	char32 save0; // efficiency: instead of copying we mask part of alpha by putting a '\0'
	bool breakAtDecimalPoint;
	
	bool compareNumber (stringInfo y) {
		return number.value < y -> number.value;
	}
	
	integer compareNumPart (stringInfo y) {
		const integer xlength = ( breakAtDecimalPoint ? numPosEnd - numPosStart : numPosEnd - numPosDot );
		const integer ylength = ( y -> breakAtDecimalPoint ? y -> numPosEnd - y -> numPosStart : y -> numPosEnd - y -> numPosDot );
		if (xlength < ylength)
			return - 1;
		else if (xlength > ylength)
			return 1;
		/*
			the digitstrings have equal lengths
			we could simply convert both to a number and compare the size
			or do a character by character <num> comparison
			*/
		char32 *p = alpha + numPosStart - 1;
		char32 *q = y -> alpha + y -> numPosStart - 1;
		for ( ; p <= alpha + xlength; p ++, q ++) {
			if (*p < *q)
				return - 1;
			else if (*p > *q)
				return 1;
		}
		if (! breakAtDecimalPoint && y -> breakAtDecimalPoint)
			return -1;
		else if (breakAtDecimalPoint && ! y -> breakAtDecimalPoint)
			return 1;
		for ( ; p < alpha + numPosEnd || q < y -> alpha + y -> numPosEnd; p ++, q ++) {
			if (*p < *q)
				return - 1;
			else if (*p > *q)
				return 1;
		}
		/*
			both numbers are equal
			compare the alphaPart
		*/
		bool xatend = setAlphaPosStartAfterNumPosEnd ();
		bool yatend = y -> setAlphaPosStartAfterNumPosEnd ();
		if (xatend && ! yatend)
			return - 1;
		else if (! xatend && yatend)
			return 1;
		else if (xatend && yatend)
			return 0;
		// both are not at the end 
		integer alpaPart =  compareAlphaPart (y);
		if (alpaPart < 0)
			return -1;
		else if (alpaPart > 0)
			return 1;
		/*
			<alphaPart>'s are also equal,
			let the leading zeros, spaces decide
		*/
		return number.compareLeadings (y -> number);
	}
	
	integer compareAlphaPart (stringInfo y) {
		const char32 xalpha = alpha [alphaPosStart - 1];
		const char32 yalpha = y -> alpha [y -> alphaPosStart - 1];
		if (xalpha <  yalpha)
			return -1;
		else if (xalpha  > yalpha)
			return 1;
		/*
			compare the <alphas> of the <alphaPart>
		*/
		setNumPosStart ();
		maskNumTrailer ();
		y -> setNumPosStart ();
		y -> maskNumTrailer ();
		char32 *p = alpha + alphaPosStart - 1;
		char32 *q = y -> alpha + y -> alphaPosStart - 1;
		integer cmp = str32cmp (p, q);
		if (cmp < 0)
			return -1;
		else if (cmp > 0)
			return 1;
		maskNumTrailer_undo ();
		y -> maskNumTrailer_undo ();
		// <alpha>'s are equal
		if (numPosStart > 0 && ! (y -> numPosStart > 0))
			return - 1;
		else if (! (numPosStart > 0) && y -> numPosStart > 0)
			return 1;
		setNumPosEndAndDot ();
		y -> setNumPosEndAndDot ();
		return compareNumPart (y);
	}
	
	integer compare (stringInfo y) {
		const char32 xalpha = alpha [alphaPosStart - 1];
		const char32 yalpha = y -> alpha [y -> alphaPosStart - 1];
		// ' ' < '1..9' < 
		const integer comparison = ( xalpha < yalpha ? - 1 : xalpha > yalpha ? 1 : xalpha == U'\0' ? 0 : 
			(xalpha >= U'0' && xalpha <= U'9') ? compareNumPart (y) : compareAlphaPart (y));
		return comparison;
	}
	
	/*
		We use a mask, i.e. a '\0' at positions in the string 'alpha' to mask the
		the <alpaPart> of the <numPart> or the <numPart> of the <alpahPart>.
		The unmask is used to undo the nask.
	*/
	
	bool compareAlpha  (stringInfo y) {
		maskNumTrailer ();
		y -> maskNumTrailer();
		bool result = Melder_cmp (alpha + alphaPosStart - 1, y -> alpha + y->alphaPosStart - 1) < 0;
		y -> maskNumTrailer_undo ();
		maskNumTrailer_undo ();
		return result;
	}
	
	void maskNumTrailer () {
		if (numPosStart > 0) {
			save0 = alpha [numPosStart - 1 ];
			alpha [numPosStart - 1] = U'\0';
		}
	}
		
	void maskNumTrailer_undo () {
		if (numPosStart > 0)
			alpha [numPosStart - 1] = save0;
	}
	void maskAlphaTrailer () { //  save the char32 after number part and replace by U'\0'
		if (numPosStart > 0 && numPosEnd < length) {  
			save0 = alpha [numPosEnd]; // save position after last digit
			alpha [numPosEnd] = U'\0'; // mark number part only
		}
	}

	void maskAlphaTrailer_undo () { // undo the masking!
		if (numPosStart > 0 && numPosEnd < length)
			alpha [numPosEnd] = save0; 
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
			numPosStart = 1 + p - alpha;
		else
			numPosStart = 0;
	}
		
	void setNumPosEndAndDot () {
		if (alphaPosStart < 0)
			return;
		const char32 *pstart = alpha + numPosStart - 1;
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
		if (p - decimalPoint == 1) // no decimal point at end!
			p --;
		numPosDot = ( (breakAtDecimalPoint || decimalPoint) ? 0 : numPosStart + decimalPoint - pstart);
		const integer relPos = p - pstart - 1;
		numPosEnd = numPosStart + relPos;
	}
		
	void setNumber () {
		setNumPosStart ();
		if (numPosStart > 0) {
			setNumPosEndAndDot ();
			char32 *pstart = alpha + numPosStart - 1;
			char32 *p = pstart;
			while (*p == U'0')
				p ++;
			if (p == alpha + numPosEnd)
				p --;
			number.numberOfLeadingZeros = p - pstart;
			maskAlphaTrailer (); // save position after last digit and put '\0'
			number.value = Melder_atof (p);
			maskAlphaTrailer_undo (); //
			p = pstart - 1;
			while (p >= alpha && *p == U' ')
					p --;
			number.numberOfLeadingSpaces = pstart - 1 - p;
			numPosStart -= number.numberOfLeadingSpaces;
		}
	}
		
	bool setAlphaPosStartAfterNumPosEnd () {
		bool notAtEnd = false;
		if (numPosStart == 0)
			alphaPosStart = length + 1; // at \0
		else if (numPosEnd <= length) {
			alphaPosStart = numPosEnd + 1;
			if (alphaPosStart <= length) {
				setNumber ();
				notAtEnd = true;
			}
		}
		return notAtEnd;
	}
	
	bool setAlphaPosStartAtNumPosStart () {
		bool notAtEnd = false;
		if (numPosStart == 0)
			alphaPosStart = length + 1; // at \0
		else if (alphaPosStart <= numPosStart) {
			alphaPosStart = numPosStart + number.numberOfLeadingSpaces;
			notAtEnd = true;
		}
		return notAtEnd;
	}
	
	bool isAlphaPart () {
		return alphaPosStart > 0 && numPosStart != alphaPosStart;
	}
	
	bool isNumPart () {
		return numPosStart > 0 && numPosStart == alphaPosStart;
	}
	
public:
			
	void init (conststring32 string, bool breakAtTheDecimalPoint) {
		breakAtDecimalPoint = breakAtTheDecimalPoint;
		alphaPosStart = 1;
		length = str32len (string);
		alpha = (char32 *)_Melder_calloc (length + 1, sizeof (char32));
		str32cpy (alpha, string); // adds the U'\0'
		number.value = 0; 
	}
};

struct structSTRVECIndexer {
private:
//	using StringInfoVec = vector<struct structStringDatum>;
//	using constStringInfoVec = constvector <struct structStringDatum>;
//	using autoStringInfoVec = autovector <struct structStringDatum>;
		
	autoStringInfoVec stringsInfoDatavector;
	bool breakAtDecimalPoint = true;
	
	autoINTVEC strvecIndex; // index of the strvec
	autoSTRVEC strvecClasses;  // the unique items in the STRVEC
	autoPermutation strvecPermutation; // keeps track of the individual item position.
	autoPermutation classesSorting; // keeps track of the sorting of 'strvecClasses'
	
	void createIndex (constSTRVEC const& v) {
		integer count = 1;
		strvecIndex = raw_INTVEC (v.size);
		strvecPermutation = Permutation_create (v.size, true);
		INTVECindex_inout (strvecPermutation -> p.get(), v);
		integer index = strvecPermutation -> p [1];
		strvecClasses.insert (count, v [index]);
		strvecIndex [index] = count;
		for (integer i = 2; i <= v.size; i ++) {
			index = strvecPermutation -> p [i];
			if (Melder_cmp (strvecClasses [count].get(), v [index]) != 0)
				strvecClasses.insert (++ count, v [index]);
			strvecIndex [index] = count;
		}
	}
	
	void init (constSTRVEC const& v,  bool breakAtDecimalPoint) {
		stringsInfoDatavector = newvectorraw <struct structStringDatum> (v.size);
		for (integer i = 1; i <= v.size; i ++) {
			stringsInfoDatavector [i].init (v [i], breakAtDecimalPoint);
		}
	}
	
 public:
	 


	autoPermutation sortSimple (constSTRVEC const& v,  bool breakAtDecimalPoint, kStrings_sorting sortingMethod) {
		Melder_require (v.size > 0,
			U"There should be at least one element in your list.");
		autoPermutation sorting = Permutation_create (v.size, true);
		if (sortingMethod == kStrings_sorting::ALPHABETICAL) {
			 INTVECindex_inout (sorting -> p.get(), v);
		} else if (sortingMethod == kStrings_sorting::NATURAL) {
			init (v, breakAtDecimalPoint);
			INTVECindex_inout (sorting -> p.get(), stringsInfoDatavector.get());
		}
		return sorting;
	}
	
	void sortWithIndex (constSTRVEC const& v,  bool breakAtTheDecimalPoint, kStrings_sorting sorting) {
		Melder_require (v.size > 0,
			U"There should be at least one element in your list.");
		breakAtDecimalPoint = breakAtTheDecimalPoint;
		createIndex (v);
		if (sorting == kStrings_sorting::ALPHABETICAL) {
			 ;
		} else if (sorting == kStrings_sorting::NATURAL) {
			classesSorting = Permutation_create (strvecClasses.size, true);
			autoPermutation  p = sortSimple (strvecClasses.get(),  breakAtDecimalPoint, sorting); 

			Permutation_permuteSTRVEC_inout (classesSorting.get(), strvecClasses);
			for (integer i = 1; i <= v.size; i ++)
				strvecIndex [i] = classesSorting -> p [strvecIndex [i]];
		}
	 }
	 
	autoStringsIndex index (constSTRVEC const& v,  bool breakAtTheDecimalPoint, kStrings_sorting sorting) {
		sortWithIndex (v, breakAtTheDecimalPoint, sorting);
		autoStringsIndex me = StringsIndex_create (v.size);
		for (integer i = 1; i <= strvecClasses.size; i ++) {
			conststring32 classi = strvecClasses [i].get();
			autoSimpleString ss = SimpleString_create (classi);
			my classes -> addItem_move (ss.move());
		}
		my classIndex.get()  <<= strvecIndex.get();
		return me;
	 }
};


template <class T, typename Tt>
void INTVECindex3_inout (INTVEC & index, T v, bool (*compare) (Tt x, Tt y)) {
	Melder_assert (v.size == index.size);
	to_INTVEC_out (index);
	if (v.size < 2)
		return;   /* Already sorted. */
	if (v.size == 2) {
		if (compare (& v [2], & v [1])) {
			index [1] = 2;
			index [2] = 1;
		}
		return;
	}
	
	if (v.size <= 12) {
		for (integer i = 1; i < v.size; i ++) {
			integer imin = i;
			Tt min =  & v [index [imin]];
			for (integer j = i + 1; j <= v.size; j ++) {
				if (compare (& v [index [j]], min)) {
					imin = j;
					min = & v [index [j]];
				}
			}
			std::swap (index [imin], index [i]);
		}
		return;
	}
	/* H1 */
	integer l = v.size / 2 + 1;
	integer r = v.size;
	for (;;) { /* H2 */
		integer k;
		if (l > 1) {
			l --;
			k = index [l];
		} else { /* l == 1 */
			k = index [r];
			index [r] = index [1];
			r --;
			if (r == 1) {
				index [1] = k;
				break;
			}
		}
		/* H3 */
		integer i, j = l;
		for (;;) {
			/* H4 */
			i = j;
			j *= 2;
			if (j > r)
				break;
			if (j < r && compare (& v [index [j]], & v [index [j + 1]]))
				j ++; /* H5 */
			index [i] = index [j]; /* H7 */
		}
		for (;;) {  /*H8' */
			j = i;
			i = j >> 1;
			/* H9' */
			if (j == l || compare (& v [k],& v [index [i]])) {
				index [j] = k;
				break;
			}
			index [j] = index [i];
		}
	}
}

inline void INTVECindex_inout (INTVEC index, StringInfoVec const& v) {
	INTVECindex3_inout <StringInfoVec, stringInfo> (index, v, [](stringInfo x, stringInfo y) -> bool 
	{ return x -> compare (y) < 0;});
}

autoPermutation Permutation_createFromSorting (constSTRVEC const& strvec, kStrings_sorting sorting, bool breakAtDecimalPoint); 

#endif /* _STRVEC_sorting_h_ */
