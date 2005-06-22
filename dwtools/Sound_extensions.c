/* Sound_extensions.c
 *
 * Copyright (C) 1993-2003 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20001109 Sound_scale->Vector_scale
 djmw 20020516 GPL header
 djmw 20020523 Removed Sound_read/writeWAVAudioFile
 djmw 20030926 Sound_changeGender
 djmw 20040405 Renamed: Sound_overrideSamplingFrequency
 djmw 20041124 Changed call to Sound_to_Spectrum & Spectrum_to_Sound.
 djmw 20050620 Changed Pitch_HERTZ to Pitch_UNIT_HERTZ
*/

#include "Sound_extensions.h"
#include "Sound_and_Spectrum.h"
#include "Sound_to_Pitch.h"
#include "Vector.h"
#include "Pitch_extensions.h"
#include "Pitch_to_PitchTier.h"
#include "Pitch_to_PointProcess.h"
#include "DurationTier.h"
#include "Manipulation.h"
#include "NUM2.h"


static void i1write (Sound me, FILE *f, long *nClip)
{
	long i; float *s = my z[1], min = -128, max = 127;
	*nClip = 0;
	for (i=1; i <= my nx; i++)
	{
		float sample = floor (s[i] * 128 + 0.5);
		if (sample > max) { sample = max; (*nClip)++; }
		else if (sample < min) { sample = min; (*nClip)++; }
		binputi1 (sample, f);
	}
}

static void i1read (Sound me, FILE *f)
{
	long i; float *s = my z[1];
	for (i = 1; i <= my nx; i++) s[i] = bingeti1 (f) / 128.0;
}

static void u1write (Sound me, FILE *f, long *nClip)
{
	long i; float *s = my z[1], min = 0, max = 255;
	*nClip = 0;
	for (i=1; i <= my nx; i++)
	{
		float sample = floor ((s[i] + 1) * 255 / 2 + 0.5);
		if (sample > max) { sample = max; (*nClip)++; }
		else if (sample < min) { sample = min; (*nClip)++; }
		binputu1 (sample, f);
	}
}

static void u1read (Sound me, FILE *f)
{
	long i; float *s = my z[1];
	for (i=1; i <= my nx; i++) s[i] = bingetu1 (f) / 128.0 - 1.0;
}

static void i2write (Sound me, FILE *f, int littleEndian, long *nClip)
{
	long i; float *s = my z[1], min = -32768, max = 32767;
	void (*put) (int, FILE *) = littleEndian ? binputi2LE: binputi2;
	*nClip = 0;
	for (i=1; i <= my nx; i++)
	{
		float sample = floor (s[i] * 32768 + 0.5);
		if (sample > max) { sample = max; (*nClip)++; }
		else if (sample < min) { sample = min; (*nClip)++; }
		put (sample, f);
	}
}

static void i2read (Sound me, FILE *f, int littleEndian)
{
	long i; float *s = my z[1];
	int (*get) (FILE *) = littleEndian ? bingeti2LE : bingeti2;
	for (i=1; i <= my nx; i++) s[i] = get (f) / 32768.;
}

static void u2write (Sound me, FILE *f, int littleEndian, long *nClip)
{
	long i; float *s = my z[1], min = 0, max = 65535;
	void (*put) (unsigned int, FILE *) = littleEndian ? binputu2LE : binputu2;
	*nClip = 0;
	for (i=1; i <= my nx; i++)
	{
		float sample = floor ((s[i] + 1) * 65535 / 2 + 0.5);
		if (sample > max) { sample = max; (*nClip)++; }
		else if (sample < min) { sample = min; (*nClip)++; }
		put (sample, f);
	}
}

static void u2read (Sound me, FILE *f, int littleEndian)
{
	long i; float *s = my z[1];
	unsigned int (*get) (FILE *) = littleEndian ? bingetu2LE : bingetu2;
	for (i=1; i <= my nx; i++) s[i] = get (f) / 32768.0 - 1.0;
}

static void i4write (Sound me, FILE *f, int littleEndian, long *nClip)
{
	long i; float *s = my z[1]; double min = -2147483648.0, max = 2147483647.0;
	void (*put) (long, FILE *) = littleEndian ? binputi4LE : binputi4;
	*nClip = 0;
	for (i=1; i <= my nx; i++)
	{
		double sample = floor (s[i] * 2147483648.0 + 0.5);
		if (sample > max) { sample = max; (*nClip)++; }
		else if (sample < min) { sample = min; (*nClip)++; }
		put (sample, f);
	}
}

static void i4read (Sound me, FILE *f, int littleEndian)
{
	long i; float *s = my z[1];
	long (*get) (FILE *) = littleEndian ? bingeti4LE : bingeti4;
	for (i=1; i <= my nx; i++) s[i] = get (f) / 2147483648.;
}


static void u4write (Sound me, FILE *f, int littleEndian, long *nClip)
{
	long i; float *s = my z[1]; double min = 0.0, max = 4294967295.0;
	void (*put) (unsigned long, FILE *) = littleEndian ? binputu4LE : binputu4;
	*nClip = 0;
	for (i=1; i <= my nx; i++)
	{
		double sample = floor (s[i] * 4294967295.0 + 0.5);
		if (sample > max) { sample = max; (*nClip)++; }
		else if (sample < min) { sample = min; (*nClip)++; }
		put (sample, f);
	}
}

static void u4read (Sound me, FILE *f, int littleEndian)
{
	long i; float *s = my z[1];
	long (*get) (FILE *) = littleEndian ? bingeti4LE : bingeti4;
	for (i=1; i <= my nx; i++) s[i] = get (f) / 2147483648.0 - 1.0;
}


static void r4write (Sound me, FILE *f)
{
	long i; float *s = my z[1];
	for (i=1; i <= my nx; i++) binputr4 (s[i], f);
}

static void r4read (Sound me, FILE *f)
{
	long i; float *s = my z[1];
	for (i=1; i <= my nx; i++) s[i] = bingetr4 (f);
}

static long fileLengthBytes (FILE *f)
{
	long begin, end, current;
	if ((current = ftell (f)) < 0 ||
		fseek (f, 0L, SEEK_SET) || (begin = ftell (f)) < 0 ||
		fseek (f, 0L, SEEK_END) || (end = ftell (f)) < 0 ||
		fseek (f, current, SEEK_SET)) end = begin = 0;
	return end - begin;
}

/* precondition: -1 <= my z[1][i] <= 1 */
static void Sound_ulawDecode (Sound me)
{
	long i; double mu = 100, lnu1 = log (1 + mu);
	for (i=1; i <= my nx; i++)
	{
		double zabs = (exp (fabs (my z[1][i]) * lnu1) - 1.0) / mu;
		my z[1][i] = my z[1][i] < 0 ? -zabs : zabs;
	} 
}

/* precondition: -1 <= my z[1][i] <= 1 */
static void Sound_alawDecode (Sound me)
{
	double a = 87.6, lna1 = 1.0+log(a); long i;
	for (i=1; i <= my nx; i++)
	{
		double zabs = fabs (my z[1][i]);
		if (zabs <= 1.0 / lna1) my z[1][i] *= lna1 / a;
		else
		{
			double t = exp (lna1 * zabs - 1.0) / a;
			my z[1][i] = my z[1][i] > 0 ? t : -t;
		}
	}
}

