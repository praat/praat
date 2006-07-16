/* Sound_files.c
 *
 * Copyright (C) 1992-2006 Paul Boersma & David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/09/12 MelderFile_checkSoundFile
 * pb 2004/10/17 test for NULL file pointer when closing in Sound_read(2)FromSoundFile
 * pb 2005/06/17 Mac headers
 * pb 2006/01/05 movies for Mac
 * pb 2006/05/29 QuickTime inclusion made optional
 */

#include "Sound.h"
#include <time.h>

/*
static void Sound_ulawDecode (Sound me) {
	double mu = 100, lnu1 = log (1 + mu);
	long i;
	for (i = 1; i <= my nx; i ++) {
		double zabs = (exp (fabs (my z [1] [i]) * lnu1) - 1.0) / mu;
		my z [1] [i] = my z [1] [i] < 0 ? -zabs : zabs;
	} 
}
static void Sound_alawDecode (Sound me) {
	double a = 87.6, lna1 = 1.0 + log (a);
	long i;
	for (i = 1; i <= my nx; i ++) {
		double zabs = fabs (my z [1] [i]);
		if (zabs <= 1.0 / lna1) {
			my z [1] [i] *= lna1 / a;
		} else {
			double t = exp (lna1 * zabs - 1.0) / a;
			my z [1] [i] = my z [1] [i] > 0 ? t : - t;
		}
	}
}
*/

#if defined (macintosh)
	#include <Resources.h>
	#include <MacMemory.h>
	#include <Sound.h>
	#if ! defined (__MACH__) || defined (__GNUC__)
		#include <SoundInput.h>
	#endif
	#include <Script.h>
	typedef struct SndResource {
		short formatType; /* Always 1, because 2 is obsolete. */
		short numberOfSynthesizers; /* 1, namely: */
		short resourceIDOfSynthesizer; /* sampledSynth */
		long initializationOption; /* e.g. initMono */
		short numberOfSoundCommands; /* 1, namely: */
		SndCommand itsSndCommand; /* bufferCmd, 0, 20 */
		SoundHeader itsSndHeader;
	} *SndResourcePtr, **SndResourceHandle;
#endif
#if defined (__MACH__) || defined (_WIN32)
	#if defined (__MACH__) && defined (__MWERKS__)
		typedef unsigned char Boolean;
	#endif
	#if defined (_WIN32)
		#define TARGET_API_MAC_CARBON  0
	#endif
	#if ! defined (DONT_INCLUDE_QUICKTIME)
		#include <QuickTime.h>
	#endif
#endif
#if defined (_WIN32) && ! defined (DONT_INCLUDE_QUICKTIME)
	#define PfromCstr(p,c)  p [0] = strlen (c), strcpy ((char *) p + 1, c);
	static int Melder_fileToMac (MelderFile file, void *void_fspec) {
		FSSpec *fspec = (FSSpec *) void_fspec;
		Str255 pname;
		OSErr err;
		PfromCstr (pname, file -> path);
		err = FSMakeFSSpec (0, 0, & pname [0], fspec);
		if (err != noErr && err != fnfErr) {
			if (err == -2095) {
				return Melder_error ("To open this movie file, you have to install QuickTime first (www.apple.com).");
			}
			return Melder_error ("Error #%d looking for file %s.", err, file -> path);
		}
		return 1;
	}
#endif

Sound Sound_readFromSoundFile (MelderFile file) {
	Sound me = NULL;
	int numberOfChannels, encoding, fileType;
	double sampleRate;
	long startOfData, numberOfSamples;
	FILE *f = NULL;
	MelderFile_open (file); cherror
	f = file -> filePointer;
	fileType = MelderFile_checkSoundFile (file, & numberOfChannels, & encoding, & sampleRate, & startOfData, & numberOfSamples); cherror
	if (fileType == 0) { Melder_error ("Not an audio file."); goto end; }
	if (numberOfChannels == 2) Melder_warning ("(Sound_readFromSoundFile:) Reading stereo file as mono.");
	if (fseek (f, startOfData, SEEK_SET) == EOF)   /* Start from beginning of Data Chunk. */
		{ Melder_error ("No data in audio file."); goto end; }
	me = Sound_createSimple (numberOfSamples / sampleRate, sampleRate); cherror
	if (encoding == Melder_SHORTEN || encoding == Melder_POLYPHONE) {
		fclose (f);
		if (TRUE /* ! unshorten (file, 1024, encoding == Melder_POLYPHONE, & me) */) {
			forget (me);
			return Melder_errorp ("(Sound_readFromSoundFile:) Cannot unshorten. Write to paul.boersma@uva.nl for more information.");
		}
		return me;
	}
	Melder_readAudioToFloat (f, numberOfChannels, encoding, my z [1], NULL, numberOfSamples); cherror
end:
	if (f) fclose (f);
	iferror { Melder_error ("(Sound_readFromSoundFile:) File %s not read.", MelderFile_messageName (file)); forget (me); }
	return me;
}

