/* manual_tutorials.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016,2017 Paul Boersma
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
#include "praat_version.h"

void manual_tutorials_init (ManPages me);
void manual_tutorials_init (ManPages me) {

MAN_BEGIN (U"What's new?", U"ppgb", 20170722)
INTRO (U"Latest changes in Praat.")
//LIST_ITEM (U"• Manual page about @@drawing a vowel triangle@.")

NORMAL (U"##6.0.30# (22 July 2017)")
LIST_ITEM (U"• Removed a bug that caused an incorrect title for a PitchTier or PointProcess window.")
LIST_ITEM (U"• Removed a bug that caused Praat to crash when doing a linear regression on a Table with no rows.")
LIST_ITEM (U"• Scripting: $$object[]$, @@Scripting 5.7. Vectors and matrices|vectors@.")
LIST_ITEM (U"• Graphics: better text drawing details.")
LIST_ITEM (U"• Linux: possibility to compile Praat without a GUI but with graphics file output.")
NORMAL (U"##6.0.29# (24 May 2017)")
LIST_ITEM (U"• Sound window: channel muting.")
LIST_ITEM (U"• Linux: support for Chinese, Japanese, Korean, Indic, Arabic and Hebrew characters in TextGrids and elsewhere.")
NORMAL (U"##6.0.28# (23 March 2017)")
LIST_ITEM (U"• Scripting: $$demoPeekInput()$ for animations in combination with $$sleep()$.")
NORMAL (U"##6.0.27# (18 March 2017)")
LIST_ITEM (U"• TextGrid: fewer error messages in concatenation of multiple TextGrids.")
LIST_ITEM (U"• Scripting: $$sleep()$ to pause Praat temporarily: useful for animations in combination with $$demoShow()$.")
NORMAL (U"##6.0.26# (2 March 2017)")
LIST_ITEM (U"• Mac: more corrections in $$demoShow()$ and $$blankWhilePlaying$.")
LIST_ITEM (U"• PraatBarren: better error message when an attempt is made to run PraatBarren interactively.")
NORMAL (U"##6.0.25# (11 February 2017)")
LIST_ITEM (U"• Mac: made $$demoShow()$ and $$blankWhilePlaying$ compatible with MacOS 10.12 Sierra.")
LIST_ITEM (U"• Mac SoundRecorder: more sampling frequencies, on behalf of external USB microphones.")
NORMAL (U"##6.0.24# (23 January 2017)")
LIST_ITEM (U"• Fixed a bug by which ##Remove right boundary# would choose the wrong tier.")
LIST_ITEM (U"• TextGrid window: click to insert a phonetic symbol from an IPA chart.")
NORMAL (U"##6.0.23# (12 December 2016)")
LIST_ITEM (U"• Linux: fixed a bug that caused Praat to crash when playing a sound of more than 7 channels.")
LIST_ITEM (U"• Change Gender: fixed a bug introduced in 6.0.22 by which the pitch range factor could not be 0.")
LIST_ITEM (U"• Improvements in the manual and in texts.")
NORMAL (U"##6.0.22# (15 November 2016)")
LIST_ITEM (U"• Scripting: correct error messages for expressions like: 5 + \"hello\"")
LIST_ITEM (U"• Command line: the --open option works correctly in the GUI if you open multiple files.")
//LIST_ITEM (U"• Sound window: option for showing time in minutes instead of just seconds.")   // too confusing in UI
NORMAL (U"##6.0.21# (25 September 2016)")
LIST_ITEM (U"• ExperimentMFC: you can now specify font sizes and response keys for goodness judgments.")
LIST_ITEM (U"• Table: when drawing ellipses, a font size of 0 can now be used to prevent drawing the labels.")
LIST_ITEM (U"• Mac: dragging selections repaired for System 10.11.6 (but we advise to upgrade to 10.12).")
LIST_ITEM (U"• Mac: re-enabled Return key for confirming some dialog boxes.")
NORMAL (U"##6.0.20# (3 September 2016)")
LIST_ITEM (U"• Can open UTF-8 text files with Byte Order Mark.")
LIST_ITEM (U"• Scripting improvement: function names can now be used as names of indexed variables.")
NORMAL (U"##6.0.19# (13 June 2016)")
LIST_ITEM (U"• Mac: dragging selections repaired for System 10.11.5.")
NORMAL (U"##6.0.18# (23 May 2016)")
LIST_ITEM (U"• Windows: better dotted lines.")
LIST_ITEM (U"• TextGrid window: again better automatic alignment.")
NORMAL (U"##6.0.17# (21 April 2016)")
LIST_ITEM (U"• TextGrid window: better automatic alignment.")
NORMAL (U"##6.0.16# (5 April 2016)")
LIST_ITEM (U"• Scripting: \"hashes\": variables can now be indexed with strings rather than only with numbers.")
LIST_ITEM (U"• TextGrid window: fewer out-of-order messages in automatic alignment.")
NORMAL (U"##6.0.15# (21 March 2016)")
LIST_ITEM (U"• TextGrid window: removed a bug whereby Praat could do automatic alignment only on sounds sampled at 44.1 kHz.")
LIST_ITEM (U"• TextGrid window: improved the location of the final boundary in automatic alignment.")
LIST_ITEM (U"• Table window: added a preference for the interpretation of graphical style symbols (\\% \\# \\^ \\_ \\@ ).")
NORMAL (U"##6.0.14# (11 February 2016)")
LIST_ITEM (U"• Linux: corrected a bug by which the Tab short-cut for playing a sound would not work if the NumLock key was on.")
LIST_ITEM (U"• Mac 64-bit: corrected a bug that could cause Praat to crash if the Tab key was pressed in a dialog window.")
NORMAL (U"##6.0.13# (31 January 2016)")
LIST_ITEM (U"• Windows: corrected a bug that could cause Praat to crash when closing a script window.")
LIST_ITEM (U"• Mac 64-bit: progress bars with movies.")
NORMAL (U"##6.0.12# (24 January 2016)")
LIST_ITEM (U"• Windows 32-bit: corrected a bug that could cause Praat to crash when closing the Sound window after popping up dialog boxes.")
LIST_ITEM (U"• Mac 64-bit: better $$demoShow()$.")
LIST_ITEM (U"• Mac 64-bit: working Matrix movies and articulatory synthesizer movies.")
NORMAL (U"##6.0.11# (18 January 2016)")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash when drawing a Spectrum with undefined values.")
LIST_ITEM (U"• Mac 64-bit: corrected a bug by which some text in the Sound window would sometimes not be drawn.")
LIST_ITEM (U"• Mac 64-bit demo window: improved the working of $$demoShow()$.")
NORMAL (U"##6.0.10# (8 January 2016)")
LIST_ITEM (U"• Corrected a bug that would cause Praat to crash when starting an ExperimentMFC window.")
LIST_ITEM (U"• Mac 64-bit: corrected a bug that would cause black selections in the Sound window.")
NORMAL (U"##6.0.09# (1 January 2016)")
LIST_ITEM (U"• Windows 64-bit: corrected a bug that could cause Praat to crash when closing the Sound window after popping up dialog boxes.")
NORMAL (U"##6.0.08# (5 December 2015)")
LIST_ITEM (U"• Windows command line: the $$--a$ option can be used when redirecting the output to pipes and files.")
LIST_ITEM (U"• Linux command line: the $$--run$ option is not needed in the Terminal when redirecting output "
	"(unless you redirect standard output, standard input %and standard error).")
NORMAL (U"##6.0.07# (30 November 2015)")
LIST_ITEM (U"• Mac 64-bit: repaired dragging selections (which got broken in the El Capitan fix in 6.0.06).")
NORMAL (U"##6.0.06# (29 November 2015)")
LIST_ITEM (U"• Mac 64-bit: correct early Info window updates and $$demoShow()$ in El Capitan.")
LIST_ITEM (U"• Manual updates.")
LIST_ITEM (U"• Corrected some small memory leaks.")
NORMAL (U"##6.0.05# (8 November 2015)")
LIST_ITEM (U"• A manual page that describes @@TextGrid file formats@.")
LIST_ITEM (U"• Corrected a bug that prevented some KlattGrid tiers from being edited.")
LIST_ITEM (U"• 32-bit Linux: correct use of PulseAudio (note: 6.0.04 on Debian already had this correct).")
NORMAL (U"##6.0.04# (1 November 2015)")
LIST_ITEM (U"• Corrected a bug that caused an incorrect number in FFNet files; "
	"unreadable FFNet files can be sent to the Praat authors for repair.")
NORMAL (U"##6.0.03# (31 October 2015)")
LIST_ITEM (U"• Corrected a bug that caused Praat to crash when the Soundrecorder window was closed.")
LIST_ITEM (U"• Scripting: when calling Praat from the command line, added the $$--run$ option to force script execution.")
NORMAL (U"##6.0.02# (30 October 2015)")
LIST_ITEM (U"• Corrected the redrawing of analyses in the Sound window after a change in the sound.")
NORMAL (U"##6.0.01# (29 October 2015)")
LIST_ITEM (U"• Windows: fixed a bug that caused Praat to crash at start-up in some Windows versions.")
NORMAL (U"##6.0# (28 October 2015)")
LIST_ITEM (U"• Linux: use PulseAudio as the server for playing sound.")
LIST_ITEM (U"• Windows: removed Praatcon.exe, because Praat.exe can now be used from the command line.")
LIST_ITEM (U"• Praat can now open files from the command line on all platforms.")
LIST_ITEM (U"• The API change that requires raising the major Praat version to 6: "
	"no longer any need for Praatcon.exe, because Praat itself can now run a script from the command line on all platforms.")
LIST_ITEM (U"• See @@Scripting 6.9. Calling from the command line@ for all command line options.")
LIST_ITEM (U"• SpeechSynthesizer: corrected a potentially crashing bug.")
LIST_ITEM (U"• Mac Retina graphics: made function drawing (e.g. in the TextGrid window) twice as fast.")
LIST_ITEM (U"• LongSound: corrected playing of more than 2 channels.")
NORMAL (U"##5.4.22# (8 October 2015)")
LIST_ITEM (U"• Sound files: added support for MP3 files with ID3 version 2 headers.")
LIST_ITEM (U"• Table: Line graph where: removed a bug that caused nothing to be drawn if the number of columns was larger than the number of rows and "
	"the column index was larger than the number of rows.")
LIST_ITEM (U"• Mac: corrected horizontal spacing in graphical text on El Capitán.")
LIST_ITEM (U"• Mac: corrected a bug that would cause Praat to crash if a PNG file could not be saved (from a script).")
LIST_ITEM (U"• Windows: enabled PNG saving from Praatcon.")
NORMAL (U"##5.4.21# (29 September 2015)")
LIST_ITEM (U"• Corrected a bug introduced in 5.4.20 that could cause the Sound window to crash.")
LIST_ITEM (U"• Repaired a very old hang with pulses in the Sound window.")
NORMAL (U"##5.4.20# (26 September 2015)")
LIST_ITEM (U"• Corrected the graphical representation of numbers with exponents (wrong since 5.4.10).")
LIST_ITEM (U"• Windows: prevented flashing console window in $$runScript()$.")
NORMAL (U"##5.4.19# (16 September 2015)")
LIST_ITEM (U"• LongSound: corrected saving of 8-bit files as 16-bit.")
NORMAL (U"##5.4.18# (7 September 2015)")
LIST_ITEM (U"• Regular expressions: if the pattern is illegal, you'll now get an error message instead of an empty string.")
LIST_ITEM (U"• LongSound: removed a bug introduced in 1999 that could play long stereo sounds much too fast if they had unusual sampling frequencies. "
	"This bug has become more noticeable on the Mac since January 2015.")
LIST_ITEM (U"• Guard better against sound files that contain 0 samples.")
LIST_ITEM (U"• Scripting: faster look-up of variables.")
NORMAL (U"##5.4.17# (20 August 2015)")
LIST_ITEM (U"• Scripting: removed a bug that could cause Praat to crash when calling $$writeFile()$ with an empty text.")
NORMAL (U"##5.4.16# (16 August 2015)")
LIST_ITEM (U"• Mac 64-bit: removed a bug by which graphical text passed over the right edge in TextGrid intervals.")
LIST_ITEM (U"• Mac 64-bit: removed a bug that allowed the general \"Zoom\" and \"Close\" commands to be accessible from a script.")
NORMAL (U"##5.4.15# (1 August 2015)")
LIST_ITEM (U"• Introduced several command line options that make it easier to call Praat from a web server.")
LIST_ITEM (U"• 64-bit Mac: removed a bug introduced in 5.4.11 that caused incorrect spacing of vertical text in picture files.")
NORMAL (U"##5.4.14# (24 July 2015)")
LIST_ITEM (U"• Windows and 32-bit Mac: removed a bug introduced in 5.4.12 that could cause weird behavior (even crashing) when "
	"pressing the Tab key in the script window or in the manual.")
NORMAL (U"##5.4.13# (22 July 2015)")
LIST_ITEM (U"• The Demo window can now save itself to a PNG or PDF file.")
LIST_ITEM (U"• Windows: removed a bug introduced in 5.4.10 whereby Praatcon would write garbled text to the console.")
NORMAL (U"##5.4.12# (10 July 2015)")
LIST_ITEM (U"• Windows: removed a bug introduced in 5.4.10 whereby SpeechSynthesizer would only pronounce the first letter of the text.")
NORMAL (U"##5.4.11# (8 July 2015)")
LIST_ITEM (U"• Windows: removed a bug introduced in 5.4.10 whereby the file selection window could try to open a file with a garbled name.")
LIST_ITEM (U"• Removed a bug whereby the PointProcess window would crash when showing a perfectly silent sound.")
NORMAL (U"##5.4.10# (27 June 2015)")
LIST_ITEM (U"• Removed a bug introduced in version 4.5.09 (January 2007) that caused incorrect "
	"upsampling of stereo sounds when the upsampling factor was exactly 2. This bug has caused "
	"incorrect playing of stereo sounds with a sampling frequency of 22050 Hz on the Mac since January 2015.")
LIST_ITEM (U"• Removed a bug introduced in 2011 that could cause Praat to crash when you scrolled the LongSound window.")
LIST_ITEM (U"• TextGrid: ##Count intervals where...# and ##Count points where...#.")
NORMAL (U"##5.4.09# (1 June 2015)")
LIST_ITEM (U"• Linux: the tab key can be used to play sounds on more computers.")
LIST_ITEM (U"• Windows: TextGrid files with non-BMP characters are now read correctly.")
LIST_ITEM (U"• Windows: files with names that contain non-BMP characters are now saved correctly.")
LIST_ITEM (U"• Updated manual.")
NORMAL (U"##5.4.08# (24 March 2015)")
LIST_ITEM (U"• Sound window: removed a bug that caused a crash in \"Editor info\".")
NORMAL (U"##5.4.07# (22 March 2015)")
LIST_ITEM (U"• TextGrid window: ##Add interval# now does the same thing as ##Add boundary# if a time stretch is selected.")
LIST_ITEM (U"• Linux: better redrawing in TextGrid window.")
NORMAL (U"##5.4.06# (21 February 2015)")
LIST_ITEM (U"• Windows: repaired a bug that could cause Praat to crash if a metafile resolution was 200 dpi.")
NORMAL (U"##5.4.05# (13 February 2015)")
LIST_ITEM (U"• Better support for big integer numbers on Windows XP.")
LIST_ITEM (U"• Sound window: guarded against empty view ranges for the intensity curve.")
NORMAL (U"##5.4.04# (28 December 2014)")
LIST_ITEM (U"• Windows audio playback: if the sound has more channels than the audio hardware, distribute them evenly.")
NORMAL (U"##5.4.03# (18 December 2014)")
LIST_ITEM (U"• TextGrid reading: Praat now corrects some incomplete TextGrid files created by others.")
LIST_ITEM (U"• Better support for text files larger than 2 GB.")
NORMAL (U"##5.4.02# (26 November 2014)")
LIST_ITEM (U"• Mac: repaired a bug by which quote characters typed into the script window could become curly instead of straight.")
NORMAL (U"##5.4.01# (9 November 2014)")
LIST_ITEM (U"• @MelSpectrogram, @BarkSpectrogram.")
LIST_ITEM (U"• Linux: removed a bug that could cause too many flashes when scrolling a Sound window.")
LIST_ITEM (U"• Mac: repaired a bug that could cause a crash in the VowelEditor window.")
ENTRY (U"What used to be new?")
LIST_ITEM (U"• @@What was new in 5.4?")
LIST_ITEM (U"• @@What was new in 5.3?")
LIST_ITEM (U"• @@What was new in 5.2?")
LIST_ITEM (U"• @@What was new in 5.1?")
LIST_ITEM (U"• @@What was new in 5.0?")
LIST_ITEM (U"• @@What was new in 4.6?")
LIST_ITEM (U"• @@What was new in 4.5?")
LIST_ITEM (U"• @@What was new in 4.4?")
LIST_ITEM (U"• @@What was new in 4.3?")
LIST_ITEM (U"• @@What was new in 4.2?")
LIST_ITEM (U"• @@What was new in 4.1?")
LIST_ITEM (U"• @@What was new in 4.0?")
LIST_ITEM (U"• @@What was new in 3.9?")
LIST_ITEM (U"• @@What was new in 3.8?")
LIST_ITEM (U"• @@What was new in 3.7?")
LIST_ITEM (U"• @@What was new in 3.6?")
LIST_ITEM (U"• @@What was new in 3.5?")
LIST_ITEM (U"• @@What was new in 3.3?")
LIST_ITEM (U"• @@What was new in 3.2?")
LIST_ITEM (U"• @@What was new in 3.1?")
MAN_END

MAN_BEGIN (U"What was new in 5.4?", U"ppgb", 20141003)
NORMAL (U"##5.4# (4 October 2014)")
NORMAL (U"##5.3.87# (3 October 2014)")
LIST_ITEM (U"• Windows scripting: prevented incorrect handling of relative paths after the use of chooseReadFile\\$ .")
LIST_ITEM (U"• Windows: repaired a bug that could cause Praat to crash if a metafile resolution was 180 or 1200 dpi.")
NORMAL (U"##5.3.86# (28 September 2014)")
LIST_ITEM (U"• Linux audio: reverted to old version of PortAudio because of race problems in the Alsa–PulseAudio cooperation.")
NORMAL (U"##5.3.85# (19 September 2014)")
LIST_ITEM (U"• Mac audio: circumvented a bug in PortAudio by which playback of sounds with sampling frequencies "
	"below 44100 Hz could be distorted on some USB headsets.")
NORMAL (U"##5.3.84# (26 August 2014)")
LIST_ITEM (U"• Manipulation: repaired a bug that could cause Praat to crash when playing a manipulation of a Sound created by the SpeechSynthesizer.")
NORMAL (U"##5.3.83# (16 August 2014)")
LIST_ITEM (U"• TextGrid window: repaired a bug with automatic alignment that could cause an analysis tier to go out of order.")
LIST_ITEM (U"• Linux audio: created a second workaround that reduces even more the chances of a freeze that is due to a potential deadlock "
	"in the collaboration between Alsa and PulseAudio that can occur when the playback of a sound is cancelled.")
LIST_ITEM (U"• Smoother communication with Phon.")
LIST_ITEM (U"• Windows: repaired a memory leak when saving PNG files.")
NORMAL (U"##5.3.82# (26 July 2014)")
LIST_ITEM (U"• Linux and Mac audio playback: if the sound has more channels than the audio hardware, distribute them evenly.")
LIST_ITEM (U"• Pause forms: more consistent appearance of the Revert button.")
LIST_ITEM (U"• Scripting: pauseScript ( ) function.")
NORMAL (U"##5.3.81# (2 July 2014)")
LIST_ITEM (U"• EEG: can work with status %numbers instead of only with status %bits.")
LIST_ITEM (U"• Windows: repaired a bug that could cause Praat to crash if there was a 96-dpi printer.")
NORMAL (U"##5.3.80# (29 June 2014)")
LIST_ITEM (U"• Praat preferences: choice between Chinese and Japanese style for Han characters.")
NORMAL (U"##5.3.79# (21 June 2014)")
LIST_ITEM (U"• Can now play sounds over more than two channels.")
LIST_ITEM (U"• Asynchronous play in scripts (see @@Demo window@).")
LIST_ITEM (U"• EEG: blue-to-red colour scale for scalp distributions.")
NORMAL (U"##5.3.78# (12 June 2014)")
LIST_ITEM (U"• Multithreading can now speed up pitch analysis by a factor of 4 or so, "
	"depending on the number of cores in your processor.")
LIST_ITEM (U"• Linux: can now open and save Photo objects (from PNG files) "
	"and use @@Insert picture from file...@.")
LIST_ITEM (U"• Open WAV files that are in the \"extensible\" format (previously \"unsupported format -2\").")
LIST_ITEM (U"• Windows: support for dropping more than one file on the Praat icon.")
LIST_ITEM (U"• Scripting: can now use the #editor command with an object's ID instead of only with its name.")
LIST_ITEM (U"• Windows: removed a bug that sometimes disallowed saving more than one JPEG file.")
LIST_ITEM (U"• Linux audio: created a workaround that reduces the chances of a freeze that is due to a potential deadlock "
	"in the collaboration between Alsa and PulseAudio that can occur when the playback of a sound is cancelled.")
NORMAL (U"##5.3.77# (18 May 2014)")
LIST_ITEM (U"• EEG: more facilities for EDF+ files.")
NORMAL (U"##5.3.76# (8 May 2014)")
LIST_ITEM (U"• One can determine the size of \"speckles\" (filled circles) with ##Speckle size...# in the #Pen menu. "
	"Speckles are used in drawing Formant, PitchTier, and several other kinds of objects.")
NORMAL (U"##5.3.75# (30 April 2014)")
LIST_ITEM (U"• Linux Matrix graphics bug fix: corrected working of ##Draw cells...#.")
LIST_ITEM (U"• Scripting bug fix: ability to use x and y as indexed variables.")
LIST_ITEM (U"• PowerCepstrogram bug fix: made old version of Paint command available again for scripts.")
NORMAL (U"##5.3.74# (24 April 2014)")
LIST_ITEM (U"• EEG: more interpretation of triggers in EDF+ files.")
NORMAL (U"##5.3.73# (21 April 2014)")
LIST_ITEM (U"• EEG: understand more EGI/NetStation files.")
NORMAL (U"##5.3.72# (17 April 2014)")
LIST_ITEM (U"• Windows: repaired a bug that caused two black edges in PNG files.")
LIST_ITEM (U"• Windows: repaired a bug that could cause Praat to crash if a metafile resolution was 360 dpi.")
LIST_ITEM (U"• Linux: repaired a bug that caused Praat to crash when cutting or pasting a sound in the Sound window.")
NORMAL (U"##5.3.71# (9 April 2014)")
LIST_ITEM (U"• Windows: brought more unity in the style of Chinese characters.")
NORMAL (U"##5.3.70# (2 April 2014)")
LIST_ITEM (U"• Added some query commands for DurationTier objects.")
LIST_ITEM (U"• Repaired a bug that caused Praat not to run as a console app.")
NORMAL (U"##5.3.69# (28 March 2014)")
LIST_ITEM (U"• Picture window: can save to 300-dpi and 600-dpi PNG files.")
LIST_ITEM (U"• Graphics: sub-pixel precision line drawing on Mac and Linux.")
LIST_ITEM (U"• Repaired a bug that could show spurious buttons in the Objects window if a plug-in created objects.")
NORMAL (U"##5.3.68# (20 March 2014)")
LIST_ITEM (U"• Mac: corrected a bug introduced in 5.3.67 that could cause crashes when drawing a spectrogram.")
LIST_ITEM (U"• Mac and Linux: @@Create Strings as file list...@ handles broken symbolic links more leniently.")
NORMAL (U"##5.3.67# (19 March 2014)")
LIST_ITEM (U"• Corrected a bug that would create strange PNG files if the selection did not touch the upper left corner of the Picture window.")
LIST_ITEM (U"• Mac: can save the Picture window to PNG file.")
LIST_ITEM (U"• EEG: understand trigger letters in BDF/EDF files.")
NORMAL (U"##5.3.66# (9 March 2014)")
LIST_ITEM (U"• Windows and Linux: can save the Picture window to PNG file.")
LIST_ITEM (U"• Windows: opening, modifying and saving PNG, TIFF or JPEG files (the Photo object, as on the Mac).")
NORMAL (U"##5.3.65# (27 February 2014)")
LIST_ITEM (U"• Scripting language: removed some bugs from runScript.")
LIST_ITEM (U"• Linux: can save the Picture window to PDF file.")
NORMAL (U"##5.3.64# (12 February 2014)")
LIST_ITEM (U"• Scripting language: writeInfo, procedure, exitScript, runScript: all with colons.")
LIST_ITEM (U"• 64-bit Mac graphics: better highlighting and unhighlighting of selection.")
LIST_ITEM (U"• 64-bit Mac graphics: full screen.")
NORMAL (U"##5.3.63# (24 January 2014)")
LIST_ITEM (U"• Scripting language: easier menu command invocation using the colon \":\".")
LIST_ITEM (U"• 64-bit Mac graphics: better handling of any absence of Doulos SIL or Charis SIL.")
LIST_ITEM (U"• Windows scripting: can now use \"~\" in file names to refer to home directory, as on Mac and Linux.")
NORMAL (U"##5.3.62# (2 January 2014)")
LIST_ITEM (U"• 64-bit Mac: removed a bug introduced in 5.3.61 that could cause text containing \"ff\" to become invisible.")
NORMAL (U"##5.3.61# (1 January 2014)")
LIST_ITEM (U"• EEG: understand status registers that contain text.")
LIST_ITEM (U"• KlattGrid: removed a bug introduced in May 2009 that could make Praat crash after editing an oral formant grid.")
NORMAL (U"##5.3.60# (8 December 2013)")
LIST_ITEM (U"• Mac 64-bit: implemented swiping (to scroll with the trackpad) and pinching (to zoom with the trackpad).")
LIST_ITEM (U"• Scripting: backslashTrigraphsToUnicode () and unicodeToBackslashTrigraphs ().")
NORMAL (U"##5.3.59# (20 November 2013)")
LIST_ITEM (U"• EEG: faster reading of BDF and EDF files.")
LIST_ITEM (U"• Batch scripting: made ##appendInfo()# write to the console in the same way as #print.")
LIST_ITEM (U"• Removed a bug introduced in 5.3.57 whereby some Praat text files could not be read.")
NORMAL (U"##5.3.58# (17 November 2013)")
LIST_ITEM (U"• EEG: support for 16-bit (next to 24-bit) BDF files and for 16-bit (next to 8-bit) statuses.")
LIST_ITEM (U"• Mac: 64-bit beta version.")
NORMAL (U"##5.3.57# (27 October 2013)")
LIST_ITEM (U"• Mac: opening, modifying and saving image files (the Photo object).")
LIST_ITEM (U"• Mac 64-bit: some small improvements in the user interface.")
NORMAL (U"##5.3.56# (15 September 2013)")
LIST_ITEM (U"• Mac: 64-bit alpha version.")
LIST_ITEM (U"• Linux: improved selecting in the Picture window.")
NORMAL (U"##5.3.55# (2 September 2013)")
LIST_ITEM (U"• Corrected a bug introduced in 5.3.54 by which you couldn't select a file for saving.")
NORMAL (U"##5.3.54# (1 September 2013)")
LIST_ITEM (U"• Sound window: removed a bug introduced in 5.3.42 by which you couldn't ask for an odd number of poles in Formant Settings "
	"(by e.g. specifying \"5.5\" for the number of formants).")
LIST_ITEM (U" Linux: improved dragging of selections in the Picture window and the Sound window.")
NORMAL (U"##5.3.53# (9 July 2013)")
LIST_ITEM (U"• Table: more drawing commands.")
NORMAL (U"##5.3.52# (12 June 2013)")
LIST_ITEM (U"• Scripting: editor windows understand #do and ##do\\$ #.")
NORMAL (U"##5.3.51# (30 May 2013)")
LIST_ITEM (U"• Sound window: ##Extract selected sound for overlap...#.")
NORMAL (U"##5.3.49# (13 May 2013)")
LIST_ITEM (U"• TextGrid window: alignment of the sound and the annotation in an interval, via Espeak.")
LIST_ITEM (U"• Scripting: repaired a bug introduced in 5.3.32 that could cause very slow running of scripts.")
NORMAL (U"##5.3.48# (1 May 2013)")
LIST_ITEM (U"• Scripting: variable-substitution-free object selection and file writing.")
LIST_ITEM (U"• Scripting: #selectObject and #removeObject can select or remove multiple objects at a time.")
NORMAL (U"##5.3.47# (23 April 2013)")
LIST_ITEM (U"• OTGrammar: included Giorgio Magri's (2012) update rule (weighted all up, high down).")
NORMAL (U"##5.3.46# (21 April 2013)")
LIST_ITEM (U"• Scripting: variable-substitution-free procedure calls.")
LIST_ITEM (U"• Linux: made the Save menu compatible with Ubuntu 12.04.")
NORMAL (U"##5.3.45# (15 April 2013)")
LIST_ITEM (U"• More parts of the manual reflect variable-substitution-free scripting.")
NORMAL (U"##5.3.44# (7 April 2013)")
LIST_ITEM (U"• ##Create Sound as pure tone...#.")
LIST_ITEM (U"• First steps towards variable-substitution-free scripting: the #do, ##do\\$ #, #writeInfo and #appendInfo functions.")
NORMAL (U"##5.3.43# (27 March 2013)")
LIST_ITEM (U"• Read and write stereo Kay sound files.")
LIST_ITEM (U"• Phonetic symbols \\ts, \\tS and \\ap.")
LIST_ITEM (U"• Network: ##Normalize weights...#, ##List weights...#, ##Weights down to Table...#.")
NORMAL (U"##5.3.42# (2 March 2013)")
LIST_ITEM (U"• Repaired some minor bugs regarding synchronization between multiple windows.")
NORMAL (U"##5.3.41# (9 February 2013)")
LIST_ITEM (U"• Linux: repaired a bug that could cause Praat to crash when closing the Info window or a script window.")
NORMAL (U"##5.3.40# (2 February 2013)")
LIST_ITEM (U"• Windows: better handling (in the LongSound window) of sounds that are too long to play.")
LIST_ITEM (U"• Mac: corrected a bug that caused Praat to crash when closing the Strings window.")
LIST_ITEM (U"• Linux: made audio output work again on 32-bit Linux.")
NORMAL (U"##5.3.39# (6 January 2013)")
LIST_ITEM (U"• Formant & Spectrogram: To IntensityTier...")
NORMAL (U"##5.3.38# (4 January 2013)")
LIST_ITEM (U"• ExperimentMFC: repaired a bug introduced in 5.3.36 that caused sound to fail to play completely if the screen did not blank.")
NORMAL (U"##5.3.37# (2 January 2013)")
LIST_ITEM (U"• ExperimentMFC: after screen blanking, reaction times count from when the response buttons appear.")
NORMAL (U"##5.3.36# (1 January 2013)")
LIST_ITEM (U"• ExperimentMFC: made it possible to blank the screen while the sound is playing.")
NORMAL (U"##5.3.35# (8 December 2012)")
LIST_ITEM (U"• SpeechSynthesizer: made it possible to have more than one at a time.")
LIST_ITEM (U"• Linux: corrected a bug that caused incorrect alignment of the buttons in the Objects window.")
NORMAL (U"##5.3.34# (21 November 2012)")
LIST_ITEM (U"• Windows: corrected a bug that caused incorrect alignment of the buttons in the Objects window.")
LIST_ITEM (U"• The new ##Sound: Combine to stereo# can now work with Sounds of different durations, as the old could.")
LIST_ITEM (U"• Corrected a bug that caused Praat to crash when creating a SpeechSynthesizer.")
NORMAL (U"##5.3.33# (20 November 2012)")
LIST_ITEM (U"• ##Sound: Combine to stereo# can now combine any number of Sounds into a new Sound whose number of channels "
	"is the sum of the numbers of channels of the original Sounds.")
LIST_ITEM (U"• ERP: Down to Sound.")
NORMAL (U"##5.3.32# (17 October 2012)")
LIST_ITEM (U"• Sound window: corrected the working of the \"by window\" scaling option.")
NORMAL (U"##5.3.31# (10 October 2012)")
LIST_ITEM (U"• ERP: Down to Table...")
LIST_ITEM (U"• Linux: corrected a bug that could cause Praat to crash after closing a file selector window for saving.")
NORMAL (U"##5.3.30# (6 October 2012)")
LIST_ITEM (U"• Circumvented a rare Windows bug that could cause Praat to start to write 1.5 as 1,5 in some countries "
	"after opening a strange directory.")
LIST_ITEM (U"• Windows: corrected a bug introduced in 5.3.24 that could cause Praat to crash when quitting.")
LIST_ITEM (U"• Windows: corrected a bug introduced in 5.3.24 that could cause the Objects window to look weird after resizing.")
NORMAL (U"##5.3.29# (30 September 2012)")
LIST_ITEM (U"• @EEG: Draw scalp..., including gray legend.")
LIST_ITEM (U"• Made the new Sound scaling options available for LongSounds.")
NORMAL (U"##5.3.28# (28 September 2012)")
LIST_ITEM (U"• Corrected a bug introduced in 5.3.24 that made Praat crash on OSX 10.5.")
NORMAL (U"##5.3.27# (27 September 2012)")
LIST_ITEM (U"• Corrected a couple of small bugs.")
NORMAL (U"##5.3.26# (26 September 2012)")
LIST_ITEM (U"• Corrected a bug introduced in 5.3.24 that prevented Praat from running in batch mode.")
NORMAL (U"##5.3.25# (26 September 2012)")
LIST_ITEM (U"• EEG: view of scalp distribution takes scaling options into account.")
LIST_ITEM (U"• Linux: swipable Sound window and manual.")
LIST_ITEM (U"• Linux: corrected a bug introduced in 5.3.24 whereby TextGrids could not be edited.")
NORMAL (U"##5.3.24# (24 September 2012)")
LIST_ITEM (U"• More kinds of vertical scaling in e.g. Sound and EEG windows.")
LIST_ITEM (U"• Rewritten user interface (may contain some bugs).")
NORMAL (U"##5.3.23# (7 August 2012)")
LIST_ITEM (U"• Mac: removed a bug introduced in September 2011 that could cause incorrect behaviour of regular expressions since July 2012.")
NORMAL (U"##5.3.22# (21 July 2012)")
LIST_ITEM (U"• Linux: removed a bug introduced in 5.3.21 that could cause Praat to crash when opening files.")
LIST_ITEM (U"• Neural networks: correct writing and reading of learning settings.")
NORMAL (U"##5.3.21# (10 July 2012)")
LIST_ITEM (U"• Linux: better folder choice in file selector window for opening files.")
LIST_ITEM (U"• Repaired a bug that caused Praat to crash when opening a ManPages file on some platforms.")
NORMAL (U"##5.3.20# (5 July 2012)")
LIST_ITEM (U"• @EEG: ##Replace TextGrid#.")
NORMAL (U"##5.3.19# (24 June 2012)")
NORMAL (U"##5.3.18# (15 June 2012)")
LIST_ITEM (U"• Corrected a bug in @@Sound: Change gender...@ that caused a part of the sound not to be changed.")
NORMAL (U"##5.3.17# (12 June 2012)")
LIST_ITEM (U"• @EEG window: extent autoscaling by window.")
LIST_ITEM (U"• ERPTier: Remove events between...")
NORMAL (U"##5.3.16# (23 May 2012)")
NORMAL (U"##5.3.15# (10 May 2012)")
LIST_ITEM (U"• Improvements in clipboards, PDF and EPS files.")
NORMAL (U"##5.3.14# (28 April 2012)")
LIST_ITEM (U"• Linux: Tab shortcut for playing the selection.")
LIST_ITEM (U"• EPS files: higher minimum image resolution for spectrograms (300 instead of 106 dpi).")
NORMAL (U"##5.3.13# (11 April 2012)")
LIST_ITEM (U"• @EEG: Extract part...")
NORMAL (U"##5.3.12# (5 April 2012)")
LIST_ITEM (U"• Praat picture file: allow larger function drawings (up to 10^9 instead of 10^6 points).")
LIST_ITEM (U"• Linux: better audio compatibility with Ubuntu 11.10 (support for unusual sampling frequencies when playing LongSounds).")
NORMAL (U"##5.3.11# (27 March 2012)")
LIST_ITEM (U"• @EEG: a selected ERPTier can extract events on the basis of a column of a selected Table.")
NORMAL (U"##5.3.10# (12 March 2012)")
LIST_ITEM (U"• @EEG: Concatenate.")
NORMAL (U"##5.3.09# (10 March 2012)")
LIST_ITEM (U"• Better text-to-speech.")
NORMAL (U"##5.3.08# (5 March 2012)")
LIST_ITEM (U"• Removed a bug introduced in 5.3.07 that could cause Praat to crash when viewing a LongSound.")
NORMAL (U"##5.3.07# (4 March 2012)")
LIST_ITEM (U"• Praat can now save Sounds as 24-bit and 32-bit WAV files.")
NORMAL (U"##5.3.06# (28 February 2012)")
NORMAL (U"##5.3.05# (19 February 2012)")
LIST_ITEM (U"• SpeechSynthesizer (#New menu \\-> #Sound): text-to-speech.")
LIST_ITEM (U"• @EEG: better scalp distribution drawing for both 32-channel and 64-channel recordings.")
NORMAL (U"##5.3.04# (12 January 2012)")
LIST_ITEM (U"• EEG: draw scalp distribution.")
LIST_ITEM (U"• Linux: better audio compatibility with Ubuntu 11.10 (support for unusual sampling frequencies when playing Sounds).")
NORMAL (U"##5.3.03# (21 November 2011)")
LIST_ITEM (U"• EEG: filtering, editing, more viewing.")
NORMAL (U"##5.3.02# (7 November 2011)")
LIST_ITEM (U"• Corrected a bug introduced in 5.3.01 that could cause the PitchTier or PointProcess window to crash.")
LIST_ITEM (U"• Corrected a bug that could cause the Info window to freeze after an error message in a script window.")
NORMAL (U"##5.3.01# (1 November 2011)")
LIST_ITEM (U"• Macintosh and Windows: better window positioning if the Dock or Task Bar is on the left or right.")
LIST_ITEM (U"• IPA symbol: you can now use \\bs.f for the half-length sign (\\.f).")
LIST_ITEM (U"• EEG window.")
MAN_END

MAN_BEGIN (U"What was new in 5.3?", U"ppgb", 20111015)
NORMAL (U"##5.3# (15 October 2011)")
NORMAL (U"##5.2.46# (7 October 2011)")
LIST_ITEM (U"• Corrected the same very old bug as in 5.2.44, but now also for opening and saving files.")
LIST_ITEM (U"• Many better messages.")
NORMAL (U"##5.2.45# (29 September 2011)")
LIST_ITEM (U"• Spectrum window: ##Move cursor to nearest peak#.")
LIST_ITEM (U"• Table: ##Save as comma-separated file...#.")
LIST_ITEM (U"• Windows: you can now change the font size of the script window.")
LIST_ITEM (U"• Windows scripting: the Info window now updates while you are writing to it.")
LIST_ITEM (U"• Windows: error messages now stay at the front so that you never have to click away "
	"an invisible message window anymore.")
NORMAL (U"##5.2.44# (23 September 2011)")
LIST_ITEM (U"• Corrected a very old bug in the history mechanism (the button title wouldn't always show up).")
NORMAL (U"##5.2.43# (21 September 2011)")
LIST_ITEM (U"• Linux: you can now change the font size of the script window.")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash when opening an already open file in the script window.")
NORMAL (U"##5.2.42# (18 September 2011)")
LIST_ITEM (U"• Corrected a bug introduced in 5.2.36 that prevented formulas from working on FormantGrid objects.")
NORMAL (U"##5.2.41# (17 September 2011)")
LIST_ITEM (U"• Improved dashed-dotted lines in EPS files.")
LIST_ITEM (U"• Corrected a bug introduced in 5.2.36 that caused Praat to crash when running an ExperimentMFC.")
NORMAL (U"##5.2.40# (11 September 2011)")
LIST_ITEM (U"• Corrected a bug in the 64-bit Windows edition that led to an incorrect location for the preferences files.")
NORMAL (U"##5.2.39# (10 September 2011)")
LIST_ITEM (U"• 64-bit edition for Windows.")
LIST_ITEM (U"• Corrected a bug that caused Praat to crash instead of saying \"undefined\" when reporting the shimmer of a sound with fewer than 3 pulses.")
NORMAL (U"##5.2.38# (6 September 2011)")
LIST_ITEM (U"• Corrected several bugs that were introduced in 5.2.36 in the Inspect window.")
NORMAL (U"##5.2.37# (2 September 2011)")
LIST_ITEM (U"• Graphics: added the Chinese phonetic symbols \\id and \\ir. See @@Phonetic symbols: vowels@.")
LIST_ITEM (U"• Corrected a bug introduced in 5.2.36 that caused Praat to crash when querying formant quantiles.")
NORMAL (U"##5.2.36# (30 August 2011)")
LIST_ITEM (U"• Graphics: added superscript diacritics as single symbols: \\^h (\\bs\\^ h), \\^j (\\bs\\^ j), "
	"and many more; also \\_u (\\bs\\_ u). See @@Phonetic symbols: diacritics@.")
LIST_ITEM (U"• Praat fully converted to C++ (this may initially cause some bugs, but will be more reliable in the end).")
NORMAL (U"##5.2.35# (5 August 2011)")
LIST_ITEM (U"• Corrected the menus of the Sound and TextGrid windows.")
NORMAL (U"##5.2.34# (3 August 2011)")
LIST_ITEM (U"• @@Insert picture from file...@ now works on Windows (as well as on the Mac).")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash when playing a LongSound.")
NORMAL (U"##5.2.33# (29 July 2011)")
LIST_ITEM (U"• Improved the reliability of sound playing and several other things.")
LIST_ITEM (U"• Improved several error messages.")
NORMAL (U"##5.2.32# (22 July 2011)")
LIST_ITEM (U"• Corrected several problems with error messages.")
LIST_ITEM (U"• Corrected a bug that could cause slow execution of scripts with forms.")
NORMAL (U"##5.2.31# (21 July 2011)")
LIST_ITEM (U"• Corrected a bug that caused an incorrect window size in the VowelEditor.")
LIST_ITEM (U"• Corrected a bug that caused incorrect error messages when opening a FLAC file as a LongSound.")
LIST_ITEM (U"• Sound window: corrected a bug that could cause Praat to crash when zooming in on the left or right edge.")
NORMAL (U"##5.2.30# (18 July 2011)")
LIST_ITEM (U"• Corrected a bug introduced in 5.2.29 whereby the list of object actions could stay empty after an error message in a script.")
LIST_ITEM (U"• Corrected a bug in Klatt synthesis whereby the generation of a sound could be refused.")
NORMAL (U"##5.2.29# (12 July 2011)")
LIST_ITEM (U"• More accurate error checking, due to Praat's conversion to C++ (last C-only version was 5.2.17).")
NORMAL (U"##5.2.28# (28 June 2011)")
LIST_ITEM (U"• Corrected some Polygon bugs.")
NORMAL (U"##5.2.27# (19 June 2011)")
LIST_ITEM (U"• @Polygon: ##Draw (closed)...#.")
LIST_ITEM (U"• @@PointProcess: To Sound (phonation)...@: "
	"corrected a bug that could cause Praat to crash if pulses lay outside the time domain.")
NORMAL (U"##5.2.26# (24 May 2011)")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash on some platforms when reading a Praat binary file.")
LIST_ITEM (U"• @ExperimentMFC: corrected a bug that caused Praat to crash if an experiment contained zero trials.")
LIST_ITEM (U"• Corrected a bug that caused Praat to crash when merging multiple IntervalTiers.")
NORMAL (U"##5.2.25# (11 May 2011)")
LIST_ITEM (U"• OT learning: corrected a crashing bug from ##Get candidate...#.")
NORMAL (U"##5.2.24# (10 May 2011)")
LIST_ITEM (U"• Ability to open WAV files that contain incorrect information about the number of samples.")
LIST_ITEM (U"• Removed an old bug that could cause Praat to fail to read a chronological TextGrid text file.")
NORMAL (U"##5.2.23# (1 May 2011)")
LIST_ITEM (U"• Removed a bug introduced recently that could cause Praat to crash when working with derivatives of TableOfReal (such as Distributions).")
NORMAL (U"##5.2.22# (14 April 2011)")
LIST_ITEM (U"• @ExperimentMFC: reaction times for key presses.")
LIST_ITEM (U"• Linux: more reliable start-up on German systems.")
NORMAL (U"##5.2.21# (29 March 2011)")
LIST_ITEM (U"• Scripting: removed a crashing bug introduced for colour names in 5.2.20.")
NORMAL (U"##5.2.20# (25 March 2011)")
LIST_ITEM (U"• Scripting: removed a large memory leak for indexed variables.")
LIST_ITEM (U"• Scripting: removed a small memory leak for colour names.")
LIST_ITEM (U"• Support for very long file paths on the Mac.")
NORMAL (U"##5.2.19# (16 March 2011)")
LIST_ITEM (U"• @ExperimentMFC: corrected a bug introduced in 5.2.18 that could cause Praat to crash when extracting results from an incomplete experiment.")
NORMAL (U"##5.2.18# (9 March 2011)")
LIST_ITEM (U"• @ExperimentMFC: incomplete experiments can nevertheless output their incomplete results.")
NORMAL (U"##5.2.17# (2 March 2011)")
LIST_ITEM (U"• Better names for Table opening and saving commands.")
LIST_ITEM (U"• @ExperimentMFC: reaction times for mouse clicks.")
LIST_ITEM (U"• Linux/GTK: corrected triple clicks in ExperimentMFC.")
NORMAL (U"##5.2.16# (20 February 2011)")
LIST_ITEM (U"• Better support for WAV files with special \"chunks\" in them.")
LIST_ITEM (U"• Manual: documentation of explicit formulas for @@Voice 2. Jitter|jitter measurements@.")
NORMAL (U"##5.2.15# (11 February 2011)")
LIST_ITEM (U"• @@Sounds: Concatenate with overlap...@.")
NORMAL (U"##5.2.14# (8 February 2011)")
LIST_ITEM (U"• Repaired crashing bug in Manipulation window introduced in 5.2.13.")
NORMAL (U"##5.2.13# (7 February 2011)")
LIST_ITEM (U"• Renamed #Write commands to #Save commands.")
LIST_ITEM (U"• Scripting: allow pause forms without #Stop button (see @@Scripting 6.6. Controlling the user@).")
LIST_ITEM (U"• GTK: correct behaviour of default buttons.")
NORMAL (U"##5.2.12# (28 January 2011)")
LIST_ITEM (U"• Renamed #Edit buttons to ##View & Edit#.")
LIST_ITEM (U"• Better visibility of dragged things on Linux.")
NORMAL (U"##5.2.11# (18 January 2011)")
LIST_ITEM (U"• Better visibility of dragged things (sound selection, tier points, TextGrid boundaries) on Windows and Linux.")
NORMAL (U"##5.2.10# (11 January 2011)")
LIST_ITEM (U"• Renamed #Read and #Write menus to #Open and #Save.")
LIST_ITEM (U"• Sound: use of ##Formula (part)...# can speed up formulas appreciably.")
NORMAL (U"##5.2.09# (9 January 2011)")
LIST_ITEM (U"• Much improved scripting tutorial.")
LIST_ITEM (U"• Listening experiments can now show pictures instead of just texts (on the Mac).")
LIST_ITEM (U"• EPS files can now be many miles wide instead of just 55 inches.")
NORMAL (U"##5.2.08# (1 January 2011)")
LIST_ITEM (U"• Improved manual.")
LIST_ITEM (U"• Improved memory allocation on 32-bit platforms: "
	"less probability of crashes when you approach the 2 GB memory limit gradually, and a new \"low on memory; save your work now\" warning.")
LIST_ITEM (U"• Removed IntervalTier and TextTier datatypes from Praat (old scripts that use them will continue to work).")
NORMAL (U"##5.2.07# (24 December 2010)")
LIST_ITEM (U"• Support for reading and writing multi-channel sound files (i.e. above two-channel stereo).")
NORMAL (U"##5.2.06# (18 December 2010)")
LIST_ITEM (U"• Picture window: a new (the fourth) line type, namely dashed-dotted.")
LIST_ITEM (U"• Support for analysing and drawing multi-channel sounds (i.e. above two-channel stereo).")
LIST_ITEM (U"• Can read some EEG files (BioSemi 24-bit BDF) as a Sound and a TextGrid object.")
LIST_ITEM (U"• Linux: compatibility with computers without English language support.")
LIST_ITEM (U"• Macintosh: support for high-plane (i.e. very uncommon) Unicode characters in file names (as already existed on Unix and Windows).")
NORMAL (U"##5.2.05# (4 December 2010)")
LIST_ITEM (U"• Regular expressions: better Unicode support.")
LIST_ITEM (U"• Scripting window: command ##Convert to C string#.")
NORMAL (U"##5.2.04# (27 November 2010)")
LIST_ITEM (U"• Scripting: allow directory (folder) selector windows; see @@Scripting 6.6. Controlling the user@.")
NORMAL (U"##5.2.03# (19 November 2010)")
LIST_ITEM (U"• Scripting: support for string arrays.")
NORMAL (U"##5.2.02# (17 November 2010)")
LIST_ITEM (U"• TextGrid window: corrected a bug that caused Praat to crash (instead of doing nothing) when you tried to add boundaries "
	"if the selection ran from the penultimate interval to the end of the TextGrid.")
LIST_ITEM (U"• Scripting: support for arrays with multiple indexes.")
LIST_ITEM (U"• Linux: made spectrogram drawing compatible with Ubuntu 10.10.")
LIST_ITEM (U"• Linux: made sound more easily available on Ubuntu 10.10.")
NORMAL (U"##5.2.01# (4 November 2010)")
LIST_ITEM (U"• Scripting: support for numeric @@Scripting 5.6. Arrays and dictionaries|arrays@.")
MAN_END

MAN_BEGIN (U"What was new in 5.2?", U"ppgb", 20101029)
NORMAL (U"##5.2# (29 October 2010)")
NORMAL (U"##5.1.45# (26 October 2010)")
LIST_ITEM (U"• Linux/GTK: allow Praat to run without an X display.")
LIST_ITEM (U"• Sounds are played synchronously in scripts run from ManPages with \\bsSC.")
NORMAL (U"##5.1.44# (4 October 2010)")
LIST_ITEM (U"• Linux/GTK: visibility of ExperimentMFC window.")
LIST_ITEM (U"• Linux/GTK: keyboard shortcuts.")
NORMAL (U"##5.1.43# (4 August 2010)")
LIST_ITEM (U"• Scripting: support for stand-alone programs; see @@Scripting 9.1. Turning a script into a stand-alone program@.")
LIST_ITEM (U"• Table: allow drawing of ellipses even if irrelevant columns contain undefined data.")
LIST_ITEM (U"• Linux/GTK: correct resizing of Table window.")
LIST_ITEM (U"• Linux/GTK: prevented multiple storing of Picture window selection in scripting history.")
NORMAL (U"##5.1.42# (26 July 2010)")
LIST_ITEM (U"• Scripting: allow file selector windows; see @@Scripting 6.6. Controlling the user@.")
LIST_ITEM (U"• Linux: multiple file selection.")
NORMAL (U"##5.1.41# (15 July 2010)")
LIST_ITEM (U"• OTGrammar: ##Compare candidates...#")
LIST_ITEM (U"• GTK: support for the Doulos SIL and Charis SIL fonts.")
LIST_ITEM (U"• GTK: working vowel editor.")
LIST_ITEM (U"• Vowel editor: repaired memory leak.")
//LIST_ITEM (U"• GTK: Picture window: front when drawing.")
//LIST_ITEM (U"• GTK: correct scrolling in manuals.")
NORMAL (U"##5.1.40# (13 July 2010)")
LIST_ITEM (U"• GTK: working Demo window (drawing, clicks, keys).")
LIST_ITEM (U"• GTK: pause forms.")
LIST_ITEM (U"• GTK: manual pages: receive a white background, for compatibility with dark-background themes.")
LIST_ITEM (U"• GTK: in settings windows, better alignment of labels to radio groups.")
LIST_ITEM (U"• GTK: rotated text.")
NORMAL (U"##5.1.39# (10 July 2010)")
LIST_ITEM (U"• GTK beta version.")
LIST_ITEM (U"• Linux: made ##Sound: Record fixed time...# work correctly (the sampling frequency was wrong).")
LIST_ITEM (U"• GTK: list of objects: multiple selection, working Rename button.")
LIST_ITEM (U"• GTK: running a script no longer deselects all objects at the start.")
LIST_ITEM (U"• GTK: working Buttons editor.")
LIST_ITEM (U"• GTK: correctly laid out settings windows.")
NORMAL (U"##5.1.38# (2 July 2010)")
LIST_ITEM (U"• Linux: made ##Sound: Record fixed time...# work correctly (the sampling frequency was wrong).")
LIST_ITEM (U"• Mac: repaired a bug introduced in 5.1.35 that could cause Praat to crash if neither Doulos SIL nor Charis SIL were installed.")
LIST_ITEM (U"• Mac: correct live scrolling in Picture window and DataEditor.")
NORMAL (U"##5.1.37# (23 June 2010)")
LIST_ITEM (U"• PitchTier window and similar windows: can drag multiple points simultaneously.")
LIST_ITEM (U"• Table: %t-tests report the number of degrees of freedom.")
LIST_ITEM (U"• GTK: correct progress bar again.")
LIST_ITEM (U"• GTK: correct behaviour of Synth menu in manipulation window.")
NORMAL (U"##5.1.36# (18 June 2010)")
LIST_ITEM (U"• Mac: live scrolling.")
LIST_ITEM (U"• Demo window: the Demo window can now run from the command line.")
LIST_ITEM (U"• Motif: corrected a bug introduced in 5.1.33 whereby things in the Picture window could be placed incorrectly.")
LIST_ITEM (U"• GTK: script window accepts Unicode again.")
NORMAL (U"##5.1.35# (10 June 2010)")
LIST_ITEM (U"• TextGrid window: removed a very old bug that could lead to reversed intervals and to crashes when you inserted a boundary after using Shift-arrow and Command-arrow.")
LIST_ITEM (U"• Graphics: Praat now uses Doulos SIL instead of Charis SIL if your font setting is \"Times\" and the font is nonbold and nonitalic, "
	"because Doulos SIL matches Times New Roman better.")
LIST_ITEM (U"• kNN: made Ola Söder's k-nearest-neighbours classification compatible with Windows.")
NORMAL (U"##5.1.34# (31 May 2010)")
LIST_ITEM (U"• Sound window: corrected a bug that caused Praat to crash if the analysis window was shorter than 2 samples.")
LIST_ITEM (U"• GTK: scrolling in the Info window and script window.")
LIST_ITEM (U"• GTK: script editor: implemented searching, replacing, and change dialogs.")
NORMAL (U"##5.1.33# (24 May 2010)")
LIST_ITEM (U"• GTK alpha version.")
LIST_ITEM (U"• Abolished resolution independence: the Sound window now looks the same on all platforms, "
	"and the Demo window has the same relative font size on your screen and on the video projector.")
LIST_ITEM (U"• GTK: support for asynchronous audio output.")
LIST_ITEM (U"• GTK: sound plays once rather than three times in Sound and other windows.")
LIST_ITEM (U"• GTK: can click more than once in the manual.")
LIST_ITEM (U"• GTK: correct pink selections in Sound and other windows.")
LIST_ITEM (U"• GTK: correct dragging in TextGrid, Manipulation, and tier windows.")
LIST_ITEM (U"• GTK: a working TextGrid window.")
LIST_ITEM (U"• GTK: no automatic triple click in the manual (and many other windows).")
LIST_ITEM (U"• GTK: moving cursor while sound is playing in Sound and other windows.")
LIST_ITEM (U"• GTK: correct colours of the rectangles in Sound and other windows.")
LIST_ITEM (U"• GTK: a working Group button.")
LIST_ITEM (U"• GTK: correct font menus, font size menus, colour menus, line type menus.")
LIST_ITEM (U"• GTK: scrolling in the manual (and many other windows).")
LIST_ITEM (U"• GTK: erase old texts in manual windows.")
LIST_ITEM (U"• GTK: made Picture window come back when drawing.")
LIST_ITEM (U"• GTK: Info window is fronted automatically.")
LIST_ITEM (U"• GTK: support sendpraat.")
NORMAL (U"##5.1.32# (30 April 2010)")
LIST_ITEM (U"• Scripting: command ##Expand include files#.")
LIST_ITEM (U"• Scripting: accept lines that start with non-breaking spaces (as may occur in scripts copied from the web manual).")
LIST_ITEM (U"• Sound files: accept MP3 files with extension written in capitals.")
LIST_ITEM (U"• Linux audio recording: corrected input choice (microphone, line).")
NORMAL (U"##5.1.31# (4 April 2010)")
LIST_ITEM (U"• @@Sounds: Convolve...@, @@Sounds: Cross-correlate...@, @@Sound: Autocorrelate...@, with full documentation.")
LIST_ITEM (U"• More query commands for IntensityTier.")
NORMAL (U"##5.1.30# (25 March 2010)")
LIST_ITEM (U"• Scripting: $$createDirectory ()$ can now work with absolute paths.")
LIST_ITEM (U"• PointProcess: made it impossible to add a point where there is already a point.")
NORMAL (U"##5.1.29# (11 March 2010)")
LIST_ITEM (U"• Full support for unicode values above 0xFFFF on Macintosh.")
NORMAL (U"##5.1.28# (10 March 2010)")
LIST_ITEM (U"• TextGrid window: removed a recently introduced bug that could cause Praat to crash when inserting a boundary.")
NORMAL (U"##5.1.27# (7 March 2010)")
LIST_ITEM (U"• Table: Wilcoxon rank sum test.")
LIST_ITEM (U"• Logistic regression: corrected a bug by which a boundary would sometimes not be drawn.")
NORMAL (U"##5.1.26# (25 February 2010)")
LIST_ITEM (U"• Experimental GTK version with the help of Franz Brauße.")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash if sound playing failed twice.")
NORMAL (U"##5.1.25# (20 January 2010)")
LIST_ITEM (U"• Script window: the new command ##Reopen from disk# allows you to edit the script with an external editor.")
LIST_ITEM (U"• Script window: removed a bug that could cause Praat to crash of you did ##Find again# before having done any #Find.")
NORMAL (U"##5.1.24# (15 January 2010)")
LIST_ITEM (U"• Formulas run 10 to 20 percent faster.")
LIST_ITEM (U"• Macintosh: support for forward delete key and Command-`.")
NORMAL (U"##5.1.23# (1 January 2010)")
LIST_ITEM (U"• Allowed multiple files to be selected with e.g. @@Read from file...@.")
LIST_ITEM (U"• Demo window: guarded against handling the Demo window from two scripts at a time.")
NORMAL (U"##5.1.22# (15 December 2009)")
LIST_ITEM (U"• Picture window: millions of @@colour|colours@ instead of just twelve.")
LIST_ITEM (U"• Sound window: ##Move frequency cursor to...#")
NORMAL (U"##5.1.21# (30 November 2009)")
LIST_ITEM (U"• @@Sound: Draw where...@")
LIST_ITEM (U"• ##Matrix: Draw contours...# and ##LogisticRegression: Draw boundary...# support reversed axes.")
LIST_ITEM (U"• Sound window: ##Move frequency cursor to...#.")
NORMAL (U"##5.1.20# (26 October 2009)")
LIST_ITEM (U"• Editor windows: repaired the Synchronized Zoom And Scroll preference.")
NORMAL (U"##5.1.19# (21 October 2009)")
LIST_ITEM (U"• Table: Randomize rows")
LIST_ITEM (U"• Tables: Append (vertically)")
LIST_ITEM (U"• Scripting: corrected a bug that could cause Praat to crash if the name of a field in a form contained a colon.")
LIST_ITEM (U"• Windows: corrected arc drawing.")
NORMAL (U"##5.1.18# (9 October 2009)")
LIST_ITEM (U"• The @@Demo window@ is less often automatically moved to the front "
	"(in order to allow it to pop up other editor windows).")
LIST_ITEM (U"• @@DTW & TextGrid: To TextGrid (warp times)@: corrected a bug that could "
	"lead to an incorrect end time of the last interval in new IntervalTiers.")
NORMAL (U"##5.1.17# (22 September 2009)")
LIST_ITEM (U"• Made more stereo movies readable.")
LIST_ITEM (U"• Editor windows now have a ##Zoom Back# button.")
NORMAL (U"##5.1.16# (17 September 2009)")
LIST_ITEM (U"• Macintosh: corrected a bug that caused incorrect phonetic symbols if Charis SIL was available but SIL Doulos IPA93 was not.")
NORMAL (U"##5.1.15# (30 August 2009)")
LIST_ITEM (U"• Corrected a bug in @@Sound: Change gender...@ introduced in 5.1.14.")
NORMAL (U"##5.1.14# (27 August 2009)")
LIST_ITEM (U"• Windows: corrected a bug introduced in 5.1.13 that caused Praat to crash during tab navigation.")
LIST_ITEM (U"• Made @@Sound: Change gender...@ compatible with elephant calls (i.e. very low F0).")
NORMAL (U"##5.1.13# (21 August 2009)")
LIST_ITEM (U"• Script window: #Find and #Replace.")
LIST_ITEM (U"• Picture window (and therefore Demo window!): @@Insert picture from file...@ (MacOS 10.4 and up).")
LIST_ITEM (U"• @@Demo window@: full screen (on the Mac).")
LIST_ITEM (U"• Scripting: faster object selection (scripts no longer slow down when there are many objects in the list).")
LIST_ITEM (U"• Scripting: $$variableExists$.")
LIST_ITEM (U"• Macintosh: PDF clipboard (MacOS 10.4 and up).")
NORMAL (U"##5.1.12# (4 August 2009)")
LIST_ITEM (U"• Macintosh: the Picture window can save to PDF file (you need MacOS 10.4 or up).")
LIST_ITEM (U"• Macintosh: corrected a bug that caused Praat to crash at start-up on MacOS 10.3.")
NORMAL (U"##5.1.11# (19 July 2009)")
NORMAL (U"##5.1.10# (8 July 2009)")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash if the Demo window was closed after an \"execute\".")
LIST_ITEM (U"• OTGrammar & PairDistribution: added ##Get minimum number correct...#.")
NORMAL (U"##5.1.09# (28 June 2009)")
LIST_ITEM (U"• Made East-European Roman characters available in EPS files.")
NORMAL (U"##5.1.08# (21 June 2009)")
LIST_ITEM (U"• Removed a bug introduced in 5.1.07 that could cause strange pictures in manual.")
LIST_ITEM (U"• Macintosh: execute @sendpraat messages immediately instead of waiting for the user to click the jumping Praat icon.")
NORMAL (U"##5.1.07# (12 May 2009)")
LIST_ITEM (U"• Demo window: navigation by arrow keys also on Windows.")
LIST_ITEM (U"• Demo window: no longer crashes on Linux.")
NORMAL (U"##5.1.06# (11 May 2009)")
LIST_ITEM (U"• Demo window.")
NORMAL (U"##5.1.05# (7 May 2009)")
LIST_ITEM (U"• KlattGrid update.")
NORMAL (U"##5.1.04# (4 April 2009)")
LIST_ITEM (U"• Corrected a bug that could cause a missing text character in EPS files produced by a version of Praat running in batch.")
LIST_ITEM (U"• Corrected a bug that could cause high values in a Matrix computed from a Pitch.")
NORMAL (U"##5.1.03# (21 March 2009)")
//LIST_ITEM (U"• OT learning: the \"Random up, highest down\" update rule.")
LIST_ITEM (U"• ExperimentMFC: corrected a bug introduced in 5.0.36 that caused Praat to crash if a sound file did not exist.")
LIST_ITEM (U"• Articulatory synthesis: corrected a bug that could cause Praat to crash when copying Artwords.")
LIST_ITEM (U"• Macintosh: corrected a bug that could cause poor text alignment in picture.")
NORMAL (U"##5.1.02# (9 March 2009)")
LIST_ITEM (U"• Allow pause forms without fields.")
LIST_ITEM (U"• The value \"undefined\" is disallowed from all fields in command windows "
	"except ##Table: Set numeric value...# and ##TableOfReal: Set value...#.")
LIST_ITEM (U"• TextGrid: ##List...# and ##Down to Table...#.")
LIST_ITEM (U"• OT learning: Giorgio Magri's \"Weighted all up, highest down\" update rule.")
NORMAL (U"##5.1.01# (26 February 2009)")
LIST_ITEM (U"• Corrected several bugs in Klatt synthesis.")
MAN_END

MAN_BEGIN (U"What was new in 5.1?", U"ppgb", 20090131)
NORMAL (U"##5.1# (31 January 2009)")
LIST_ITEM (U"• Editors for Klatt synthesis.")
LIST_ITEM (U"• Corrected many bugs.")
NORMAL (U"##5.0.47# (21 January 2009)")
LIST_ITEM (U"• Extensive pause windows: @@Scripting 6.6. Controlling the user@.")
NORMAL (U"##5.0.46# (7 January 2009)")
LIST_ITEM (U"• More Klatt synthesizer.")
LIST_ITEM (U"• First pause form support.")
LIST_ITEM (U"• Renewed CategoriesEditor.")
LIST_ITEM (U"• Repaired several memory leaks.")
NORMAL (U"##5.0.45# (29 December 2008)")
LIST_ITEM (U"• Bug fixes in Klatt synthesizer.")
NORMAL (U"##5.0.44# (24 December 2008)")
LIST_ITEM (U"• David's Klatt synthesizer: @KlattGrid.")
NORMAL (U"##5.0.43# (9 December 2008)")
LIST_ITEM (U"• Scripting tutorial: local variables in procedures.")
NORMAL (U"##5.0.42# (26 November 2008)")
LIST_ITEM (U"• Removed a bug that could cause Praat to crash when drawing pictures in the manual window.")
LIST_ITEM (U"• Removed a bug that could cause Praat to crash when drawing multi-line text.")
NORMAL (U"##5.0.41# (23 November 2008)")
LIST_ITEM (U"• ExperimentMFC: allow multiple lines in all texts.")
LIST_ITEM (U"• Regular expressions: removed a bug that could cause Praat to hang when using \".*\".")
LIST_ITEM (U"• Table: Draw ellipses: removed a bug that could cause Praat to crash if some cells were undefined.")
NORMAL (U"##5.0.40# (10 November 2008)")
LIST_ITEM (U"• Improved reading and writing of text files (faster, bigger).")
NORMAL (U"##5.0.39# (1 November 2008)")
LIST_ITEM (U"• praatcon -a (for sending ANSI encoding when redirected)")
NORMAL (U"##5.0.38# (28 October 2008)")
LIST_ITEM (U"• @FormantGrid: To Formant...")
NORMAL (U"##5.0.36# (20 October 2008)")
LIST_ITEM (U"• @ExperimentMFC: accepts nonstandard sound files.")
NORMAL (U"##5.0.35# (5 October 2008)")
LIST_ITEM (U"• ##Scale times by...# and ##Scale times to...# (Modify menu).")
NORMAL (U"##5.0.34# (22 September 2008)")
LIST_ITEM (U"• ##Shift times by...# and ##Shift times to...# (Modify menu).")
LIST_ITEM (U"• Sound: @@Combine to stereo@ works even if the two mono sounds have different durations or time domains.")
NORMAL (U"##5.0.33# (9 September 2008)")
LIST_ITEM (U"• Windows: prevented warning messages about \"Widget type\".")
NORMAL (U"##5.0.32# (12 August 2008)")
LIST_ITEM (U"• Contributed by Ola Söder: @@kNN classifiers@ and @@k-means clustering@.")
LIST_ITEM (U"• Made UTF-16-encoded chronological TextGrid files readable.")
NORMAL (U"##5.0.31# (6 August 2008)")
LIST_ITEM (U"• Macintosh: corrected a bug introduced in 5.0.30 that caused Praat to crash when you pressed the Tab key "
	"in a window without text fields.")
NORMAL (U"##5.0.30# (22 July 2008)")
LIST_ITEM (U"• Macintosh and Windows: tab navigation.")
NORMAL (U"##5.0.29# (8 July 2008)")
LIST_ITEM (U"• OTMulti: can record history.")
LIST_ITEM (U"• Picture window: corrected text in Praat picture files.")
NORMAL (U"##5.0.28# (3 July 2008)")
LIST_ITEM (U"• Windows: audio output uses DirectX (next to Multi-Media Extensions).")
NORMAL (U"##5.0.27# (28 June 2008)")
LIST_ITEM (U"• @@Phonetic symbols@: breve (a\\N^).")
LIST_ITEM (U"• Annotation: improved some SpellingChecker commands.")
LIST_ITEM (U"• Table: can now set string values that contain spaces.")
NORMAL (U"##5.0.26# (15 June 2008)")
LIST_ITEM (U"• Windows: sound recording no longer makes your laptop's fan spin.")
LIST_ITEM (U"• Windows: no longer any 64 MB limit on recorded sounds.")
LIST_ITEM (U"• Linux: audio input and output uses Alsa (next to OSS).")
NORMAL (U"##5.0.25# (31 May 2008)")
LIST_ITEM (U"• OT learning: added decision strategy ExponentialMaximumEntropy.")
NORMAL (U"##5.0.24# (14 May 2008)")
LIST_ITEM (U"• Linux: corrected a bug at start-up.")
NORMAL (U"##5.0.23# (9 May 2008)")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash when you edited an Artword that you had read from a file.")
NORMAL (U"##5.0.22# (26 April 2008)")
LIST_ITEM (U"• Editing formant contours: @FormantGrid.")
NORMAL (U"##5.0.21# (22 April 2008)")
LIST_ITEM (U"• Annotating with Cyrillic, Arabic, Chinese, Korean characters, and many more (on Macintosh and Windows).")
NORMAL (U"##5.0.20# (8 April 2008)")
LIST_ITEM (U"• @ExperimentMFC: prevented the OK key from working if no response (and goodness choice) had been made.")
LIST_ITEM (U"• OT learning: sped up learning from partial outputs by a factor of five or more.")
NORMAL (U"##5.0.19# (4 April 2008)")
LIST_ITEM (U"• TextGrid window: corrected a bug introduced in 5.0.17 that could cause Praat to crash when handling point tiers.")
NORMAL (U"##5.0.18# (31 March 2008)")
LIST_ITEM (U"• Manipulation window: corrected a bug introduced in 5.0.17 that caused Praat to crash when adding the first duration point.")
LIST_ITEM (U"• Sound: added ##Extract all channels#.")
LIST_ITEM (U"• OT learning: added @@OTGrammar & PairDistribution: Find positive weights...@.")
LIST_ITEM (U"• ExperimentMFC: corrected a bug that caused Praat to crash when the second of multiple experiments "
	"referred to non-existing sound files.")
NORMAL (U"##5.0.17# (29 March 2008)")
LIST_ITEM (U"• Sped up vowel editor by a large factor.")
LIST_ITEM (U"• OT learning: corrected Exponential HG update rule from OT-GLA to HG-GLA.")
LIST_ITEM (U"• OT learning: shift Exponential HG average constraint weight to zero after every learning step.")
NORMAL (U"##5.0.16# (25 March 2008)")
LIST_ITEM (U"• Macintosh: returned to old compiler because of incompatibility with MacOS X 10.4.")
NORMAL (U"##5.0.15# (21 March 2008)")
LIST_ITEM (U"• Windows: more reliable dropping of files on the Praat icon when Praat is already running.")
NORMAL (U"##5.0.14# (20 March 2008)")
LIST_ITEM (U"• David's vowel editor (New \\-> Sound).")
LIST_ITEM (U"• Formulas: corrected scaling of sinc function.")
NORMAL (U"##5.0.13# (18 March 2008)")
LIST_ITEM (U"• Corrected drawing of histograms.")
LIST_ITEM (U"• TextGrid window: selected part of the TextGrid can be extracted and saved.")
LIST_ITEM (U"• TextGrid: more complete conversion between backslash trigraphs and Unicode.")
LIST_ITEM (U"• Windows: more reliable dropping of files on the Praat icon when Praat is not running yet.")
LIST_ITEM (U"• Formulas: sinc function.")
NORMAL (U"##5.0.12# (12 March 2008)")
LIST_ITEM (U"• Bigger ligature symbol (k\\lip).")
NORMAL (U"##5.0.11# (7 March 2008)")
LIST_ITEM (U"• Corrected saving of new binary Manipulation files (you can send any unreadable Manipulation files to Paul Boersma for correction).")
NORMAL (U"##5.0.10# (27 February 2008)")
LIST_ITEM (U"• Added the characters \\d- and \\D-.")
LIST_ITEM (U"• Windows: made ##praatcon.exe# compatible with Unicode command lines.")
NORMAL (U"##5.0.09# (16 February 2008)")
LIST_ITEM (U"• Windows: corrected a bug by which Praat would not open files that were dragged on the Praat icon "
	"if the names of these files or their directory paths contained non-ASCII characters.")
LIST_ITEM (U"• Linux: ignore the Mod2 key, because of its unpredictable assignment.")
NORMAL (U"##5.0.08# (10 February 2008)")
LIST_ITEM (U"• Corrected the minus sign (\\bs-m = \"\\-m\").")
NORMAL (U"##5.0.07# (8 February 2008)")
LIST_ITEM (U"• Object IDs are visible in editor windows.")
NORMAL (U"##5.0.06# (31 January 2008)")
LIST_ITEM (U"• Corrected a bug that caused ##Find again# in the TextGrid window not to work.")
LIST_ITEM (U"• Macintosh: made Praat work correctly on 10.3 computers with missing fonts.")
NORMAL (U"##5.0.05# (19 January 2008)")
LIST_ITEM (U"• All Matrix, Sound, Spectrogram, Pitch, Ltas, and Spectrum objects (and more) are now in 52-bit relative precision (instead of the earlier 23 bits).")
LIST_ITEM (U"• Corrected a bug that could lead to \"Unknown opcode (0)\" messages when drawing large sounds (more than 16 million samples) in the Picture window.")
LIST_ITEM (U"• Macintosh: solved around a bug in the C library that could cause incorrect representation of non-ASCII characters (in the OTGrammar window).")
NORMAL (U"##5.0.04# (12 January 2008)")
LIST_ITEM (U"• Windows: corrected a bug introduced in 5.0.03 that caused Praat to crash if you pressed a key in the Sound window.")
LIST_ITEM (U"• Macintosh: some cosmetic corrections.")
NORMAL (U"##5.0.03# (9 January 2008)")
LIST_ITEM (U"• Scripting: guard against opening the same file more than once.")
LIST_ITEM (U"• Table: possibility of regarding a column as a distribution (#Generate menu).")
LIST_ITEM (U"• Macintosh: corrected line colours.")
NORMAL (U"##5.0.02# (27 December 2007)")
LIST_ITEM (U"• TextGrid window: corrected the drawing of numbers to the right of the tiers.")
LIST_ITEM (U"• Corrected a bug that caused Praat to crash when doing ##SpectrumTier: List#.")
NORMAL (U"##5.0.01# (18 December 2007)")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash when redrawing the sound or TextGrid window.")
MAN_END

MAN_BEGIN (U"What was new in 5.0?", U"ppgb", 20071210)
NORMAL (U"##5.0# (10 December 2007)")
LIST_ITEM (U"• Corrected many bugs.")
LIST_ITEM (U"• Display font sizes in points rather than pixels.")
NORMAL (U"##4.6.41# (9 December 2007)")
LIST_ITEM (U"• Windows: corrected a bug that could cause listening experiments not to run when the directory path included non-ASCII characters; "
	"the same bug could (under comparable circumstances) cause scripted menu commands not to work.")
LIST_ITEM (U"• Corrected a bug that could cause null bytes in data files when the text output encoding preference was ##try ISO Latin-1, then UTF-16#.")
NORMAL (U"##4.6.40# (3 December 2007)")
LIST_ITEM (U"• Corrected some minor bugs.")
NORMAL (U"##4.6.39# (1 December 2007)")
LIST_ITEM (U"• Manual: corrected a bug that could cause Praat to crash when viewing certain manual pages with pictures.")
LIST_ITEM (U"• Scripting: corrected a bug that could cause Praat to crash when a long string was used as an argument to a procedure.")
NORMAL (U"##4.6.38# (19 November 2007)")
LIST_ITEM (U"• More extensive @@logistic regression@.")
NORMAL (U"##4.6.37# (15 November 2007)")
LIST_ITEM (U"• Object list shows numbers.")
LIST_ITEM (U"• Macintosh: corrected saving of non-ASCII text settings.")
NORMAL (U"##4.6.36# (2 November 2007)")
LIST_ITEM (U"• Sound and TextGrid windows: direct drawing of intensity, formants, pulses, and TextGrid.")
LIST_ITEM (U"• Regular expressions: corrected handling of newlines on Windows.")
LIST_ITEM (U"• Scripting: improved positioning of settings windows for script commands in editors on Windows.")
NORMAL (U"##4.6.35# (22 October 2007)")
LIST_ITEM (U"• Windows and Linux: better positioning of form windows of editor scripts.")
LIST_ITEM (U"• Macintosh: OTMulti learning window more compatible with non-ASCII characters.")
NORMAL (U"##4.6.34# (18 October 2007)")
LIST_ITEM (U"• Corrected a bug introduced in September that could cause Praat to crash when starting up "
	"if the user had explicitly made action commands visible or invisible in an earlier session.")
NORMAL (U"##4.6.33# (16 October 2007)")
LIST_ITEM (U"• Corrected a bug introduced in September that caused Praat to crash when a PointProcess window was opened without a Sound.")
LIST_ITEM (U"• Macintosh: objects with non-ASCII names show up correctly in the list.")
NORMAL (U"##4.6.32# (14 October 2007)")
LIST_ITEM (U"• Unicode support for names of objects.")
LIST_ITEM (U"• Linux: first Unicode support (in window titles).")
LIST_ITEM (U"• Windows scripting: corrected a bug that caused weird characters in Paste History.")
NORMAL (U"##4.6.31# (8 October 2007)")
LIST_ITEM (U"• TextGrid window: made Save command available again for TextGrid windows without a sound.")
LIST_ITEM (U"• Corrected a bug that caused binary Collection files with objects with names with non-ASCII characters to be unreadable.")
NORMAL (U"##4.6.30# (3 October 2007)")
LIST_ITEM (U"• OTMulti: added an evaluate command in the Objects window, so that paced learning becomes scriptable.")
LIST_ITEM (U"• Macintosh: worked around a feature of a system library that could cause Praat to crash when reading a Collection text file "
	"that contained objects with non-ASCII names.")
NORMAL (U"##4.6.29# (1 October 2007)")
LIST_ITEM (U"• OT learning: leak and constraint in OTMulti.")
LIST_ITEM (U"• Support for saving Table, TableOfReal, Strings, OTGrammar and OTMulti in Unicode.")
NORMAL (U"##4.6.28# (1 October 2007)")
LIST_ITEM (U"• OT learning: positive constraint satisfactions in OTMulti tableaus.")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash when reading a non-UTF-8 text file "
	"when the \"Text reading preference\" had been set to UTF-8.")
NORMAL (U"##4.6.27# (29 September 2007)")
LIST_ITEM (U"• Corrected redrawing of text with non-ASCII characters.")
NORMAL (U"##4.6.26# (29 September 2007)")
LIST_ITEM (U"• Corrected reading of high UTF-8 codes.")
NORMAL (U"##4.6.25# (26 September 2007)")
LIST_ITEM (U"• @ExperimentMFC: can set font size for response buttons.")
NORMAL (U"##4.6.24# (24 September 2007)")
LIST_ITEM (U"• Czech, Polish, Croatian, and Hungarian characters such as \\c< \\uo \\t< \\e; \\s' \\l/ \\c\' \\o: (see @@Special symbols@).")
LIST_ITEM (U"• Some support for Hebrew characters such as \\?+ \\B+ \\sU (see @@Special symbols@).")
NORMAL (U"##4.6.23# (22 September 2007)")
LIST_ITEM (U"• Corrected a bug introduced in 4.6.13 that caused crashes in text handling (mainly on Linux).")
LIST_ITEM (U"• Info commands in editors.")
NORMAL (U"##4.6.22# (17 September 2007)")
LIST_ITEM (U"• Phonetic symbols: added the nonsyllabicity diacritic (a\\nv).")
LIST_ITEM (U"• Macintosh: worked around a feature of a system library that could cause strange behaviour of forms in scripts with non-ASCII characters.")
NORMAL (U"##4.6.21# (5 September 2007)")
LIST_ITEM (U"• Sound and TextGrid windows: direct drawing of selected sound to the picture window.")
NORMAL (U"##4.6.20# (2 September 2007)")
LIST_ITEM (U"• Introduced direct drawing of spectrogram and pitch to the Praat picture window from Sound windows and TextGrid windows.")
LIST_ITEM (U"• Corrected a bug introduced in 4.6.13 by which Inspect did not show all data in a TextGrid.")
NORMAL (U"##4.6.19# (31 August 2007)")
LIST_ITEM (U"• Macintosh: worked around a bug in a system library that caused Praat to crash (since version 4.6.13) "
	"when removing a boundary from a TextGrid interval in MacOS X 10.3 or earlier.")
NORMAL (U"##4.6.18# (28 August 2007)")
LIST_ITEM (U"• Sound: Filter (formula): now works in the same way as the other filter commands "
	"(without adding an empty space at the end), and on stereo sounds.")
NORMAL (U"##4.6.17# (25 August 2007)")
LIST_ITEM (U"• Windows: improved rotated text, also for copy-paste and printing.")
LIST_ITEM (U"• Windows: phonetic characters on the screen now require the Charis SIL or Doulos SIL font.")
LIST_ITEM (U"• Picture settings report (mainly for script writers).")
LIST_ITEM (U"• Corrected a bug that could cause Praat to crash when closing a manual page that had been read from a file.")
NORMAL (U"##4.6.16# (22 August 2007)")
LIST_ITEM (U"• Macintosh: corrected a bug introduced in 4.6.13 that could cause Praat to crash when drawing a spectrogram in MacOS X 10.3 or earlier.")
NORMAL (U"##4.6.15# (21 August 2007)")
LIST_ITEM (U"• Corrected a bug introduced in 4.6.14 that prevented the use of Helvetica in the Picture window.")
LIST_ITEM (U"• Corrected a bug in \"Read Table from table file...\", introduced in 4.6.13.")
NORMAL (U"##4.6.14# (20 August 2007)")
LIST_ITEM (U"• Corrected a bug introduced in 4.6.13 that prevented any other font than Palatino in the Picture window.")
LIST_ITEM (U"• Macintosh: corrected height of subscripts and superscripts (broken in 4.6.13).")
NORMAL (U"##4.6.13# (16 August 2007)")
LIST_ITEM (U"• TextGrid: corrected reading of chronological files.")
LIST_ITEM (U"• Macintosh: text looks better (and rotated text is now readable on Intel Macs).")
LIST_ITEM (U"• Macintosh: phonetic characters on the screen now require the Charis SIL or Doulos SIL font.")
NORMAL (U"##4.6.12# (27 July 2007)")
LIST_ITEM (U"• OTGrammar bug fix: leak and constraint plasticity correctly written into OTGrammar text files.")
NORMAL (U"##4.6.11# (25 July 2007)")
LIST_ITEM (U"• OTGrammar: introduced %%constraint plasticity% for slowing down or halting the speed with which constraints are reranked.")
LIST_ITEM (U"• OTGrammar: introduced %%leak% for implementing forgetful learning of correlations.")
LIST_ITEM (U"• OTGrammar: positive constraint satisfactions are drawn as `+' in tableaus.")
NORMAL (U"##4.6.10# (22 July 2007)")
LIST_ITEM (U"• Improved reading of UTF-16 data files.")
LIST_ITEM (U"• Improved error messages when reading text files (line numbers are mentioned).")
LIST_ITEM (U"• Table: Get group mean (Student t)...")
NORMAL (U"##4.6.09# (24 June 2007)")
LIST_ITEM (U"• Corrected a bug introduced in 4.6.07 that caused a crash when reading Collections.")
LIST_ITEM (U"• Corrected a bug introduced in 4.6.07 that caused incorrect Open buttons in Inspect.")
LIST_ITEM (U"• How come 4.6.07 introduced those bugs? Because of large changes in the Praat source code as a result of the transition to Unicode.")
NORMAL (U"##4.6.08# (22 June 2007)")
LIST_ITEM (U"• Windows: worked around a `feature' of the C library that caused 3-byte line-breaks in the buttons file.")
LIST_ITEM (U"• Windows: returned to smaller font in script window.")
LIST_ITEM (U"• OT learning: corrected a bug in PositiveHG.")
NORMAL (U"##4.6.07# (20 June 2007)")
LIST_ITEM (U"• Sound files: MP3 as LongSound (implemented by Erez Volk).")
LIST_ITEM (U"• Scripting: Unicode support for strings and script window (Mac and Windows only).")
NORMAL (U"##4.6.06# (4 June 2007)")
LIST_ITEM (U"• Script window: corrected a bug introduced in 4.6.05 that could cause incorrect symbols in saved files.")
NORMAL (U"##4.6.05# (2 June 2007)")
LIST_ITEM (U"• Sound files: reading MP3 audio files (implemented by Erez Volk).")
NORMAL (U"##4.6.04# (29 May 2007)")
LIST_ITEM (U"• OT learning: added decision strategy PositiveHG.")
NORMAL (U"##4.6.03# (24 May 2007)")
LIST_ITEM (U"• Spectral slices have better names.")
NORMAL (U"##4.6.02# (17 May 2007)")
LIST_ITEM (U"• Sound files: saving FLAC audio files (implemented by Erez Volk).")
NORMAL (U"##4.6.01# (16 May 2007)")
LIST_ITEM (U"• Removed a bug that caused downsampling (and therefore formant measurements) "
	"to be incorrect for stereo sounds.")
MAN_END

MAN_BEGIN (U"What was new in 4.6?", U"ppgb", 20070512)
NORMAL (U"##4.6# (12 May 2007)")
NORMAL (U"##4.5.26# (8 May 2007)")
LIST_ITEM (U"• Sound files: reading FLAC audio files (implemented by Erez Volk).")
NORMAL (U"##4.5.25# (7 May 2007)")
LIST_ITEM (U"• Table: Rows to columns...")
LIST_ITEM (U"• Table: Collapse rows... (renamed from Pool).")
LIST_ITEM (U"• Table: Formula (column range)...")
LIST_ITEM (U"• OT learning: OTGrammar window shows harmonies.")
NORMAL (U"##4.5.24# (27 April 2007)")
LIST_ITEM (U"• OT learning: added decision strategy MaximumEntropy; "
	"this has the same harmony determination method as Harmonic Grammar (include the additive constraint noise), "
	"but there is some more variability, in that every candidate gets a relative probability of exp(harmony).")
NORMAL (U"##4.5.23# (26 April 2007)")
LIST_ITEM (U"• Macintosh: much smaller sizes (in kilobytes) of spectrograms for printing and clipboard; "
	"this improves the compatibility with other programs such as Microsoft Word for large spectrograms.")
NORMAL (U"##4.5.22# (25 April 2007)")
LIST_ITEM (U"• Macintosh: improved drawing of spectrograms for printing and clipboard "
	"(this was crippled in 4.5.18, but now it is better than before 4.5.18).")
NORMAL (U"##4.5.21# (24 April 2007)")
LIST_ITEM (U"• OT learning: corrected HarmonicGrammar (and LinearOT) learning procedure "
	"to the stochastic gradient ascent method applied by @@Jäger (2003)@ to MaxEnt grammars.")
LIST_ITEM (U"• Scripting: removed a bug that could make selection checking (in command windows) unreliable after a script was run.")
NORMAL (U"##4.5.20# (19 April 2007)")
LIST_ITEM (U"• Scripting: allow assignments like $$pitch = To Pitch... 0 75 600$.")
LIST_ITEM (U"• PitchTier Formula: guard against undefined values.")
NORMAL (U"##4.5.19# (2 April 2007)")
LIST_ITEM (U"• Scripting: allow comments with \"\\# \" and \";\" in forms.")
LIST_ITEM (U"• Windows audio playing: attempt at more compatibility with Vista.")
NORMAL (U"##4.5.18# (30 March 2007)")
LIST_ITEM (U"• Macintosh: better image drawing (more grey values).")
LIST_ITEM (U"• More tabulation commands.")
LIST_ITEM (U"• More SpectrumTier commands.")
LIST_ITEM (U"• Picture window: keyboard shortcut for ##Erase all#.")
NORMAL (U"##4.5.17# (19 March 2007)")
LIST_ITEM (U"• Picture window: can change arrow size.")
LIST_ITEM (U"• Several #List commands.")
LIST_ITEM (U"• Spectrum: To SpectrumTier (peaks).")
NORMAL (U"##4.5.16# (22 February 2007)")
LIST_ITEM (U"• Sound-to-Intensity: made resistant against undefined settings.")
LIST_ITEM (U"• Windows: made Ctrl-. available as a shortcut.")
LIST_ITEM (U"• Linux: made it more likely to find the correct fonts.")
NORMAL (U"##4.5.15# (12 February 2007)")
LIST_ITEM (U"• Windows XP: worked around a bug in Windows XP that could cause Praat to crash "
	"when the user moved the mouse pointer over a file in the Desktop in the file selector. "
	"The workaround is to temporarily disable file info tips when the file selector window is on the screen.")
NORMAL (U"##4.5.14# (5 February 2007)")
LIST_ITEM (U"• Scripting: some new predefined string variables like preferencesDirectory\\$ .")
NORMAL (U"##4.5.13# (3 February 2007)")
LIST_ITEM (U"• For stereo sounds, pitch analysis is based on correlations pooled over channels "
	"(rather than on correlations of the channel average).")
LIST_ITEM (U"• For stereo sounds, spectrogram analysis is based on power density averaged across channels "
	"(rather than on the power density of the channel average).")
LIST_ITEM (U"• Scripting: removed a bug introduced in 4.5.06 that caused some variables not to be substituted.")
NORMAL (U"##4.5.12# (30 January 2007)")
LIST_ITEM (U"• Made cross-correlation pitch analysis as fast as it used to be before 4.5.11.")
NORMAL (U"##4.5.11# (29 January 2007)")
LIST_ITEM (U"• Sound objects can be stereo, for better playback quality "
	"(most analyses will work on the averaged mono signal).")
LIST_ITEM (U"• Macintosh: recording a sound now uses CoreAudio instead of SoundManager, "	
	"for more compatibility with modern recording devices, "
	"and the possibility to record with a sampling frequency of 96 kHz.")
LIST_ITEM (U"• @ManPages allow picture scripts with separate object lists.")
LIST_ITEM (U"• Linux: better scroll bars in object list for Lesstif (Debian).")
LIST_ITEM (U"• Linux: made @@Create Strings as file list...@ work on Reiser.")
LIST_ITEM (U"• @sendpraat scripts correctly wait until sounds have played.")
NORMAL (U"##4.5.08# (20 December 2006)")
LIST_ITEM (U"• ExperimentMFC: can use stereo sounds.")
NORMAL (U"##4.5.07# (16 December 2006)")
LIST_ITEM (U"• Macintosh: playing a sound now uses CoreAudio instead of SoundManager.")
LIST_ITEM (U"• Phonetic symbols: \\\'1primary stress and \\\'1secon\\\'2dary stress.")
NORMAL (U"##4.5.06# (13 December 2006)")
LIST_ITEM (U"• Support for 32-bit floating-point WAV files.")
LIST_ITEM (U"• Scripting: removed several kinds of restrictions on string length.")
LIST_ITEM (U"• SSCP: Draw confidence ellipse: corrected a bug that would sometimes not draw the ellipse when %N was very large.")
NORMAL (U"##4.5.05# (5 December 2006)")
LIST_ITEM (U"• Macintosh scripting: European symbols such as ö and é and ç are now allowed in file names in scripts "
	"and in MacRoman-encoded file names sent by other programs through the sendpraat subroutine.")
NORMAL (U"##4.5.04# (1 December 2006)")
LIST_ITEM (U"• @@Sound: Change gender...@: corrected a bug that often caused a female-to-male conversion to sound monotonous.")
NORMAL (U"##4.5.03# (29 November 2006)")
LIST_ITEM (U"• Table: added independent-samples t-test.")
LIST_ITEM (U"• Linux: corrected a bug introduced in 4.5.02 that prevented sounds from playing and other weird things.")
NORMAL (U"##4.5.02# (16 November 2006)")
LIST_ITEM (U"• Corrected yet another bug in the new @@Sound: To TextGrid (silences)...@.")
NORMAL (U"##4.5.01# (28 October 2006)")
LIST_ITEM (U"• Sound window: the pitch drawing method is #Curves, #Speckles, or #Automatic.")
LIST_ITEM (U"• Corrected another bug in the new @@Sound: To TextGrid (silences)...@.")
MAN_END

MAN_BEGIN (U"What was new in 4.5?", U"ppgb", 20061026)
NORMAL (U"##4.5# (26 October 2006)")
NORMAL (U"##4.4.35# (20 October 2006)")
LIST_ITEM (U"• In @ManPages you can now draw pictures.")
NORMAL (U"##4.4.34# (19 October 2006)")
LIST_ITEM (U"• Corrected a bug in the new @@Sound: To TextGrid (silences)...@.")
NORMAL (U"##4.4.33# (4 October 2006)")
LIST_ITEM (U"• Windows: corrected a bug introduced in 4.4.31 that caused Praat to skip the first line of the Buttons file.")
NORMAL (U"##4.4.32# (30 September 2006)")
LIST_ITEM (U"• Scripting: more techniques for @@Scripting 4. Object selection|object selection@.")
LIST_ITEM (U"• Scripting: more support for putting the results of the #Info command into a string variable.")
NORMAL (U"##4.4.31# (23 September 2006)")
LIST_ITEM (U"• Support for @@plug-ins@.")
LIST_ITEM (U"• Split between @@Create Strings as file list...@ and @@Create Strings as directory list...@.")
NORMAL (U"##4.4.30# (28 August 2006)")
LIST_ITEM (U"• Table: Draw ellipse (standard deviation)...")
NORMAL (U"##4.4.29# (21 August 2006)")
LIST_ITEM (U"• Allowed \"European\" symbols in file names and object names.")
NORMAL (U"##4.4.28# (10 August 2006)")
LIST_ITEM (U"• Windows XP: Praat files can finally again be opened by double-clicking and by dragging them onto the Praat icon.")
LIST_ITEM (U"• Scripting (Windows): removed a bug that caused Praat to crash if the script window was closed when a file selector window was open.")
NORMAL (U"##4.4.27# (4 August 2006)")
LIST_ITEM (U"• Table window: corrected vertical scroll bar (on Windows).")
LIST_ITEM (U"• Formulas: invSigmoid.")
LIST_ITEM (U"• Logging: added 'power' (and documented the 'freq' command).")
LIST_ITEM (U"• Removed a bug that caused ##Read two Sounds from stereo file...# not to work in scripts.")
NORMAL (U"##4.4.26# (24 July 2006)")
LIST_ITEM (U"• ##Sound & FormantTier: Filter#: much more accurate.")
NORMAL (U"##4.4.25# (16 July 2006)")
LIST_ITEM (U"• TextGrid reading: don't set first boundary to zero for .wrd label files.")
NORMAL (U"##4.4.24# (19 June 2006)")
LIST_ITEM (U"• Scripting: regular expressions allow replacement with empty string.")
NORMAL (U"##4.4.23# (1 June 2006)")
LIST_ITEM (U"• Table: ignore more white space.")
NORMAL (U"##4.4.22# (30 May 2006)")
LIST_ITEM (U"• Scripting: replacing with regular expression. See @@Formulas 5. String functions@.")
NORMAL (U"##4.4.21# (29 May 2006)")
LIST_ITEM (U"• Made Manipulation objects readable again.")
NORMAL (U"##4.4.20# (3 May 2006)")
LIST_ITEM (U"• Removed limit on number of menus (Praat could crash if the number of open windows was high).")
NORMAL (U"##4.4.19# (28 April 2006)")
LIST_ITEM (U"• Table: ##Get mean#, ##Get standard deviation#, ##Get quantile#.")
NORMAL (U"##4.4.18# (24 April 2006)")
LIST_ITEM (U"• Table: ##View & Edit#: view the contents of a table.")
LIST_ITEM (U"• Table: ##Scatter plot#.")
LIST_ITEM (U"• Scripting: more warnings against missing or extra spaces.")
NORMAL (U"##4.4.17# (19 April 2006)")
LIST_ITEM (U"• Table: #Pool: computing averages and medians of dependent variables "
	"for a selected combination of independent variables.")
LIST_ITEM (U"• Table: #Formula accepts string expressions as well as numeric expressions.")
LIST_ITEM (U"• Table: #Sort can sort by any number of columns.")
LIST_ITEM (U"• Table: ##Create with column names#.")
LIST_ITEM (U"• Table: ##Report mean#.")
LIST_ITEM (U"• Formulas: @@Formulas 7. Attributes of objects|row\\$  and col\\$  attributes@.")
LIST_ITEM (U"• Warning when trying to read data files whose format is newer than the Praat version.")
NORMAL (U"##4.4.16# (1 April 2006)")
LIST_ITEM (U"• Spectrum window: dynamic range setting.")
LIST_ITEM (U"• SoundRecorder: corrected a bug in the Intel Mac edition.")
NORMAL (U"##4.4.15# (30 March 2006)")
LIST_ITEM (U"• Source code even more compatible with 64-bit compilers.")
NORMAL (U"##4.4.14# (29 March 2006)")
LIST_ITEM (U"• Source code more compatible with 64-bit compilers.")
NORMAL (U"##4.4.13# (8 March 2006)")
LIST_ITEM (U"• Table To TableOfReal: better handling of --undefined-- values (are now numeric).")
LIST_ITEM (U"• MacOS X: TextGrid files can be double-clicked to open.")
LIST_ITEM (U"• @@Create Strings as file list...@: now handles up to 1,000,000 files per directory.")
NORMAL (U"##4.4.12# (24 February 2006)")
LIST_ITEM (U"• TextGrid: removed a bug introduced in 4.4.10 that caused Praat to crash when converting an IntervalTier into a TextGrid.")
NORMAL (U"##4.4.11# (23 February 2006)")
LIST_ITEM (U"• Listening experiments: removed a bug that could cause Praat to crash when an ExperimentMFC object was removed.")
NORMAL (U"##4.4.10# (20 February 2006)")
LIST_ITEM (U"• Intel computers: corrected reading and writing of 24-bit sound files (error introduced in 4.4.09).")
LIST_ITEM (U"• Create TextGrid: guard against zero tiers.")
LIST_ITEM (U"• MacOS X: correct visibility of Praat icon.")
LIST_ITEM (U"• MacOS X: correct dropping of Praat files on Praat icon.")
NORMAL (U"##4.4.09# (19 February 2006)")
LIST_ITEM (U"• Macintosh: first Intel Macintosh version.")
LIST_ITEM (U"• Windows: Create Strings from directory list...")
NORMAL (U"##4.4.08# (6 February 2006)")
LIST_ITEM (U"• Much improved cepstral smoothing.")
NORMAL (U"##4.4.07# (2 February 2006)")
LIST_ITEM (U"• More scripting facilities (local variables in procedures, e.g. .x and .text\\$ ).")
LIST_ITEM (U"• Faster formulas.")
NORMAL (U"##4.4.06# (30 January 2006)")
LIST_ITEM (U"• More scripting facilities (Object_xxx [ ], Self.nx, Table_xxx\\$  [ ], better messages).")
LIST_ITEM (U"• Better reading and writing of Xwaves label files.")
NORMAL (U"##4.4.05# (26 January 2006)")
LIST_ITEM (U"• @ExperimentMFC: removed a bug that caused Praat to crash when the Oops button was pressed after the experiment finished.")
LIST_ITEM (U"• TextGrid: an IntervalTier can be written to an Xwaves label file.")
NORMAL (U"##4.4.04# (6 January 2006)")
LIST_ITEM (U"• Windows: Quicktime support (see at 4.4.03).")
NORMAL (U"##4.4.03# (6 January 2006)")
LIST_ITEM (U"• Macintosh: Quicktime support, i.e., \"Read from file\" can now read the audio from several kinds of movie files (.mov, .avi).")
NORMAL (U"##4.4.02# (5 January 2006)")
LIST_ITEM (U"• OT learning: allow the decision strategies of Harmonic Grammar and Linear OT.")
NORMAL (U"##4.4.01# (2 January 2006)")
LIST_ITEM (U"• Picture window: \"Logarithmic marks\" allows reversed axes.")
LIST_ITEM (U"• Manipulation window: removed a bug from \"Shift frequencies\" that caused much too small shifts in semitones.")
LIST_ITEM (U"• TextGrid: \"Remove point...\".")
MAN_END

MAN_BEGIN (U"What was new in 4.4?", U"ppgb", 20051219)
NORMAL (U"##4.4# (19 December 2005)")
NORMAL (U"##4.3.37# (15 December 2005)")
LIST_ITEM (U"• @@Principal component analysis@: now accepts tables with more variables (columns) than cases (rows).")
LIST_ITEM (U"• TIMIT label files: removed a bug that caused Praat to crash for files whose first part was not labelled.")
NORMAL (U"##4.3.36# (11 December 2005)")
LIST_ITEM (U"• Ltas: Average.")
LIST_ITEM (U"• Optimality Theory: compute crucial rankings (select OTGrammar + PairDistribution).")
NORMAL (U"##4.3.35# (8 December 2005)")
LIST_ITEM (U"• @ExperimentMFC: switched off warnings for stereo files.")
NORMAL (U"##4.3.34# (8 December 2005)")
LIST_ITEM (U"• Sound window: the arrow scroll step is settable.")
LIST_ITEM (U"• You can now run multiple listening experiments (@ExperimentMFC) in one \"Run\".")
LIST_ITEM (U"• Formant: Get quantile of bandwidth...")
NORMAL (U"##4.3.33# (6 December 2005)")
LIST_ITEM (U"• Removed three bugs introduced in 4.3.32 in @ExperimentMFC.")
NORMAL (U"##4.3.32# (5 December 2005)")
LIST_ITEM (U"• Many more possibilities in @ExperimentMFC.")
NORMAL (U"##4.3.31# (27 November 2005)")
LIST_ITEM (U"• @@Sound: To Ltas (pitch-corrected)...@")
NORMAL (U"##4.3.30# (18 November 2005)")
LIST_ITEM (U"• TableOfReal: Scatter plot: allows reversed axes.")
NORMAL (U"##4.3.29# (11 November 2005)")
LIST_ITEM (U"• Windows: many more font sizes.")
NORMAL (U"##4.3.28# (7 November 2005)")
LIST_ITEM (U"• Fontless EPS files: corrected character width for Symbol font (depended on SILIPA setting).")
LIST_ITEM (U"• Windows: more reliable detection of home directory.")
NORMAL (U"##4.3.27# (7 October 2005)")
LIST_ITEM (U"• TextGrid & Pitch: draw with text alignment.")
NORMAL (U"##4.3.26# (29 September 2005)")
LIST_ITEM (U"• Macintosh: corrected error introduced in 4.3.25.")
NORMAL (U"##4.3.25# (28 September 2005)")
LIST_ITEM (U"• Macintosh: allowed recording with sampling frequencies of 12 and 64 kHz.")
NORMAL (U"##4.3.24# (26 September 2005)")
LIST_ITEM (U"• Table: Down to TableOfReal...: one column of the Table can be used as the row labels for the TableOfReal, "
	"and the strings in the remaining columns of the Table are replaced with whole numbers assigned in alphabetical order.")
NORMAL (U"##4.3.23# (24 September 2005)")
LIST_ITEM (U"• Read Table from comma-separated file...")
LIST_ITEM (U"• Read Table from tab-separated file...")
LIST_ITEM (U"• Write picture as fontless EPS file: choice between XIPA and SILIPA93.")
LIST_ITEM (U"• Bold IPA symbols in EPS files (fontless SILIPA93 only).")
NORMAL (U"##4.3.22# (8 September 2005)")
LIST_ITEM (U"• Macintosh: variable scroll bar size (finally, 7 years since System 8.5).")
NORMAL (U"##4.3.21# (1 September 2005)")
LIST_ITEM (U"• Macintosh: error message if any of the fonts Times, Helvetica, Courier and Symbol are unavailable at start-up.")
LIST_ITEM (U"• Renamed \"Control\" menu to \"Praat\" on all platforms (as on the Mac), "
	"to reflect the fact that no other programs than Praat have used the Praat shell for five years.")
LIST_ITEM (U"• Script editor: Undo and Redo buttons (only on the Mac for now).")
LIST_ITEM (U"• Manual: corrected a bug that sometimes caused Praat to crash when trying to print.")
NORMAL (U"##4.3.20# (18 August 2005)")
LIST_ITEM (U"• Log files: include name of editor window.")
NORMAL (U"##4.3.19# (20 July 2005)")
LIST_ITEM (U"• Improved buttons in manual.")
LIST_ITEM (U"• TableOfReal: Read from headerless spreadsheet file: allow row and column labels to be 30,000 rather than 100 characters.")
NORMAL (U"##4.3.18# (12 July 2005)")
LIST_ITEM (U"• Glottal source for sound synthesis, corrected and documented.")
NORMAL (U"##4.3.17# (7 July 2005)")
LIST_ITEM (U"• Glottal source for sound synthesis.")
LIST_ITEM (U"• Multi-level Optimality Theory: parallel evaluation and bidirectional learning.")
NORMAL (U"##4.3.16# (22 June 2005)")
LIST_ITEM (U"• Pitch drawing: corrected logarithmic scales.")
NORMAL (U"##4.3.15# (22 June 2005)")
LIST_ITEM (U"• Graphics: better dotted lines in pitch contours; clipped pitch curves in editor windows.")
LIST_ITEM (U"• Pitch analysis: more different units (semitones %re 1 Hz).")
NORMAL (U"##4.3.14# (14 June 2005)")
LIST_ITEM (U"• Scripting: regular expressions.")
LIST_ITEM (U"• Removed a bug that caused Praat to crash if a proposed object name was longer than 200 characters.")
NORMAL (U"##4.3.13# (19 May 2005)")
LIST_ITEM (U"• Macintosh: an option to switch off screen previews in EPS files.")
LIST_ITEM (U"• Sources: compatibility of makefiles with MinGW (Minimalist GNU for Windows).")
NORMAL (U"##4.3.12# (10 May 2005)")
LIST_ITEM (U"• Some more manual tricks.")
NORMAL (U"##4.3.11# (6 May 2005)")
LIST_ITEM (U"• TextGrid editor: show number of intervals.")
NORMAL (U"##4.3.10# (25 April 2005)")
LIST_ITEM (U"• Table: Get logistic regression.")
NORMAL (U"##4.3.08# (19 April 2005)")
LIST_ITEM (U"• OT learning: store history with \"OTGrammar & Strings: Learn from partial outputs...\".")
NORMAL (U"##4.3.07# (31 March 2005)")
LIST_ITEM (U"• Linux: removed a bug that could cause a sound to stop playing.")
NORMAL (U"##4.3.04# (9 March 2005)")
LIST_ITEM (U"• Use SIL Doulos IPA 1993/1996 instead of 1989.")
NORMAL (U"##4.3.03# (2 March 2005)")
LIST_ITEM (U"• TextGrid window: green colouring of matching text.")
LIST_ITEM (U"• Regular expressions can be used in many places.")
LIST_ITEM (U"• Pitch analysis: switched off formant-pulling.")
NORMAL (U"##4.3.02# (16 February 2005)")
LIST_ITEM (U"• TextGrid: Remove boundary at time...")
LIST_ITEM (U"• Scripting: corrected %nowarn.")
LIST_ITEM (U"• Linux: guard against blocking audio device.")
LIST_ITEM (U"• Macintosh: guard against out-of-range audio level meter.")
NORMAL (U"##4.3.01# (9 February 2005)")
LIST_ITEM (U"• Replaced PostScript font SILDoulosIPA with XIPA (adapted for Praat by Rafael Laboissière).")
LIST_ITEM (U"• Sound: Set part to zero...")
LIST_ITEM (U"• Pitch: To Sound (sine)...")
LIST_ITEM (U"• Sound & TextGrid: Clone time domain.")
MAN_END

MAN_BEGIN (U"What was new in 4.3?", U"ppgb", 20050126)
ENTRY (U"Praat 4.3, 26 January 2005")
	NORMAL (U"General:")
	LIST_ITEM (U"• `Apply' button in settings windows for menu commands and in script forms.")
	LIST_ITEM (U"• Info window can be saved.")
	LIST_ITEM (U"• Removed 30,000-character limit in Info window.")
	NORMAL (U"Phonetics:")
	LIST_ITEM (U"• Speeded up intensity analysis by a factor of 10 "
		"(by making its time resolution 0.01 ms rather than 0.0001 ms at a sampling frequency of 44 kHz).")
	LIST_ITEM (U"• Speeded up pitch analysis and spectrogram analysis by a factor of two.")
	LIST_ITEM (U"• Sound: To Spectrum... now has a reasonably fast non-FFT version.")
	LIST_ITEM (U"• Calibrated long-term average spectrum (Sound: To Ltas...).")
	LIST_ITEM (U"• Pitch-corrected LTAS analysis.")
	LIST_ITEM (U"• Sound: Scale intensity.")
	LIST_ITEM (U"• PitchTier: To Sound (sine).")
	LIST_ITEM (U"• Better warnings against use of the LPC object.")
	NORMAL (U"Voice:")
	LIST_ITEM (U"• July 9, 2004 (4.2.08): Shimmer measurements: more accurate and less sensitive to additive noise.")
	LIST_ITEM (U"• More extensive voice report: pitch statistics; harmonicity.")
	NORMAL (U"Audio:")
	LIST_ITEM (U"• Reading and opening 24-bit and 32-bit sound files (saving still goes in 16 bits).")
	LIST_ITEM (U"• LongSound: save separate channels.")
	LIST_ITEM (U"• Macintosh: much faster reading of WAV files.")
	NORMAL (U"Listening experiments:")
	LIST_ITEM (U"• Subjects can now respond with keyboard presses.")
	NORMAL (U"Graphics:")
	LIST_ITEM (U"• One can now drag the inner viewport in the Picture window, excluding the margins. "
		"This is nice e.g. for creating square viewports or for drawing a waveform and a spectrogram in close contact.")
	LIST_ITEM (U"• Unix: picture highlighting as on Mac and Windows.")
	LIST_ITEM (U"• More drawing methods for Sound and Ltas (curve, bars, poles, speckles).")
	NORMAL (U"OT learning:")
	LIST_ITEM (U"• Monitor rankings when learning.")
	LIST_ITEM (U"• OTGrammar: Remove harmonically bounded candidates...")
	LIST_ITEM (U"• OTGrammar: Save as headerless spreadsheet file...")
	LIST_ITEM (U"• Metrics grammar: added *Clash, *Lapse, WeightByPosition and *MoraicConsonant.")
	NORMAL (U"Scripting:")
	LIST_ITEM (U"• nowarn, noprogress, nocheck.")
	LIST_ITEM (U"• Line numbers.")
MAN_END

MAN_BEGIN (U"What was new in 4.2?", U"ppgb", 20040304)
ENTRY (U"Praat 4.2, 4 March 2004")
	NORMAL (U"General:")
	LIST_ITEM (U"• July 10, 2003: Open source code (General Public Licence).")
	NORMAL (U"Phonetics:")
	LIST_ITEM (U"• Faster computation of spectrum, spectrogram, and pitch.")
	LIST_ITEM (U"• More precision in numeric libraries.")
	LIST_ITEM (U"• PitchTier: Interpolate quadratically.")
	LIST_ITEM (U"• TextGrids can be saved chronologically (and Praat can read that file again).")
	LIST_ITEM (U"• Sound editor window @@Time step settings...@: \"Automatic\", \"Fixed\", and \"View-dependent\".")
	LIST_ITEM (U"• Sound window: distinguish basic from advanced spectrogram and pitch settings.")
	LIST_ITEM (U"• Read TableOfReal from headerless spreadsheet file...: cells with strings are considered zero.")
	LIST_ITEM (U"• Sound window: introduced time step as advanced setting.")
	LIST_ITEM (U"• Sound window: reintroduced view range as advanced setting.")
	LIST_ITEM (U"• Ltas: Compute trend line, Subtract trend line.")
	NORMAL (U"Audio:")
	LIST_ITEM (U"• Sun workstations: support audio servers.")
	NORMAL (U"Graphics:")
	LIST_ITEM (U"• Better selections in Picture window and editor windows.")
	LIST_ITEM (U"• Picture window: better handling of rectangles and ellipses for reversed axes.")
	LIST_ITEM (U"• Windows: corrected positioning of pictures on clipboard and in metafiles.")
	LIST_ITEM (U"• Windows: EPS files check availability of Times and TimesNewRomanPSMT.")
	LIST_ITEM (U"• Polygon: can now also paint in colour instead of only in grey values.")
	LIST_ITEM (U"• Unlimited number of points for polygons in PostScript (may not work on very old printers).")
	LIST_ITEM (U"• Picture window: line widths on all printers and clipboards are now equal to line widths used on PostScript printers: "
		"a line with a line width of \"1\" will be drawn with a width 3/8 points. This improves the looks of pictures printed "
		"on non-PostScript printers, improves the looks of pictures copied to your wordprocessor when printed, "
		"and changes the looks of pictures copied to your presentation program.")
	NORMAL (U"OT learning:")
	LIST_ITEM (U"• Metrics grammar supports \'impoverished overt forms\', "
		"i.e. without secondary stress even if surface structures do have secondary stress.")
	LIST_ITEM (U"• Support for crucially tied constraints and tied candidates.")
	LIST_ITEM (U"• Support for backtracking in EDCD.")
	LIST_ITEM (U"• Queries for testing grammaticality.")
	NORMAL (U"Scripting:")
	LIST_ITEM (U"• ManPages: script links can receive arguments.")
	LIST_ITEM (U"• ManPages: variable duration of recording.")
	LIST_ITEM (U"• Support for unlimited size of script files in editor window on Windows XP and MacOS X (the Unix editions already had this).")
	LIST_ITEM (U"• Improved the reception of %sendpraat commands on Windows XP.")
MAN_END

MAN_BEGIN (U"What was new in 4.1?", U"ppgb", 20030605)
ENTRY (U"Praat 4.1, 5 June 2003")
	NORMAL (U"General:")
	LIST_ITEM (U"• MacOS X edition.")
	LIST_ITEM (U"• Removed licensing.")
	LIST_ITEM (U"• More than 99 percent of the source code distributed under the General Public Licence.")
	LIST_ITEM (U"• Windows 2000 and XP: put preferences files in home directory.")
	NORMAL (U"Phonetics:")
	LIST_ITEM (U"• Spectrum: the sign of the Fourier transform has changed, to comply with common use "
		"in technology and physics. Old Spectrum files are converted when read.")
	LIST_ITEM (U"• Spectral moments.")
	LIST_ITEM (U"• Many jitter and shimmer measures, also in the Sound editor window.")
	LIST_ITEM (U"• PitchTier: shift or multiply frequencies (also in ManipulationEditor).")
	LIST_ITEM (U"• TextGrid: shift times, scale times.")
	LIST_ITEM (U"• Overlap-add synthesis: reduced buzz in voiceless parts.")
	LIST_ITEM (U"• @@Sound: Change gender...")
	LIST_ITEM (U"• Editors: @@Intro 3.6. Viewing a spectral slice@.")
	LIST_ITEM (U"• Editors: Get spectral power at cursor cross.")
	LIST_ITEM (U"• @@Sound: To PointProcess (periodic, peaks)...@")
	LIST_ITEM (U"• Ltas: merge.")
	NORMAL (U"Listening experiments:")
	LIST_ITEM (U"• Goodness judgments.")
	LIST_ITEM (U"• Multiple ResultsMFC: ##To Table#, so that the whole experiment can go into a single statistics file.")
	LIST_ITEM (U"• Stimulus file path can be relative to directory of experiment file.")
	LIST_ITEM (U"• @ExperimentMFC: multiple substimuli for discrimination tests.")
	NORMAL (U"Statistics:")
	LIST_ITEM (U"• New @Table object for column @statistics: Pearson's %r, Kendall's %\\ta-%b, %t-test.")
	LIST_ITEM (U"• Table: scatter plot.")
	LIST_ITEM (U"• Table: column names as variables.")
	LIST_ITEM (U"• @@T-test@.")
	LIST_ITEM (U"• TableOfReal: Extract rows where column...")
	LIST_ITEM (U"• TableOfReal: Get correlation....")
	LIST_ITEM (U"• @@Correlation: Confidence intervals...")
	LIST_ITEM (U"• @@SSCP: Get diagonality (bartlett)...")
	NORMAL (U"OT learning:")
	LIST_ITEM (U"• Tutorial for bidirectional learning.")
	LIST_ITEM (U"• Random choice between equally violating candidates.")
	LIST_ITEM (U"• More constraints in metrics grammar.")
	LIST_ITEM (U"• Learning in editor.")
	NORMAL (U"Graphics:")
	LIST_ITEM (U"• Printing: hard-coded image interpolation for EPS files and PostScript printers.")
	NORMAL (U"Scripting:")
	LIST_ITEM (U"• New @Formulas tutorial.")
	LIST_ITEM (U"• @Formulas: can use variables without quotes.")
	LIST_ITEM (U"• Formulas for PitchTier, IntensityTier, AmplitudeTier, DurationTier.")
	LIST_ITEM (U"• Refer to any matrices and tables in formulas, e.g. Sound_hello (x) or Table_everything [row, col] "
		"or Table_tokens [i, \"F1\"].")
	LIST_ITEM (U"• Assignment by modification, as with += -= *= /=.")
	LIST_ITEM (U"• New functions: date\\$ (), extractNumber, extractWord\\$ , extractLine\\$ . See @@Formulas 5. String functions@.")
	LIST_ITEM (U"• @@Scripting 5.8. Including other scripts@.")
	LIST_ITEM (U"• String formulas in the calculator.")
	LIST_ITEM (U"• Stopped support of things that had been undocumented for the last four years: "
		"#let, #getnumber, #getstring, #ARGS, #copy, #proc, variables with capitals, and strings in numeric variables; "
		"there are messages about how to modify your old scripts.")
	LIST_ITEM (U"• Disallowed ambiguous expressions like -3\\^ 2.")
MAN_END

MAN_BEGIN (U"What was new in 4.0?", U"ppgb", 20011015)
ENTRY (U"Praat 4.0, 15 October 2001")
	NORMAL (U"Editors:")
	LIST_ITEM (U"• Simplified selection and cursor in editor windows.")
	LIST_ITEM (U"• Spectrogram, pitch contour, formant contour, and intensity available in the "
		"Sound, LongSound, and TextGrid editors.")
	LIST_ITEM (U"• TextGrid editor: additions and improvements.")
	LIST_ITEM (U"• @@Log files@.")
	NORMAL (U"Phonetics library:")
	LIST_ITEM (U"• @ExperimentMFC: multiple-forced-choice listening experiments.")
	LIST_ITEM (U"• @@Sound: To Pitch (ac)...@: pitch contour less dependent on time resolution. "
		"This improves the constancy of the contours in the editors when zooming.")
	LIST_ITEM (U"• TextGrid: additions and improvements.")
	LIST_ITEM (U"• Sounds: Concatenate recoverably. Creates a TextGrid whose interval labels are the original "
		"names of the sounds.")
	LIST_ITEM (U"• Sound & TextGrid: Extract all intervals. The reverse of the previous command.")
	LIST_ITEM (U"• Filterbank analyses, @MelFilter, @BarkFilter and "
		"@FormantFilter, by @@band filtering in the frequency domain@." )
	LIST_ITEM (U"• Cepstrum by David Weenink: @MFCC, @LFCC. "
		"@Cepstrum object is a representation of the %%complex cepstrum%.")
	LIST_ITEM (U"• Intensity: To IntensityTier (peaks, valleys).")
	LIST_ITEM (U"• Replaced Analysis and AnalysisEditor with @Manipulation and @ManipulationEditor.")
	NORMAL (U"Phonology library:")
	LIST_ITEM (U"• PairDistribution: Get percentage correct (maximum likelihood, probability matching).")
	LIST_ITEM (U"• OTGrammar & PairDistribution: Get percentage correct...")
	NORMAL (U"Graphics:")
	LIST_ITEM (U"• Improved spectrogram drawing.")
	LIST_ITEM (U"• @@Special symbols@: háček.")
	LIST_ITEM (U"• Macintosh: improved screen rendition of rotated text.")
	NORMAL (U"Audio:")
	LIST_ITEM (U"• Macintosh: support for multiple audio input devices (sound cards).")
	NORMAL (U"Statistics and numerics library:")
	LIST_ITEM (U"• More statistics by David Weenink.")
	LIST_ITEM (U"• Improved random numbers and other numerical stuff.")
	LIST_ITEM (U"• @@Regular expressions@.")
	NORMAL (U"Scripting:")
	LIST_ITEM (U"• Formatting in variable substitution, e.g. 'pitch:2' gives two digits after the decimal point.")
	LIST_ITEM (U"• Added ##fixed\\$ # to scripting language for formatting of numbers.")
	NORMAL (U"Documentation:")
	LIST_ITEM (U"• @@Multidimensional scaling@ tutorial.")
	LIST_ITEM (U"• Enabled debugging-at-a-distance.")
MAN_END

MAN_BEGIN (U"What was new in 3.9?", U"ppgb", 20001018)
ENTRY (U"Praat 3.9, 18 October 2000")
	NORMAL (U"Editors:")
	LIST_ITEM (U"• Shift-click and shift-drag extend or shrink selection in editor windows.")
	LIST_ITEM (U"• Grouped editors can have separate zooming and scrolling (FunctionEditor preferences).")
	LIST_ITEM (U"• Cursor follows playing sound in editors; interruption by Escape key moves the cursor.")
	LIST_ITEM (U"• TextGridEditor: optimized for transcribing large corpora: text field, directly movable boundaries, "
		"more visible text in tiers, @SpellingChecker, "
		"type while the sound is playing, complete keyboard navigation, control font size, control text alignment, "
		"shift-click near boundary adds interval to selection.")
	LIST_ITEM (U"• Stereo display in LongSound and TextGrid editors.")
	LIST_ITEM (U"• LongSoundEditor and TextGridEditor: write selection to audio file.")
	LIST_ITEM (U"• SoundEditor: added command \"Extract selection (preserve times)\".")
	LIST_ITEM (U"• IntervalTierEditor, DurationTierEditor.")
	LIST_ITEM (U"• Added many query commands in editors.")
	NORMAL (U"Phonetics library:")
	LIST_ITEM (U"• Sound: To Formant...: sample-rate-independent formant analysis.")
	LIST_ITEM (U"• Sound: To Harmonicity (glottal-to-noise excitation ratio).")
	LIST_ITEM (U"• Pitch: support for ERB units, draw all combinations of line/speckle and linear/logarithmic/semitones/mels/erbs, "
		"optionally with TextGrid, Subtract linear fit.")
	LIST_ITEM (U"• Spectrum: Draw along logarithmic frequency axis.")
	LIST_ITEM (U"• TextGrid:  modification commands, Extract part, Shift to zero, Scale times (with Sound or LongSound).")
	LIST_ITEM (U"• @@Matrix: To TableOfReal@, Draw contour...")
	LIST_ITEM (U"• Concatenate Sound and LongSound objects.")
	LIST_ITEM (U"• File formats: save PitchTier in spreadsheet format, read CGN syntax files (XML version), "
		"text files now completely file-server-safe (independent from Windows/Macintosh/Unix line separators).")
	NORMAL (U"Statistics and numerics library:")
	LIST_ITEM (U"• @@Principal component analysis@.")
	LIST_ITEM (U"• @@Discriminant analysis@.")
	LIST_ITEM (U"• @Polynomial: drawing, @@Roots|root@ finding etc.")
	LIST_ITEM (U"• @@TableOfReal: Draw box plots...@.")
	LIST_ITEM (U"• @@Covariance: To TableOfReal (random sampling)...@.")
	LIST_ITEM (U"• @@SSCP: Get sigma ellipse area...@.")
	LIST_ITEM (U"• Query @DTW for 'weighted distance' of time warp.")
	LIST_ITEM (U"• Distributions: To Strings (exact)...")
	LIST_ITEM (U"• Strings: Randomize.")
	NORMAL (U"Phonology library:")
	LIST_ITEM (U"• OTGrammar: To PairDistribution.")
	NORMAL (U"Graphics:")
	LIST_ITEM (U"• Full support for colour inkjet printers on Windows and Macintosh.")
	LIST_ITEM (U"• Full support for high-resolution colour clipboards and metafiles for "
		"Windows and Macintosh programs that support them (this include MS Word "
		"for Windows, but unfortunately not MS Word for Macintosh).")
	LIST_ITEM (U"• Colour in EPS files.")
	LIST_ITEM (U"• Interpolating grey images, i.e. better zoomed spectrograms.")
	LIST_ITEM (U"• Linux: support for 24-bits screens.")
	NORMAL (U"Audio:")
	LIST_ITEM (U"• Asynchronous sound play.")
	LIST_ITEM (U"• Linux: solved problems with /dev/mixer (\"Cannot read MIC gain.\") on many computers.")
	LIST_ITEM (U"• Added possibility of zero padding for sound playing, "
		"in order to reduce clicks on some Linux and Sun computers.")
	LIST_ITEM (U"• LongSound supports mono and stereo, 8-bit and 16-bit, %\\mu-law and A-law, "
		"big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.")
	LIST_ITEM (U"• \"Read two Sounds from stereo file...\" supports 8-bit and 16-bit, %\\mu-law and A-law, "
		"big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.")
	LIST_ITEM (U"• SoundRecorder writes to 16-bit AIFC, WAV, NeXT/Sun, and NIST mono and stereo files.")
	LIST_ITEM (U"• Sound & LongSound: write part or whole to mono or stereo audio file.")
	LIST_ITEM (U"• Read Sound from raw Alaw file.")
	LIST_ITEM (U"• Artword & Speaker (& Sound) movie: real time on all platforms.")
	NORMAL (U"Scripting:")
	LIST_ITEM (U"• @@Formulas 4. Mathematical functions@: added statistical functions: %\\ci^2, Student T, Fisher F, binomial, "
		"and their inverse functions.")
	LIST_ITEM (U"• Windows: program #praatcon for use as a Unix-style console application.")
	LIST_ITEM (U"• Windows and Unix: Praat can be run with a command-line interface without quitting on errors.")
	LIST_ITEM (U"• Unix & Windows: can use <stdout> as a file name (supports pipes for binary data).")
	LIST_ITEM (U"• @sendpraat now also for Macintosh.")
	LIST_ITEM (U"• @@Scripting 6.7. Sending a message to another program|sendsocket@.")
	LIST_ITEM (U"• @@Read from file...@ recognizes script files if they begin with \"\\# !\".")
	LIST_ITEM (U"• Script links in @ManPages.")
	NORMAL (U"Documentation")
	LIST_ITEM (U"• Tutorials on all subjects available through @Intro.")
MAN_END

MAN_BEGIN (U"What was new in 3.8?", U"ppgb", 19990112)
ENTRY (U"Praat 3.8, 12 January 1999")
	NORMAL (U"Phonetics library")
	LIST_ITEM (U"• New objects: @LongSound (view and label long sound files), with editor; PairDistribution.")
	LIST_ITEM (U"• @@Overlap-add@ manipulation of voiceless intervals, version 2: quality much better now; "
		"target duration is exactly as expected from Duration tier or specified lengthening in @@Sound: Lengthen (overlap-add)...@.")
	LIST_ITEM (U"• Audio: Escape key stops audio playing (on Mac also Command-period).")
	LIST_ITEM (U"• @SoundRecorder: allows multiple recordings without close; Play button; Write buttons; buffer size can be set.")
	LIST_ITEM (U"• Reverse a Sound or a selection of a Sound.")
	LIST_ITEM (U"• @@Sound: Get nearest zero crossing...@.")
	LIST_ITEM (U"• Formant: \"Scatter plot (reversed axes)...\".")
	LIST_ITEM (U"• TextGrid & Pitch: \"Speckle separately...\".")
	LIST_ITEM (U"• \"Extract Sound selection (preserve times)\" in TextGridEditor.")
	LIST_ITEM (U"• More query commands for Matrix, TableOfReal, Spectrum, PointProcess.")
	NORMAL (U"Phonology library")
	LIST_ITEM (U"• 25-page OT learning tutorial.")
	LIST_ITEM (U"• Made the OT learner 14 times as fast.")
	NORMAL (U"Platforms")
	LIST_ITEM (U"• May 23: Windows beta version.")
	LIST_ITEM (U"• April 24: Windows alpha version.")
	NORMAL (U"Files")
	LIST_ITEM (U"• Read more Kay, Sun (.au), and WAV sound files.")
	LIST_ITEM (U"• \"Read Strings from raw text file...\"")
	LIST_ITEM (U"• @@Create Strings as file list...@.")
	LIST_ITEM (U"• \"Read IntervalTier from Xwaves...\"")
	LIST_ITEM (U"• hidden \"Read from old Windows Praat picture file...\"")
	NORMAL (U"Graphics")
	LIST_ITEM (U"• Use colours (instead of only greys) in \"Paint ellipse...\" etc.")
	LIST_ITEM (U"• More true colours (maroon, lime, navy, teal, purple, olive).")
	LIST_ITEM (U"• Direct printing from Macintosh to PostScript printers.")
	LIST_ITEM (U"• Hyperpage printing to PostScript printers and PostScript files.")
	LIST_ITEM (U"• Phonetic symbols: raising sign, lowering sign, script g, corner, ligature, pointing finger.")
	NORMAL (U"Shell")
	LIST_ITEM (U"• November 4: all dialogs are modeless (which is new for Unix and Mac).")
	LIST_ITEM (U"• September 27: @sendpraat for Windows.")
	NORMAL (U"Scripting")
	LIST_ITEM (U"• January 7: scriptable editors.")
	LIST_ITEM (U"• October 7: file I/O in scripts.")
	LIST_ITEM (U"• August 23: script language includes all the important functions for string handling.")
	LIST_ITEM (U"• June 24: string variables in scripts.")
	LIST_ITEM (U"• June 22: faster look-up of script variables.")
	LIST_ITEM (U"• June 22: unlimited number of script variables.")
	LIST_ITEM (U"• April 5: suspended chopping of trailing spaces.")
	LIST_ITEM (U"• March 29: enabled formulas as arguments to dialogs (also interactive).")
MAN_END

MAN_BEGIN (U"What was new in 3.7?", U"ppgb", 19980324)
ENTRY (U"Praat 3.7, 24 March 1998")
	NORMAL (U"Editors:")
	LIST_ITEM (U"• In all FunctionEditors: drag to get a selection.")
	NORMAL (U"Phonetics library:")
	LIST_ITEM (U"• Many new query (#Get) commands for @Sound, @Intensity, @Harmonicity, @Pitch, "
		"@Formant, @Ltas, @PitchTier, @IntensityTier, @DurationTier, #FormantTier.")
	LIST_ITEM (U"• Many new modification commands.")
	LIST_ITEM (U"• Many new interpolations.")
	LIST_ITEM (U"• Sound enhancements: @@Sound: Lengthen (overlap-add)...@, @@Sound: Deepen band modulation...@")
	LIST_ITEM (U"• @@Source-filter synthesis@ tutorial, @@Sound & IntensityTier: Multiply@, "
		"##Sound & FormantTier: Filter#, @@Formant: Formula (frequencies)...@, @@Sound: Pre-emphasize (in-line)...@.")
	NORMAL (U"Labelling")
	LIST_ITEM (U"• TextGrid queries (#Get times and labels in a script).")
	LIST_ITEM (U"• @@TextGrid: Count labels...@.")
	LIST_ITEM (U"• @@PointProcess: To TextGrid (vuv)...@: get voiced/unvoiced information from a point process.")
	LIST_ITEM (U"• IntervalTier to TableOfReal: labels become row labels.")
	LIST_ITEM (U"• TextTier to TableOfReal.")
	NORMAL (U"Numerics and statistics library")
	LIST_ITEM (U"• Multidimensional scaling (Kruskal, INDSCAL, etc).")
	LIST_ITEM (U"• @TableOfReal: Set value, Formula, Remove column, Insert column, Draw as squares, To Matrix.")
	NORMAL (U"Phonology library")
	LIST_ITEM (U"• OT learning: new strategies: weighted symmetric plasticity (uncancelled or all).")
	NORMAL (U"Praat shell")
	LIST_ITEM (U"• First Linux version.")
	LIST_ITEM (U"• Eight new functions like e.g. %hertzToBark in @@Formulas 4. Mathematical functions@.")
	LIST_ITEM (U"• @@Praat script@: procedure arguments; object names.")
	NORMAL (U"Documentation:")
	LIST_ITEM (U"• 230 more man pages (now 630).")
	LIST_ITEM (U"• Hypertext: increased readability of formulas, navigation with keyboard.")
MAN_END

MAN_BEGIN (U"What was new in 3.6?", U"ppgb", 19971027)
ENTRY (U"Praat 3.6, 27 October 1997")
	NORMAL (U"Editors:")
	LIST_ITEM (U"• Intuitive position of B and E buttons on left-handed mice.")
	LIST_ITEM (U"• @SoundEditor: copy %windowed selection to list of objects.")
	LIST_ITEM (U"• @SoundEditor: undo Cut, Paste, Zero.")
	LIST_ITEM (U"• @SpectrumEditor: copy band-filtered spectrum or sound to list of objects.")
	LIST_ITEM (U"• @ManipulationEditor: LPC-based pitch manipulation.")
	NORMAL (U"Objects:")
	LIST_ITEM (U"• Use '-', and '+' in object names.")
	NORMAL (U"Phonetics library")
	LIST_ITEM (U"• LPC-based resynthesis in @ManipulationEditor.")
	LIST_ITEM (U"• @Sound: direct modification without formulas (addition, multiplication, windowing)")
	LIST_ITEM (U"• @Sound: filtering in spectral domain by formula.")
	LIST_ITEM (U"• Create a simple @Pitch object from a @PitchTier (for %F__0_) and a @Pitch (for V/U).")
	LIST_ITEM (U"• Semitones in @PitchTier tables.")
	LIST_ITEM (U"• @PointProcess: transplant time domain from @Sound.")
	LIST_ITEM (U"• Much more...")
	NORMAL (U"Phonology library")
	LIST_ITEM (U"• Computational Optimality Theory. See @@OT learning@.")
	NORMAL (U"Hypertext")
	LIST_ITEM (U"• You can use @ManPages files for creating your own tutorials. "
		"These contains buttons for playing and recording sounds, so you can use this for creating "
		"an interactive IPA sound training course.")
	NORMAL (U"Scripting:")
	LIST_ITEM (U"• Programmable @@Praat script@ language: variables, expressions, control structures, "
		"procedures, complete dialog box, exchange of information with Info window, continuation lines.")
	LIST_ITEM (U"• Use platform-independent relative file paths in @@Praat script@.")
	LIST_ITEM (U"• @ScriptEditor: Run selection.")
	NORMAL (U"Graphics:")
	LIST_ITEM (U"• Rotation and scaling while printing the @@Picture window@.")
	LIST_ITEM (U"• Apart from bold and italic, now also bold-italic (see @@Text styles@).")
	LIST_ITEM (U"• Rounded rectangles.")
	LIST_ITEM (U"• Conversion of millimetres and world coordinates.")
	LIST_ITEM (U"• Measurement of text widths (screen and PostScript).")
	NORMAL (U"Unix:")
	LIST_ITEM (U"• Use the @sendpraat program for sending messages to running Praat programs.")
	NORMAL (U"Mac:")
	LIST_ITEM (U"• Praat looks best with the new and beautiful System 8.")
MAN_END

MAN_BEGIN (U"What was new in 3.5?", U"ppgb", 19970527)
ENTRY (U"Praat 3.5, 27 May 1997")
	NORMAL (U"New editors:")
	LIST_ITEM (U"• #TextGridEditor replaces and extends LabelEditor: edit points as well as intervals.")
	LIST_ITEM (U"• #AnalysisEditor replaces and extends PsolaEditor: view pitch, spectrum, formant, and intensity "
		"analyses in a single window, and allow pitch and duration resynthesis by overlap-add and more (would be undone in 3.9.19).")
	LIST_ITEM (U"• #SpectrumEditor allows you to view and edit spectra.")
	NORMAL (U"Praat shell:")
	LIST_ITEM (U"• ##History mechanism# remembers all the commands that you have chosen, "
		"and allows you to put them into a script.")
	LIST_ITEM (U"• #ScriptEditor allows you to edit and run any Praat script, and to put it under a button.")
	LIST_ITEM (U"• All added and removed buttons are remembered across sessions.")
	LIST_ITEM (U"• #ButtonEditor allows you to make buttons visible or invisible.")
	NORMAL (U"Evaluations:")
	LIST_ITEM (U"• In his 1996 doctoral thesis, Henning Reetz "
		"compared five pitch analysis routines; @@Sound: To Pitch (ac)...@ appeared to make the fewest errors. "
		"H. Reetz (1996): %%Pitch Perception in Speech: a Time Domain Approach%, Studies in Language and Language Use #26, "
		"IFOTT, Amsterdam (ICG Printing, Dordrecht).")
	NORMAL (U"Documentation:")
	LIST_ITEM (U"• 140 more man pages (now 330).")
	LIST_ITEM (U"• Tables and pictures in manual.")
	LIST_ITEM (U"• Printing the entire manual.")
	LIST_ITEM (U"• Logo.")
	NORMAL (U"New types:")
	LIST_ITEM (U"• Labelling & segmentation: #TextGrid, #IntervalTier, #TextTier.")
	LIST_ITEM (U"• Analysis & manipulation: #Analysis.")
	LIST_ITEM (U"• Statistics: #TableOfReal, #Distributions, #Transition")
	NORMAL (U"File formats:")
	LIST_ITEM (U"• Read and write rational numbers in text files.")
	LIST_ITEM (U"• Read 8-bit .au sound files.")
	LIST_ITEM (U"• Read and write raw 8-bit two\'s-complement and offset-binary sound files.")
	NORMAL (U"Audio:")
	LIST_ITEM (U"• 16-bit interactive Sound I/O on Mac.")
	LIST_ITEM (U"• Record sounds at 9.8 kHz on SGI.")
	NORMAL (U"New commands:")
	LIST_ITEM (U"• Two more pitch-analysis routines.")
	LIST_ITEM (U"• Sound to PointProcess: collect all maxima, minima, zero crossings.")
	LIST_ITEM (U"• PointProcess: set calculus.")
	LIST_ITEM (U"• TextGrid: extract time-point information.")
	LIST_ITEM (U"• Compute pitch or formants at given time points.")
	LIST_ITEM (U"• Put pitch, formants etc. in tables en get statistics.")
	LIST_ITEM (U"• Many more...")
	NORMAL (U"Macintosh:")
	LIST_ITEM (U"• 16-bit interactive sound I/O.")
	LIST_ITEM (U"• Fast and interpolating spectrogram drawing.")
	LIST_ITEM (U"• Phonetic Mac screen font included in source code (as a fallback to using SIL Doulos IPA).")
	LIST_ITEM (U"• Keyboard shortcuts, text editor, help under question mark, etc.")
MAN_END

MAN_BEGIN (U"What was new in 3.3?", U"ppgb", 19961006)
ENTRY (U"Praat 3.3, 6 October 1996")
	LIST_ITEM (U"• Documentation: hypertext help browser, including the first 190 man pages.")
	LIST_ITEM (U"• New editors: type #TextTier for labelling times instead of intervals.")
	LIST_ITEM (U"• New actions: #Formant: Viterbi tracker, Statistics menu, Scatter plot.")
	LIST_ITEM (U"• Evaluation: For HNR analysis of speech, the cross-correlation method, "
		"which has a sensitivity of 60 dB and a typical time resolution of 12 milliseconds, "
		"must be considered better than the autocorrelation method, "
		"which has a better sensitivity (80 dB), but a much worse time resolution (30 ms). "
		"For pitch analysis, the autocorrelation method still beats the cross-correlation method "
		"because of its better resistance against noise and echos, "
		"and despite its marginally poorer resolution (15 vs. 12 ms).")
	LIST_ITEM (U"• User preferences are saved across sessions.")
	LIST_ITEM (U"• The phonetic X screen font included in the source code.")
	LIST_ITEM (U"• Xwindows resources included in the source code")
	LIST_ITEM (U"• Graphics: eight colours, small caps, text rotation.")
	LIST_ITEM (U"• File formats: Sun/NexT mu-law files, raw matrix text files, Xwaves mark files.")
	LIST_ITEM (U"• Accelerations: keyboard shortcuts, faster dynamic menu, Shift-OK keeps file selector on screen.")
	LIST_ITEM (U"• Object type changes: #StylPitch and #MarkTier are now called #PitchTier and #TextTier, respectively. "
		"Old files can still be read.")
	LIST_ITEM (U"• Script warning: all times in dialogs are in seconds now: milliseconds have gone.")
MAN_END

MAN_BEGIN (U"What was new in 3.2?", U"ppgb", 19960429)
ENTRY (U"Praat 3.2, 29 April 1996")
	LIST_ITEM (U"• Sound I/O for HPUX, Sun Sparc 5, and Sun Sparc LX.")
	LIST_ITEM (U"• Cross-correlation pitch and HNR analysis.")
	LIST_ITEM (U"• Facilities for generating tables from scripts.")
	LIST_ITEM (U"• Editing and playing stylized pitch contours and point processes.")
	LIST_ITEM (U"• Overlap-add pitch manipulation.")
	LIST_ITEM (U"• Spectral smoothing techniques: cepstrum and LPC.")
	LIST_ITEM (U"• Time-domain pitch analysis with jitter measurement.")
	LIST_ITEM (U"• Read and write Bell-Labs sound files and Kay CSL audio files.")
	LIST_ITEM (U"• Replaced IpaTimes font by free SILDoulos-IPA font, and embedded phonetic font in PostScript picture.")
	LIST_ITEM (U"• Completed main phonetic characters.")
MAN_END

MAN_BEGIN (U"What was new in 3.1?", U"ppgb", 19951205)
ENTRY (U"Praat 3.1, 5 December 1995")
	LIST_ITEM (U"• Add and remove buttons dynamically.")
	LIST_ITEM (U"• DataEditor (Inspect button).")
	LIST_ITEM (U"• Initialization scripts.")
	LIST_ITEM (U"• Logarithmic axes.")
	LIST_ITEM (U"• Call remote ADDA server directly.")
MAN_END
/*
   BUGBASE

>* The Artword editor would be easier to read if the vertical axis of the
>graphs were squeezed a little.  As it is, the line for a target of 1
>throughout the utterance merges into the top bounding box and is
>invisible.
>

>* In drawing a picture of an Artword for a Speaker, the Pen|Line-width
>option only works for dotted and dashed lines.  It's ignored for plain
>lines (and is WAY too wide).

ENTRY (U"To do")
	LIST_ITEM (U"• TextGrid & Sound: Extract intervals with margins.")
	LIST_ITEM (U"• Spectrum: draw power, re, im, phase.")
	LIST_ITEM (U"• Formant: To Spectrum (slice)... (combines Formant-to-LPC and LPC-to-Spectrum-slice)")
	LIST_ITEM (U"• Read and/or write Matlab files, MBROLA files, Xwaves files, CHAT files.") // Aix
	LIST_ITEM (U"• Matrix: draw numbers.")
	LIST_ITEM (U"• Fractions with \\bsf{a|b}.")
	LIST_ITEM (U"• Move objects up and down list.")
	LIST_ITEM (U"• Spectrogram cross-correlation.")
	LIST_ITEM (U"• Labels in AIFC file.") // Theo Veenker 19980323
	LIST_ITEM (U"• Improve scrolling and add selection in hyperpages.")
	LIST_ITEM (U"• Segment spectrograph?") // Ton Wempe, Jul 16 1996
	LIST_ITEM (U"• Phoneme-to-articulation conversion??") // Mirjam Ernestus, Jul 1 1996
ENTRY (U"Known bugs in the Windows version")
	LIST_ITEM (U"• Cannot stand infinitesimal zooming in SpectrogramEditor.")
*/
 
