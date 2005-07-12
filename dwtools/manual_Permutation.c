/* manual_Permutation.c */
/* David Weenink, 9 juli 2005 */

/*
	20050713 Latest modification
*/
#include "ManPagesM.h"

void manual_Permutation_init (ManPages me);
void manual_Permutation_init (ManPages me)
{

MAN_BEGIN ("Permutation", "djmw", 20050709)
INTRO ("One of the @@types of objects@ in P\\sc{raat}. A Permutation object with %n elements consists of some ordering of the numbers 1,2...%n.")
ENTRY ("Commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Create Permutation...@")
NORMAL ("Query")
LIST_ITEM ("##Get number of elements#")
LIST_ITEM ("@@Permutation: Get value...|Get value...@")
NORMAL ("Modification")
LIST_ITEM ("")
NORMAL ("")
LIST_ITEM ("@@Permutation: Permute one...|Permute one...@")
LIST_ITEM ("@@Permutation: Permute randomly...|Permute randomly...@")
LIST_ITEM ("@@Permutation: Permute randomly (blocks)...|Permute randomly (blocks)...@")
LIST_ITEM ("@@Permutation: Permute (stepdown blocks)...|Permute (stepdown blocks)...@")
LIST_ITEM ("@@Permutation: Cycle...|Cycle...@")
LIST_ITEM ("@@Permutation: Reverse...|Reverse...@")
LIST_ITEM ("@@Permutation: Invert|Invert@")
NORMAL ("Successive permutations")
LIST_ITEM ("@@Permutations: Multiply|Multiply@")
MAN_END

MAN_BEGIN ("Create Permutation...", "djmw", 20050709)
INTRO ("A command to create a @Permutation of the numbers 1,2,...%numberOfElements.")
ENTRY ("Arguments")
TAG ("%Name")
DEFINITION ("determines the name of the new permutation.")
TAG ("%%Number of elements%")
DEFINITION ("determines the number of elements in the permutation.")
TAG ("%%Identity Permutation")
NORMAL ("determines whether the permution will be a randomly chosen one or the @@identity permutation@.")
MAN_END

MAN_BEGIN ("identity permutation", "djmw", 20050713)
INTRO ("The identity permutation consist of the numbers "
	"1, 2,...%numberOfElements in their natural order. ")
MAN_END

MAN_BEGIN ("Permutation: Get value...", "djmw", 20050709)
INTRO ("Get the value of the chosen element.")
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

MAN_BEGIN ("Permutation: Permute one...", "djmw", 20050709)
INTRO ("An element, randomly chosen from a range, will be permuted with an element at a given index position.")
ENTRY ("Arguments")
TAG ("%%Range")
DEFINITION ("defines the range of indices from which one will be randomly chosen.")
TAG ("%%Index%")
DEFINITION ("defines the special index position whose element will be interchanged with the one chosen from the range.")
TAG ("%%Forbid same")
DEFINITION ("when on, forbids the randomly chosen position and the index position to be the same. "
	"This switch is only of relevance when the chosen range happens to overlap the index position.")
MAN_END

MAN_BEGIN ("Permutation: Permute randomly...", "djmw", 20050709)
INTRO ("Generates a random permutation for the elements in the given range.")
ENTRY ("Arguments")
TAG ("%%Range")
DEFINITION ("defines the range of elements that will be permuted.")
ENTRY ("Example")
NORMAL ("With a range of [3,6], the permutation (1,2,3,4,5,6,7) might generate (1,2,(4,5,3,6),7).")
MAN_END

MAN_BEGIN ("Permutation: Permute randomly (blocks)...", "djmw", 20050709)
INTRO ("Generates a new @Permutation by randomly permuting blocks of size %blocksize.")
ENTRY ("Parameters")
TAG ("%%Range")
DEFINITION ("defines the range of elements whose blocks will be permuted.")
TAG ("%%Block size")
DEFINITION ("define the size of the blocks that will be permuted. There must fit an integer number of blocks "
	"in the chosen range.")
TAG ("%%Permute within blocks%")
DEFINITION ("when %on, the elements in each block are also randomly permuted.")
TAG ("%%No doublets%")
DEFINITION ("guarantees that the first element in each block does not equal the last element of the previous block modulo the block size. E.g. the numbers 3, 6, 9 are all equal modulo 3. This parameter only has effect when \"Permute within blocks\" is %on.")
ENTRY ("Examples")
NORMAL ("1. With range [0,0], a block size of 3 and \"Permute within blocks\" %off the permutation ((1,2,3),(4,5,6),(7,8,9)) "
	"might generate ((4,5,6),(7,8,9),(1,2,3)). (The option \"No doublets\" will be ignored and the parenthesis are only there to indicate the blocks.)")
NORMAL ("2. With range [0,0], a block size of 3, \"Permute within blocks\" %on and \"No doublets\" %off, "
	"the permutation ((1,2,3),(4,5,6),(7,8,9)) might generate ((5,4,6),(9,8,7),(3,1,2)).")
NORMAL ("3. With the same options as 2 but \"No doublets\" %on, the previously given outcome is forbidden because "
	"the last element of the first block (6) and the first element of the next block (9) are equal modulo 3 (the "
	"blocksize). A valid outcome might then be ((5,4,6),(8,9,7),(3,1,2)).")
MAN_END

MAN_BEGIN ("Permutation: Permute (stepdown blocks)...", "djmw", 20050709)
INTRO ("Generates a new @Permutation by, starting with the first element in the first block, taking the second "
	"element from the second block, the third element from the third block and so on. When the last element of "
	"a block is reached and the number of blocks is not exhausted the next element will be the first in the "
	"next block. When the last block is reached, we start the same cycle again with the next lower element in "
	"the first block (which by the way need not be second element, see also example 2).")
ENTRY ("Parameters")
TAG ("%%Range")
DEFINITION ("defines the range of elements that will be permuted.")
TAG ("%%Block size")
DEFINITION ("define the size of a block. There must fit an integer number of blocks "
	"in the chosen range.")
ENTRY ("Examples")
NORMAL ("1. With range [0,0] and a block size of 3, the permutation ((1,2,3),(4,5,6),(7,8,9)) will generate (1,5,9,2,6,7,3,4,8)")
NORMAL ("2. With range [0,0] and a block size of 4, the permutation ((1,2,3,4),(5,6,7,8)) will generate (1,6,3,8,2,7,4,5).")
MAN_END

MAN_BEGIN ("Permutation: Cycle...", "djmw", 20050709)
INTRO ("A circular shift of all elements within the given range.")
ENTRY ("Parameters")
TAG ("%%Range")
DEFINITION ("defines the range of elements that will be circurlarly permuted.")
TAG ("%%Step size")
DEFINITION ("define how many positions each element will be shifted.")
ENTRY ("Examples")
NORMAL ("1. With a step of size 2 and range [1,5], the permutation (1,2,3,4,5) will generate (4,5,1,2,3). ")
NORMAL ("2. With a step of size 2 and range [2,5], the permutation ((1),(2,3,4,5)) will generate ((1),(4,5,2,3))")
MAN_END

MAN_BEGIN ("Permutation: Invert", "djmw", 20050709)
INTRO ("Generates the inverse of the selected @Permutation.")
ENTRY ("Example")
NORMAL ("If the permutation is (1,5,3,2,4) the generated inverse will be (1,4,3,5,2). If we @@Permutations: Multiply|multiply@ these two permutations the result will be the identity permutation (1,2,3,4,5).")
MAN_END

MAN_BEGIN ("Permutations: Multiply", "djmw", 20050709)
INTRO ("Apply the selected @@Permutation@s successively.")
MAN_END

MAN_BEGIN ("Permutation: Set to identity", "djmw", 20050709)
INTRO ("Set the selected @@Permutation@ to the @@identity permutation@.")
MAN_END

MAN_BEGIN ("TableOfReal & Permutation: Permute rows", "djmw", 20050709)
INTRO ("Generate a new @TableOfReal with a row ordering detemined by the @Permutation.")
ENTRY ("Example")
NORMAL ("If the selected TableOfReal has 5 rows and the permutation is (5,4,3,2,1) the first row of the new TableOfReal equals the fifth row of the selected, the second row of new equals the fourth row of the selected and so on.")
MAN_END

}
/* End of file manual_David.c */