int Sound_read2FromSoundFile (MelderFile file, Sound *return_left, Sound *return_right) {
	Sound left = NULL, right = NULL;
	int numberOfChannels, encoding, fileType;
	double sampleRate;
	long startOfData, numberOfSamples;
	FILE *f = NULL;
	MelderFile_open (file); cherror
	f = file -> filePointer;
	fileType = MelderFile_checkSoundFile (file, & numberOfChannels, & encoding, & sampleRate, & startOfData, & numberOfSamples); cherror
	if (fileType == 0) { Melder_error ("Can only read AIFF/C, WAV, NeXT/Sun, and NIST files."); goto end; }
	if (encoding == Melder_SHORTEN || encoding == Melder_POLYPHONE) {
		fclose (f);
		return Melder_error ("(Sound_read2FromSoundFile:) Cannot unshorten two channels.");
	}
	if (fseek (f, startOfData, SEEK_SET) == EOF)   /* Start from beginning of sample data. */
		{ Melder_error ("No data in audio file."); goto end; }
	left = Sound_createSimple (numberOfSamples / sampleRate, sampleRate); cherror
	if (numberOfChannels == 2) {
		right = Sound_createSimple (numberOfSamples / sampleRate, sampleRate); cherror
	}
	Melder_readAudioToFloat (f, numberOfChannels, encoding, left -> z [1], right ? right -> z [1] : NULL, numberOfSamples); cherror
end:
	if (f) fclose (f);
	iferror { Melder_error ("(Sound_read2FromSoundFile:) File %s not read.", MelderFile_messageName (file)); forget (left); forget (right); }
	*return_left = left;
	*return_right = right;
	return 1;
}

Sound Sound_readFromSesamFile (MelderFile fs) {
	Sound me = NULL;
	FILE *f = Melder_fopen (fs, "rb");
	long header [1 + 128];
	double samplingFrequency;
	long numberOfSamples, i;
	if (! f) return 0;
	for (i = 1; i <= 128; i ++) header [i] = bingeti4LE (f);
	/* Try SESAM header. */
	samplingFrequency = header [126];
	numberOfSamples = header [127];
	if (samplingFrequency == 0.0 || numberOfSamples == 0) {
		/* Try LVS header. */
		samplingFrequency = header [62];
		numberOfSamples = (header [6] << 8) - header [68];
	}
	if (numberOfSamples < 1 || numberOfSamples > 1000000000 ||
			samplingFrequency < 10.0 || samplingFrequency > 100000000.0) {
		fclose (f);
		return Melder_errorp ("(Sound_readFromSesamFile:) "
			"File \"%s\" is not a correct SESAM or LVS file.", MelderFile_messageName (fs));
	}
	me = Sound_createSimple (numberOfSamples / samplingFrequency, samplingFrequency);
	if (! me) return NULL;
	for (i = 1; i <= numberOfSamples; i ++) my z [1] [i] = bingeti2LE (f) * (1.0 / 2048);   /* 12 bits. */
	if (fclose (f) == EOF) return Melder_errorp ("Error reading file \"%s\".", MelderFile_messageName (fs));
	return me;
}

