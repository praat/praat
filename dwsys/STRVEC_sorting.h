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
	
	<alphaPart> sorts alphabetical, <numPart> sorts numerical:
	0a, 1a, 11a, 2a, 11a -> 0a, 1a, 2a, 11a, 11a
	a, 00a, 0a -> 00a, 0a, a (numerical as well as alphabetical order are equal)
	00a1, 00a2, 00a11, 0b0
*/
typedef struct structSTRVECIndexer *STRVECIndexer;
struct structSTRVECIndexer {
	struct structStringClassesInfo {
		private:
		struct structStringClassInfoData {
			mutablestring32 alpha; // either <alphaPart> or <numPart>
			integer length; // strlen (alpha)
			integer alphaPosStart; // start positions of alphaPart; alphaPosEnd is always fixed at the end U'\0'
			integer numPosStart; // start of num part in alpha, else 0 if no num part
			integer numPosDot; // numPosStart < numPosDot < numPosEnd if a dot occurs in the num part and breakAtDecimalPoint==true
			integer numPosEnd; // >= numPosStart if numPosStart > 0 else undefined
			DigitstringNumber number;	// alpha [numPosStart-1..numPosEnd-1] as a number
			char32 save0; // efficiency: instead of copying we mask part of alpha by putting a '\0'
		};
		using STRINFOVEC = vector<struct structStringClassInfoData>;
		using autoSTRINFOVEC = autovector<struct structStringClassInfoData>;
		using constSTRINFOVEC = constvector <struct structStringClassInfoData>;
		typedef struct structStringClassInfoData *stringInfo; 
		
		autoSTRINFOVEC stringsInfoDatavector;
		bool breakAtDecimalPoint = true;
		
		/*
			We use a mask, i.e. a '\0' at positions in the string 'alpha' to mask the
			the <alpaPart> of the <numPart> or the <numPart> of the <alpahPart>.
			The unmask is used to undo the nask.
		*/
		void maskAlphaTrailer (stringInfo info) { //  save the char32 after number part and replace by U'\0'
			if (info -> numPosStart > 0 && info -> numPosEnd < info -> length) {  
				info -> save0 = info -> alpha [info -> numPosEnd]; // save position after last digit
				info -> alpha [info -> numPosEnd] = U'\0'; // mark number part only
			}
		}
	
		void maskAlphaTrailer_undo (stringInfo info) { // undo the masking!
			if (info -> numPosStart > 0 && info -> numPosEnd < info -> length)
				info -> alpha [info -> numPosEnd] = info -> save0; 
		}
		
		void maskNumTrailer (stringInfo info) {
			if (info -> numPosStart > 0) {
				info -> save0 = info -> alpha [info -> numPosStart - 1];
				info -> alpha [info -> numPosStart - 1] = U'\0';
			}
		}
		
		void maskNumTrailer_undo (stringInfo info) {
			if (info -> numPosStart > 0)
				info -> alpha [info -> numPosStart - 1] = info -> save0;
		}
		