MAN_BEGIN (U"Acknowledgments", U"ppgb", 20161227)
NORMAL (U"The following people contributed source code to Praat:")
LIST_ITEM (U"Paul Boersma: user interface, graphics, @printing, @@Intro|sound@, "
	"@@Intro 3. Spectral analysis|spectral analysis@, @@Intro 4. Pitch analysis|pitch analysis@, "
	"@@Intro 5. Formant analysis|formant analysis@, @@Intro 6. Intensity analysis|intensity analysis@, "
	"@@Intro 7. Annotation|annotation@, @@Intro 8. Manipulation|speech manipulation@, @@voice|voice report@, "
	"@@ExperimentMFC|listening experiments@, "
	"@@articulatory synthesis@, @@OT learning|optimality-theoretic learning@, "
	"tables, @formulas, @scripting, and adaptation of PortAudio, GLPK, and regular expressions.")
LIST_ITEM (U"David Weenink: "
	"@@feedforward neural networks@, @@principal component analysis@, @@multidimensional scaling@, @@discriminant analysis@, @LPC, "
	"@VowelEditor, "
	"and adaptation of GSL, LAPACK, fftpack, regular expressions, and Espeak.")
LIST_ITEM (U"Stefan de Konink and Franz Brauße: major help in port to GTK.")
LIST_ITEM (U"Tom Naughton: major help in port to Cocoa.")
LIST_ITEM (U"Erez Volk: adaptation of FLAC and MAD.")
LIST_ITEM (U"Ola Söder: @@kNN classifiers@, @@k-means clustering@.")
LIST_ITEM (U"Rafael Laboissière: adaptation of XIPA, audio bug fixes for Linux.")
LIST_ITEM (U"Darryl Purnell created the first version of audio for Praat for Linux.")
NORMAL (U"We included the following freely available software libraries in Praat (sometimes with adaptations):")
LIST_ITEM (U"XIPA: IPA font for Unix by Fukui Rei (GPL).")
LIST_ITEM (U"GSL: GNU Scientific Library by Gerard Jungman and Brian Gough (GPL 3 or later).")
LIST_ITEM (U"GLPK: GNU Linear Programming Kit by Andrew Makhorin (GPL 3 or later); "
	"contains AMD software by the same author (LGPL 2.1 or later).")