#ifdef macintosh
Sound Sound_readFromMacSoundFile (MelderFile file) {
	Sound me = NULL;
	FSSpec fspec;
	int path;
	Handle han;
	long numberOfSamples;
	double samplingFrequency;
	SoundHeader *header;
	unsigned const char *from;
	float *to;
	long i;
	Melder_fileToMac (file, & fspec);
	path = FSpOpenResFile (& fspec, fsRdPerm);   /* Open resource fork; there are the sounds. */
	if (path == -1)
		return Melder_errorp ("(Sound_readFromMacSoundFile:) Error opening resource fork.");
	if (Count1Resources ('snd ') == 0) {   /* Are there really any sounds in this file? */
		CloseResFile (path);
		return Melder_errorp ("(Sound_readFromMacSoundFile:) No sound resources found.");
	}
	han = Get1IndResource ('snd ', 1);   /* Take the first sound from this file. */
	if (! han) {
		CloseResFile (path);
		return Melder_errorp ("(Sound_readFromMacSoundFile:) Sound too large.");
	}
	HNoPurge (han);   /* Resources in general, 'snd ' in particular, are often purgeable. */
	DetachResource (han);   /* Release the sound's binding with the Resource Map. */
	CloseResFile (path);   /* Remove the Resource Map; the sound is ours now. */
	{
		SndResourcePtr p = (SndResourcePtr) *han;
		if (p -> formatType != 1 ||
			 p -> numberOfSynthesizers != 1 ||
			 p -> resourceIDOfSynthesizer != sampledSynth ||
			 p -> numberOfSoundCommands != 1 ||
			 p -> itsSndCommand. cmd != bufferCmd + dataOffsetFlag ||
			 p -> itsSndCommand. param2 != 20 ||
			 p -> itsSndHeader. samplePtr != NULL)
		{
			DisposeHandle (han);
			return Melder_errorp ("(Sound_readFromMacSoundFile:) Sound has an unknown format.");
		}
	}
	header = & (**(SndResourceHandle) han).itsSndHeader;
	numberOfSamples = header -> length;
	samplingFrequency = (long) header -> sampleRate / 65536.0;
	me = Sound_createSimple (numberOfSamples / samplingFrequency, samplingFrequency);
	if (me == NULL) return NULL;
	header = & (**(SndResourceHandle) han).itsSndHeader;   /* Do not move memory. */
	from = (unsigned const char *) & header -> sampleArea - 1;
	to = my z [1];
	for (i = 1; i <= numberOfSamples; i ++)
		to [i] = (from [i] - 128) * (1.0 / 128);
	DisposeHandle (han);
	return me;
}
#endif

