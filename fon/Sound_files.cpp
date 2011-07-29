/* Sound_files.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma & David Weenink
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
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/30 stereo
 * pb 2006/01/01 more stereo
 * pb 2007/01/28 removed a warning
 * pb 2007/01/28 made readFromMovieFile compatible with stereo
 * pb 2007/03/17 resistant against conflicting declarations of Collection
 * pb 2007/05/08 removed warning about stereo sounds
 * pb 2007/10/05 removed FSSpec
 * pb 2007/10/05 made Sound_readFromMacSoundFile compatible with sample rates between 32768 and 65535 Hz
 * pb 2008/01/19 double
 * pb 2009/09/21 made stereo movies readable
 * pb 2010/12/27 support for multiple channels (i.e. more than two)
 * pb 2011/06/07 C++
 */

/*
static void Sound_ulawDecode (Sound me) {
	double mu = 100, lnu1 = log (1 + mu);
	for (long i = 1; i <= my nx; i ++) {
		double zabs = (exp (fabs (my z [1] [i]) * lnu1) - 1.0) / mu;
		my z [1] [i] = my z [1] [i] < 0 ? -zabs : zabs;
	} 
}
static void Sound_alawDecode (Sound me) {
	double a = 87.6, lna1 = 1.0 + log (a);
	for (long i = 1; i <= my nx; i ++) {
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
#include <time.h>
#include "Sound.h"

#if defined (macintosh)
	#include "macport_on.h"
	#include <Carbon/Carbon.h>
	#include "macport_off.h"
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
#if defined (macintosh) || defined (_WIN32)
	#if defined (macintosh) && defined (__MWERKS__)
		#define DONT_INCLUDE_QUICKTIME
	#endif
	#if defined (_WIN32)
		#undef false
		#undef true
		#define TARGET_API_MAC_CARBON  0
	#endif
	#if ! defined (DONT_INCLUDE_QUICKTIME)
		#include "macport_on.h"
		#if defined (macintosh) && defined (__MWERKS__)
			typedef unsigned char Boolean;
		#endif
		#include <QuickTime.h>
		#include "macport_off.h"
	#endif
#endif
#define PtoCstr(p)  (p [p [0] + 1] = '\0', (char *) p + 1)
#define PfromCstr(p,c)  p [0] = strlen (c), strcpy ((char *) p + 1, c);
#if defined (_WIN32) && ! defined (DONT_INCLUDE_QUICKTIME)
	static void Melder_fileToMac (MelderFile file, void *void_fspec) {
		FSSpec *fspec = (FSSpec *) void_fspec;
		Str255 pname;
		OSErr err;
		char path [1000];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, path);
		PfromCstr (pname, path);
		err = FSMakeFSSpec (0, 0, & pname [0], fspec);
		if (err != noErr && err != fnfErr) {
			if (err == -2095)
				Melder_throw ("To open this movie file, you have to install QuickTime first (www.apple.com).");
			Melder_throw ("Error #", err, " looking for file ", file -> path, ".");
		}
	}
#elif defined (macintosh)
	static void Melder_fileToMac (MelderFile file, void *void_fspec) {
		char path [1000];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, path);
		FSRef fsref;
		OSStatus err = FSPathMakeRef ((unsigned char *) path, & fsref, NULL);
		if (err != noErr && err != fnfErr)
			Melder_throw ("Error #", err, " translating file name ", file -> path, ".");
		FSSpec *fspec = (FSSpec *) void_fspec;
		err = FSGetCatalogInfo (& fsref, kFSCatInfoNone, NULL, NULL, fspec, NULL);
		if (err != noErr) {
			/*
				File does not exist. Get its parent directory instead.
			*/
			structMelderDir parentDir = { { 0 } };
			char romanName [260];
			Str255 pname;
			FSCatalogInfo info;
			FSRef parentDirectory;
			MelderFile_getParentDir (file, & parentDir);
			Melder_wcsTo8bitFileRepresentation_inline (parentDir. path, path);
			err = FSPathMakeRef ((unsigned char *) path, & parentDirectory, NULL);
			if (err != noErr)
				Melder_throw ("Error #", err, " translating directory name ", parentDir. path, ".");
			err = FSGetCatalogInfo (& parentDirectory, kFSCatInfoVolume | kFSCatInfoNodeID, & info, NULL, NULL, NULL);
			if (err != noErr)
				Melder_throw ("Error #", err, " looking for directory of ", file -> path, ".");
			/*
				Convert from Unicode to MacRoman.
			*/
			CFStringGetCString ((CFStringRef) Melder_peekWcsToCfstring (MelderFile_name (file)), romanName, 260, kCFStringEncodingMacRoman);   // BUG
			PfromCstr (pname, romanName);
			err = FSMakeFSSpec (info. volume, info. nodeID, & pname [0], fspec);
			if (err != noErr && err != fnfErr)
				Melder_throw ("Error #", err, " looking for file ", file -> path, ".");
		}
	}
