/* manual_soundFiles.cpp
 *
 * Copyright (C) 1992-2005,2007,2008,2010,2011,2014-2017 Paul Boersma
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

#include "ManPagesM.h"

void manual_soundFiles_init (ManPages me);
void manual_soundFiles_init (ManPages me) {

MAN_BEGIN (U"Sound files", U"ppgb", 20070602)
INTRO (U"This tutorial describes the sound files that you can read "
	"and write with Praat. It assumes you are familiar with the @Intro.")
NORMAL (U"You can read this tutorial sequentially with the help of the \"< 1\" and \"1 >\" buttons.")
LIST_ITEM (U"1. @@Sound files 1. General structure|General structure@")
LIST_ITEM1 (U"1.1. @@Sound files 1.1. Sampling|Sampling@ (sampling frequency)")
LIST_ITEM1 (U"1.2. @@Sound files 1.2. Quantization|Quantization@ (linear, endian, μ-law, A-law)")
LIST_ITEM1 (U"1.3. @@Sound files 1.3. Channels|Channels@ (mono, stereo)")
LIST_ITEM1 (U"1.4. @@Sound files 1.4. The header|The header@")
LIST_ITEM1 (U"1.5. @@Sound files 1.5. Size|Size@")
LIST_ITEM1 (U"1.6. @@Sound files 1.6. Compression|Compression@")
LIST_ITEM (U"2. @@Sound files 2. File types|File types@")
LIST_ITEM1 (U"2.1. @@Sound files 2.1. WAV files|WAV files@")
LIST_ITEM1 (U"2.2. @@Sound files 2.2. AIFF files|AIFF files@")
LIST_ITEM1 (U"2.3. @@Sound files 2.3. AIFC files|AIFC files@")
LIST_ITEM1 (U"2.4. @@Sound files 2.4. NeXT/Sun (.au) files|NeXT/Sun (.au) files@")
LIST_ITEM1 (U"2.5. @@Sound files 2.5. NIST files|NIST files@")
LIST_ITEM1 (U"2.6. @@Sound files 2.6. FLAC files|FLAC files@")
LIST_ITEM1 (U"2.7. @@Sound files 2.7. MP3 files|MP3 files@")
LIST_ITEM (U"3. @@Sound files 3. Files that Praat can read|Files that Praat can read@")
LIST_ITEM (U"4. @@Sound files 4. Files that Praat can write|Files that Praat can write@")
MAN_END

MAN_BEGIN (U"Sound files 1. General structure", U"ppgb", 20000123)
INTRO (U"This is chapter 1 of the @@Sound files@ tutorial. "
	"It describes the general structure of the headers and data parts of sound files.")
LIST_ITEM (U"1.1. @@Sound files 1.1. Sampling|Sampling@ (sampling frequency)")
LIST_ITEM (U"1.2. @@Sound files 1.2. Quantization|Quantization@ (linear, μ-law, A-law)")
LIST_ITEM (U"1.3. @@Sound files 1.3. Channels|Channels@ (mono, stereo)")
LIST_ITEM (U"1.4. @@Sound files 1.4. The header|The header@")
LIST_ITEM (U"1.5. @@Sound files 1.5. Size|Size@")
LIST_ITEM (U"1.6. @@Sound files 1.6. Compression|Compression@")
MAN_END

MAN_BEGIN (U"Sound files 1.1. Sampling", U"ppgb", 20040330)
NORMAL (U"When a sound signal from a microphone or on a tape needs to be read into a computer, "
	"it is %digitized, which means that it is %sampled and %quantized.")
NORMAL (U"Sampling is the discretization of the time domain of the signal: "
	"each second of the signal is divided up into 11025, 22050, or 44100 "
	"slices (or any other suitable number), and a %%sample value% is associated with "
	"each slice. For instance, a continuous 377-Hz sine wave is expressed by")
FORMULA (U"%x(%t) = sin (2%π 377 %t)")
NORMAL (U"If the %%sampling frequency% (or %%sample rate%) is 44100 Hz, this sine wave will be sampled at "
	"points spaced Δ%t = 1/44100 second apart, and the sample values will be")
FORMULA (U"%x__%i_ = sin (2%π 377 (%t__0_ + %i Δ%t))")
NORMAL (U"where %t__0_ is the time after which sampling begins. "
	"Δ%t is called the %%sample period%.")
NORMAL (U"Quantization is handled in the next section (@@Sound files 1.2. Quantization|§1.2@).")
MAN_END

MAN_BEGIN (U"Sound files 1.2. Quantization", U"ppgb", 20190517)
NORMAL (U"Apart from sampling (@@Sound files 1.1. Sampling|§1.1@), "
	"digitization also involves quantization, which is the conversion of a sample value "
	"into a number that fits into 8, 16, 24 or 32 bits.")
ENTRY (U"Saving")
NORMAL (U"When exporting a @Sound object as a 16-bit audio file on disk, "
	"Praat multiplies the samples of the Sound by 32768, and then rounds them to integer numbers, "
	"so that air pressure values between -1.0 and +1.0 Pa will end up being coded as integers between -32768 and +32767. "
	"Sample values below -1.0 will be coded as -32768, and sample values above +1.0 will be coded as +32767. "
	"To avoid such \"clipping\", keep the absolute amplitude in your @Sound object below 1.000, "
	"perhaps by using @@Sound: Scale peak...@ before you save.")
NORMAL (U"When exporting a @Sound object as a 24-bit audio file on disk, "
	"Praat multiplies the samples of the Sound by 8388608 instead.")
ENTRY (U"Opening")
NORMAL (U"For a 16-bit sound file, the sample values on disk are integers between -32768 and +32767. "
	"When turning the file into a @Sound object, Praat converts these integers into real values between -1.0 and +1.0, "
	"by dividing the sample values by 32768.0 (i.e. 2^^(16 – 1)^). "
	"Praat subsequently regards these real values as air pressures in pascal (see @@sound pressure calibration@). "
	"For a 24-bit AIFF file, the sample values on disk are integers between -8388608 and +8388607, "
	"and Praat divides these by 8388608.0 to get values between -1.0 and +1.0 Pa in the @Sound object.")
MAN_END

MAN_BEGIN (U"Sound files 1.3. Channels", U"ppgb", 20000126)
NORMAL (U"For most file types, Praat supports sounds with one channel (mono) and two channels (stereo).")
MAN_END

MAN_BEGIN (U"Sound files 1.4. The header", U"ppgb", 20040223)
NORMAL (U"The bulk of most sound files is formed by the samples. "
	"The sample part is usually preceded by a header, which contains "
	"information about:")
LIST_ITEM (U"1. The type of file (WAV, AIFF, AIFC, NeXT/Sun, NIST, Kay, FLAC...).")
LIST_ITEM (U"2. The sampling frequency (@@Sound files 1.1. Sampling|§1.1@).")
LIST_ITEM (U"3. The encoding (linear, 8 or 16 bit, byte order, μ-law, A-law, see @@Sound files 1.2. Quantization|§1.2@).")
LIST_ITEM (U"4. The number of channels (mono, stereo, see @@Sound files 1.3. Channels|§1.3@).")
LIST_ITEM (U"5. The duration (usually the number of samples).")
MAN_END

MAN_BEGIN (U"Sound files 1.5. Size", U"ppgb", 20000126)
NORMAL (U"The size of a sound file is equal to the size of the header "
	"plus the product of the number of samples, the quantization size, "
	"and the number of channels.")
NORMAL (U"For instance, a stereo sound with a duration of 3 seconds, sampled at 44100 Hz, "
	"would, when written into a 16-bit NeXT/Sun file, take up a disk space of")
FORMULA (U"28 + 2 * 3.0 * 44100 * 2 = 529228 bytes")
NORMAL (U"whereas the same sound, when averaged to mono, downsampled to 8000 Hz, "
	"and written into a μ-law NeXT/Sun file, take up only a disk space of")
FORMULA (U"28 + 1 * 3.0 * 8000 * 1 = 24028 bytes")
NORMAL (U"The first example is typical of CD quality, the second of telephone speech.")
MAN_END

MAN_BEGIN (U"Sound files 1.6. Compression", U"ppgb", 20201229)
NORMAL (U"Praat used to be able to read some compressed sound file formats (shortened NIST, Polyphone), "
	"but because of licensing problems (Praat went open source, Shorten did not), "
	"you now need to use other (freely available) programs to do the conversion before reading them into Praat. "
	"Praat can decode (but not create) MP3 files and Ogg Vorbis files. "
	"Praat fully supports FLAC compressed files.")
MAN_END

MAN_BEGIN (U"Sound files 2. File types", U"ppgb", 20201229)
LIST_ITEM (U"2.1. @@Sound files 2.1. WAV files|WAV files@")
LIST_ITEM (U"2.2. @@Sound files 2.2. AIFF files|AIFF files@")
LIST_ITEM (U"2.3. @@Sound files 2.3. AIFC files|AIFC files@")
LIST_ITEM (U"2.4. @@Sound files 2.4. NeXT/Sun (.au) files|NeXT/Sun (.au) files@")
LIST_ITEM (U"2.5. @@Sound files 2.5. NIST files|NIST files@")
LIST_ITEM (U"2.6. @@Sound files 2.6. FLAC files|FLAC files@")
LIST_ITEM (U"2.7. @@Sound files 2.7. MP3 files|MP3 files@")
LIST_ITEM (U"2.8. @@Sound files 2.8. Ogg Vorbis files|Ogg Vorbis files@")
LIST_ITEM (U"2.9. @@Sound files 2.9. Ogg Opus files|Ogg Opus files@")
MAN_END

MAN_BEGIN (U"Sound files 2.1. WAV files", U"ppgb", 20040223)
NORMAL (U"The audio file type most commonly used on Windows computers, also very common on the Internet.")
MAN_END

MAN_BEGIN (U"Sound files 2.2. AIFF files", U"ppgb", 20190517)
NORMAL (U"AIFF stands for: Audio Interchange File Format.")
NORMAL (U"This standard format for sound files was defined by Apple. "
	"It is also the format of the sound files on the Iris Indigo, "
	"where each sample is quantized into 16 bits.")
ENTRY (U"Reading")
NORMAL (U"To open an AIFF file from disk, choose @@Read from file...@ from the Open menu. "
	"Praat recognizes AIFF and AIFC files with plain 8-bit, 16-bit, 24-bit or 32-bit encoding, "
	"but not compressed AIFC files. Praat recognizes any sampling frequency, and any number of channels.")
NORMAL (U"The resulting Sound will appear in the List of Objects. "
	"Its name will be identical to the file name, without extension.")
ENTRY (U"Saving")
NORMAL (U"To export a @Sound object to a 16-bit AIFF file, select the @Sound and choose @@Save as AIFF file...@ from the Save menu.")
MAN_END

MAN_BEGIN (U"Sound files 2.3. AIFC files", U"ppgb", 20190517)
NORMAL (U"AIFC is short for AIFF(C) or AIFF-C, i.e. the Audio Interchange File Format "
	"(@@Sound files 2.2. AIFF files|§2.2@) with optional compression.")
NORMAL (U"Praat reads uncompressed AIFC files with plain 8-bit, 16-bit, 24-bit or 32-bit encoding, "
	"but does not support compressed AIFC files.")
MAN_END

MAN_BEGIN (U"Sound files 2.4. NeXT/Sun (.au) files", U"ppgb", 20110131)
NORMAL (U"This is the format of the sound files on the Sun.")
ENTRY (U"Reading")
NORMAL (U"To read a #Sound from a Sun audio file on disk, use @@Read from file...@."
	"Praat then asks you for a file name. "
	"After you click OK, Praat determines the encoding of the file. "
	"If the encoding is 16-bit linear, the 16-bit sample values are divided by 32768 so that the amplitude "
	"of the resulting Sound is between -1.0 and +1.0. "
	"If the encoding is 8-bit %μ-law, the 16-bit sample value is determined by table look-up first.")
NORMAL (U"The resulting Sound will appear in the list of objects; "
	"its name will be equal to the file name, without extension.")
ENTRY (U"Saving")
NORMAL (U"Use @@Save as NeXT/Sun file...@. The samples of the Sound are multiplied by 32768 "
	"and quantized between -32768 and 32767; "
	"the result is written to the file in 16-bit mono Sun audio format.")
NORMAL (U"To avoid clipping, keep the absolute amplitude below 1.000. "
	"If the maximum sound pressure level is 91 dB (top = 32767), "
	"the quantization threshold is (top = 1/2) -5 dB.")
MAN_END

MAN_BEGIN (U"Sound files 2.5. NIST files", U"ppgb", 20040223)
NORMAL (U"An audio file type used by speech researchers. Used, for instance, in the TIMIT database. "
	"Praat reads several kinds of NIST files: big-endian, little-endian, μ-law, A-law, Polyphone. "
	"NIST files compressed with #shorten are no longer supported "
	"(see @@Sound files 1.6. Compression|§1.6@)")
MAN_END

MAN_BEGIN (U"Sound files 2.6. FLAC files", U"Erez Volk", 20070514)
NORMAL (U"A lossless compressed audio format (see ##http://flac.sourceforge.net##). "
	"Praat reads FLAC files in all bitrate/frequency settings "
	"(see also @@Sound files 1.6. Compression|§1.6@)")
MAN_END

MAN_BEGIN (U"Sound files 2.7. MP3 files", U"Erez Volk", 20070601)
NORMAL (U"A ubiquitous lossy audio compression format. "
	"Praat supports MP3 decoding through the MPEG Audio Decoder library "
	"##http://www.underbit.com/products/mad/##. "
	"(see also @@Sound files 1.6. Compression|§1.6@)")
MAN_END

MAN_BEGIN (U"Sound files 2.8. Ogg Vorbis files", U"djmw", 20210102)
NORMAL (U"Vorbis is a general purpose patent-free lossy audio compression format. "
	"Praat supports Ogg Vorbis decoding through open source code made available at "
	"##https://xiph.org/downloads/##. We used libogg-1.3.4 and libvorbis-1.3.7. "
	"(see also @@Sound files 1.6. Compression|§1.6@)")
MAN_END

MAN_BEGIN (U"Sound files 2.9. Ogg Opus files", U"djmw", 20210102)
NORMAL (U"Opus is a general purpose patent-free lossy audio compression format. "
	"It is a newer and better formant than  @@Sound files 2.8. Ogg Vorbis files|Vorbis@. "
	"It was developed by the Xiph.Org Foundation and standardized by the Internet Engineering Task Force, designed to efficiently "
	"code speech and general audio in a single format, while remaining low-latency enough for real-time interactive communication "
	"and low-complexity enough for low-end embedded processors. "
	"Praat supports Ogg Opus decoding through open source code made available at "
	"##https://opus-codec.org/downloads/##. We used opus-1.3.1 and opusfile-0.12. "
	"(see also @@Sound files 1.6. Compression|§1.6@)")
MAN_END

MAN_BEGIN (U"Sound files 3. Files that Praat can read", U"ppgb", 20070602)
INTRO (U"Praat can read five types of standard sound files in several formats, "
	"and a number of proprietary types of sound files as well.")
ENTRY (U"Standard sound files")
NORMAL (U"The @@Open menu@ contains two commands for opening sound files:")
LIST_ITEM (U"• With @@Read from file...@, you read the entire file into memory. "
	"A @Sound object will appear in the list. "
	"This is appropriate if your sound is not too long to fit into memory. "
	"The advantage of having a @Sound object is that you can perform analysis "
	"on it directly.")
LIST_ITEM (U"• With @@Open long sound file...@, you open a sound file that is too long "
	"to read into memory completely. A @LongSound object will appear in the list. "
	"You will typically choose #View to view the contents of this sound and analyse it. "
	"Praat will only read so much of the file into memory as is needed to play or display "
	"parts of it.")
NORMAL (U"Both commands understand the following five standard audio file formats:")
LIST_ITEM (U"• WAV:")
LIST_ITEM1 (U"• linear 16-bit little-endian")
LIST_ITEM1 (U"• 8-bit μ-law")
LIST_ITEM1 (U"• 8-bit A-law")
LIST_ITEM1 (U"• linear 8-bit unsigned")
LIST_ITEM (U"• AIFF or AIFC:")
LIST_ITEM1 (U"• linear 16-bit big-endian")
LIST_ITEM1 (U"• linear 8-bit signed")
LIST_ITEM (U"• NeXT/Sun (.au):")
LIST_ITEM1 (U"• linear 16-bit big-endian")
LIST_ITEM1 (U"• 8-bit μ-law")
LIST_ITEM1 (U"• 8-bit A-law")
LIST_ITEM1 (U"• linear 8-bit signed")
LIST_ITEM (U"• NIST:")
LIST_ITEM1 (U"• linear 16-bit little-endian")
LIST_ITEM1 (U"• linear 16-bit big-endian")
LIST_ITEM1 (U"• 8-bit μ-law")
LIST_ITEM1 (U"• 8-bit A-law")
LIST_ITEM1 (U"• linear 8-bit signed")
LIST_ITEM (U"• FLAC:")
LIST_ITEM1 (U"• 8-bit, 16-bit, 24-bit or 32-bit")
LIST_ITEM1 (U"• all sampling frequencies")
LIST_ITEM (U"• MP3:")
LIST_ITEM1 (U"• all constant or variable bitrates")
MAN_END

MAN_BEGIN (U"Sound files 4. Files that Praat can write", U"ppgb", 20110129)
INTRO (U"Praat can write five types of standard sound files in an appropriate linear 16-bit formats, "
	"and a number of proprietary types of sound files as well:")
LIST_ITEM (U"• @@Save as WAV file...@ (16-bit little-endian)")
LIST_ITEM (U"• @@Save as AIFF file...@ (16-bit big-endian)")
LIST_ITEM (U"• @@Save as AIFC file...@ (16-bit big-endian)")
LIST_ITEM (U"• @@Save as NeXT/Sun file...@ (16-bit big-endian)")
LIST_ITEM (U"• @@Save as NIST file...@ (16-bit little-endian)")
LIST_ITEM (U"• @@Save as FLAC file...@ (16-bit)")
MAN_END

MAN_BEGIN (U"Save as WAV file...", U"ppgb", 20110129)
INTRO (U"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit little-endian WAV file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (U"Save as AIFF file...", U"ppgb", 20110129)
INTRO (U"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit big-endian AIFF file on disk. See the @@Sound files@ tutorial for more information.")
/*ENTRY (U"AIFF file format for writing")
NORMAL (U"Although")*/
MAN_END

