/* Sound_files.c
 *
 * Copyright (C) 1992-2009 Paul Boersma & David Weenink
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
 * pb 2007/10/05 made Sound_readFromMacSoundFile compatible with sample rates between 32768 and 65535 Hertz
 * pb 2008/01/19 double
 * pb 2009/09/21 made stereo movies readable
 */

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
	static int Melder_fileToMac (MelderFile file, void *void_fspec) {
		FSSpec *fspec = (FSSpec *) void_fspec;
		Str255 pname;
		OSErr err;
		char path [1000];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, path);
		PfromCstr (pname, path);
		err = FSMakeFSSpec (0, 0, & pname [0], fspec);
		if (err != noErr && err != fnfErr) {
			if (err == -2095) {
				return Melder_error1 (L"To open this movie file, you have to install QuickTime first (www.apple.com).");
			}
			return Melder_error5 (L"Error #", Melder_integer (err), L" looking for file ", file -> path, L".");
		}
		return 1;
	}
#elif defined (macintosh)
	static int Melder_fileToMac (MelderFile file, void *void_fspec) {
		char path [1000];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, path);
		FSRef fsref;
		OSStatus err = FSPathMakeRef ((unsigned char *) path, & fsref, NULL);
		if (err != noErr && err != fnfErr)
			return Melder_error5 (L"Error #", Melder_integer (err), L" translating file name ", file -> path, L".");
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
				return Melder_error5 (L"Error #", Melder_integer (err), L" translating directory name ", parentDir. path, L".");
			err = FSGetCatalogInfo (& parentDirectory, kFSCatInfoVolume | kFSCatInfoNodeID, & info, NULL, NULL, NULL);
			if (err != noErr)
				return Melder_error5 (L"Error #", Melder_integer (err), L" looking for directory of ", file -> path, L".");
			/*
				Convert from Unicode to MacRoman.
			*/
			CFStringGetCString (Melder_peekWcsToCfstring (MelderFile_name (file)), romanName, 260, kCFStringEncodingMacRoman);   // BUG
			PfromCstr (pname, romanName);
			err = FSMakeFSSpec (info. volume, info. nodeID, & pname [0], fspec);
			if (err != noErr && err != fnfErr)
				return Melder_error5 (L"Error #", Melder_integer (err), L" looking for file ", file -> path, L".");
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
	if (fileType == 0)
		error1 (L"Not an audio file.")
	if (fseek (f, startOfData, SEEK_SET) == EOF)   /* Start from beginning of Data Chunk. */
		error1 (L"No data in audio file.")
	me = Sound_createSimple (numberOfChannels, numberOfSamples / sampleRate, sampleRate); cherror
	if (encoding == Melder_SHORTEN || encoding == Melder_POLYPHONE) {
		fclose (f);
		if (TRUE /* ! unshorten (file, 1024, encoding == Melder_POLYPHONE, & me) */) {
			forget (me);
			return Melder_errorp1 (L"(Sound_readFromSoundFile:) Cannot unshorten. Write to paul.boersma@uva.nl for more information.");
		}
		return me;
	}
	Melder_readAudioToFloat (f, numberOfChannels, encoding, my z [1], numberOfChannels == 1 ? NULL : my z [2], numberOfSamples); cherror
end:
	if (f) fclose (f);
	iferror { Melder_error3 (L"(Sound_readFromSoundFile:) File ", MelderFile_messageName (file), L" not read."); forget (me); }
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
	if (fileType == 0)
		error1 (L"Can only read AIFF/C, WAV, NeXT/Sun, and NIST files.")
	if (encoding == Melder_SHORTEN || encoding == Melder_POLYPHONE) {
		fclose (f);
		return Melder_error1 (L"(Sound_read2FromSoundFile:) Cannot unshorten two channels.");
	}
	if (fseek (f, startOfData, SEEK_SET) == EOF)   /* Start from beginning of sample data. */
		error1 (L"No data in audio file.")
	left = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate); cherror
	if (numberOfChannels == 2) {
		right = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate); cherror
	}
	Melder_readAudioToFloat (f, numberOfChannels, encoding, left -> z [1], right ? right -> z [1] : NULL, numberOfSamples); cherror
