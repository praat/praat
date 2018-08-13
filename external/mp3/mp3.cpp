/*
 * Praat wrappers for libMAD (MPEG Audio Decoder) Copyright 2007 Erez Volk
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
 *
 *
 *
 * Exact seeking in an MP3 file turns out to be quite complex.
 *
 * For constant bit rate (CBR) files, frame size is fixed and we can calculate
 * in advance the location of every frame.
 *
 * For variable bit rate (VBR) files, no option gives us more than a rough
 * estimate of where every percent of the file is lcoated, with no way of
 * checking (after we make a seek) where we ended up.  The only reliable way we
 * have, then, is to scan the entire file in advance and keep a table with the
 * offsets of all (or some) frames.  In the worst case, we will have to scan
 * the file *twice*, since we can't know the exact number of frames.
 *
 * What's worse, there is no guaranteed standard way of checking whether an MP3
 * file is CBR or VBR.  We therefore use the following compromise, which is
 * guaranteed to work on all files:
 *
 * - If there is a Xing header, read it to get the number of frames.
 * - Otherwise *estimate* the number of frames.
 * - Keep a reasonably (?) sized table of "key" offsets.
 * - Scan all the headers and keep required offsets in the table.
 * - After the scan, we also know the precise number of frames and samples.
 *
 * TODO: Find exactly what the encoder delay is.
 *       (see http://mp3decoders.mp3-tech.org/decoders_lame.html)
 * TODO: Compensate for end padding.
 * TODO: Better conversion to short.
 *
 */

/*#define MP3_DEBUG*/

#include "mp3.h"

extern "C" {
	#include "mad_config.h"
	#include "mad_decoder.h"
}

#define MP3F_BUFFER_SIZE (8 * 1024)
#define MP3F_MAX_LOCATIONS 1024

/*
 * MP3 encoders and decoders add a number of silent samples at the beginning.
 * I don't know of any reliable way to detect these delays in an MP3 file.
 * The numbers below are (hopefully reasonable) estimates based on LAME and
 * some testing; it is more important to preserve all the meaningful samples
 * than to eliminate all silent ones.
 */
#define MP3F_DECODER_DELAY 529
/*#define MP3F_ENCODER_DELAY 576*/
#define MP3F_ENCODER_DELAY 96

#ifdef MP3_DEBUG
#   define MP3_DPRINTF(x) printf x
#   define MP3_PERCENT(base, value) (((float)(value) - (float)(base)) * 100.0 / (float)(base))
#else /* !MP3_DEBUG */
#   define MP3_DPRINTF(x)
#endif /* MP3_DEBUG */

struct _MP3_FILE
{
	struct mad_decoder decoder;

	FILE *f;
	unsigned char buffer [MP3F_BUFFER_SIZE];

	int xing;

	unsigned channels;
	unsigned frequency;
	unsigned frames;
	unsigned samples_per_frame;
	MP3F_OFFSET samples;

	MP3F_OFFSET locations[MP3F_MAX_LOCATIONS]; 
	unsigned num_locations;
	unsigned frames_per_location;

	unsigned delay;

	MP3F_CALLBACK callback;
	void *context;

	MP3F_OFFSET next_read_position;
	MP3F_OFFSET read_amount;
	MP3F_OFFSET first_offset;
	unsigned skip_amount;
	int need_seek;
	MP3F_OFFSET id3TagSize_bytes; /* David Weenink */
};

static enum mad_flow mp3f_mad_error (void *context, struct mad_stream *stream, struct mad_frame *frame);
static enum mad_flow mp3f_mad_input (void *context, struct mad_stream *stream);
static enum mad_flow mp3f_mad_first_header (void *context, struct mad_header const *header);
static enum mad_flow mp3f_mad_first_filter (void *context,
		struct mad_stream const *stream,
		struct mad_frame *frame);