Sound Sound_readFromMovieFile (MelderFile file) {
	Sound me = NULL;
#if (defined (__MACH__) || defined (_WIN32)) && ! defined (DONT_INCLUDE_QUICKTIME)
	int debug = 0;
	FSSpec fspec;
	short refNum = 0, resourceID = 0;
	OSErr err = noErr;
	Movie movie = 0;
	Boolean wasChanged;
	Track track;
	Media media = NULL;
	long numberOfMediaSamples, numberOfSamplesPerMediaSample;
	double duration;
	SoundDescriptionHandle hSoundDescription = NULL;   /* Not NewHandle yet (on Windows, initialize QuickTime first). */
	Handle extension = NULL;
	AudioFormatAtomPtr decompressionAtom = NULL;
	short numberOfChannels, sampleSize;
	double samplingFrequency;
	unsigned long inputBufferSize, outputBufferSize;
	unsigned long numberOfInputFrames, numberOfOutputFrames, numberOfOutputBytes;
	Handle inputBufferHandle = NULL;
	TimeValue actualTime, durationPerSample;
	long inputNumberOfBytes, actualNumberOfSamples;
	long isamp;
	SoundComponentData input, output;
	SoundConverter soundConverter = NULL;

	#ifdef _WIN32
		InitializeQTML (0);
	#endif
	EnterMovies ();
	Melder_fileToMac (file, & fspec); cherror
	err = OpenMovieFile (& fspec, & refNum, fsRdPerm);
	if (err != noErr) {
		Melder_error ("Not a movie file.");
		goto end;
	}
	err = NewMovieFromFile (& movie, refNum, & resourceID, NULL, newMovieActive, & wasChanged);
	if (err != noErr) {
		Melder_error ("Cannot find the movie in the movie file.");
		goto end;
	}
	track = GetMovieIndTrackType (movie, 1, SoundMediaType, movieTrackMediaType);
	if (track == NULL) {
		Melder_error ("Invalid track in movie file.");
		goto end;
	}
	media = GetTrackMedia (track);
	if (media == NULL) {
		Melder_error ("Invalid media in movie file.");
		goto end;
	}
	hSoundDescription = (SoundDescriptionHandle) NewHandle (0);
	numberOfMediaSamples = GetMediaSampleCount (media);
	duration = (double) GetMediaDuration (media) / GetMediaTimeScale (media);
	GetMediaSampleDescription (media, 1, (SampleDescriptionHandle) hSoundDescription);
	if (GetMoviesError () != noErr) {
		Melder_error ("Cannot get sound description in movie file.");
		goto end;
	}
	err = GetSoundDescriptionExtension (hSoundDescription, & extension, siDecompressionParams);
	if (err == noErr) {
		Size size = GetHandleSize (extension);
		HLock (extension);
		decompressionAtom = (AudioFormatAtomPtr) NewPtr (size);
		err = MemError ();
		if (err != noErr) {
			Melder_error ("No memory left when looking into movie file.");
			goto end;
		}
		BlockMoveData (*extension, decompressionAtom, size);
		HUnlock (extension);
	} else {
		err = noErr;   /* No atom: OK. */
	}
	numberOfChannels = (*hSoundDescription) -> numChannels;
	sampleSize = (*hSoundDescription) -> sampleSize;
	samplingFrequency = (double) (*hSoundDescription) -> sampleRate / 65536.0;
	me = Sound_createSimple (duration, samplingFrequency); cherror
	numberOfSamplesPerMediaSample = my nx / numberOfMediaSamples;
	if (my nx % numberOfMediaSamples != 0) {
		Melder_error ("Media samples not equally long: %ld / %ld gives a remainder.", my nx, numberOfMediaSamples);
		goto end;
	}
	/*
	 * The sound converter has to decompress the sound data to 16-bit mono.
	 */
	input. flags = output. flags = 0;
	input. format = (*hSoundDescription) -> dataFormat;
	output. format = kSoundNotCompressed;
	input. numChannels = (*hSoundDescription) -> numChannels;
	output. numChannels = 1;
	input. sampleSize = (*hSoundDescription) -> sampleSize;
	output. sampleSize = 16;
	input. sampleRate = output. sampleRate = (*hSoundDescription) -> sampleRate;
	input. sampleCount = output. sampleCount = 0;
	input. buffer = output. buffer = NULL;
	input. reserved = output. reserved = 0;
	err = SoundConverterOpen (& input, & output, & soundConverter);
	if (err != noErr) {
		Melder_error ("Cannot open sound converter.");
		goto end;
	}
	err = SoundConverterSetInfo (soundConverter, siClientAcceptsVBR, (Ptr) true);
	if (err != noErr && err != siUnknownInfoType) {
		Melder_error ("Don't like VBR.");
		goto end;
	}
	err = SoundConverterSetInfo (soundConverter, siDecompressionParams, decompressionAtom);
	if (err != noErr && err != siUnknownInfoType) {
		Melder_error ("Don't like that decompression.");
		goto end;
	}
	outputBufferSize = my nx * 2;
	if (debug) Melder_casual ("Before SoundConverterGetBufferSizes:\n"
		"   outputBufferSize = %ld", outputBufferSize);
	SoundConverterGetBufferSizes (soundConverter, outputBufferSize, & numberOfInputFrames,
		& inputBufferSize, & outputBufferSize);
	inputBufferHandle = NewHandle (inputBufferSize);
	if (inputBufferHandle == NULL) {
		Melder_error ("No room for input buffer.");
		goto end;
	}
	if (debug) Melder_casual ("Between SoundConverterGetBufferSizes and GetMediaSample:\n"
		"   numberOfInputFrames = %ld\n   inputBufferSize = %ld\n   outputBufferSize = %ld",
		numberOfInputFrames, inputBufferSize, outputBufferSize);
	err = GetMediaSample (media,
		inputBufferHandle, inputBufferSize,
		& inputNumberOfBytes,
		0,   /* Start time. */
		& actualTime, & durationPerSample, NULL, NULL, numberOfMediaSamples*10, & actualNumberOfSamples, NULL);
	if (err != noErr) {
		Melder_error ("Cannot get media samples.");
		goto end;
	}
	if (debug) Melder_casual ("Between GetMediaSample and SoundConverterBeginConversion:\n"
		"   inputBufferSize = %ld\n   inputNumberOfBytes = %ld\n   actualTime = %ld\n"
		"   durationPerSample = %ld\n   actualNumberOfMediaSamples = %ld",
		inputBufferSize, inputNumberOfBytes, actualTime, durationPerSample, actualNumberOfSamples);
	/*compressionID = (*hSoundDescription) -> compressionID;*/
	err = SoundConverterBeginConversion (soundConverter);
	if (err != noErr) {
		Melder_error ("Cannot begin sound conversion.");
		goto end;
	}
	HLock (inputBufferHandle);
	err = SoundConverterConvertBuffer (soundConverter,
		*inputBufferHandle, numberOfInputFrames,
		& my z [1] [1], & numberOfOutputFrames, & numberOfOutputBytes);
	HUnlock (inputBufferHandle);
	if (err != noErr) {
		Melder_error ("Cannot convert sound. Error #%ld. "
			"%ld frames in, %ld frames out, %ld bytes out.", (long) err,
			numberOfInputFrames, numberOfOutputFrames, numberOfOutputBytes);
		goto end;
	}
/*	err = SoundConverterEndConversion (soundConverter, & my z [1] [1], & numberOfOutputFrames, & numberOfOutputBytes);
	if (err != noErr) {
		Melder_error ("Cannot end sound conversion.");
		goto end;
	}*/
	if (numberOfOutputBytes != my nx * 2) {
		Melder_error ("Promised %ld samples, but got %ld after conversion.", my nx, numberOfOutputBytes / 2);
		goto end;
	}
	for (isamp = my nx; isamp > 0; isamp --) {
		my z [1] [isamp] = ((short *) & my z [1] [1]) [isamp - 1] / 32768.0;
	}
end:
	if (extension) DisposeHandle (extension);
	if (hSoundDescription) DisposeHandle ((Handle) hSoundDescription);
	if (movie) DisposeMovie (movie);
	if (inputBufferHandle) DisposeHandle (inputBufferHandle);
	if (decompressionAtom) DisposePtr ((Ptr) decompressionAtom);
	if (soundConverter) SoundConverterClose (soundConverter);
	if (refNum) CloseMovieFile (refNum);
	iferror forget (me);
#else
	Melder_error ("This edition of Praat cannot handle movie files.");
#endif
	return me;
}

