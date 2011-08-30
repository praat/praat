/* manual_soundFiles.cpp
 *
 * Copyright (C) 1992-2008 Paul Boersma
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

#include "ManPagesM.h"

void manual_soundFiles_init (ManPages me);
void manual_soundFiles_init (ManPages me) {

MAN_BEGIN (L"Sound files", L"ppgb", 20070602)
INTRO (L"This tutorial describes the sound files that you can read "
	"and write with Praat. It assumes you are familiar with the @Intro.")
NORMAL (L"You can read this tutorial sequentially with the help of the \"< 1\" and \"1 >\" buttons.")
LIST_ITEM (L"1. @@Sound files 1. General structure|General structure@")
LIST_ITEM1 (L"1.1. @@Sound files 1.1. Sampling|Sampling@ (sampling frequency)")
LIST_ITEM1 (L"1.2. @@Sound files 1.2. Quantization|Quantization@ (linear, endian, \\mu-law, A-law)")
LIST_ITEM1 (L"1.3. @@Sound files 1.3. Channels|Channels@ (mono, stereo)")
LIST_ITEM1 (L"1.4. @@Sound files 1.4. The header|The header@")
LIST_ITEM1 (L"1.5. @@Sound files 1.5. Size|Size@")
LIST_ITEM1 (L"1.6. @@Sound files 1.6. Compression|Compression@")
LIST_ITEM (L"2. @@Sound files 2. File types|File types@")
LIST_ITEM1 (L"2.1. @@Sound files 2.1. WAV files|WAV files@")
LIST_ITEM1 (L"2.2. @@Sound files 2.2. AIFF files|AIFF files@")
LIST_ITEM1 (L"2.3. @@Sound files 2.3. AIFC files|AIFC files@")
LIST_ITEM1 (L"2.4. @@Sound files 2.4. NeXT/Sun (.au) files|NeXT/Sun (.au) files@")
LIST_ITEM1 (L"2.5. @@Sound files 2.5. NIST files|NIST files@")
LIST_ITEM1 (L"2.6. @@Sound files 2.6. FLAC files|FLAC files@")
LIST_ITEM1 (L"2.7. @@Sound files 2.7. MP3 files|MP3 files@")
LIST_ITEM (L"3. @@Sound files 3. Files that Praat can read|Files that Praat can read@")
LIST_ITEM (L"4. @@Sound files 4. Files that Praat can write|Files that Praat can write@")
MAN_END

MAN_BEGIN (L"Sound files 1. General structure", L"ppgb", 20000123)
INTRO (L"This is chapter 1 of the @@Sound files@ tutorial. "
	"It describes the general structure of the headers and data parts of sound files.")
LIST_ITEM (L"1.1. @@Sound files 1.1. Sampling|Sampling@ (sampling frequency)")
LIST_ITEM (L"1.2. @@Sound files 1.2. Quantization|Quantization@ (linear, \\mu-law, A-law)")
LIST_ITEM (L"1.3. @@Sound files 1.3. Channels|Channels@ (mono, stereo)")
LIST_ITEM (L"1.4. @@Sound files 1.4. The header|The header@")
LIST_ITEM (L"1.5. @@Sound files 1.5. Size|Size@")
LIST_ITEM (L"1.6. @@Sound files 1.6. Compression|Compression@")
MAN_END

MAN_BEGIN (L"Sound files 1.1. Sampling", L"ppgb", 20040330)
NORMAL (L"When a sound signal from a microphone or on a tape needs to be read into a computer, "
	"it is %digitized, which means that it is %sampled and %quantized.")
NORMAL (L"Sampling is the discretization of the time domain of the signal: "
	"each second of the signal is divided up into 11025, 22050, or 44100 "
	"slices (or any other suitable number), and a %%sample value% is associated with "
	"each slice. For instance, a continuous 377-Hz sine wave is expressed by")
FORMULA (L"%x(%t) = sin (2%\\pi 377 %t)")
NORMAL (L"If the %%sampling frequency% (or %%sample rate%) is 44100 Hz, this sine wave will be sampled at "
	"points spaced \\De%t = 1/44100 second apart, and the sample values will be")
FORMULA (L"%x__%i_ = sin (2%\\pi 377 (%t__0_ + %i \\De%t))")
NORMAL (L"where %t__0_ is the time after which sampling begins. "
	"\\De%t is called the %%sample period%.")
NORMAL (L"Quantization is handled in the next section (@@Sound files 1.2. Quantization|\\SS1.2@).")
MAN_END

MAN_BEGIN (L"Sound files 1.2. Quantization", L"ppgb", 20000126)
NORMAL (L"Apart from sampling (@@Sound files 1.1. Sampling|\\SS1.1@), "
	"digitization also involves quantization, which is the conversion of a sample value "
	"into a number that fits into 8 or 16 bits.")
MAN_END

MAN_BEGIN (L"Sound files 1.3. Channels", L"ppgb", 20000126)
NORMAL (L"For most file types, Praat supports sounds with one channel (mono) and two channels (stereo).")
MAN_END

MAN_BEGIN (L"Sound files 1.4. The header", L"ppgb", 20040223)
NORMAL (L"The bulk of most sound files is formed by the samples. "
	"The sample part is usually preceded by a header, which contains "
	"information about:")
LIST_ITEM (L"1. The type of file (WAV, AIFF, AIFC, NeXT/Sun, NIST, Kay, FLAC...).")
LIST_ITEM (L"2. The sampling frequency (@@Sound files 1.1. Sampling|\\SS1.1@).")
LIST_ITEM (L"3. The encoding (linear, 8 or 16 bit, byte order, \\mu-law, A-law, see @@Sound files 1.2. Quantization|\\SS1.2@).")
LIST_ITEM (L"4. The number of channels (mono, stereo, see @@Sound files 1.3. Channels|\\SS1.3@).")
LIST_ITEM (L"5. The duration (usually the number of samples).")
MAN_END

MAN_BEGIN (L"Sound files 1.5. Size", L"ppgb", 20000126)
NORMAL (L"The size of a sound file is equal to the size of the header "
	"plus the product of the number of samples, the quantization size, "
	"and the number of channels.")
NORMAL (L"For instance, a stereo sound with a duration of 3 seconds, sampled at 44100 Hz, "
	"would, when written into a 16-bit NeXT/Sun file, take up a disk space of")
FORMULA (L"28 + 2 * 3.0 * 44100 * 2 = 529228 bytes")
NORMAL (L"whereas the same sound, when averaged to mono, downsampled to 8000 Hz, "
	"and written into a \\mu-law NeXT/Sun file, take up only a disk space of")
FORMULA (L"28 + 1 * 3.0 * 8000 * 1 = 24028 bytes")
NORMAL (L"The first example is typical of CD quality, the second of telephone speech.")
MAN_END

MAN_BEGIN (L"Sound files 1.6. Compression", L"ppgb", 20040602)
NORMAL (L"Praat used to be able to read some compressed sound file formats (shortened NIST, Polyphone), "
	"but because of licensing problems (Praat went open source, Shorten did not), "
	"you now need to use other (freely available) programs to do the conversion before reading them into Praat. "
	"Praat can decode (but not create) MP3 files. "
	"Praat fully supports FLAC compressed files.")
MAN_END

MAN_BEGIN (L"Sound files 2. File types", L"ppgb", 20070602)
LIST_ITEM (L"2.1. @@Sound files 2.1. WAV files|WAV files@")
LIST_ITEM (L"2.2. @@Sound files 2.2. AIFF files|AIFF files@")
LIST_ITEM (L"2.3. @@Sound files 2.3. AIFC files|AIFC files@")
LIST_ITEM (L"2.4. @@Sound files 2.4. NeXT/Sun (.au) files|NeXT/Sun (.au) files@")
LIST_ITEM (L"2.5. @@Sound files 2.5. NIST files|NIST files@")
LIST_ITEM (L"2.6. @@Sound files 2.6. FLAC files|FLAC files@")
LIST_ITEM (L"2.7. @@Sound files 2.7. MP3 files|MP3 files@")
MAN_END

MAN_BEGIN (L"Sound files 2.1. WAV files", L"ppgb", 20040223)
NORMAL (L"The audio file type most commonly used on Windows computers, also very common on the Internet.")
MAN_END

MAN_BEGIN (L"Sound files 2.2. AIFF files", L"ppgb", 20040223)
NORMAL (L"AIFF stands for: Audio Interchange File Format.")
NORMAL (L"This standard format for sound files was defined by Apple. "
	"It is also the format of the sound files on the Iris Indigo, "
	"where each sample is quantized into 16 bits.")
MAN_END

MAN_BEGIN (L"Sound files 2.3. AIFC files", L"ppgb", 20040223)
NORMAL (L"AIFC is short for AIFF(C) or AIFF-C, i.e. the Audio Interchange File Format "
	"(@@Sound files 2.2. AIFF files|\\SS2.2@) with optional compression.")
NORMAL (L"Praat reads and write uncompressed AIFC files, but does not support compressed AIFC files.")
MAN_END

MAN_BEGIN (L"Sound files 2.4. NeXT/Sun (.au) files", L"ppgb", 20110131)
NORMAL (L"This is the format of the sound files on the Sun.")
ENTRY (L"Reading")
NORMAL (L"To read a #Sound from a Sun audio file on disk, use @@Read from file...@."
	"Praat then asks you for a file name. "
	"After you click OK, Praat determines the encoding of the file. "
	"If the encoding is 16-bit linear, the 16-bit sample values are divided by 32768 so that the amplitude "
	"of the resulting Sound is between -1.0 and +1.0. "
	"If the encoding is 8-bit %\\mu-law, the 16-bit sample value is determined by table look-up first.")
NORMAL (L"The resulting Sound will appear in the list of objects; "
	"its name will be equal to the file name, without extension.")
ENTRY (L"Saving")
NORMAL (L"Use @@Save as NeXT/Sun file...@. The samples of the Sound are multiplied by 32768 "
	"and quantized between -32768 and 32767; "
	"the result is written to the file in 16-bit mono Sun audio format.")
NORMAL (L"To avoid clipping, keep the absolute amplitude below 1.000. "
	"If the maximum sound pressure level is 91 dB (top = 32767), "
	"the quantization threshold is (top = 1/2) -5 dB.")
MAN_END

MAN_BEGIN (L"Sound files 2.5. NIST files", L"ppgb", 20040223)
NORMAL (L"An audio file type used by speech researchers. Used, for instance, in the TIMIT database. "
	"Praat reads several kinds of NIST files: big-endian, little-endian, \\mu-law, A-law, Polyphone. "
	"NIST files compressed with #shorten are no longer supported "
	"(see @@Sound files 1.6. Compression|\\SS1.6@)")
MAN_END

MAN_BEGIN (L"Sound files 2.6. FLAC files", L"Erez Volk", 20070514)
NORMAL (L"A lossless compressed audio format (see ##http://flac.sourceforge.net##). "
	"Praat reads FLAC files in all bitrate/frequency settings "
	"(see also @@Sound files 1.6. Compression|\\SS1.6@)")
MAN_END

MAN_BEGIN (L"Sound files 3. Files that Praat can read", L"ppgb", 20070602)
INTRO (L"Praat can read five types of standard sound files in several formats, "
	"and a number of proprietary types of sound files as well.")
ENTRY (L"Standard sound files")
NORMAL (L"The @@Open menu@ contains two commands for opening sound files:")
LIST_ITEM (L"\\bu With @@Read from file...@, you read the entire file into memory. "
	"A @Sound object will appear in the list. "
	"This is appropriate if your sound is not too long to fit into memory. "
	"The advantage of having a @Sound object is that you can perform analysis "
	"on it directly.")
LIST_ITEM (L"\\bu With @@Open long sound file...@, you open a sound file that is too long "
	"to read into memory completely. A @LongSound object will appear in the list. "
	"You will typically choose #View to view the contents of this sound and analyse it. "
	"Praat will only read so much of the file into memory as is needed to play or display "
	"parts of it.")
NORMAL (L"Both commands understand the following five standard audio file formats:")
LIST_ITEM (L"\\bu WAV:")
LIST_ITEM1 (L"\\bu linear 16-bit little-endian")
LIST_ITEM1 (L"\\bu 8-bit \\mu-law")
LIST_ITEM1 (L"\\bu 8-bit A-law")
LIST_ITEM1 (L"\\bu linear 8-bit unsigned")
LIST_ITEM (L"\\bu AIFF or AIFC:")
LIST_ITEM1 (L"\\bu linear 16-bit big-endian")
LIST_ITEM1 (L"\\bu linear 8-bit signed")
LIST_ITEM (L"\\bu NeXT/Sun (.au):")
LIST_ITEM1 (L"\\bu linear 16-bit big-endian")
LIST_ITEM1 (L"\\bu 8-bit \\mu-law")
LIST_ITEM1 (L"\\bu 8-bit A-law")
LIST_ITEM1 (L"\\bu linear 8-bit signed")
LIST_ITEM (L"\\bu NIST:")
LIST_ITEM1 (L"\\bu linear 16-bit little-endian")
LIST_ITEM1 (L"\\bu linear 16-bit big-endian")
LIST_ITEM1 (L"\\bu 8-bit \\mu-law")
LIST_ITEM1 (L"\\bu 8-bit A-law")
LIST_ITEM1 (L"\\bu linear 8-bit signed")
LIST_ITEM (L"\\bu FLAC:")
LIST_ITEM1 (L"\\bu 8-bit, 16-bit, 24-bit or 32-bit")
LIST_ITEM1 (L"\\bu all sampling frequencies")
LIST_ITEM (L"\\bu MP3:")
LIST_ITEM1 (L"\\bu all constant or variable bitrates")
MAN_END

MAN_BEGIN (L"Sound files 4. Files that Praat can write", L"ppgb", 20110129)
INTRO (L"Praat can write five types of standard sound files in an appropriate linear 16-bit formats, "
	"and a number of proprietary types of sound files as well:")
LIST_ITEM (L"\\bu @@Save as WAV file...@ (16-bit little-endian)")
LIST_ITEM (L"\\bu @@Save as AIFF file...@ (16-bit big-endian)")
LIST_ITEM (L"\\bu @@Save as AIFC file...@ (16-bit big-endian)")
LIST_ITEM (L"\\bu @@Save as NeXT/Sun file...@ (16-bit big-endian)")
LIST_ITEM (L"\\bu @@Save as NIST file...@ (16-bit little-endian)")
LIST_ITEM (L"\\bu @@Save as FLAC file...@ (16-bit)")
MAN_END

MAN_BEGIN (L"AIFF and AIFC files", L"ppgb", 20110131)
INTRO (L"Ways for storing a @Sound object on disk.")
ENTRY (L"File format")
ENTRY (L"Reading")
NORMAL (L"@@Read from file...@ recognizes AIFF and AIFC files with 8-bit and 16-bit encoding, "
	"but not compressed AIFC files. It recognizes any sampling frequency. The two channels of stereo files "
	"are averaged. ##Read two Sounds from AIFF file...# reads both channels separately and names "
	"them %left and %right.")
NORMAL (L"The sample values are divided by 2^^(%numberOfBitsPerSample \\-- 1)^, "
	"so that the amplitude of the resulting Sound is between \\--1.0 and +1.0; "
	"the maximum sound pressure level for a sine wave is therefore: "
	"20 \\.c ^^10^log (\\Vr2 / 2\\.c10^^\\--5^) = 91 dB.")
NORMAL (L"The resulting Sound will appear in the List of Objects; "
	"its name will be equal to the file name, without extension.")
ENTRY (L"Saving")
NORMAL (L"With @@Save as AIFF file...@.")
NORMAL (L"The samples of the Sound are multiplied by 32768 "
	"and quantized between \\--32768 and 32767.")
NORMAL (L"To avoid clipping, keep the absolute amplitude below 1.000. "
	"If the maximum sound pressure level is 91 dB (top = 32767), "
	"the quantization threshold is (top = 1/2) \\--5 dB.")
MAN_END

MAN_BEGIN (L"Save as WAV file...", L"ppgb", 20110129)
INTRO (L"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit little-endian WAV file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (L"Save as AIFF file...", L"ppgb", 20110129)
INTRO (L"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit big-endian AIFF file on disk. See the @@Sound files@ tutorial for more information.")
/*ENTRY (L"AIFF file format for writing")
NORMAL (L"Although")*/
MAN_END