static int nistGetValue (const char *header, const char *object, double *rval, const char *sval)
{
	char obj[30], type[10], *match = strstr (header, object);
	if (! match) return 0;
	if (sscanf (match, "%s%s%s", obj, type, sval) != 3) return 0;
	if (strequ (type, "-i") || strequ (type, "-r")) *rval = atof (sval);
	else if (strncmp (type, "-s", 2)) return 0;
	return 1;
}
/* pcm mono & embedded-shorten signals read (e.g.TIMIT data base, Groningen corpus) */	
#if 0
static Sound Sound_readFromNistAudioFile (MelderFile file)
{
	Sound me = NULL; FILE *f; int littleEndian = 0, alaw = 0, ulaw = 0;
	char header[1024], sval[100], *end_head;
 	long nSamples, nBytesPerSample, nChannels;
 	double fsamp, rval;
	
	if (! (f = Melder_fopen (file, "rb"))) return NULL;
	if (fread (header, 1, 1024, f) != 1024)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromNistAudioFile: cannot read header.");
	}
	if (strncmp (header, "NIST_1A\n   1024\n", 16))
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromNistAudioFile: not a NIST sound file.");
	}
	if (! nistGetValue (header, "sample_count", & rval, sval) || rval < 1)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromNistAudioFile: incorrect number of samples.");
	}
	nSamples = rval;
	if (! nistGetValue (header, "sample_n_bytes", & rval, sval) || rval < 1 || rval > 2)
	{
		Melder_fclose (fileName, f);
		return Melder_errorp ("Sound_readFromNistAudioFile: incorrect number of bytes per sample.");
	}
	nBytesPerSample = rval;
	if (! nistGetValue (header, "channel_count", & rval, sval) || rval < 1)
	{
		Melder_fclose (fileName, f);
		return Melder_errorp ("Sound_readFromNistAudioFile: incorrect number of channels.");
	}
	nChannels = rval;
	if (nChannels > 1)
	{
		Melder_fclose (fileName, f);
		return Melder_errorp ("Sound_readFromNistAudioFile: multichannel, we cannot read it.");
	}
	if (! nistGetValue (header, "sample_rate", & fsamp, sval) || fsamp < 1)
	{
		Melder_fclose (fileName, f);
		return Melder_errorp ("Sound_readFromNistAudioFile: incorrect sample frequency.");
	}
	/* big or little endian, we (sgi) are big */
	if (nistGetValue (header, "sample_byte_format", & rval, sval) &&
		strequ (sval, "01")) littleEndian = 1;
	if (! (me = Sound_createSimple (nSamples/fsamp, fsamp))) goto error;
	/* display the header */
	if (end_head = strstr (header, "end_head"))
	{
		header[end_head - header + 8] = '\0'; 
		Melder_casual ("NIST header from file: %s\n%s\n", Melder_FILENAME (fileName), header);
	}
	/* check for compression */
	if (nistGetValue (header, "sample_coding", & rval, sval) && (strnequ (sval, "pcm", 3) ||
		(ulaw = strnequ (sval, "ulaw", 4)) ||
		(alaw = strnequ (sval, "alaw", 4))) &&
		strstr (sval, "embedded-shorten-v"))
	{
		/* FIX:
		The SPEX POLYPHONE-NL database is encoded as alaw,embedded-shorten-v1.09.
		SPEX has extended Shorten version 1.09 with alaw encoding. The characterization
		in the encoded file is TYPE_ALAW (an enumerated type with value 7).
		In Shorten version 2.1 enumerated type value 7 is used for lossy ulaw.
		*/
		int isSpexPolyphone = nistGetValue (header, "database_id", &rval, sval) && strequ (sval,"POLYPHONE-NL");
		Melder_fclose (fileName, f);
		if (! unshorten (fileName, 1024, isSpexPolyphone, & me))
		{
			Melder_error ("Sound_readFromNistAudioFile: cannot unshorten.");
			goto error;
		}
		return me;
	}
	
	/* read the samples from the file */
	if (nBytesPerSample == 1)
	{
		long i = 1; float *s = my z[1]; int c;	
		if (ulaw)
		{
			while (i <= my nx && (c = getc (f)) != EOF)
			{
				float tmp = Sulaw2linear (c);
				s[i++] = tmp / 32768.0;
			}
		}
		else if (alaw)
		{
			while (i <= my nx && (c = getc (f)) != EOF)
			{
				float tmp = Salaw2linear (c);
				s[i++] = tmp / 32768.0;
			}
		}
		else
		{
			for (i=1; i <= my nx; i++)
			{
				float tmp = bingeti1 (f);
				s[i] = tmp / 128.0;
			}
		}
	}
	else if (nBytesPerSample == 2) i2read (me, f, littleEndian);	
	if (feof (f) || ferror (f))
	{
		Melder_error ("Sound_readFromNistAudioFile: not completed.");
		goto error;
	}
	Melder_fclose (fileName, f);
	return me;
error:
	forget (me);
	Melder_fclose (fileName, f);
	return Melder_errorp("Sound_readFromNistfile: reading from file \"%s\" not performed", Melder_FILENAME (fileName));
}
#endif
static void warning (char *p, long nClip, long nSamp)
{
	Melder_warning ("%s: %ld from %ld samples have been clipped.\n"
		"Advice: you could scale the amplitudes or write to a binary file.",  
		p, nClip, nSamp);
}
#if 0
int Sound_writeToNistAudioFile (Sound me, const char *name)
{
	char header[1024]; long nClip, i, samplingFrequency = (1.0 / my dx);
	int littleEndian = 1; short i2min, i2max;
	FILE *f; float max, min; 
	if (! (f = Melder_fopen (name, "wb"))) return 0;
	/*
		Put 0-bytes in header
	*/
	memset (header, 0, 1024);
	/*
		Find extrema
	*/
	max = min = my z[1][1];
	for (i=2; i <= my nx; i++)
	{
		float val = my z[1][i];
		if (val > max) max = val; else if (val < min) min = val;
	}
	max = floor (max * 32768 + 0.5);
	if (max > 32767) max = 32767;
	min = floor (min * 32768 + 0.5);
	if (min < -32768) min = -32767;
	i2max = max; i2min = min;
	sprintf (header, "NIST_1A\n   1024\n"
		"channel_count -i 1\n"
		"sample_count -i %d\n"
		"sample_n_bytes -i 2\n"
		"sample_byte_format -s2 01\n" /* 01=LE 10=BE */
		"sample_coding -s3 pcm\n"
		"sample_rate -i %d\n"
		"sample_min -i %d\n"
		"sample_max -i %d\n"
		"end_head\n", my nx, samplingFrequency, i2min, i2max);
	fwrite (header, 1, 1024, f);
	i2write (me, f, littleEndian, & nClip);
	if (nClip > 0) warning ("Sound_writeToNistAudioFile", nClip, my nx);
	Melder_fclose (name, f);
	return 1;
}
#endif
/* Old TIMIT sound-file format */
Sound Sound_readFromCmuAudioFile (MelderFile file)
{
	Sound me = NULL; long nSamples; int littleEndian = 1; short nChannels;
	FILE *f;
	
	if (! (f = Melder_fopen (file, "rb"))) return NULL;

	if (bingeti2LE (f) != 6)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromCmuAudioFile: incorrect header size.");
	}
	(void) bingeti2LE (f);
	if ((nChannels = bingeti2LE (f)) < 1)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromCmuAudioFile: incorrect number of channels.");
	}
	if (nChannels > 1)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromCmuAudioFile: multi channel, cannot read.");
	}
	if (bingeti2LE (f) < 1)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromCmuAudioFile: incorrect sampling frequency.");
	}
	if ((nSamples = bingeti4LE (f)) < 1)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromCmuAudioFile: incorrect number of samples. "); 
	}
	if (! (me = Sound_createSimple (nSamples/16000., 16000))) goto error;
	i2read (me, f, littleEndian);
	if (feof (f) || ferror (f))
	{
		Melder_error ("Sound_readFromCmuAudioFile: not completed.");
		goto error;
	}
	Melder_fclose (file, f);
	return me;
error:
	forget (me);		
	Melder_fclose (file, f);
	return Melder_errorp("Sound_readFromCmuAudioFile: Reading from file \"%s\" not performed.", MelderFile_messageName (file));
}
 
Sound Sound_readFromRawFile (MelderFile file, const char *format, int nBitsCoding,
	int littleEndian, int unSigned, long skipNBytes, double samplingFrequency)
{
	Sound me = NULL; long nSamples, nBytesPerSample;
	FILE *f = Melder_fopen (file, "rb");

	if (! f) return NULL;

	if (! format) format = "integer";
	if (nBitsCoding <= 0) nBitsCoding = 16;
	nBytesPerSample = ( nBitsCoding + 7) / 8;
	if (strequ (format, "float")) nBytesPerSample = 4;
	if (nBytesPerSample == 3 || nBytesPerSample > 4)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromRawFile: number of bytes"
		" per sample should be 1, 2 or 4.");
	}
	if (skipNBytes <= 0) skipNBytes = 0;
	nSamples = ( fileLengthBytes (f) - skipNBytes) / nBytesPerSample;
	if (nSamples < 1)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromRawFile: no samples left to read");
	}
	if (! (me = Sound_createSimple (nSamples/samplingFrequency, samplingFrequency)))
	{
		Melder_fclose (file, f);
		return Melder_errorp ("Sound_readFromRawFile: no memory for Sound.");
	}
	fseek (f, skipNBytes, SEEK_SET);
	if (      nBytesPerSample == 1 &&   unSigned) u1read (me, f);
	else if ( nBytesPerSample == 1 && ! unSigned) i1read (me, f);
	else if ( nBytesPerSample == 2 &&   unSigned) u2read (me, f, littleEndian);
	else if ( nBytesPerSample == 2 && ! unSigned) i2read (me, f, littleEndian);
	else if ( nBytesPerSample == 4 &&   unSigned) u4read (me, f, littleEndian);
	else if ( nBytesPerSample == 4 && ! unSigned) i4read (me, f, littleEndian);
	else if ( nBytesPerSample == 4 && strequ (format, "float")) r4read (me, f);
	if (feof (f) || ferror (f))
	{
		Melder_error ("Sound_readFromRawFile: not completed."); goto error;
	}
	Melder_fclose (file, f);
	return me;
