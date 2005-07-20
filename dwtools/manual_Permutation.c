/* manual_Permutation.c */
/* David Weenink, 9 juli 2005 */

/*
	20050717 Latest modification
*/
#include "ManPagesM.h"

void manual_Permutation_init (ManPages me);
void manual_Permutation_init (ManPages me)
{

MAN_BEGIN ("Permutation", "djmw", 20050717)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. A Permutation object with %n elements consists of some ordering of the numbers 1,2...%n.")
ENTRY ("Interpretation")
NORMAL ("A permutation like for example (2,3,5,4,1) is an %arrangement of the five objects 1, 2, 3, 4, and 5. "
	"If we combine a Permutation together with an other object, like a @Strings for example, we want a new arrangement of the strings, "
		"according to the specification in the Permutation. " )
ENTRY ("Commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Create Permutation...@")
NORMAL ("Query:")
LIST_ITEM ("\\bu ##Get number of elements#")
LIST_ITEM ("\\bu @@Permutation: Get value...|Get value...@")
LIST_ITEM ("\\bu @@Permutation: Get index...|Get index...@")
NORMAL ("Modification:")
LIST_ITEM ("\\bu @@Permutation: Sort|Sort@")
LIST_ITEM ("\\bu @@Permutation: Swap one pair...|Swap one pair...@")
LIST_ITEM ("\\bu @@Permutation: Swap one from range...|Swap one from range...@")
NORMAL ("Permutations:")
LIST_ITEM ("\\bu @@Permutation: Permute randomly...|Permute randomly...@")
LIST_ITEM ("\\bu @@Permutation: Permute randomly (blocks)...|Permute randomly (blocks)...@")
LIST_ITEM ("\\bu @@Permutation: Interleave...|Interleave...@")
LIST_ITEM ("\\bu @@Permutation: Rotate...|Rotate...@")
LIST_ITEM ("\\bu @@Permutation: Reverse...|Reverse...@")
LIST_ITEM ("\\bu @@Permutation: Invert|Invert@")
NORMAL ("Successive permutations:")
LIST_ITEM ("\\bu @@Permutations: Multiply|Multiply@")
ENTRY ("Usage")
LIST_ITEM ("@@Strings & Permutation: Permute strings@ to rearrange the strings in a Strings object.")
LIST_ITEM ("@@TableOfReal & Permutation: Permute rows@ to rearrange the rows in a TableOfReal object.")
MAN_END

MAN_BEGIN ("Create Permutation...", "djmw", 20050709)
INTRO ("A command to create a @Permutation of the numbers 1,2, ..., %numberOfElements.")
ENTRY ("Arguments")
TAG ("%Name")
DEFINITION ("determines the name of the new permutation.")
TAG ("%%Number of elements%")
DEFINITION ("determines the number of elements in the permutation.")
TAG ("%%Identity Permutation")
NORMAL ("determines whether the permution will be a randomly chosen one, or the @@identity permutation@.")
MAN_END

MAN_BEGIN ("identity permutation", "djmw", 20050713)
INTRO ("The identity permutation is (1,2,3,...,%numberOfElements), i.e. the numbers 1 to "
	"%numberOfElements in their natural order. ")
MAN_END

MAN_BEGIN ("Permutation: Get value...", "djmw", 20050709)
INTRO ("Get the value at the index position.")
ENTRY ("Example")
NORMAL ("The query for the value at index 3 for the permutation (3,2,4,5,1) gives 4.")
MAN_END

MAN_BEGIN ("Permutation: Get index...", "djmw", 20050714)
INTRO ("Get the index position of the value. ")
ENTRY ("Example")
NORMAL ("The query for the index of value 3 for the permutation (3,2,4,5,1) gives 1.")
MAN_END

MAN_BEGIN ("Permutation: Reverse...", "djmw", 20050709)
INTRO ("Reverse the elements in the given range.")
ENTRY ("Arguments")
TAG ("%%Range")
DEFINITION ("defines the range of elements that will be reversed.")
ENTRY ("Examples")
NORMAL ("1. With the %range [0,0], the permutation (1,2,3,4,5) will generate (5,4,3,2,1). ")
NORMAL ("2. With the %range [3,0], the permutation (1,2,3,4,5) will generate (1,2,5,4,3). ")
MAN_END

MAN_BEGIN ("Permutation: Swap one from range...", "djmw", 20050709)
INTRO ("An element, randomly chosen from a range, will be permuted with an element at a given index position.")
ENTRY ("Arguments")
TAG ("%%Range")
DEFINITION ("defines the range of indices from which one will be randomly chosen.")
TAG ("%%Index%")
DEFINITION ("defines the special index position whose element will be interchanged with the one chosen from the range.")
TAG ("%%Forbid same")
DEFINITION ("when on, forbids the randomly chosen position and the index position to be the same. "
	"This switch is only of relevance when the chosen range happens to overlap the index position.")
ENTRY ("Examples")
NORMAL ("With range [0,0], an index of 3, \"Forbid same\" %off and (1,2,3,4,5) as the starting permutation, the outcome might be one of "
	"the five permutations (3,2,1,4,5), (1,3,2,4,5), (1,2,3,4,5), (1,2,4,3,5), (1,2,5,4,3). If \"Forbid same\" were chosen as %on, the "
	"(1,2,3,4,5) permutation is forbidden and the only outcome could be one of other four permutations.")
MAN_END

MAN_BEGIN ("Permutation: Permute randomly...", "djmw", 20050709)
INTRO ("Generates a random permutation for the elements in the given range.")
ENTRY ("Arguments")
TAG ("%%Range")
DEFINITION ("defines the range of elements that will be permuted.")
ENTRY ("Example")
NORMAL ("With a range of [3,6], the permutation (1,2,3,4,5,6,7) generates one of the 24 possible permutations, for example (1,2,(4,5,3,6),7).")
MAN_END

MAN_BEGIN ("Permutation: Permute randomly (blocks)...", "djmw", 20050709)
INTRO ("Generates a new @Permutation by randomly permuting blocks of size %blocksize.")
ENTRY ("Parameters")
TAG ("%%Range")
DEFINITION ("defines the range of elements whose blocks will be permuted.")
TAG ("%%Block size")
DEFINITION ("defines the size of the blocks that will be permuted. There must fit an integer number of blocks "
	"in the chosen range.")
TAG ("%%Permute within blocks%")
DEFINITION ("when %on, the elements in each block are also randomly permuted.")
TAG ("%%No doublets%")
DEFINITION ("guarantees that the first element in each block does not equal the last element of the previous block modulo "
	"the block size. E.g. the numbers 3, 6, 9 are all equal modulo 3. "
	"This parameter only has effect when \"Permute within blocks\" is %on.")
ENTRY ("Examples")
NORMAL ("1. With range [0,0], a block size of 3 and \"Permute within blocks\" %off the permutation ((1,2,3),(4,5,6),(7,8,9)) "
	"generates one of the six posible permutations, for example ((4,5,6),(7,8,9),(1,2,3)). (The option \"No doublets\" will be ignored and the parentheses are only there to indicate the blocks.)")
NORMAL ("2. With range [0,0], a block size of 3, \"Permute within blocks\" %on and \"No doublets\" %off, "
	"the permutation ((1,2,3),(4,5,6),(7,8,9)) might generate ((5,4,6),(9,8,7),(3,1,2)).")
NORMAL ("3. With the same options as 2 but \"No doublets\" %on, the previously given outcome is forbidden because "
	"the last element of the first block (6) and the first element of the next block (9) are equal modulo 3 (the "
	"blocksize). A valid outcome might then be ((5,4,6),(8,9,7),(3,1,2)).")
MAN_END

MAN_BEGIN ("Permutation: Swap one pair...", "djmw", 20050714)
INTRO ("A command to swap the elements at two positions in the selected @Permutation.")
ENTRY ("Parameters")
TAG ("%First, %Second")
DEFINITION ("define the two index positions whose elements are to be swapped.")
ENTRY ("Example")
NORMAL ("With %first = 1 and %second = 4, the permutation (1,2,3,4,5) will be modified to (4,2,3,1,5).")
MAN_END

MAN_BEGIN ("Permutation: Interleave...", "djmw", 20050714)
INTRO ("Generates a new @Permutation by interleaving elements from successive blocks. ")
NORMAL ("We always start with the first element in the first block. When the offset is zero, the next element will be the first "
	"element of the second block, then the first element of the third block. After the first element of the last block, we start again "
	"with the second elements in each block. And so on. (In card playing, with two blocks of 26 cards each, this is called a faro "
	"shuffle and eight successive faro shuffles will return the deck to precisely the order in which you began.)")
NORMAL ("If the offset differs from zero and equals 1 for example, we start with the first element in the first block, then the "
	"second element in the second block, the third element in the third block and so on. When the last element of a block is reached "
	"and the number of blocks is not exhausted the next element will be the first from the next block. When the last block is reached, "
	"we start the same cycle again with the next lower element in the first block (which by the way need not be the second element, "
	"see also example 4).")
ENTRY ("Parameters")
TAG ("%%Range%")
DEFINITION ("defines the range of elements that will be permuted.")
TAG ("%%Block size%")
DEFINITION ("defines the size of a block. There must fit an integer number of blocks "
	"in the chosen range.")
TAG ("%%Offset%")
DEFINITION ("defines the relative positions of selected elements in successive blocks.")
ENTRY ("Examples")
NORMAL ("1. With range [0,0], a block size of 3, and an offset of 0, the permutation ((1,2,3),(4,5,6),(7,8,9)) will generate (1,4,7,2,5,8,3,6,9).")
NORMAL ("2. With range [0,0], a block size of 3, and an offset of 1, the permutation ((1,2,3),(4,5,6),(7,8,9)) will generate (1,5,9,2,6,7,3,4,8).")
NORMAL ("3. With range [0,0], a block size of 3, and an offset of 2, the permutation ((1,2,3),(4,5,6),(7,8,9)) will generate (1,6,8,2,4,9,3,5,7).")
NORMAL ("4. With range [0,0], a block size of 4, and an offset of 1, the permutation ((1,2,3,4),(5,6,7,8)) will generate (1,6,3,8,2,7,4,5).")
MAN_END

MAN_BEGIN ("Permutation: Rotate...", "djmw", 20050714)
INTRO ("A circular shift of all elements within the given range.")
ENTRY ("Parameters")
TAG ("%%Range")
DEFINITION ("defines the range of elements that will be circularly permuted.")
TAG ("%%Step size")
DEFINITION ("define how many positions each element will be shifted.")
ENTRY ("Examples")
NORMAL ("1. With a step of size 2 and range [1,5], the permutation (1,2,3,4,5) will generate (4,5,1,2,3). ")
NORMAL ("2. With a step of size 2 and range [2,5], the permutation ((1),(2,3,4,5)) will generate ((1),(4,5,2,3))")
NORMAL ("3. With a step of size -1 and range [0,0], the permutation (1,2,3,4,5) will generate (2,3,4,5,1).")
MAN_END

MAN_BEGIN ("Permutation: Invert", "djmw", 20050709)
INTRO ("Generates the inverse of the selected @Permutation.")
ENTRY ("Example")
NORMAL ("If the permutation is (1,5,3,2,4) the generated inverse will be (1,4,3,5,2). If we @@Permutations: Multiply|multiply@ these two permutations the result will be the identity permutation (1,2,3,4,5).")
MAN_END

MAN_BEGIN ("Permutations: Multiply", "djmw", 20050717)
INTRO ("Apply the selected @@Permutation@s one after the other. ")
NORMAL ("Permutations are %not commutative, i.e. applying permutation %p__1_ after %p__2_ might not give the same outcome as applying "
	"%p__2_ after %p__1_.")
MAN_END

MAN_BEGIN ("Permutation: Sort", "djmw", 20050709)
INTRO ("Sorts the elements ascending, i.e. set the selected @@Permutation@ to the @@identity permutation@.")
MAN_END

MAN_BEGIN ("TableOfReal & Permutation: Permute rows", "djmw", 20050709)
INTRO ("Generate a new @TableOfReal with a row ordering determined by the @Permutation.")
ENTRY ("Example")
NORMAL ("If the selected TableOfReal has 5 rows and the permutation is (5,4,3,2,1) the first row of the new TableOfReal equals the fifth row of the selected, the second row of new equals the fourth row of the selected and so on.")
MAN_END

MAN_BEGIN ("Strings & Permutation: Permute strings", "djmw", 20050714)
INTRO ("Generate a new @Strings with a strings ordering determined by the @Permutation.")
NORMAL ("The number of strings in the #Strings and the number of elements in the #Permutation have to be equal.")
ENTRY ("Examples")
NORMAL ("1. If the selected Strings has the 4 strings \"heed\", \"hid\", \"hood\", \"hud\", and the permutation is (4,3,2,1) the new Strings has the strings \"hud\", \"hood\", \"hid\", \"heed\".")
NORMAL ("2. In the example that is discussed in the @@ExperimentMFC|listening experiment@ section, we have four stimuli \"heed.wav\", "
	"\"hid.wav\", \"hood.wav\", \"hud.wav\" that we want to present three times to each subject with a <PermuteBalancedNoDoublets> "
	"randomization strategy, i.e. stimuli presented in blocks of four, randomized, and no two successive stimuli equal. "
	"This type of randomization can easily be accomplished with a Permutation object and a Strings." )
LIST_ITEM ("1. Fill the Strings object with 12 strings, i.e. three repetitions of the four stimuli. ")
LIST_ITEM ("2. Create a Permutation object with 12 elements and perform  ##@@Permutation: Permute randomly (blocks)...|Permute randomly (blocks)...@ 0 0 4 yes yes#.")
LIST_ITEM ("3. Select the Strings and the Permutation together and chose ##Permute strings#.")
MAN_END

MAN_BEGIN ("Strings: To Permutation (sort)", "djmw", 20050714)
INTRO ("Generates the @Permutation with an element ordering that can be used to sort the String (alphabetically ascending).")
MAN_END

}
/* End of file manual_David.c */