#endif

Sound Sound_readFromSoundFile (MelderFile file) {
	try {
		autoMelderFile mfile = MelderFile_open (file);
		int numberOfChannels, encoding;
		double sampleRate;
		long startOfData, numberOfSamples;
		int fileType = MelderFile_checkSoundFile (file, & numberOfChannels, & encoding, & sampleRate, & startOfData, & numberOfSamples);
		if (fileType == 0)
			Melder_throw ("Not an audio file.");
		if (fseek (file -> filePointer, startOfData, SEEK_SET) == EOF)   // start from beginning of Data Chunk
			Melder_throw ("No data in audio file.");
		autoSound me = Sound_createSimple (numberOfChannels, numberOfSamples / sampleRate, sampleRate);
		if (encoding == Melder_SHORTEN || encoding == Melder_POLYPHONE)
			Melder_throw ("Cannot unshorten. Write to paul.boersma@uva.nl for more information.");
		Melder_readAudioToFloat (file -> filePointer, numberOfChannels, encoding, my z, numberOfSamples); therror
		mfile.close ();
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not read from sound file ", MelderFile_messageName (file), ".");
	}
}

Sound Sound_readFromSesamFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		long header [1 + 128];
		for (long i = 1; i <= 128; i ++)
			header [i] = bingeti4LE (f);
		/*
		 * Try SESAM header.
		 */
		double samplingFrequency = header [126];
		long numberOfSamples = header [127];
		if (samplingFrequency == 0.0 || numberOfSamples == 0) {
			/*
			 * Try LVS header.
			 */
			samplingFrequency = header [62];
			numberOfSamples = (header [6] << 8) - header [68];
		}
		if (numberOfSamples < 1 || numberOfSamples > 1000000000 || samplingFrequency < 10.0 || samplingFrequency > 100000000.0)
			Melder_throw ("Not a correct SESAM or LVS file.");
		autoSound me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency);
		for (long i = 1; i <= numberOfSamples; i ++) {
			my z [1] [i] = bingeti2LE (f) * (1.0 / 2048);   // 12 bits
		}
		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not read from Sesam file ", MelderFile_messageName (file), ".");
	}
}

#ifdef macintosh
Sound Sound_readFromMacSoundFile (MelderFile file) {
	try {
		FSRef fsRef;
		Melder_fileToMach (file, & fsRef);
		int path = FSOpenResFile (& fsRef, fsRdPerm);   // open resource fork; that's where the sounds are
		if (path == -1)
			Melder_throw ("Error opening resource fork.");
		if (Count1Resources ('snd ') == 0) {   // are there really any sounds in this file?
			CloseResFile (path);
			Melder_throw ("No sound resources found.");
		}
		Handle han = Get1IndResource ('snd ', 1);   // take the first sound from this file
		if (! han) {
			CloseResFile (path);
			Melder_throw ("Sound too large.");
		}
		DetachResource (han);   // release the sound's binding with the Resource Map
		CloseResFile (path);   // remove the Resource Map; the sound is ours now
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
			Melder_throw ("The sound has an unknown format.");
		}
		SoundHeader *header = & (**(SndResourceHandle) han).itsSndHeader;
		long numberOfSamples = header -> length;
		double samplingFrequency = (unsigned long) header -> sampleRate / 65536.0;
		autoSound me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency);
		header = & (**(SndResourceHandle) han).itsSndHeader;   // do not move memory
		unsigned const char *from = (unsigned const char *) & header -> sampleArea - 1;
		double *to = my z [1];
		for (long i = 1; i <= numberOfSamples; i ++)
			to [i] = (from [i] - 128) * (1.0 / 128);
		DisposeHandle (han);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not read from Mac sound file ", MelderFile_messageName (file), ".");
	}
}
#endif