		void setNumber (stringInfo info) {
			setNumPosStart (info);
			if (info -> numPosStart > 0) {
				setNumPosEndAndDot (info);
				char32 *pstart = info -> alpha + info -> numPosStart - 1;
				char32 *p = pstart;
				while (*p == U'0')
					p ++;
				if (p == info -> alpha + info -> numPosEnd)
					p --;
				info -> number.numberOfLeadingZeros = p - pstart;
				maskAlphaTrailer (info); // save position after last digit and put '\0'
				info -> number.value = Melder_atof (p);
				maskAlphaTrailer_undo (info); //
			}
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
		void setNumPosStart (stringInfo info) {
			const char32 *p = info -> alpha + info -> alphaPosStart - 1;
			while (*p != U'\0' && (*p < U'0' || *p > U'9'))
				p ++;
			if (*p != U'\0')
				info -> numPosStart = 1 + p - info -> alpha;
			else
				info -> numPosStart = 0;
		}
		
		void setNumPosEndAndDot (stringInfo info) {
			if (info -> alphaPosStart < 0)
				return;
			const char32 *pstart = info -> alpha + info -> numPosStart - 1;
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
			info -> numPosDot = ( (breakAtDecimalPoint || decimalPoint) ? 0 : info -> numPosStart + decimalPoint - pstart);
			const integer relPos = p - pstart - 1;
			info -> numPosEnd = info -> numPosStart + relPos;
		}
		
		inline stringInfo getStringInfo (integer index) {
			Melder_assert (index > 0 && index <= stringsInfoDatavector.size);
			return & stringsInfoDatavector [index];
		}
				
		public:
			
		void init (constSTRVEC const& strings) { // strings are unicized!
			stringsInfoDatavector = newvectorraw <struct structStringClassInfoData> (strings.size);
			for (integer i = 1; i <= strings.size; i++) {
				stringInfo info  = getStringInfo (i);
				info -> alphaPosStart = 1;
				info -> length = str32len (strings [i]);
				info -> alpha = (char32 *)_Melder_calloc (info -> length + 1, sizeof (char32));
				str32cpy (info -> alpha, strings [i]); // adds the U'\0'
				info -> number.value = -1.0; 
			}
		}
		
		integer setAlphaPosStartAfterNumPosEnd (constINTVEC set) {
			integer notAtEnd = 0;
			for (integer i = 1; i <= set.size; i ++) {
				stringInfo info = getStringInfo (set [i]);
				if (info -> numPosStart == 0)
					info -> alphaPosStart = info -> length + 1; // at \0
				else if (info -> numPosEnd <= info -> length) {
					info -> alphaPosStart = info -> numPosEnd + 1;
					if (info -> alphaPosStart <= info -> length) {
						setNumber (info);
						notAtEnd ++;
					}
				}
			}
			return notAtEnd;
		}
		
		integer setAlphaPosStartAtNumPosStart (constINTVEC set) {
			integer notAtEnd = 0;
			for (integer i = 1; i <= set.size; i ++) {
				stringInfo info = getStringInfo (set [i]);
				if (info -> numPosStart == 0)
					info -> alphaPosStart = info -> length + 1; // at \0
				else if (info -> alphaPosStart <= info -> numPosStart) {
					info -> alphaPosStart = info -> numPosStart;
					notAtEnd ++;
				}
			}
			return notAtEnd;
		}
		
		void updateNumPart (constINTVEC set) {
			for (integer i = 1; i <= set.size; i++) {
				stringInfo info = getStringInfo (set [i]);
				setNumber (info);
			}
		}
			
		bool separateAlphaAndNumSets (constINTVEC const& subset, autoINTVEC & numStartSet, autoINTVEC & alphaStartSet) {
			integer num = 0, alpha = 0, done = 0;
			for (integer i = 1; i <= subset.size; i ++) {
				integer irow = subset [i];
				stringInfo info = getStringInfo (irow);
				if (info -> numPosStart > 0 && info -> numPosStart == info -> alphaPosStart)
						numStartSet.insert (++ num, irow);
				else {
					if (info -> alphaPosStart > 0)
						alphaStartSet.insert (++ alpha, irow);
					else
						done ++;
				}
			}
			numStartSet.resize (num);
			alphaStartSet.resize (alpha);
			return done != subset.size;
		}
		
		autoDigitstringNumberVEC extractNums (constINTVEC const& subset) {
			autoDigitstringNumberVEC numbers = newvectorraw<DigitstringNumber> (subset.size);
			for (integer i = 1; i <= subset.size; i ++) {
				const integer irow = subset [i];
				stringInfo info = getStringInfo (irow);
				numbers [i] = info -> number;
			}
			return numbers;
		}
		
		autoSTRVEC extractAlphas (constINTVEC const& subset) {
			autoSTRVEC alphas (subset.size);
			for (integer i = 1; i <= subset.size; i ++) {
				const integer irow = subset [i];
				stringInfo info = getStringInfo (irow);
				maskNumTrailer (info);
				alphas [i] = Melder_dup (info -> alpha + info -> alphaPosStart - 1);
				maskNumTrailer_undo (info);
			}
			return alphas;
		}
		
		autoSTRVEC extractAlphaPart (constINTVEC const& subset) {
			autoSTRVEC alphaPart (subset.size);
			for (integer i = 1; i <= subset.size; i ++) {
				const integer irow = subset [i];
				stringInfo info = getStringInfo (irow);
				alphaPart [i] = Melder_dup (info -> alpha + info -> alphaPosStart - 1);
			}
			return alphaPart;
		}
		
		inline integer getNumstringLength (integer index) {
			stringInfo info = getStringInfo (index);
			return info -> numPosEnd - info -> numPosStart + 1;
		}
		
		autostring32 extractNumAsString (integer index) {
				stringInfo info = getStringInfo (index);
				maskAlphaTrailer (info);
				autostring32 result = Melder_dup (info -> alpha + info -> alphaPosStart - 1);
				maskAlphaTrailer_undo (info);
				return result;
		}
		
		autoINTVEC getNumstringLengths (constINTVEC const& subset) {
			autoINTVEC lengths = raw_INTVEC (subset.size);
			for (integer i = 1; i <= subset.size; i ++)
				lengths [i] = getNumstringLength (subset [i]);
			return lengths;
		}
	};
	
private:
	autoINTVEC strvecIndex; // index of the strvec
	autoSTRVEC strvecClasses;  // the unique items in the STRVEC
	autoPermutation strvecPermutation; // keeps track of the individual item position.
	autoPermutation classesSorting; // keeps track of the sorting of 'strvecClasses'
	autoINTVEC classChangePositios; // where does the class change in the created index at the start
	struct structStringClassesInfo stringsInfo;
	bool breakAtDecimalPoint;
	
