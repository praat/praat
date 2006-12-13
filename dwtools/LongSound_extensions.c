/* LongSound_extensions.c
 *
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 20020627 GPL header
 djmw 20030913 changed 'f' to 'file' as argument in Melder_checkSoundFile
 djmw 20060206 Set errno = 0:  "An application that needs to examine the value
 	of errno to determine the error should set it to 0 before a function call,
 	then inspect it before a subsequent function call."
*/

#include "LongSound_extensions.h"
#include "NUM2.h"
#if defined (_WIN32)
	#include <windows.h>
#elif defined(linux)
	#include <unistd.h>
	#include <sys/types.h>
	#include <string.h>    
#endif                            
#include <errno.h>     
                           
/*
  Precondition: size (my buffer) >= nbuf
*/ 
static int _LongSound_to_multichannel_buffer (LongSound me, short *buffer, long nbuf,
	int nchannels, int ichannel, long ibuf)
{
	long numberOfReads = (my nx - 1) / nbuf + 1;
	long i, n_to_read = 0, imin;
	
	if (ibuf <= numberOfReads)
	{
		n_to_read = ibuf == numberOfReads ? (my nx - 1) % nbuf + 1 : nbuf;
		imin = (ibuf - 1) * nbuf + 1;
		if (fseek (my f, my startOfData + (imin - 1) * my numberOfChannels * 
			my numberOfBytesPerSamplePoint, SEEK_SET))
		{
			return Melder_error ("Cannot seek in file %s.", 
				MelderFile_messageName (& my file));
		}
		Melder_readAudioToShort (my f, my numberOfChannels, my encoding, my buffer, 
			n_to_read);
		
		for (i = 1; i <= n_to_read; i++)
		{
			buffer[nchannels * (i - 1) + ichannel] = my buffer[i];
		}
	}
	if (ibuf >= numberOfReads)
	{
		for (i = n_to_read + 1; i <= nbuf; i++)
		{
			buffer[nchannels * (i - 1) + ichannel] = 0;
		}
	}
	return ! Melder_hasError ();
} 

int LongSounds_writeToStereoAudioFile16 (LongSound me, LongSound thee,
	int audioFileType, MelderFile file)
{
	char *proc = "LongSounds_writeToStereoAudioFile16";
	short *buffer; 
	long i, nchannels = 2; 
	long nbuf = my nmax < thy nmax ? my nmax : thy nmax; 
	long nx = my nx > thy nx ? my nx : thy nx; 
	long numberOfReads = (nx - 1) / nbuf + 1;
	
	if (thy numberOfChannels != my numberOfChannels || my numberOfChannels != 1)
	{
		return Melder_error ("%s: LongSounds must be mono.", proc);
	}
	if (my sampleRate != thy sampleRate)
	{
		return Melder_error ("%s: Sample rates must be equal.", proc);
	}
	
	/* 
		Allocate a stereo buffer of size (2 * the smallest)!
		WE SUPPOSE THAT SMALL IS LARGE ENOUGH.
		Read the same number of samples from both files, despite
		eventual differences in internal buffer size. 
	*/
	
	buffer = NUMsvector (1, nchannels * nbuf);
	if (buffer == NULL) return 0;
		
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), "PpgB", 
		Melder_winAudioFileExtension (audioFileType));
	if (! file -> filePointer) goto end;
	Melder_writeAudioFileHeader16 (file -> filePointer, audioFileType, 
		my sampleRate, nx, nchannels);	

	for (i = 1; i <= numberOfReads; i++)
	{
		long n_to_write = i == numberOfReads ? (nx - 1) % nbuf + 1: nbuf;
		if (! _LongSound_to_multichannel_buffer (me, buffer, nbuf, nchannels, 1, i) ||
			! _LongSound_to_multichannel_buffer (thee, buffer, nbuf, nchannels, 2, i))
			goto end;
		Melder_writeShortToAudio (file -> filePointer, nchannels, 
			Melder_defaultAudioFileEncoding16 (audioFileType), buffer, n_to_write);			
	}
end:
	MelderFile_close (file);
	NUMsvector_free (buffer, 1);	
	return ! Melder_hasError();
}