LIST_ITEM (U"PortAudio: Portable Audio Library by Ross Bencina, Phil Burk, Bjorn Roche, Dominic Mazzoni, Darren Gibbs "
	"(CC-BY-like license).")
LIST_ITEM (U"Espeak: text-to-speech synthesizer by Jonathan Duddington (GPL 3 or later).")
LIST_ITEM (U"MAD: MPEG Audio Decoder by Underbit Technologies (GPL 2 or later).")
LIST_ITEM (U"FLAC: Free Lossless Audio Codec by Josh Coalson (BSD 3-clause license).")
LIST_ITEM (U"fftpack: public domain Fourier transforms by Paul Swarztrauber and Christopher Montgomery.")
LIST_ITEM (U"LAPACK: public domain numeric algorithms by Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., "
	"Courant Institute, Argonne National Lab, and Rice University.")
LIST_ITEM (U"Regular expressions by Henry Spencer, Mark Edel, Christopher Conrad, Eddy De Greef (GPL 2 or later).")
NORMAL (U"For their financial support during the development of Praat:")
LIST_ITEM (U"Netherlands Organization for Scientific Research (NWO) (1996–1999).")
LIST_ITEM (U"Nederlandse Taalunie (2006–2008).")
LIST_ITEM (U"Talkbank project, Carnegie Mellon / Linguistic Data Consortium (2002–2003).")
LIST_ITEM (U"Stichting Spraaktechnologie (2014–2016).")
LIST_ITEM (U"Spoken Dutch Corpus (CGN) (1999–2001).")
LIST_ITEM (U"Laboratorium Experimentele OtoRhinoLaryngologie, KU Leuven.")
LIST_ITEM (U"DFG-Projekt Dialektintonation, Universität Freiburg.")
LIST_ITEM (U"Department of Linguistics and Phonetics, Lund University.")
LIST_ITEM (U"Centre for Cognitive Neuroscience, University of Turku.")
LIST_ITEM (U"Linguistics Department, University of Joensuu.")
LIST_ITEM (U"Laboratoire de Sciences Cognitives et Psycholinguistique, Paris.")
LIST_ITEM (U"Department of Linguistics, Northwestern University.")
LIST_ITEM (U"Department of Finnish and General Linguistics, University of Tampere.")
LIST_ITEM (U"Institute for Language and Speech Processing, Paradissos Amaroussiou.")
LIST_ITEM (U"Jörg Jescheniak, Universität Leipzig.")
LIST_ITEM (U"The Linguistics Teaching Laboratory, Ohio State University.")
LIST_ITEM (U"Linguistics & Cognitive Science, Dartmouth College, Hanover NH.")
LIST_ITEM (U"Cornell Phonetics Lab, Ithaca NY.")
NORMAL (U"Finally we thank:")
LIST_ITEM (U"Ton Wempe and Dirk Jan Vet, for technical support and advice.")
LIST_ITEM (U"Daniel Hirst, for managing the Praat Discussion list.")
LIST_ITEM (U"Rafael Laboissière and Andreas Tille, for maintaining the Debian package.")
LIST_ITEM (U"Jason Bacon, for maintaining the FreeBSD port.")
LIST_ITEM (U"José Joaquín Atria and Ingmar Steiner, for setting up the source-code repository on GitHub.")
LIST_ITEM (U"Hundreds of Praat users, for sending suggestions and notifying us of problems and thus helping us to improve Praat.")
MAN_END

