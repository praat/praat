/* manual_Permutation.cpp
 *
 * Copyright (C) 2005-2019 David Weenink
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

/*
  20050709 djmw
*/

#include "ManPagesM.h"

void manual_Permutation_init (ManPages me);
void manual_Permutation_init (ManPages me)
{

MAN_BEGIN (U"Permutation", U"djmw", 20050721)
INTRO (U"One of the @@types of objects@ in Praat. A Permutation object with %n elements consists of some ordering of "
	"the numbers 1,2...%n.")
ENTRY (U"Interpretation")
NORMAL (U"A permutation like for example (2,3,5,4,1) is an %arrangement of the five objects 1, 2, 3, 4, and 5. "
	"It tells us that the second object is in the first position, the third object is in the second position, "
	"the fifth object in the third position and so on.")
NORMAL (U"If we combine a Permutation together with an other object, like a Strings for example, we may force a  "
	"new arrangement of the strings, according to the specification in the Permutation (see @@Strings & Permutation: Permute strings@)." )
ENTRY (U"Commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@Create Permutation...@")
NORMAL (U"Query:")
LIST_ITEM (U"\\bu ##Get number of elements#")
LIST_ITEM (U"\\bu @@Permutation: Get value...|Get value...@")
LIST_ITEM (U"\\bu @@Permutation: Get index...|Get index...@")
NORMAL (U"Modification:")
LIST_ITEM (U"\\bu @@Permutation: Sort|Sort@")
LIST_ITEM (U"\\bu @@Permutation: Swap blocks...|Swap blocks...@")
LIST_ITEM (U"\\bu @@Permutation: Swap positions...|Swap positions...@")
LIST_ITEM (U"\\bu @@Permutation: Swap numbers...|Swap numbers...@")
LIST_ITEM (U"\\bu @@Permutation: Swap one from range...|Swap one from range...@")
NORMAL (U"Permutations:")
LIST_ITEM (U"\\bu @@Permutation: Permute randomly...|Permute randomly...@")
LIST_ITEM (U"\\bu @@Permutation: Permute randomly (blocks)...|Permute randomly (blocks)...@")
LIST_ITEM (U"\\bu @@Permutation: Interleave...|Interleave...@")
LIST_ITEM (U"\\bu @@Permutation: Rotate...|Rotate...@")
LIST_ITEM (U"\\bu @@Permutation: Reverse...|Reverse...@")
LIST_ITEM (U"\\bu @@Permutation: Invert|Invert@")
NORMAL (U"Successive permutations:")
LIST_ITEM (U"\\bu @@Permutations: Multiply|Multiply@")
ENTRY (U"Usage")
LIST_ITEM (U"@@Strings & Permutation: Permute strings@ to rearrange the strings in a Strings object.")
LIST_ITEM (U"@@TableOfReal & Permutation: Permute rows@ to rearrange the rows in a TableOfReal object.")
MAN_END

MAN_BEGIN (U"Create Permutation...", U"djmw", 20050709)
INTRO (U"A command to create a @Permutation of the numbers 1,2, ..., %numberOfElements.")
ENTRY (U"Settings")
TAG (U"##Name")
DEFINITION (U"the name of the new permutation.")
TAG (U"##Number of elements%")
DEFINITION (U"the number of elements in the permutation.")
TAG (U"##Identity permutation")
DEFINITION (U"determines whether the permution will be a randomly chosen one, or the @@identity permutation@.")
MAN_END

MAN_BEGIN (U"identity permutation", U"djmw", 20050713)
INTRO (U"The identity permutation is (1,2,3,...,%numberOfElements), i.e. the numbers 1 to "
	"%numberOfElements in their natural order. ")
MAN_END

MAN_BEGIN (U"Permutation: Get value...", U"djmw", 20050709)
INTRO (U"Get the value at the index position.")
ENTRY (U"Example")
NORMAL (U"The query for the value at index 3 for the permutation (3,2,4,5,1) gives 4.")
MAN_END

MAN_BEGIN (U"Permutation: Get index...", U"djmw", 20050714)
INTRO (U"Get the index position of the value. ")
ENTRY (U"Example")
NORMAL (U"The query for the index of value 3 for the permutation (3,2,4,5,1) gives 1.")
MAN_END

MAN_BEGIN (U"Permutation: Table jump...", U"djmw", 20191211)
INTRO (U"Reorder by jumping columnwise to next row over the permutation indices layed out like a table with "
	"%%jumpSize% columns. ")
ENTRY (U"Settings")
TAG (U"##Jump size#")
DEFINITION (U"defines how many indices to jump over, i.e. the number of columns in the \"table\".")
TAG (U"##First#")
DEFINITION (U"The first new position.")
ENTRY (U"Example 1")
NORMAL (U"Consider the identity permutation of 14 elements layed out like a table with %%jumpSize% = 4 columns.")
CODE (U" 1  2  3  4")
CODE (U" 5  6  7  8")
CODE (U" 9 10 11 12")
CODE (U"13 14")
NORMAL (U"1. ##Table jump: 4, 1# will result in the permutation (1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 4, 8, 12)")
NORMAL (U"2. ##Table jump: 4, 2# will result in the permutation (2, 6, 10, 14, 3, 7, 11, 4, 8, 12, 1, 5. 9, 13)")
NORMAL (U"3. ##Table jump: 4, 5# will result in the permutation (5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 4, 8, 12, 1)")
NORMAL (U"4. ##Table jump: 4, 12# will result in the permutation (12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 4, 8)")
ENTRY (U"Example 2")
NORMAL (U"Consider the identity permutation of 14 elements layed out like a table with %%jumpSize% = 7 columns.")
CODE (U" 1  2  3  4  5  6  7")
CODE (U" 8  9 10 11 12 13 14")
NORMAL (U"1. ##Table jump: 7, 1# will result in the permutation (1, 8, 2, 9, 3, 10, 4, 11, 5, 12, 6, 13, 7, 14)")
MAN_END

MAN_BEGIN (U"Permutation: Reverse...", U"djmw", 20110105)
INTRO (U"Reverse the elements in the given range.")
ENTRY (U"Setting")
TAG (U"##Index range#")
DEFINITION (U"defines the range of indices that will be reversed.")
ENTRY (U"Examples")
NORMAL (U"1. With ##Index range# = [0,0], the permutation (1,2,3,4,5) is turned into (5,4,3,2,1). ")
NORMAL (U"2. With ##Index range# = [3,0], the permutation (1,2,3,4,5) is turned into (1,2,5,4,3). ")
MAN_END

MAN_BEGIN (U"Permutation: Swap one from range...", U"djmw", 20110105)
INTRO (U"An element at an index, randomly chosen from a range, will be permuted with an element at a prescribed index position.")
ENTRY (U"Settings")
TAG (U"##Index range#")
DEFINITION (U"defines the range of indices from which one will be randomly chosen.")
TAG (U"##Index#")
DEFINITION (U"defines the special index position whose element will be interchanged with the one chosen from the range.")
TAG (U"##Forbid same")
DEFINITION (U"when %on, forbids the randomly chosen position and the index position to be the same. "
	"This switch is only of relevance when the chosen range happens to overlap the index position.")
ENTRY (U"Examples")
NORMAL (U"With ##Index range# = [0,0], ##Index# = 3, ##Forbid same# is %off and (1,2,3,4,5) as the starting permutation, the outcome might be one of "
	"the five permutations (3,2,1,4,5), (1,3,2,4,5), (1,2,3,4,5), (1,2,4,3,5), (1,2,5,4,3). If ##Forbid same# were chosen as %on, the "
	"(1,2,3,4,5) permutation is forbidden and the outcome could only be one of the four remaining permutations.")
MAN_END

MAN_BEGIN (U"Permutation: Permute randomly...", U"djmw", 20111123)
INTRO (U"Generates a new @@Permutation@ by randomly permuting a range of elements in the selected Permutation object.")
ENTRY (U"Setting")
TAG (U"##Index range#")
DEFINITION (U"defines the range of elements that will be permuted. The elements outside this range will be kept intact.")
ENTRY (U"Example")
NORMAL (U"If we start with the permutation (4,6,3,1,5,2,7) and a chosen ##Index range# that runs from 3 to 6, a new permutation will be generated as follows:")
LIST_ITEM (U"1. A new permutation of the same dimension as the selected one will be created. ")
LIST_ITEM (U"2. Because the index range starts at 3, the first two elements of the selected permutation will be copied "
	"to the first two locations in the newly created permutation. The new permutation is now (4,6,.,.,.,.,.), where a "
	"dot (.) means that the element is unspecified.")
LIST_ITEM (U"3. The elements 3 to 6 of the selected permutation, i.e. the numbers (3,1,5,2) will be randomly permuted. "
	"There are 24 possible permutations of these 4 numbers. Say the outcome happens to be (5,1,3,2). The new permutation is now (4,6,5,1,3,2,.).")
LIST_ITEM (U"4. The remaining element (7) is copied to the new permutation. Finally, this results "
	"in the new permutation being (4,6,5,1,3,2,7).")
MAN_END

MAN_BEGIN (U"Permutation: Permute randomly (blocks)...", U"djmw", 20110105)
INTRO (U"Generates a new @Permutation by randomly permuting blocks of size %blocksize.")
ENTRY (U"Settings")
TAG (U"##Index range#")
DEFINITION (U"the range of elements whose blocks will be permuted.")
TAG (U"##Block size#")
DEFINITION (U"the size of the blocks that will be permuted. There must fit an integer number of blocks "
	"in the chosen range.")
TAG (U"##Permute within blocks#")
DEFINITION (U"when %on, the elements in each block are also randomly permuted.")
TAG (U"##No doublets#")
DEFINITION (U"guarantees that the first element in each block does not equal the last element of the previous block modulo "
	"the block size. E.g. the numbers 3, 6, 9 are all equal modulo 3. "
	"This parameter only has effect when ##Permute within blocks# is %on.")
ENTRY (U"Examples")
NORMAL (U"1. With ##Index range# = [0,0], ##Block size# = 3 and ##Permute within blocks# is %off, the permutation ((1,2,3),(4,5,6),(7,8,9)) "
	"is turned into one of six possible permutations, for example into ((4,5,6),(7,8,9),(1,2,3)). (The option ##No doublets# will be ignored and the parentheses are only there to indicate the blocks.)")
NORMAL (U"2. With ##Index range# = [0,0], ##Block size# = 3, ##Permute within blocks# is %on and ##No doublets# is %off, "
	"the permutation ((1,2,3),(4,5,6),(7,8,9)) might turn into ((5,4,6),(9,8,7),(3,1,2)).")
NORMAL (U"3. With the same options as 2 but ##No doublets# is %on, the previously given outcome is forbidden because "
	"the last element of the first block (6) and the first element of the next block (9) are equal modulo 3 (the "
	"blocksize). A valid outcome might then be ((5,4,6),(8,9,7),(3,1,2)).")
MAN_END

MAN_BEGIN (U"Permutation: Swap blocks...", U"djmw", 20110105)
INTRO (U"A command to swap the contents of two index ranges in the selected @Permutation.")
ENTRY (U"Settings")
TAG (U"##From index#, ##To index#")
DEFINITION (U"the two starting positions from where elements are to be swapped. The blocks may overlap.")
TAG (U"##Block size#")
DEFINITION (U"determines the number of pairs to swap. ")
ENTRY (U"Behaviour")
NORMAL (U"If the ##Block size# equals one, only the elements at the ##From index# and ##To index# position are swapped. If blocksize is greater than one, the two elements at ##From index#+1 and ##To index#+1 will be swapped too. This goes on until the last two elements in each block have been swapped.")
ENTRY (U"Examples")
NORMAL (U"1. Swap two blocks: with ##From index# = 1, ##To index# = 4, and ##Block size# = 2, the permutation (1,2,3,4,5) is turned into (4,5,3,1,2).")
NORMAL (U"2. Swap two elements: with ##From index# = 1, ##To index# = 4, and ##Block size# = 1, the permutation (1,2,3,4,5) is turned into (4,2,3,1,5).")
NORMAL (U"3. Swap two overlapping blocks: with ##From index# = 1, ##To index# = 3, and ##Block size# = 3, the permutation (1,2,3,4,5) is turned into "
	"(3,4,5,2,1).")
MAN_END

MAN_BEGIN (U"Permutation: Swap positions...", U"djmw", 20110105)
INTRO (U"Swaps the contents at two indices in the selected @@Permutation@.")
ENTRY (U"Settings")
TAG (U"##First index#, ##Second index#")
DEFINITION (U"the two indices from where elements have to be swapped. The order of these indices is not important.")
ENTRY (U"Example")
NORMAL (U"With ##First index# = 1 and ##Second index# = 3, the permutation (1,3,4,2,5) is turned into (4,3,1,2,5).")
MAN_END

MAN_BEGIN (U"Permutation: Swap numbers...", U"djmw", 20110105)
INTRO (U"Swaps two numbers in the selected @@Permutation@.")
ENTRY (U"Settings")
TAG (U"##First number#, ##Second number#")
DEFINITION (U"the two numbers that have to be swapped. The order of these numbers is not important.")
ENTRY (U"Example")
NORMAL (U"With ##First number# = 1 and ##Second number# = 3, the permutation (1,3,4,2,5) is turned into (3,1,4,2,5).")
MAN_END

MAN_BEGIN (U"Permutation: Interleave...", U"djmw", 20110105)
INTRO (U"Generates a new @Permutation by interleaving elements from successive blocks. ")
NORMAL (U"We always start with the first element in the first block. When the offset is zero, the next element will be the first "
	"element of the second block, then the first element of the third block. After the first element of the last block, we start again "
	"with the second elements in each block. And so on. (In card playing, with two blocks of 26 cards each, this is called a faro "
	"shuffle and eight successive faro shuffles will return the deck to precisely the order in which you began.)")
NORMAL (U"If the offset differs from zero and equals 1 for example, we start with the first element in the first block, then the "
	"second element in the second block, the third element in the third block and so on. When the last element of a block is reached "
	"and the number of blocks is not exhausted the next element will be the first from the next block. When the last block is reached, "
	"we start the same cycle again with the next lower element in the first block (which by the way need not be the second element, "
	"see also example 4).")
ENTRY (U"Settings")
TAG (U"##Index range#")
DEFINITION (U"the range of elements that will be permuted.")
TAG (U"##Block size#")
DEFINITION (U"the size of a block. An integer number of blocks must fit "
	"in the chosen ##Index range#.")
TAG (U"##Offset#")
DEFINITION (U"determines the relative positions of selected elements in successive blocks.")
ENTRY (U"Examples")
NORMAL (U"1. With ##Index range# = [0,0], ##Block size# = 3, and ##Offset# = 0, the permutation ((1,2,3),(4,5,6),(7,8,9)) is turned into (1,4,7,2,5,8,3,6,9).")
NORMAL (U"2. With ##Index range# = [0,0], ##Block size# = 3, and ##Offset# = 1, the permutation ((1,2,3),(4,5,6),(7,8,9)) is turned into (1,5,9,2,6,7,3,4,8).")
NORMAL (U"3. With ##Index range# = [0,0], ##Block size# = 3, and ##Offset# = 2, the permutation ((1,2,3),(4,5,6),(7,8,9)) is turned into (1,6,8,2,4,9,3,5,7).")
NORMAL (U"4. With ##Index range# = [0,0], ##Block size# = 4, and ##Offset# = 1, the permutation ((1,2,3,4),(5,6,7,8)) is turned into (1,6,3,8,2,7,4,5).")
MAN_END

MAN_BEGIN (U"lexicographic permutation order", U"djmw", 20140131)
INTRO (U"We can order the %n numbers 1, 2, 3,..., n in %n! different ways. Each of these n! orderings represents a different permutation of "
	"the numbers 1, 2, 3,..., n. For example, if %n equals 3 we have 6 (=3\\.c2\\.c1) possible orderings: (1,2,3), (1,3,2), (2,1,3), (2,3,1), (3,1,2) and (3,2,1). "
	"The %%lexicographic permutation order% starts from the identity permutation (1,2,..., n). By successively swapping only two numbers one "
	"obtains all possible permutations. The last permutation in lexicographic order will be the permutation with all numbers in reversed order, "
	"i.e. (n,n-1,...,2,1). The example given above has all 6 permutations in lexicographic permutation order.")
MAN_END

MAN_BEGIN (U"Permutation: Next", U"djmw", 20140131)
INTRO (U"Get the next @@Permutation|permutation@ in @@lexicographic permutation order@. ")
NORMAL (U"The next permutation is obtained from the selected permutation "
	"by swapping values %%at only two positions%. Starting with the identity permutation and "
	"repeatedly applying this function will iterate through all possible permutations. If no further permutation "
	"is available, i.e. the selected permutation is at the lexicographic end position (n, n-1, ..., 3, 2, 1), the current permutation will not change anymore. ")
ENTRY (U"Examples")
NORMAL (U"If we start with (1,2,3,4) successively applying ##Next# will generate the following sequence (1,2,4,3), (1,3,2,4), (1,3,4,2), (1,4,2,3), (1,4,3,2), etc.")
MAN_END

MAN_BEGIN (U"Permutation: Previous", U"djmw", 20140131)
NORMAL (U"Get the previous @@Permutation|permutation@ in @@lexicographic permutation order@. The previous permutation is obtained "
	"from the selected permutation by swapping values %%at only two positions%. If no further permutation "
	"is available, i.e. the current permutation is at the lexicographic start position (1, 2, 3, ..., n-1, n), the current permutation will not change anymore. ")
NORMAL (U"The ##Previous# operation follows the opposite order of @@Permutation: Next@.")
MAN_END

MAN_BEGIN (U"Permutation: Rotate...", U"djmw", 20110105)
INTRO (U"A circular shift of all elements within the given range.")
ENTRY (U"Settings")
TAG (U"##Index range#")
DEFINITION (U"the range of elements that will be circularly permuted.")   // ambiguous; are these the positions or the numbers?
TAG (U"##Step size#")
DEFINITION (U"define how many positions each element will be shifted.")
ENTRY (U"Examples")
NORMAL (U"1. With ##Step size# = 2 and ##Index range# = [1,5], the permutation (1,2,3,4,5) is turned into (4,5,1,2,3). ")
NORMAL (U"2. With ##Step size# = 2 and ##Index range# = [2,5], the permutation ((1),(2,3,4,5)) is turned into ((1),(4,5,2,3))")
NORMAL (U"3. With ##Step size# = -1 and ##Index range# = [0,0], the permutation (1,2,3,4,5) is turned into (2,3,4,5,1).")
MAN_END

MAN_BEGIN (U"Permutation: Invert", U"djmw", 20050709)
INTRO (U"Generates the inverse of the selected @Permutation.")
ENTRY (U"Example")
NORMAL (U"If the permutation is (1,5,3,2,4) the inverse will be (1,4,3,5,2). If we @@Permutations: Multiply|multiply@ these two permutations the result will be the identity permutation (1,2,3,4,5).")
MAN_END

MAN_BEGIN (U"Permutations: Multiply", U"djmw", 20050717)
INTRO (U"Apply the selected @@Permutation@s one after the other. ")
NORMAL (U"Permutations are %not commutative, i.e. applying permutation %p__1_ after %p__2_ might not give the same outcome as applying "
	"%p__2_ after %p__1_.")
MAN_END

MAN_BEGIN (U"Permutation: Sort", U"djmw", 20050709)
INTRO (U"Sorts the elements ascending, i.e. set the selected @@Permutation@ to the @@identity permutation@.")
MAN_END

MAN_BEGIN (U"TableOfReal & Permutation: Permute rows", U"djmw", 20050709)
INTRO (U"Generate a new @TableOfReal with a row ordering determined by the @Permutation.")
ENTRY (U"Example")
NORMAL (U"If the selected TableOfReal has 5 rows and the permutation is (5,4,3,2,1) the first row of the new TableOfReal equals the fifth row of the selected, the second row of new equals the fourth row of the selected and so on.")
MAN_END

MAN_BEGIN (U"Strings & Permutation: Permute strings", U"djmw", 20140130)
INTRO (U"Generate a new @Strings with a strings ordering determined by the @Permutation.")
NORMAL (U"The number of strings in the #Strings and the number of elements in the #Permutation have to be equal.")
ENTRY (U"Examples")
NORMAL (U"1. If the selected Strings has the 4 strings ordered as \"heed\", \"hid\", \"hood\", \"hud\", and the permutation is "
	"(4,3,2,1), the new Strings has the ordering \"hud\", \"hood\", \"hid\", \"heed\".")
NORMAL (U"2. In the example that is discussed in the @@ExperimentMFC|listening experiment@ section, we have four stimuli \"heed.wav\", "
	"\"hid.wav\", \"hood.wav\", \"hud.wav\" that we want to present three times to each subject with a <PermuteBalancedNoDoublets> "
	"randomization strategy, i.e. stimuli presented in blocks of four, randomized, and no two successive stimuli equal. "
	"This type of randomization can easily be accomplished with a Permutation object and a Strings." )
LIST_ITEM (U"1. Fill the Strings object with 12 strings, i.e. three repetitions of the four stimuli. ")
LIST_ITEM (U"2. Create a Permutation object with 12 elements and perform  ##@@Permutation: Permute randomly (blocks)...|Permute randomly (blocks):@ 0, 0, 4, \"yes\", \"yes\"#. We randomly permute blocks of size 4 and permute randomly within these blocks and make sure that on the transition from on block to the other no two stimuli are equal. (Of course, the random permutation of the blocks makes no difference here since all the blocks have the same content.)")
LIST_ITEM (U"3. Select the Strings and the Permutation together and choose ##Permute strings#. "
	"Now the new Strings will contain the new ordering of the stimuli.")
MAN_END

MAN_BEGIN (U"Strings: To Permutation...", U"djmw", 20050721)
INTRO (U"Generates a @Permutation with the same number of elements as the @Strings.")
ENTRY (U"Setting")
TAG (U"##Sort")
DEFINITION (U"determines whether the Permutation will have an element ordering that can be used to sort the Strings alphabetically.")
ENTRY (U"Example")
NORMAL (U"If \"Sort\" is %on, and the selected Strings contains 4 strings ordered as \"hud\", \"hid\", \"hood\", "
	"\"heed\", the generated Permutation will be (4,2,3,1). If you now select the String and the Permutation "
	"together and choose @@Strings & Permutation: Permute strings|Permute strings@, the new Strings will have "
	"the strings ordered alphabetically as \"heed\", \"hid\", \"hood\", \"hud\". "
	"You can also sort the Strings alphabetically descending, by first @@Permutation: Reverse...|reversing@ "
	"the elements in the Permutation before you select the Permutation and the Strings together. ")
MAN_END

MAN_BEGIN (U"Index", U"djmw", 20050725)
INTRO (U"One of the @@Types of objects|types of objects@ in the P\\s{RAAT} program.")
MAN_END

MAN_BEGIN (U"Strings: To Index", U"djmw", 20050721)
INTRO (U"Generates an @Index from the selected @Strings.")
ENTRY (U"Example")
NORMAL (U"We start from the following #Strings:")
CODE (U"6 (number of strings)")
CODE (U"\"hallo\"")
CODE (U"\"dag allemaal\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
NORMAL (U"This will give us the following #Index:")
CODE (U"1 (number of columns) \"\" (no column name)")
CODE (U"\"dag allemaal\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
CODE (U"6 (number of elements)")
CODE (U"2")
CODE (U"1")
CODE (U"2")
CODE (U"3")
CODE (U"2")
CODE (U"3")
MAN_END

MAN_BEGIN (U"Index: To Permutation...", U"djmw", 20050725)
INTRO (U"Generates a @Permutation from the selected @Index by randomly permuting blocks of equivalent elements.")
NORMAL (U"Suppose your data consists of groups of equivalent elements and the number of elements in the groups are not equal. You want to make random ordering of your data such that the elements in a group stay together. The following example shows you how.")
ENTRY (U"Setting")
TAG (U"##Permute within classes")
DEFINITION (U"determines whether the elements within a class will be randomly permuted.")
ENTRY (U"Example")
NORMAL (U"Suppose your data, for example a @Strings, consists of groups of equivalent elements and the number of elements in the groups are not equal. You want to make a random ordering of your data such that the elements in a group stay together. The following example shows you how.")
NORMAL (U"We start from the following Strings:")
CODE (U"6 (number of strings)")
CODE (U"\"hallo\"")
CODE (U"\"dag allemaal\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
NORMAL (U"We choose @@Strings: To Index|To Index@ which will give us the following #Index:")
CODE (U"1 (number of columns) \"\" (no column name)")
CODE (U"\"dag allemaal\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
CODE (U"6 (number of elements)")
CODE (U"2")
CODE (U"1")
CODE (U"2")
CODE (U"3")
CODE (U"2")
CODE (U"3")
NORMAL (U"We choose ##To Permutation# and with ##Permute within classes# %off, this might generate the permutation (2,4,6,1,3,5).")
NORMAL (U"Selecting the Permutation and the Strings together and choosing @@Strings & Permutation: "
	"Permute strings|Permute strings@ will generate the following Strings:")
CODE (U"\"dag allemaal\"")
CODE (U"\"tot morgen\"")
CODE (U"\"tot morgen\"")
CODE (U"\"hallo\"")
CODE (U"\"hallo\"")
CODE (U"\"hallo\"")
NORMAL (U"We see that the permutation always keeps identical strings together.")
MAN_END

MAN_BEGIN (U"Index: Extract part...", U"djmw", 20050725)
INTRO (U"Creates a new @Index by copying a part of selected Index.")
ENTRY (U"Example")
NORMAL (U"Given the following Index:")
CODE (U"1 (number of columns) \"\" (no column name)")
CODE (U"\"dag allemaal\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
CODE (U"6 (number of elements)")
CODE (U"2")
CODE (U"1")
CODE (U"2")
CODE (U"3")
CODE (U"2")
CODE (U"3")
NORMAL (U"The command ##Extract part... 1 2# gives you the new Index:")
CODE (U"1 (number of columns) \"\" (no column name)")
CODE (U"\"dag allemaal\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
CODE (U"6 (number of elements)")
CODE (U"2")
CODE (U"1")
NORMAL (U"Note that all classes stay intact and may have zero references like for example the \"tot morgen\" class. ")
MAN_END
}

/* End of file manual_Permutation.cpp */