end:
	if (f) fclose (f);
	iferror { Melder_error3 (L"(Sound_read2FromSoundFile:) File ", MelderFile_messageName (file), L" not read."); forget (left); forget (right); }
	*return_left = left;
	*return_right = right;
	return 1;
}

Sound Sound_readFromSesamFile (MelderFile file) {
	Sound me = NULL;
	FILE *f = Melder_fopen (file, "rb");
	long header [1 + 128];
	double samplingFrequency;
	long numberOfSamples;
	if (! f) return 0;
	for (long i = 1; i <= 128; i ++) header [i] = bingeti4LE (f);
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
		return Melder_errorp3 (L"(Sound_readFromSesamFile:) "
			"File ", MelderFile_messageName (file), L" is not a correct SESAM or LVS file.");
	}
	me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency);
	if (! me) return NULL;
	for (long i = 1; i <= numberOfSamples; i ++) my z [1] [i] = bingeti2LE (f) * (1.0 / 2048);   /* 12 bits. */
	if (fclose (f) == EOF) return Melder_errorp3 (L"Error reading file ", MelderFile_messageName (file), L".");
	return me;
}

Sound Sound_readFromBdfFile (MelderFile file, bool isBdfFile) {
	Sound me = NULL;
	FILE *f = Melder_fopen (file, "rb");
	char buffer [81];
	fread (buffer, 1, 8, f); buffer [8] = '\0';
	fread (buffer, 1, 80, f); buffer [80] = '\0';
	Melder_casual ("Local subject identification: \"%s\"", buffer);
	fread (buffer, 1, 80, f); buffer [80] = '\0';
	Melder_casual ("Local recording identification: \"%s\"", buffer);
	fread (buffer, 1, 8, f); buffer [8] = '\0';
	Melder_casual ("Start date of recording: \"%s\"", buffer);
	fread (buffer, 1, 8, f); buffer [8] = '\0';
	Melder_casual ("Start time of recording: \"%s\"", buffer);
	fread (buffer, 1, 8, f); buffer [8] = '\0';
	long numberOfBytesInHeaderRecord = atol (buffer);
	Melder_casual ("Number of bytes in header record: %ld", numberOfBytesInHeaderRecord);
	fread (buffer, 1, 44, f); buffer [44] = '\0';
	Melder_casual ("Version of data format: \"%s\"", buffer);
	fread (buffer, 1, 8, f); buffer [8] = '\0';
	long numberOfDataRecords = atol (buffer);
	Melder_casual ("Number of data records: %ld", numberOfDataRecords);
	fread (buffer, 1, 8, f); buffer [8] = '\0';
	double durationOfDataRecord = atof (buffer);
	Melder_casual ("Duration of a data record: \"%f\"", durationOfDataRecord);
	fread (buffer, 1, 4, f); buffer [4] = '\0';
	long numberOfChannels = atol (buffer);
	Melder_casual ("Number of channels in data record: %ld", numberOfChannels);
	if (numberOfBytesInHeaderRecord != (numberOfChannels + 1) * 256)
		error5 (L"(Read from BDF file:) Number of bytes in header record (", Melder_integer (numberOfBytesInHeaderRecord),
			L") doesn't match number of channels (", Melder_integer (numberOfChannels), L").")
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 16, f); buffer [16] = '\0';   // labels of the channels
	}
	double samplingFrequency = NUMundefined;
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 80, f); buffer [80] = '\0';   // transducer type
	}
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 8, f); buffer [8] = '\0';   // physical dimension of channels
	}
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 8, f); buffer [8] = '\0';   // physical minimum in units of physical dimension
	}
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 8, f); buffer [8] = '\0';   // physical maximum in units of physical dimension
	}
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 8, f); buffer [8] = '\0';   // digital minimum
	}
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 8, f); buffer [8] = '\0';   // digital maximum
	}
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 80, f); buffer [80] = '\0';   // prefiltering
	}
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 8, f); buffer [8] = '\0';   // number of samples in each data record
		long numberOfSamplesPerDataRecord = atol (buffer);
		if (samplingFrequency == NUMundefined) {
			samplingFrequency = numberOfSamplesPerDataRecord / durationOfDataRecord;
		}
		if (numberOfSamplesPerDataRecord / durationOfDataRecord != samplingFrequency)
			error7 (L"(Read from BDF file:) Number of samples per data record in channel ", Melder_integer (ichannel),
				L" (", Melder_integer (numberOfSamplesPerDataRecord),
				L") doesn't match sampling frequency of channel 1 (", Melder_integer (numberOfChannels), L").")
	}
	for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		fread (buffer, 1, 32, f); buffer [32] = '\0';   // reserved
	}
	me = Sound_createSimple (numberOfChannels, numberOfDataRecords * durationOfDataRecord, samplingFrequency);