Sound Sound_readFromMovieFile (MelderFile file) {
	#if (defined (macintosh) || defined (_WIN32)) && ! defined (DONT_INCLUDE_QUICKTIME)
		Handle extension = NULL;
		SoundDescriptionHandle hSoundDescription = NULL;   // not NewHandle yet (on Windows, initialize QuickTime first)
		Movie movie = 0;
		Handle inputBufferHandle = NULL;
		AudioFormatAtomPtr decompressionAtom = NULL;
		SoundConverter soundConverter = NULL;
		short refNum = 0;
	#endif
	try {
		#if (defined (macintosh) || defined (_WIN32)) && ! defined (DONT_INCLUDE_QUICKTIME)
			int debug = Melder_debug == 35;
			short resourceID = 0;
			OSErr err = noErr;
			MacintoshBoolean wasChanged;
			long numberOfMediaSamples, numberOfSamplesPerMediaSample;
			short numberOfChannels, sampleSize;
			double samplingFrequency;
			unsigned long inputBufferSize, outputBufferSize;
			unsigned long numberOfInputFrames, numberOfOutputFrames, numberOfOutputBytes;
			TimeValue actualTime, durationPerSample;
			long inputNumberOfBytes, actualNumberOfSamples;
			SoundComponentData input, output;

			#ifdef _WIN32
				InitializeQTML (0);
			#endif
			EnterMovies ();
			FSSpec fspec;
			Melder_fileToMac (file, & fspec);
			err = OpenMovieFile (& fspec, & refNum, fsRdPerm);
			if (err != noErr)
				Melder_throw ("Not a movie file.");
			err = NewMovieFromFile (& movie, refNum, & resourceID, NULL, newMovieActive, & wasChanged);
			if (err != noErr)
				Melder_throw ("Cannot find the movie.");
			Track track = GetMovieIndTrackType (movie, 1, SoundMediaType, movieTrackMediaType);
			if (track == NULL)
				Melder_throw ("Invalid track.");
			Media media = GetTrackMedia (track);
			if (media == NULL)
				Melder_throw ("Invalid media.");
			hSoundDescription = (SoundDescriptionHandle) NewHandle (0);
			numberOfMediaSamples = GetMediaSampleCount (media);
			double duration = (double) GetMediaDuration (media) / GetMediaTimeScale (media);
			GetMediaSampleDescription (media, 1, (SampleDescriptionHandle) hSoundDescription);
			if (GetMoviesError () != noErr)
				Melder_throw ("Cannot get sound description.");
			err = GetSoundDescriptionExtension (hSoundDescription, & extension, siDecompressionParams);
			if (err == noErr) {
				Size size = GetHandleSize (extension);
				HLock (extension);
				decompressionAtom = (AudioFormatAtomPtr) NewPtr (size);
				err = MemError ();
				if (err != noErr)
					Melder_throw ("No memory left.");
				BlockMoveData (*extension, decompressionAtom, size);
				HUnlock (extension);
			} else {
				err = noErr;   // no atom: OK
			}
			numberOfChannels = (*hSoundDescription) -> numChannels;
			sampleSize = (*hSoundDescription) -> sampleSize;
			samplingFrequency = (double) (*hSoundDescription) -> sampleRate / 65536.0;
			autoSound me = Sound_createSimple (numberOfChannels, duration, samplingFrequency);
			numberOfSamplesPerMediaSample = my nx / numberOfMediaSamples;
			if (my nx % numberOfMediaSamples != 0)
				Melder_throw ("Media samples not equally long: ", my nx, " / ", numberOfMediaSamples, L" gives a remainder.");
			/*
			 * The sound converter has to decompress the sound data to 16-bit mono.
			 */
			input. flags = output. flags = 0;
			input. format = (*hSoundDescription) -> dataFormat;
			output. format = kSoundNotCompressed;
			input. numChannels = (*hSoundDescription) -> numChannels;
			if (debug) Melder_casual ("File number of channels: %d", input. numChannels);
			output. numChannels = input. numChannels;
			input. sampleSize = (*hSoundDescription) -> sampleSize;
			output. sampleSize = 16;
			input. sampleRate = output. sampleRate = (*hSoundDescription) -> sampleRate;
			input. sampleCount = output. sampleCount = 0;
			input. buffer = output. buffer = NULL;
			input. reserved = output. reserved = 0;
			err = SoundConverterOpen (& input, & output, & soundConverter);
			if (err != noErr)
				Melder_throw ("Cannot open sound converter.");
			err = SoundConverterSetInfo (soundConverter, siClientAcceptsVBR, (Ptr) true);
			if (err != noErr && err != siUnknownInfoType)
				Melder_throw ("Don't like VBR.");
			err = SoundConverterSetInfo (soundConverter, siDecompressionParams, decompressionAtom);
			if (err != noErr && err != siUnknownInfoType)
				Melder_throw ("Don't like that decompression.");
			outputBufferSize = my ny * my nx * 2;
			if (debug) Melder_casual ("Before SoundConverterGetBufferSizes:\n"
				"   outputBufferSize = %ld", outputBufferSize);
			SoundConverterGetBufferSizes (soundConverter, outputBufferSize, & numberOfInputFrames,
				& inputBufferSize, & outputBufferSize);
			inputBufferHandle = NewHandle (inputBufferSize);
			if (inputBufferHandle == NULL)
				Melder_throw ("No room for input buffer.");
			if (debug) Melder_casual ("Between SoundConverterGetBufferSizes and GetMediaSample:\n"
				"   numberOfInputFrames = %ld\n   inputBufferSize = %ld\n   outputBufferSize = %ld",
				numberOfInputFrames, inputBufferSize, outputBufferSize);
			err = GetMediaSample (media,
				inputBufferHandle, inputBufferSize,
				& inputNumberOfBytes,
				0,   /* Start time. */
				& actualTime, & durationPerSample, NULL, NULL, numberOfMediaSamples*10, & actualNumberOfSamples, NULL);
			if (err != noErr)
				Melder_throw ("Cannot get media samples.");
			if (debug) Melder_casual ("Between GetMediaSample and SoundConverterBeginConversion:\n"
				"   inputBufferSize = %ld\n   inputNumberOfBytes = %ld\n   actualTime = %ld\n"
				"   durationPerSample = %ld\n   actualNumberOfMediaSamples = %ld",
				inputBufferSize, inputNumberOfBytes, actualTime, durationPerSample, actualNumberOfSamples);
			/*compressionID = (*hSoundDescription) -> compressionID;*/
			err = SoundConverterBeginConversion (soundConverter);
			if (err != noErr)
				Melder_throw ("Cannot begin sound conversion.");

			autoNUMvector <short> buffer (0L, numberOfChannels * my nx);
			HLock (inputBufferHandle);
			err = SoundConverterConvertBuffer (soundConverter,
				*inputBufferHandle, numberOfInputFrames,
				buffer.peek(), & numberOfOutputFrames, & numberOfOutputBytes);
			HUnlock (inputBufferHandle);
			if (err != noErr)
				Melder_throw ("Cannot convert sound. Error #", err, ". ", numberOfInputFrames,
					" frames in, ", numberOfOutputFrames, " frames out, ", numberOfOutputBytes, " bytes out.");
		/*	err = SoundConverterEndConversion (soundConverter, & my z [1] [1], & numberOfOutputFrames, & numberOfOutputBytes);
			if (err != noErr)
				Melder_throw ("Cannot end sound conversion.");

			*/
			if (numberOfOutputBytes != my ny * my nx * 2)
				Melder_throw ("Promised ", my nx, " samples, but got ", numberOfOutputBytes / 2, " after conversion.");
			for (long channel = 1; channel <= my ny; channel ++) {
				for (long isamp = my nx; isamp > 0; isamp --) {
					my z [channel] [isamp] = buffer [(isamp - 1) * numberOfChannels + channel - 1] / 32768.0;
				}
			}
			DisposeHandle (extension);
			DisposeHandle ((Handle) hSoundDescription);
			DisposeMovie (movie);
			DisposeHandle (inputBufferHandle);
			if (decompressionAtom) DisposePtr ((Ptr) decompressionAtom);
			SoundConverterClose (soundConverter);
			CloseMovieFile (refNum);
			return me.transfer();
		#else
			Melder_throw ("This edition of Praat cannot handle movie files.");
			return NULL;
		#endif
	} catch (MelderError) {
		#if (defined (macintosh) || defined (_WIN32)) && ! defined (DONT_INCLUDE_QUICKTIME)
			if (extension) DisposeHandle (extension);
			if (hSoundDescription) DisposeHandle ((Handle) hSoundDescription);
			if (movie) DisposeMovie (movie);
			if (inputBufferHandle) DisposeHandle (inputBufferHandle);
			if (decompressionAtom) DisposePtr ((Ptr) decompressionAtom);
			if (soundConverter) SoundConverterClose (soundConverter);
			if (refNum) CloseMovieFile (refNum);
		#endif
		Melder_throw ("Sound not read from movie file ", MelderFile_messageName (file), ".");
	}
}

