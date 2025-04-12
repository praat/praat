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

Thing_implement (SoundIntoLPCStatus, SoundIntoSampledStatus, 0);

autoSoundIntoLPCStatus SoundIntoLPCStatus_create (integer numberOfFrames) {
	try {
		autoSoundIntoLPCStatus me = Thing_new (SoundIntoLPCStatus);
		SoundIntoSampledStatus_init (me.get(), numberOfFrames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundIntoLPCStatus.");
	}
}

Thing_implement (SoundIntoCepstrogramStatus, SoundIntoSampledStatus, 0);

autoSoundIntoCepstrogramStatus SoundIntoCepstrogramStatus_create (integer numberOfFrames) {
	try {
		autoSoundIntoCepstrogramStatus me = Thing_new (SoundIntoCepstrogramStatus);
		SoundIntoSampledStatus_init (me.get(), numberOfFrames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundIntoCepstrogramStatus.");
	}
}


Thing_implement (PowerCepstrogramIntoMatrixStatus, SampledIntoSampledStatus, 0);

autoPowerCepstrogramIntoMatrixStatus PowerCepstrogramIntoMatrixStatus_create (integer numberOfFrames) {
	try {
		autoPowerCepstrogramIntoMatrixStatus me = Thing_new (PowerCepstrogramIntoMatrixStatus);
		SampledIntoSampledStatus_init (me.get(), numberOfFrames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create PowerCepstrogramIntoMatrixStatus.");
	}
	
}


Thing_implement (LPCIntoFormantStatus, SampledIntoSampledStatus, 0);

autoLPCIntoFormantStatus LPCIntoFormantStatus_create (integer numberOfFrames) {
	try {
		autoLPCIntoFormantStatus me = Thing_new (LPCIntoFormantStatus);
		SampledIntoSampledStatus_init (me.get(), numberOfFrames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create LPCIntoFormantStatus.");
	}
}


Thing_implement (LPCAndSoundIntoLPCRobustStatus, SampledIntoSampledStatus, 0);

autoLPCAndSoundIntoLPCRobustStatus LPCAndSoundIntoLPCRobustStatus_create (integer numberOfFrames) {
	try {
		autoLPCAndSoundIntoLPCRobustStatus me = Thing_new (LPCAndSoundIntoLPCRobustStatus);
		SampledIntoSampledStatus_init (me.get(), numberOfFrames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create LPCAndSoundIntoLPCRobustStatus.");
	}
}

Thing_implement (SoundIntoLPCRobustStatus, SampledIntoSampledStatus, 0);

autoSoundIntoLPCRobustStatus SoundIntoLPCRobustStatus_create (integer numberOfFrames) {
	try {
		autoSoundIntoLPCRobustStatus me = Thing_new (SoundIntoLPCRobustStatus);
		my soundIntoLPCStatus = SoundIntoLPCStatus_create (numberOfFrames);
		my lpcAndSoundIntoLPCRobustStatus = LPCAndSoundIntoLPCRobustStatus_create (numberOfFrames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create LPCAndSoundIntoLPCRobustStatus.");
	}
}

Thing_implement (SoundIntoFormantStatus, SampledIntoSampledStatus, 0);

autoSoundIntoFormantStatus SoundIntoFormantStatus_create (integer numberOfFrames) {
	try {
		autoSoundIntoFormantStatus me = Thing_new (SoundIntoFormantStatus);
		my soundIntoLPCStatus = SoundIntoLPCStatus_create (numberOfFrames);
		my lpcIntoFormantStatus = LPCIntoFormantStatus_create (numberOfFrames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create LPCAndSoundIntoLPCRobustStatus.");
	}
}


Thing_implement (SoundIntoFormantRobustStatus, SampledIntoSampledStatus, 0);

autoSoundIntoFormantRobustStatus SoundIntoFormantRobustStatus_create (integer numberOfFrames) {
	try {
		autoSoundIntoFormantRobustStatus me = Thing_new (SoundIntoFormantRobustStatus);
		my soundIntoLPCRobustStatus = SoundIntoLPCRobustStatus_create (numberOfFrames);
		my lpcIntoFormantStatus = LPCIntoFormantStatus_create (numberOfFrames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundIntoFormantRobustStatus.");
	}
}


Thing_implement (SoundAndLPCIntoFormantRobustStatus, SampledIntoSampledStatus, 0);

autoSoundAndLPCIntoFormantRobustStatus SoundAndLPCIntoFormantRobustStatus_create (integer numberOfFrames) {
	try {
		autoSoundAndLPCIntoFormantRobustStatus me = Thing_new (SoundAndLPCIntoFormantRobustStatus);
		my lpcAndSoundIntoLPCRobustStatus = LPCAndSoundIntoLPCRobustStatus_create (numberOfFrames);
		my lpcIntoFormantStatus = LPCIntoFormantStatus_create (numberOfFrames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundAndLPCIntoFormantRobustStatus.");
	}
}

/* End of file SampledIntoSampledStatus.cpp */	