Sound Sound_readFromBellLabsFile (MelderFile fs) {
	Sound me = NULL;
	FILE *f = NULL;
	char tag [200], *endOfTag, *lines = NULL, *psamples, *pfrequency;
	long numberOfSamples = 0, tagLength, headerLength, i;
	double samplingFrequency = 0.0;

	/*
	 * Check existence and permissions of file.
	 */
	if ((f = Melder_fopen (fs, "rb")) == NULL) goto error;

	/*
	 * Check identity of file: first line is "SIG", second line contains a number.
	 */
	if (fread (tag, 1, 16, f) < 16 || ! strnequ (tag, "SIG\n", 4))
		{ Melder_error ("Not a Bell-Labs sound file."); goto error; }
	if ((endOfTag = strchr (tag + 4, '\n')) == NULL)
		{ Melder_error ("Second line missing or too long."); goto error; }
	tagLength = endOfTag - tag + 1;   /* Probably 12. */
	if ((headerLength = atol (tag + 4)) <= 0)
		{ Melder_error ("Wrong header-length info."); goto error; }

	/*
	 * Read data from header.
	 * Use defaults if necessary.
	 */
	if ((lines = Melder_calloc (1, headerLength + 1)) == NULL) goto error;
	if (fread (lines, 1, headerLength, f) < headerLength)
		{ Melder_error ("Header too short."); goto error; }
	psamples = lines - 1;
	while ((psamples = strstr (psamples + 1, "samples ")) != NULL)   /* Take last occurrence. */
		numberOfSamples = atol (psamples + 8);
	if (numberOfSamples < 1) {
		/* Use file length. */
		fseek (f, 0, SEEK_END);   /* Position file pointer at end of file. */
		numberOfSamples = (ftell (f) - tagLength - headerLength) / 2;
	}
	if (numberOfSamples < 1)
		{ Melder_error ("No samples found."); goto error; }
	pfrequency = lines - 1;
	while ((pfrequency = strstr (pfrequency + 1, "frequency ")) != NULL)   /* Take last occurrence. */
		samplingFrequency = atof (pfrequency + 10);
	if (samplingFrequency <= 0.0)
		samplingFrequency = 16000.0;

	/*
	 * Create sound.
	 */
	if (! (me = Sound_createSimple (numberOfSamples / samplingFrequency, samplingFrequency))) goto error;

	/*
	 * Read samples.
	 */
	fseek (f, tagLength + headerLength, SEEK_SET);
	for (i = 1; i <= numberOfSamples; i ++) my z [1] [i] = bingeti2 (f) * (1.0 / 32768); /* 16 bits Big-Endian. */

	if (fclose (f) != 0)
		{ Melder_error ("Error reading file."); goto error; }
	Melder_free (lines);
	return me;
error:
	if (f) fclose (f);
	Melder_free (lines);
	return Melder_errorp ("(Sound_readFromBellLabsFile:) File \"%s\" not read.", MelderFile_messageName (fs));
}