MAN_BEGIN (U"Praat menu", U"ppgb", 20161227)
INTRO (U"The first menu in the @@Object window@. "
	"In macOS, this menu is in the main menu bar along the top of the screen.")
MAN_END

MAN_BEGIN (U"Copy...", U"ppgb", 20111018)
INTRO (U"One of the fixed buttons in the @@Object window@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one object of any type.")
ENTRY (U"Behaviour")
NORMAL (U"The Object window copies the selected object, and all the data it contains, "
	"to a new object, which will appear at the bottom of the List of Objects.")
ENTRY (U"Example")
NORMAL (U"If you select \"Sound hallo\" and click `Copy...', "
	"a command window will appear, which prompts you for a name; "
	"after you click OK, a new object will appear in the list, bearing that name.")
MAN_END

MAN_BEGIN (U"Draw menu", U"ppgb", 20010417)
INTRO (U"A menu that occurs in the @@Dynamic menu@ for many objects.")
NORMAL (U"This menu contains commands for drawing the object to the @@Picture window@, "
	"which will allow you to print the drawing or to copy it to your word processor.")
MAN_END

MAN_BEGIN (U"Dynamic menu", U"ppgb", 20110131)
INTRO (U"A column of buttons in the right-hand part of the @@Object window@, "
	"plus the #Save menu in the Object window.")