end:
	return me;
}

#ifdef macintosh
Sound Sound_readFromMacSoundFile (MelderFile file) {
	Sound me = NULL;
	FSRef fsRef;
	Melder_fileToMach (file, & fsRef);
	int path = FSOpenResFile (& fsRef, fsRdPerm);   /* Open resource fork; there are the sounds. */
	if (path == -1)
		return Melder_errorp ("(Sound_readFromMacSoundFile:) Error opening resource fork.");
	if (Count1Resources ('snd ') == 0) {   /* Are there really any sounds in this file? */
		CloseResFile (path);
		return Melder_errorp ("(Sound_readFromMacSoundFile:) No sound resources found.");
	}
	Handle han = Get1IndResource ('snd ', 1);   /* Take the first sound from this file. */
	if (! han) {
		CloseResFile (path);
		return Melder_errorp ("(Sound_readFromMacSoundFile:) Sound too large.");
	}
	DetachResource (han);   /* Release the sound's binding with the Resource Map. */
	CloseResFile (path);   /* Remove the Resource Map; the sound is ours now. */
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
	SoundHeader *header = & (**(SndResourceHandle) han).itsSndHeader;
	long numberOfSamples = header -> length;
	double samplingFrequency = (unsigned long) header -> sampleRate / 65536.0;
	me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency);
	if (me == NULL) return NULL;
	header = & (**(SndResourceHandle) han).itsSndHeader;   /* Do not move memory. */
	unsigned const char *from = (unsigned const char *) & header -> sampleArea - 1;
	double *to = my z [1];
	for (long i = 1; i <= numberOfSamples; i ++)
		to [i] = (from [i] - 128) * (1.0 / 128);
	DisposeHandle (han);
	return me;
}
#endif