static int mp3f_check_xing (MP3_FILE mp3f, struct mad_stream const *stream);
static enum mad_flow mp3f_mad_scan_header (void *context, struct mad_header const *header);
static enum mad_flow mp3f_mad_report_samples (void *context, struct mad_header const *header, struct mad_pcm *pcm);

int mp3_recognize (int nread, const char *data)
{
	const unsigned char *bytes = (const unsigned char *)data;

	if (nread < 3)
		return 0;

	/* MP3 files can start with an ID3 tag */
	if (bytes [0] == 'I' && bytes [1] == 'D' && bytes [2] == '3')
		return 1;

	/* Otherwise the beginning of the file must be an MP3 frame */
	if (bytes [0] != 0xFF)
		return 0;

	/* This is not a foolproof check, but it is similar to file(1) */
	return
		((bytes [1] & 0xFE) == 0xFA) || /* MPEG ADTS, layer III, v1 */
		((bytes [1] & 0xFE) == 0xFC) || /* MPEG ADTS, layer II, v1 */
		((bytes [1] & 0xFE) == 0xFE) || /* MPEG ADTS, layer I, v1 */
		((bytes [1] & 0xFE) == 0xF2) || /* MPEG ADTS, layer III, v2 */
		((bytes [1] & 0xFE) == 0xF4) || /* MPEG ADTS, layer II, v2 */
		((bytes [1] & 0xFE) == 0xF6) || /* MPEG ADTS, layer I, v2 */
		((bytes [1] & 0xFE) == 0xE2);   /* MPEG ADTS, layer III, v2.5 */
}

MP3_FILE mp3f_new ()
{
	try {
		return Melder_calloc (struct _MP3_FILE, 1);
	} catch (MelderError) {
		Melder_throw (U"Cannot create MP3 file object.");
	}
}

void mp3f_delete (MP3_FILE mp3f)
{
	Melder_free (mp3f);
}

void mp3f_set_file (MP3_FILE mp3f, FILE *f)
{
	mp3f -> f = f;
	if (! f)
		return;
	fseek (f, 0, SEEK_SET);
	/* David Weenink 20151005
		Check if an ID3 header version 2 or newer is present at the START of the file (older header types are always at the END of the file). We calculate the size of the header (in bytes), store it in the mp3f -> id3TagSize_bytes field and simply skip this amount of bytes before analyzing/decoding starts.
		
		According to http://id3.org/id3v2-00:
		The ID3v2 tag header, which should be the FIRST information in the file, is 10 bytes as follows:

		ID3/file identifier      "ID3"
		ID3 version              $02 00
		ID3 flags                %xx000000
		ID3 size             4 * %0xxxxxxx

		The first three bytes of the tag are always "ID3" to indicate that
		this is an ID3 tag, directly followed by the two version bytes. The
		first byte of ID3 version is it's major version, while the second byte
		is its revision number. All revisions are backwards compatible while
		major versions are not. If software with ID3v2 and below support
		should encounter version three or higher it should simply ignore the
		whole tag. Version and revision will never be $FF.

		The first bit (bit 7) in the 'ID3 flags' is indicating whether or not
		unsynchronisation is used (see section 5 for details); a set bit
		indicates usage.

		The second bit (bit 6) is indicating whether or not compression is
		used; a set bit indicates usage. Since no compression scheme has been
		decided yet, the ID3 decoder (for now) should just ignore the entire
		tag if the compression bit is set.

		The ID3 tag size is encoded with four bytes where the first bit (bit
		7) is set to zero in every byte, making a total of 28 bits. The zeroed
		bits are ignored, so a 257 bytes long tag is represented as $00 00 02 01.

		The ID3 tag size is the size of the complete tag after
		unsychronisation, including padding, excluding the header (total tag
		size - 10). The reason to use 28 bits (representing up to 256MB) for
		size description is that we don't want to run out of space here.

		A ID3v2 tag can be detected with the following pattern:
		$49 44 33 yy yy xx zz zz zz zz
		Where yy is less than $FF, xx is the 'flags' byte and zz is less than$80.
	*/
	{
		unsigned char bytes [10];
		(void) fread (& bytes, 1, 10, mp3f -> f);
		mp3f -> id3TagSize_bytes = 0;
		if (bytes[0] == 'I' && bytes[1] == 'D' && bytes[2] == '3') {
			if (bytes[3] < 0xFF && bytes[4] < 0xFF &&
				bytes[6] < 0x80 && bytes[7] < 0x80 && bytes[8] < 0x80 && bytes[9] < 0x80 ) {
				/* 
				Ignore version: bytes[3] & bytes[4] 
				Ignore flags: bytes[5]
					The only purpose of the 'unsychronisation scheme' is to make the ID3v2
					tag as compatible as possible with existing software. There is no use
					in 'unsynchronising' tags if the file is only to be processed by new
					software. Unsynchronisation may only be made with MPEG 2 layer I, II
					and III and MPEG 2.5 files.
				 */
				mp3f -> id3TagSize_bytes = (bytes[6] << 21 | bytes[7] << 14 | bytes[8] << 7 | bytes[9]) + 10;
			}
		}
	}
	
	fseek (f, mp3f -> id3TagSize_bytes, SEEK_SET); // David Weenink

	mp3f -> next_read_position = 0;
	mp3f -> need_seek = 0;
	mp3f -> delay = MP3F_DECODER_DELAY + MP3F_ENCODER_DELAY;
	mp3f -> skip_amount = mp3f -> delay;
	mp3f -> first_offset = 0;
}