NORMAL (U"If you select one or more @objects in the list, "
	"the possible actions that you can perform with the selected objects "
	"will appear in the dynamic menu. "
	"These actions can include viewing & editing, saving, drawing, "
	"conversions to other types (including analysis and synthesis), and more.")
ENTRY (U"Example of analysis:")
NORMAL (U"Record a Sound, select it, and click on ##To Pitch...#. "
	"This will create a new Pitch object and put it in the list of objects. "
	"You can then edit, write, and draw this Pitch object.")
ENTRY (U"Example of synthesis:")
NORMAL (U"Create a #Speaker, create and edit an #Artword, and click on ##To Sound...#.")
MAN_END

MAN_BEGIN (U"View & Edit", U"ppgb", 20110128)
INTRO (U"A command in the @@Dynamic menu@ of several types of @objects.")
NORMAL (U"This command puts an @@Editors|editor@ window on the screen, which shows the contents of the selected object. "
	"This window will allow your to view and modify the contents of this object.")
MAN_END

MAN_BEGIN (U"Extract visible formant contour", U"ppgb", 20030316)
INTRO (U"One of the commands in the Formant menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 5. Formant analysis@")
MAN_END

MAN_BEGIN (U"Extract visible intensity contour", U"ppgb", 20030316)
INTRO (U"One of the commands in the Intensity menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 6. Intensity analysis@")
MAN_END

MAN_BEGIN (U"Extract visible pitch contour", U"ppgb", 20030316)
INTRO (U"One of the commands in the Pitch menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 4. Pitch analysis@")
MAN_END

MAN_BEGIN (U"Extract visible spectrogram", U"ppgb", 20030316)
INTRO (U"One of the commands in the Spectrogram menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 3. Spectral analysis@")
MAN_END

MAN_BEGIN (U"FAQ (Frequently Asked Questions)", U"ppgb", 20071210)
LIST_ITEM (U"@@FAQ: How to cite Praat")
LIST_ITEM (U"@Unicode")
LIST_ITEM (U"@@FAQ: Formant analysis")
LIST_ITEM (U"@@FAQ: Pitch analysis")
LIST_ITEM (U"@@FAQ: Spectrograms")
LIST_ITEM (U"@@FAQ: Scripts")
/* Hardware */
/* Pitch: octave errors */
MAN_END

MAN_BEGIN (U"FAQ: Formant analysis", U"ppgb", 20030916)
NORMAL (U"#Problem: I get different formant values if I choose to analyse 3 formants "
	"than if I choose to analyse 4 formants.")
NORMAL (U"Solution: the \"number of formants\" in formant analysis determines the "
	"number of peaks with which the %entire spectrum is modelled. For an average "
	"female voice, you should choose to analyse 5 formants in the region up to 5500 Hz, "
	"even if you are interested only in the first three formants.")
NORMAL (U"")
NORMAL (U"#Problem: I often get only 1 formant in a region where I see clearly 2 formants "
	"in the spectrogram.")
NORMAL (U"This occurs mainly in back vowels (F1 and F2 close together) for male voices, "
	"if the \"maximum formant\" is set to the standard of 5500 Hz, which is appropriate "
	"for female voices. Set the \"maximum formant\" down to 5000 Hz. "
	"No, Praat comes without a guarantee: the formant analysis is based on LPC, "
	"and this comes with several assumptions as to what a speech spectrum is like.")
NORMAL (U"")
NORMAL (U"#Question: what algorithm is used for formant analysis?")
NORMAL (U"Answer: see @@Sound: To Formant (burg)...@.")
MAN_END

MAN_BEGIN (U"FAQ: How to cite Praat", U"ppgb", 20140826)
NORMAL (U"#Question: how do I cite Praat in my articles?")
NORMAL (U"Answer: nowadays most journals allow you to cite computer programs and web sites. "
	"The style approved by the American Psychological Association, "
	"and therefore by many journals, is like the following "
	"(change the dates and version number as needed):")
#define xstr(s) str(s)
#define str(s) #s
NORMAL (U"Boersma, Paul & Weenink, David (" xstr(PRAAT_YEAR) "). "
	"Praat: doing phonetics by computer [Computer program]. "
	"Version " xstr(PRAAT_VERSION_STR) ", retrieved " xstr(PRAAT_DAY) " " xstr(PRAAT_MONTH) " " xstr(PRAAT_YEAR) " from http://www.praat.org/")
NORMAL (U"If the journal does not allow you to cite a web site, then try:")
NORMAL (U"Boersma, Paul (2001). Praat, a system for doing phonetics by computer. "
	"%%Glot International% ##5:9/10#, 341-345.")
NORMAL (U"This paper can be downloaded from Boersma's website.")
MAN_END

MAN_BEGIN (U"FAQ: Pitch analysis", U"ppgb", 20060913)
NORMAL (U"#Question: what algorithm is used for pitch analysis?")
NORMAL (U"Answer: see @@Sound: To Pitch (ac)...@. The 1993 article is downloadable from "
	"http://www.fon.hum.uva.nl/paul/")
NORMAL (U"#Question: why does Praat consider my sound voiceless while I hear it as voiced?")
NORMAL (U"There are at least five possibilities. Most of them can be checked by zooming in on the @waveform.")
NORMAL (U"The first possibility is that the pitch has fallen below the @@pitch floor@. For instance, "
	"your pitch floor could be 75 Hz but the English speaker produces creak at the end of the utterance. "
	"Or your pitch floor could be 75 Hz but the Chinese speaker is in the middle of a third tone. "
	"If this happens, it may help to lower the pitch floor to e.g. 40 Hz (@@Pitch settings...@), "
	"although that may also smooth the pitch curve too much in other places.")
NORMAL (U"The second possibility is that the pitch has moved too fast. This could happen at the end of a Chinese fourth tone, "
	"which drops very fast. If this happens, it may help to use the \"optimize for voice analysis\" setting, "
	"(@@Pitch settings...@), although Praat may then hallucinate pitches in other places that you would prefer to consider voiceless.")
NORMAL (U"The third possibility is that the periods are very irregular, as in some pathological voices. "
	"If you want to see a pitch in those cases, it may help to use the \"optimize for voice analysis\" setting "
	"(@@Pitch settings...@). Or it may help to lower the \"voicing threshold\" setting (@@Advanced pitch settings...@) "
	"to 0.25 (instead of the standard 0.45) or so.")
NORMAL (U"The fourth possibility is that there is a lot of background noise, as in a recording on a busy street. "
	"In such a case, it may help to lower the \"voicing threshold\" setting (@@Advanced pitch settings...@) "
	"to 0.25 (instead of the standard 0.45) or so. The disadvantage of lowering this setting is that for non-noisy "
	"recordings, Praat will become too eager to find voicing in some places that you would prefer to consider voiceless; "
	"so make sure to set it back to 0.45 once you have finished analysing the noisy recordings.")
NORMAL (U"The fifth possibility is that the part analysed as voiceless is much less loud than the rest of the sound, "
	"or that the sound contains a loud noise elsewhere. This can be checked by zooming in on the part analysed as voiceless: "
	"if Praat suddenly considers it as voiced, this is a sign that this part is much quieter than the rest. "
	"To make Praat analyse this part as voiced, you can lower the \"silence threshold\" setting to 0.01 "
	"(instead of the standard 0.03) or so. The disadvantage of lowering this setting is that Praat may start to consider "
	"some distant background sounds (and quiet echos, for instance) as voiced.")
NORMAL (U"#Question: why do I get different results for the maximum pitch if...?")
NORMAL (U"If you select a Sound and choose @@Sound: To Pitch...@, the time step will usually "
	"be 0.01 seconds. The resulting @Pitch object will have values for times that are "
	"0.01 seconds apart. If you then click Info or choose ##Get maximum pitch# from the #Query menu, "
	"the result is based on those time points. By contrast, if you choose ##Get maximum pitch# "
	"from the #Pitch menu in the SoundEditor window, the result will be based on the visible points, "
	"of which there tend to be a hundred in the visible window. These different time spacings will "
	"lead to slightly different pitch contours.")
NORMAL (U"If you choose ##Move cursor to maximum pitch#, then choose ##Get pitch# from the "
	"#%Pitch menu, the result will be different again. This is because ##Get maximum pitch# "
	"can do a parabolic interpolation around the maximum, whereas ##Get pitch#, not realizing "
	"that the cursor is at a maximum, does a stupid linear interpolation, which tends to lead to "
	"lower values.")
MAN_END

MAN_BEGIN (U"FAQ: Scripts", U"ppgb", 20140120)
NORMAL (U"#Question: how do I do something to all the files in a directory?")
NORMAL (U"Answer: look at @@Create Strings as file list...@.")
NORMAL (U"")
NORMAL (U"#Question: why doesn't the editor window react to my commands?")
NORMAL (U"Your commands are probably something like:")
CODE (U"Read from file: \"hello.wav\"")
CODE (U"View & Edit")
CODE (U"Zoom: 0.3, 0.5")
NORMAL (U"Answer: Praat doesn't know it has to send the #Zoom command to the editor "
	"window called ##14. Sound hello#. There could be several Sound editor windows on your "
	"screen. According to @@Scripting 7.1. Scripting an editor from a shell script@, "
	"you will have to say this explicitly:")
CODE (U"Read from file: \"hello.wav\"")
CODE (U"View & Edit")
CODE (U"editor: \"Sound hello\"")
CODE (U"Zoom: 0.3, 0.5")
NORMAL (U"")
NORMAL (U"#Problem: a line like \"Number = 1\" does not work.")
NORMAL (U"Solution: names of variables should start with a lower-case letter.")
NORMAL (U"")
NORMAL (U"#Question: why do names of variables have to start with a lower-case letter? "
	"I would like to do things like \"F0 = Get mean pitch\".")
NORMAL (U"Answer (using the shorthand script syntax): Praat scripts combine button commands with things that only occur "
	"in scripts. Button commands always start with a capital letter, e.g. \"Play\". "
	"Script commands always start with lower case, e.g. \"echo Hello\". "
	"A minimal pair is \"select\", which simulates a mouse click in the object list, "
	"versus \"Select...\", which sets the selection in editor windows. Variable names "
	"that start with a capital letter would be rather ambiguous in assignments, "
	"as in \"x = Get\", where \"Get\" would be a variable, versus \"x = Get mean\", "
	"where \"Get mean\" is a button command. To prevent this, Praat enforces "
	"a rigorous lower-case/upper-case distinction.")
NORMAL (U"")
NORMAL (U"#Question: how do I convert a number into a string?")
NORMAL (U"Answer: a\\$  = string\\$  (a)")
NORMAL (U"#Question: how do I convert a string into a number?")
NORMAL (U"Answer: a = number (a\\$ )")
MAN_END

MAN_BEGIN (U"FAQ: Spectrograms", U"ppgb", 20030916)
NORMAL (U"#Problem: the background is grey instead of white (too little contrast)")
NORMAL (U"Solution: reduce the \"dynamic range\" in the spectrogram settings. The standard value is 50 dB, "
	"which is fine for detecting small things like plosive voicing in well recorded speech. "
	"For gross features like vowel formants, or for noisy speech, you may want to change the dynamic range "
	"to 40 or even 30 dB.")
MAN_END

MAN_BEGIN (U"File menu", U"ppgb", 20021204)
INTRO (U"One of the menus in all @editors, in the @manual, and in the @@Picture window@.")
MAN_END

MAN_BEGIN (U"Filtering", U"ppgb", 20100324)
INTRO (U"This tutorial describes the use of filtering techniques in Praat. "
	"It assumes you are familiar with the @Intro.")
ENTRY (U"Frequency-domain filtering")
NORMAL (U"Modern computer techniques make possible an especially simple batch filtering method: "
	"multiplying the complex spectrum in the frequency domain by any real-valued filter function. "
	"This leads to a zero phase shift for each frequency component. The impulse response is symmetric "
	"in the time domain, which also means that the filter is %acausal: the filtered signal will show components "
	"before they start in the original.")
LIST_ITEM (U"• @@Spectrum: Filter (pass Hann band)...@")
LIST_ITEM (U"• @@Spectrum: Filter (stop Hann band)...@")
LIST_ITEM (U"• @@Sound: Filter (pass Hann band)...@")
LIST_ITEM (U"• @@Sound: Filter (stop Hann band)...@")
LIST_ITEM (U"• @@Sound: Filter (formula)...@")
NORMAL (U"Spectro-temporal:")
LIST_ITEM (U"• @@band filtering in the frequency domain@")
ENTRY (U"Fast time-domain filtering")
NORMAL (U"Some very fast Infinite Impulse Response (IIR) filters can be defined in the time domain. "
	"These include recursive all-pole filters and pre-emphasis. These filters are causal but have non-zero phase shifts. "
	"There are versions that create new Sound objects:")
LIST_ITEM (U"• @@Sound: Filter (one formant)...@")
LIST_ITEM (U"• @@Sound: Filter (pre-emphasis)...@")
LIST_ITEM (U"• @@Sound: Filter (de-emphasis)...@")
NORMAL (U"And there are in-line versions, which modify the existing Sound objects:")
LIST_ITEM (U"• @@Sound: Filter with one formant (in-line)...@")
LIST_ITEM (U"• @@Sound: Pre-emphasize (in-line)...@")
LIST_ITEM (U"• @@Sound: De-emphasize (in-line)...@")
ENTRY (U"Convolution")
NORMAL (U"A Finite Impulse Response (FIR) filter can be described as a sampled sound. "
	"Filtering with such a filter amounts to a %#convolution of the original sound and the filter:")
LIST_ITEM (U"• @@Sounds: Convolve...@")
ENTRY (U"Described elsewhere")
NORMAL (U"Described in the @@Source-filter synthesis@ tutorial:")
LIST_ITEM (U"• @@Sound & Formant: Filter@")
LIST_ITEM (U"• @@Sound & FormantGrid: Filter@")
LIST_ITEM (U"• @@LPC & Sound: Filter...@")
LIST_ITEM (U"• @@LPC & Sound: Filter (inverse)@")
MAN_END

MAN_BEGIN (U"Formants & LPC menu", U"ppgb", 20011107)
INTRO (U"A menu that occurs in the @@Dynamic menu@ for a @Sound.")
NORMAL (U"This menu contains commands for analysing the formant contours of the selected Sound:")
LIST_ITEM (U"@@Sound: To Formant (burg)...")
LIST_ITEM (U"@@Sound: To Formant (keep all)...")
LIST_ITEM (U"@@Sound: To Formant (sl)...")
LIST_ITEM (U"@@Sound: To LPC (autocorrelation)...")
LIST_ITEM (U"@@Sound: To LPC (covariance)...")
LIST_ITEM (U"@@Sound: To LPC (burg)...")
LIST_ITEM (U"@@Sound: To LPC (marple)...")
LIST_ITEM (U"@@Sound: To MFCC...")
MAN_END

MAN_BEGIN (U"Get first formant", U"ppgb", 20011107)
INTRO (U"One of the commands in the @@Query menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Get pitch", U"ppgb", 20010417)
INTRO (U"One of the commands in the @@Query menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Get second formant", U"ppgb", 20011107)
INTRO (U"One of the commands in the @@Query menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Goodies", U"ppgb", 20050822)
INTRO (U"The title of a submenu of the @@Praat menu@.")
MAN_END

MAN_BEGIN (U"Info", U"ppgb", 19980101)
INTRO (U"One of the fixed buttons in the @@Object window@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after choosing one object.")
ENTRY (U"Purpose")
NORMAL (U"To get some information about the selected object.")
ENTRY (U"Behaviour")
NORMAL (U"The information will appear in the @@Info window@.")
MAN_END

MAN_BEGIN (U"Info window", U"ppgb", 20030528)
INTRO (U"A text window into which many query commands write their answers.")
NORMAL (U"You can select text from this window and copy it to other places.")
NORMAL (U"In a @@Praat script@, you can bypass the Info window by having a query command "
	"writing directly into a script variable.")
NORMAL (U"Apart from the @Info command, which writes general information about the selected object, "
	"the following more specific commands also write into the Info window:")
MAN_END

MAN_BEGIN (U"Inspect", U"ppgb", 19960904)
INTRO (U"One of the fixed buttons in the @@Object window@.")
NORMAL (U"You can use this command after selecting one object in the list.")
NORMAL (U"The contents of the selected object will become visible in a Data Editor. "
	"You can then view and change the data in the object, "
	"but beware: changing the data directly in this way may render them inconsistent.")
NORMAL (U"Changes that you make to the data with another Editor (e.g., a SoundEditor), "
	"or with the commands under #%Modify, "
	"are immediately reflected in the top-level Data Editor; "
	"any subeditors are destroyed, however, because they may now refer to invalid data.")
NORMAL (U"Changes that you make to the data with a Data Editor, "
	"are immediately reflected in any open type-specific Editors (e.g., a SoundEditor).")
MAN_END

MAN_BEGIN (U"Intro", U"ppgb", 20110131)
INTRO (U"This is an introductory tutorial to Praat, a computer program "
	"with which you can analyse, synthesize, and manipulate speech, "
	"and create high-quality pictures for your articles and thesis. "
	"You are advised to work through all of this tutorial.")
NORMAL (U"You can read this tutorial sequentially with the help of the \"1 >\" and \"< 1\" buttons, "
	"or go to the desired information by clicking on the blue links.")
LIST_ITEM (U"@@Intro 1. How to get a sound@: "
	"@@Intro 1.1. Recording a sound|record@, "
	"@@Intro 1.2. Reading a sound from disk|read@, "
	"@@Intro 1.3. Creating a sound from a formula|formula@.")
LIST_ITEM (U"@@Intro 2. What to do with a sound@: "
	"@@Intro 2.1. Saving a sound to disk|write@, "
	"@@Intro 2.2. Viewing and editing a sound|view@.")
LIST_ITEM (U"@@Intro 3. Spectral analysis")
LIST_ITEM1 (U"spectrograms: @@Intro 3.1. Viewing a spectrogram|view@, "
	"@@Intro 3.2. Configuring the spectrogram|configure@, "
	"@@Intro 3.3. Querying the spectrogram|query@, "
	"@@Intro 3.4. Printing the spectrogram|print@, "
	"@@Intro 3.5. The Spectrogram object|the Spectrogram object@.")
LIST_ITEM1 (U"spectral slices: @@Intro 3.6. Viewing a spectral slice|view@, "
	"@@Intro 3.7. Configuring the spectral slice|configure@, "
	"@@Intro 3.8. The Spectrum object|the Spectrum object@.")
LIST_ITEM (U"@@Intro 4. Pitch analysis")
LIST_ITEM1 (U"pitch contours: @@Intro 4.1. Viewing a pitch contour|view@, "
	"@@Intro 4.2. Configuring the pitch contour|configure@, "
	"@@Intro 4.3. Querying the pitch contour|query@, "
	"@@Intro 4.4. Printing the pitch contour|print@, "
	"@@Intro 4.5. The Pitch object|the Pitch object@.")
LIST_ITEM (U"@@Intro 5. Formant analysis")
LIST_ITEM1 (U"formant contours: @@Intro 5.1. Viewing formant contours|view@, "
	"@@Intro 5.2. Configuring the formant contours|configure@, "
	"@@Intro 5.3. Querying the formant contours|query@, "
	"@@Intro 5.4. The Formant object|the Formant object@.")
LIST_ITEM (U"@@Intro 6. Intensity analysis")
LIST_ITEM1 (U"intensity contours: @@Intro 6.1. Viewing an intensity contour|view@, "
	"@@Intro 6.2. Configuring the intensity contour|configure@, "
	"@@Intro 6.3. Querying the intensity contour|query@, "
	"@@Intro 6.4. The Intensity object|the Intensity object@.")
LIST_ITEM (U"@@Intro 7. Annotation")
LIST_ITEM (U"@@Intro 8. Manipulation@: of "
	"@@Intro 8.1. Manipulation of pitch|pitch@, "
	"@@Intro 8.2. Manipulation of duration|duration@, "
	"@@Intro 8.3. Manipulation of intensity|intensity@, "
	"@@Intro 8.4. Manipulation of formants|formants@.")
