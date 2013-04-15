/* manual_Permutation.cpp
 *
 * Copyright (C) 2005-2013 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
  20050709 djmw
*/

#include "ManPagesM.h"

void manual_Permutation_init (ManPages me);
void manual_Permutation_init (ManPages me)
{

MAN_BEGIN (L"Permutation", L"djmw", 20050721)
INTRO (L"One of the @@types of objects@ in Praat. A Permutation object with %n elements consists of some ordering of "
	"the numbers 1,2...%n.")
ENTRY (L"Interpretation")
NORMAL (L"A permutation like for example (2,3,5,4,1) is an %arrangement of the five objects 1, 2, 3, 4, and 5. "
	"It tells us that the second object is in the first position, the third object is in the second position, "
	"the fifth object in the third position and so on.")
NORMAL (L"If we combine a Permutation together with an other object, like a Strings for example, we may force a  "
	"new arrangement of the strings, according to the specification in the Permutation (see @@Strings & Permutation: Permute strings@)." )
ENTRY (L"Commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Create Permutation...@")
NORMAL (L"Query:")
LIST_ITEM (L"\\bu ##Get number of elements#")
LIST_ITEM (L"\\bu @@Permutation: Get value...|Get value...@")
LIST_ITEM (L"\\bu @@Permutation: Get index...|Get index...@")
NORMAL (L"Modification:")
LIST_ITEM (L"\\bu @@Permutation: Sort|Sort@")
LIST_ITEM (L"\\bu @@Permutation: Swap blocks...|Swap blocks...@")
LIST_ITEM (L"\\bu @@Permutation: Swap positions...|Swap positions...@")
LIST_ITEM (L"\\bu @@Permutation: Swap numbers...|Swap numbers...@")
LIST_ITEM (L"\\bu @@Permutation: Swap one from range...|Swap one from range...@")
NORMAL (L"Permutations:")
LIST_ITEM (L"\\bu @@Permutation: Permute randomly...|Permute randomly...@")
LIST_ITEM (L"\\bu @@Permutation: Permute randomly (blocks)...|Permute randomly (blocks)...@")
LIST_ITEM (L"\\bu @@Permutation: Interleave...|Interleave...@")
LIST_ITEM (L"\\bu @@Permutation: Rotate...|Rotate...@")
LIST_ITEM (L"\\bu @@Permutation: Reverse...|Reverse...@")
LIST_ITEM (L"\\bu @@Permutation: Invert|Invert@")
NORMAL (L"Successive permutations:")
LIST_ITEM (L"\\bu @@Permutations: Multiply|Multiply@")
ENTRY (L"Usage")
LIST_ITEM (L"@@Strings & Permutation: Permute strings@ to rearrange the strings in a Strings object.")
LIST_ITEM (L"@@TableOfReal & Permutation: Permute rows@ to rearrange the rows in a TableOfReal object.")
MAN_END

MAN_BEGIN (L"Create Permutation...", L"djmw", 20050709)
INTRO (L"A command to create a @Permutation of the numbers 1,2, ..., %numberOfElements.")
ENTRY (L"Settings")
TAG (L"##Name")
DEFINITION (L"the name of the new permutation.")
TAG (L"##Number of elements%")
DEFINITION (L"the number of elements in the permutation.")
TAG (L"##Identity permutation")
DEFINITION (L"determines whether the permution will be a randomly chosen one, or the @@identity permutation@.")
MAN_END

MAN_BEGIN (L"identity permutation", L"djmw", 20050713)
INTRO (L"The identity permutation is (1,2,3,...,%numberOfElements), i.e. the numbers 1 to "
	"%numberOfElements in their natural order. ")
MAN_END

MAN_BEGIN (L"Permutation: Get value...", L"djmw", 20050709)
INTRO (L"Get the value at the index position.")
ENTRY (L"Example")
NORMAL (L"The query for the value at index 3 for the permutation (3,2,4,5,1) gives 4.")
MAN_END

MAN_BEGIN (L"Permutation: Get index...", L"djmw", 20050714)
INTRO (L"Get the index position of the value. ")
ENTRY (L"Example")
NORMAL (L"The query for the index of value 3 for the permutation (3,2,4,5,1) gives 1.")
MAN_END

MAN_BEGIN (L"Permutation: Reverse...", L"djmw", 20110105)
INTRO (L"Reverse the elements in the given range.")
ENTRY (L"Setting")
TAG (L"##Index range#")
DEFINITION (L"defines the range of indices that will be reversed.")
ENTRY (L"Examples")
NORMAL (L"1. With ##Index range# = [0,0], the permutation (1,2,3,4,5) is turned into (5,4,3,2,1). ")
NORMAL (L"2. With ##Index range# = [3,0], the permutation (1,2,3,4,5) is turned into (1,2,5,4,3). ")
MAN_END

MAN_BEGIN (L"Permutation: Swap one from range...", L"djmw", 20110105)
INTRO (L"An element at an index, randomly chosen from a range, will be permuted with an element at a prescribed index position.")
ENTRY (L"Settings")
TAG (L"##Index range#")
DEFINITION (L"defines the range of indices from which one will be randomly chosen.")
TAG (L"##Index#")
DEFINITION (L"defines the special index position whose element will be interchanged with the one chosen from the range.")
TAG (L"##Forbid same")
DEFINITION (L"when %on, forbids the randomly chosen position and the index position to be the same. "
	"This switch is only of relevance when the chosen range happens to overlap the index position.")
ENTRY (L"Examples")
NORMAL (L"With ##Index range# = [0,0], ##Index# = 3, ##Forbid same# is %off and (1,2,3,4,5) as the starting permutation, the outcome might be one of "
	"the five permutations (3,2,1,4,5), (1,3,2,4,5), (1,2,3,4,5), (1,2,4,3,5), (1,2,5,4,3). If ##Forbid same# were chosen as %on, the "
	"(1,2,3,4,5) permutation is forbidden and the outcome could only be one of the four remaining permutations.")
MAN_END

MAN_BEGIN (L"Permutation: Permute randomly...", L"djmw", 20111123)
INTRO (L"Generates a new @@Permutation@ by randomly permuting a range of elements in the selected Permutation object.")
ENTRY (L"Setting")
TAG (L"##Index range#")
DEFINITION (L"defines the range of elements that will be permuted. The elements outside this range will be kept intact.")
ENTRY (L"Example")
NORMAL (L"If we start with the permutation (4,6,3,1,5,2,7) and a chosen ##Index range# that runs from 3 to 6, a new permutation will be generated as follows:")
LIST_ITEM (L"1. A new permutation of the same dimension as the selected one will be created. ")
LIST_ITEM (L"2. Because the index range starts at 3, the first two elements of the selected permutation will be copied "
	"to the first two locations in the newly created permutation. The new permutation is now (4,6,.,.,.,.,.), where a "
	"dot (.) means that the element is unspecified.")
LIST_ITEM (L"3. The elements 3 to 6 of the selected permutation, i.e. the numbers (3,1,5,2) will be randomly permuted. "
	"There are 24 possible permutations of these 4 numbers. Say the outcome happens to be (5,1,3,2). The new permutation is now (4,6,5,1,3,2,.).")
LIST_ITEM (L"4. The remaining element (7) is copied to the new permutation. Finally, this results "
	"in the new permutation being (4,6,5,1,3,2,7).")
MAN_END

MAN_BEGIN (L"Permutation: Permute randomly (blocks)...", L"djmw", 20110105)
INTRO (L"Generates a new @Permutation by randomly permuting blocks of size %blocksize.")
ENTRY (L"Settings")
TAG (L"##Index range#")
DEFINITION (L"the range of elements whose blocks will be permuted.")
TAG (L"##Block size#")
DEFINITION (L"the size of the blocks that will be permuted. There must fit an integer number of blocks "
	"in the chosen range.")
TAG (L"##Permute within blocks#")
DEFINITION (L"when %on, the elements in each block are also randomly permuted.")
TAG (L"##No doublets#")
DEFINITION (L"guarantees that the first element in each block does not equal the last element of the previous block modulo "
	"the block size. E.g. the numbers 3, 6, 9 are all equal modulo 3. "
	"This parameter only has effect when ##Permute within blocks# is %on.")
ENTRY (L"Examples")
NORMAL (L"1. With ##Index range# = [0,0], ##Block size# = 3 and ##Permute within blocks# is %off, the permutation ((1,2,3),(4,5,6),(7,8,9)) "
	"is turned into one of six possible permutations, for example into ((4,5,6),(7,8,9),(1,2,3)). (The option ##No doublets# will be ignored and the parentheses are only there to indicate the blocks.)")
NORMAL (L"2. With ##Index range# = [0,0], ##Block size# = 3, ##Permute within blocks# is %on and ##No doublets# is %off, "
	"the permutation ((1,2,3),(4,5,6),(7,8,9)) might turn into ((5,4,6),(9,8,7),(3,1,2)).")
NORMAL (L"3. With the same options as 2 but ##No doublets# is %on, the previously given outcome is forbidden because "
	"the last element of the first block (6) and the first element of the next block (9) are equal modulo 3 (the "
	"blocksize). A valid outcome might then be ((5,4,6),(8,9,7),(3,1,2)).")
MAN_END

MAN_BEGIN (L"Permutation: Swap blocks...", L"djmw", 20110105)
INTRO (L"A command to swap the contents of two index ranges in the selected @Permutation.")
ENTRY (L"Settings")
TAG (L"##From index#, ##To index#")
DEFINITION (L"the two starting positions from where elements are to be swapped. The blocks may overlap.")
TAG (L"##Block size#")
DEFINITION (L"determines the number of pairs to swap. ")
ENTRY (L"Behaviour")
NORMAL (L"If the ##Block size# equals one, only the elements at the ##From index# and ##To index# position are swapped. If blocksize is greater than one, the two elements at ##From index#+1 and ##To index#+1 will be swapped too. This goes on until the last two elements in each block have been swapped.")
ENTRY (L"Examples")
NORMAL (L"1. Swap two blocks: with ##From index# = 1, ##To index# = 4, and ##Block size# = 2, the permutation (1,2,3,4,5) is turned into (4,5,3,1,2).")
NORMAL (L"2. Swap two elements: with ##From index# = 1, ##To index# = 4, and ##Block size# = 1, the permutation (1,2,3,4,5) is turned into (4,2,3,1,5).")
NORMAL (L"3. Swap two overlapping blocks: with ##From index# = 1, ##To index# = 3, and ##Block size# = 3, the permutation (1,2,3,4,5) is turned into "
	"(3,4,5,2,1).")
MAN_END

MAN_BEGIN (L"Permutation: Swap positions...", L"djmw", 20110105)
INTRO (L"Swaps the contents at two indices in the selected @@Permutation@.")
ENTRY (L"Settings")
TAG (L"##First index#, ##Second index#")
DEFINITION (L"the two indices from where elements have to be swapped. The order of these indices is not important.")
ENTRY (L"Example")
NORMAL (L"With ##First index# = 1 and ##Second index# = 3, the permutation (1,3,4,2,5) is turned into (4,3,1,2,5).")
MAN_END

MAN_BEGIN (L"Permutation: Swap numbers...", L"djmw", 20110105)
INTRO (L"Swaps two numbers in the selected @@Permutation@.")
ENTRY (L"Settings")
TAG (L"##First number#, ##Second number#")
DEFINITION (L"the two numbers that have to be swapped. The order of these numbers is not important.")
ENTRY (L"Example")
NORMAL (L"With ##First number# = 1 and ##Second number# = 3, the permutation (1,3,4,2,5) is turned into (3,1,4,2,5).")
MAN_END

MAN_BEGIN (L"Permutation: Interleave...", L"djmw", 20110105)
INTRO (L"Generates a new @Permutation by interleaving elements from successive blocks. ")
NORMAL (L"We always start with the first element in the first block. When the offset is zero, the next element will be the first "
	"element of the second block, then the first element of the third block. After the first element of the last block, we start again "
	"with the second elements in each block. And so on. (In card playing, with two blocks of 26 cards each, this is called a faro "
	"shuffle and eight successive faro shuffles will return the deck to precisely the order in which you began.)")
NORMAL (L"If the offset differs from zero and equals 1 for example, we start with the first element in the first block, then the "
	"second element in the second block, the third element in the third block and so on. When the last element of a block is reached "
	"and the number of blocks is not exhausted the next element will be the first from the next block. When the last block is reached, "
	"we start the same cycle again with the next lower element in the first block (which by the way need not be the second element, "
	"see also example 4).")
ENTRY (L"Settings")
TAG (L"##Index range#")
DEFINITION (L"the range of elements that will be permuted.")
TAG (L"##Block size#")
DEFINITION (L"the size of a block. An integer number of blocks must fit "
	"in the chosen ##Index range#.")
TAG (L"##Offset#")
DEFINITION (L"determines the relative positions of selected elements in successive blocks.")
ENTRY (L"Examples")
NORMAL (L"1. With ##Index range# = [0,0], ##Block size# = 3, and ##Offset# = 0, the permutation ((1,2,3),(4,5,6),(7,8,9)) is turned into (1,4,7,2,5,8,3,6,9).")
NORMAL (L"2. With ##Index range# = [0,0], ##Block size# = 3, and ##Offset# = 1, the permutation ((1,2,3),(4,5,6),(7,8,9)) is turned into (1,5,9,2,6,7,3,4,8).")
NORMAL (L"3. With ##Index range# = [0,0], ##Block size# = 3, and ##Offset# = 2, the permutation ((1,2,3),(4,5,6),(7,8,9)) is turned into (1,6,8,2,4,9,3,5,7).")
NORMAL (L"4. With ##Index range# = [0,0], ##Block size# = 4, and ##Offset# = 1, the permutation ((1,2,3,4),(5,6,7,8)) is turned into (1,6,3,8,2,7,4,5).")
MAN_END

MAN_BEGIN (L"Permutation: Next", L"djmw", 20100521)
NORMAL (L"Get the next @@Permutation|permutation@ in lexicographic order. Starting with the identity permutation and "
	"repeatedly applying this function will iterate through all possible permutations. If no further permutation "
	"is available the current permutation will not change. ")
ENTRY (L"Examples")
NORMAL (L"If we start with (1,2,3,4) successively applying Next will generate the following sequence (1,2,4,3), (1,3,2,4), (1,3,4,2), (1,4,2,3), (1,4,3,2), etc.")
MAN_END

MAN_BEGIN (L"Permutation: Previous", L"djmw", 20100521)
NORMAL (L"Get the previous @@Permutation|permutation@ in lexicographic order. If no further permutation "
	"is available the current permutation will not change. ")
NORMAL (L"This Previous operation follows the opposite order of @@Permutation: Next@.")
MAN_END

MAN_BEGIN (L"Permutation: Rotate...", L"djmw", 20110105)
INTRO (L"A circular shift of all elements within the given range.")
ENTRY (L"Settings")
TAG (L"##Index range#")
DEFINITION (L"the range of elements that will be circularly permuted.")   // ambiguous; are these the positions or the numbers?
TAG (L"##Step size#")
DEFINITION (L"define how many positions each element will be shifted.")
ENTRY (L"Examples")
NORMAL (L"1. With ##Step size# = 2 and ##Index range# = [1,5], the permutation (1,2,3,4,5) is turned into (4,5,1,2,3). ")
NORMAL (L"2. With ##Step size# = 2 and ##Index range# = [2,5], the permutation ((1),(2,3,4,5)) is turned into ((1),(4,5,2,3))")
NORMAL (L"3. With ##Step size# = -1 and ##Index range# = [0,0], the permutation (1,2,3,4,5) is turned into (2,3,4,5,1).")
MAN_END

MAN_BEGIN (L"Permutation: Invert", L"djmw", 20050709)
INTRO (L"Generates the inverse of the selected @Permutation.")
ENTRY (L"Example")
NORMAL (L"If the permutation is (1,5,3,2,4) the inverse will be (1,4,3,5,2). If we @@Permutations: Multiply|multiply@ these two permutations the result will be the identity permutation (1,2,3,4,5).")
MAN_END

MAN_BEGIN (L"Permutations: Multiply", L"djmw", 20050717)
INTRO (L"Apply the selected @@Permutation@s one after the other. ")
NORMAL (L"Permutations are %not commutative, i.e. applying permutation %p__1_ after %p__2_ might not give the same outcome as applying "
	"%p__2_ after %p__1_.")
MAN_END

MAN_BEGIN (L"Permutation: Sort", L"djmw", 20050709)
INTRO (L"Sorts the elements ascending, i.e. set the selected @@Permutation@ to the @@identity permutation@.")
MAN_END

MAN_BEGIN (L"TableOfReal & Permutation: Permute rows", L"djmw", 20050709)
INTRO (L"Generate a new @TableOfReal with a row ordering determined by the @Permutation.")
ENTRY (L"Example")
NORMAL (L"If the selected TableOfReal has 5 rows and the permutation is (5,4,3,2,1) the first row of the new TableOfReal equals the fifth row of the selected, the second row of new equals the fourth row of the selected and so on.")
MAN_END

MAN_BEGIN (L"Strings & Permutation: Permute strings", L"djmw", 20050721)
INTRO (L"Generate a new @Strings with a strings ordering determined by the @Permutation.")
NORMAL (L"The number of strings in the #Strings and the number of elements in the #Permutation have to be equal.")
ENTRY (L"Examples")
NORMAL (L"1. If the selected Strings has the 4 strings ordered as \"heed\", \"hid\", \"hood\", \"hud\", and the permutation is "
	"(4,3,2,1), the new Strings has the ordering \"hud\", \"hood\", \"hid\", \"heed\".")
NORMAL (L"2. In the example that is discussed in the @@ExperimentMFC|listening experiment@ section, we have four stimuli \"heed.wav\", "
	"\"hid.wav\", \"hood.wav\", \"hud.wav\" that we want to present three times to each subject with a <PermuteBalancedNoDoublets> "
	"randomization strategy, i.e. stimuli presented in blocks of four, randomized, and no two successive stimuli equal. "
	"This type of randomization can easily be accomplished with a Permutation object and a Strings." )
LIST_ITEM (L"1. Fill the Strings object with 12 strings, i.e. three repetitions of the four stimuli. ")
LIST_ITEM (L"2. Create a Permutation object with 12 elements and perform  ##@@Permutation: Permute randomly (blocks)...|Permute randomly (blocks)...@ 0 0 4 yes yes#. We randomly permute blocks of size 4 and permute randomly within these blocks and make sure that on the transition from on block to the other no two stimuli are equal. (Of course, the random permutation of the blocks makes no difference here since all the blocks have the same content.)")
LIST_ITEM (L"3. Select the Strings and the Permutation together and choose ##Permute strings#. "
	"Now the new Strings will contain the new ordering of the stimuli.")
MAN_END

MAN_BEGIN (L"Strings: To Permutation...", L"djmw", 20050721)
INTRO (L"Generates a @Permutation with the same number of elements as the @Strings.")
ENTRY (L"Setting")
TAG (L"##Sort")
DEFINITION (L"determines whether the Permutation will have an element ordering that can be used to sort the Strings alphabetically.")
ENTRY (L"Example")
NORMAL (L"If \"Sort\" is %on, and the selected Strings contains 4 strings ordered as \"hud\", \"hid\", \"hood\", "
	"\"heed\", the generated Permutation will be (4,2,3,1). If you now select the String and the Permutation "
	"together and choose @@Strings & Permutation: Permute strings|Permute strings@, the new Strings will have "
	"the strings ordered alphabetically as \"heed\", \"hid\", \"hood\", \"hud\". "
	"You can also sort the Strings alphabetically descending, by first @@Permutation: Reverse...|reversing@ "
	"the elements in the Permutation before you select the Permutation and the Strings together. ")
MAN_END

MAN_BEGIN (L"Index", L"djmw", 20050725)
INTRO (L"One of the @@Types of objects|types of objects@ in the P\\s{RAAT} program.")
MAN_END

MAN_BEGIN (L"Strings: To Index", L"djmw", 20050721)
INTRO (L"Generates an @Index from the selected @Strings.")
ENTRY (L"Example")
NORMAL (L"We start from the following #Strings:")
CODE (L"6 (number of strings)")
CODE (L"\"hallo\"")
CODE (L"\"dag allemaal\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
NORMAL (L"This will give us the following #Index:")
CODE (L"1 (number of columns) \"\" (no column name)")
CODE (L"\"dag allemaal\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
CODE (L"6 (number of elements)")
CODE (L"2")
CODE (L"1")
CODE (L"2")
CODE (L"3")
CODE (L"2")
CODE (L"3")
MAN_END

MAN_BEGIN (L"Index: To Permutation...", L"djmw", 20050725)
INTRO (L"Generates a @Permutation from the selected @Index by randomly permuting blocks of equivalent elements.")
NORMAL (L"Suppose your data consists of groups of equivalent elements and the number of elements in the groups are not equal. You want to make random ordering of your data such that the elements in a group stay together. The following example shows you how.")
ENTRY (L"Setting")
TAG (L"##Permute within classes")
DEFINITION (L"determines whether the elements within a class will be randomly permuted.")
ENTRY (L"Example")
NORMAL (L"Suppose your data, for example a @Strings, consists of groups of equivalent elements and the number of elements in the groups are not equal. You want to make a random ordering of your data such that the elements in a group stay together. The following example shows you how.")
NORMAL (L"We start from the following Strings:")
CODE (L"6 (number of strings)")
CODE (L"\"hallo\"")
CODE (L"\"dag allemaal\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
NORMAL (L"We choose @@Strings: To Index|To Index@ which will give us the following #Index:")
CODE (L"1 (number of columns) \"\" (no column name)")
CODE (L"\"dag allemaal\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
CODE (L"6 (number of elements)")
CODE (L"2")
CODE (L"1")
CODE (L"2")
CODE (L"3")
CODE (L"2")
CODE (L"3")
NORMAL (L"We choose ##To Permutation# and with ##Permute within classes# %off, this might generate the permutation (2,4,6,1,3,5).")
NORMAL (L"Selecting the Permutation and the Strings together and choosing @@Strings & Permutation: "
	"Permute strings|Permute strings@ will generate the following Strings:")
CODE (L"\"dag allemaal\"")
CODE (L"\"tot morgen\"")
CODE (L"\"tot morgen\"")
CODE (L"\"hallo\"")
CODE (L"\"hallo\"")
CODE (L"\"hallo\"")
NORMAL (L"We see that the permutation always keeps identical strings together.")
MAN_END

MAN_BEGIN (L"Index: Extract part...", L"djmw", 20050725)
INTRO (L"Creates a new @Index by copying a part of selected Index.")
ENTRY (L"Example")
NORMAL (L"Given the following Index:")
CODE (L"1 (number of columns) \"\" (no column name)")
CODE (L"\"dag allemaal\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
CODE (L"6 (number of elements)")
CODE (L"2")
CODE (L"1")
CODE (L"2")
CODE (L"3")
CODE (L"2")
CODE (L"3")
NORMAL (L"The command ##Extract part... 1 2# gives you the new Index:")
CODE (L"1 (number of columns) \"\" (no column name)")
CODE (L"\"dag allemaal\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
CODE (L"6 (number of elements)")
CODE (L"2")
CODE (L"1")
NORMAL (L"Note that all classes stay intact and may have zero references like for example the \"tot morgen\" class. ")
MAN_END
}

/* End of file manual_Permutation.cpp */