error:
	forget (me);
	Melder_fclose (file, f);
	return Melder_errorp("Sound_readFromRawFile: Reading from file \"%s\" not performed", MelderFile_messageName (file));
}

int Sound_writeToRawFile (Sound me, MelderFile file, const char *format, int littleEndian, 
	int nBitsCoding, int unSigned)
{
	long nBytesPerSample, nClip = 0;
	FILE *f = Melder_fopen (file, "wb");
	if (! f)
	{
		return Melder_error ("Sound_readWriteToRawFile: cannot open.");
	}
	if (! format) format = "integer";
	if (nBitsCoding <= 0) nBitsCoding = 16;
	nBytesPerSample = ( nBitsCoding + 7) / 8;
	if (strequ (format, "float")) nBytesPerSample = 4;
	if (nBytesPerSample == 3 || nBytesPerSample > 4)
		{ Melder_error ("number of bytes per sample should be 1, 2 or 4."); goto error; }
	if (     nBytesPerSample == 1 &&   unSigned) u1write (me, f, & nClip);
	else if (nBytesPerSample == 1 && ! unSigned) i1write (me, f, & nClip);
	else if (nBytesPerSample == 2 &&   unSigned) u2write (me, f, littleEndian, & nClip);
	else if (nBytesPerSample == 2 && ! unSigned) i2write (me, f, littleEndian, & nClip);
	else if (nBytesPerSample == 4 &&   unSigned) u4write (me, f, littleEndian, & nClip);
	else if (nBytesPerSample == 4 && ! unSigned) i4write (me, f, littleEndian, & nClip);
	else if (nBytesPerSample == 4 && strequ (format, "float")) r4write (me, f);
	if (nClip > 0) warning ("Sound_writeToRawAudioFile", nClip, my nx);
	if (feof (f) || ferror (f))
	{ 
		Melder_error ("Sound_writeToRawFile: not completed"); goto error;
	}
	Melder_fclose (file, f);
	return 1;
error:
	Melder_fclose (file, f);
	return Melder_error("Sound_writeToRawFile: writing to file \"%s\" not performed.", MelderFile_messageName (file));
}

struct dialogic_adpcm 
{
	char code;
	short last, index;
	short step_size[49];
	short adjust[8];		
};

static void dialogic_adpcm_init (struct dialogic_adpcm *adpcm)
{
	short step_size[49] = {
		16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 
		41, 45, 50, 55, 60, 66, 73, 80, 88, 97,
		107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 
		279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 
		724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552};
	short adjust[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };
	long i;
			
	adpcm -> last = 0;
	adpcm -> index = 0;

	for (i = 0; i < 49; i++) adpcm -> step_size[i] = step_size[i];
	for (i = 0; i <  8; i++) adpcm ->    adjust[i] = adjust[i];
	
}

/*
	The code is adapted from: 
	Bob Edgar (), "PC Telephony - The complete guide to designing, 
		building and programming systems using Dialogic and Related 
		Hardware", 272-276.
*/
static float dialogic_adpcm_decode (struct dialogic_adpcm *adpcm)
{
	short diff, e, ss, s;
	float scale = 32767.0 / 32768.0 / 2048.0;

	/*
		nibble = B3 B2 B1 B0 (4 lower bits)
		d(n) = ss(n)*B2 + ss(n)/2 *B1 + ss(n)/4*B0 + ss(n)/8
	*/
	
	ss = adpcm -> step_size[adpcm -> index];
	e = ss / 8;
	if (adpcm -> code & 0x01) e += ss / 4;
	if (adpcm -> code & 0x02) e += ss / 2;
	if (adpcm -> code & 0x04) e += ss;
	
	/*
		If B3==1 then d(n) = -d(n);
	*/
	
	diff = (adpcm -> code & 0x08) ? -e : e;
	
	/*
		x(n) = x(n-1)+d(n)
	*/
	
	s = adpcm -> last + diff;
	if (s > 2048) s = 2048; 
	if (s < -2048) s = -2048;
	adpcm -> last = s;
	
	/*
		ss(n+1) = ss(n) * 1.1*M(L(n)) via lookup table
	*/
	
	adpcm -> index += adpcm -> adjust[adpcm -> code & 0x07];
	if (adpcm -> index <  0) adpcm -> index = 0;
	if (adpcm -> index > 48) adpcm -> index = 48;
	
	return scale * s; 
}

Sound Sound_readFromDialogicADPCMFile (MelderFile file, double sampleRate)
{
	char *proc = "Sound_readFromDialogicADPCMFile";
	struct dialogic_adpcm adpcm;
	unsigned char sc;
	Sound me = NULL;
	long i, n = 1, numberOfSamples, filelength;
	
	FILE *f = Melder_fopen (file, "rb");
	
	if (! f) return NULL;

	filelength = MelderFile_length (file);
	if (filelength <= 0)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("%s: File is empty", proc);
	}
	
	/*
		Two samples in each byte
	*/
	
	numberOfSamples = 2 * filelength;
	if (numberOfSamples <= 0)
	{
		Melder_fclose (file, f);
		return Melder_errorp ("%s: File too long", proc);
	}
	me = Sound_createSimple (numberOfSamples /sampleRate, sampleRate);
	if (me == NULL) return NULL;
	
	/*
		Read all bytes and decode 
	*/	
	
	dialogic_adpcm_init (& adpcm);
	
	for (n = 1, i = 1; i <= filelength; i++)
	{
		(void) fread (&sc, 1, 1, f);
		adpcm.code = (char) ((sc >> 4) & 0x0f);
		my z[1][n++] = dialogic_adpcm_decode(& adpcm);
		adpcm.code = (char) (sc & 0x0f);
		my z[1][n++] = dialogic_adpcm_decode(& adpcm);
	}
	
	Melder_fclose (file, f);
	return me;
}

void Sound_preEmphasis (Sound me, double preEmphasisFrequency)
{
	long i; float *s = my z[1]; 
	double preEmphasis = exp(- 2.0 * NUMpi * preEmphasisFrequency * my dx);
	for (i=my nx; i >= 2; i--) s[i] -= preEmphasis * s[i-1];
}

void Sound_deEmphasis (Sound me, double deEmphasisFrequency)
{
	long i; float *s = my z[1]; 
	double deEmphasis = exp(- 2.0 * NUMpi * deEmphasisFrequency * my dx);
	for (i=2; i <= my nx; i++) s[i] += deEmphasis * s[i-1];
}

Sound Sound_createGaussian (double windowDuration, double samplingFrequency)
{
	Sound me = Sound_createSimple (windowDuration, samplingFrequency);
	double imid, edge;
	long i;
	float *s = my z[1];
	
	if (me == NULL) return NULL;
	
	imid = 0.5 * (my nx + 1); edge = exp (-12.0);
	for (i=1; i <= my nx; i++)
	{
		s[i] = (exp (-48.0*(i-imid)*(i-imid)/(my nx+1)/(my nx+1)) - edge)
			/ (1-edge);
	}
	return me;
}

Sound Sound_createHamming (double windowDuration, double samplingFrequency)
{
	Sound me = Sound_createSimple (windowDuration, samplingFrequency);
	double p;
	float *s = my z[1];
	long i;
	
	if (me == NULL) return NULL;
	
	p = 2 * NUMpi / (my nx - 1);
	for (i=1; i <= my nx; i++)
	{
		s[i] = 0.54 - 0.46 * cos ((i-1) * p);
	}
	return me;
}