Sound Sound_readFromMovieFile (MelderFile file) {
	Sound me = NULL;
#if (defined (macintosh) || defined (_WIN32)) && ! defined (DONT_INCLUDE_QUICKTIME)
	int debug = Melder_debug == 35;
	FSSpec fspec;
	short refNum = 0, resourceID = 0;
	OSErr err = noErr;
	Movie movie = 0;
	MacintoshBoolean wasChanged;
	Track track;
	Media media = NULL;
	long numberOfMediaSamples, numberOfSamplesPerMediaSample;
	double duration;
	SoundDescriptionHandle hSoundDescription = NULL;   /* Not NewHandle yet (on Windows, initialize QuickTime first). */
	Handle extension = NULL;
	AudioFormatAtomPtr decompressionAtom = NULL;
	short numberOfChannels, sampleSize, *buffer = NULL;
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
	if (err != noErr)
		error1 (L"Not a movie file.")
	err = NewMovieFromFile (& movie, refNum, & resourceID, NULL, newMovieActive, & wasChanged);
	if (err != noErr)
		error1 (L"Cannot find the movie in the movie file.")
	track = GetMovieIndTrackType (movie, 1, SoundMediaType, movieTrackMediaType);
	if (track == NULL)
		error1 (L"Invalid track in movie file.")
	media = GetTrackMedia (track);
	if (media == NULL)
		error1 (L"Invalid media in movie file.")
	hSoundDescription = (SoundDescriptionHandle) NewHandle (0);
	numberOfMediaSamples = GetMediaSampleCount (media);
	duration = (double) GetMediaDuration (media) / GetMediaTimeScale (media);
	GetMediaSampleDescription (media, 1, (SampleDescriptionHandle) hSoundDescription);
	if (GetMoviesError () != noErr)
		error1 (L"Cannot get sound description in movie file.")
	err = GetSoundDescriptionExtension (hSoundDescription, & extension, siDecompressionParams);
	if (err == noErr) {
		Size size = GetHandleSize (extension);
		HLock (extension);
		decompressionAtom = (AudioFormatAtomPtr) NewPtr (size);
		err = MemError ();
		if (err != noErr)
			error1 (L"No memory left when looking into movie file.")
		BlockMoveData (*extension, decompressionAtom, size);
		HUnlock (extension);
	} else {
		err = noErr;   /* No atom: OK. */
	}
	numberOfChannels = (*hSoundDescription) -> numChannels;
	sampleSize = (*hSoundDescription) -> sampleSize;
	samplingFrequency = (double) (*hSoundDescription) -> sampleRate / 65536.0;
	me = Sound_createSimple (numberOfChannels, duration, samplingFrequency); cherror
	numberOfSamplesPerMediaSample = my nx / numberOfMediaSamples;
	if (my nx % numberOfMediaSamples != 0)
		error5 (L"Media samples not equally long: ", Melder_integer (my nx), L" / ", Melder_integer (numberOfMediaSamples), L" gives a remainder.")
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
		error1 (L"Cannot open sound converter.")
	err = SoundConverterSetInfo (soundConverter, siClientAcceptsVBR, (Ptr) true);
	if (err != noErr && err != siUnknownInfoType)
		error1 (L"Don't like VBR.")
	err = SoundConverterSetInfo (soundConverter, siDecompressionParams, decompressionAtom);
	if (err != noErr && err != siUnknownInfoType)
		error1 (L"Don't like that decompression.")
	outputBufferSize = my ny * my nx * 2;
	if (debug) Melder_casual ("Before SoundConverterGetBufferSizes:\n"
		"   outputBufferSize = %ld", outputBufferSize);
	SoundConverterGetBufferSizes (soundConverter, outputBufferSize, & numberOfInputFrames,
		& inputBufferSize, & outputBufferSize);
	inputBufferHandle = NewHandle (inputBufferSize);
	if (inputBufferHandle == NULL)
		error1 (L"No room for input buffer.")
	if (debug) Melder_casual ("Between SoundConverterGetBufferSizes and GetMediaSample:\n"
		"   numberOfInputFrames = %ld\n   inputBufferSize = %ld\n   outputBufferSize = %ld",
		numberOfInputFrames, inputBufferSize, outputBufferSize);
	err = GetMediaSample (media,
		inputBufferHandle, inputBufferSize,
		& inputNumberOfBytes,
		0,   /* Start time. */
		& actualTime, & durationPerSample, NULL, NULL, numberOfMediaSamples*10, & actualNumberOfSamples, NULL);
	if (err != noErr)
		error1 (L"Cannot get media samples.")
	if (debug) Melder_casual ("Between GetMediaSample and SoundConverterBeginConversion:\n"
		"   inputBufferSize = %ld\n   inputNumberOfBytes = %ld\n   actualTime = %ld\n"
		"   durationPerSample = %ld\n   actualNumberOfMediaSamples = %ld",
		inputBufferSize, inputNumberOfBytes, actualTime, durationPerSample, actualNumberOfSamples);
	/*compressionID = (*hSoundDescription) -> compressionID;*/
	err = SoundConverterBeginConversion (soundConverter);
	if (err != noErr)
		error1 (L"Cannot begin sound conversion.")

	HLock (inputBufferHandle);
	buffer = NUMsvector (0, numberOfChannels * my nx);
	err = SoundConverterConvertBuffer (soundConverter,
		*inputBufferHandle, numberOfInputFrames,
		buffer, & numberOfOutputFrames, & numberOfOutputBytes);
	HUnlock (inputBufferHandle);
	if (err != noErr)
		error9 (L"Cannot convert sound. Error #", Melder_integer (err), L". ", Melder_integer (numberOfInputFrames),
			L" frames in, ", Melder_integer (numberOfOutputFrames), L" frames out, ", Melder_integer (numberOfOutputBytes), L" bytes out.")
/*	err = SoundConverterEndConversion (soundConverter, & my z [1] [1], & numberOfOutputFrames, & numberOfOutputBytes);
	if (err != noErr) {
		Melder_error1 (L"Cannot end sound conversion.");
		goto end;
	}*/
	if (numberOfOutputBytes != my ny * my nx * 2)
		error5 (L"Promised ", Melder_integer (my nx), L" samples, but got ", Melder_integer (numberOfOutputBytes / 2), L" after conversion.")
	for (long channel = 1; channel <= my ny; channel ++) {
		for (isamp = my nx; isamp > 0; isamp --) {
			my z [channel] [isamp] = buffer [(isamp - 1) * numberOfChannels + channel - 1] / 32768.0;
		}
	}
end:
	if (extension) DisposeHandle (extension);
	if (hSoundDescription) DisposeHandle ((Handle) hSoundDescription);
	if (movie) DisposeMovie (movie);
	if (inputBufferHandle) DisposeHandle (inputBufferHandle);
	if (decompressionAtom) DisposePtr ((Ptr) decompressionAtom);
	if (soundConverter) SoundConverterClose (soundConverter);
	if (refNum) CloseMovieFile (refNum);
	NUMsvector_free (buffer, 0);
	iferror forget (me);
#else
	Melder_error1 (L"This edition of Praat cannot handle movie files.");
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
		{ Melder_error1 (L"Not a Bell-Labs sound file."); goto error; }
	if ((endOfTag = strchr (tag + 4, '\n')) == NULL)
		{ Melder_error1 (L"Second line missing or too long."); goto error; }
	tagLength = endOfTag - tag + 1;   /* Probably 12. */
	if ((headerLength = atol (tag + 4)) <= 0)
		{ Melder_error1 (L"Wrong header-length info."); goto error; }

	/*
	 * Read data from header.
	 * Use defaults if necessary.
	 */
	if ((lines = Melder_calloc (char, headerLength + 1)) == NULL) goto error;
	if (fread (lines, 1, headerLength, f) < headerLength)
		{ Melder_error1 (L"Header too short."); goto error; }
	psamples = lines - 1;
	while ((psamples = strstr (psamples + 1, "samples ")) != NULL)   /* Take last occurrence. */
		numberOfSamples = atol (psamples + 8);
	if (numberOfSamples < 1) {
		/* Use file length. */
		fseek (f, 0, SEEK_END);   /* Position file pointer at end of file. */
		numberOfSamples = (ftell (f) - tagLength - headerLength) / 2;
	}
	if (numberOfSamples < 1)
		{ Melder_error1 (L"No samples found."); goto error; }
	pfrequency = lines - 1;
	while ((pfrequency = strstr (pfrequency + 1, "frequency ")) != NULL)   /* Take last occurrence. */
		samplingFrequency = atof (pfrequency + 10);
	if (samplingFrequency <= 0.0)
		samplingFrequency = 16000.0;

	/*
	 * Create sound.
	 */
	if (! (me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency))) goto error;

	/*
	 * Read samples.
	 */
	fseek (f, tagLength + headerLength, SEEK_SET);
	for (i = 1; i <= numberOfSamples; i ++) my z [1] [i] = bingeti2 (f) * (1.0 / 32768); /* 16 bits Big-Endian. */

	if (fclose (f) != 0)
		{ Melder_error1 (L"Error reading file."); goto error; }
	Melder_free (lines);
	return me;