Sound Sound_readFromBellLabsFile (MelderFile file) {
	try {
		/*
		 * Check existence and permissions of file.
		 */
		autofile f = Melder_fopen (file, "rb");

		/*
		 * Check identity of file: first line is "SIG", second line contains a number.
		 */
		char tag [200];
		if (fread (tag, 1, 16, f) < 16 || ! strnequ (tag, "SIG\n", 4))
			Melder_throw ("Not a Bell-Labs sound file.");
		char *endOfTag = strchr (tag + 4, '\n');
		if (endOfTag == NULL)
			Melder_throw ("Second line missing or too long.");
		unsigned long tagLength = endOfTag - tag + 1;   // probably 12
		unsigned long headerLength = atol (tag + 4);
		if (headerLength <= 0)
			Melder_throw ("Wrong header-length info.");

		/*
		 * Read data from header.
		 * Use defaults if necessary.
		 */
		autostring8 lines = Melder_calloc (char, headerLength + 1);
		if (fread (lines.peek(), 1, headerLength, f) < headerLength)
			Melder_throw ("Header too short.");
		unsigned long numberOfSamples = 0;
		char *psamples = & lines [-1];
		while ((psamples = strstr (psamples + 1, "samples ")) != NULL)   // take last occurrence
			numberOfSamples = atol (psamples + 8);
		if (numberOfSamples < 1) {
			/* Use file length. */
			fseek (f, 0, SEEK_END);   /* Position file pointer at end of file. */
			numberOfSamples = (ftell (f) - tagLength - headerLength) / 2;
		}
		if (numberOfSamples < 1)
			Melder_throw ("No samples found.");
		double samplingFrequency = 0.0;
		char *pfrequency = & lines [-1];
		while ((pfrequency = strstr (pfrequency + 1, "frequency ")) != NULL)   // take last occurrence
			samplingFrequency = atof (pfrequency + 10);
		if (samplingFrequency <= 0.0)
			samplingFrequency = 16000.0;

		/*
		 * Create sound.
		 */
		autoSound me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency);

		/*
		 * Read samples.
		 */
		fseek (f, tagLength + headerLength, SEEK_SET);
		for (long i = 1; i <= numberOfSamples; i ++)
			my z [1] [i] = bingeti2 (f) * (1.0 / 32768);   // 16-bits big-endian

		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not read from Bell Labs sound file ", MelderFile_messageName (file), ".");
	}
}