	void createIndex (constSTRVEC const& v) {
		integer count = 1;
		strvecIndex = raw_INTVEC (v.size);
		strvecPermutation = Permutation_create (v.size, true);
		INTVECindex_inout (strvecPermutation -> p.get(), v);
		strvecClasses.insert (count, v [strvecPermutation -> p [1]]);
		strvecIndex [1] = count;
		classChangePositios.insert (count, 1);
		for (integer i = 2; i <= v.size; i ++) {
			const integer index = strvecPermutation -> p [i];
			conststring32 str = v [index];
			if (Melder_cmp (strvecClasses [count].get(), str) != 0) {
				strvecClasses.insert (++ count, str);
				classChangePositios.insert (count, i);
			}
			strvecIndex [i] = count;
		}
	}
	
	// the elements of set sets must always refer to the position in the structSTRVECIndex::classes
	autoPermutation sortAlphaPartSet (constINTVEC const& set, integer level) {
		Melder_assert (set.size > 0);
		autoPermutation pset = Permutation_create (set.size, true);
		autoSTRVEC alphas = stringsInfo.extractAlphas (set);
		if (level > 1) // already sorted by init
			INTVECindex_inout (pset -> p.get(), alphas.get()); // we want s01 before s1!
		
		integer startOfEquals = 1;
		conststring32 value = alphas [pset -> p [1]].get(); // the smallest 
		for (integer i = 2; i <= set.size; i ++) {
			conststring32 current = alphas [pset -> p [i]].get();
			const bool valueChange = Melder_cmp (current, value) != 0;
			if (valueChange || i == set.size) {
				const integer numberOfEquals = ( valueChange ? i - startOfEquals : i - startOfEquals + 1 );
				if (numberOfEquals > 1) {
					autoINTVEC equalsSet_local = raw_INTVEC (numberOfEquals);
					autoINTVEC equalsSet_global = raw_INTVEC (numberOfEquals);
					/*for (integer j = 1; j <= numberOfEquals; j ++) {
						const integer index_local = pset -> p [startOfEquals + j - 1];
						equalsSet_local [j] = index_local;
						equalsSet_global [j] = set [index_local];
					}*/
					for (integer j = 1; j <= numberOfEquals; j ++) {
						const integer index_local = startOfEquals + j - 1;
						equalsSet_local [j] = index_local;
						equalsSet_global [j] = set [pset -> p [index_local]];
					}
					if (stringsInfo.setAlphaPosStartAtNumPosStart (equalsSet_global.get())) {
						autoPermutation pequals = sortNumPartSet (equalsSet_global.get(), level);
						Permutation_permuteSubsetByOther_inout (pset.get(), equalsSet_local.get(), pequals.get());
					}
				}
				value = current;
				startOfEquals = i;
			}
		}
		return pset;
	}
	
	/*
		The elements of the set to be sorted must always refer to the position in structSTRVECIndex::strvecClasses
		Precondition: alphabetical sorting before sortNumPartSet is called.
		strings {"4b","04b","4a","004a","1","d","c","004b" } were already sorted as {"004a","004b","04b","1","4a","4b","c","d"},
		we want to sort them as {"1", "004a","004b","4a","04b","4b","c","d"}
		if the strings before the alpha reduce to the same number, they are already in the correct order.
			("004a", "4a", "04b", "4b")
		strings <num><alpha><num> {"4b1","4b01"} 
	*/
	autoPermutation sortNumPartSet (constINTVEC const& set, integer level) {
		/*
			Convert numeric part to numbers
		*/
		const integer numberOfStrings = set.size;
		autoDigitstringNumberVEC numbers = stringsInfo.extractNums (set);
		autoPermutation	pset = Permutation_create (numberOfStrings, true);
		INTVECindex_inout (pset -> p.get(), numbers.get());

		integer startOfEquals = 1;
		DigitstringNumber value = numbers [pset -> p [1]]; // the smallest DigitstringNumber
		for (integer i = 2; i <= numberOfStrings; i++) {
			const DigitstringNumber current = numbers [pset -> p [i]];
			const bool valueChange = ( current.compare (value) != 0);
			if (valueChange || i == set.size) {
				const integer numberOfEquals = ( valueChange ? i - startOfEquals : i - startOfEquals + 1 );
				if (numberOfEquals > 1) {
					autoINTVEC equalsSet_local = raw_INTVEC (numberOfEquals);
					autoINTVEC equalsSet_global = raw_INTVEC (numberOfEquals);
					for (integer j = 1; j <= numberOfEquals; j ++) {
						const integer index_local = startOfEquals + j - 1;
						equalsSet_local [j] = index_local;
						equalsSet_global [j] = set [pset -> p [index_local]];
					}
					if (stringsInfo.setAlphaPosStartAfterNumPosEnd (equalsSet_global.get())) {
						autoPermutation pequals = sortAlphaPartSet (equalsSet_global.get(), level + 1);
						Permutation_permuteSubsetByOther_inout (pset.get(), equalsSet_local.get(), pequals.get());
					}
				}
				value = current;
				startOfEquals = i;
			}
		}
		return pset;
	}
	