int mp3f_analyze (MP3_FILE mp3f)
{
	struct mad_decoder *decoder = & mp3f -> decoder;
	int status;
#ifdef MP3_DEBUG
	unsigned estimate, last;
#endif /* MP3_DEBUG */

	if (! mp3f || ! mp3f -> f)
		return 0;

	fseek (mp3f -> f, mp3f -> id3TagSize_bytes, SEEK_SET); // David Weenink

	mp3f -> xing = 0;
	mp3f -> channels = 0;
	mp3f -> frequency = 0;
	mp3f -> frames = 0;
	mp3f -> samples = 0;
	mp3f -> samples_per_frame = 0;
	mp3f -> num_locations = 0;

	/* Read first frames to get basic parameters and hopefully Xing */
	mad_decoder_init (decoder, 
			mp3f,
			mp3f_mad_input,
			mp3f_mad_first_header,
			mp3f_mad_first_filter,
			nullptr /* Output: Don't actually decode for now */,
			mp3f_mad_error,
			nullptr /* Message */);

	status = mad_decoder_run (decoder, MAD_DECODER_MODE_SYNC);
	if (status != 0)
		goto end;

	/*
	 * If we don't have a Xing header we need to estimate the frame count.
	 * This doesn't have to be accurate since we're going to count them
	 * later when we scan for header offsets.
	 */
	if (! mp3f -> xing) {
		MP3F_OFFSET file_size, frame_size;

		/* Take size of first frame */
		frame_size = mp3f -> locations [1] - mp3f -> locations[0];

		/* For file size, seek to end */
		fseek (mp3f -> f, mp3f -> id3TagSize_bytes, SEEK_END); // David Weenink
		file_size = ftell (mp3f -> f);

		/* This estimate will be pretty accurate for CBR */
		mp3f -> frames = file_size / frame_size;

		MP3_DPRINTF (("File size: %lu bytes\n", (unsigned long)file_size));
		MP3_DPRINTF (("First frame size: %lu bytes\n", (unsigned long)frame_size));
		MP3_DPRINTF (("Estimated frames: %lu\n", (unsigned long)mp3f -> frames));
	}

	/* Calculate how many frames can fit in a "location" */
	if (mp3f -> frames <= MP3F_MAX_LOCATIONS)
		mp3f -> frames_per_location = 1;
	else
		mp3f -> frames_per_location = (mp3f -> frames + MP3F_MAX_LOCATIONS - 1) / MP3F_MAX_LOCATIONS;

	MP3_DPRINTF (("MP3: Each location is %u frame(s) (%.3fs), each %u samples\n",
				mp3f -> frames_per_location,
				mp3f -> frames_per_location * mp3f -> samples_per_frame / (float)mp3f -> frequency,
				mp3f -> samples_per_frame));

	/* Read all frames to get offsets*/
#ifdef MP3_DEBUG
	estimate = mp3f -> frames;
#endif /* MP3_DEBUG */
	mp3f -> num_locations = 0;
	mp3f -> frames = 0;
	mp3f -> samples = 0;

	fseek (mp3f -> f, mp3f -> id3TagSize_bytes, SEEK_SET); // David Weenink
	mad_decoder_init (decoder, 
			mp3f,
			mp3f_mad_input,
			mp3f_mad_scan_header,
			nullptr /* Filter */,
			nullptr /* Output */,
			mp3f_mad_error,
			nullptr /* Message */);

	status = mad_decoder_run (decoder, MAD_DECODER_MODE_SYNC);

	MP3_DPRINTF (("MP3 Frames: %u, estimated %u (%+.2f%%)\n",
		       	mp3f -> frames,
		       	estimate,
			MP3_PERCENT (mp3f -> frames, estimate)));
#ifdef MP3_DEBUG
	last = mp3f -> frames - (mp3f -> frames_per_location * (mp3f -> num_locations - 1));
	MP3_DPRINTF (("MP3F: Last location frames = %u (%+.2f%%) = %.3fs\n",
				last,
				MP3_PERCENT (mp3f -> frames_per_location, last),
				last * mp3f -> samples_per_frame / (float)mp3f -> frequency));
#endif /* MP3_DEBUG */

if(status!=-1)   // ppgb 2015-01-17
	mp3f_seek (mp3f, 0);

end:
	mad_decoder_finish (decoder);

	return (status == 0);
}