error:
	if (f) fclose (f);
	Melder_free (lines);
	return Melder_errorp3 (L"(Sound_readFromBellLabsFile:) File ", MelderFile_messageName (fs), L" not read.");
}

static Sound readError (MelderFile file) {
	return Melder_errorp3 (L"(Sound_readFrom...File:) Cannot read file ", MelderFile_messageName (file), L".");
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
		return Melder_errorp3 (L"(Sound_readFromKayFile:) File ", MelderFile_messageName (fs), L" is not a KAY DS-16 file.");
	}

	/* HEDR chunk */

	if (fread (data, 1, 4, f) < 4) return readError (fs);
	if (! strnequ (data, "HEDR", 4)) {
		fclose (f);
		return Melder_errorp3 (L"(Sound_readFromKayFile:) File ", MelderFile_messageName (fs), L" does not contain HEDR chunk.");
	}
	chunkSize = bingeti4LE (f);
	if (chunkSize & 1) ++ chunkSize;
	if (fread (data, 1, chunkSize - 12, f) < chunkSize - 12) return readError (fs);
	samplingFrequency = bingeti4LE (f);
	numberOfSamples = bingeti4LE (f);
	if (samplingFrequency <= 0 || samplingFrequency > 1e7 ||
	    numberOfSamples <= 0 || numberOfSamples >= 1000000000) {
		fclose (f);
		return Melder_errorp3 (L"(Sound_readFromKayFile:) File ", MelderFile_messageName (fs), L" is not a correct Kay file.");
	}
	if (fread (data, 1, 4, f) < 4) return readError (fs);   /* Absolute extrema A/B. */

	/* SD chunk */

	if (fread (data, 1, 4, f) < 4) return readError (fs);
	while (! strnequ (data, "SDA_", 4) && ! strnequ (data, "SD_B", 4)) {
		if (feof (f)) {
			fclose (f);
			return Melder_errorp3 (L"(Sound_readFromKayFile:) File ", MelderFile_messageName (fs), L" does not contain readable SD chunk.");
		}
		chunkSize = bingeti4LE (f);
		if (chunkSize & 1) ++ chunkSize;
		if (fread (data, 1, chunkSize, f) < chunkSize) return readError (fs);
		if (fread (data, 1, 4, f) < 4) return readError (fs);
	}
	chunkSize = bingeti4LE (f);
	if (chunkSize != numberOfSamples * 2) {
		fclose (f);
		return Melder_errorp3 (L"(Sound_readFromKayFile:) File ", MelderFile_messageName (fs), L" does not contain readable SD chunk.");
	}

	me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency);
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
	me = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate); cherror
	Melder_readAudioToFloat (f, 1, Melder_ALAW, my z [1], NULL, numberOfSamples); cherror