NORMAL (U"There are also more specialized tutorials:")
LIST_ITEM (U"Phonetics:")
LIST_ITEM1 (U"• Voice analysis (jitter, shimmer, noise): @Voice")
LIST_ITEM1 (U"• Listening experiments: @@ExperimentMFC@")
LIST_ITEM1 (U"• @@Sound files@")
LIST_ITEM1 (U"• @@Filtering@")
LIST_ITEM1 (U"• @@Source-filter synthesis@")
LIST_ITEM1 (U"• @@Articulatory synthesis@")
LIST_ITEM (U"Learning:")
LIST_ITEM1 (U"• @@Feedforward neural networks@")
LIST_ITEM1 (U"• @@OT learning@")
LIST_ITEM (U"Statistics:")
LIST_ITEM1 (U"• @@Principal component analysis@")
LIST_ITEM1 (U"• @@Multidimensional scaling@")
LIST_ITEM1 (U"• @@Discriminant analysis@")
LIST_ITEM (U"General:")
LIST_ITEM1 (U"• @@Printing@")
LIST_ITEM1 (U"• @@Scripting@")
LIST_ITEM1 (U"• @@Demo window@")
ENTRY (U"The authors")
NORMAL (U"The Praat program was created by Paul Boersma and David Weenink of "
	"the Institute of Phonetics Sciences of the University of Amsterdam. "
	"Home page: ##http://www.praat.org# or ##http://www.fon.hum.uva.nl/praat/#.")
NORMAL (U"For questions and suggestions, mail to the Praat discussion list, "
	"which is reachable from the Praat home page, or directly to ##paul.boersma\\@ uva.nl#.")
MAN_END

MAN_BEGIN (U"Intro 1. How to get a sound", U"ppgb", 20021212)
INTRO (U"Most of the things most people do with Praat start with a sound. "
	"There are at least three ways to get a sound:")
LIST_ITEM (U"@@Intro 1.1. Recording a sound")
LIST_ITEM (U"@@Intro 1.2. Reading a sound from disk")
LIST_ITEM (U"@@Intro 1.3. Creating a sound from a formula")
MAN_END

MAN_BEGIN (U"Intro 1.1. Recording a sound", U"ppgb", 20110128)
#ifdef macintosh
	INTRO (U"To record a speech sound into Praat, you need a computer with a microphone.")
	NORMAL (U"To record from the microphone, perform the following steps:")
#else
	INTRO (U"To record a speech sound into Praat, you need a computer with a microphone. "
		"If you do not have a microphone, try to record from an audio CD instead.")
	NORMAL (U"To record from the microphone (or the CD), perform the following steps:")
#endif
LIST_ITEM (U"1. Choose @@Record mono Sound...@ from the @@New menu@ in the @@Object window@. "
	"A @SoundRecorder window will appear on your screen.")
#if defined (_WIN32)
	LIST_ITEM (U"2. Choose the appropriate input device, namely the microphone, by the following steps. "
		"If there is a small loudspeaker symbol in the Windows Start bar, double click it and you will see the %playing mixer. "
		"If there is no loudspeaker symbol, go to ##Control Panels#, then ##Sounds and Audio Devices#, then #Volume, then #Advanced, "
		"and you will finally see the playing mixer. "
		"Once you see the Windows playing mixer, choose #Properties from the #Option menu, "
		"then click #Recording, then #OK. You now see the %recording mixer, where you can select the microphone "
		"(if you do not like this complicated operation, try Praat on Macintosh or Linux instead of Windows).")
#elif defined (macintosh)
	LIST_ITEM (U"2. In the SoundRecorder window, choose the appropriate input device, e.g. choose ##Internal microphone#.")
#else
	LIST_ITEM (U"2. In the SoundRecorder window, choose the appropriate input device, i.e. choose #Microphone (or #CD, or #Line).")
#endif
#ifdef macintosh
	LIST_ITEM (U"3. Use the #Record and #Stop buttons to record a few seconds of your speech.")
#else
	LIST_ITEM (U"3. Use the #Record and #Stop buttons to record a few seconds of your speech "
		"(or a few seconds of music from your playing CD).")
#endif
LIST_ITEM (U"4. Use the #Play button to hear what you have recorded.")
LIST_ITEM (U"5. Repeat steps 3 and 4 until you are satisfied with your recording.")
LIST_ITEM (U"6. Click the ##Save to list# button. Your recording will now appear in the Object window, "
	"where it will be called \"Sound sound\".")
LIST_ITEM (U"7. You can now close the SoundRecorder window.")
LIST_ITEM (U"8. When you saved your sound to the Object window, some buttons appeared in that window. "
	"These buttons show you what you can do with the sound. Try the #Play and @@View & Edit@ buttons.")
NORMAL (U"For more information on recording, see the @SoundRecorder manual page.")
MAN_END

MAN_BEGIN (U"Intro 1.2. Reading a sound from disk", U"ppgb", 20041126)
INTRO (U"Apart from recording a new sound from a microphone, you could read an existing sound file from your disk.")
NORMAL (U"With @@Read from file...@ from the @@Open menu@, "
	"Praat will be able to read most standard types of sound files, e.g. WAV files. "
	"They will appear as @Sound objects in the Object window. For instance, if you open the file ##hello.wav#, "
	"an object called \"Sound hello\" will appear in the list.")
NORMAL (U"If you do not have a sound file on your disk, you can download a WAV file (or so) from the Internet, "
	"then read that file into Praat with ##Read from file...#.")
MAN_END

MAN_BEGIN (U"Intro 1.3. Creating a sound from a formula", U"ppgb", 20070225)
INTRO (U"If you have no microphone, no sound files on disk, and no access to the Internet, "
	"you could still create a sound with @@Create Sound from formula...@ from the @@New menu@.")
MAN_END

MAN_BEGIN (U"Intro 2. What to do with a sound", U"ppgb", 20110131)
INTRO (U"As soon as you have a @Sound in the @@List of Objects@, "
	"the buttons in the @@Dynamic menu@ (the right-hand part of the @@Object window@) "
	"will show you what you can do with it.")
LIST_ITEM (U"@@Intro 2.1. Saving a sound to disk")
LIST_ITEM (U"@@Intro 2.2. Viewing and editing a sound")
MAN_END

MAN_BEGIN (U"Intro 2.1. Saving a sound to disk", U"ppgb", 20110131)
INTRO (U"There are several ways to write a sound to disk.")
NORMAL (U"First, the @@File menu@ of the @SoundRecorder window contains commands to save the left "
	"channel, the right channel, or both channels of the recorded sound to any of four standard types "
	"of sound files (WAV, AIFC, NeXT/Sun, NIST). These four file types are all equally good for Praat: "
	"Praat will handle them equally well on every computer. The first three of these types will "
	"also be recognized by nearly all other sound-playing programs.")
NORMAL (U"Then, once you have a @Sound object in the @@List of Objects@, "
	"you can save it in several formats with the commands in the @@Save menu@. "
	"Again, the WAV, AIFF, AIFC, NeXT/Sun, and NIST formats are equally fine.")
NORMAL (U"For more information, see the @@Sound files@ tutorial.")
MAN_END

MAN_BEGIN (U"Intro 2.2. Viewing and editing a sound", U"ppgb", 20110212)
NORMAL (U"To see the wave form of a @Sound that is in the list of objects, "
	"select that Sound and click @@View & Edit@. A @SoundEditor window will appear on your screen. "
	"You see a waveform (or two waveforms, if you have a stereo sound) and probably some \"analyses\" below it. "
	"You can zoom in and scroll to see the various parts of the sound in detail. "
	"You can select a part of the sound by dragging with the mouse. "
	"To play a part of the sound, click on any of the rectangles below it. "
	"To move a selected part of the sound to another location, use #Cut and #Paste from the #Edit menu. "
	"You can open sound windows for more than one sound, and then cut, copy, and paste between the sounds, "
	"just as you are used to do with text and pictures in word processing programs.")
NORMAL (U"If your sound file is longer than a couple of minutes, "
	"or if you want to see and listen to both channels of a stereo sound, "
	"you may prefer to open it with @@Open long sound file...@. "
	"This puts a @LongSound object into the list. In this way, most of the sound will stay in the file on disk, "
	"and at most 60 seconds will be read into memory each time you play or view a part of it. "
	"To change these 60 seconds to something else, e.g. 500 seconds, choose ##LongSound prefs...# from the #Preferences submenu.")
MAN_END

MAN_BEGIN (U"Intro 3. Spectral analysis", U"ppgb", 20070905)
INTRO (U"This section describes how you can analyse the spectral content of an existing sound. "
	"You will learn how to use %spectrograms and %%spectral slices%.")
LIST_ITEM (U"@@Intro 3.1. Viewing a spectrogram")
LIST_ITEM (U"@@Intro 3.2. Configuring the spectrogram")
LIST_ITEM (U"@@Intro 3.3. Querying the spectrogram")
LIST_ITEM (U"@@Intro 3.4. Printing the spectrogram")
LIST_ITEM (U"@@Intro 3.5. The Spectrogram object")
LIST_ITEM (U"@@Intro 3.6. Viewing a spectral slice")
LIST_ITEM (U"@@Intro 3.7. Configuring the spectral slice")
LIST_ITEM (U"@@Intro 3.8. The Spectrum object")
MAN_END

MAN_BEGIN (U"Intro 3.1. Viewing a spectrogram", U"ppgb", 20110128)
INTRO (U"To see the spectral content of a sound as a function of time, "
	"select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor or @LongSoundEditor window will appear on your screen. "
	"In the entire bottom half of this window you will see a greyish image, which is called a %spectrogram. "
	"If you do not see it, choose @@Show spectrogram@ from the #Spectrogram menu.")
NORMAL (U"The spectrogram is a @@spectro-temporal representation@ of the sound. "
	"The horizontal direction of the spectrogram represents @time, the vertical direction represents @frequency. "
	"The time scale of the spectrogram is the same as that of the waveform, so the spectrogram reacts "
	"to your zooming and scrolling. "
	"To the left of the spectrogram, you see the frequency scale. The frequency at the bottom of the spectrogram "
	"is usually 0 Hz (hertz, cps, cycles per second), and a common value for the frequency at the top is 5000 Hz.")
NORMAL (U"Darker parts of the spectrogram mean higher energy densities, lighter parts mean lower energy densities. "
	"If the spectrogram has a dark area around a time of 1.2 seconds and a frequency of 4000 Hz, "
	"this means that the sound has lots of energy for those high frequencies at that time. "
	"For many examples of spectrograms of speech sounds, see the textbook by @@Ladefoged (2001)@ and "
	"the reference work by @@Ladefoged & Maddieson (1996)@.")
NORMAL (U"To see what time and frequency a certain part of the spectrogram is associated with, "
	"just click on the spectrogram and you will see the vertical time cursor showing the time above "
	"the waveform and the horizontal frequency cursor showing the frequency to the left of the spectrogram. "
	"This is one of the ways to find the %formant frequencies for vowels, or the main spectral peaks "
	"for fricatives.")
ENTRY (U"Hey, there are white vertical stripes at the edges!")
NORMAL (U"This is normal. Spectral analysis requires an %%analysis window% of a certain duration. "
	"For instance, if Praat wants to know the spectrum at 1.342 seconds, it needs to include information "
	"about the signal in a 10-milliseconds window around this time point, i.e., Praat will use "
	"signal information about all times between 1.337 and 1.347 seconds. At the very edges of the sound, "
	"this information is not available: "
	"if the sound runs from 0 to 1.8 seconds, no spectrum can be computed between 0 and 0.005 "
	"seconds or between 1.795 and 1.800 seconds. Hence the white stripes. If you do not see them "
	"immediately when you open the sound, zoom in on the beginning or end of the sound.")
NORMAL (U"When you zoom in on the middle of the sound (or anywhere not near the edges), the white stripes vanish. "
	"Suddenly you see only the time stretch between 0.45 and 1.35 seconds, for instance. "
	"But Praat did not forget what the signal looks like just outside the edges of this time window. "
	"To display a spectrogram from 0.45 to 1.35 seconds, Praat will use information from the wave form "
	"between 0.445 and 1.355 seconds, and if this is available, you will see no white stripes at the edges of the window.")
ENTRY (U"Hey, it changes when I scroll!")
NORMAL (U"This is normal as well, especially for long windows. If your visible time window is 20 seconds long, "
	"and the window takes up 1000 screen pixels horizontally, "
	"then you might think that every one-pixel-wide vertical line should represent the spectrum of 20 milliseconds of sound. "
	"But for reasons of computation speed, Praat will only show the spectrum of the part of the sound "
	"that lies around the centre of those 20 milliseconds, "
	"not the average or sum of all the spectra in those 20 milliseconds. "
	"This %undersampling of the underlying spectrogram is different from what happens in the drawing of the wave form, "
	"where a vertical black line connects the minimum and maximum amplitude of all the samples that fall inside a "
	"screen pixel. We cannot do something similar for spectrograms. And since scrolling goes by fixed time steps "
	"(namely, 5 percent of the duration of the visible window), rather than by a whole number of screen pixels, "
	"the centres of the pixels will fall in different parts of the spectrogram with each scroll. "
	"Hence the apparent changes. If your visible window is shorter than a couple of seconds, "
	"the scrolling spectrogram will appear much smoother.")
NORMAL (U"The darkness of the spectrogram will also change when you scroll, because the visible part with the most "
	"energy is defined as black. When a very energetic part of the signal scrolls out of view, the spectrogram "
	"will turn darker. The next section will describe a way to switch this off.")
MAN_END

MAN_BEGIN (U"Intro 3.2. Configuring the spectrogram", U"ppgb", 20110128)
NORMAL (U"With @@Spectrogram settings...@ from the #Spectrogram menu, "
	"you can determine how the spectrogram is computed and how it is displayed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (\"factory settings\"), which appear "
	"when you click ##Standards#.")
TAG (U"%%View range% (Hz)")
DEFINITION (U"the range of frequencies to display. The standard is 0 Hz at the bottom and 5000 Hz at the top. "
	"If this maximum frequency is higher than the Nyquist frequency of the Sound "
	"(which is half its sampling frequency), some values in the spectrogram will be zero, and the higher "
	"frequencies will be drawn in white. You can see this if you record a Sound at 44100 Hz and set the "
	"view range from 0 Hz to 25000 Hz.")
TAG (U"%%Window length")
DEFINITION (U"the duration of the analysis window. If this is 0.005 seconds (the standard), "
	"Praat uses for each frame the part of the sound that lies between 0.0025 seconds before "
	"and 0.0025 seconds after the centre of that frame "
	"(for Gaussian windows, Praat actually uses a bit more than that). "
	"The window length determines the %bandwidth of the spectral analysis, "
	"i.e. the width of the horizontal line in the spectrogram of a pure sine wave (see below). "
	"For a Gaussian window, the -3 dB bandwidth is 2*sqrt(6*ln(2))/(\\pi*%%Window length%), "
	"or 1.2982804 / %%Window length%. "
	"To get a `broad-band' spectrogram (bandwidth 260 Hz), keep the standard window length of 5 ms; "
	"to get a `narrow-band' spectrogram (bandwidth 43 Hz), set it to 30 ms (0.03 seconds). "
	"The other window shapes give slightly different values.")
TAG (U"%%Dynamic range% (dB)")
DEFINITION (U"All values that are more than %%Dynamic range% dB below the maximum (perhaps after dynamic compression, "
	"see @@Advanced spectrogram settings...@) "
	"will be drawn in white. Values in-between have appropriate shades of grey. Thus, if the highest "
	"peak in the spectrogram has a height of 30 dB/Hz, and the dynamic range is 50 dB (which is the standard value), "
	"then values below -20 dB/Hz will be drawn in white, and values between -20 dB/Hz and 30 dB/Hz will be drawn "
	"in various shades of grey.")
ENTRY (U"The bandwidth")
NORMAL (U"To see how the window length influences the bandwidth, "
	"first create a 1000-Hz sine wave with @@Create Sound from formula...@ "
	"by typing $$1/2 * sin (2*pi*1000*x)$ as the formula, then click ##View & Edit#. "
	"The spectrogram will show a horizontal black line. "
	"You can now vary the window length in the spectrogram settings and see how the thickness "
	"of the lines varies. The line gets thinner if you raise the window length. "
	"Apparently, if the analysis window comprises more periods of the wave, "
	"the spectrogram can tell us the frequency of the wave with greater precision.")
NORMAL (U"To see this more precisely, create a sum of two sine waves, with frequencies of 1000 and 1200 Hz. "
	"the formula is $$1/4 * sin (2*pi*1000*x) + 1/4 * sin (2*pi*1200*x)$. In the editor, you will see "
	"a single thick band if the analysis window is short (5 ms), and two separate bands if the analysis "
	"window is long (30 ms). Apparently, the frequency resolution gets better with longer analysis windows.")
NORMAL (U"So why don't we always use long analysis windows? The answer is that their time resolution is poor. "
	"To see this, create a sound that consists of two sine waves and two short clicks. The formula is "
	"$$0.02*(sin(2*pi*1000*x)+sin(2*pi*1200*x)) + (col=10000)+(col=10200)$. "
	"If you view this sound, you can see that the two clicks will overlap "
	"in time if the analysis window is long, and that the sine waves overlap in frequency if the "
	"analysis window is short. Apparently, there is a trade-off between time resolution and "
	"frequency resolution. One cannot know both the time and the frequency with great precision.")
ENTRY (U"Advanced settings")
NORMAL (U"The Spectrum menu also contains @@Advanced spectrogram settings...@.")
MAN_END

MAN_BEGIN (U"Advanced spectrogram settings...", U"ppgb", 20120531)
ENTRY (U"Optimization")
TAG (U"%%Number of time steps%")
DEFINITION (U"the maximum number of points along the time window for which Praat has to compute "
	"the spectrum. If your screen is not wider than 1200 pixels, then the standard of 1000 is "
	"appropriate, since there is no point in computing more than one spectrum per one-pixel-wide vertical line. "
	"If you have a really wide screen, you may see improvement if you raise this number to 1500.")
TAG (U"%%Number of frequency steps%")
DEFINITION (U"the maximum number of points along the frequency axis for which Praat has to compute "
	"the spectrum. If your screen is not taller than 768 pixels, then the standard of 250 is "
	"appropriate, since there is no point in computing more than one spectrum per one-pixel-height horizontal line. "
	"If you have a really tall screen, you may see improvement if you raise this number.")
NORMAL (U"For purposes of computation speed, Praat may decide to change the time step and the frequency step. "
	"This is because the time step never needs to be smaller than 1/(8\\Vr\\pi) of the window length, "
	"and the frequency step never needs to be smaller than (\\Vr\\pi)/8 of the inverse of the window length. "
	"For instance, if the window length is 5 ms, "
	"the actual time step will never be less than 5/(8\\Vr\\pi) = 0.353 ms, "
	"and the actual frequency step will never be less than (\\Vr\\pi)/8/0.005 = 44.31 Hz.")
ENTRY (U"Spectrogram analysis settings")
TAG (U"%%Method")
DEFINITION (U"there is currently only one method available in this window for computing a spectrum from "
	"a sound: the Fourier transform.")
TAG (U"%%Window shape")
DEFINITION (U"the shape of the analysis window. To compute the spectrum at, say, 3.850 seconds, "
	"samples that lie close to 3.850 seconds are given more weight than samples further away. "
	"The relative extent to which each sample contributes to the spectrum is given by the window shape. "
	"You can choose from: Gaussian, Square (none, rectangular), Hamming (raised sine-squared), "
	"Bartlett (triangular), Welch (parabolic), and Hanning (sine-squared). "
	"The Gaussian window is superior, as it gives no %sidelobes in your spectrogram (see below); "
	"it analyzes a factor of 2 slower than the other window shapes, "
	"because the analysis is actually performed on twice as many samples per frame.")
ENTRY (U"Sidelobes; anybody wants to win a cake?")
NORMAL (U"The Gaussian window is the only shape that we can consider seriously as a candidate for "
	"the analysis window. To see this, create a 1000-Hz sine wave with @@Create Sound from formula...@ "
	"by typing $$1/2 * sin (2*pi*1000*x)$ as the formula, then click ##View & Edit#. "
	"If the window shape is Gaussian, the spectrogram will show a horizontal black line. "
	"If the window shape is anything else, the spectrogram will show many horizontal grey lines (%sidelobes), "
	"which do not represent anything that is available in the signal. They are artifacts of the "
	"window shapes.")
NORMAL (U"We include these other window shapes only for pedagogical purposes "
	"and because the Hanning and Hamming windows have traditionally been used in other programs before "
	"computers were as fast as they are now (a spectrogram is computed twice as fast "
	"with these other windows). Several other programs still use these inferior window shapes, and you are "
	"likely to run into people who claim that the Gaussian window has disadvantages. "
	"We promise such people a large cake if they can come up with sounds that look better "
	"with Hanning or Hamming windows than with a Gaussian window. An example of the reverse is easy "
	"to find; we have just seen one.")
ENTRY (U"Spectrogram blackness settings")
TAG (U"%%Autoscaling%")
TAG (U"%%Maximum% (dB/Hz)")
DEFINITION (U"all parts of the spectrogram that have a power above %maximum (after preemphasis) "
	"will be drawn in black. The standard maximum is 100 dB/Hz, but if %autoscaling is on (which is the standard), "
	"Praat will use the maximum of the visible part of the spectrogram instead; "
	"this ensures that the window will always look well, but it also means that the blackness "
	"of a certain part of the spectrogram will change as you scroll.")
TAG (U"%%Preemphasis% (dB/octave)")
DEFINITION (U"determines the steepness of a high-pass filter, "
	"i.e., how much the power of higher frequencies will be raised before drawing, as compared to lower frequencies. "
	"Since the spectral slope of human vowels is approximately -6 dB per octave, "
	"the standard value for this setting is +6 dB per octave, "
	"so that the spectrum is flattened and the higher formants look as strong as the lower ones. "
	"When you raise the preemphasis, frequency bands above 1000 Hz will become darker, those below 1000 Hz will become lighter.")
TAG (U"%%Dynamic compression")
DEFINITION (U"determines how much stronger weak spectra should be made before drawing. "
	"Normally, this parameter is between 0 and 1. If it is 0 (the standard value), there is no dynamic compression. "
	"If it is 1, all spectra will be drawn equally strong, "
	"i.e., all of them will contain frequencies that are drawn in black. "
	"If this parameter is 0.4 and the global maximum is at 80 dB, then a spectrum with a maximum at 20 dB "
	"(which will normally be drawn all white if the dynamic range is 50 dB), "
	"will be raised by 0.4 * (80 - 20) = 24 dB, "
	"so that its maximum will be seen at 44 dB (thus making this frame visible).")
MAN_END

MAN_BEGIN (U"Intro 3.3. Querying the spectrogram", U"ppgb", 20030403)
NORMAL (U"If you click anywhere inside the spectrogram, a cursor cross will appear, "
	"and you will see the time and frequency in red at the top and to the left of the window. "
	"To see the time in the Info window, "
	"choose ##Get cursor# from the #Query menu or press the F6 key. "
	"To see the frequency in the Info window, "
	"choose ##Get frequency# from the #Spectrum menu.")
NORMAL (U"To query the power of the spectrogram at the cursor cross, "
	"choose ##Get spectral power at cursor cross# from the #Spectrum menu or press the F9 key. "
	"The Info window will show you the power density, expressed in Pascal^2/Hz.")
MAN_END

MAN_BEGIN (U"Intro 3.4. Printing the spectrogram", U"ppgb", 20070905)
NORMAL (U"To print a spectrogram, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to paint it into the @@Picture window@. "
	"You do this by choosing ##Paint visible spectrogram...# "
	"from the Spectrum menu in the Sound or TextGrid window. "
	"From the File menu in the Picture window, you can then print it, save it to an EPS file, "
	"or copy it to the clipboard (to do Paste in your word processor, for instance).")
MAN_END

MAN_BEGIN (U"Intro 3.5. The Spectrogram object", U"ppgb", 20070905)
NORMAL (U"To do more with spectrograms, you can create a @Spectrogram object in the @@List of Objects@. "
	"You do this either by choosing ##Extract visible spectrogram# "
	"from the Spectrum menu in the Sound or TextGrid window, "
	"or by selecting a Sound object in the list and choosing @@Sound: To Spectrogram...@ from the #Spectrum menu. "
	"In either case, a new Spectrogram object will appear in the list. "
	"To draw this Spectrogram object to the @@Picture window@, "
	"select it and choose the @@Spectrogram: Paint...@ command. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard. "
	"Many other commands are available in the @@dynamic menu@.")
MAN_END

MAN_BEGIN (U"Intro 3.6. Viewing a spectral slice", U"ppgb", 20030316)
INTRO (U"With ##View spectral slice# from the #Spectrum menu in the @SoundEditor and the @TextGridEditor, "
	"you can see the frequency spectrum at the time cursor "
	"or the average frequency spectrum in the time selection.")
ENTRY (U"Spectral slice at the cursor")
NORMAL (U"If you click anywhere in the wave form of the SoundEditor or TextGridEditor windows, "
	"a cursor will appear at that time. If you then choose ##View spectral slice#, "
	"Praat will create a @Spectrum object named %slice in the Objects window and show it in a @SpectrumEditor window. "
	"In this way, you can inspect the frequency contents of the signal around the cursor position.")
ENTRY (U"Spectral slice from a selection")
NORMAL (U"If you drag the mouse through the wave form of the SoundEditor or TextGridEditor windows, "
	"a @@time selection@ will appear. If you then choose ##View spectral slice#, "
	"Praat will again create a @Spectrum object named %slice in the Objects window and show it in a @SpectrumEditor window. "
	"In this way, you can inspect the frequency contents of the signal in the selection.")
MAN_END


MAN_BEGIN (U"Intro 3.7. Configuring the spectral slice", U"ppgb", 20110128)
ENTRY (U"Spectral slice at the cursor")
NORMAL (U"What Praat does precisely, depends on your Spectrogram settings. "
	"Suppose that the %%window length% setting is 0.005 seconds (5 milliseconds). "
	"If the %%window shape% is not Gaussian, Praat will extract the part of the sound "
	"that runs from 2.5 milliseconds before the cursor to 2.5 ms after the cursor. "
	"Praat then multiplies this 5 ms long signal by the window shape, then computes a spectrum "
	"with the method of @@Sound: To Spectrum...@, which is put into the Objects window and opened in an editor window. "
	"If the window shape is Gaussian, Praat will extract a part of the sound "
	"that runs from 5 milliseconds before the cursor to 5 ms after the cursor. The spectrum will then be based "
	"on a `physical' window length of 10 ms, although the `effective' window length is still 5 ms "
	"(see @@Intro 3.2. Configuring the spectrogram@ for details).")
ENTRY (U"Spectral slice from a selection")
NORMAL (U"What Praat does precisely, again depends on the %%window shape% of your Spectrogram settings. "
	"Suppose that your selection is 50 ms long. Praat will extract the entire selection, "
	"then multiply this 50 ms long signal by the window shape, then compute a spectrum, put it into the Objects window and open it an editor window. "
	"This procedure is equivalent to choosing ##Extract windowed selection...# (with a %%relative duration% of 1.0), "
	"followed by ##To Spectrum...# (with %fast switched on), followed by #Edit.")
NORMAL (U"If the window is Gaussian, Praat will still only use the selection, without doubling its duration. "
	"This means that the spectrum that you see in this case will mainly be based on the centre half of the selection, "
	"and the signal near the edges will be largely ignored.")
MAN_END

MAN_BEGIN (U"Intro 3.8. The Spectrum object", U"ppgb", 20030403)
NORMAL (U"To compute a Fourier frequency spectrum of an entire sound, "
	"select a @Sound object and choose @@Sound: To Spectrum...|To Spectrum...@ from the #Spectrum menu. "
	"A new @Spectrum object will appear in the @@List of Objects@. "
	"To view or modify it (or listen to its parts), click @@View & Edit@. "
	"To print it, choose one of the #Draw commands to draw the Spectrum object to the @@Picture window@ first.")
MAN_END

MAN_BEGIN (U"Intro 4. Pitch analysis", U"ppgb", 20070905)
INTRO (U"This section describes how you can analyse the pitch contour of an existing sound.")
LIST_ITEM (U"@@Intro 4.1. Viewing a pitch contour")
LIST_ITEM (U"@@Intro 4.2. Configuring the pitch contour")
LIST_ITEM (U"@@Intro 4.3. Querying the pitch contour")
LIST_ITEM (U"@@Intro 4.4. Printing the pitch contour")
LIST_ITEM (U"@@Intro 4.5. The Pitch object")
MAN_END

MAN_BEGIN (U"Intro 4.1. Viewing a pitch contour", U"ppgb", 20110128)
NORMAL (U"To see the pitch contour of an existing sound as a function of time, "
	"select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor window will appear on your screen. "
	"The bottom half of this window will contain a pitch contour, drawn as a blue line or as a sequence of blue dots. "
	"If you do not see the pitch contour, choose @@Show pitch@ from the #Pitch menu.")
NORMAL (U"To the right of the window, you may see three pitch values, written with blue digits: "
	"at the bottom, you see the floor of the viewable pitch range, perhaps 75 Hz; at the top, "
	"you see the ceiling of the pitch range, perhaps 600 Hz; and somewhere in between, you see the pitch value "
	"at the cursor, or the average pitch in the selection.")
MAN_END

MAN_BEGIN (U"Intro 4.2. Configuring the pitch contour", U"ppgb", 20050830)
NORMAL (U"With @@Pitch settings...@ from the #Pitch menu, "
	"you can determine how the pitch contour is displayed and how it is computed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (\"factory settings\"), which appear "
	"when you click #Standards.")
ENTRY (U"The %%pitch range% setting")
NORMAL (U"This is the most important setting for pitch analysis. The standard range is from 75 to 500 hertz, "
	"which means that the pitch analysis method will only find values between 75 and 500 Hz. "
	"The range that you set here will be shown to the right of the analysis window.")
NORMAL (U"For a male voice, you may want to set the floor to 75 Hz, and the ceiling to 300 Hz; "
	"for a female voice, set the range to 100-500 Hz instead. For creaky voice you will want to set it much "
	"lower than 75 Hz.")
NORMAL (U"Here is why you have to supply these settings. If the pitch floor is 75 Hz, "
	"the pitch analysis method requires a 40-millisecond analysis window, "
	"i.e., in order to measure the F0 at a time of, say, 0.850 seconds, "
	"Praat needs to consider a part of the sound that runs from 0.830 to 0.870 seconds. "
	"These 40 milliseconds correspond to 3 maximum pitch periods (3/75 = 0.040). "
	"If you set the pitch floor down to 25 Hz, the analysis window will grow to 120 milliseconds "
	"(which is again 3 maximum pitch periods), i.e., all times between 0.790 and 0.910 seconds will be considered. "
	"This makes it less easy to see fast F0 changes.")
NORMAL (U"So setting the floor of the pitch range is a technical requirement for the pitch analysis. "
	"If you set it too low, you will miss very fast F0 changes, and if you set it too high, "
	"you will miss very low F0 values. For children's voices you can often use 200 Hz, "
	"although 75 Hz will still give you the same time resolution as you get for the males.")
ENTRY (U"The %units setting")
NORMAL (U"This setting determines the units of the vertical pitch scale. Most people like to see the pitch range "
	"in hertz, but there are several other possibilities.")
ENTRY (U"Advanced settings")
NORMAL (U"The Pitch menu also contains @@Advanced pitch settings...@.")
MAN_END

MAN_BEGIN (U"Time step settings...", U"ppgb", 20031003)
INTRO (U"A command in the #View menu of the @SoundEditor and @TextGridEditor "
	"to determine the time interval between consecutive measurements "
	"of pitch, formants, and intensity.")
ENTRY (U"Automatic time steps")
NORMAL (U"It is recommended that you set the %%Time step strategy% to #Automatic. "
	"In this way, Praat computes just enough pitch, formant, and intensity values to draw "
	"reliable pitch, formant, and intensity contours. In general, Praat will compute 4 values "
	"within an analysis window (\"four times oversampling\").")
NORMAL (U"As described in @@Sound: To Pitch...@, Praat's standard time step for pitch analysis is 0.75 divided by the pitch floor, "
	"e.g., if the pitch floor is 75 Hz, the time step will be 0.01 seconds. "
	"In this way, there will be 4 pitch measurements within an analysis window, which is 3 / (75 Hz) = 40 milliseconds long.")
NORMAL (U"As described in @@Sound: To Formant (burg)...@, Praat's standard time step for formant measurements is the %%Window length% divided by 4, "
	"e.g. if the window length is 0.025 seconds, the time step will be 6.25 milliseconds.")
NORMAL (U"As described in @@Sound: To Intensity...@, Praat's standard time step for intensity measurements is 0.8 divided by the pitch floor, "
	"e.g. if the pitch floor is 75 Hz, the time step will be 10.6666667 milliseconds. "
	"In this way, there will be 4 intensity measurements within an intensity analysis window, "
	"which is 3.2 / (75 Hz) = 42.6666667 milliseconds long.")
ENTRY (U"Fixed time step")
NORMAL (U"You can override the automatic time step by setting the %%Time step strategy% to #Fixed. "
	"The %%Fixed time step% setting then determines the time step that Praat will use: "
	"if you set it to 0.001 seconds, Praat will compute pitch, formant, and intensity values for every millisecond. "
	"Beware that this can slow down the editor window appreciably, because this step is much smaller "
	"than usual values of the automatic time step (see above).")
NORMAL (U"Enlarging the time step to e.g. 0.1 seconds will speed up the editor window "
	"but may render the pitch, formant, and intensity curves less exact (they become %undersampled), "
	"which will influence your measurements and the locations of the pulses.")
NORMAL (U"If there are fewer than 2.0 pitch measurement points per analysis window, "
	"Praat will draw the pitch curve as separate little blue disks "
	"rather than as a continuous blue curve, in order to warn you of the undersampling. "
	"E.g. if the pitch floor is 75 Hz, Praat will draw the pitch curve as disks if the time step is greater than 0.02 seconds.")
ENTRY (U"View-dependent time step")
NORMAL (U"Another way to override the standard time step is by setting the %%Time step strategy% to ##View-dependent#. "
	"The %%Number of time steps per view% setting then determines the time step that Praat will use: "
	"if you set it to 100, Praat will always compute 100 pitch, formant, and intensity values within the view window. "
	"More precisely: if you zoom the view window to 3 seconds, Praat will show you 100 pitch, formant, and intensity points at distances "
	"of 0.03 seconds (or fewer than 100, if you are near the left or right edge of the signal). "
	"As with the %%Fixed time step% setting, Praat will draw the pitch as separate disks in case of undersampling. "
	"You may want to use this setting if you want the pitch curve to be drawn equally fast independently of the degree "
	"of zooming.")
MAN_END

MAN_BEGIN (U"Advanced pitch settings...", U"ppgb", 20110808)
INTRO (U"A command in the #Pitch menu of the @SoundEditor or @TextGridEditor windows. "
	"Before changing the advanced pitch settings, make sure you understand "
	"@@Intro 4.2. Configuring the pitch contour@.")