unsigned mp3f_channels (MP3_FILE mp3f)
{
	return mp3f -> channels;
}

unsigned mp3f_frequency (MP3_FILE mp3f)
{
	return mp3f -> frequency;
}

MP3F_OFFSET mp3f_samples (MP3_FILE mp3f)
{
	return mp3f -> samples - mp3f -> delay;
}

void mp3f_set_callback (MP3_FILE mp3f,
	       	MP3F_CALLBACK callback, void *context)
{
	mp3f -> callback = callback;
	mp3f -> context = context;
}

int mp3f_seek (MP3_FILE mp3f, MP3F_OFFSET sample)
{
	MP3F_OFFSET frame, location, base, offset;

	if (! mp3f || ! mp3f -> f)
		return 0;

	if (! mp3f -> frames_per_location)
		if (! mp3f_analyze (mp3f))
			return 0;

	/* Compensate for initial empty frames */
	sample += mp3f -> delay;

	/* Calculate where we need to seek */
	frame = sample / mp3f -> samples_per_frame;
	if ( frame ) /* libMAD can skip the first frame... */
		-- frame; 
	if ( frame ) /* ...and the first frame it decodes is useless */
		-- frame; 
Melder_assert (mp3f -> frames_per_location > 0);
Melder_assert (mp3f -> num_locations > 0);
	location = frame / mp3f -> frames_per_location;
	if (location >= mp3f -> num_locations)
		location = mp3f -> num_locations - 1;
	frame = location * mp3f -> frames_per_location;
	base = frame * mp3f -> samples_per_frame;

Melder_assert (location >= 0);
	offset = mp3f -> locations [location];
	if (fseek (mp3f -> f, offset, SEEK_SET) < 0)
		return 0;

	mp3f -> first_offset = offset;
	mp3f -> skip_amount = sample - base;
	mp3f -> need_seek = 0;

	MP3_DPRINTF (("SEEK to %lu (%lu + %u): Frame %lu, location %lu, offset %lu, base %lu, skip %u\n",
			(unsigned long)sample, 
			(unsigned long)sample - mp3f -> delay,
		       	mp3f -> delay,
			(unsigned long)frame, 
			(unsigned long)location, 
			(unsigned long)offset,
			(unsigned long)base,
		       	mp3f -> skip_amount));

	return 1;
}