static Sound readError (MelderFile file) {
	return Melder_errorp ("(Sound_readFrom...File:) Cannot read file \"%s\".", MelderFile_messageName (file));
}

Sound Sound_readFromKayFile (MelderFile fs) {
	Sound me = NULL;
	long i;
	char data [100];
	double samplingFrequency;
	long chunkSize, numberOfSamples;
	FILE *f;

	if ((f = Melder_fopen (fs, "rb")) == NULL) return 0;

	/* Read header of KAY file: 12 bytes. */

	if (fread (data, 1, 12, f) < 12) return readError (fs);
	if (! strnequ (data, "FORMDS16", 8)) {
		fclose (f);
		return Melder_errorp ("(Sound_readFromKayFile:) File \"%s\" is not a KAY DS-16 file.", MelderFile_messageName (fs));
	}

	/* HEDR chunk */

	if (fread (data, 1, 4, f) < 4) return readError (fs);
	if (! strnequ (data, "HEDR", 4)) {
		fclose (f);
		return Melder_errorp ("(Sound_readFromKayFile:) File \"%s\" does not contain HEDR chunk.", MelderFile_messageName (fs));
	}
	chunkSize = bingeti4LE (f);
	if (chunkSize & 1) ++ chunkSize;
	if (fread (data, 1, chunkSize - 12, f) < chunkSize - 12) return readError (fs);
	samplingFrequency = bingeti4LE (f);
	numberOfSamples = bingeti4LE (f);
	if (samplingFrequency <= 0 || samplingFrequency > 1e7 ||
	    numberOfSamples <= 0 || numberOfSamples >= 1000000000) {
		fclose (f);
		return Melder_errorp ("(Sound_readFromKayFile:) File \"%s\" is not a correct Kay file.", MelderFile_messageName (fs));
	}
	if (fread (data, 1, 4, f) < 4) return readError (fs);   /* Absolute extrema A/B. */

	/* SD chunk */

	if (fread (data, 1, 4, f) < 4) return readError (fs);
	while (! strnequ (data, "SDA_", 4) && ! strnequ (data, "SD_B", 4)) {
		if (feof (f)) {
			fclose (f);
			return Melder_errorp ("(Sound_readFromKayFile:) File \"%s\" does not contain readable SD chunk.", MelderFile_messageName (fs));
		}
		chunkSize = bingeti4LE (f);
		if (chunkSize & 1) ++ chunkSize;
		if (fread (data, 1, chunkSize, f) < chunkSize) return readError (fs);
		if (fread (data, 1, 4, f) < 4) return readError (fs);
	}
	chunkSize = bingeti4LE (f);
	if (chunkSize != numberOfSamples * 2) {
		fclose (f);
		return Melder_errorp ("(Sound_readFromKayFile:) File \"%s\" does not contain readable SD chunk.", MelderFile_messageName (fs));
	}

	me = Sound_createSimple (numberOfSamples / samplingFrequency, samplingFrequency);
	if (! me) return NULL;
	for (i = 1; i <= numberOfSamples; i ++)
		my z [1] [i] = bingeti2LE (f) / 32768.0;
	fclose (f);
	return me;
}