static Sound Sound_create2 (double minimumTime, double maximumTime, double samplingFrequency)
{
	return Sound_create (minimumTime, maximumTime, floor ((maximumTime - minimumTime) * samplingFrequency + 0.5),
		1.0 / samplingFrequency, minimumTime + 0.5 / samplingFrequency);
}

static Sound Sound_createSimpleTone (double minimumTime, double maximumTime,
	double samplingFrequency, double frequency, double amplitude,
	double initialPhase)
{
	Sound me = Sound_create2 (minimumTime, maximumTime, samplingFrequency);
	double w = 2 * NUMpi * frequency; long i;
	if (! me) return NULL;
	for (i=1; i <= my nx; i++)
	{
		double t = my x1 + (i - 1) * my dx; /* Sampled_indexToX */
		my z[1][i] = amplitude * sin (w * t + initialPhase);
	}
	return me;
}

/*
	Trig functions whose arguments form a linear sequence x = x1 + n.dx, 
	for n=0,1,2,... are efficiently calculated by the following recurrence:
		cos(a+dx) = cos(a) - (alpha . cos(a) + beta . sin(a))
		sin(a+dx) = sin(a) - (alpha . sin(a) - beta . sin(a))
	where alpha and beta are precomputed coefficients
		alpha = 2 sin^2(dx/2) and beta = sin(dx)
	In this way aplha and beta do not loose significance if the increment 
	dx is small.

*/

static Sound Sound_createToneComplex (double minimumTime, double maximumTime, double samplingFrequency,
	double firstFrequency, long numberOfComponents, double frequencyDistance,
	long mistunedComponent, double mistuningFraction, int scaleAmplitudes)
{
	Sound me = Sound_create2 (minimumTime, maximumTime, samplingFrequency);
	long i, j;
	if (! me) return NULL;
	for (j=1; j <= numberOfComponents; j++)
	{
		double fraction = j == mistunedComponent ? mistuningFraction : 0;
		double w = 2 * NUMpi * (firstFrequency + (j - 1 + fraction) * frequencyDistance);
		double delta = w * my dx;
		double alpha = 2 * sin (delta / 2) * sin (delta / 2);
		double beta = sin (delta);
		double sint = sin (w * my x1);
		double cost = cos (w * my x1);
		my z[1][1] += sint;
		for (i=2; i <= my nx; i++)
		{
			double costd = cost - (alpha * cost + beta * sint);
			double sintd = sint - (alpha * sint - beta * cost);
			my z[1][i] += sintd;
			cost = costd; sint = sintd;
		}
	}
	if (scaleAmplitudes) Vector_scale (me, 0.99996948);
	return me;
}


Sound Sound_createSimpleToneComplex (double minimumTime, double maximumTime, double samplingFrequency,
	double firstFrequency, long numberOfComponents, double frequencyDistance, int scaleAmplitudes)
{
	if (firstFrequency + (numberOfComponents - 1) * frequencyDistance > samplingFrequency / 2)
	{
		Melder_warning ("Sound_createSimpleToneComplex: frequency of (some) components too high.");
		numberOfComponents = 1.0 + (samplingFrequency / 2 - firstFrequency) / frequencyDistance;
	}
	return Sound_createToneComplex (minimumTime, maximumTime, samplingFrequency,
		firstFrequency, numberOfComponents, frequencyDistance, 0, 0, scaleAmplitudes);
}

Sound Sound_createMistunedHarmonicComplex (double minimumTime, double maximumTime, double samplingFrequency,
	double firstFrequency, long numberOfComponents, long mistunedComponent, 
	double mistuningFraction, int scaleAmplitudes)
{
	if (firstFrequency + (numberOfComponents - 1) * firstFrequency > samplingFrequency/2)
	{
		Melder_warning ("Sound_createMistunedHarmonicComplex: frequency of (some) components too high.");
		numberOfComponents = 1.0 + (samplingFrequency / 2 - firstFrequency) / firstFrequency;
	}
	if (mistunedComponent > numberOfComponents) Melder_warning ("Sound_createMistunedHarmonicComplex:"
		"mistuned component too high.");
	return Sound_createToneComplex (minimumTime, maximumTime, samplingFrequency,
		firstFrequency, numberOfComponents, firstFrequency, mistunedComponent,
		mistuningFraction, scaleAmplitudes);
}

/*
	The gammachirp is a "chirp tone" with a  gamma-function envelope:
	f(t) = t^(n-1) exp (-2 pi b t) cos (2 pi f0 t + c ln (t) + p0)
	     = t^(n-1) exp (-2 pi b t) cos (phi(t))
	Instantaneous frequency f is defined as f = d phi(t) / dt / (2 pi)
	and so: f = f0 + c /(2 pi t)
	Irino: bandwidth = (frequency * (6.23e-6 * frequency + 93.39e-3) + 28.52)
*/
Sound Sound_createGammaTone (double minimumTime, double maximumTime, double samplingFrequency,
	long gamma, double frequency, double bandwidth, double initialPhase, double addition,
	int scaleAmplitudes)
{
	Sound me = Sound_create2 (minimumTime, maximumTime, samplingFrequency); long i;
	double twoPi = 2 * NUMpi, nyquistFrequency = samplingFrequency / 2;
	double b2pi = twoPi * bandwidth, w = twoPi * frequency;
	
	if (! me) return NULL;
	for (i=1; i <= my nx; i++)
	{
		double t = (i - 0.5) * my dx;
		double f = frequency + addition / (twoPi * t);
		if (f > 0 && f < nyquistFrequency) my z[1][i] = pow (t, gamma - 1) * exp (- b2pi * t) *
			cos (w * t + addition * log (t) + initialPhase);
	}
	if (scaleAmplitudes) Vector_scale (me, 0.99996948);
	return me;
}