MAN_BEGIN (U"Save as AIFC file...", U"ppgb", 20110129)
INTRO (U"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit big-endian AIFC file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (U"Save as NeXT/Sun file...", U"ppgb", 20110129)
INTRO (U"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit big-endian NeXT/Sun (.au) file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (U"Save as NIST file...", U"ppgb", 20110129)
INTRO (U"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit little-endian NIST audio file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (U"Save as FLAC file...", U"ppgb", 20110129)
INTRO (U"With this command, you save one or more selected @Sound and/or @LongSound objects "
	"as a single 16-bit FLAC audio file on disk. See the @@Sound files@ tutorial for more information.")
MAN_END

MAN_BEGIN (U"NIST files", U"Erez Volk", 20110131)
INTRO (U"A way for storing a @Sound object on disk.")
ENTRY (U"File format")
NORMAL (U"The compressed sound files of the Timit database, and the Groningen speech corpus.")
ENTRY (U"Reading")
NORMAL (U"With @@Read from file...@.")
ENTRY (U"Saving")
NORMAL (U"With ##Save as NIST audio file...#.")
MAN_END

MAN_BEGIN (U"FLAC files", U"Erez Volk", 20110131)
INTRO (U"A way for storing a @Sound object on disk.")
ENTRY (U"File format")
NORMAL (U"A lossless compressed audio file format.")
ENTRY (U"Reading")
NORMAL (U"With @@Read from file...@.")
ENTRY (U"Saving")
NORMAL (U"With ##Save as FLAC audio file...#.")
MAN_END

MAN_BEGIN (U"How to concatenate sound files", U"ppgb", 20110131)
INTRO (U"You can concatenate any combination of AIFF, AIFC, WAV, NeXT/Sun, NIST "
	"and FLAC audio files, and other files that you have read into memory.")
NORMAL (U"For instance, if you want to concatenate a 30-minute AIFF file, "
	"a 4-minute Kay sound file, and a 60-minute Next/Sun file, "
	"by saving them into a 94-minute WAV file, "
	"you do the following:")
LIST_ITEM (U"1. Open the AIFF file with @@Open long sound file...@ from the @@Open menu@. "
	"A @LongSound object will appear in the list.")
LIST_ITEM (U"2. Read the Kay sound file into memory with @@Read from file...@. "
	"A @Sound object will appear in the list.")
LIST_ITEM (U"3. Open the AIFF file with @@Open long sound file...@ from the @@Open menu@. "
	"A second @LongSound object will appear in the list.")
LIST_ITEM (U"4. Select the three objects and choose @@Save as WAV file...@ "
	"from the @@Save menu@.")
NORMAL (U"This only works if all the sounds have the same sampling frequency "
	"and the same number of channels.")
ENTRY (U"Available formats")
NORMAL (U"The format of the original sound files may be 16-bit linear "
	"(with big-endian or little-endian byte order), 8-bit linear (signed or unsigned), "
	"8-bit μ-law, or 8-bit A-law. The format of the resulting sound file is always "
	"16-bit linear, with an appropriate default byte order. The following commands "
	"are available in the @@Save menu@ if you select any combination of @LongSound "
	"and/or @Sound objects:")
LIST_ITEM (U"• @@Save as WAV file...@ (little-endian)")
LIST_ITEM (U"• @@Save as AIFF file...@ (big-endian)")
LIST_ITEM (U"• @@Save as AIFC file...@ (big-endian)")
LIST_ITEM (U"• @@Save as NeXT/Sun file...@ (big-endian)")
LIST_ITEM (U"• @@Save as NIST file...@ (little-endian)")
LIST_ITEM (U"• @@Save as FLAC file...@")
MAN_END

MAN_BEGIN (U"LongSound", U"ppgb", 20110131)
INTRO (U"One of the @@types of objects@ in Praat. See the @@Sound files@ tutorial.")
NORMAL (U"A LongSound object gives you the ability to view and label "
	"a sound file that resides on disk. You will want to use it for sounds "
	"that are too long to read into memory as a @Sound object (typically, a few minutes).")
ENTRY (U"How to create a LongSound object")
NORMAL (U"You create a LongSound object with @@Open long sound file...@ from the @@Open menu@.")
ENTRY (U"What you can do with a LongSound object")
NORMAL (U"You can save a LongSound object to a new sound file, "
	"perhaps in a different format (AIFF, AIFC, WAV, NeXT/Sun, NIST, FLAC) "
	"with the commands in the Save menu. You can also concatenate several "
	"LongSound objects in this way. See @@How to concatenate sound files@.")
ENTRY (U"How to view and edit a LongSound object")
NORMAL (U"You can view a LongSound object in a @LongSoundEditor by choosing @@LongSound: View@. "
	"This also allows you to extract parts of the LongSound as @Sound objects, "
	"or save these parts as a sound file. "
	"There are currently no ways to actually change the data in the file.")
ENTRY (U"How to annotate a LongSound object")
NORMAL (U"You can label and segment a LongSound object after the following steps:")
LIST_ITEM (U"1. Select the LongSound object.")
LIST_ITEM (U"2. Choose @@LongSound: To TextGrid...@ and specify your tiers.")
LIST_ITEM (U"3. Select the resulting @TextGrid object together with the LongSound object, and click ##View & Edit#.")
NORMAL (U"A @TextGridEditor will appear on the screen, with a copy of the LongSound object in it.")
ENTRY (U"Limitations")
NORMAL (U"The length of the sound file is limited to 2 gigabytes, which is 3 hours of CD-quality stereo, "
	"or 12 hours 16-bit mono sampled at 22050 Hz.")
MAN_END

MAN_BEGIN (U"LongSound: To TextGrid...", U"ppgb", 19980730)
INTRO (U"A command to create a @TextGrid without any labels, copying the time domain from the selected @LongSound.")
NORMAL (U"See @@Sound: To TextGrid...@ for the settings.")
MAN_END

MAN_BEGIN (U"LongSound: View", U"ppgb", 19980730)
INTRO (U"A command to view the selected @LongSound object in a @LongSoundEditor.")
MAN_END

MAN_BEGIN (U"LongSoundEditor", U"ppgb", 20041124)
INTRO (U"One of the @Editors in Praat, for viewing a @LongSound object.")
NORMAL (U"This viewer allows you:")
LIST_ITEM (U"• to view and hear parts of the sound as it is on disk;")
LIST_ITEM (U"• to copy a selected part as a @Sound object to the list of objects, so that you "
	"can perform analyses on it or save it to a smaller sound file;")
LIST_ITEM (U"• to copy a selected part as a @Sound object to the @Sound clipboard, "
	"so that you can paste it into another Sound object that you are viewing in a @SoundEditor.")
NORMAL (U"To label and segment the LongSound object, use the @TextGridEditor instead (see @LongSound).")
NORMAL (U"The display and playback of the samples is restricted to 60 seconds at a time, for reasons of speed "
	"(although you can change this number with ##LongSound prefs# from the main #Preferences menu; "
	"the sound file itself can contain several hours of sound.")
MAN_END

MAN_BEGIN (U"Macintosh sound files", U"ppgb", 20110131)
INTRO (U"A way for storing a @Sound object on disk.")
ENTRY (U"File format")
NORMAL (U"The double-clickable sound file of the Macintosh (8 bits per sample). "
	"Cannot be ported to other machines, because the sound is in the @@resource fork@.")
ENTRY (U"Reading")
NORMAL (U"With @@Read from file...@.")
NORMAL (U"To read a Sound from a Macintosh sound file on disk, use @@Read from file...@ (Macintosh only).")
NORMAL (U"The 8-bit sample values are divided by 128 so that the amplitude "
	"of the resulting Sound is between –1.0 and +1.0.")
NORMAL (U"The resulting Sound will appear in the list of objects; "
	"its name will be equal to the file name, without extension.")
ENTRY (U"Saving")
NORMAL (U"With ##Save as Mac sound file...#. Praat asks you for a file name. "
	"After you click OK, 0.5 is added to the samples of the Sound, "
	"they are multiplied by 128 and quantized between 0 and 255; "
	"the result is written to the file in 8-bit linear Macintosh sound-file format.")
NORMAL (U"To avoid clipping, keep the absolute amplitude below 1.000. "
	"If the maximum sound pressure level is 91 dB (top = 127), "
	"the quantization threshold for a sine wave is (top = 1/2) 43 dB.")
MAN_END

MAN_BEGIN (U"Open long sound file...", U"ppgb", 19980730)
INTRO (U"A command in the @@Open menu@ that creates a @LongSound object.")
NORMAL (U"The file will be opened for reading only. The file stays open until you remove the LongSound object.")
MAN_END

MAN_BEGIN (U"Sesam/LVS files", U"ppgb", 20170828)
INTRO (U"A way for storing a @Sound object on disk.")
ENTRY (U"File format")
NORMAL (U"The sound files used by the SESAM and LVS programs. Each sample is normally quantized into 12 bits.")
ENTRY (U"Reading")
NORMAL (U"To read a #Sound from a Sesam file on disk, use @@Read from file...@. "
	"The file name is expected to end in \".sdf\" or \".SDF\".")
NORMAL (U"The 12-bit sample values are divided by 2048 so that the amplitude "
	"of the resulting Sound is between -1.0 and +1.0.")
NORMAL (U"The resulting #Sound will appear in the List of Objects; "
	"its name will be equal to the file name, without extension.")
NORMAL (U"If the sound was encoded in 16 bits per sample, you should divide by 16 after reading "
	"(with $$Formula: ~ self / 16$)")
ENTRY (U"Saving")
NORMAL (U"With ##Save as Sesam file...#. Praat then asks you for a file name. "
	"After you click OK, the samples of the Sound are multiplied by 2048 "
	"and quantized between -2048 and 2047; "
	"the result is written to the file in 12-bit LVS and Sesam format.")
NORMAL (U"To avoid clipping, keep the absolute amplitude below 1.000. "
	"If the maximum sound pressure level is 91 dB (top = 2047), "
	"the quantization threshold is (top = 1/2) 19 dB.")
NORMAL (U"If you prefer 16-bit encoding, you should multiply by 16 before saving "
	"(with $$Formula: ~ self * 16$)")
MAN_END

}

/* End of file manual_soundFiles.cpp */