Sound Sound_readFromRawAlawFile (MelderFile file) {
	Sound me = NULL;
	long numberOfSamples;
	double sampleRate = 8000.0;
	FILE *f = Melder_fopen (file, "rb"); cherror
	fseek (f, 0, SEEK_END);
	numberOfSamples = ftell (f);
	rewind (f);
	me = Sound_createSimple (numberOfSamples / sampleRate, sampleRate); cherror
	Melder_readAudioToFloat (f, 1, Melder_ALAW, my z [1], NULL, numberOfSamples); cherror
end:
	Melder_fclose (file, f);
	iferror { Melder_error ("(Sound_readFromRawAlawFile:) File %s not read.", MelderFile_messageName (file)); forget (me); }
	return me;
}

int Sound_writeToAudioFile16 (Sound me, Sound right, MelderFile file, int audioFileType) {
	long n = right == NULL || my nx > right -> nx ? my nx : right -> nx;
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), "PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer) {
		Melder_writeAudioFileHeader16 (file -> filePointer, audioFileType, floor (1.0 / my dx + 0.5), n, right ? 2 : 1);
		Melder_writeFloatToAudio (file -> filePointer, Melder_defaultAudioFileEncoding16 (audioFileType),
			& my z [1] [1], my nx, right ? & right -> z [1] [1] : NULL, right ? right -> nx : 0, TRUE);
	}
	MelderFile_close (file);
	iferror return 0;
	return 1;
}

int Sound_writeToSesamFile (Sound me, MelderFile file) {
	FILE *f = Melder_fopen (file, "wb");
	long header [1 + 128], i, tail;
	if (! f) return 0;
	MelderFile_setMacTypeAndCreator (file, 'BINA', 'PpgB');
	for (i = 1; i <= 128; i ++) header [i] = 0;
	/* ILS header. */
		header [6] = ((my nx - 1) >> 8) + 1;   /* Number of disk blocks. */
		header [64] = 32149;   /* ILS magic. */
	/* LVS header. */
		header [62] = floor (1 / my dx + 0.5);   /* Sampling frequency, rounded to n Hz. */
		header [63] = -32000;   /* Magic: "sampled signal." */
		header [66] = 2047;   /* Maximum absolute value: 12 bits. */
		header [67] = 2047;   /* LVS magic. */
		header [68] = my nx % 256;   /* Number of samples in last block. */
		header [69] = 1;   /* ? */
	/* Sesam header. */
		header [126] = floor (1 / my dx + 0.5);   /* Sampling frequency, rounded to n Hz. */
		header [127] = my nx;   /* Number of samples. */
	Melder_warning ("Sound_writeToSesamFile: writing %ld samples at %g Hz", my nx, 1 / my dx);
	for (i = 1; i <= 128; i ++) binputi4LE (header [i], f);
	for (i = 1; i <= my nx; i ++) binputi2LE (floor (my z [1] [i] * 2048 + 0.5), f);
	tail = 256 - my nx % 256;
	if (tail == 256) tail = 0;
	for (i = 1; i <= tail; i ++) binputi2LE (0, f);   /* Pad last block with zeroes. */
	if (fclose (f) == EOF) return Melder_error ("Error writing file \"%s\".", MelderFile_messageName (file));
	return 1;
}