int mp3f_read (MP3_FILE mp3f, MP3F_OFFSET num_samples)
{
	int status;
	struct mad_decoder *decoder = &(mp3f -> decoder);

	if (! mp3f || ! mp3f -> f || ! mp3f -> callback)
		return 0;

	/* Seek if the last read left us in mid-frame */
	if (mp3f -> need_seek)
		if (! mp3f_seek (mp3f, mp3f -> next_read_position))
			return 0;

	mad_decoder_init (decoder, 
			mp3f,
			mp3f_mad_input,
			nullptr /* Header */,
			nullptr /* Filter */,
			mp3f_mad_report_samples,
			mp3f_mad_error,
			nullptr /* Message */);

	mp3f -> read_amount = num_samples;
	status = mad_decoder_run (decoder, MAD_DECODER_MODE_SYNC);
	mad_decoder_finish (decoder);

	mp3f -> next_read_position += num_samples;
	return (status == 0);
}

static enum mad_flow mp3f_mad_report_samples (void *context, struct mad_header const *header, struct mad_pcm *pcm)
{
	MP3_FILE mp3f = (MP3_FILE) context;
	const int *channels [] = { pcm -> samples [0], pcm -> samples [1] };
	unsigned length = pcm -> length;

	if (! mp3f || ! mp3f -> callback)
		return MAD_FLOW_BREAK;
	
	if (mp3f -> first_offset) {
		/* libMAD can decide to skip the first frame */
		if (header -> offset > mp3f -> first_offset) {
			MP3_DPRINTF (("Skip %u of %lu\n", length, mp3f -> skip_amount));
			mp3f -> skip_amount -= length;
		}
		mp3f -> first_offset = 0;
	}

	if (mp3f -> skip_amount >= length) {
		mp3f -> skip_amount -= length;
		return MAD_FLOW_IGNORE;
	}

	if (mp3f -> skip_amount > 0) {
		channels [0] += mp3f -> skip_amount;
		channels [1] += mp3f -> skip_amount;
		length -= mp3f -> skip_amount;
		mp3f -> skip_amount = 0;
	}

	if (length > mp3f -> read_amount) {
		length = mp3f -> read_amount;
		mp3f -> need_seek = 1;
	}

	if (length > 0)
		mp3f -> callback (channels, length, mp3f -> context);

	mp3f -> read_amount -= length;
	return (mp3f -> read_amount > 0) ? MAD_FLOW_CONTINUE : MAD_FLOW_STOP;
}