static void readError () {
	Melder_throw ("Error reading bytes from file.");
}

Sound Sound_readFromKayFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");

		/* Read header of KAY file: 12 bytes. */

		char data [100];
		if (fread (data, 1, 12, f) < 12) readError ();
		if (! strnequ (data, "FORMDS16", 8))
			Melder_throw ("Not a KAY DS-16 file.");

		/* HEDR chunk */

		if (fread (data, 1, 4, f) < 4) readError ();
		if (! strnequ (data, "HEDR", 4))
			Melder_throw ("Missing HEDR chunk.");
		unsigned long chunkSize = bingetu4LE (f); therror
		if (chunkSize & 1) ++ chunkSize;
		if (fread (data, 1, chunkSize - 12, f) < chunkSize - 12) readError ();
		double samplingFrequency = bingetu4LE (f); therror
		unsigned long numberOfSamples = bingetu4LE (f); therror
		if (samplingFrequency <= 0 || samplingFrequency > 1e7 || numberOfSamples >= 1000000000)
			Melder_throw ("Not a correct Kay file.");
		if (fread (data, 1, 4, f) < 4) readError ();   // absolute extrema A/B

		/* SD chunk */

		if (fread (data, 1, 4, f) < 4) readError ();
		while (! strnequ (data, "SDA_", 4) && ! strnequ (data, "SD_B", 4)) {
			if (feof (f))
				Melder_throw ("Missing or unreadable SD chunk.");
			chunkSize = bingetu4LE (f); therror
			if (chunkSize & 1) ++ chunkSize;
			if (fread (data, 1, chunkSize, f) < chunkSize) readError ();
			if (fread (data, 1, 4, f) < 4) readError ();
		}
		chunkSize = bingetu4LE (f); therror
		if (chunkSize != numberOfSamples * 2)
			Melder_throw ("Incomplete SD chunk.");

		autoSound me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency);
		for (unsigned long i = 1; i <= numberOfSamples; i ++) {
			my z [1] [i] = bingeti2LE (f) / 32768.0; therror
		}
		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not read from Kay file ", MelderFile_messageName (file), ".");
	}
}