ENTRY (U"View range different from analysis range")
NORMAL (U"Normally, the range of pitch values that can be seen in the editor window is equal to the range of pitch values "
	"that the analysis algorithm can determine. If you set the analysis range from 75 to 500 Hz, this will be the range "
	"you see in the editor window as well. If the pitch values in the curve happen to be between 350 and 400 Hz, "
	"you may want to zoom in to the 350-400 Hz pitch region. "
	"You will usually do this by changing the pitch range in the @@Pitch settings...@ window. "
	"However, the analysis range will also change in that case, so that the curve itself may change. "
	"If you do not want that, you can change the %%View range% settings "
	"from \"0.0 (= auto)\" - \"0.0 (= auto)\" to something else, perhaps \"350\" - \"400\".")
ENTRY (U"Pitch analysis settings")
NORMAL (U"For information about these, see @@Sound: To Pitch (ac)...@. The standard settings are best in most cases. "
	"For some pathological voices, you will want to set the voicing threshold to much less than the standard of 0.45, "
	"in order to get pitch values even in irregular parts of the signal.")
MAN_END

MAN_BEGIN (U"Intro 4.3. Querying the pitch contour", U"ppgb", 20040614)
NORMAL (U"With @@Get pitch@ from the #Pitch menu in the @SoundEditor or @TextGridEditor, "
	"you get information about the pitch at the cursor or in the selection. "
	"If a cursor is visible in the window, ##Get pitch# writes to the @@Info window@ "
	"the linearly interpolated pitch at that time; "
	"if a time selection is visible inside the window, ##Get pitch# writes to the @@Info window@ "
	"the mean (average) pitch in the visible part of that selection; "
	"otherwise, ##Get pitch# writes the average pitch in the visible part of the sound.")
MAN_END

MAN_BEGIN (U"Intro 4.4. Printing the pitch contour", U"ppgb", 20070905)
NORMAL (U"To print a pitch contour, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to draw it into the @@Picture window@. "
	"You do this by choosing ##Draw visible pitch contour...# "
	"from the Pitch menu in the Sound or TextGrid window. "
	"From the File menu in the Picture window, you can then print it, save it to an EPS file, "
	"or copy it to the clipboard (to do Paste in your word processor, for instance).")
MAN_END

MAN_BEGIN (U"Intro 4.5. The Pitch object", U"ppgb", 20110128)
NORMAL (U"The pitch contour that is visible in the @SoundEditor or @TextGridEditor window, "
	"can be copied as a separate @Pitch object to the @@List of Objects@. To do this, "
	"choose @@Extract visible pitch contour@ from the #Pitch menu.")
NORMAL (U"Another way to get a separate Pitch object is to select a @Sound object in the list "
	"choose @@Sound: To Pitch...@ (preferred) or any of the other methods from the @@Periodicity menu@.")
NORMAL (U"To view and modify the contents of a Pitch object, select it and choose @@View & Edit@. "
	"This creates a @PitchEditor window on your screen.")
NORMAL (U"To save a pitch contour to disk, select the @Pitch object in the list and choose one of the commands in the @@Save menu@.")
NORMAL (U"Later on, you can read the saved file again with @@Read from file...@ from the @@Open menu@.")
NORMAL (U"To draw a @Pitch object to the @@Picture window@, select it and choose any of the commands in the @@Draw menu@. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard for inclusion in your word processor.")
MAN_END

MAN_BEGIN (U"Advanced pulses settings...", U"ppgb", 20110220)
INTRO (U"A command in the #Pulses menu of the @SoundEditor or @TextGridEditor windows. "
	"Before changing the advanced pulses settings, make sure you understand "
	"the @@Voice@ tutorial.")
NORMAL (U"For information about the ##Maximum period factor# setting, see @@PointProcess: Get jitter (local)...@. "
	"The standard setting is best in most cases. "
	"The ##Period floor# and ##Period ceiling# settings derive from the pitch floor and pitch ceiling (@@Pitch settings...@), "
	"according to a formula given in @@Voice 2. Jitter@.")
MAN_END

MAN_BEGIN (U"Intro 5. Formant analysis", U"ppgb", 20030316)
INTRO (U"This section describes how you can analyse the formant contours of an existing sound.")
LIST_ITEM (U"@@Intro 5.1. Viewing formant contours")
LIST_ITEM (U"@@Intro 5.2. Configuring the formant contours")
LIST_ITEM (U"@@Intro 5.3. Querying the formant contours")
LIST_ITEM (U"@@Intro 5.4. The Formant object")
MAN_END

MAN_BEGIN (U"Intro 5.1. Viewing formant contours", U"ppgb", 20110128)
NORMAL (U"To see the formant contours of a sound as functions of time, select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor window will appear on your screen. "
	"The analysis part of this window will contain formant contours, drawn as red speckles. "
	"If you do not see the formant contours, choose @@Show formant@ from the #Formant menu.")
MAN_END

MAN_BEGIN (U"Intro 5.2. Configuring the formant contours", U"ppgb", 20030316)
NORMAL (U"The formant analysis parameters, with you can set with the #Formant menu, are important. "
	"For a female voice, you may want to set the maximum frequency to 5500 Hz; "
	"for a male voice, set it to 5000 Hz instead. "
	"For more information about analysis parameters, see @@Sound: To Formant (burg)...@.")
MAN_END

MAN_BEGIN (U"Intro 5.3. Querying the formant contours", U"ppgb", 20040616)
NORMAL (U"With @@Get first formant@ from the Formant menu in the @SoundEditor or @TextGridEditor, "
	"you get information about the first formant at the cursor or in the selection. "
	"If there is a cursor, ##Get first formant# writes to the @@Info window@ the linearly interpolated first formant at that time. "
	"If there is a true selection, ##Get first formant# writes to the @@Info window@ the mean first formant in the visble part of that selection. "
	"The same goes for @@Get second formant@ and so on.")
MAN_END

MAN_BEGIN (U"Intro 5.4. The Formant object", U"ppgb", 20030316)
NORMAL (U"The formant contours that are visible in the @SoundEditor or @TextGridEditor window, "
	"can be copied as a separate @Formant object to the @@List of Objects@. To do this, "
	"choose @@Extract visible formant contour@ from the Formant menu.")
NORMAL (U"Another way to get a separate Formant object is to select a @Sound object in the list "
	"choose @@Sound: To Formant (burg)...@ (preferred) or any of the other methods "
	"from the @@Formants & LPC menu@.")
ENTRY (U"Saving formant contours to disk")
NORMAL (U"To save formant contours to disk, select the @Formant object in the list and choose one of the commands in the @@Save menu@.")
NORMAL (U"Later on, you can read the saved file again with @@Read from file...@ from the @@Open menu@.")
ENTRY (U"Drawing formant contours")
NORMAL (U"To draw a @Formant object to the @@Picture window@, select it and choose any of the commands in the @@Draw menu@. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard for inclusion in your word processor.")
MAN_END

MAN_BEGIN (U"Intro 6. Intensity analysis", U"ppgb", 20030316)
INTRO (U"This section describes how you can analyse the intensity contour of an existing sound.")
LIST_ITEM1 (U"@@Intro 6.1. Viewing an intensity contour")
LIST_ITEM1 (U"@@Intro 6.2. Configuring the intensity contour")
LIST_ITEM1 (U"@@Intro 6.3. Querying the intensity contour")
LIST_ITEM1 (U"@@Intro 6.4. The Intensity object")
MAN_END

MAN_BEGIN (U"Intro 6.1. Viewing an intensity contour", U"ppgb", 20110128)
INTRO (U"To see the intensity contour of a sound as a function of time, select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor window will appear on your screen. "
	"The analysis part of this window will contain an intensity contour, drawn as a yellow or green line "
	"(choose ##Show intensity# from the #Intensity menu if it is not visible). "
	"This also works in the @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Intro 6.2. Configuring the intensity contour", U"ppgb", 20041123)
INTRO (U"With ##Intensity settings...# from the #Intensity menu, "
	"you can control how the intensity contour is computed and how it is shown.")
ENTRY (U"The view range")
NORMAL (U"By changing these two numbers you can set the vertical scale. "
	"The standard setting is from 50 to 100 dB, but if you are interested in the power of the background noise "
	"you may want to set it to the range from 0 to 100 dB.")
ENTRY (U"The averaging method")
NORMAL (U"Averaging is what occurs if you make a @@time selection@ (rather than setting the cursor at a single time). "
	"The green number in dB to the left or right side of the intensity contour will show the average "
	"intensity in the selection. The same value is what you get when choosing ##Get intensity# from "
	"the #Intensity menu. You can choose any of the three averaging methods "
	"available for @@Intensity: Get mean...@, or choose for the #median value in the selection.")
ENTRY (U"Pitch floor")
NORMAL (U"The intensity curve is %smoothed, since you usually do not want the intensity curve to go up and down with "
	"the intensity variations within a pitch period. To avoid such pitch-synchronous variations, "
	"the intensity at every time point is a weighted average over many neighbouring time points. "
	"The weighting is performed by a Gaussian (`double Kaiser') window that has a duration that is determined by the "
	"##Minimum pitch# setting (see @@Intro 4.2. Configuring the pitch contour@). "
	"To see more detail than usual, raise the minimum pitch; to get more smoothing than usual, lower it. "
	"For more information, see @@Sound: To Intensity...@.")
ENTRY (U"Subtract mean pressure or not?")
NORMAL (U"Many microphones, microphone preamplifiers, or other components in the recording system "
	"can add a constant level to the air pressure (a %%DC offset%). "
	"You can see this at quiet locations in the wave form, "
	"where the average pressure is not zero but the background noise hovers about some non-zero value. "
	"You will usually not want to take this constant pressure seriously, i.e. the intensity curve should "
	"normally not take into account the energy provided by this constant pressure level. "
	"If you switch on the ##Subtract mean pressure# switch (whose standard setting is also on), "
	"the intensity value at a time point is computed by first subtracting the mean pressure around this point, "
	"and then applying the Gaussian window.")
MAN_END

MAN_BEGIN (U"Intro 6.3. Querying the intensity contour", U"ppgb", 20041123)
INTRO (U"To ask for the intensity at the cursor, or the average intensity in the visible part of the selection, "
	"choose ##Get intensity# from the #Intensity menu or press the F11 key. You get the same value in dB that is visible "
	"to the left or right side of the intensity curve, but with more digits.")
MAN_END

MAN_BEGIN (U"Intro 6.4. The Intensity object", U"ppgb", 20041123)
NORMAL (U"To print an intensity contour, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to create an @Intensity object in the @@List of Objects@. "
	"You do this either by choosing ##Extract visible intensity contour# "
	"from the #Intensity menu in the SoundEditor or TextGridEditor window, "
	"or by selecting a Sound object in the list and choosing @@Sound: To Intensity...@. "
	"In either case, a new Intensity object will appear in the list. "
	"To draw the Intensity object to the @@Picture window@, select it and choose ##Draw...#. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard.")
MAN_END

MAN_BEGIN (U"Intro 7. Annotation", U"ppgb", 20110129)
INTRO (U"You can annotate existing @Sound objects and sound files (@LongSound objects).")
NORMAL (U"The labelling data will reside in a @TextGrid object. This object is separate "
	"from the sound, which means that you will often see two objects in the list: a Sound or LongSound, "
	"and a TextGrid.")
ENTRY (U"Creating a TextGrid")
NORMAL (U"You create a new empty TextGrid from the Sound or LongSound with @@Sound: To TextGrid...@ "
	"or @@LongSound: To TextGrid...@ from the #Annotate menu (which shows up in the Objects window if you select a Sound or LongSound). In this way, the time domain "
	"of the @TextGrid will automatically equal that of the sound (if you choose @@Create TextGrid...@ from "
	"the @@New menu@ instead, you will have to supply the time domain by yourself).")
NORMAL (U"When you create a TextGrid, you specify the names of the %tiers. For instance, if you want to segment "
	"the sound into words and into phonemes, you may want to create two tiers and call them "
	"\"words\" and \"phonemes\" (you can easily add, remove, and rename tiers later). "
	"Since both of these tiers are %%interval tiers% (you label the intervals between the word and phoneme "
	"boundaries, not the boundaries themselves), you specify \"phonemes words\" for %%Tier names%, "
	"and you leave the %%Point tiers% empty.")
ENTRY (U"View and edit")
NORMAL (U"You can edit a TextGrid object all by itself, but you will normally want to see "
	"the sound in the editor window as well. To achieve this, you select both the Sound (or LongSound) and "
	"the TextGrid, and click @@View & Edit@. A @TextGridEditor will appear on your screen. "
	"Like the Sound editor, the TextGrid editor will show you a spectrogram, a pitch contour, a formant contour, "
	"and an intensity contour. This editor will allow "
	"you to add, remove, and edit labels, boundaries, and tiers. Under Help in the TextGridEditor, you will "
	"find the @TextGridEditor manual page. You are strongly advised to read it, because it will show you "
	"how you can quickly zoom (drag the mouse), play (click a rectangle), or edit a label (just type).")
ENTRY (U"Save")
NORMAL (U"You will normally write the TextGrid to disk with @@Save as text file...@ "
	"or @@Save as short text file...@. It is true that @@Save as binary file...@ will also work, but the others "
	"give you a file you can read with any text editor.")
NORMAL (U"However you saved it, you can read the TextGrid into Praat later with @@Read from file...@.")
MAN_END

MAN_BEGIN (U"Intro 8. Manipulation", U"ppgb", 20021212)
LIST_ITEM (U"@@Intro 8.1. Manipulation of pitch")
LIST_ITEM (U"@@Intro 8.2. Manipulation of duration")
LIST_ITEM (U"@@Intro 8.3. Manipulation of intensity")
MAN_END

MAN_BEGIN (U"Intro 8.1. Manipulation of pitch", U"ppgb", 20110128)
INTRO (U"To modify the pitch contour of an existing @Sound object, "
	"you select this @Sound and click ##To Manipulation#. "
	"A @Manipulation object will then appear in the list. "
	"You can then click @@View & Edit@ to raise a @ManipulationEditor, "
	"which will show the pitch contour (@PitchTier) as a series of thick dots. "
	"To reduce the number of dots, choose ##Stylize pitch (2 st)# "
	"from the #Pitch menu; it will then be easy to drag the dots "
	"about the time-pitch area.")
NORMAL (U"If you click any of the rectangles "
	"(or choose any of the #Play commands from the #View menu), "
	"you will hear the modified sound. By shift-clicking, you will hear "
	"the original sound.")
NORMAL (U"To get the modified sound as a separate object, "
	"choose ##Publish resynthesis# from the @@File menu@.")
NORMAL (U"If you modify the duration curve as well (see @@Intro 8.2. Manipulation of duration@), "
	"the modified sound will be based on the modified pitch and duration.")
ENTRY (U"Cloning a pitch contour")
NORMAL (U"To use the pitch contour of one Manipulation object as the pitch contour of another Manipulation object, "
	"you first choose ##Extract pitch tier# for the first Manipulation object, "
	"then select the resulting PitchTier object together with the other Manipulation object "
	"(e.g. by a click on the PitchTier and a Command-click on the Manipulation), "
	"and choose ##Replace pitch tier#.")
ENTRY (U"Precise manipulation of pitch")
NORMAL (U"If you know exactly what pitch contour you want, "
	"you can create an empty PitchTier with @@Create PitchTier...@ from the New menu, "
	"then add points with @@PitchTier: Add point...@.")
NORMAL (U"For instance, suppose you want to have a pitch that falls from 350 to 150 Hz in one second. "
	"You create the PitchTier, then add a point at 0 seconds and a frequency of 350 Hz, "
	"and a point at 1 second with a frequency of 150 Hz. "
	"You can put this PitchTier into a Manipulation object in the way described above.")
MAN_END

MAN_BEGIN (U"Intro 8.2. Manipulation of duration", U"ppgb", 20140421)
INTRO (U"You can use Praat to modify the relative durations in an existing sound.")
NORMAL (U"First, you select a @Sound object and click \"To Manipulation\". "
	"A @Manipulation object will then appear in the list. "
	"You can then click @@View & Edit@ to raise a @ManipulationEditor, "
	"which will show an empty @DurationTier. "
	"You can add targets to this tier by choosing \"Add duration point at cursor\" "
	"from the \"Dur\" menu. The targets will show up as green dots, which you can easily drag "
	"about the duration area.")
NORMAL (U"If you click any of the rectangles "
	"(or choose any of the @Play commands from the @View menu), "
	"you will hear the modified sound. By shift-clicking, you will hear "
	"the original sound.")
NORMAL (U"To get the modified sound as a separate object, "
	"choose ##Publish resynthesis# from the @@File menu@.")
NORMAL (U"If you modify the pitch curve as well (see @@Intro 8.1. Manipulation of pitch@), "
	"the modified sound will be based on the modified duration and pitch.")
ENTRY (U"Precise manipulation of duration")
NORMAL (U"If you know exactly the times and relative durations, it is advisable to write a script (see @Scripting). "
	"Suppose, for instance, that you have a 355-ms piece of sound, and you want to shorten the first 85 ms to 70 ms, "
	"and the remaining 270 ms to 200 ms.")
NORMAL (U"In your first 85 ms, your relative duration should be 70/85, "
	"and during the last 270 ms, it should be 200/270. "
	"The DurationTier does linear interpolation, so it can only be approximate these precise times, "
	"but fortunately to any precision you like:")
CODE (U"Create DurationTier: \"shorten\", 0, 0.085 + 0.270")
CODE (U"Add point: 0.000 70/85")
CODE (U"Add point: 0.084999, 70/85")
CODE (U"Add point: 0.085001, 200/270")
CODE (U"Add point: 0.355, 200/270")
NORMAL (U"To put this DurationTier back into a Manipulation object, you select the two objects together "
	"(e.g. a click on the DurationTier and a Command-click on the Manipulation), "
	"and choose ##Replace duration tier#.")
MAN_END

MAN_BEGIN (U"Intro 8.3. Manipulation of intensity", U"ppgb", 20110128)
INTRO (U"You can modify the intensity contour of an existing sound.")
NORMAL (U"While the pitch and duration of a sound can be modified with the @ManipulationEditor "
	"(see @@Intro 8.1. Manipulation of pitch@ and @@Intro 8.2. Manipulation of duration@), "
	"the modification of the intensity curve is done in a different way.")
NORMAL (U"You can create an @IntensityTier with the command @@Create IntensityTier...@ "
	"from the @@New menu@. With @@View & Edit@, you can add points to this tier. "
	"You can then \"multiply\" this tier with a sound, by selecting the @Sound and the "
	"IntensityTier together and clicking @@Sound & IntensityTier: Multiply|Multiply@. "
	"The points in the IntensityTier represent relative intensities in dB; "
	"therefore, the sound pressure values in the Sound are multiplied by 10^^(dB/20)^.")
NORMAL (U"Instead of an IntensityTier, you can use an @AmplitudeTier; "
	"when you click #Multiply for a selected Sound and AmplitudeTier, "
	"the sound pressure values in the Sound are directly multiplied by the values in the AmplitudeTier.")
MAN_END

MAN_BEGIN (U"Intro 8.4. Manipulation of formants", U"ppgb", 20010408)
INTRO (U"The manipulation of formant contours cannot be as straightforward as the manipulation "
	"of pitch, duration, or intensity contours. See the @@Source-filter synthesis@ tutorial "
	"for an explanation of how formants can be modified in Praat.")
MAN_END


/*
 * Who uses Praat?
 * phonetics
 * linguistics
 * speech pathology
 * psychology
 * bioacoustics: e.g.
hapalemur, crow, monkey, frog, turtle dove, grasshopper, cricket, alligator, crocodile,
gecko, elephant, swallow, chicken, dolphin, humpback, [killer] whale, [electric] fish,
sparrow, meerkat, [fruit] bat, lion, parrot, red deer, sperm whale, [bearded] seal, mouse, cracid, chacma baboon
* music
*/

MAN_BEGIN (U"Labelling", U"ppgb", 20010408)
INTRO (U"See @@Intro 7. Annotation@.")
MAN_END

MAN_BEGIN (U"List of Objects", U"ppgb", 19981024)
INTRO (U"A list in the left-hand part of the @@Object window@.")
ENTRY (U"Purpose")
NORMAL (U"If you select one or more @objects in this list, "
	"the possible actions that you can perform with the selected objects "
	"will appear in the @@Dynamic menu@.")
ENTRY (U"How to select objects")
NORMAL (U"To select one object (and deselect all the others), click on the object.")
NORMAL (U"To extend the selection, drag the mouse (Unix, Windows) or use Shift-click (all platforms).")
NORMAL (U"To change the selection of one object (without changing the others), "
	"use Control-click (Unix, Windows) or Command-click (Macintosh).")
MAN_END

MAN_BEGIN (U"New menu", U"ppgb", 20161013)
INTRO (U"The ##New menu# is one of the menus in the @@Object window@. "
	"You use this menu to create new objects from scratch. It contains the following commands:")
LIST_ITEM (U"• @@Record mono Sound...@")
LIST_ITEM (U"• @@Record stereo Sound...@")
LIST_ITEM (U"• @@Create Sound from formula...@")
LIST_ITEM (U"• @@Create Sound as tone complex...")
LIST_ITEM (U"• @@Create Sound as gammatone...")
LIST_ITEM (U"• @@Create Sound as Shepard tone...")
LIST_ITEM (U"• @@Create Matrix...@ (from a formula)")
LIST_ITEM (U"• @@Create simple Matrix...@ (from a formula)")
LIST_ITEM (U"• @@Create empty PointProcess...@")
LIST_ITEM (U"• @@Create Poisson process...@")
LIST_ITEM (U"• @@Create PitchTier...@")
LIST_ITEM (U"• @@Create DurationTier...@")
LIST_ITEM (U"• @@Create IntensityTier...@")
LIST_ITEM (U"• @@Create FormantGrid...@")
LIST_ITEM (U"• @@Create Strings as file list...@")
LIST_ITEM (U"• @@Create TextGrid...@")
LIST_ITEM (U"• @@OT learning@ tutorial")
LIST_ITEM (U"• @@Create tongue-root grammar...@")
NORMAL (U"To create new objects from files on disk, use the @@Open menu@ instead. "
	"Objects can also often be create from other objects, with commands that start with ##To#.")
MAN_END

MAN_BEGIN (U"Object window", U"ppgb", 20140212)
INTRO (U"One of the two main windows in the Praat program.")
ENTRY (U"Subdivision")
LIST_ITEM (U"To the left: the @@List of Objects@.")
LIST_ITEM (U"To the right: the @@Dynamic menu@.")
ENTRY (U"Fixed buttons")
NORMAL (U"The following buttons appear below the List of Objects:")
LIST_ITEM (U"• @@Rename...")
LIST_ITEM (U"• @@Info")
LIST_ITEM (U"• @@Copy...")
LIST_ITEM (U"• @@Remove")
LIST_ITEM (U"• @@Inspect")
ENTRY (U"Menus")
LIST_ITEM (U"The Object window contains several fixed menus: "
	"the #Praat, #New, #Open, and #Help menus. "
	"It also contains the #Save menu, whose contents vary with the kinds of selected objects, "
	"and must, therefore, be considered part of the dynamic menu.")
ENTRY (U"The Praat menu")
LIST_ITEM (U"• @@New Praat script@: creates an empty @@ScriptEditor@")
LIST_ITEM (U"• @@Open Praat script...@: creates a @@ScriptEditor@ with a script from disk")
LIST_ITEM (U"• The ##Goodies submenu#: for doing things (like using the Calculator) "
	"that do not create new objects and do not depend on the kinds of selected objects.")
LIST_ITEM (U"• The ##Preferences submenu#: for program-wide preferences, "
	"like audio input and output settings.")
LIST_ITEM (U"• ##Buttons...#: raises a @@ButtonEditor@")
LIST_ITEM (U"• (@@Add menu command...@)")
LIST_ITEM (U"• (@@Add action command...@)")
LIST_ITEM (U"• @@Quit")
ENTRY (U"Other menus")
LIST_ITEM (U"• The @@New menu@: for creating objects from scratch.")
LIST_ITEM (U"• The @@Open menu@: for reading objects from file into memory.")
LIST_ITEM (U"• The @@Save menu@: for writing objects from memory to file.")
LIST_ITEM (U"• The ##Help menu#: for viewing the manual.")
MAN_END

MAN_BEGIN (U"Periodicity menu", U"ppgb", 20010417)
INTRO (U"A menu that occurs in the @@Dynamic menu@ for a @Sound.")
NORMAL (U"This menu contains commands for analysing the pitch contour of the selected Sound:")
LIST_ITEM (U"@@Sound: To Pitch...")
LIST_ITEM (U"@@Sound: To Pitch (ac)...")
LIST_ITEM (U"@@Sound: To Pitch (cc)...")
LIST_ITEM (U"@@Sound: To Harmonicity (cc)...")
LIST_ITEM (U"@@Sound: To Harmonicity (ac)...")
MAN_END

MAN_BEGIN (U"Pitch settings...", U"ppgb", 20030316)
INTRO (U"A command in the Pitch menu of the @SoundEditor and @TextGridEditor windows. "
	"See @@Intro 4.2. Configuring the pitch contour@.")
MAN_END

MAN_BEGIN (U"Play", U"ppgb", 20021212)
INTRO (U"A command that is available if you select a @Sound, @Pitch, or @PointProcess object. "
	"It gives you an acoustic representation of the selected object, if your loudspeakers are on "
	"and you did not \"mute\" your computer sound system.")
NORMAL (U"A Play button is also available in the @SoundRecorder window "
	"and in the @View menu of the @SoundEditor or @TextGridEditor. In the editors, "
	"you will usually play a sound by clicking on any of the rectangles around the data.")
MAN_END

MAN_BEGIN (U"Query", U"ppgb", 20021218)
INTRO (U"Query commands give you information about objects.")
NORMAL (U"Most query commands start with the word #%Get or sometimes the word #%Count. "
	"You will find these commands in two places: under the #Query menu that usually appears if you "
	"select an @@Objects|object@ in the list, and in the #Query menus of the @editors.")
ENTRY (U"Behaviour")
NORMAL (U"If you click a query command, the answer will be written to the @@Info window@.")
ENTRY (U"Scripting")
NORMAL (U"In a script, you can still use query commands to write the information to the Info window "
	"but you can also use any query command to put the information into a variable. "
	"(see @@Scripting 6.3. Query commands@). In such a case, the value will not be written into the Info window.")
ENTRY (U"Query commands in the Praat program")
NORMAL (U"The Praat program contains the following query commands:")
MAN_END

MAN_BEGIN (U"Query menu", U"ppgb", 20010417)
INTRO (U"One of the menus in most @editors.")
MAN_END

MAN_BEGIN (U"Quit", U"ppgb", 20050822)
INTRO (U"One of the commands in the @@Praat menu@.")
ENTRY (U"Purpose")
NORMAL (U"To leave the program.")
ENTRY (U"Behaviour")
NORMAL (U"All @objects not written to a file will be lost. "
	"However, file-based objects (like large lexica) will be saved correctly.")
ENTRY (U"Usage")
NORMAL (U"To save your data to a disk file before quitting, choose a command from the @@Save menu@.")
MAN_END

MAN_BEGIN (U"Read from file...", U"ppgb", 20111018)
INTRO (U"One of the commands in the @@Open menu@.")
ENTRY (U"Purpose")
NORMAL (U"To read one or more @objects from a file on disk.")
ENTRY (U"Behaviour")
NORMAL (U"Many kinds of files are recognized:")
LIST_ITEM (U"1. Text files that are structured as described under @@Save as text file...@; "
	"these can contain an object of any type, or a collection of objects.")
LIST_ITEM (U"2. Files that were produced by @@Save as binary file...@ (any type).")
LIST_ITEM (U"3. Files in a LISP text format (only for object types that can be written to a LISP file).")
LIST_ITEM (U"4. Files that were made recognizable by the libraries built on Praat. "
	"For instance, the phonetics library adds recognizers for many kinds of sound files.")
NORMAL (U"If the file contains more than one object, these objects will appear in the list, "
	"and their names will be the same as the names that they had "
	"when they were saved with ##Save as text file...# or ##Save as binary file...#.")
ENTRY (U"Examples")
LIST_ITEM (U"• If the file contains only one Pitch object and is called \"hallo.pit\", "
	"an object with the name \"Pitch hallo\" will appear in the list of objects. "
	"You may have more objects with the same name.")
LIST_ITEM (U"• If the file contains one object of type Pitch, named \"hallo\", "
	"and one object of type Polygon, named \"kromme\", "
	"there will appear two objects in the list, "
	"called \"Pitch hallo\" and \"Polygon kromme\".")
MAN_END

MAN_BEGIN (U"Open menu", U"ppgb", 20110111)
INTRO (U"One of the menus in the @@Object window@.")
NORMAL (U"With the Open menu, you read one or more @objects from a file on disk into memory. "
	"The resulting object(s) will appear in the @@List of Objects@.")
NORMAL (U"The Open menu contains the command @@Read from file...@, which recognizes most file types, "
	"and perhaps several other commands for reading unrecognizable file types (e.g., raw sound data), "
	"or for interpreting known file types in a different way "
	"(e.g., reading two mono sounds from one stereo sound file):")
MAN_END

MAN_BEGIN (U"Remove", U"ppgb", 20021212)
INTRO (U"One of the fixed buttons in the @@Object window@.")
NORMAL (U"You can choose this command after selecting one or more @objects in the list.")
NORMAL (U"The selected objects will permanently disappear from the list, "
	"and the computer memory that they occupied will be freed.")
NORMAL (U"To save your data before removing, choose a command from the @@Save menu@.")
MAN_END

MAN_BEGIN (U"Rename...", U"ppgb", 20111018)
INTRO (U"One of the fixed buttons in the @@Object window@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one object of any type.")
ENTRY (U"Purpose")
NORMAL (U"You can give the selected object a new name.")
ENTRY (U"Behaviour")
NORMAL (U"If you type special symbols or spaces, the Object window will replace them with underscores.")
MAN_END

MAN_BEGIN (U"Resource fork", U"ppgb", 20030316)
INTRO (U"One of the two %forks of a Macintosh file (the other is the %%data fork%). "
	"If a Macintosh file is moved to another platform directly, the resource fork is lost. "
	"To backup your Macintosh files, use compression, for instance with #DropStuff\\tm.")
MAN_END

MAN_BEGIN (U"Save menu", U"ppgb", 20111018)
INTRO (U"One of the menus in the @@Object window@.")
ENTRY (U"Purpose")
NORMAL (U"With the #Save menu, you write one or more selected @objects from memory to a file on disk. "
	"The data can be read in again with one of the commands in the @@Open menu@ "
	"(most often simply with @@Read from file...@).")
ENTRY (U"Usage: save your work")
NORMAL (U"You will often choose a command from this menu just before clicking the @Remove button "
	"or choosing the @Quit command.")
ENTRY (U"Fixed commands")
NORMAL (U"If no object is selected, the #Save menu is empty. "
	"If any object is selected, it will at least contain the following commands:")
LIST_ITEM (U"• @@Write to console")
LIST_ITEM (U"• @@Save as text file...")
LIST_ITEM (U"• @@Save as short text file...")
LIST_ITEM (U"• @@Save as binary file...")
ENTRY (U"Dynamic commands")
NORMAL (U"Depending on the type of the selected object, the following commands may be available "
	"in the #Save menu:")
MAN_END

MAN_BEGIN (U"Save as binary file...", U"ppgb", 20110129)
INTRO (U"One of the commands in the @@Save menu@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one or more @objects.")
ENTRY (U"Behaviour")
NORMAL (U"The Object window will ask you for a file name. "
	"After you click OK, the objects will be written to a binary file on disk.")
ENTRY (U"Usage")
NORMAL (U"The file can be read again with @@Read from file...@.")
ENTRY (U"File format")
NORMAL (U"These files are in a device-independent binary format, "
	"and can be written and read on any machine.")
MAN_END

MAN_BEGIN (U"Save as short text file...", U"ppgb", 20110129)
INTRO (U"One of the commands in the @@Save menu@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one or more @objects.")
ENTRY (U"Behaviour")
NORMAL (U"The Object window will ask you for a file name. "
	"After you click OK, the objects will be written to a text file on disk.")
ENTRY (U"File format")
NORMAL (U"The format is much shorter than the one described at @@Save as text file...@. "
	"Most of the comments are gone, and there is normally one piece of data per line.")
NORMAL (U"The file can be read again with the all-purpose @@Read from file...@.")
MAN_END

MAN_BEGIN (U"Save as text file...", U"ppgb", 20110129)
INTRO (U"One of the commands in the @@Save menu@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one or more @objects.")
ENTRY (U"Behaviour")
NORMAL (U"The Object window will ask you for a file name. "
	"After you click OK, the objects will be written to a text file on disk.")
ENTRY (U"File format")
NORMAL (U"If you selected a single object, e.g., of type Pitch, "
	"the file will start with the lines:")
CODE (U"File type = \"ooTextFile\"")
CODE (U"Object class = \"Pitch\"")
NORMAL (U"After this, the pitch data will follow.")
LIST_ITEM (U"If you selected more than one object, e.g., `Pitch hallo' and `Polygon kromme', "
	"the file will look like:")
CODE (U"File type = \"ooTextFile\"")
CODE (U"Object class = \"Collection\"")
CODE (U"size = 2")
CODE (U"item []:")
CODE (U"    item [1]:")
CODE (U"        class = \"Pitch\"")
CODE (U"        name = \"hallo\"")
CODE (U"        (pitch data...)")
CODE (U"    item [2]:")
CODE (U"        class = \"Polygon\"")
CODE (U"        name = \"kromme\"")
CODE (U"        (polygon data...)")
NORMAL (U"The file can be read again with @@Read from file...@, "
	"which, by the way, does not need the verbosity of the above example. "
	"The following minimal format will also be read correctly:")
CODE (U"\"ooTextFile\"")
CODE (U"\"Collection\"  2")
CODE (U"\"Pitch\"  \"hallo\"  (pitch data...)")
CODE (U"\"Polygon\"  \"kromme\"  (polygon data...)")
NORMAL (U"Thus, all text that is not a free-standing number and is not enclosed in double quotes or < >, "
	"is considered a comment, as is all text following an exclamation mark (`!') on the same line.")
MAN_END

MAN_BEGIN (U"Segmentation", U"ppgb", 20010408)
INTRO (U"See @@Intro 7. Annotation@.")
MAN_END

MAN_BEGIN (U"Show formant", U"ppgb", 20030316)
INTRO (U"One of the commands in the Formant menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 5. Formant analysis@.")
MAN_END

