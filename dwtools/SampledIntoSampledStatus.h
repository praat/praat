#ifndef _SampledIntoSampledStatus_h_
#define _SampledIntoSampledStatus_h_
/* SampledIntoSampledStatus.h
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


#include "Data.h"
#include "melder.h"
#include "Sampled.h"
#include "SampledIntoSampledStatus.h"

#include "SampledIntoSampledStatus_def.h"

void SampledIntoSampledStatus_init (integer numberOfFrames);
autoSampledIntoSampledStatus SampledIntoSampledStatus_create (integer numberOfFrames);

void SoundIntoSampledStatus_init (integer numberOfFrames);
autoSoundIntoSampledStatus SoundIntoSampledStatus_create (integer numberOfFrames);


Thing_define (SoundIntoLPCStatus, SoundIntoSampledStatus) {
};

void SoundIntoLPCStatus_init (SoundIntoSampledStatus me, integer numberOfFrames);
autoSoundIntoLPCStatus SoundIntoLPCStatus_create (integer numberOfFrames);


Thing_define (SoundIntoPowerCepstrogramStatus, SoundIntoSampledStatus) {
};

autoSoundIntoPowerCepstrogramStatus SoundIntoPowerCepstrogramStatus_create (integer numberOfFrames);


Thing_define (PowerCepstrogramIntoMatrixStatus, SampledIntoSampledStatus) {
		autoVEC slopes, intercepts;
		autoINTVEC startFrames, numberOfTries;
		void showStatus () override;
};

autoPowerCepstrogramIntoMatrixStatus PowerCepstrogramIntoMatrixStatus_create (integer numberOfFrames);


Thing_define (LPCIntoFormantStatus, SampledIntoSampledStatus) {};

autoLPCIntoFormantStatus LPCIntoFormantStatus_create (integer numberOfFrames);



Thing_define (LPCAndSoundIntoLPCRobustStatus, SampledIntoSampledStatus) {	
};

autoLPCAndSoundIntoLPCRobustStatus LPCAndSoundIntoLPCRobustStatus_create (integer numberOfFrames);


Thing_define (SoundIntoLPCRobustStatus, SampledIntoSampledStatus) {
	autoSoundIntoLPCStatus soundIntoLPCStatus;
	autoLPCAndSoundIntoLPCRobustStatus lpcAndSoundIntoLPCRobustStatus;
};

autoSoundIntoLPCRobustStatus SoundIntoLPCRobustStatus_create (integer numberOfFrames);


Thing_define (SoundIntoFormantStatus, SampledIntoSampledStatus) {
	autoSoundIntoLPCStatus soundIntoLPCStatus;
	autoLPCIntoFormantStatus lpcIntoFormantStatus;
};

autoSoundIntoFormantStatus SoundIntoFormantStatus_create (integer numberOfFrames);


Thing_define (SoundIntoFormantRobustStatus, SampledIntoSampledStatus) {
	autoSoundIntoLPCRobustStatus soundIntoLPCRobustStatus;
	autoLPCIntoFormantStatus lpcIntoFormantStatus;
};

autoSoundIntoFormantRobustStatus SoundIntoFormantRobustStatus_create (integer numberOfFrames);


Thing_define (SoundAndLPCIntoFormantRobustStatus, SampledIntoSampledStatus) {
	autoLPCAndSoundIntoLPCRobustStatus lpcAndSoundIntoLPCRobustStatus;
	autoLPCIntoFormantStatus lpcIntoFormantStatus;
};

autoSoundAndLPCIntoFormantRobustStatus SoundAndLPCIntoFormantRobustStatus_create (integer numberOfFrames);

#endif /* _SampledIntoSampledStatus_h_ */	