 public:
	autoPermutation sortSimple (constSTRVEC const& v,  bool breakAtTheDecimalPoint, kStrings_sorting sortingMethod) {
		Melder_require (v.size > 0,
			U"There should be at least one element in your list.");
		autoPermutation sorting = Permutation_create (v.size, true);
		if (sortingMethod == kStrings_sorting::ALPHABETICAL) {
			 INTVECindex_inout (sorting -> p.get(), v);
		} else if (sortingMethod == kStrings_sorting::NUMERICAL_PART) {
			breakAtDecimalPoint = breakAtTheDecimalPoint;
			stringsInfo.init (v);
			stringsInfo.updateNumPart (sorting -> p.get());
			autoINTVEC numStartSet, alphaStartSet;
			Melder_require (stringsInfo.separateAlphaAndNumSets (sorting -> p.get(), numStartSet, alphaStartSet),
				U"error");
			Melder_assert (numStartSet.size + alphaStartSet.size == v.size);
			if (numStartSet.size > 0) {
				autoPermutation pnumPart = sortNumPartSet (numStartSet.get(), 1_integer);
				Permutation_permuteINTVEC_inout (pnumPart.get(), numStartSet.get());
				if (alphaStartSet.size == 0)
					sorting -> p.get()  <<=  numStartSet.get();
			}
			if (alphaStartSet.size > 0) {
				autoPermutation palphaPart = sortAlphaPartSet (alphaStartSet.get(), 1);
				Permutation_permuteINTVEC_inout (palphaPart.get(), alphaStartSet.get());
				if (numStartSet.size == 0)
					sorting -> p.get()  <<=  alphaStartSet.get();
			}
			if (numStartSet.size > 0 && alphaStartSet.size > 0) {
				sorting -> p.part (1, numStartSet.size)  <<=  numStartSet.get();
				sorting -> p.part (numStartSet.size + 1, sorting -> numberOfElements)  <<=  alphaStartSet.get();
				Permutation_checkInvariant (sorting.get());
			}
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
		} else if (sorting == kStrings_sorting::NUMERICAL_PART) {
			stringsInfo.init (strvecClasses.get());
			classesSorting = Permutation_create (strvecClasses.size, true);
			stringsInfo.updateNumPart (classesSorting -> p.get());
			autoINTVEC numStartSet, alphaStartSet;
			Melder_require (stringsInfo.separateAlphaAndNumSets (classesSorting -> p.get(), numStartSet, alphaStartSet),
				U"error");
			Melder_assert (numStartSet.size + alphaStartSet.size == strvecClasses.size);
			if (numStartSet.size > 0) {
				autoPermutation pnumPart = sortNumPartSet (numStartSet.get(), 1);
				Permutation_permuteINTVEC_inout (pnumPart.get(), numStartSet.get());
				if (alphaStartSet.size == 0)
					classesSorting -> p.get()  <<=  numStartSet.get();
			}
			if (alphaStartSet.size > 0) {
				autoPermutation palphaPart = sortAlphaPartSet (alphaStartSet.get(), 1);
				Permutation_permuteINTVEC_inout (palphaPart.get(), alphaStartSet.get());
				if (numStartSet.size == 0)
					classesSorting -> p.get()  <<=  alphaStartSet.get();
			}
			if (numStartSet.size > 0 && alphaStartSet.size > 0) {
				classesSorting -> p.part (1, numStartSet.size)  <<=  numStartSet.get();
				classesSorting -> p.part (numStartSet.size + 1, classesSorting -> numberOfElements)  <<=  alphaStartSet.get();
				Permutation_checkInvariant (classesSorting.get());
			}
			Permutation_permuteSTRVEC_inout (classesSorting.get(), strvecClasses);
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

autoPermutation Permutation_createFromSorting (constSTRVEC const& strvec, kStrings_sorting sorting, bool breakAtDecimalPoint); 

#endif /* _STRVEC_sorting_h_ */
