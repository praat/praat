#ifndef _SoundSet_h_
#define _SoundSet_h_
/* SoundSet.h
 *
 * Copyright (C) 2019 Paul Boersma
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

#include "Sound.h"
#include "Table.h"
#include "PatternList.h"

/*
	Abstract.
*/
Collection_define (SoundSet, OrderedOf, Sound) {
};

integer SoundSet_getMinimumNumberOfSamples (SoundSet me);
autoMAT SoundSet_getRandomizedPatterns (SoundSet me, integer numberOfPatterns, integer patternSize);
void SoundSet_Table_getRandomizedPatterns (SoundSet me, Table thee, conststring32 columnName, integer numberOfPatterns, integer inputSize, integer outputSize,
	autoPatternList *out_inputs, autoPatternList *out_outputs);

/* End of file Sound.h */
#endif