/*
	BSD systems provide ftruncate, several others supply chsize, and a few 
	may provide a (possibly undocumented) fcntl option F_FREESP. Under MS-DOS,
	you can sometimes use write(fd, "", 0). However, there is no portable 
	solution, nor a way to delete blocks at the beginning.
*/
static int MelderFile_truncate (MelderFile me, long size)
{
	char *proc = "MelderFile_truncate";
		
#if defined (_WIN32)

	HANDLE hFile;
	DWORD fdwAccess = GENERIC_READ | GENERIC_WRITE, fPos;
	DWORD fdwShareMode = 0; /* File cannot be shared */
	LPSECURITY_ATTRIBUTES lpsa = NULL;
	DWORD fdwCreate = OPEN_EXISTING;
    LARGE_INTEGER fileSize;

	MelderFile_close (me);
  
  	hFile = CreateFile (my path, fdwAccess, fdwShareMode, lpsa, fdwCreate,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return Melder_error ("%s: "
        "Can't open file \"%s\".", proc, MelderFile_messageName (me));
        
    /*                       
        Set current file pointer to position 'size'
    */
    
    fileSize.LowPart = size;
    fileSize.HighPart = 0; /* Limit the file size to 2^32 - 2 bytes */
	fPos = SetFilePointer (hFile, fileSize.LowPart, &fileSize.HighPart, FILE_BEGIN);
	if (fPos == 0xFFFFFFFF) return Melder_error ("%s: Can't set the position at "
		"size %d for file \"%s\".", proc, size, MelderFile_messageName (me));
  
	/*
		Limit the file size as the current position of the file pointer.
	*/
 
	SetEndOfFile (hFile);
	CloseHandle (hFile);
	
#elif defined(linux) || defined(macintosh)
	
	MelderFile_close (me);

    if (truncate (my path, size) == -1)
    {   
		return Melder_error ("%s: Truncating file \"%s\" failed: %s.", proc,
			MelderFile_messageName (me), strerror (errno));
	}
#else

	return 0;
	
#endif

	return 1;
}


static void writePartToOpenFile16 (LongSound me, int audioFileType, long imin, long n, MelderFile file) {
	long ibuffer, numberOfBuffers, numberOfSamplesInLastBuffer;
	if (fseek (my f, my startOfData + (imin - 1) * my numberOfChannels * my numberOfBytesPerSamplePoint, SEEK_SET))
		{ Melder_error ("Cannot seek in file %s.", MelderFile_messageName (& my file)); goto end; }
	numberOfBuffers = (n - 1) / my nmax + 1;
	numberOfSamplesInLastBuffer = (n - 1) % my nmax + 1;
	if (file -> filePointer) for (ibuffer = 1; ibuffer <= numberOfBuffers; ibuffer ++) {
		long numberOfSamplesToCopy = ibuffer < numberOfBuffers ? my nmax : numberOfSamplesInLastBuffer;
		Melder_readAudioToShort (my f, my numberOfChannels, my encoding, my buffer, numberOfSamplesToCopy);
		Melder_writeShortToAudio (file -> filePointer, my numberOfChannels, Melder_defaultAudioFileEncoding16 (audioFileType), my buffer, numberOfSamplesToCopy);
	}
end:
	/*
	 * We "have" no samples any longer.
	 */
	my imin = 1;
	my imax = 0;
}

int LongSounds_appendToExistingSoundFile (Ordered me, MelderFile file)
{
	char *proc = "LongSounds_appendToFile";
	FILE *f;
	int numberOfChannels, encoding, audioFileType, status = 0;
	long i, startOfData, numberOfSamples, sampleRate, pre_append_endpos = 0;
	double sampleRate_d;

	if (my size < 1) return Melder_error ("%s: No Sound or LongSound objects "
		"to append.", proc);

	/*
		We have to open with "r+" mode because this will position the stream
		at the beginning of the file. The "a" mode does not allow us to
		seek before the end-of-file.

		For Linux: If the file is already opened (e.g. by a LongSound) object we
			should deny access!
		Under Windows deny access is default?!
	*/
	
	f = Melder_fopen (file, "r+b");
	file -> filePointer = f;	
	if (f == NULL) return 0;
	
	audioFileType = MelderFile_checkSoundFile (file, &numberOfChannels, 
		&encoding, &sampleRate_d, &startOfData, &numberOfSamples);
		
	if (audioFileType == 0)
	{
		(void) Melder_error ("%s: Not a sound file", proc);
		goto end;
	}
	
	/*
		Check whether all the sample rates and channels match.
	*/
	
	sampleRate = sampleRate_d;
	for (i = 1; i <= my size; i++)
	{
		int sampleRatesMatch, numbersOfChannelsMatch;
		Data data = my item [i];
		if (data -> methods == (Data_Table) classSound)
		{
			Sound sound = (Sound) data;
			sampleRatesMatch = floor (1.0 / sound -> dx + 0.5) == sampleRate;
			numbersOfChannelsMatch = 1 == numberOfChannels;
			numberOfSamples += sound -> nx;
		}
		else
		{
			LongSound longSound = (LongSound) data;
			sampleRatesMatch = longSound -> sampleRate == sampleRate;
			numbersOfChannelsMatch = longSound -> numberOfChannels == numberOfChannels;
			numberOfSamples += longSound -> nx;
		}
		if (! sampleRatesMatch)
		{
			(void) Melder_error ("%s: Sample rates do not match.", proc);
			goto end;
		}
		if (! numbersOfChannelsMatch)
		{
			(void) Melder_error ("%s: Cannot mix stereo and mono.", proc);
			goto end;
		}
	}

	/*
		Search the end of the file, count the number of bytes and append.
	*/
	
	MelderFile_seek (file, 0, SEEK_END);
	if (Melder_hasError ()) goto end;
	
	pre_append_endpos = MelderFile_tell (file);
	if (Melder_hasError ()) goto end;
	
	errno = 0;
	for (i = 1; i <= my size; i++)
	{
		Data data = my item [i];
		if (data -> methods == (Data_Table) classSound)
		{
			Sound sound = (Sound) data;
			if (file -> filePointer)
			{
				Melder_writeFloatToAudio (file -> filePointer, Melder_defaultAudioFileEncoding16 (audioFileType),
					& sound -> z [1] [1], sound -> nx, NULL, 0, TRUE);
			}
		}
		else
		{
			LongSound longSound = (LongSound) data;
			writePartToOpenFile16 (longSound, audioFileType, 1, longSound -> nx, file);
		}
		if (errno != 0) goto end;
	}
	
	/*
		Update header
	*/

	MelderFile_rewind (file);
	(void) Melder_writeAudioFileHeader16 (file -> filePointer, audioFileType,
		 sampleRate, numberOfSamples, numberOfChannels);
	if (Melder_hasError ())
	{
		(void) Melder_error ("%s: Cannot write header.", proc);
		goto end;
	}
	
	status = 1;
	
end:

	MelderFile_close (file);
	
	if (errno != 0 && pre_append_endpos > 0)
	{
		/* Restore file at original size */
		int error = errno;
		status = MelderFile_truncate (file, pre_append_endpos);
		return Melder_error ("%s: File \"%s\" restored to original size. Hint: %s.", proc, 
			MelderFile_messageName (file), strerror (error));
	}
	
	return status;
}

/* End of file LongSound_extensions.c */	 