#ifdef macintosh
int Sound_writeToMacSoundFile (Sound me, MelderFile file) {
	int path;
	FSSpec fspec;
	long numberOfSamples = my nx;
	SndResourceHandle dataH = (SndResourceHandle) NewHandle (42 + numberOfSamples);
	SndResourcePtr data;
	long i;
	float *from;
	unsigned char *to;
	if (! dataH) return Melder_error ("Sound_writeToMacSoundFile: not enough memory.");
	data = *dataH;
	data -> formatType = 1;
	data -> numberOfSynthesizers = 1;
	data -> resourceIDOfSynthesizer = sampledSynth;
	data -> initializationOption = initMono;
	data -> numberOfSoundCommands = 1;
	data -> itsSndCommand. cmd = bufferCmd + dataOffsetFlag;
	data -> itsSndCommand. param1 = 0;
	data -> itsSndCommand. param2 = 20;
	data -> itsSndHeader. samplePtr = NULL;
	data -> itsSndHeader. length = numberOfSamples;
	data -> itsSndHeader. sampleRate =
		(Fixed) (long) floor (65536.0 / my dx);
	data -> itsSndHeader. loopStart = 0L;
	data -> itsSndHeader. loopEnd = 0L;
	data -> itsSndHeader. encode = stdSH;
	data -> itsSndHeader. baseFrequency = 60;
	from = my z [1];
	to = (unsigned char *) & data -> itsSndHeader. sampleArea - 1;
	for (i = 1; i <= numberOfSamples; i ++) to [i] = (int) (from [i] * 128 + 128);

	Melder_fileToMac (file, & fspec);
	FSpDelete (& fspec);   /* Overwrite existing file with same name. */
	/* FSpCreate (& fspec, 'PpgB', 'sfil', smSystemScript);   /* File type is 'sfil' (sound file). */
	FSpCreateResFile (& fspec, 'PpgB', 'sfil', smSystemScript);   /* Make a resource fork... */
	path = FSpOpenResFile (& fspec, fsWrPerm);   /* ...and open it. */

	/* Write the data to the file as an 'snd ' resource. */
	/* The Id of this resource is 128, which is the same as ResEdit would suggest. */
	/* The name of the resource will be equal to the name of the file, so that: */
	/* 1. we can play the file from the Finder with a double click. */
	/* 2. we can use it as a warning signal (control panel Sound) in the System file. */
	/* 3. we can hear when the Macintosh boots if it is in the folder "Starting up". */

	AddResource ((Handle) dataH, 'snd ', 128, fspec. name);   /* Resource manager's. */
	if (ResError () != noErr) {
		CloseResFile (path);
		return Melder_error ("Sound_writeToMacSoundFile: not enough disk space.");
	}
	SetResAttrs ((Handle) dataH, resPurgeable + resChanged);
		/* Make purgeable, like system sounds. Keep the changes. */
	CloseResFile (path);
	return 1;
}
#endif

int Sound_writeToKayFile (Sound me, MelderFile file) {
	FILE *f = Melder_fopen (file, "wb");
	long i;
	time_t today = time (NULL);
	char date [100];
	int maximum = 0;
	if (! f) return 0;
	MelderFile_setMacTypeAndCreator (file, 'BINA', 'PpgB');

	/* Form Chunk: contains all other chunks. */
	fwrite ("FORMDS16", 1, 8, f);
	binputi4LE (48 + my nx * 2, f);   /* Size of Form Chunk. */
	fwrite ("HEDR", 1, 4, f);
	binputi4LE (32, f);

	strcpy (date, ctime (& today));	
	fwrite (date+4, 1, 20, f);   /* Skip weekday. */

	binputi4LE (floor (1 / my dx + 0.5), f);   /* Sampling frequency. */
	binputi4LE (my nx, f);   /* Number of samples. */
	for (i = 1; i <= my nx; i ++) {
		long value = floor (my z [1] [i] * 32768 + 0.5);
		if (value < - maximum) maximum = - value;
		if (value > maximum) maximum = value;
	}
	binputi2LE (maximum, f);   /* Absolute maximum window A. */
	binputi2LE (-1, f);   /* Absolute maximum window B. */
	fwrite ("SDA_", 1, 4, f);
	binputi4LE (my nx * 2, f);   /* Chunk size. */

	Melder_writeFloatToAudio (f, Melder_LINEAR_16_LITTLE_ENDIAN,
		& my z [1] [1], my nx, NULL, 0, TRUE); cherror
end:
	Melder_fclose (file, f);
	iferror return 0;
	return 1;
}

int Sound_writeToRawSoundFile (Sound me, Sound right, MelderFile file, int encoding) {
	FILE *f = Melder_fopen (file, "wb");
	if (! f) return 0;
	MelderFile_setMacTypeAndCreator (file, 'BINA', 'PpgB');
	Melder_writeFloatToAudio (f, encoding,
		& my z [1] [1], my nx, right ? & right -> z [1] [1] : NULL, right ? right -> nx : 0, TRUE); cherror
end:
	Melder_fclose (file, f);
	iferror return 0;
	return 1;
}

int Sound_writeToRaw8bitSignedFile (Sound me, MelderFile file) {
	return Sound_writeToRawSoundFile (me, NULL, file, Melder_LINEAR_8_SIGNED);
}

int Sound_writeToRaw8bitUnsignedFile (Sound me, MelderFile file) {
	return Sound_writeToRawSoundFile (me, NULL, file, Melder_LINEAR_8_UNSIGNED);
}

/* End of file Sound_files.c */