static void NUMgammatoneFilter4 (double *x, double *y, long n, double centre_frequency,
	double bandwidth, double samplingFrequency)
{

	long i, j, n8; 
	double a[5], b[9], zr, zi, dr, di, tr, ti, nr, ni, n2, gr, gi, gain;
	double dt = 1.0 / samplingFrequency, wt = NUMpi * centre_frequency * dt;
	double bt = 2 * NUMpi * bandwidth * dt, dt2 = dt * dt, dt4 = dt2 * dt2;

	Melder_assert (n > 0 && centre_frequency > 0 && bandwidth >= 0 && 
		samplingFrequency > 0);

	/*
		The filter function is:
			H(z) = sum (i=0..4, a[i] z^-i) / sum (j=0..4, b[j] z^-j)
		Coefficients a & b according to:
		Slaney (1993), An efficient implementation of the Patterson-Holdsworth 
		auditory filterbank, Apple Computer Technical Report 35, 41 pages.
		For the a's we have left out an overal scale factor of dt^4. 
		This makes a[0] = 1.
	*/
	
	a[0]= dt4;
	a[1]= -4 * dt4 * cos (2 * wt) * exp (-    bt);
	a[2]=  6 * dt4 * cos (4 * wt) * exp (-2 * bt);
	a[3]= -4 * dt4 * cos (6 * wt) * exp (-3 * bt);
	a[4]=      dt4 * cos (8 * wt) * exp (-4 * bt);
		
	b[0] = 1;
	b[1]= -8 * cos (2 * wt)                           * exp (-    bt);
	b[2]= (16 + 12 * cos (4 * wt))                    * exp (-2 * bt);
	b[3]= (-48 * cos (2 * wt) - 8 * cos (6 * wt))     * exp (-3 * bt);
	b[4]= (36 + 32 * cos (4 * wt) + 2 * cos (8 * wt)) * exp (-4 * bt);
	b[5]= (-48 * cos (2 * wt) - 8 * cos (6 * wt))     * exp (-5 * bt);
	b[6]= (16 + 12 * cos (4 * wt))                    * exp (-6 * bt);
	b[7]= -8 * cos (2 * wt)                           * exp (-7 * bt);
	b[8]=                                               exp (-8 * bt);
	
	/*
		Calculate gain (= Abs (H(z); f=fc) and scale a[0-4] with it.
	*/
	
	zr =  cos (2 * wt);
	zi = -sin (2 * wt);

	dr = a[4];
    di = 0;

  	for (j = 1; j <= 4; j++)
  	{
  		tr = a[4-j] + zr * dr - zi * di;
		ti = zi * dr + zr * di;
    	dr = tr; di = ti;
	}
	
	dr = b[8];
	di = 0;
	for (j = 1; j <= 8; j++)
	{
		nr = b[8 - j] + zr * dr - zi * di;
		ni = zi * dr + zr * di;
		dr = nr; di = ni;
	}

	n2 = nr * nr + ni * ni;
	gr = tr * nr + ti * ni;
	gi = ti * nr - tr * ni;
	gain = sqrt (gr * gr + gi * gi) / n2;

  	for (j = 0; j <= 4; j++)
	{
		a[j] /= gain;
	}
	
	if (Melder_debug == -1)
	{
		Melder_info ("--gammatonefilter4--\nF = %s, B = %s, T = %s\nGain = %s", 
			Melder_double (centre_frequency), Melder_double (bandwidth),
			Melder_double (dt), Melder_double (gain));
		for (i = 0; i <= 4; i++)
		{
			Melder_info ("a[%d] = %s", i, Melder_double (a[i]));
		}
		for (i = 0; i <= 8; i++)
		{
			Melder_info ("b[%d] = %s", i, Melder_double (b[i]));
		}
	}
	/*
		Perform the filtering. For the first 8 samples we must do some
		extra work. 
	*/

	n8 = n < 8 ? n : 8;	
	for (i = 1; i <= n8; i++)
	{
		y[i] = a[0] * x[i];
		if (i > 1) y[i] += a[1] * x[i-1] - b[1] * y[i-1]; else continue;
		if (i > 2) y[i] += a[2] * x[i-2] - b[2] * y[i-2]; else continue;
		if (i > 3) y[i] += a[3] * x[i-3] - b[3] * y[i-3]; else continue;	
		if (i > 4) y[i] += a[4] * x[i-4] - b[4] * y[i-4]; else continue;	
		if (i > 5) y[i] -= b[5] * y[i-5]; else continue;	
		if (i > 6) y[i] -= b[6] * y[i-6]; else continue;	
		if (i > 7) y[i] -= b[7] * y[i-7];
	}
	
	for (i = n8 + 1; i <= n; i++)
	{
		y[i]  = a[0] * x[i];
		y[i] += a[1] * x[i-1] + a[2] * x[i-2] + a[3] * x[i-3] + a[4] * x[i-4];
		y[i] -= b[1] * y[i-1] + b[2] * y[i-2] + b[3] * y[i-3] + b[4] * y[i-4];
		y[i] -= b[5] * y[i-5] + b[6] * y[i-6] + b[7] * y[i-7] + b[8] * y[i-8];
	}
}


