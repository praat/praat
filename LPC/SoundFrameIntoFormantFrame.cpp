/* SoundFrameIntoFormantFrame.cpp
 *
 * Copyright (C) 2024-2025 David Weenink
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

#include "SoundFrameIntoFormantFrame.h"

#include "oo_DESTROY.h"
#include "SoundFrameIntoFormantFrame_def.h"
#include "oo_COPY.h"
#include "SoundFrameIntoFormantFrame_def.h"
#include "oo_EQUAL.h"
#include "SoundFrameIntoFormantFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundFrameIntoFormantFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundFrameIntoFormantFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundFrameIntoFormantFrame_def.h"
#include "oo_READ_TEXT.h"
#include "SoundFrameIntoFormantFrame_def.h"
#include "oo_READ_BINARY.h"
#include "SoundFrameIntoFormantFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundFrameIntoFormantFrame_def.h"


Thing_implement (SoundFrameIntoFormantFrame, SoundFrameIntoSampledFrame, 0);

void structSoundFrameIntoFormantFrame :: allocateOutputFrames () {
	soundIntoLPC -> allocateOutputFrames ();
	lpcIntoFormant -> allocateOutputFrames ();
}

void structSoundFrameIntoFormantFrame :: getInputFrame (void) {
	soundIntoLPC -> getInputFrame ();
}

bool structSoundFrameIntoFormantFrame :: inputFrameToOutputFrame () {
	bool step1 = soundIntoLPC -> inputFrameToOutputFrame ();
	soundIntoLPC -> saveOutputFrame ();
	lpcIntoFormant -> getInputFrame ();
	bool step2 = lpcIntoFormant -> inputFrameToOutputFrame ();
	return step1 && step2;
}

void structSoundFrameIntoFormantFrame :: saveOutputFrame () {
	lpcIntoFormant -> saveOutputFrame ();
}

autoSoundFrameIntoFormantFrame SoundFrameIntoFormantFrame_create (SoundFrameIntoLPCFrame silpc, LPCFrameIntoFormantFrame lpcif) {
	try {
		autoSoundFrameIntoFormantFrame me = Thing_new (SoundFrameIntoFormantFrame);
		my soundIntoLPC.adoptFromAmbiguousOwner (silpc);
		my lpcIntoFormant.adoptFromAmbiguousOwner (lpcif);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundFrameIntoFormantFrame.");
	}
}

/* End of file SoundFrameIntoFormantFrame.cpp */
