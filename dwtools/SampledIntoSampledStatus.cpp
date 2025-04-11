/* SampledIntoSampledStatus.cpp
 *
 * Copyright (C) 2025 David Weenink
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


#include "SampledIntoSampledStatus.h"
#include "melder.h"

#include "oo_DESTROY.h"
#include "SampledIntoSampledStatus_def.h"
#include "oo_COPY.h"
#include "SampledIntoSampledStatus_def.h"
#include "oo_EQUAL.h"
#include "SampledIntoSampledStatus_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SampledIntoSampledStatus_def.h"
#include "oo_WRITE_TEXT.h"
#include "SampledIntoSampledStatus_def.h"
#include "oo_WRITE_BINARY.h"
#include "SampledIntoSampledStatus_def.h"
#include "oo_READ_TEXT.h"
#include "SampledIntoSampledStatus_def.h"
#include "oo_READ_BINARY.h"
#include "SampledIntoSampledStatus_def.h"
#include "oo_DESCRIPTION.h"
#include "SampledIntoSampledStatus_def.h"

Thing_implement (SampledIntoSampledStatus, Daata, 0);

void SampledIntoSampledStatus_init (SampledIntoSampledStatus me, integer numberOfFrames) {
	my numberOfFrames = numberOfFrames;
	my frameIntoFrameInfo = zero_INTVEC (numberOfFrames);
}

Thing_implement (SoundIntoSampledStatus, SampledIntoSampledStatus, 0);

void SoundIntoSampledStatus_init (SoundIntoSampledStatus me, integer numberOfFrames) {
	SampledIntoSampledStatus_init (me, numberOfFrames);
	my soundFrameBegins = zero_INTVEC (numberOfFrames);
}


/* End of file SampledIntoSampledStatus.cpp */	