Sound Sound_readFromRawAlawFile (MelderFile file) {
	try {
		double sampleRate = 8000.0;
		autofile f = Melder_fopen (file, "rb");
		fseek (f, 0, SEEK_END);
		long numberOfSamples = ftell (f);
		rewind (f);
		autoSound me = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate); 
		Melder_readAudioToFloat (f, 1, Melder_ALAW, my z, numberOfSamples); therror
		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not read from raw A-law file ", MelderFile_messageName (file), ".");
	}
}

void Sound_writeToAudioFile16 (Sound me, MelderFile file, int audioFileType) {
	try {
		autoMelderFile mfile = MelderFile_create (file, Melder_macAudioFileType (audioFileType), L"PpgB", Melder_winAudioFileExtension (audioFileType));
		MelderFile_writeAudioFileHeader16 (file, audioFileType, floor (1.0 / my dx + 0.5), my nx, my ny); therror
		MelderFile_writeFloatToAudio (file, my ny, Melder_defaultAudioFileEncoding16 (audioFileType), my z, my nx, TRUE); therror
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, ": not written to 16-bit sound file ", MelderFile_messageName (file), ".");
	}
}

void Sound_writeToSesamFile (Sound me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "wb");
		long header [1 + 128], tail;
		MelderFile_setMacTypeAndCreator (file, 'BINA', 'PpgB');
		for (long i = 1; i <= 128; i ++) header [i] = 0;
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
		for (long i = 1; i <= 128; i ++) binputi4LE (header [i], f);
		for (long i = 1; i <= my nx; i ++) binputi2LE (floor (my z [1] [i] * 2048 + 0.5), f);
		tail = 256 - my nx % 256;
		if (tail == 256) tail = 0;
		for (long i = 1; i <= tail; i ++) binputi2LE (0, f);   /* Pad last block with zeroes. */
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, ": not written to Sesam file ", MelderFile_messageName (file), ".");
	}
}

