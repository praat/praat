#ifndef _MixingMatrix_h_
#define _MixingMatrix_h_
/* MixingMatrix.h
 *
 * Copyright (C) 2010-2017 David Weenink
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

#include "TableOfReal.h"

Thing_define (MixingMatrix, TableOfReal) {
};

autoMixingMatrix MixingMatrix_create (integer numberOfOutputChannels, integer numberOfInputChannels);

autoMixingMatrix MixingMatrix_createSimple (integer numberOfOutputChannels, integer numberOfInputChannels, conststring32 elements);

void MixingMatrix_multiplyInputChannel (MixingMatrix me, integer inputChannel, double value);

void MixingMatrix_muteAndActivateChannels (MixingMatrix me, constBOOLVEC const& muteChannels);

void MixingMatrix_setStandardChannelInterpretation (MixingMatrix me);

void MixingMatrix_setRandomGauss (MixingMatrix me, double mean, double stdev);


#endif
/* End of file MixingMatrix.h */
