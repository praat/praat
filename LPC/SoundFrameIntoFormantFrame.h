#ifndef _SoundFrameIntoFormantFrame_h_
#define _SoundFrameIntoFormantFrame_h_
/* SoundFrameIntoFormantFrame.h
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

#include "Formant.h"
#include "LPC.h"
#include "Sound.h"
#include "LPCFrameIntoFormantFrame.h"
#include "SoundFrameIntoLPCFrame.h"

#include "SoundFrameIntoFormantFrame_def.h"

autoSoundFrameIntoFormantFrame SoundFrameIntoFormantFrame_create (autoSoundFrameIntoLPCFrame silpc, autoLPCFrameIntoFormantFrame lpcif);

#endif /*_SoundFrameIntoFormantFrame_h_ */