MAN_BEGIN (U"Show intensity", U"ppgb", 20030316)
INTRO (U"One of the commands in the Intensity menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 6. Intensity analysis@.")
MAN_END

MAN_BEGIN (U"Show pitch", U"ppgb", 20030316)
INTRO (U"One of the commands in the Pitch menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 4. Pitch analysis@.")
MAN_END

MAN_BEGIN (U"Show pulses", U"ppgb", 20030316)
INTRO (U"One of the commands in the Pulses menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @Voice.")
MAN_END

MAN_BEGIN (U"Show spectrogram", U"ppgb", 20030316)
INTRO (U"One of the commands in the Spectrogram menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 3. Spectral analysis@.")
MAN_END

MAN_BEGIN (U"Source-filter synthesis", U"ppgb", 20050713)
INTRO (U"This tutorial describes how you can do acoustic synthesis with Praat. "
	"It assumes that you are familiar with the @Intro.")
ENTRY (U"1. The source-filter theory of speech production")
NORMAL (U"The source-filter theory (@@Fant (1960)|Fant 1960@) hypothesizes that an acoustic speech signal can be seen "
	"as a %source signal (the glottal source, or noise generated at a constriction in the vocal tract), "
	"%filtered with the resonances in the cavities of the vocal tract downstream from the glottis "
	"or the constriction. The %%Klatt synthesizer% (@@Klatt & Klatt (1990)|Klatt & Klatt 1990@), for instance, "
	"is based on this idea.")
NORMAL (U"In the Praat program, you can create a %source signal from scratch of from an existing "
	"speech signal, and you can create a %filter from scratch or extract it from an existing speech signal. "
	"You can manipulate (change, adapt) both the source and the filter before doing "
	"the actual synthesis, which combines the two.")
LIST_ITEM (U"@@Source-filter synthesis 1. Creating a source from pitch targets")
LIST_ITEM (U"@@Source-filter synthesis 2. Filtering a source")
LIST_ITEM (U"@@Source-filter synthesis 3. The ba-da continuum")
LIST_ITEM (U"@@Source-filter synthesis 4. Using existing sounds")
MAN_END

MAN_BEGIN (U"Source-filter synthesis 1. Creating a source from pitch targets", U"ppgb", 20140421)
INTRO (U"Creating a glottal source signal for speech synthesis involves creating a @PointProcess, "
	"which is a series of time points that should represent the exact moments of glottal closure.")
NORMAL (U"You may want to start with creating a well-defined pitch contour. "
	"Suppose you want to create a sound with a duration of half a second with a pitch that falls from 300 to 200 Hz "
	"during that time. You first create an empty @PitchTier by choosing @@Create PitchTier...@ from the #New menu "
	"(I call this PitchTier \"empty\" because it does not contain any pitch information yet); "
	"you may want to name the PitchTier \"source\" and have it start at 0 seconds and end at 0.5 seconds. "
	"Once the PitchTier exists and is selected, you can ##View & Edit# it to add pitch points (pitch targets) to it at certain times "
	"(or you choose @@PitchTier: Add point...@ from the #Modify menu repeatedly). "
	"You could add a pitch point of 150 Hz at time 0.0 and a pitch point of 100 Hz at time 0.5. "
	"In the PitchTier window, you can see that the pitch curve falls linearly "
	"from 150 to 100 Hz during its time domain:")
SCRIPT (4.5, 2.5,
	U"Create PitchTier... source 0 0.5\n"
	"Add point... 0.0 150\n"
	"Add point... 0.5 100\n"
	"Draw... 0 0 0 200 yes lines and speckles\n"
	"Text top... no %%An F0 linearly falling between two points\n"
	"One mark left... 100 yes yes no\n"
	"Remove\n"
)
NORMAL (U"You can hear the falling pitch by clicking on the rectangles in the PitchTier window "
	"(or by clicking ##Play pulses#, #Hum, or ##Play sine# in the Objects window).")
NORMAL (U"From this PitchTier, you can create a @PointProcess with @@PitchTier: To PointProcess@. "
	"The resulting PointProcess now represents a series of glottal pulses. To make some parts of this "
	"point process voiceless, you can use @@PointProcess: Remove points between...@. "
	"It is advisable to make the very beginning and end of this point process voiceless, so that the filtered sound "
	"will not start or end abruptly. In the following example, the first and last 20 ms are devoiced, "
	"and a stretch of 70 ms in the middle "
	"is made voiceless as well, perhaps because you want to simulate a voiceless plosive there:")
SCRIPT (4.5, 2.5,
	U"pitchTier = Create PitchTier... source 0 0.5\n"
	"Add point... 0.0 150\n"
	"Add point... 0.5 100\n"
	"pulses = To PointProcess\n"
	"Remove points between... 0 0.02\n"
	"Remove points between... 0.24 0.31\n"
	"Remove points between... 0.48 0.5\n"
	"Draw... 0 0 yes\n"
	"Text top... no %%The times of the glottal pulses\n"
	"One mark bottom... 0.24 yes yes no\n"
	"One mark bottom... 0.31 yes yes no\n"
	"plus pitchTier\n"
	"Remove\n"
)
NORMAL (U"Now that we have a glottal point process (a glottal pulse train), the only thing left "
	"is to turn it into a sound by choosing @@PointProcess: To Sound (phonation)...@. "
	"If you use the standard settings of this command (but with ##Adaptation factor# set to 0.6), the result will be a @Sound with "
	"reasonable glottal flow derivatives centred around each of the original pulses in the point process. "
	"You can check this by selecting the Sound and choosing ##View & Edit#:")
SCRIPT (4.5, 2.5,
	U"pitchTier = Create PitchTier... source 0 0.5\n"
	"Add point... 0.0 150\n"
	"Add point... 0.5 100\n"
	"pulses = To PointProcess\n"
	"Remove points between... 0 0.02\n"
	"Remove points between... 0.24 0.31\n"
	"Remove points between... 0.48 0.5\n"
	"source = To Sound (phonation)... 44100 0.6 0.05 0.7 0.03 3.0 4.0\n"
	"Draw... 0 0 0 0 yes curve\n"
	"Text top... no %%The source waveform\n"
	"One mark bottom... 0.24 yes yes no\n"
	"One mark bottom... 0.31 yes yes no\n"
	"plus pitchTier\n"
	"plus pulses\n"
	"Remove\n"
)
NORMAL (U"You will also see that the amplitude of the first two glottal wave shapes of every voiced stretch "
	"is (realistically) somewhat smaller than the amplitudes of the following wave shapes; "
	"This is the result of setting ##Adaptation factor# to 0.6.")
NORMAL (U"What you have now is what we call a %%glottal source signal%. It does two things: it contains information on the glottal flow, "
	"and it already takes into account one aspect of the %filter, namely the radiation at the lips. "
	"This combination is standard procedure in acoustic synthesis.")
NORMAL (U"The glottal source signal sounds as a voice without a vocal tract. "
	"The following section describes how you add vocal-tract resonances, i.e. the %filter.")
ENTRY (U"Automation")
NORMAL (U"In a clean Praat script, the procedure described above will look as follows:")
CODE (U"pitchTier = Create PitchTier: \"source\", 0, 0.5")
CODE (U"Add point: 0.0, 150")
CODE (U"Add point: 0.5, 100")
CODE (U"pulses = To PointProcess")
CODE (U"Remove points between: 0, 0.02")
CODE (U"Remove points between: 0.24, 0.31")
CODE (U"Remove points between: 0.48, 0.5")
CODE (U"source = To Sound (phonation): 44100, 0.6, 0.05, 0.7, 0.03, 3.0, 4.0")
CODE (U"removeObject: pitchTier, pulses")
CODE (U"selectObject: source")
MAN_END

MAN_BEGIN (U"Source-filter synthesis 2. Filtering a source", U"ppgb", 20140421)
INTRO (U"Once you have a glottal source signal, you are ready to create a filter that represents "
	"the resonances of the vocal tract, as a function of time. In other words, you create a @FormantGrid object.")
NORMAL (U"For a vowel spoken by an average (i.e. adult female) human voice, tradition assumes five formants in the range "
	"between 0 and 5500 hertz. This number comes from a computation of the formants of a "
	"straight tube, which has resonances at wavelengths of four tube lengths, four thirds of a tube length, "
	"four fifths, and so on. For a straight tube 16 centimetres long, the shortest wavelength is 64 cm, "
	"which, with a sound velocity of 352 m/s, means a resonance frequency of 352/0.64 = 550 hertz. "
	"The other resonances will be at 1650, 2750, 3850, and 4950 hertz.")
NORMAL (U"You can create a @FormantGrid object with @@Create FormantGrid...@ from the #New menu (submenu #Tiers):")
CODE (U"Create FormantGrid: \"filter\", 0, 0.5, 10, 550, 1100, 60, 50")
NORMAL (U"This creates a FormantGrid with 10 formants and a single frequency value for each formant: %F__1_ is 550 Hz "
	"and the higher formants are spaced 1100 Hz apart, i.e., they are "
	"1650, 2750, 3850, 4950, 6050, 7150, 8250, 9350, and 10450 hertz; "
	"the ten bandwidths start at 60 Hz and have a spacing of 50 Hz, "
	"i.e., they are 60, 110, 160, 210, 260, 310, 360, 410, 460, and 510 hertz.")
NORMAL (U"You can then create formant contours with @@FormantGrid: Add formant point...@:")
CODE (U"Remove formant points between: 1, 0, 0.5")
CODE (U"Add formant point: 1, 0.00, 100")
CODE (U"Add formant point: 1, 0.05, 700")
CODE (U"Remove formant points between: 2, 0, 0.5")
CODE (U"Add formant point: 2, 0.00, 500")
CODE (U"Add formant point: 2, 0.05, 1100")
NORMAL (U"This example creates a spectral specification whose %F__1_ rises from 100 to 700 hertz during the "
	"first 50 milliseconds (as for any obstruent), and whose %F__2_ rises from 500 to 1100 hertz. "
	"The other eight formants keep their original values, as do the ten bandwidths. "
	"The resulting sound may be a [ba]-like formant transition.")
NORMAL (U"To get the final acoustic result (the sound), you select the glottal source signal "
	"together with the FormantGrid and choose @@Sound & FormantGrid: Filter@.")
NORMAL (U"The resulting sound will have a fairly straight intensity contour. You can change this with "
	"the #Formula command (@@Sound: Formula...@), or by multiplying the source signal or the "
	"acoustic result with an @Intensity or @IntensityTier object.")
MAN_END

MAN_BEGIN (U"Source-filter synthesis 3. The ba-da continuum", U"ppgb", 20140421)
INTRO (U"As an example, we are going to create a male [ba]-[da] continuum in six steps. The acoustic difference "
	"between [ba] and [da] is the initial %F__2_, which is 500 Hz for [ba], and 2500 Hz for [da].")
NORMAL (U"We use the same @PitchTier throughout, to model a falling intonation contour:")
CODE (U"Create PitchTier: \"f0\", 0.00, 0.50")
CODE (U"Add point: 0.00, 150")
CODE (U"Add point: 0.50, 100")
NORMAL (U"The first and last 50 milliseconds are voiceless:")
CODE (U"To PointProcess")
CODE (U"Remove points between: 0.00, 0.05")
CODE (U"Remove points between: 0.45, 0.50")
NORMAL (U"Generate the glottal source signal:")
CODE (U"To Sound (phonation): 44100, 0.6, 0.05, 0.7, 0.03, 3.0, 4.0")
NORMAL (U"During the labial or coronal closure, the sound is almost silent, so we use an @IntensityTier "
	"that models this:")
CODE (U"Create IntensityTier: \"intens\", 0.00, 0.50")
CODE (U"Add point: 0.05, 60")
CODE (U"Add point: 0.10, 80")
NORMAL (U"Generate the source signal:")
CODE (U"#plusObject: \"Sound f0\"")
CODE (U"Multiply")
CODE (U"Rename: \"source\"")
NORMAL (U"The ten sounds are generated in a loop:")
CODE (U"#for i #from 1 #to 10")
	CODE1 (U"f2_locus = 500 + (2500/9) * (i - 1) ; variable names start with lower case!")
	CODE1 (U"Create FormantGrid: \"filter\", 0.0, 0.5, 9, 800, 1000, 60, 80")
	CODE1 (U"Remove formant points between: 1, 0.0, 0.5")
	CODE1 (U"Add formant point: 1, 0.05, 100")
	CODE1 (U"Add bandwidth point: 1, 0.05, 50")
	CODE1 (U"Add formant point: 2, 0.05, f2_locus")
	CODE1 (U"Add bandwidth point: 2, 0.05, 100")
	CODE1 (U"#plusObject: \"Sound source\"")
	CODE1 (U"Filter (no scale)")
	CODE1 (U"Rename: \"bada\" + string\\$  (i)")
	CODE1 (U"#removeObject: \"FormantGrid filter\"")
CODE (U"#endfor")
NORMAL (U"Clean up:")
CODE (U"#removeObject: \"Sound source\", \"Sound f0\", \"IntensityTier intens\",")
CODE (U"... \"PointProcess f0\", \"PitchTier f0\"")
NORMAL (U"In this example, filtering was done without automatic scaling, so that "
	"the resulting signals have equal intensities in the areas where they have "
	"equal formants. You will probably want to multiply all these signals with "
	"the same value in order to bring their amplitudes in a suitable range "
	"between -1 and +1 Pascal.")
MAN_END

MAN_BEGIN (U"Source-filter synthesis 4. Using existing sounds", U"ppgb", 20140421)
ENTRY (U"1. How to extract the %filter from an existing speech sound")
NORMAL (U"You can separate source and filter with the help of the technique of %%linear prediction% "
	"(see @@Sound: LPC analysis@). This technique tries to approximate a given frequency spectrum with "
	"a small number of peaks, for which it finds the mid frequencies and the bandwidths. "
	"If we do this for an overlapping sequence of windowed parts of a sound signal "
	"(i.e. a %%short-term analysis%), we get a quasi-stationary approximation of the signal's "
	"spectral characteristics as a function of time, i.e. a smoothed version of the @Spectrogram.")
NORMAL (U"For a speech signal, the peaks are identified with the resonances (%formants) of the vocal tract. "
	"Since the spectrum of a vowel spoken by an average human being falls off with approximately "
	"6 dB per octave, %%pre-emphasis% is applied to the signal before the linear-prediction analysis, "
	"so that the algorithm will not try to match only the lower parts of the spectrum.")
NORMAL (U"For an adult female human voice, tradition assumes five formants in the range "
	"between 0 and 5500 hertz, say at 550, 1650, 2750, 3850, and 4950 hertz. For the linear prediction in "
	"Praat, you will have to implement this 5500-Hz band-limiting by resampling the original "
	"speech signal to 11 kHz. For a male voice, you would use 10 kHz; for a young child, 20 kHz.")
NORMAL (U"To perform the resampling, you use @@Sound: Resample...@: "
	"you select a @Sound object, and click ##Resample...#. "
	"In the rest of this tutorial, I will use the syntax that you would use in a script, "
	"though you will usually do these things by clicking on objects and buttons. Thus:")
CODE (U"#selectObject: \"Sound hallo\"")
CODE (U"Resample: 11000, 50")
NORMAL (U"You can then perform a linear-prediction analysis on the resampled sound "
	"with @@Sound: To LPC (burg)...@:")
CODE (U"#selectObject: \"Sound hallo_11000\"")
CODE (U"To LPC (burg): 10, 0.025, 0.005, 50")
NORMAL (U"This says that your analysis is done with 10 linear-prediction parameters "
	"(which will yield at most five formant-bandwidth pairs), with an analysis window "
	"effectively 25 milliseconds long, with time steps of 5 milliseconds (so that the windows "
	"will appreciably overlap), and with a pre-emphasis frequency of 50 Hz (which is the point "
	"above which the sound will be amplified by 6 dB/octave prior to the analysis proper).")
NORMAL (U"As a result, an object called \"LPC hallo\" will appear in the list of objects. "
	"This @LPC object is a time function with 10 %%linear-prediction coefficients% in each %%time frame%. "
	"These coefficients are rather opaque even to the expert (try to view them with @Inspect), "
	"but they are the raw material from which formant and bandwidth values can be computed. "
	"To see the smoothed @Spectrogram associated with the LPC object, choose @@LPC: To Spectrogram...@:")
CODE (U"#selectObject: \"LPC hallo_11000\"")
CODE (U"To Spectrogram: 20, 0, 50")
CODE (U"Paint: 0, 0, 0, 0, 50, 0, 0, \"yes\"")
NORMAL (U"Note that when drawing this Spectrogram, you will want to set the pre-emphasis to zero "
	"(the fifth 0 in the last line), because pre-emphasis has already been applied in the analysis.")
NORMAL (U"You can get and draw the formant-bandwidth pairs from the LPC object, "
	"with @@LPC: To Formant@ and @@Formant: Speckle...@:")
CODE (U"#selectObject: \"LPC hallo_11000\"")
CODE (U"To Formant")
CODE (U"Speckle: 0, 0, 5500, 30, \"yes\"")
NORMAL (U"Note that in converting the @LPC into a @Formant object, you may have lost some "
	"information about spectral peaks at very low frequencies (below 50 Hz) or at very high "
	"frequencies (near the @@Nyquist frequency@ of 5500 Hz. Such peaks usually try to fit "
	"an overall spectral slope (if the 6 dB/octave model is inappropriate), and are not seen "
	"as related with resonances in the vocal tract, so they are ignored in a formant analysis. "
	"For resynthesis purposes, they might still be important.")
NORMAL (U"Instead of using the intermediate LPC object, you could have done a formant analysis "
	"directly on the original Sound, with @@Sound: To Formant (burg)...@:")
CODE (U"#selectObject: \"Sound hallo\"")
CODE (U"To Formant (burg): 0.005, 5, 5500, 0.025, 50")
NORMAL (U"A @Formant object has a fixed sampling (time step, frame length), and for every "
	"%%formant frame%, it contains a number of formant-bandwidth pairs.")
NORMAL (U"From a Formant object, you can create a @FormantGrid with @@Formant: Down to FormantGrid@. "
	"A FormantGrid object contains a number of tiers with time-stamped %%formant points% and %%bandwidth points%.")
NORMAL (U"Any of these three types (@LPC, @Formant, and @FormantGrid) can represent the %filter "
	"in source-filter synthesis.")
ENTRY (U"2. How to extract the %source from an existing speech sound")
NORMAL (U"If you are only interested in the %filter characteristics, you can get by with @Formant objects. "
	"To get at the %source signal, however, you need the raw @LPC object: "
	"you select it together with the resampled @Sound, and apply %%inverse filtering%:")
CODE (U"#selectObject: \"Sound hallo_11000\", \"LPC hallo_11000\"")
CODE (U"Filter (inverse)")
NORMAL (U"A new Sound named \"hallo_11000\" will appear in the list of objects "
	"(you could rename it to \"source\"). "
	"This is the estimated source signal. Since the LPC analysis was designed to yield a spectrally "
	"flat filter (through the use of pre-emphasis), this source signal represents everything in the "
	"speech signal that cannot be attributed to the resonating cavities. Thus, the \"source signal\" "
	"will consist of the glottal volume-velocity source (with an expected spectral slope of "
	"-12 dB/octave for vowels) and the radiation characteristics at the lips, which cause a "
	"6 dB/octave spectral rise, so that the resulting spectrum of the \"source signal\" "
	"is actually the %derivative of the glottal flow, with an expected spectral slope of -6 dB/octave.")
NORMAL (U"Note that with inverse filtering you cannot measure the actual spectral slope of the source signal. "
	"Even if the actual slope is very different from -6 dB/octave, formant extraction will try to "
	"match the pre-emphasized spectrum. Thus, by choosing a pre-emhasis of -6 dB/octave, "
	"you %impose a slope of -6 dB/octave on the source signal.")
ENTRY (U"3. How to do the synthesis")
NORMAL (U"You can create a new Sound from a source Sound and a filter, in at least four ways.")
NORMAL (U"If your filter is an @LPC object, you select it and the source, and choose @@LPC & Sound: Filter...@:")
CODE (U"#selectObject: \"Sound source\", \"LPC filter\"")
CODE (U"Filter: \"no\"")
NORMAL (U"If you had computed the source and filter from an LPC analysis, this procedure should give "
	"you back the original Sound, except that windowing has caused 25 milliseconds at the beginning "
	"and end of the signal to be set to zero.")
NORMAL (U"If your filter is a @Formant object, you select it and the source, and choose @@Sound & Formant: Filter@:")
CODE (U"#selectObject: \"Sound source\", \"Formant filter\"")
CODE (U"Filter")
NORMAL (U"If you had computed the source and filter from an LPC analysis, this procedure will not generally give "
	"you back the original Sound, because some linear-prediction coefficients will have been ignored "
	"in the conversion to formant-bandwidth pairs.")
NORMAL (U"If your filter is a @FormantGrid object, you select it and the source, and choose @@Sound & FormantGrid: Filter@:")
CODE (U"#selectObject: \"Sound source\", \"FormantGrid filter\"")
CODE (U"Filter")
NORMAL (U"Finally, you could just know the %%impulse response% of your filter (in a @Sound object). "
	"You then select both Sound objects, and choose @@Sounds: Convolve...@:")
CODE (U"#selectObject: \"Sound source\", \"Sound filter\"")
CODE (U"Convolve: \"integral\", \"zero\"")
NORMAL (U"As a last step, you may want to bring the resulting sound within the [-1; +1] range:")
CODE (U"Scale peak: 0.99")
ENTRY (U"4. How to manipulate the filter")
NORMAL (U"You can hardly change the values in an @LPC object in a meaningful way: "
	"you would have to manually change its rather opaque data with the help of @Inspect.")
NORMAL (U"A @Formant object can be changed in a friendlier way, with @@Formant: Formula (frequencies)...@ "
	"and @@Formant: Formula (bandwidths)...@. For instance, to multiply all formant frequencies by 0.9, "
	"you do")
CODE (U"#selectObject: \"Formant filter\"")
CODE (U"Formula (frequencies): \"self * 0.9\"")
NORMAL (U"To add 200 hertz to all values of %F__2_, you do")
CODE (U"Formula (frequencies): \"if row = 2 then self + 200 else self fi\"")
NORMAL (U"A @FormantGrid object can be changed by adding or removing points:")
LIST_ITEM (U"@@FormantGrid: Add formant point...@")
LIST_ITEM (U"@@FormantGrid: Add bandwidth point...@")
LIST_ITEM (U"@@FormantGrid: Remove formant points between...@")
LIST_ITEM (U"@@FormantGrid: Remove bandwidth points between...@")
ENTRY (U"5. How to manipulate the source signal")
NORMAL (U"You can manipulate the source signal in the same way you that would manipulate any sound, "
	"for instance with the @ManipulationEditor.")
MAN_END

MAN_BEGIN (U"Spectrogram settings...", U"ppgb", 20030316)
INTRO (U"A command in the Spectrogram menu of the @SoundEditor and @TextGridEditor windows. "
	"See @@Intro 3.2. Configuring the spectrogram@.")
MAN_END

MAN_BEGIN (U"Technical", U"ppgb", 20120915)
INTRO (U"The title of a submenu of the @@Praat menu@.")
MAN_END

MAN_BEGIN (U"Types of objects", U"ppgb", 20141109)
INTRO (U"Praat contains the following types of objects and @Editors. "
	"For an introduction and tutorials, see @Intro.")
NORMAL (U"General purpose:")
LIST_ITEM (U"• @Matrix: a sampled real-valued function of two variables")
LIST_ITEM (U"• @Polygon")
LIST_ITEM (U"• @PointProcess: a point process (@PointEditor)")
LIST_ITEM (U"• @Sound: a sampled continuous process (@SoundEditor, @SoundRecorder, @@Sound files@)")
LIST_ITEM (U"• @LongSound: a file-based version of a sound (@LongSoundEditor)")
LIST_ITEM (U"• @Strings")
LIST_ITEM (U"• @Distributions, @PairDistribution")
LIST_ITEM (U"• @Table, @TableOfReal")
LIST_ITEM (U"• @Permutation")
LIST_ITEM (U"• @ParamCurve")
NORMAL (U"Periodicity analysis:")
LIST_ITEM (U"• Tutorials:")
LIST_ITEM1 (U"• @@Intro 4. Pitch analysis")
LIST_ITEM1 (U"• @@Intro 6. Intensity analysis")
LIST_ITEM1 (U"• @Voice (jitter, shimmer, noise)")
LIST_ITEM (U"• @Pitch: articulatory fundamental frequency, acoustic periodicity, or perceptual pitch (@PitchEditor)")
LIST_ITEM (U"• @Harmonicity: degree of periodicity")
LIST_ITEM (U"• @Intensity, @IntensityTier: intensity contour")
NORMAL (U"Spectral analysis:")
LIST_ITEM (U"• Tutorials:")
LIST_ITEM1 (U"• @@Intro 3. Spectral analysis")
LIST_ITEM1 (U"• @@Intro 5. Formant analysis")
LIST_ITEM (U"• @Spectrum: complex-valued equally spaced frequency spectrum (@SpectrumEditor)")
LIST_ITEM (U"• @Ltas: long-term average spectrum")
LIST_ITEM (U"• Spectro-temporal: @Spectrogram, @BarkSpectrogram, @MelSpectrogram")
LIST_ITEM (U"• @Formant: acoustic formant contours")
LIST_ITEM (U"• @LPC: coefficients of Linear Predictive Coding, as a function of time")
LIST_ITEM (U"• @Cepstrum, @CC, @LFCC, @MFCC (cepstral coefficients)")
LIST_ITEM (U"• @Excitation: excitation pattern of basilar membrane")
LIST_ITEM (U"• @Excitations: an ensemble of #Excitation objects")
LIST_ITEM (U"• @Cochleagram: excitation pattern as a function of time")
NORMAL (U"Labelling and segmentation (see @@Intro 7. Annotation@):")
LIST_ITEM (U"• @TextGrid (@TextGridEditor)")
NORMAL (U"Listening experiments:")
LIST_ITEM (U"• @ExperimentMFC")
NORMAL (U"Manipulation of sound:")
LIST_ITEM (U"• Tutorials:")
LIST_ITEM1 (U"• @@Intro 8.1. Manipulation of pitch")
LIST_ITEM1 (U"• @@Intro 8.2. Manipulation of duration")
LIST_ITEM1 (U"• @@Intro 8.3. Manipulation of intensity")
LIST_ITEM1 (U"• @@Filtering")
LIST_ITEM1 (U"• @@Source-filter synthesis")
LIST_ITEM (U"• @PitchTier (@PitchTierEditor)")
LIST_ITEM (U"• @Manipulation (@ManipulationEditor): @@overlap-add@")
LIST_ITEM (U"• @DurationTier")
LIST_ITEM (U"• @FormantGrid")
NORMAL (U"Articulatory synthesis (see the @@Articulatory synthesis@ tutorial):")
LIST_ITEM (U"• @Speaker: speaker characteristics of a woman, a man, or a child")
LIST_ITEM (U"• #Articulation: snapshot of articulatory specifications (muscle activities)")
LIST_ITEM (U"• @Artword: articulatory target specifications as functions of time")
LIST_ITEM (U"• (@VocalTract: area function)")
NORMAL (U"Neural net package:")
LIST_ITEM (U"• @FFNet: feed-forward neural net")
LIST_ITEM (U"• @PatternList")
LIST_ITEM (U"• @Categories: for classification (#CategoriesEditor)")
NORMAL (U"Numerical and statistical analysis:")
LIST_ITEM (U"• @Eigen: eigenvectors and eigenvalues")
LIST_ITEM (U"• @Polynomial, @Roots, @ChebyshevSeries, @LegendreSeries, @ISpline, @MSpline")
LIST_ITEM (U"• @Covariance: covariance matrix")
LIST_ITEM (U"• @Confusion: confusion matrix")
LIST_ITEM (U"• @@Discriminant analysis@: @Discriminant")
LIST_ITEM (U"• @@Principal component analysis@: @PCA")
LIST_ITEM (U"• @Correlation, @ClassificationTable, @SSCP")
LIST_ITEM (U"• @DTW: dynamic time warping")
NORMAL (U"@@Multidimensional scaling@:")
LIST_ITEM (U"• @Configuration (@Salience)")
LIST_ITEM (U"• @@Kruskal analysis@: @Dissimilarity (@Weight), @Similarity")
LIST_ITEM (U"• @@INDSCAL analysis@: @Distance, @ScalarProduct")
LIST_ITEM (U"• @@Correspondence analysis@: @ContingencyTable")
NORMAL (U"Optimality-theoretic learning (see the @@OT learning@ tutorial)")
LIST_ITEM (U"• @OTGrammar (@OTGrammarEditor)")
NORMAL (U"Bureaucracy")
LIST_ITEM (U"• @WordList, @SpellingChecker")
MAN_END

MAN_BEGIN (U"Unicode", U"ppgb", 20110129)
INTRO (U"Praat is becoming a fully international program: "
	"the texts in Praat's TextGrids, Tables, scripts, or Info window (and elsewhere) can contain many types of characters "
	"(see @@special symbols@). "
	"For this reason, Praat saves its text files in one of two possible formats: ASCII or UTF-16.")
ENTRY (U"ASCII text files")
NORMAL (U"If your TextGrid (or Table, or script, or Info window...) contains only characters that can be encoded as ASCII, "
	"namely the characters !\\\" \\# \\$ \\% &\\' ()*+,-./0123456789:;<=>?\\@  "
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\bs]\\^ \\_ ` abcdefghijklmnopqrstuvwxyz{|}~, "
	"then when you say @@Save as text file...@ or #Save, Praat will write an ASCII text file, "
	"which is a text file in which every character is encoded in a single byte (8 bits). "
	"All programs that can read plain text files can read such files produced by Praat.")
ENTRY (U"UTF-16 text files")
NORMAL (U"If your TextGrid (or Table, or script, or Info window...) contains one or more characters that cannot be encoded as ASCII, "
	"for instance West-European characters such as \\ao\\c,\\e'\\o\"\\ss\\?d, East-European characters such as \\c<\\l/\\o:\\s<\\uo\\z', "
	"or Hebrew characters such as \\?+\\B+\\G+\\M%\\vO\\sU, "
	"then when you say @@Save as text file...@ or #Save, Praat will write an UTF-16 text file, "
	"which is a text file in which every character is encoded in two bytes (and some very rare characters in four bytes). "
	"Many programs can read such text files, for instance NotePad, WordPad, Microsoft Word, and TextWrangler.")
ENTRY (U"What if my other programs cannot read UTF-16 text files?")
NORMAL (U"If you want to export your Table to Microsoft Excel or to SPSS, or if you want your TextGrid file to be read "
	"by somebody else's Perl script, then there will be no problem if your Table contains only ASCII characters (see above). "
	"But if your Table contains any other (i.e. non-ASCII) characters, you may be in trouble, because Praat will write the Table "
	"as an UTF-16 text file, and not all of the programs just mentioned can read such files yet.")
NORMAL (U"What you can do is go to ##Text writing preferences...# in the #Preferences submenu of the #Praat menu, "
	"and there set the output encoding to ##UTF-8#. Praat will from then on save your text files in the UTF-8 format, "
	"which means one byte for every ASCII character and 2 to 4 bytes for every non-ASCII character. "
	"Especially on Linux, many programs understand UTF-8 text and will display the correct characters. "
	"Programs such as SPSS do not understand UTF-8 but will still display ASCII characters correctly; "
	"for instance, the names München and Wałęsa may appear as M\\A~\\:-nchen and Wa\\Ao,\\A\"\\TMsa or so.")
NORMAL (U"If you can get by with West-European characters (on Windows), "
	"then you may choose ##try ISO Latin-1, then UTF-16# for the output encoding. "
	"It is possible (but not guaranteed) that programs like SPSS then display your West-European text correctly. "
	"This trick is of limited use, because it will not work if your operating system is set to a \"codepage\" "
	"differently from ISO Latin-1 (or \"ANSI\"), or if you need East-European or Hebrew characters, or if you want "
	"to share your text files with Macintosh users.")
NORMAL (U"If you already have some UTF-16 text files and you want to convert them to UTF-8 or ISO Latin-1 (the latter only if "
	"they do not contain non-West-European characters), then you can read them into Praat and save them again "
	"(with the appropriate output encoding setting). "
	"Other programs, such a NotePad and TextWrangler, can also do this conversion.")
NORMAL (U"Finally, it is still possible to make sure that all texts are ASCII, e.g. you type the characters ß and ő "
	"as \\bsss and \\bso: respectively. See @@special symbols@.")
MAN_END

MAN_BEGIN (U"View", U"ppgb", 20010512)
INTRO (U"One of the menus in several @editors and in the @manual.")
MAN_END

MAN_BEGIN (U"Write to console", U"ppgb", 20110129)
INTRO (U"One of the commands in the @@Save menu@.")
NORMAL (U"You can choose this command after selecting one object. "
	"The data that it contains, is written to the Console window "
	"(the terminal window, if you started up Praat from a terminal window), "
	"in the same format as with the @@Save as text file...@ command, "
	"except for the first line, which reads something like:")
CODE (U"Write to console: class Sound,  name \"hallo\"")
MAN_END

}

/*
> I would like to be able to extract the duration
> and pitch measurements of several words in each utterance

# A Sound and a TextGrid have to be selected first.
textgrid = selected ("TextGrid")
sound = selected ("Sound")
select 'sound'
To Pitch... 0.001 75 600
pitch = selected ("Pitch")
select 'textgrid'
numberOfIntervals = Get number of intervals... 1
filedelete out.txt
for interval to numberOfIntervals
   select 'textgrid'
   tmin = Get starting point... 1 interval
   tmax = Get end point... 1 interval
   duration = tmax - tmin
   select 'pitch'
   f0 = Get quantile... tmin tmax 0.50 Hertz
   fileappend out.txt 'duration:6' 'f0:3''newline$'
endfor
0.225953 377.677
0.170586 376.039
0.328049 376.521
0.275413 378.538
*/

/*
echo Band powers:
binWidth = Get bin width
approximateDuration = 1 / binWidth
call band 125 160
call band 160 200
call band 200 250
call band 5000 6300

procedure band fmin fmax
   bandEnergy = Get band energy... fmin fmax
   rmsPressure_Pascal = sqrt (bandEnergy / approximateDuration)
   intensity_dB = 20 * log10 (rmsPressure_Pascal / 2e-5)
   printline 'fmin'...'fmax': 'intensity_dB:2' dB
endproc
*/

/* drawing overwrites until Erase all */
/* pitch in readable format */

/* End of file manual_tutorials.cpp */
