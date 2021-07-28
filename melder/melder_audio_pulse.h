#pragma once
/* melder_audio_pulse.h
 *
 * Copyright (C) 2015-2020 David Weenink
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
 
#if defined HAVE_PULSEAUDIO
	#include <pulse/pulseaudio.h>

#include "melder.h"

#define PA_GETTINGINFO 1
#define PA_GETTINGINFO_DONE 2
#define PA_WRITING 4
#define PA_WRITING_DONE 8
#define PA_RECORDING 16
#define PA_RECORDING_DONE 32
#define PA_QUERY_NUMBEROFCHANNELS 64
#define PA_QUERY_NUMBEROFCHANNELS_DONE 128


#endif
