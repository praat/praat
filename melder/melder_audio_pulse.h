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

typedef struct pulseAudio {
	pa_sample_spec sample_spec;
	pa_threaded_mainloop *mainloop = nullptr;
	pa_mainloop_api *mainloop_api = nullptr;
	pa_context *context = nullptr;
	pa_stream *stream = nullptr;
	pa_operation *operation_drain = nullptr;
	pa_operation *operation_info = nullptr;
	pa_stream_flags_t stream_flags;
	const pa_timing_info *timing_info = nullptr;
	struct timeval startTime = {0, 0};
	pa_usec_t timer_event_usec = 50000; // 50 ms
	pa_time_event *timer_event = nullptr;
	const pa_channel_map *channel_map = nullptr;
	pa_usec_t r_usec;
	pa_buffer_attr buffer_attr;
	uint32 latency = 0; // in bytes of buffer
	int32 latency_msec = 20;
	bool pulseAudioInitialized = false;
	unsigned int occupation = PA_WRITING;
} pulseAudioStruct;

#endif