/* This conversion function was taken from minimad, and it could be better */
short mp3f_sample_to_short (MP3F_SAMPLE sample)
{
	/* round */
	sample += (1L << (MAD_F_FRACBITS - 16));

	/* clip */
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	/* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/* This function was adapted from libmad */
static enum mad_flow mp3f_mad_error(void *context, struct mad_stream *stream, struct mad_frame *frame)
{
	(void) context;
	(void) stream;
	(void) frame;
	return MAD_FLOW_CONTINUE;
}

/* This function was adapted from Audacity */
static enum mad_flow mp3f_mad_input(void *context, struct mad_stream *stream)
{
	MP3_FILE mp3f = (MP3_FILE) context;
	FILE *f = mp3f -> f;
	unsigned char *buffer = nullptr;
	unsigned nthrown = 0, ncopied = 0, size = 0;
	size_t nread = 0;
	MP3F_OFFSET offset;

	if (feof (f))
		return MAD_FLOW_STOP;

	if (stream -> next_frame) {
		nthrown = stream -> next_frame - mp3f -> buffer; 
		ncopied = MP3F_BUFFER_SIZE - nthrown;
		memmove (mp3f -> buffer, stream -> next_frame, ncopied);
	}

	buffer = mp3f -> buffer + ncopied;
	size = MP3F_BUFFER_SIZE - ncopied;
	offset = ftell (f) - ncopied;

	if (size > 0)
		nread = fread (buffer, 1, size, f);

	mad_stream_buffer_offset (stream, mp3f -> buffer, nread + ncopied, offset);
	return MAD_FLOW_CONTINUE;
}

static enum mad_flow mp3f_mad_first_header(void *context, struct mad_header const *header)
{
	MP3_FILE mp3f = (MP3_FILE) context;

	mp3f -> channels = MAD_NCHANNELS (header);
	mp3f -> frequency = header -> samplerate;
	mp3f -> samples_per_frame = 32 * MAD_NSBSAMPLES (header);
	/* Just in case there is no Xing header: */
	mp3f -> locations [mp3f -> num_locations ++] = header -> offset;

	return MAD_FLOW_CONTINUE;
}

static enum mad_flow mp3f_mad_first_filter (void *context,
		struct mad_stream const *stream,
		struct mad_frame *frame)
{
	MP3_FILE mp3f = (MP3_FILE) context;

	(void) frame;

	mp3f -> xing = mp3f_check_xing (mp3f, stream);

	/* Xing? No need to look further */
	if (mp3f -> xing)
		return MAD_FLOW_STOP;

	/* Otherwise, read two frames for size estimate */
	return (mp3f -> num_locations < 2) ? MAD_FLOW_IGNORE : MAD_FLOW_STOP;
}

static enum mad_flow mp3f_mad_scan_header(void *context, struct mad_header const *header)
{
	MP3_FILE mp3f = (MP3_FILE) context;

	/* Some sanity checks */
	if (mp3f -> channels != MAD_NCHANNELS (header))
		return MAD_FLOW_BREAK;

	if (mp3f -> frequency != header -> samplerate)
		return MAD_FLOW_BREAK;

	if (mp3f -> samples_per_frame != 32 * MAD_NSBSAMPLES (header))
		return MAD_FLOW_BREAK;

	/* Check whether to log this offset in the table */
	if ((mp3f -> frames % mp3f -> frames_per_location) == 0 &&
			mp3f -> num_locations < MP3F_MAX_LOCATIONS)
		mp3f -> locations [mp3f -> num_locations ++] = header -> offset;

	/* Count this frame */
	++ mp3f -> frames;
	mp3f -> samples += mp3f -> samples_per_frame;

	return MAD_FLOW_IGNORE;
}

/*
 * Identify a Xing VBR header.
 * This was adapted from madplay.
 */
# define XING_VBR_MAGIC	(('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')
# define XING_CBR_MAGIC	(('I' << 24) | ('n' << 16) | ('f' << 8) | 'o')

enum {
  XING_FLAGS_FRAMES = 0x0000'0001L,
  XING_FLAGS_BYTES  = 0x0000'0002L,
  XING_FLAGS_TOC    = 0x0000'0004L,
  XING_FLAGS_SCALE  = 0x0000'0008L
};

static int mp3f_check_xing (MP3_FILE mp3f, struct mad_stream const *stream)
{
	struct mad_bitptr ptr = stream -> anc_ptr;
	unsigned long magic, flags;

	/* When we get here we have the following data from the header:
	 *  channels
	 *  frequency
	 *  samples_per_frame
	 * We need to calculate the total number of frames and samples.
	 */

	magic = mad_bit_read (&ptr, 32);
	if (magic != XING_CBR_MAGIC && magic != XING_VBR_MAGIC)
		return 0;

	flags = mad_bit_read (&ptr, 32);
	if ((flags & XING_FLAGS_FRAMES) != XING_FLAGS_FRAMES)
		return 0;

	mp3f -> frames = mad_bit_read (&ptr, 32);
	mp3f -> samples = mp3f -> samples_per_frame * mp3f -> frames;

	return 1;
}