#ifdef macintosh
void Sound_writeToMacSoundFile (Sound me, MelderFile file) {
	try {
		long numberOfSamples = my nx;
		SndResourceHandle dataH = (SndResourceHandle) NewHandle (42 + numberOfSamples);
		if (! dataH)
			Melder_throw ("Not enough memory.");
		SndResourcePtr data = *dataH;
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
		data -> itsSndHeader. sampleRate = (Fixed) (unsigned long) floor (65536.0 / my dx);
		data -> itsSndHeader. loopStart = 0L;
		data -> itsSndHeader. loopEnd = 0L;
		data -> itsSndHeader. encode = stdSH;
		data -> itsSndHeader. baseFrequency = 60;
		double *from = my z [1];
		unsigned char *to = (unsigned char *) & data -> itsSndHeader. sampleArea - 1;
		for (long i = 1; i <= numberOfSamples; i ++)
			to [i] = (int) (from [i] * 128 + 128);

		HFSUniStr255 resourceForkName;
		FSGetResourceForkName (& resourceForkName);
		FSRef fsRef;
		char pathUtf8 [1000];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, pathUtf8);
		OSStatus err = FSPathMakeRef ((unsigned char *) pathUtf8, & fsRef, NULL);
		if (err == fnfErr) {
			structMelderDir dir;
			MelderFile_getParentDir (file, & dir);
			FSRef parentRef;
			Melder_dirToMach (& dir, & parentRef);
			OSErr err = FSCreateResourceFile (& parentRef, wcslen (MelderFile_name (file)) /* BUG */,
				Melder_peekWcsToUtf16 (MelderFile_name (file)), 0, NULL,
				resourceForkName. length, resourceForkName. unicode, & fsRef, NULL);
			if (err != noErr)
				Melder_throw ("Error #", err, " when trying to create a Mac sound resource file.");
		}
		Melder_fileToMach (file, & fsRef);
		FSDeleteFork (& fsRef, resourceForkName. length, resourceForkName. unicode);
		OSErr err2 = FSCreateResourceFork (& fsRef, resourceForkName. length, resourceForkName. unicode, 0);
		if (err2 == nsvErr) {
			Melder_throw ("File not found when trying to create a Mac sound resource file.");
		} else if (err2 != noErr)
			Melder_throw ("Unexpected error #", err2, L" trying to create a Mac sound file.");
		int path = FSOpenResFile (& fsRef, fsWrPerm);

		/* Write the data to the file as an 'snd ' resource. */
		/* The Id of this resource is 128, which is the same as ResEdit would suggest. */
		/* The name of the resource will be equal to the name of the file, so that: */
		/* 1. we can play the file from the Finder with a double click. */
		/* 2. we can use it as a warning signal (control panel Sound) in the System file. */
		/* 3. we can hear when the Macintosh boots if it is in the folder "Starting up". */

		AddResource ((Handle) dataH, 'snd ', 128, (unsigned char *) "sound");   /* Resource manager's. */
		if (ResError () != noErr) {
			CloseResFile (path);
			Melder_throw (L"Sound_writeToMacSoundFile: not enough disk space.");
		}
		SetResAttrs ((Handle) dataH, resPurgeable + resChanged);
			/* Make purgeable, like system sounds. Keep the changes. */
		CloseResFile (path);
	} catch (MelderError) {
		Melder_throw (me, ": not written to Mac sound file ", MelderFile_messageName (file), ".");
	}
}
#endif

void Sound_writeToKayFile (Sound me, MelderFile file) {
	try {
		autoMelderFile mfile = MelderFile_create (file, L"BINA", L"PpgB", L"KaySound");

		/* Form Chunk: contains all other chunks. */
		fwrite ("FORMDS16", 1, 8, file -> filePointer);
		binputi4LE (48 + my nx * 2, file -> filePointer);   // size of Form Chunk
		fwrite ("HEDR", 1, 4, file -> filePointer);
		binputi4LE (32, file -> filePointer);

		char date [100];
		time_t today = time (NULL);
		strcpy (date, ctime (& today));	
		fwrite (date+4, 1, 20, file -> filePointer);   // skip weekday

		binputi4LE (floor (1 / my dx + 0.5), file -> filePointer);   // sampling frequency
		binputi4LE (my nx, file -> filePointer);   // number of samples
		int maximum = 0;
		for (long i = 1; i <= my nx; i ++) {
			long value = floor (my z [1] [i] * 32768 + 0.5);
			if (value < - maximum) maximum = - value;
			if (value > maximum) maximum = value;
		}
		binputi2LE (maximum, file -> filePointer);   // absolute maximum window A
		binputi2LE (-1, file -> filePointer);   // absolute maximum window B
		fwrite ("SDA_", 1, 4, file -> filePointer);
		binputi4LE (my nx * 2, file -> filePointer);   // chunk size

		MelderFile_writeFloatToAudio (file, my ny, Melder_LINEAR_16_LITTLE_ENDIAN, my z, my nx, TRUE); therror
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, ": not written to Kay sound file ", MelderFile_messageName (file), ".");
	}
}

void Sound_writeToRawSoundFile (Sound me, MelderFile file, int encoding) {
	try {
		autoMelderFile mfile = MelderFile_create (file, L"BINA", L"PpgB", L"rawSound");
		MelderFile_writeFloatToAudio (file, my ny, encoding, my z, my nx, TRUE); therror
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, ": not written to raw sound file ", MelderFile_messageName (file), ".");
	}
}

void Sound_writeToRaw8bitSignedFile (Sound me, MelderFile file) {
	Sound_writeToRawSoundFile (me, file, Melder_LINEAR_8_SIGNED);
}

void Sound_writeToRaw8bitUnsignedFile (Sound me, MelderFile file) {
	Sound_writeToRawSoundFile (me, file, Melder_LINEAR_8_UNSIGNED);
}

/* End of file Sound_files.cpp */