Sound Sound_filterByGammaToneFilter4 (Sound me, double centre_frequency, double bandwidth)
{
	Sound thee = NULL; long i; double *y = NULL, *x = NULL, fs = 1 / my dx;

	if (centre_frequency <= 0 || bandwidth < 0) return NULL;
	
	if (! (thee = Sound_create (my xmin, my xmax, my nx, my dx, my x1)) ||
		! (y = NUMdvector (1, my nx)) ||
		! (x = NUMdvector (1, my nx))) goto end;
		
	for (i=1; i <= my nx; i++) x[i] = my z[1][i];
	
	NUMgammatoneFilter4 (x, y, my nx, centre_frequency, bandwidth, fs);
	
	for (i=1; i <= my nx; i++)
	{
		thy z[1][i] = y[i];
	}
	
end:
	NUMdvector_free (x, 1); NUMdvector_free (y, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}


static Sound Sound_filterByGammaToneFilter42 (Sound me, double centre_frequency, double bandwidth)
{
	Sound thee = NULL; long i, j; double *y = NULL, *x = NULL;
	double a[5], b[9], zr, zi, dr, di, tr, ti, nr, ni, n2, gr, gi, gain;
	double dt = my dx, wt = 2 * NUMpi * centre_frequency * dt;
	double bt = 2 * NUMpi * bandwidth * dt;

	if (centre_frequency < 50 || centre_frequency > 0.5 / my dx ||
		bandwidth < 0) return  NULL;
	
	if (! (thee = Sound_create (my xmin, my xmax, my nx, my dx, my x1)) ||
		! (y = NUMdvector (-7, my nx)) ||
		! (x = NUMdvector (-3, my nx))) goto end;
	
	for (i=1; i <= my nx; i++)
	{
		x[i] = my z[1][i];
	}
	
	b[0] = 1;
	b[1]= -8 * cos (wt) * exp (-bt);
	b[2]= (16 + 12 * cos (2 * wt)) * exp (-2 * bt);
	b[3]= (-48 * cos (wt) - 8 * cos (3 * wt)) * exp (-3 * bt);
	b[4]= (36 + 32 * cos (2 * wt) + 2 * cos (4 * wt)) * exp (-4 * bt);
	b[5]= (-48 * cos (wt) - 8 * cos (3 * wt)) * exp (-5 * bt);
	b[6]= (16 + 12*cos (2 * wt)) * exp (-6 * bt);
	b[7]= (-8 * cos (wt)) * exp (-7 * bt);
	b[8]= exp (-8 * bt);

	/*
		Leave out overall scale factor dt^4, this makes a[0] = 1 
	*/
	
	a[0]= 1;
	a[1]= -4 * cos (    wt) * exp (-bt);
	a[2]=  6 * cos (2 * wt) * exp (-2 * bt);
	a[3]= -4 * cos (3 * wt) * exp (-3 * bt);
	a[4]=      cos (4 * wt) * exp (-4 * bt);
	
	/*
		Calculate gain and scale a[0-4] with it.
		H(z) = sum (i=0..4, a[i] z^-i) / sum (j=0..4, b[j] z^-j)
		gain = Abs (H(z); f=fc)
	*/
	
	zr = cos (wt);
	zi = -sin (wt);

	dr = a[4];
    di = 0;

  	for (j=1; j <= 4; j++)
  	{
  		tr = a[4-j] + zr * dr - zi * di;
		ti = zi * dr + zr * di;
    	dr = tr; di = ti;
	}
	
	dr = b[8];
	di = 0;
	for (j=1; j <= 8; j++)
	{
		nr = b[8 - j] + zr * dr - zi * di;
		ni = zi * dr + zr * di;
		dr = nr; di = ni;
	}

	n2 = nr * nr + ni * ni;
	gr = tr * nr + ti * ni;
	gi = ti * nr - tr * ni;
	gain = sqrt (gr * gr + gi * gi) / n2;

  	for (j=0; j <= 4; j++) a[j] /= gain;
	
	/* perform the filtering */
	
	for (i=1; i <= my nx; i++)
	{
		y[i] = a[0] * x[i];
		y[i] += a[1] * x[i-1] + a[2] * x[i-2] + a[3] * x[i-3] + a[4] * x[i-4];
		y[i] -= b[1] * y[i-1] + b[2] * y[i-2] + b[3] * y[i-3] + b[4] * y[i-4];
		y[i] -= b[5] * y[i-5] + b[6] * y[i-6] + b[7] * y[i-7] + b[8] * y[i-8];
		thy z[1][i] = y[i];
	}
	
end:
	NUMdvector_free (x, -3); NUMdvector_free (y, -7);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

/*
Sound Sound_createShepardTone (double minimumTime, double maximumTime, double samplingFrequency,
	double baseFrequency, double frequencyShiftFraction, double maximumFrequency, double amplitudeRange)
{
	Sound me; long i, j, nComponents = 1 + log2 (maximumFrequency / 2 / baseFrequency);
	double lmin = pow (10, - amplitudeRange / 10);
	double twoPi = 2.0 * NUMpi, f = baseFrequency * (1 + frequencyShiftFraction);
	if (nComponents < 2) Melder_warning ("Sound_createShepardTone: only 1 component.");
	Melder_casual ("Sound_createShepardTone: %ld components.", nComponents);
	if (! (me = Sound_create2 (minimumTime, maximumTime, samplingFrequency))) return NULL;
	
	for (j=1; j <= nComponents; j++)
	{
		double fj = f * pow (2, j-1), wj = twoPi * fj;
		double amplitude = lmin + (1 - lmin) *
			(1 - cos (twoPi * log (fj + 1) / log (maximumFrequency + 1))) / 2;
		for (i=1; i <= my nx; i++)
		{
			my z[1][i] += amplitude * sin (wj * (i - 0.5) * my dx);
		}
	}
	Vector_scale (me, 0.99996948);
	return me;
}
*/

/* can be implemented more efficiently with sin recurrence? */
/* amplitude(f) = min + (1-min)*(1-cos(2*pi*(ln(f/f1) / ln(fn/f1)))/2 */
Sound Sound_createShepardTone (double minimumTime, double maximumTime, 
	double samplingFrequency, double lowestFrequency, long numberOfComponents, 
	double frequencyChange_st, double amplitudeRange)
{
	Sound me; long i, j;
	double scale = pow (2, numberOfComponents);
	double maximumFrequency = lowestFrequency * scale;
	double lmin = pow (10, - amplitudeRange / 10), twoPi = 2.0 * NUMpi;
	double ln2t0 = log (2) * frequencyChange_st / 12;
	double lnf1 = log (lowestFrequency + 1);
	double amplarg = twoPi / log ((maximumFrequency + 1) / 
		(lowestFrequency + 1));
		
	if (lowestFrequency > samplingFrequency / 2) return Melder_errorp
		("Sound_createShepardTone: lowest frequency too high.");
	if (maximumFrequency > samplingFrequency / 2) return Melder_errorp
		("Sound_createShepardTone: frequency of highest component too high.");
	me = Sound_create2 (minimumTime, maximumTime, samplingFrequency);
	if (me == NULL) return NULL;
	
	for (i = 1; i <= my nx; i++)
	{
		double amplitude, argt, t = (i - 0.5) * my dx, ft = lowestFrequency;
		if (frequencyChange_st != 0)
		{
			double expt = exp (ln2t0 * t);
			argt = twoPi * lowestFrequency * (expt - 1) / ln2t0;
			ft *= expt;
		}
		else argt = twoPi * ft * t;
		for (j=1; j <= numberOfComponents; j++)
		{
			while (ft >= maximumFrequency) { ft /= scale; argt /= scale; }
		/*amplitude = lmin + (1 - lmin) * (1 - cos (twoPi * log (ft + 1) / log (maximumFrequency + 1))) / 2;*/
			amplitude = lmin + (1 - lmin) * (1 - cos (amplarg * (log (ft + 1) - lnf1))) / 2;
			my z[1][i] += amplitude * sin (argt);
			ft *= 2; argt *= 2;
		}
	}
	Vector_scale (me, 0.99996948);
	return me;
}

Sound Sound_createPattersonWightmanTone (double minimumTime, double maximumTime, double samplingFrequency,
	double baseFrequency, double frequencyShiftRatio, long numberOfComponents)
{
	Sound me; long i, j; double w0 = 2 * NUMpi * baseFrequency;
	if ((numberOfComponents - 1 + frequencyShiftRatio) * baseFrequency >  samplingFrequency/2) return Melder_errorp
		("Sound_createPattersonWightmanTone: frequency of one or more components too large.");
	if (! (me = Sound_create2 (minimumTime, maximumTime, samplingFrequency))) return NULL;
	for (i=1; i <= my nx; i++)
	{
		double a = 0, t = (i - 0.5) * my dx;
		for (j=1; j <= numberOfComponents; j++) a += sin ((j + frequencyShiftRatio) * w0 * t);
		my z[1][i] = a;
	}
	Vector_scale (me, 0.99996948);
	return me;
}

Sound Sound_createPlompTone (double minimumTime, double maximumTime, double samplingFrequency,
	double baseFrequency, double frequencyFraction, long m)
{
	Sound me; long i, j;
	double w1 = 2 * NUMpi * (1 - frequencyFraction) * baseFrequency;
	double w2 = 2 * NUMpi * (1 + frequencyFraction) * baseFrequency;
	if (12 * (1 + frequencyFraction) * baseFrequency >  samplingFrequency/2) return Melder_errorp
		("Sound_createPlompTone: frequency of one or more components too large.");
	if (! (me = Sound_create2 (minimumTime, maximumTime, samplingFrequency))) return NULL;
	for (i=1; i <= my nx; i++)
	{
		double a = 0, t = (i - 0.5) * my dx;
		for (j=1; j <= m; j++) a += sin (j * w1 * t);
		for (j=m+1; j <= 12; j++) a += sin (j * w2 * t);
		my z[1][i] = a;
	}
	Vector_scale (me, 0.99996948);
	return me;
	
}

void Sounds_multiply (Sound me, Sound thee)
{
	long i, n = my nx < thy nx ? my nx : thy nx;
	float *s1 = my z[1], *s2 = thy z[1];
	
	for (i=1; i <= n; i++) s1[i] *= s2[i];
}

static Matrix Sound_to_KoonenMatrix (Sound me, long octavesUp)
{
	Spectrum thee = NULL; Matrix him = NULL; int status = 0;
	/*
		C C# D Eb E F F# G G#  A Bb  B  C
	    1  2 3  4 5 6  7 8  9 10 11 12
	    A = 220 hz (octave -1), A' = 440 Hz (octave 0),	A'' = 880 (octave 1); 
	*/
	double a0 = 440; 
	/* minimum and maximum frequencies needed */
	double fmin, fmax;
	/* first frequency (C) of octave in which a = 440 */
	double c0 = a0 * pow (2, - 9 / 12.0);
	float *flow = NULL;
	long i, j, kf, octavesDown, nOctaves, nflow;
	/*
		maximum number of octaves up w.r.t. sampling frequency.
		sampling frequency == 1 / (2 * dx)
	*/
	long  octavesUpMax = - NUMlog2 (my dx * 2 * c0); 
	
	if (octavesUp > octavesUpMax) octavesUp = octavesUpMax;
	/*
		we don't need more than 3 octaves down -> a = 55 Hz (440 / 2^3) -> c = 32.7 Hz
	*/
	octavesDown = 3; 
	nOctaves = octavesUp + 1 + octavesDown;
	nflow = nOctaves * 12 + 1;
	if (! (thee = Sound_to_Spectrum (me, TRUE)) ||
		! (him = Matrix_create (0.5, 12.5, 12, 1, 1, 0.5, nOctaves + 0.5, nOctaves, 1, 1)) ||
		! (flow = NUMfvector (1, nflow))) goto end;
	/*
		calculate lower frequency of each interval
	*/
	fmin = c0 * pow (2, - octavesDown - 0.5 / 12);
	for (kf=1; kf <= nflow; kf++) flow[kf] = fmin * pow (2, (kf - 1) / 12.);
	fmax = flow[nflow];
	kf = 1;
	/*
		sum the energy in the Spectrum in the corresbonding bins
	*/
	for (i=2; i <= thy nx; i++)
	{
		double f = i * thy dx; long irow, icol;
		if (f < fmin) continue;
		if (f > fmax) break;
		if (f > flow[kf+1]) kf++;
		irow = (kf - 1) / 12 + 1; icol = (kf - 1) % 12 + 1;
		his z[irow][icol] += thy z[1][i] * thy z[1][i] + thy z[2][i] * thy z[2][i];
	}
	for (i=1; i <= nOctaves; i++) for (j=1; j <= 12; j++) /* dB's */
	{
		double dbm = 2 * his z[i][j] * thy dx / 4.0e-10;
		if (dbm > 0) his z[i][j] = 10 * log10 (dbm);
	}
	status = 1;
end:
	forget (thee); NUMfvector_free (flow, 1);
	if (Melder_hasError ())  forget (him);
	return him;
}

static void Sound_drawKoonenPlot (Sound me, Graphics g, long octavesDown, long octavesUp, int garnish)
{
	Spectrum thee = NULL; Matrix him = NULL;
	double a = 440, fc0 = a * pow (2, -9 / 12), fmin, fmax;
	long i, j, upmax = - NUMlog2 (fc0 * my dx * 2), nOctaves;
	
	if (octavesDown > 3) octavesDown = 3;
	if (octavesUp > upmax) octavesUp = upmax;
	nOctaves = octavesUp + 1 + octavesDown;
	fmin = fc0 * pow (2, - octavesDown); fmax = fc0 * pow (2, octavesUp+1);
	if (! (thee = Sound_to_Spectrum (me, TRUE)) ||
		! (him = Matrix_create (0.5, 12.5, 12, 1, 1, 0.5, nOctaves + 0.5, nOctaves, 1, 1))) goto end;
	for (i=2; i <= thy nx; i++)
	{
		double f = i * thy dx, nr; long n, irow, icol;
		if (f < fmin) continue;
		if (f > fmax) break;
		nr = 12 * NUMlog2 (f / fc0); /* f = fc0 * 2 ^(n/12) */
		n = 12 * octavesDown + 1 + nr;
		irow = (n - 1) / 12 + 1; icol = (n - 1) % 12 + 1;
		his z[irow][icol] += thy z[1][i] * thy z[1][i] + thy z[2][i] * thy z[2][i];
	}
	for (i=1; i <= nOctaves; i++) for (j=1; j <= 12; j++) /* dB's */
	{
		double dbm = 2 * his z[i][j] * thy dx / 4.0e-10;
		if (dbm > 0) his z[i][j] = 10 * log10 (dbm);
	}
	Matrix_paintCells (him, g, 0, 0, 0, 0, 0, 0);
	if (garnish)
	{
	}
end:
	forget (thee);
	forget (him);
}

double Sound_power (Sound me)
{
	double e = 0; float *amplitude = my z[1]; long i;
	for (i=1; i <= my nx; i++) e += amplitude[i] * amplitude[i];
	return sqrt (e) * my dx / (my xmax - my xmin);
}

double Sound_correlateParts (Sound me, double tx, double ty, double duration)
{
	double xm = 0, ym = 0, sxx = 0, syy = 0, sxy = 0, denum, rxy;
	float *x, *y; long i, nbx, nby, ney, ns, increment = 0, decrement = 0;
	
	if (ty < tx ) { double t = tx; tx = ty; ty = t; }
	nbx = Sampled_xToNearestIndex (me, tx);
	nby = Sampled_xToNearestIndex (me, ty);
	ney = Sampled_xToNearestIndex (me, ty + duration);
	if (nbx < 1) increment = 1 - nbx;
	if (ney > my nx) decrement = ney - my nx;
	ns = duration / my dx - increment - decrement;
	if (ns < 1) return 0;
	x = & my z[1][nbx + increment - 1];
	y = & my z[1][nby + increment - 1];
	for (i=1; i <= ns; i++) { xm += x[i]; ym += y[i]; }
	xm /= ns; ym /= ns;
	for (i=1; i <= ns; i++)
	{
		double xt = x[i] - xm, yt = y[i] - ym;
		sxx += xt * xt; syy += yt * yt; sxy += xt * yt;
	}
	denum = sxx * syy;
	rxy = denum > 0 ? sxy / sqrt (denum) : 0;
	return rxy;
}

static double Sound_correlateParts2 (Sound me, double tx, double ty, double duration)
{
	double xsum = 0, x2sum = 0, ysum = 0, y2sum = 0, xysum = 0, denum, rxy;
	float *z = my z[1]; long i, nbx, nby, ney, ns, increment = 0, decrement = 0;
	
	if (ty < tx ) { double t = tx; tx = ty; ty = t; }
	nbx = Sampled_xToNearestIndex (me, tx);
	nby = Sampled_xToNearestIndex (me, ty);
	ney = Sampled_xToNearestIndex (me, ty + duration);
	if (nbx < 1) increment = 1 - nbx;
	if (ney > my nx) decrement = ney - my nx;
	ns = duration / my dx - increment - decrement;
	if (ns < 1) return 0;
	for (i=1; i <= ns; i++)
	{
		double x = z[nbx + increment - 1 + i];
		double y = z[nby + increment - 1 + i];
		xsum += x; x2sum += x * x; xysum += x * y;
		ysum += y; y2sum += y * y;
	}
	denum = (ns * x2sum - xsum * xsum) * (ns * y2sum - ysum * ysum);
	rxy = denum > 0 ? (ns * xysum - xsum * ysum) / sqrt (denum) : 0;
	return rxy;
}

void Sound_localMean (Sound me, double fromTime, double toTime, double *mean)
{
	long i, n1 = Sampled_xToNearestIndex (me, fromTime);
	long n2 = Sampled_xToNearestIndex (me, toTime);
	float *s = my z[1];
	*mean = 0;
	if (fromTime > toTime) return;
	if (n1 < 1) n1 = 1;
	if (n2 > my nx) n2 = my nx;
	for (i=n1; i <= n2; i++) *mean += s[i];
	*mean /= n2 - n1 + 1;
}

void Sound_localPeak (Sound me, double fromTime, double toTime, double ref, double *peak)
{
	long i, n1 = Sampled_xToNearestIndex (me, fromTime);
	long n2 = Sampled_xToNearestIndex (me, toTime);
	float *s = my z[1];
	*peak = -1e38;
	if (fromTime > toTime) return;
	if (n1 < 1) n1 = 1;
	if (n2 > my nx) n2 = my nx;
	for (i=n1; i <= n2; i++)
	{
		double ds = fabs (s[i] - ref);
		if (ds > *peak) *peak = ds;
	}
}

void Sound_into_Sound (Sound me, Sound to, double startTime)
{
	long i, index = Sampled_xToNearestIndex (me, startTime);
	
	for (i=1; i <= to -> nx; i++)
	{
		long j = index - 1 + i;
		to -> z[1][i] = j < 1 || j > my nx ? 0 : my z[1][j];
	}	
}

/*
IntervalTier Sound_PointProcess_to_IntervalTier (Sound me, PointProcess thee, double window);
IntervalTier Sound_PointProcess_to_IntervalTier (Sound me, PointProcess thee, double window)
{
	IntervalTier him;
	TextInterval interval;
	double t1, t2, t, window2 = window / 2;
	long i;
	
	him = IntervalTier_create (my xmin, my xmax);
	if (him == NULL) return NULL;

	t1 = thy t[1] - window2;
	if (t1 < my xmin) t1 = my xmin;
	t2 = t1 + window2;
	if (t2 > my xmax) t2 = my xmax;
	interval = TextInterval_create (t1, t2, "yes");
	if (interval == NULL ||
		! Collection_addItem (his intervals, interval)) goto end;
	
	for (i = 2; i <= thy nt; i++)
	{
		t =  thy t[i];
		
		if (t  <= t2)
		{
			long index = his points -> size;
			RealPoint point = his points -> item[index];
			t2 = t + window2;
			if (t2 > my xmax) t2 = my xmax;
			point -> value = t2;	
		}
		else
		{
			t2 = t + window2;
			if (t2 > my xmax) t2 = my xmax;			
			if (! RealTier_addPoint (him, t, t2)) goto end;
		}
	}

end:
	if (Melder_hasError()) forget (him);
	return him;		
}
*/

int Sound_overwritePart (Sound me, double t1, double t2, Sound thee, double t3)
{
	char *proc = "Sound_overwritePart";
	long i, i1, i2, i3, i4;

	if (my dx != thy dx) return Melder_error 
		("%s: Sample rates must be equal.", proc);
			
	if (t1 == 0) t1 =  my xmin;
	if (t2 == 0) t2 =  my xmax;
	
	i1 = Sampled_xToHighIndex (me, t1);
	i2 = Sampled_xToLowIndex (me, t2);
	if (i1 > i2 || i2 > my nx || i1 < 1) return Melder_error 
		("%s: Times of part to be overwritten must be within the sound.", proc);
	
	if (t3 == 0) t3 = thy xmin;
	i3 = Sampled_xToHighIndex (thee, t3);
	i4 = Sampled_xToLowIndex (thee, t3 + t2 - t1);
	if (i4 > thy nx || i3 < 1) return Melder_error ("%s: Not enough samples to be copied", proc);

	if (i4 - i3 != i2 - i1) return Melder_error ("%s: ", proc);

	for (i = i1; i <= i2; i++)
	{
		my z[1][i] = thy z[1][i - i1 + i3];
	}
	return 1;
}

int Sound_filter_part_formula (Sound me, double t1, double t2, const char *formula)
{
	Sound part = NULL, filtered = NULL;
	Spectrum spec = NULL;
	int status = 0;
	
	part = Sound_extractPart (me, t1, t2, enumi(Sound_WINDOW, Rectangular), 1, 1);
	if (part == NULL) goto end;
	
	spec = Sound_to_Spectrum (part, TRUE); 
	if (spec == NULL) goto end;
	
	if (! Matrix_formula ((Matrix) spec, formula, 0)) goto end;
	
	filtered = Spectrum_to_Sound (spec);
	if (filtered == NULL) goto end;
	
	/* Overwrite part between t1 and t2 of original with the filtered signal */
	
	status = Sound_overwritePart (me, t1, t2, filtered, 0);
	
end:

	forget (filtered);
	forget (spec); 
	forget (part);
	
	return status;
}

/*
   First approximation on the basis of differences in the sampled signal.
   The underlying analog signal still could have jumps undetected by this algorithm.
    We could get a better approximation by first upsampling the signal.
*/
PointProcess Sound_to_PointProcess_getJumps (Sound me, double minimumJump, double dt)
{
	PointProcess thee;
	float *s = my z[1];
	long i = 1, dtn = dt / my dx;

	thee = PointProcess_create (my xmin, my xmax, 10);	
	if (thee == NULL) return NULL;
	
	if (dtn < 1) dtn = 1;
	while (i < my nx)
	{
		long j, step = 1;
		j = i + 1;
		while (j <= i + dtn && j <= my nx)
		{
			if (fabs (s[i] - s[j]) > minimumJump)
			{
				double t = Sampled_indexToX (me, i);
				if (! PointProcess_addPoint (thee, t))
				{
					forget (thee); return NULL;
				}				
				step = j - i + 1; break;
			}
			j++;
		}
		i += step;
	} 	
	return thee;
}

Sound Sound_and_Pitch_changeGender_old (Sound me, Pitch him, double fmin, double formantRatio, 
	double new_pitch, double pitchRangeFactor, double durationFactor)
{
	char *proc = "Sound_changeGender";
	Sound sound = NULL, thee = NULL; 
	Pitch pitch = NULL;
	PointProcess pulses = NULL;
	PitchTier pitchTier = NULL;
	DurationTier duration = NULL;
	double samplingFrequency_old = 1 / my dx;
	double median, factor;
	
	if (my xmin != his xmin || my xmax != his xmax) return Melder_errorp 
		("%s: The Pitch and the Sound object must have the same starting times and finishing times.", proc);

	sound = Data_copy (me);
	if (sound == NULL) return NULL;
	
	Vector_subtractMean (sound);
		
	if (formantRatio != 1)
	{
		/* Shift all frequencies (inclusive pitch!) */
		
		Sound_overrideSamplingFrequency (sound, samplingFrequency_old * formantRatio);
	}
	
	pitch = Pitch_scaleTime (him, 1 / formantRatio);		
	if (pitch == NULL) goto end;

	pulses = Sound_Pitch_to_PointProcess_cc (sound, pitch);
	if (pulses == NULL) goto end;

	pitchTier = Pitch_to_PitchTier (pitch);
	if (pitchTier == NULL) goto end;
		
	median = Pitch_getQuantile (pitch, 0, 0, 0.5, Pitch_UNIT_HERTZ);
	if (median != 0 && median != NUMundefined)
	{
		/* Incorporate pitch shift from overriding the sampling frequency */
	
		if (new_pitch == 0) new_pitch = median / formantRatio;
		factor = new_pitch / median;
		PitchTier_multiplyFrequencies (pitchTier, sound -> xmin, sound -> xmax, factor);

		PitchTier_modifyRange (pitchTier, sound -> xmin, sound -> xmax,
			fmin, pitchRangeFactor, new_pitch);
	}
	else
	{
		Melder_warning ("%s: There were no voiced segments found.", proc);	
	}
	duration = DurationTier_create (my xmin, my xmax);
	if (duration == NULL) goto end;	
	if (! RealTier_addPoint (duration, (my xmin + my xmax) / 2,
		formantRatio * durationFactor)) goto end;
	
	thee = Sound_Point_Pitch_Duration_to_Sound (sound, pulses, pitchTier, 
		duration, 1.5 / fmin);
	if (thee == NULL) goto end;
	
	/* Resample to the original sampling frequency */
	
	if (formantRatio != 1)
	{
		Sound tmp = thee;			
		thee = Sound_resample (tmp, samplingFrequency_old, 10);
		forget (tmp);
	}
	
end:

	forget (sound); forget (pitch); forget (pulses);
	forget (pitchTier); forget (duration);
	
	return thee;
}

/*
To do: 
	Remove the pitchRangeFactor parameter: We want a new command to change the pitch range,
		'Change pitch dynamics...' or 'Change pitch excurions...' or ???
	In the form we want better names: 'Formant frequencies ratio', 'Pitch ratio', 'Duration ratio' 
*/
Sound Sound_changeGender_old (Sound me, double fmin, double fmax, double formantRatio, 
	double new_pitch, double pitchRangeFactor, double durationFactor)
{
	Pitch pitch = NULL;
	Sound thee = NULL;
	
	pitch = Sound_to_Pitch (me, 0.8 / fmin, fmin, fmax);
	if (pitch == NULL) return NULL;
	
	thee = Sound_and_Pitch_changeGender_old (me, pitch, fmin, formantRatio, 
		new_pitch, pitchRangeFactor, durationFactor);
		
	forget (pitch);
	
	return thee;
}

Sound Sound_changeGender (Sound me, double pitchMin, double pitchMax, double pitchRatio, 
	double formantFrequenciesRatio, double durationRatio)
{
	Pitch pitch = NULL;
	Sound thee = NULL;
	
	pitch = Sound_to_Pitch (me, 0.8 / pitchMin, pitchMin, pitchMax);
	if (pitch == NULL) return NULL;
	
	thee = Sound_and_Pitch_changeGender (me, pitch, pitchMin, pitchRatio, 
		formantFrequenciesRatio, durationRatio);
		
	forget (pitch);
	
	return thee;
}

/* Sound_and_Pitch_changeGender was adapted from a script by Ton Wempe */
Sound Sound_and_Pitch_changeGender (Sound me, Pitch him, double pitchMin, double pitchRatio, 
	double formantFrequenciesRatio, double durationRatio)
{
	char *proc = "Sound_changeGender";
	Sound sound = NULL, thee = NULL; 
	Pitch pitch = NULL;
	PointProcess pulses = NULL;
	PitchTier pitchTier = NULL;
	DurationTier duration = NULL;
	double samplingFrequency_old = 1 / my dx;
	double median;
	
	if (my xmin != his xmin || my xmax != his xmax) return Melder_errorp 
		("%s: The Pitch and the Sound object must have the same starting times and finishing times.", proc);

	sound = Data_copy (me);
	if (sound == NULL) return NULL;
	
	Vector_subtractMean (sound);
		
	if (formantFrequenciesRatio != 1)
	{
		/* Shift all frequencies (inclusive pitch!) */
		
		Sound_overrideSamplingFrequency (sound, samplingFrequency_old * formantFrequenciesRatio);
	}
	
	pitch = Pitch_scaleTime (him, 1 / formantFrequenciesRatio);		
	if (pitch == NULL) goto end;

	pulses = Sound_Pitch_to_PointProcess_cc (sound, pitch);
	if (pulses == NULL) goto end;

	pitchTier = Pitch_to_PitchTier (pitch);
	if (pitchTier == NULL) goto end;
		
	median = Pitch_getQuantile (pitch, 0, 0, 0.5, Pitch_UNIT_HERTZ);
	if (median != 0 && median != NUMundefined)
	{
		/* Incorporate pitch shift from overriding the sampling frequency */
			
		pitchRatio /= formantFrequenciesRatio;
		PitchTier_multiplyFrequencies (pitchTier, sound -> xmin, sound -> xmax, pitchRatio);

	}
	else
	{
		Melder_warning ("%s: There were no voiced segments found.", proc);	
	}
	duration = DurationTier_create (my xmin, my xmax);
	if (duration == NULL) goto end;	
	if (! RealTier_addPoint (duration, (my xmin + my xmax) / 2,
		formantFrequenciesRatio * durationRatio)) goto end;
	
	thee = Sound_Point_Pitch_Duration_to_Sound (sound, pulses, pitchTier, 
		duration, 1.5 / pitchMin);
	if (thee == NULL) goto end;
	
	/* Resample to the original sampling frequency */
	
	if (formantFrequenciesRatio != 1)
	{
		Sound tmp = thee;			
		thee = Sound_resample (tmp, samplingFrequency_old, 10);
		forget (tmp);
	}
	
end:

	forget (sound); forget (pitch); forget (pulses);
	forget (pitchTier); forget (duration);
	
	return thee;
}


/* End of file Sound_extensions.c */