MAN_BEGIN (L"Save as AIFC file...", L"ppgb", 20110129)
INTRO (L"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit big-endian AIFC file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (L"Save as NeXT/Sun file...", L"ppgb", 20110129)
INTRO (L"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit big-endian NeXT/Sun (.au) file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (L"Save as NIST file...", L"ppgb", 20110129)
INTRO (L"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit little-endian NIST audio file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (L"Save as FLAC file...", L"ppgb", 20110129)
INTRO (L"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit FLAC audio file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (L"Sound files 2.7. MP3 files", L"Erez Volk", 20070601)
NORMAL (L"A ubiquitous lossy audio compression format. "
	"Praat supports MP3 decoding through the MPEG Audio Decoder library "
	"##http://www.underbit.com/products/mad/##. "
	"(see also @@Sound files 1.6. Compression|\\SS1.6@)")
MAN_END

MAN_BEGIN (L"NIST files", L"Erez Volk", 20110131)
INTRO (L"A way for storing a @Sound object on disk.")
ENTRY (L"File format")
NORMAL (L"The compressed sound files of the Timit database, and the Groningen speech corpus.")
ENTRY (L"Reading")
NORMAL (L"With @@Read from file...@.")
ENTRY (L"Saving")
NORMAL (L"With ##Save as NIST audio file...#.")
MAN_END

MAN_BEGIN (L"FLAC files", L"Erez Volk", 20110131)
INTRO (L"A way for storing a @Sound object on disk.")
ENTRY (L"File format")
NORMAL (L"A lossless compressed audio file format.")
ENTRY (L"Reading")
NORMAL (L"With @@Read from file...@.")
ENTRY (L"Saving")
NORMAL (L"With ##Save as FLAC audio file...#.")
MAN_END

MAN_BEGIN (L"How to concatenate sound files", L"ppgb", 20110131)
INTRO (L"You can concatenate any combination of AIFF, AIFC, WAV, NeXT/Sun, NIST "
	"and FLAC audio files, and other files that you have read into memory.")
NORMAL (L"For instance, if you want to concatenate a 30-minute AIFF file, "
	"a 4-minute Kay sound file, and a 60-minute Next/Sun file, "
	"by saving them into a 94-minute WAV file, "
	"you do the following:")
LIST_ITEM (L"1. Open the AIFF file with @@Open long sound file...@ from the @@Open menu@. "
	"A @LongSound object will appear in the list.")
LIST_ITEM (L"2. Read the Kay sound file into memory with @@Read from file...@. "
	"A @Sound object will appear in the list.")
LIST_ITEM (L"3. Open the AIFF file with @@Open long sound file...@ from the @@Open menu@. "
	"A second @LongSound object will appear in the list.")
LIST_ITEM (L"4. Select the three objects and choose @@Save as WAV file...@ "
	"from the @@Save menu@.")
NORMAL (L"This only works if all the sounds have the same sampling frequency "
	"and the same number of channels.")
ENTRY (L"Available formats")
NORMAL (L"The format of the original sound files may be 16-bit linear "
	"(with big-endian or little-endian byte order), 8-bit linear (signed or unsigned), "
	"8-bit \\mu-law, or 8-bit A-law. The format of the resulting sound file is always "
	"16-bit linear, with an appropriate default byte order. The following commands "
	"are available in the @@Save menu@ of you select any combination of @LongSound "
	"and/or @Sound objects:")
LIST_ITEM (L"\\bu @@Save as WAV file...@ (little-endian)")
LIST_ITEM (L"\\bu @@Save as AIFF file...@ (big-endian)")
LIST_ITEM (L"\\bu @@Save as AIFC file...@ (big-endian)")
LIST_ITEM (L"\\bu @@Save as NeXT/Sun file...@ (big-endian)")
LIST_ITEM (L"\\bu @@Save as NIST file...@ (little-endian)")
LIST_ITEM (L"\\bu @@Save as FLAC file...@")
MAN_END

MAN_BEGIN (L"LongSound", L"ppgb", 20110131)
INTRO (L"One of the @@types of objects@ in Praat. See the @@Sound files@ tutorial.")
NORMAL (L"A LongSound object gives you the ability to view and label "
	"a sound file that resides on disk. You will want to use it for sounds "
	"that are too long to read into memory as a @Sound object (typically, a few minutes).")
ENTRY (L"How to create a LongSound object")
NORMAL (L"You create a LongSound object with @@Open long sound file...@ from the @@Open menu@.")
ENTRY (L"What you can do with a LongSound object")
NORMAL (L"You can save a LongSound object to a new sound file, "
	"perhaps in a different format (AIFF, AIFC, WAV, NeXT/Sun, NIST, FLAC) "
	"with the commands in the Save menu. You can also concatenate several "
	"LongSound objects in this way. See @@How to concatenate sound files@.")
ENTRY (L"How to view and edit a LongSound object")
NORMAL (L"You can view a LongSound object in a @LongSoundEditor by choosing @@LongSound: View@. "
	"This also allows you to extract parts of the LongSound as @Sound objects, "
	"or save these parts as a sound file. "
	"There are currently no ways to actually change the data in the file.")
ENTRY (L"How to annotate a LongSound object")
NORMAL (L"You can label and segment a LongSound object after the following steps:")
LIST_ITEM (L"1. Select the LongSound object.")
LIST_ITEM (L"2. Choose @@LongSound: To TextGrid...@ and specify your tiers.")
LIST_ITEM (L"3. Select the resulting @TextGrid object together with the LongSound object, and click ##View & Edit#.")
NORMAL (L"A @TextGridEditor will appear on the screen, with a copy of the LongSound object in it.")
ENTRY (L"Limitations")
NORMAL (L"The length of the sound file is limited to 2 gigabytes, which is 3 hours of CD-quality stereo, "
	"or 12 hours 16-bit mono sampled at 22050 Hz.")
MAN_END

MAN_BEGIN (L"LongSound: To TextGrid...", L"ppgb", 19980730)
INTRO (L"A command to create a @TextGrid without any labels, copying the time domain from the selected @LongSound.")
NORMAL (L"See @@Sound: To TextGrid...@ for the settings.")
MAN_END

MAN_BEGIN (L"LongSound: View", L"ppgb", 19980730)
INTRO (L"A command to view the selected @LongSound object in a @LongSoundEditor.")
MAN_END

MAN_BEGIN (L"LongSoundEditor", L"ppgb", 20041124)
INTRO (L"One of the @Editors in Praat, for viewing a @LongSound object.")
NORMAL (L"This viewer allows you:")
LIST_ITEM (L"\\bu to view and hear parts of the sound as it is on disk;")
LIST_ITEM (L"\\bu to copy a selected part as a @Sound object to the list of objects, so that you "
	"can perform analyses on it or save it to a smaller sound file;")
LIST_ITEM (L"\\bu to copy a selected part as a @Sound object to the @Sound clipboard, "
	"so that you can paste it into another Sound object that you are viewing in a @SoundEditor.")
NORMAL (L"To label and segment the LongSound object, use the @TextGridEditor instead (see @LongSound).")
NORMAL (L"The display and playback of the samples is restricted to 60 seconds at a time, for reasons of speed "
	"(although you can change this number with ##LongSound prefs# from the main #Preferences menu; "
	"the sound file itself can contain several hours of sound.")
MAN_END

MAN_BEGIN (L"Macintosh sound files", L"ppgb", 20110131)
INTRO (L"A way for storing a @Sound object on disk.")
ENTRY (L"File format")
NORMAL (L"The double-clickable sound file of the Macintosh (8 bits per sample). "
	"Cannot be ported to other machines, because the sound is in the @@resource fork@.")
ENTRY (L"Reading")
NORMAL (L"With @@Read from file...@.")
NORMAL (L"To read a Sound from a Macintosh sound file on disk, use @@Read from file...@ (Macintosh only).")
NORMAL (L"The 8-bit sample values are divided by 128 so that the amplitude "
	"of the resulting Sound is between \\--1.0 and +1.0.")
NORMAL (L"The resulting Sound will appear in the list of objects; "
	"its name will be equal to the file name, without extension.")
ENTRY (L"Saving")
NORMAL (L"With ##Save as Mac sound file...#. Praat asks you for a file name. "
	"After you click OK, 0.5 is added to the samples of the Sound, "
	"they are multiplied by 128 and quantized between 0 and 255; "
	"the result is written to the file in 8-bit linear Macintosh sound-file format.")
NORMAL (L"To avoid clipping, keep the absolute amplitude below 1.000. "
	"If the maximum sound pressure level is 91 dB (top = 127), "
	"the quantization threshold for a sine wave is (top = 1/2) 43 dB.")
MAN_END

MAN_BEGIN (L"Open long sound file...", L"ppgb", 19980730)
INTRO (L"A command in the @@Open menu@ that creates a @LongSound object.")
NORMAL (L"The file will be opened for reading only. The file stays open until you remove the LongSound object.")
MAN_END

MAN_BEGIN (L"Sesam/LVS files", L"ppgb", 20110131)
INTRO (L"A way for storing a @Sound object on disk.")
ENTRY (L"File format")
NORMAL (L"The sound files used by the SESAM and LVS programs. Each sample is normally quantized into 12 bits.")
ENTRY (L"Reading")
NORMAL (L"To read a #Sound from a Sesam file on disk, use @@Read from file...@. "
	"The file name is expected to end in \".sdf\" or \".SDF\".")
NORMAL (L"The 12-bit sample values are divided by 2048 so that the amplitude "
	"of the resulting Sound is between -1.0 and +1.0.")
NORMAL (L"The resulting #Sound will appear in the List of Objects; "
	"its name will be equal to the file name, without extension.")
NORMAL (L"If the sound was encoded in 16 bits per sample, you should divide by 16 after reading "
	"(with $$Formula... self/16$)")
ENTRY (L"Saving")
NORMAL (L"With ##Save as Sesam file...#. Praat then asks you for a file name. "
	"After you click OK, the samples of the Sound are multiplied by 2048 "
	"and quantized between -2048 and 2047; "
	"the result is written to the file in 12-bit LVS and Sesam format.")
NORMAL (L"To avoid clipping, keep the absolute amplitude below 1.000. "
	"If the maximum sound pressure level is 91 dB (top = 2047), "
	"the quantization threshold is (top = 1/2) 19 dB.")
NORMAL (L"If you prefer 16-bit encoding, you should multiply by 16 before saving "
	"(with $$Formula... self*16$)")
MAN_END

}

/* End of file manual_soundFiles.cpp */