end:
	Melder_fclose (file, f);
	iferror { Melder_error3 (L"(Sound_readFromRawAlawFile:) File ", MelderFile_messageName (file), L" not read."); forget (me); }
	return me;
}

int Sound_writeToAudioFile16 (Sound me, MelderFile file, int audioFileType) {
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), L"PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer) {
		MelderFile_writeAudioFileHeader16 (file, audioFileType, floor (1.0 / my dx + 0.5), my nx, my ny);
		MelderFile_writeFloatToAudio (file, Melder_defaultAudioFileEncoding16 (audioFileType),
			& my z [1] [1], my nx, my ny > 1 ? & my z [2] [1] : NULL, my ny > 1 ? my nx : 0, TRUE);
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
	for (i = 1; i <= 128; i ++) binputi4LE (header [i], f);
	for (i = 1; i <= my nx; i ++) binputi2LE (floor (my z [1] [i] * 2048 + 0.5), f);
	tail = 256 - my nx % 256;
	if (tail == 256) tail = 0;
	for (i = 1; i <= tail; i ++) binputi2LE (0, f);   /* Pad last block with zeroes. */
	if (fclose (f) == EOF) return Melder_error3 (L"Error writing file ", MelderFile_messageName (file), L".");
	return 1;
}

#ifdef macintosh
int Sound_writeToMacSoundFile (Sound me, MelderFile file) {
	long numberOfSamples = my nx;
	SndResourceHandle dataH = (SndResourceHandle) NewHandle (42 + numberOfSamples);
	SndResourcePtr data;
	long i;
	double *from;
	unsigned char *to;
	if (! dataH) return Melder_error1 (L"Sound_writeToMacSoundFile: not enough memory.");
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
		(Fixed) (unsigned long) floor (65536.0 / my dx);
	data -> itsSndHeader. loopStart = 0L;
	data -> itsSndHeader. loopEnd = 0L;
	data -> itsSndHeader. encode = stdSH;
	data -> itsSndHeader. baseFrequency = 60;
	from = my z [1];
	to = (unsigned char *) & data -> itsSndHeader. sampleArea - 1;
	for (i = 1; i <= numberOfSamples; i ++) to [i] = (int) (from [i] * 128 + 128);

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
			return Melder_error1 (L"Error %d when trying to create a Mac sound resource file.");
	}
	Melder_fileToMach (file, & fsRef);
	FSDeleteFork (& fsRef, resourceForkName. length, resourceForkName. unicode);
	OSErr err2 = FSCreateResourceFork (& fsRef, resourceForkName. length, resourceForkName. unicode, 0);
	if (err2 == nsvErr) {
		return Melder_error1 (L"File not found when trying to create a Mac sound resource file.");
	} else if (err2 != noErr)
		return Melder_error3 (L"Unexpected error ", Melder_integer (err2), L" trying to create a Mac sound file.");
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
		return Melder_error1 (L"Sound_writeToMacSoundFile: not enough disk space.");
	}
	SetResAttrs ((Handle) dataH, resPurgeable + resChanged);
		/* Make purgeable, like system sounds. Keep the changes. */
	CloseResFile (path);
	return 1;
}
#endif

int Sound_writeToKayFile (Sound me, MelderFile file) {
	FILE *previous = file -> filePointer;
	FILE *f = Melder_fopen (file, "wb");
	long i;
	time_t today = time (NULL);
	char date [100];
	int maximum = 0;
	if (! f) return 0;
	file -> filePointer = f;
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

	MelderFile_writeFloatToAudio (file, Melder_LINEAR_16_LITTLE_ENDIAN,
		& my z [1] [1], my nx, NULL, 0, TRUE); cherror
end:
	Melder_fclose (file, f);
	file -> filePointer = previous;
	iferror return 0;
	return 1;
}

int Sound_writeToRawSoundFile (Sound me, MelderFile file, int encoding) {
	FILE *previous = file -> filePointer;
	FILE *f = Melder_fopen (file, "wb");
	if (! f) return 0;
	file -> filePointer = f;
	MelderFile_setMacTypeAndCreator (file, 'BINA', 'PpgB');
	MelderFile_writeFloatToAudio (file, encoding,
		& my z [1] [1], my nx, my ny > 1 ? & my z [2] [1] : NULL, my ny > 1 ? my nx : 0, TRUE); cherror
end:
	Melder_fclose (file, f);
	file -> filePointer = previous;
	iferror return 0;
	return 1;
}

int Sound_writeToRaw8bitSignedFile (Sound me, MelderFile file) {
	return Sound_writeToRawSoundFile (me, file, Melder_LINEAR_8_SIGNED);
}

int Sound_writeToRaw8bitUnsignedFile (Sound me, MelderFile file) {
	return Sound_writeToRawSoundFile (me, file, Melder_LINEAR_8_UNSIGNED);
}

/* End of file Sound_files.c */
