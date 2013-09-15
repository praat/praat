/* manual_tutorials.cpp
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma
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
#include "praat_version.h"

void manual_tutorials_init (ManPages me);
void manual_tutorials_init (ManPages me) {

MAN_BEGIN (L"What's new?", L"ppgb", 20130915)
INTRO (L"Latest changes in Praat.")
/*LIST_ITEM (L"\\bu Manual page about @@drawing a vowel triangle@.")*/
NORMAL (L"##5.3.56# (15 September 2013)")
LIST_ITEM (L"\\bu Mac: 64-bit alpha version.")
LIST_ITEM (L"\\bu Linux: improved selecting in the Picture window.")
NORMAL (L"##5.3.55# (2 September 2013)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 5.3.54 by which you couldn't select a file for saving.")
NORMAL (L"##5.3.54# (1 September 2013)")
LIST_ITEM (L"\\bu Sound window: removed a bug introduced in 5.3.42 by which you couldn't ask for an odd number of poles in Formant Settings "
	"(by e.g. specifying \"5.5\" for the number of formants).")
LIST_ITEM (L"Linux: improved dragging of selections in the Picture window and the Sound window.")
NORMAL (L"##5.3.53# (9 July 2013)")
LIST_ITEM (L"\\bu Table: more drawing commands.")
NORMAL (L"##5.3.52# (12 June 2013)")
LIST_ITEM (L"\\bu Scripting: editor windows understand #do and ##do\\$ #.")
NORMAL (L"##5.3.51# (30 May 2013)")
LIST_ITEM (L"\\bu Sound window: ##Extract selected sound for overlap...#.")
NORMAL (L"##5.3.49# (13 May 2013)")
LIST_ITEM (L"\\bu TextGrid window: alignment of the sound and the annotation in an interval, via Espeak.")
LIST_ITEM (L"\\bu Scripting: repaired a bug introduced in 5.3.32 that could cause very slow running of scripts.")
NORMAL (L"##5.3.48# (1 May 2013)")
LIST_ITEM (L"\\bu Scripting: variable-substitution-free object selection and file writing.")
LIST_ITEM (L"\\bu Scripting: #selectObject and #removeObject can select or remove multiple objects at a time.")
NORMAL (L"##5.3.47# (23 April 2013)")
LIST_ITEM (L"\\bu OTGrammar: included Giorgio Magri's (2012) update rule (weighted all up, high down).")
NORMAL (L"##5.3.46# (21 April 2013)")
LIST_ITEM (L"\\bu Scripting: variable-substitution-free procedure calls.")
LIST_ITEM (L"\\bu Linux: made the Save menu compatible with Ubuntu 12.04.")
NORMAL (L"##5.3.45# (15 April 2013)")
LIST_ITEM (L"\\bu More parts of the manual reflect variable-substitution-free scripting.")
NORMAL (L"##5.3.44# (7 April 2013)")
LIST_ITEM (L"\\bu ##Create Sound as pure tone...#.")
LIST_ITEM (L"\\bu First steps towards variable-substitution-free scripting: the #do, ##do\\$ #, #writeInfo and #appendInfo functions.")
NORMAL (L"##5.3.43# (27 March 2013)")
LIST_ITEM (L"\\bu Read and write stereo Kay sound files.")
LIST_ITEM (L"\\bu Phonetic symbols \\ts, \\tS and \\ap.")
LIST_ITEM (L"\\bu Network: ##Normalize weights...#, ##List weights...#, ##Weights down to Table...#.")
NORMAL (L"##5.3.42# (2 March 2013)")
LIST_ITEM (L"\\bu Repaired some minor bugs regarding synchronization between multiple windows.")
NORMAL (L"##5.3.41# (9 February 2013)")
LIST_ITEM (L"\\bu Linux: repaired a bug that could cause Praat to crash when closing the Info window or a script window.")
NORMAL (L"##5.3.40# (2 February 2013)")
LIST_ITEM (L"\\bu Windows: better handling (in the LongSound window) of sounds that are too long to play.")
LIST_ITEM (L"\\bu Mac: corrected a bug that caused Praat to crash when closing the Strings window.")
LIST_ITEM (L"\\bu Linux: made audio output work again on 32-bit Linux.")
NORMAL (L"##5.3.39# (6 January 2013)")
LIST_ITEM (L"\\bu Formant & Spectrogram: To IntensityTier...")
NORMAL (L"##5.3.38# (4 January 2013)")
LIST_ITEM (L"\\bu ExperimentMFC: repaired a bug introduced in 5.3.36 that caused sound to fail to play completely if the screen did not blank.")
NORMAL (L"##5.3.37# (2 January 2013)")
LIST_ITEM (L"\\bu ExperimentMFC: after screen blanking, reaction times count from when the response buttons appear.")
NORMAL (L"##5.3.36# (1 January 2013)")
LIST_ITEM (L"\\bu ExperimentMFC: made it possible to blank the screen while the sound is playing.")
NORMAL (L"##5.3.35# (8 December 2012)")
LIST_ITEM (L"\\bu SpeechSynthesizer: made it possible to have more than one at a time.")
LIST_ITEM (L"\\bu Linux: corrected a bug that caused incorrect alignment of the buttons in the Objects window.")
NORMAL (L"##5.3.34# (21 November 2012)")
LIST_ITEM (L"\\bu Windows: corrected a bug that caused incorrect alignment of the buttons in the Objects window.")
LIST_ITEM (L"\\bu The new ##Sound: Combine to stereo# can now work with Sounds of different durations, as the old could.")
LIST_ITEM (L"\\bu Corrected a bug that caused Praat to crash when creating a SpeechSynthesizer.")
NORMAL (L"##5.3.33# (20 November 2012)")
LIST_ITEM (L"\\bu ##Sound: Combine to stereo# can now combine any number of Sounds into a new Sound whose number of channels "
	"is the sum of the numbers of channels of the original Sounds.")
LIST_ITEM (L"\\bu ERP: Down to Sound.")
NORMAL (L"##5.3.32# (17 October 2012)")
LIST_ITEM (L"\\bu Sound window: corrected the working of the \"by window\" scaling option.")
NORMAL (L"##5.3.31# (10 October 2012)")
LIST_ITEM (L"\\bu ERP: Down to Table...")
LIST_ITEM (L"\\bu Linux: corrected a bug that could cause Praat to crash after closing a file selector window for saving.")
NORMAL (L"##5.3.30# (6 October 2012)")
LIST_ITEM (L"\\bu Circumvented a rare Windows bug that could cause Praat to start to write 1.5 as 1,5 in some countries "
	"after opening a strange directory.")
LIST_ITEM (L"\\bu Windows: corrected a bug introduced in 5.3.24 that could cause Praat to crash when quitting.")
LIST_ITEM (L"\\bu Windows: corrected a bug introduced in 5.3.24 that could cause the Objects window to look weird after resizing.")
NORMAL (L"##5.3.29# (30 September 2012)")
LIST_ITEM (L"\\bu @EEG: Draw scalp..., including gray legend.")
LIST_ITEM (L"\\bu Made the new Sound scaling options available for LongSounds.")
NORMAL (L"##5.3.28# (28 September 2012)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 5.3.24 that made Praat crash on OSX 10.5.")
NORMAL (L"##5.3.27# (27 September 2012)")
LIST_ITEM (L"\\bu Corrected a couple of small bugs.")
NORMAL (L"##5.3.26# (26 September 2012)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 5.3.24 that prevented Praat from running in batch mode.")
NORMAL (L"##5.3.25# (26 September 2012)")
LIST_ITEM (L"\\bu EEG: view of scalp distribution takes scaling options into account.")
LIST_ITEM (L"\\bu Linux: swipable Sound window and manual.")
LIST_ITEM (L"\\bu Linux: corrected a bug introduced in 5.3.24 whereby TextGrids could not be edited.")
NORMAL (L"##5.3.24# (24 September 2012)")
LIST_ITEM (L"\\bu More kinds of vertical scaling in e.g. Sound and EEG windows.")
LIST_ITEM (L"\\bu Rewritten user interface (may contain some bugs).")
NORMAL (L"##5.3.23# (7 August 2012)")
LIST_ITEM (L"\\bu Mac: removed a bug introduced in September 2011 that could cause incorrect behaviour of regular expressions since July 2012.")
NORMAL (L"##5.3.22# (21 July 2012)")
LIST_ITEM (L"\\bu Linux: removed a bug introduced in 5.3.21 that could cause Praat to crash when opening files.")
LIST_ITEM (L"\\bu Neural networks: correct writing and reading of learning settings.")
NORMAL (L"##5.3.21# (10 July 2012)")
LIST_ITEM (L"\\bu Linux: better folder choice in file selector window for opening files.")
LIST_ITEM (L"\\bu Repaired a bug that caused Praat to crash when opening a ManPages file on some platforms.")
NORMAL (L"##5.3.20# (5 July 2012)")
LIST_ITEM (L"\\bu @EEG: ##Replace TextGrid#.")
NORMAL (L"##5.3.19# (24 June 2012)")
NORMAL (L"##5.3.18# (15 June 2012)")
LIST_ITEM (L"\\bu Corrected a bug in @@Sound: Change gender...@ that caused a part of the sound not to be changed.")
NORMAL (L"##5.3.17# (12 June 2012)")
LIST_ITEM (L"\\bu @EEG window: extent autoscaling by window.")
LIST_ITEM (L"\\bu ERPTier: Remove events between...")
NORMAL (L"##5.3.16# (23 May 2012)")
NORMAL (L"##5.3.15# (10 May 2012)")
LIST_ITEM (L"\\bu Improvements in clipboards, PDF and EPS files.")
NORMAL (L"##5.3.14# (28 April 2012)")
LIST_ITEM (L"\\bu Linux: Tab shortcut for playing the selection.")
LIST_ITEM (L"\\bu EPS files: higher minimum image resolution for spectrograms (300 instead of 106 dpi).")
NORMAL (L"##5.3.13# (11 April 2012)")
LIST_ITEM (L"\\bu @EEG: Extract part...")
NORMAL (L"##5.3.12# (5 April 2012)")
LIST_ITEM (L"\\bu Praat picture file: allow larger function drawings (up to 10^9 instead of 10^6 points).")
LIST_ITEM (L"\\bu Linux: better audio compatibility with Ubuntu 11.10 (support for unusual sampling frequencies when playing LongSounds).")
NORMAL (L"##5.3.11# (27 March 2012)")
LIST_ITEM (L"\\bu @EEG: a selected ERPTier can extract events on the basis of a column of a selected Table.")
NORMAL (L"##5.3.10# (12 March 2012)")
LIST_ITEM (L"\\bu @EEG: Concatenate.")
NORMAL (L"##5.3.09# (10 March 2012)")
LIST_ITEM (L"\\bu Better text-to-speech.")
NORMAL (L"##5.3.08# (5 March 2012)")
LIST_ITEM (L"\\bu Removed a bug introduced in 5.3.07 that could cause Praat to crash when viewing a LongSound.")
NORMAL (L"##5.3.07# (4 March 2012)")
LIST_ITEM (L"\\bu Praat can now save Sounds as 24-bit and 32-bit WAV files.")
NORMAL (L"##5.3.06# (28 February 2012)")
NORMAL (L"##5.3.05# (19 February 2012)")
LIST_ITEM (L"\\bu SpeechSynthesizer (#New menu \\-> #Sound): text-to-speech.")
LIST_ITEM (L"\\bu @EEG: better scalp distribution drawing for both 32-channel and 64-channel recordings.")
NORMAL (L"##5.3.04# (12 January 2012)")
LIST_ITEM (L"\\bu EEG: draw scalp distribution.")
LIST_ITEM (L"\\bu Linux: better audio compatibility with Ubuntu 11.10 (support for unusual sampling frequencies when playing Sounds).")
NORMAL (L"##5.3.03# (21 November 2011)")
LIST_ITEM (L"\\bu EEG: filtering, editing, more viewing.")
NORMAL (L"##5.3.02# (7 November 2011)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 5.3.01 that could cause the PitchTier or PointProcess window to crash.")
LIST_ITEM (L"\\bu Corrected a bug that could cause the Info window to freeze after an error message in a script window.")
NORMAL (L"##5.3.01# (1 November 2011)")
LIST_ITEM (L"\\bu Macintosh and Windows: better window positioning if the Dock or Task Bar is on the left or right.")
LIST_ITEM (L"\\bu IPA symbol: you can now use \\bs.f for the half-length sign (\\.f).")
LIST_ITEM (L"\\bu EEG window.")
ENTRY (L"What used to be new?")
LIST_ITEM (L"\\bu @@What was new in 5.3?")
LIST_ITEM (L"\\bu @@What was new in 5.2?")
LIST_ITEM (L"\\bu @@What was new in 5.1?")
LIST_ITEM (L"\\bu @@What was new in 5.0?")
LIST_ITEM (L"\\bu @@What was new in 4.6?")
LIST_ITEM (L"\\bu @@What was new in 4.5?")
LIST_ITEM (L"\\bu @@What was new in 4.4?")
LIST_ITEM (L"\\bu @@What was new in 4.3?")
LIST_ITEM (L"\\bu @@What was new in 4.2?")
LIST_ITEM (L"\\bu @@What was new in 4.1?")
LIST_ITEM (L"\\bu @@What was new in 4.0?")
LIST_ITEM (L"\\bu @@What was new in 3.9?")
LIST_ITEM (L"\\bu @@What was new in 3.8?")
LIST_ITEM (L"\\bu @@What was new in 3.7?")
LIST_ITEM (L"\\bu @@What was new in 3.6?")
LIST_ITEM (L"\\bu @@What was new in 3.5?")
LIST_ITEM (L"\\bu @@What was new in 3.3?")
LIST_ITEM (L"\\bu @@What was new in 3.2?")
LIST_ITEM (L"\\bu @@What was new in 3.1?")
MAN_END

MAN_BEGIN (L"What was new in 5.3?", L"ppgb", 20111015)
NORMAL (L"##5.3# (15 October 2011)")
NORMAL (L"##5.2.46# (7 October 2011)")
LIST_ITEM (L"\\bu Corrected the same very old bug as in 5.2.44, but now also for opening and saving files.")
LIST_ITEM (L"\\bu Many better messages.")
NORMAL (L"##5.2.45# (29 September 2011)")
LIST_ITEM (L"\\bu Spectrum window: ##Move cursor to nearest peak#.")
LIST_ITEM (L"\\bu Table: ##Save as comma-separated file...#.")
LIST_ITEM (L"\\bu Windows: you can now change the font size of the script window.")
LIST_ITEM (L"\\bu Windows scripting: the Info window now updates while you are writing to it.")
LIST_ITEM (L"\\bu Windows: error messages now stay at the front so that you never have to click away "
	"an invisible message window anymore.")
NORMAL (L"##5.2.44# (23 September 2011)")
LIST_ITEM (L"\\bu Corrected a very old bug in the history mechanism (the button title wouldn't always show up).")
NORMAL (L"##5.2.43# (21 September 2011)")
LIST_ITEM (L"\\bu Linux: you can now change the font size of the script window.")
LIST_ITEM (L"\\bu Corrected a bug that could cause Praat to crash when opening an already open file in the script window.")
NORMAL (L"##5.2.42# (18 September 2011)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 5.2.36 that prevented formulas from working on FormantGrid objects.")
NORMAL (L"##5.2.41# (17 September 2011)")
LIST_ITEM (L"\\bu Improved dashed-dotted lines in EPS files.")
LIST_ITEM (L"\\bu Corrected a bug introduced in 5.2.36 that caused Praat to crash when running an ExperimentMFC.")
NORMAL (L"##5.2.40# (11 September 2011)")
LIST_ITEM (L"\\bu Corrected a bug in the 64-bit Windows edition that led to an incorrect location for the preferences files.")
NORMAL (L"##5.2.39# (10 September 2011)")
LIST_ITEM (L"\\bu 64-bit edition for Windows.")
LIST_ITEM (L"\\bu Corrected a bug that caused Praat to crash instead of saying \"undefined\" when reporting the shimmer of a sound with fewer than 3 pulses.")
NORMAL (L"##5.2.38# (6 September 2011)")
LIST_ITEM (L"\\bu Corrected several bugs that were introduced in 5.2.36 in the Inspect window.")
NORMAL (L"##5.2.37# (2 September 2011)")
LIST_ITEM (L"\\bu Graphics: added the Chinese phonetic symbols \\id and \\ir. See @@Phonetic symbols: vowels@.")
LIST_ITEM (L"\\bu Corrected a bug introduced in 5.2.36 that caused Praat to crash when querying formant quantiles.")
NORMAL (L"##5.2.36# (30 August 2011)")
LIST_ITEM (L"\\bu Graphics: added superscript diacritics as single symbols: \\^h (\\bs\\^ h), \\^j (\\bs\\^ j), "
	"and many more; also \\_u (\\bs\\_ u). See @@Phonetic symbols: diacritics@.")
LIST_ITEM (L"\\bu Praat fully converted to C++ (this may initially cause some bugs, but will be more reliable in the end).")
NORMAL (L"##5.2.35# (5 August 2011)")
LIST_ITEM (L"\\bu Corrected the menus of the Sound and TextGrid windows.")
NORMAL (L"##5.2.34# (3 August 2011)")
LIST_ITEM (L"\\bu @@Insert picture from file...@ now works on Windows (as well as on the Mac).")
LIST_ITEM (L"\\bu Corrected a bug that could cause Praat to crash when playing a LongSound.")
NORMAL (L"##5.2.33# (29 July 2011)")
LIST_ITEM (L"\\bu Improved the reliability of sound playing and several other things.")
LIST_ITEM (L"\\bu Improved several error messages.")
NORMAL (L"##5.2.32# (22 July 2011)")
LIST_ITEM (L"\\bu Corrected several problems with error messages.")
LIST_ITEM (L"\\bu Corrected a bug that could cause slow execution of scripts with forms.")
NORMAL (L"##5.2.31# (21 July 2011)")
LIST_ITEM (L"\\bu Corrected a bug that caused an incorrect window size in the VowelEditor.")
LIST_ITEM (L"\\bu Corrected a bug that caused incorrect error messages when opening a FLAC file as a LongSound.")
LIST_ITEM (L"\\bu Sound window: corrected a bug that could cause Praat to crash when zooming in on the left or right edge.")
NORMAL (L"##5.2.30# (18 July 2011)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 5.2.29 whereby the list of object actions could stay empty after an error message in a script.")
LIST_ITEM (L"\\bu Corrected a bug in Klatt synthesis whereby the generation of a sound could be refused.")
NORMAL (L"##5.2.29# (12 July 2011)")
LIST_ITEM (L"\\bu More accurate error checking, due to Praat's conversion to C++ (last C-only version was 5.2.17).")
NORMAL (L"##5.2.28# (28 June 2011)")
LIST_ITEM (L"\\bu Corrected some Polygon bugs.")
NORMAL (L"##5.2.27# (19 June 2011)")
LIST_ITEM (L"\\bu @Polygon: ##Draw (closed)...#.")
LIST_ITEM (L"\\bu @@PointProcess: To Sound (phonation)...@: "
	"corrected a bug that could cause Praat to crash if pulses lay outside the time domain.")
NORMAL (L"##5.2.26# (24 May 2011)")
LIST_ITEM (L"\\bu Corrected a bug that could cause Praat to crash on some platforms when reading a Praat binary file.")
LIST_ITEM (L"\\bu @ExperimentMFC: corrected a bug that caused Praat to crash if an experiment contained zero trials.")
LIST_ITEM (L"\\bu Corrected a bug that caused Praat to crash when merging multiple IntervalTiers.")
NORMAL (L"##5.2.25# (11 May 2011)")
LIST_ITEM (L"\\bu OT learning: corrected a crashing bug from ##Get candidate...#.")
NORMAL (L"##5.2.24# (10 May 2011)")
LIST_ITEM (L"\\bu Ability to open WAV files that contain incorrect information about the number of samples.")
LIST_ITEM (L"\\bu Removed an old bug that could cause Praat to fail to read a chronological TextGrid text file.")
NORMAL (L"##5.2.23# (1 May 2011)")
LIST_ITEM (L"\\bu Removed a bug introduced recently that could cause Praat to crash when working with derivatives of TableOfReal (such as Distributions).")
NORMAL (L"##5.2.22# (14 April 2011)")
LIST_ITEM (L"\\bu @ExperimentMFC: reaction times for key presses.")
LIST_ITEM (L"\\bu Linux: more reliable start-up on German systems.")
NORMAL (L"##5.2.21# (29 March 2011)")
LIST_ITEM (L"\\bu Scripting: removed a crashing bug introduced for colour names in 5.2.20.")
NORMAL (L"##5.2.20# (25 March 2011)")
LIST_ITEM (L"\\bu Scripting: removed a large memory leak for indexed variables.")
LIST_ITEM (L"\\bu Scripting: removed a small memory leak for colour names.")
LIST_ITEM (L"\\bu Support for very long file paths on the Mac.")
NORMAL (L"##5.2.19# (16 March 2011)")
LIST_ITEM (L"\\bu @ExperimentMFC: corrected a bug introduced in 5.2.18 that could cause Praat to crash when extracting results from an incomplete experiment.")
NORMAL (L"##5.2.18# (9 March 2011)")
LIST_ITEM (L"\\bu @ExperimentMFC: incomplete experiments can nevertheless output their incomplete results.")
NORMAL (L"##5.2.17# (2 March 2011)")
LIST_ITEM (L"\\bu Better names for Table opening and saving commands.")
LIST_ITEM (L"\\bu @ExperimentMFC: reaction times for mouse clicks.")
LIST_ITEM (L"\\bu Linux/GTK: corrected triple clicks in ExperimentMFC.")
NORMAL (L"##5.2.16# (20 February 2011)")
LIST_ITEM (L"\\bu Better support for WAV files with special \"chunks\" in them.")
LIST_ITEM (L"\\bu Manual: documentation of explicit formulas for @@Voice 2. Jitter|jitter measurements@.")
NORMAL (L"##5.2.15# (11 February 2011)")
LIST_ITEM (L"\\bu @@Sounds: Concatenate with overlap...@.")
NORMAL (L"##5.2.14# (8 February 2011)")
LIST_ITEM (L"\\bu Repaired crashing bug in Manipulation window introduced in 5.2.13.")
NORMAL (L"##5.2.13# (7 February 2011)")
LIST_ITEM (L"\\bu Renamed #Write commands to #Save commands.")
LIST_ITEM (L"\\bu Scripting: allow pause forms without #Stop button (see @@Scripting 6.6. Controlling the user@).")
LIST_ITEM (L"\\bu GTK: correct behaviour of default buttons.")
NORMAL (L"##5.2.12# (28 January 2011)")
LIST_ITEM (L"\\bu Renamed #Edit buttons to ##View & Edit#.")
LIST_ITEM (L"\\bu Better visibility of dragged things on Linux.")
NORMAL (L"##5.2.11# (18 January 2011)")
LIST_ITEM (L"\\bu Better visibility of dragged things (sound selection, tier points, TextGrid boundaries) on Windows and Linux.")
NORMAL (L"##5.2.10# (11 January 2011)")
LIST_ITEM (L"\\bu Renamed #Read and #Write menus to #Open and #Save.")
LIST_ITEM (L"\\bu Sound: use of ##Formula (part)...# can speed up formulas appreciably.")
NORMAL (L"##5.2.09# (9 January 2011)")
LIST_ITEM (L"\\bu Much improved scripting tutorial.")
LIST_ITEM (L"\\bu Listening experiments can now show pictures instead of just texts (on the Mac).")
LIST_ITEM (L"\\bu EPS files can now be many miles wide instead of just 55 inches.")
NORMAL (L"##5.2.08# (1 January 2011)")
LIST_ITEM (L"\\bu Improved manual.")
LIST_ITEM (L"\\bu Improved memory allocation on 32-bit platforms: "
	"less probability of crashes when you approach the 2 GB memory limit gradually, and a new \"low on memory; save your work now\" warning.")
LIST_ITEM (L"\\bu Removed IntervalTier and TextTier datatypes from Praat (old scripts that use them will continue to work).")
NORMAL (L"##5.2.07# (24 December 2010)")
LIST_ITEM (L"\\bu Support for reading and writing multi-channel sound files (i.e. above two-channel stereo).")
NORMAL (L"##5.2.06# (18 December 2010)")
LIST_ITEM (L"\\bu Picture window: a new (the fourth) line type, namely dashed-dotted.")
LIST_ITEM (L"\\bu Support for analysing and drawing multi-channel sounds (i.e. above two-channel stereo).")
LIST_ITEM (L"\\bu Can read some EEG files (BioSemi 24-bit BDF) as a Sound and a TextGrid object.")
LIST_ITEM (L"\\bu Linux: compatibility with computers without English language support.")
LIST_ITEM (L"\\bu Macintosh: support for high-plane (i.e. very uncommon) Unicode characters in file names (as already existed on Unix and Windows).")
NORMAL (L"##5.2.05# (4 December 2010)")
LIST_ITEM (L"\\bu Regular expressions: better Unicode support.")
LIST_ITEM (L"\\bu Scripting window: command ##Convert to C string#.")
NORMAL (L"##5.2.04# (27 November 2010)")
LIST_ITEM (L"\\bu Scripting: allow directory (folder) selector windows; see @@Scripting 6.6. Controlling the user@.")
NORMAL (L"##5.2.03# (19 November 2010)")
LIST_ITEM (L"\\bu Scripting: support for string arrays.")
NORMAL (L"##5.2.02# (17 November 2010)")
LIST_ITEM (L"\\bu TextGrid window: corrected a bug that caused Praat to crash (instead of doing nothing) when you tried to add boundaries "
	"if the selection ran from the penultimate interval to the end of the TextGrid.")
LIST_ITEM (L"\\bu Scripting: support for arrays with multiple indexes.")
LIST_ITEM (L"\\bu Linux: made spectrogram drawing compatible with Ubuntu 10.10.")
LIST_ITEM (L"\\bu Linux: made sound more easily available on Ubuntu 10.10.")
NORMAL (L"##5.2.01# (4 November 2010)")
LIST_ITEM (L"\\bu Scripting: support for numeric @@Scripting 5.6. Arrays|arrays@.")
MAN_END

MAN_BEGIN (L"What was new in 5.2?", L"ppgb", 20101029)
NORMAL (L"##5.2# (29 October 2010)")
NORMAL (L"##5.1.45# (26 October 2010)")
LIST_ITEM (L"\\bu Linux/GTK: allow Praat to run without an X display.")
LIST_ITEM (L"\\bu Sounds are played synchronously in scripts run from ManPages with \\bsSC.")
NORMAL (L"##5.1.44# (4 October 2010)")
LIST_ITEM (L"\\bu Linux/GTK: visibility of ExperimentMFC window.")
LIST_ITEM (L"\\bu Linux/GTK: keyboard shortcuts.")
NORMAL (L"##5.1.43# (4 August 2010)")
LIST_ITEM (L"\\bu Scripting: support for stand-alone programs; see @@Scripting 9.1. Turning a script into a stand-alone program@.")
LIST_ITEM (L"\\bu Table: allow drawing of ellipses even if irrelevant columns contain undefined data.")
LIST_ITEM (L"\\bu Linux/GTK: correct resizing of Table window.")
LIST_ITEM (L"\\bu Linux/GTK: prevented multiple storing of Picture window selection in scripting history.")
NORMAL (L"##5.1.42# (26 July 2010)")
LIST_ITEM (L"\\bu Scripting: allow file selector windows; see @@Scripting 6.6. Controlling the user@.")
LIST_ITEM (L"\\bu Linux: multiple file selection.")
NORMAL (L"##5.1.41# (15 July 2010)")
LIST_ITEM (L"\\bu OTGrammar: ##Compare candidates...#")
LIST_ITEM (L"\\bu GTK: support for the Doulos SIL and Charis SIL fonts.")
LIST_ITEM (L"\\bu GTK: working vowel editor.")
LIST_ITEM (L"\\bu Vowel editor: repaired memory leak.")
//LIST_ITEM (L"\\bu GTK: Picture window: front when drawing.")
//LIST_ITEM (L"\\bu GTK: correct scrolling in manuals.")
NORMAL (L"##5.1.40# (13 July 2010)")
LIST_ITEM (L"\\bu GTK: working Demo window (drawing, clicks, keys).")
LIST_ITEM (L"\\bu GTK: pause forms.")
LIST_ITEM (L"\\bu GTK: manual pages: receive a white background, for compatibility with dark-background themes.")
LIST_ITEM (L"\\bu GTK: in settings windows, better alignment of labels to radio groups.")
LIST_ITEM (L"\\bu GTK: rotated text.")
NORMAL (L"##5.1.39# (10 July 2010)")
LIST_ITEM (L"\\bu GTK beta version.")
LIST_ITEM (L"\\bu Linux: made ##Sound: Record fixed time...# work correctly (the sampling frequency was wrong).")
LIST_ITEM (L"\\bu GTK: list of objects: multiple selection, working Rename button.")
LIST_ITEM (L"\\bu GTK: running a script no longer deselects all objects at the start.")
LIST_ITEM (L"\\bu GTK: working Buttons editor.")
LIST_ITEM (L"\\bu GTK: correctly laid out settings windows.")
NORMAL (L"##5.1.38# (2 July 2010)")
LIST_ITEM (L"\\bu Linux: made ##Sound: Record fixed time...# work correctly (the sampling frequency was wrong).")
LIST_ITEM (L"\\bu Mac: repaired a bug introduced in 5.1.35 that could cause Praat to crash if neither Doulos SIL nor Charis SIL were installed.")
LIST_ITEM (L"\\bu Mac: correct live scrolling in Picture window and DataEditor.")
NORMAL (L"##5.1.37# (23 June 2010)")
LIST_ITEM (L"\\bu PitchTier window and similar windows: can drag multiple points simultaneously.")
LIST_ITEM (L"\\bu Table: %t-tests report the number of degrees of freedom.")
LIST_ITEM (L"\\bu GTK: correct progress bar again.")
LIST_ITEM (L"\\bu GTK: correct behaviour of Synth menu in manipulation window.")
NORMAL (L"##5.1.36# (18 June 2010)")
LIST_ITEM (L"\\bu Mac: live scrolling.")
LIST_ITEM (L"\\bu Demo window: the Demo window can now run from the command line.")
LIST_ITEM (L"\\bu Motif: corrected a bug introduced in 5.1.33 whereby things in the Picture window could be placed incorrectly.")
LIST_ITEM (L"\\bu GTK: script window accepts Unicode again.")
NORMAL (L"##5.1.35# (10 June 2010)")
LIST_ITEM (L"\\bu TextGrid window: removed a very old bug that could lead to reversed intervals and to crashes when you inserted a boundary after using Shift-arrow and Command-arrow.")
LIST_ITEM (L"\\bu Graphics: Praat now uses Doulos SIL instead of Charis SIL if your font setting is \"Times\" and the font is nonbold and nonitalic, "
	"because Doulos SIL matches Times New Roman better.")
LIST_ITEM (L"\\bu kNN: made Ola S\\o\"der's k-nearest-neighbours classification compatible with Windows.")
NORMAL (L"##5.1.34# (31 May 2010)")
LIST_ITEM (L"\\bu Sound window: corrected a bug that caused Praat to crash if the analysis window was shorter than 2 samples.")
LIST_ITEM (L"\\bu GTK: scrolling in the Info window and script window.")
LIST_ITEM (L"\\bu GTK: script editor: implemented searching, replacing, and change dialogs.")
NORMAL (L"##5.1.33# (24 May 2010)")
LIST_ITEM (L"\\bu GTK alpha version.")
LIST_ITEM (L"\\bu Abolished resolution independence: the Sound window now looks the same on all platforms, "
	"and the Demo window has the same relative font size on your screen and on the video projector.")
LIST_ITEM (L"\\bu GTK: support for asynchronous audio output.")
LIST_ITEM (L"\\bu GTK: sound plays once rather than three times in Sound and other windows.")
LIST_ITEM (L"\\bu GTK: can click more than once in the manual.")
LIST_ITEM (L"\\bu GTK: correct pink selections in Sound and other windows.")
LIST_ITEM (L"\\bu GTK: correct dragging in TextGrid, Manipulation, and tier windows.")
LIST_ITEM (L"\\bu GTK: a working TextGrid window.")
LIST_ITEM (L"\\bu GTK: no automatic triple click in the manual (and many other windows).")
LIST_ITEM (L"\\bu GTK: moving cursor while sound is playing in Sound and other windows.")
LIST_ITEM (L"\\bu GTK: correct colours of the rectangles in Sound and other windows.")
LIST_ITEM (L"\\bu GTK: a working Group button.")
LIST_ITEM (L"\\bu GTK: correct font menus, font size menus, colour menus, line type menus.")
LIST_ITEM (L"\\bu GTK: scrolling in the manual (and many other windows).")
LIST_ITEM (L"\\bu GTK: erase old texts in manual windows.")
LIST_ITEM (L"\\bu GTK: made Picture window come back when drawing.")
LIST_ITEM (L"\\bu GTK: Info window is fronted automatically.")
LIST_ITEM (L"\\bu GTK: support sendpraat.")
NORMAL (L"##5.1.32# (30 April 2010)")
LIST_ITEM (L"\\bu Scripting: command ##Expand include files#.")
LIST_ITEM (L"\\bu Scripting: accept lines that start with non-breaking spaces (as may occur in scripts copied from the web manual).")
LIST_ITEM (L"\\bu Sound files: accept MP3 files with extension written in capitals.")
LIST_ITEM (L"\\bu Linux audio recording: corrected input choice (microphone, line).")
NORMAL (L"##5.1.31# (4 April 2010)")
LIST_ITEM (L"\\bu @@Sounds: Convolve...@, @@Sounds: Cross-correlate...@, @@Sound: Autocorrelate...@, with full documentation.")
LIST_ITEM (L"\\bu More query commands for IntensityTier.")
NORMAL (L"##5.1.30# (25 March 2010)")
LIST_ITEM (L"\\bu Scripting: $$createDirectory ()$ can now work with absolute paths.")
LIST_ITEM (L"\\bu PointProcess: made it impossible to add a point where there is already a point.")
NORMAL (L"##5.1.29# (11 March 2010)")
LIST_ITEM (L"\\bu Full support for unicode values above 0xFFFF on Macintosh.")
NORMAL (L"##5.1.28# (10 March 2010)")
LIST_ITEM (L"\\bu TextGrid window: removed a recently introduced bug that could cause Praat to crash when inserting a boundary.")
NORMAL (L"##5.1.27# (7 March 2010)")
LIST_ITEM (L"\\bu Table: Wilcoxon rank sum test.")
LIST_ITEM (L"\\bu Logistic regression: corrected a bug by which a boundary would sometimes not be drawn.")
NORMAL (L"##5.1.26# (25 February 2010)")
LIST_ITEM (L"\\bu Experimental GTK version with the help of Franz Brau\\sse.")
LIST_ITEM (L"\\bu Corrected a bug that could cause Praat to crash if sound playing failed twice.")
NORMAL (L"##5.1.25# (20 January 2010)")
LIST_ITEM (L"\\bu Script window: the new command ##Reopen from disk# allows you to edit the script with an external editor.")
LIST_ITEM (L"\\bu Script window: removed a bug that could cause Praat to crash of you did ##Find again# before having done any #Find.")
NORMAL (L"##5.1.24# (15 January 2010)")
LIST_ITEM (L"\\bu Formulas run 10 to 20 percent faster.")
LIST_ITEM (L"\\bu Macintosh: support for forward delete key and Command-`.")
NORMAL (L"##5.1.23# (1 January 2010)")
LIST_ITEM (L"\\bu Allowed multiple files to be selected with e.g. @@Read from file...@.")
LIST_ITEM (L"\\bu Demo window: guarded against handling the Demo window from two scripts at a time.")
NORMAL (L"##5.1.22# (15 December 2009)")
LIST_ITEM (L"\\bu Picture window: millions of @@colour|colours@ instead of just twelve.")
LIST_ITEM (L"\\bu Sound window: ##Move frequency cursor to...#")
NORMAL (L"##5.1.21# (30 November 2009)")
LIST_ITEM (L"\\bu @@Sound: Draw where...@")
LIST_ITEM (L"\\bu ##Matrix: Draw contours...# and ##LogisticRegression: Draw boundary...# support reversed axes.")
LIST_ITEM (L"\\bu Sound window: ##Move frequency cursor to...#.")
NORMAL (L"##5.1.20# (26 October 2009)")
LIST_ITEM (L"\\bu Editor windows: repaired the Synchronized Zoom And Scroll preference.")
NORMAL (L"##5.1.19# (21 October 2009)")
LIST_ITEM (L"\\bu Table: Randomize rows")
LIST_ITEM (L"\\bu Tables: Append (vertically)")
LIST_ITEM (L"\\bu Scripting: corrected a bug that could cause Praat to crash if the name of a field in a form contained a colon.")
LIST_ITEM (L"\\bu Windows: corrected arc drawing.")
NORMAL (L"##5.1.18# (9 October 2009)")
LIST_ITEM (L"\\bu The @@Demo window@ is less often automatically moved to the front "
	"(in order to allow it to pop up other editor windows).")
LIST_ITEM (L"\\bu @@DTW & TextGrid: To TextGrid (warp times)@: corrected a bug that could "
	"lead to an incorrect end time of the last interval in new IntervalTiers.")
NORMAL (L"##5.1.17# (22 September 2009)")
LIST_ITEM (L"\\bu Made more stereo movies readable.")
LIST_ITEM (L"\\bu Editor windows now have a ##Zoom Back# button.")
NORMAL (L"##5.1.16# (17 September 2009)")
LIST_ITEM (L"\\bu Macintosh: corrected a bug that caused incorrect phonetic symbols if Charis SIL was available but SIL Doulos IPA93 was not.")
NORMAL (L"##5.1.15# (30 August 2009)")
LIST_ITEM (L"\\bu Corrected a bug in @@Sound: Change gender...@ introduced in 5.1.14.")
NORMAL (L"##5.1.14# (27 August 2009)")
LIST_ITEM (L"\\bu Windows: corrected a bug introduced in 5.1.13 that caused Praat to crash during tab navigation.")
LIST_ITEM (L"\\bu Made @@Sound: Change gender...@ compatible with elephant calls (i.e. very low F0).")
NORMAL (L"##5.1.13# (21 August 2009)")
LIST_ITEM (L"\\bu Script window: #Find and #Replace.")
LIST_ITEM (L"\\bu Picture window (and therefore Demo window!): @@Insert picture from file...@ (MacOS 10.4 and up).")
LIST_ITEM (L"\\bu @@Demo window@: full screen (on the Mac).")
LIST_ITEM (L"\\bu Scripting: faster object selection (scripts no longer slow down when there are many objects in the list).")
LIST_ITEM (L"\\bu Scripting: $$variableExists$.")
LIST_ITEM (L"\\bu Macintosh: PDF clipboard (MacOS 10.4 and up).")
NORMAL (L"##5.1.12# (4 August 2009)")
LIST_ITEM (L"\\bu Macintosh: the Picture window can save to PDF file (you need MacOS 10.4 or up).")
LIST_ITEM (L"\\bu Macintosh: corrected a bug that caused Praat to crash at start-up on MacOS 10.3.")
NORMAL (L"##5.1.11# (19 July 2009)")
NORMAL (L"##5.1.10# (8 July 2009)")
LIST_ITEM (L"\\bu Corrected a bug that could cause Praat to crash if the Demo window was closed after an \"execute\".")
LIST_ITEM (L"\\bu OTGrammar & PairDistribution: added ##Get minimum number correct...#.")
NORMAL (L"##5.1.09# (28 June 2009)")
LIST_ITEM (L"\\bu Made East-European Roman characters available in EPS files.")
NORMAL (L"##5.1.08# (21 June 2009)")
LIST_ITEM (L"\\bu Removed a bug introduced in 5.1.07 that could cause strange pictures in manual.")
LIST_ITEM (L"\\bu Macintosh: execute @sendpraat messages immediately instead of waiting for the user to click the jumping Praat icon.")
NORMAL (L"##5.1.07# (12 May 2009)")
LIST_ITEM (L"\\bu Demo window: navigation by arrow keys also on Windows.")
LIST_ITEM (L"\\bu Demo window: no longer crashes on Linux.")
NORMAL (L"##5.1.06# (11 May 2009)")
LIST_ITEM (L"\\bu Demo window.")
NORMAL (L"##5.1.05# (7 May 2009)")
LIST_ITEM (L"\\bu KlattGrid update.")
NORMAL (L"##5.1.04# (4 April 2009)")
LIST_ITEM (L"\\bu Corrected a bug that could cause a missing text character in EPS files produced by a version of Praat running in batch.")
LIST_ITEM (L"\\bu Corrected a bug that could cause high values in a Matrix computed from a Pitch.")
NORMAL (L"##5.1.03# (21 March 2009)")
//LIST_ITEM (L"\\bu OT learning: the \"Random up, highest down\" update rule.")
LIST_ITEM (L"\\bu ExperimentMFC: corrected a bug introduced in 5.0.36 that caused Praat to crash if a sound file did not exist.")
LIST_ITEM (L"\\bu Articulatory synthesis: corrected a bug that could cause Praat to crash when copying Artwords.")
LIST_ITEM (L"\\bu Macintosh: corrected a bug that could cause poor text alignment in picture.")
NORMAL (L"##5.1.02# (9 March 2009)")
LIST_ITEM (L"\\bu Allow pause forms without fields.")
LIST_ITEM (L"\\bu The value \"undefined\" is disallowed from all fields in command windows "
	"except ##Table: Set numeric value...# and ##TableOfReal: Set value...#.")
LIST_ITEM (L"\\bu TextGrid: ##List...# and ##Down to Table...#.")
LIST_ITEM (L"\\bu OT learning: Giorgio Magri's \"Weighted all up, highest down\" update rule.")
NORMAL (L"##5.1.01# (26 February 2009)")
LIST_ITEM (L"\\bu Corrected several bugs in Klatt synthesis.")
MAN_END

MAN_BEGIN (L"What was new in 5.1?", L"ppgb", 20090131)
NORMAL (L"##5.1# (31 January 2009)")
LIST_ITEM (L"\\bu Editors for Klatt synthesis.")
LIST_ITEM (L"\\bu Corrected many bugs.")
NORMAL (L"##5.0.47# (21 January 2009)")
LIST_ITEM (L"\\bu Extensive pause windows: @@Scripting 6.6. Controlling the user@.")
NORMAL (L"##5.0.46# (7 January 2009)")
LIST_ITEM (L"\\bu More Klatt synthesizer.")
LIST_ITEM (L"\\bu First pause form support.")
LIST_ITEM (L"\\bu Renewed CategoriesEditor.")
LIST_ITEM (L"\\bu Repaired several memory leaks.")
NORMAL (L"##5.0.45# (29 December 2008)")
LIST_ITEM (L"\\bu Bug fixes in Klatt synthesizer.")
NORMAL (L"##5.0.44# (24 December 2008)")
LIST_ITEM (L"\\bu David's Klatt synthesizer: @KlattGrid.")
NORMAL (L"##5.0.43# (9 December 2008)")
LIST_ITEM (L"\\bu Scripting tutorial: local variables in procedures.")
NORMAL (L"##5.0.42# (26 November 2008)")
LIST_ITEM (L"\\bu Removed a bug that could cause Praat to crash when drawing pictures in the manual window.")
LIST_ITEM (L"\\bu Removed a bug that could cause Praat to crash when drawing multi-line text.")
NORMAL (L"##5.0.41# (23 November 2008)")
LIST_ITEM (L"\\bu ExperimentMFC: allow multiple lines in all texts.")
LIST_ITEM (L"\\bu Regular expressions: removed a bug that could cause Praat to hang when using \".*\".")
LIST_ITEM (L"\\bu Table: Draw ellipses: removed a bug that could cause Praat to crash if some cells were undefined.")
NORMAL (L"##5.0.40# (10 November 2008)")
LIST_ITEM (L"\\bu Improved reading and writing of text files (faster, bigger).")
NORMAL (L"##5.0.39# (1 November 2008)")
LIST_ITEM (L"\\bu praatcon -a (for sending ANSI encoding when redirected)")
NORMAL (L"##5.0.38# (28 October 2008)")
LIST_ITEM (L"\\bu @FormantGrid: To Formant...")
NORMAL (L"##5.0.36# (20 October 2008)")
LIST_ITEM (L"\\bu @ExperimentMFC: accepts nonstandard sound files.")
NORMAL (L"##5.0.35# (5 October 2008)")
LIST_ITEM (L"\\bu ##Scale times by...# and ##Scale times to...# (Modify menu).")
NORMAL (L"##5.0.34# (22 September 2008)")
LIST_ITEM (L"\\bu ##Shift times by...# and ##Shift times to...# (Modify menu).")
LIST_ITEM (L"\\bu Sound: @@Combine to stereo@ works even if the two mono sounds have different durations or time domains.")
NORMAL (L"##5.0.33# (9 September 2008)")
LIST_ITEM (L"\\bu Windows: prevented warning messages about \"Widget type\".")
NORMAL (L"##5.0.32# (12 August 2008)")
LIST_ITEM (L"\\bu Contributed by Ola S\\o\"der: @@kNN classifiers@ and @@k-means clustering@.")
LIST_ITEM (L"\\bu Made UTF-16-encoded chronological TextGrid files readable.")
NORMAL (L"##5.0.31# (6 August 2008)")
LIST_ITEM (L"\\bu Macintosh: corrected a bug introduced in 5.0.30 that caused Praat to crash when you pressed the Tab key "
	"in a window without text fields.")
NORMAL (L"##5.0.30# (22 July 2008)")
LIST_ITEM (L"\\bu Macintosh and Windows: tab navigation.")
NORMAL (L"##5.0.29# (8 July 2008)")
LIST_ITEM (L"\\bu OTMulti: can record history.")
LIST_ITEM (L"\\bu Picture window: corrected text in Praat picture files.")
NORMAL (L"##5.0.28# (3 July 2008)")
LIST_ITEM (L"\\bu Windows: audio output uses DirectX (next to Multi-Media Extensions).")
NORMAL (L"##5.0.27# (28 June 2008)")
LIST_ITEM (L"\\bu @@Phonetic symbols@: breve (a\\N^).")
LIST_ITEM (L"\\bu Annotation: improved some SpellingChecker commands.")
LIST_ITEM (L"\\bu Table: can now set string values that contain spaces.")
NORMAL (L"##5.0.26# (15 June 2008)")
LIST_ITEM (L"\\bu Windows: sound recording no longer makes your laptop's fan spin.")
LIST_ITEM (L"\\bu Windows: no longer any 64 MB limit on recorded sounds.")
LIST_ITEM (L"\\bu Linux: audio input and output uses Alsa (next to OSS).")
NORMAL (L"##5.0.25# (31 May 2008)")
LIST_ITEM (L"\\bu OT learning: added decision strategy ExponentialMaximumEntropy.")
NORMAL (L"##5.0.24# (14 May 2008)")
LIST_ITEM (L"\\bu Linux: corrected a bug at start-up.")
NORMAL (L"##5.0.23# (9 May 2008)")
LIST_ITEM (L"\\bu Corrected a bug that could cause Praat to crash when you edited an Artword that you had read from a file.")
NORMAL (L"##5.0.22# (26 April 2008)")
LIST_ITEM (L"\\bu Editing formant contours: @FormantGrid.")
NORMAL (L"##5.0.21# (22 April 2008)")
LIST_ITEM (L"\\bu Annotating with Cyrillic, Arabic, Chinese, Korean characters, and many more (on Macintosh and Windows).")
NORMAL (L"##5.0.20# (8 April 2008)")
LIST_ITEM (L"\\bu @ExperimentMFC: prevented the OK key from working if no response (and goodness choice) had been made.")
LIST_ITEM (L"\\bu OT learning: sped up learning from partial outputs by a factor of five or more.")
NORMAL (L"##5.0.19# (4 April 2008)")
LIST_ITEM (L"\\bu TextGrid window: corrected a bug introduced in 5.0.17 that could cause Praat to crash when handling point tiers.")
NORMAL (L"##5.0.18# (31 March 2008)")
LIST_ITEM (L"\\bu Manipulation window: corrected a bug introduced in 5.0.17 that caused Praat to crash when adding the first duration point.")
LIST_ITEM (L"\\bu Sound: added ##Extract all channels#.")
LIST_ITEM (L"\\bu OT learning: added @@OTGrammar & PairDistribution: Find positive weights...@.")
LIST_ITEM (L"\\bu ExperimentMFC: corrected a bug that caused Praat to crash when the second of multiple experiments "
	"referred to non-existing sound files.")
NORMAL (L"##5.0.17# (29 March 2008)")
LIST_ITEM (L"\\bu Sped up vowel editor by a large factor.")
LIST_ITEM (L"\\bu OT learning: corrected Exponential HG update rule from OT-GLA to HG-GLA.")
LIST_ITEM (L"\\bu OT learning: shift Exponential HG average constraint weight to zero after every learning step.")
NORMAL (L"##5.0.16# (25 March 2008)")
LIST_ITEM (L"\\bu Macintosh: returned to old compiler because of incompatibility with MacOS X 10.4.")
NORMAL (L"##5.0.15# (21 March 2008)")
LIST_ITEM (L"\\bu Windows: more reliable dropping of files on the Praat icon when Praat is already running.")
NORMAL (L"##5.0.14# (20 March 2008)")
LIST_ITEM (L"\\bu David's vowel editor (New \\-> Sound).")
LIST_ITEM (L"\\bu Formulas: corrected scaling of sinc function.")
NORMAL (L"##5.0.13# (18 March 2008)")
LIST_ITEM (L"\\bu Corrected drawing of histograms.")
LIST_ITEM (L"\\bu TextGrid window: selected part of the TextGrid can be extracted and saved.")
LIST_ITEM (L"\\bu TextGrid: more complete conversion between backslash trigraphs and Unicode.")
LIST_ITEM (L"\\bu Windows: more reliable dropping of files on the Praat icon when Praat is not running yet.")
LIST_ITEM (L"\\bu Formulas: sinc function.")
NORMAL (L"##5.0.12# (12 March 2008)")
LIST_ITEM (L"\\bu Bigger ligature symbol (k\\lip).")
NORMAL (L"##5.0.11# (7 March 2008)")
LIST_ITEM (L"\\bu Corrected saving of new binary Manipulation files (you can send any unreadable Manipulation files to Paul Boersma for correction).")
NORMAL (L"##5.0.10# (27 February 2008)")
LIST_ITEM (L"\\bu Added the characters \\d- and \\D-.")
LIST_ITEM (L"\\bu Windows: made ##praatcon.exe# compatible with Unicode command lines.")
NORMAL (L"##5.0.09# (16 February 2008)")
LIST_ITEM (L"\\bu Windows: corrected a bug by which Praat would not open files that were dragged on the Praat icon "
	"if the names of these files or their directory paths contained non-ASCII characters.")
LIST_ITEM (L"\\bu Linux: ignore the Mod2 key, because of its unpredictable assignment.")
NORMAL (L"##5.0.08# (10 February 2008)")
LIST_ITEM (L"\\bu Corrected the minus sign (\\bs-m = \"\\-m\").")
NORMAL (L"##5.0.07# (8 February 2008)")
LIST_ITEM (L"\\bu Object IDs are visible in editor windows.")
NORMAL (L"##5.0.06# (31 January 2008)")
LIST_ITEM (L"\\bu Corrected a bug that caused ##Find again# in the TextGrid window not to work.")
LIST_ITEM (L"\\bu Macintosh: made Praat work correctly on 10.3 computers with missing fonts.")
NORMAL (L"##5.0.05# (19 January 2008)")
LIST_ITEM (L"\\bu All Matrix, Sound, Spectrogram, Pitch, Ltas, and Spectrum objects (and more) are now in 52-bit relative precision (instead of the earlier 23 bits).")
LIST_ITEM (L"\\bu Corrected a bug that could lead to \"Unknown opcode (0)\" messages when drawing large sounds (more than 16 million samples) in the Picture window.")
LIST_ITEM (L"\\bu Macintosh: solved around a bug in the C library that could cause incorrect representation of non-ASCII characters (in the OTGrammar window).")
NORMAL (L"##5.0.04# (12 January 2008)")
LIST_ITEM (L"\\bu Windows: corrected a bug introduced in 5.0.03 that caused Praat to crash if you pressed a key in the Sound window.")
LIST_ITEM (L"\\bu Macintosh: some cosmetic corrections.")
NORMAL (L"##5.0.03# (9 January 2008)")
LIST_ITEM (L"\\bu Scripting: guard against opening the same file more than once.")
LIST_ITEM (L"\\bu Table: possibility of regarding a column as a distribution (#Generate menu).")
LIST_ITEM (L"\\bu Macintosh: corrected line colours.")
NORMAL (L"##5.0.02# (27 December 2007)")
LIST_ITEM (L"\\bu TextGrid window: corrected the drawing of numbers to the right of the tiers.")
LIST_ITEM (L"\\bu Corrected a bug that caused Praat to crash when doing ##SpectrumTier: List#.")
NORMAL (L"##5.0.01# (18 December 2007)")
LIST_ITEM (L"\\bu Corrected a bug that could cause Praat to crash when redrawing the sound or TextGrid window.")
MAN_END

MAN_BEGIN (L"What was new in 5.0?", L"ppgb", 20071210)
NORMAL (L"##5.0# (10 December 2007)")
LIST_ITEM (L"\\bu Corrected many bugs.")
LIST_ITEM (L"\\bu Display font sizes in points rather than pixels.")
NORMAL (L"##4.6.41# (9 December 2007)")
LIST_ITEM (L"\\bu Windows: corrected a bug that could cause listening experiments not to run when the directory path included non-ASCII characters; "
	"the same bug could (under comparable circumstances) cause scripted menu commands not to work.")
LIST_ITEM (L"\\bu Corrected a bug that could cause null bytes in data files when the text output encoding preference was ##try ISO Latin-1, then UTF-16#.")
NORMAL (L"##4.6.40# (3 December 2007)")
LIST_ITEM (L"\\bu Corrected some minor bugs.")
NORMAL (L"##4.6.39# (1 December 2007)")
LIST_ITEM (L"\\bu Manual: corrected a bug that could cause Praat to crash when viewing certain manual pages with pictures.")
LIST_ITEM (L"\\bu Scripting: corrected a bug that could cause Praat to crash when a long string was used as an argument to a procedure.")
NORMAL (L"##4.6.38# (19 November 2007)")
LIST_ITEM (L"\\bu More extensive @@logistic regression@.")
NORMAL (L"##4.6.37# (15 November 2007)")
LIST_ITEM (L"\\bu Object list shows numbers.")
LIST_ITEM (L"\\bu Macintosh: corrected saving of non-ASCII text settings.")
NORMAL (L"##4.6.36# (2 November 2007)")
LIST_ITEM (L"\\bu Sound and TextGrid windows: direct drawing of intensity, formants, pulses, and TextGrid.")
LIST_ITEM (L"\\bu Regular expressions: corrected handling of newlines on Windows.")
LIST_ITEM (L"\\bu Scripting: improved positioning of settings windows for script commands in editors on Windows.")
NORMAL (L"##4.6.35# (22 October 2007)")
LIST_ITEM (L"\\bu Windows and Linux: better positioning of form windows of editor scripts.")
LIST_ITEM (L"\\bu Macintosh: OTMulti learning window more compatible with non-ASCII characters.")
NORMAL (L"##4.6.34# (18 October 2007)")
LIST_ITEM (L"\\bu Corrected a bug introduced in September that could cause Praat to crash when starting up "
	"if the user had explicitly made action commands visible or invisible in an earlier session.")
NORMAL (L"##4.6.33# (16 October 2007)")
LIST_ITEM (L"\\bu Corrected a bug introduced in September that caused Praat to crash when a PointProcess window was opened without a Sound.")
LIST_ITEM (L"\\bu Macintosh: objects with non-ASCII names show up correctly in the list.")
NORMAL (L"##4.6.32# (14 October 2007)")
LIST_ITEM (L"\\bu Unicode support for names of objects.")
LIST_ITEM (L"\\bu Linux: first Unicode support (in window titles).")
LIST_ITEM (L"\\bu Windows scripting: corrected a bug that caused weird characters in Paste History.")
NORMAL (L"##4.6.31# (8 October 2007)")
LIST_ITEM (L"\\bu TextGrid window: made Save command available again for TextGrid windows without a sound.")
LIST_ITEM (L"\\bu Corrected a bug that caused binary Collection files with objects with names with non-ASCII characters to be unreadable.")
NORMAL (L"##4.6.30# (3 October 2007)")
LIST_ITEM (L"\\bu OTMulti: added an evaluate command in the Objects window, so that paced learning becomes scriptable.")
LIST_ITEM (L"\\bu Macintosh: worked around a feature of a system library that could cause Praat to crash when reading a Collection text file "
	"that contained objects with non-ASCII names.")
NORMAL (L"##4.6.29# (1 October 2007)")
LIST_ITEM (L"\\bu OT learning: leak and constraint in OTMulti.")
LIST_ITEM (L"\\bu Support for saving Table, TableOfReal, Strings, OTGrammar and OTMulti in Unicode.")
NORMAL (L"##4.6.28# (1 October 2007)")
LIST_ITEM (L"\\bu OT learning: positive constraint satisfactions in OTMulti tableaus.")
LIST_ITEM (L"\\bu Corrected a bug that could cause Praat to crash when reading a non-UTF-8 text file "
	"when the \"Text reading preference\" had been set to UTF-8.")
NORMAL (L"##4.6.27# (29 September 2007)")
LIST_ITEM (L"\\bu Corrected redrawing of text with non-ASCII characters.")
NORMAL (L"##4.6.26# (29 September 2007)")
LIST_ITEM (L"\\bu Corrected reading of high UTF-8 codes.")
NORMAL (L"##4.6.25# (26 September 2007)")
LIST_ITEM (L"\\bu @ExperimentMFC: can set font size for response buttons.")
NORMAL (L"##4.6.24# (24 September 2007)")
LIST_ITEM (L"\\bu Czech, Polish, Croatian, and Hungarian characters such as \\c< \\uo \\t< \\e; \\s' \\l/ \\c\' \\o: (see @@Special symbols@).")
LIST_ITEM (L"\\bu Some support for Hebrew characters such as \\?+ \\B+ \\sU (see @@Special symbols@).")
NORMAL (L"##4.6.23# (22 September 2007)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 4.6.13 that caused crashes in text handling (mainly on Linux).")
LIST_ITEM (L"\\bu Info commands in editors.")
NORMAL (L"##4.6.22# (17 September 2007)")
LIST_ITEM (L"\\bu Phonetic symbols: added the nonsyllabicity diacritic (a\\nv).")
LIST_ITEM (L"\\bu Macintosh: worked around a feature of a system library that could cause strange behaviour of forms in scripts with non-ASCII characters.")
NORMAL (L"##4.6.21# (5 September 2007)")
LIST_ITEM (L"\\bu Sound and TextGrid windows: direct drawing of selected sound to the picture window.")
NORMAL (L"##4.6.20# (2 September 2007)")
LIST_ITEM (L"\\bu Introduced direct drawing of spectrogram and pitch to the Praat picture window from Sound windows and TextGrid windows.")
LIST_ITEM (L"\\bu Corrected a bug introduced in 4.6.13 by which Inspect did not show all data in a TextGrid.")
NORMAL (L"##4.6.19# (31 August 2007)")
LIST_ITEM (L"\\bu Macintosh: worked around a bug in a system library that caused Praat to crash (since version 4.6.13) "
	"when removing a boundary from a TextGrid interval in MacOS X 10.3 or earlier.")
NORMAL (L"##4.6.18# (28 August 2007)")
LIST_ITEM (L"\\bu Sound: Filter (formula): now works in the same way as the other filter commands "
	"(without adding an empty space at the end), and on stereo sounds.")
NORMAL (L"##4.6.17# (25 August 2007)")
LIST_ITEM (L"\\bu Windows: improved rotated text, also for copy-paste and printing.")
LIST_ITEM (L"\\bu Windows: phonetic characters on the screen now require the Charis SIL or Doulos SIL font.")
LIST_ITEM (L"\\bu Picture settings report (mainly for script writers).")
LIST_ITEM (L"\\bu Corrected a bug that could cause Praat to crash when closing a manual page that had been read from a file.")
NORMAL (L"##4.6.16# (22 August 2007)")
LIST_ITEM (L"\\bu Macintosh: corrected a bug introduced in 4.6.13 that could cause Praat to crash when drawing a spectrogram in MacOS X 10.3 or earlier.")
NORMAL (L"##4.6.15# (21 August 2007)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 4.6.14 that prevented the use of Helvetica in the Picture window.")
LIST_ITEM (L"\\bu Corrected a bug in \"Read Table from table file...\", introduced in 4.6.13.")
NORMAL (L"##4.6.14# (20 August 2007)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 4.6.13 that prevented any other font than Palatino in the Picture window.")
LIST_ITEM (L"\\bu Macintosh: corrected height of subscripts and superscripts (broken in 4.6.13).")
NORMAL (L"##4.6.13# (16 August 2007)")
LIST_ITEM (L"\\bu TextGrid: corrected reading of chronological files.")
LIST_ITEM (L"\\bu Macintosh: text looks better (and rotated text is now readable on Intel Macs).")
LIST_ITEM (L"\\bu Macintosh: phonetic characters on the screen now require the Charis SIL or Doulos SIL font.")
NORMAL (L"##4.6.12# (27 July 2007)")
LIST_ITEM (L"\\bu OTGrammar bug fix: leak and constraint plasticity correctly written into OTGrammar text files.")
NORMAL (L"##4.6.11# (25 July 2007)")
LIST_ITEM (L"\\bu OTGrammar: introduced %%constraint plasticity% for slowing down or halting the speed with which constraints are reranked.")
LIST_ITEM (L"\\bu OTGrammar: introduced %%leak% for implementing forgetful learning of correlations.")
LIST_ITEM (L"\\bu OTGrammar: positive constraint satisfactions are drawn as `+' in tableaus.")
NORMAL (L"##4.6.10# (22 July 2007)")
LIST_ITEM (L"\\bu Improved reading of UTF-16 data files.")
LIST_ITEM (L"\\bu Improved error messages when reading text files (line numbers are mentioned).")
LIST_ITEM (L"\\bu Table: Get group mean (Student t)...")
NORMAL (L"##4.6.09# (24 June 2007)")
LIST_ITEM (L"\\bu Corrected a bug introduced in 4.6.07 that caused a crash when reading Collections.")
LIST_ITEM (L"\\bu Corrected a bug introduced in 4.6.07 that caused incorrect Open buttons in Inspect.")
LIST_ITEM (L"\\bu How come 4.6.07 introduced those bugs? Because of large changes in the Praat source code as a result of the transition to Unicode.")
NORMAL (L"##4.6.08# (22 June 2007)")
LIST_ITEM (L"\\bu Windows: worked around a `feature' of the C library that caused 3-byte line-breaks in the buttons file.")
LIST_ITEM (L"\\bu Windows: returned to smaller font in script window.")
LIST_ITEM (L"\\bu OT learning: corrected a bug in PositiveHG.")
NORMAL (L"##4.6.07# (20 June 2007)")
LIST_ITEM (L"\\bu Sound files: MP3 as LongSound (implemented by Erez Volk).")
LIST_ITEM (L"\\bu Scripting: Unicode support for strings and script window (Mac and Windows only).")
NORMAL (L"##4.6.06# (4 June 2007)")
LIST_ITEM (L"\\bu Script window: corrected a bug introduced in 4.6.05 that could cause incorrect symbols in saved files.")
NORMAL (L"##4.6.05# (2 June 2007)")
LIST_ITEM (L"\\bu Sound files: reading MP3 audio files (implemented by Erez Volk).")
NORMAL (L"##4.6.04# (29 May 2007)")
LIST_ITEM (L"\\bu OT learning: added decision strategy PositiveHG.")
NORMAL (L"##4.6.03# (24 May 2007)")
LIST_ITEM (L"\\bu Spectral slices have better names.")
NORMAL (L"##4.6.02# (17 May 2007)")
LIST_ITEM (L"\\bu Sound files: saving FLAC audio files (implemented by Erez Volk).")
NORMAL (L"##4.6.01# (16 May 2007)")
LIST_ITEM (L"\\bu Removed a bug that caused downsampling (and therefore formant measurements) "
	"to be incorrect for stereo sounds.")
MAN_END

MAN_BEGIN (L"What was new in 4.6?", L"ppgb", 20070512)
NORMAL (L"##4.6# (12 May 2007)")
NORMAL (L"##4.5.26# (8 May 2007)")
LIST_ITEM (L"\\bu Sound files: reading FLAC audio files (implemented by Erez Volk).")
NORMAL (L"##4.5.25# (7 May 2007)")
LIST_ITEM (L"\\bu Table: Rows to columns...")
LIST_ITEM (L"\\bu Table: Collapse rows... (renamed from Pool).")
LIST_ITEM (L"\\bu Table: Formula (column range)...")
LIST_ITEM (L"\\bu OT learning: OTGrammar window shows harmonies.")
NORMAL (L"##4.5.24# (27 April 2007)")
LIST_ITEM (L"\\bu OT learning: added decision strategy MaximumEntropy; "
	"this has the same harmony determination method as Harmonic Grammar (include the additive constraint noise), "
	"but there is some more variability, in that every candidate gets a relative probability of exp(harmony).")
NORMAL (L"##4.5.23# (26 April 2007)")
LIST_ITEM (L"\\bu Macintosh: much smaller sizes (in kilobytes) of spectrograms for printing and clipboard; "
	"this improves the compatibility with other programs such as Microsoft Word for large spectrograms.")
NORMAL (L"##4.5.22# (25 April 2007)")
LIST_ITEM (L"\\bu Macintosh: improved drawing of spectrograms for printing and clipboard "
	"(this was crippled in 4.5.18, but now it is better than before 4.5.18).")
NORMAL (L"##4.5.21# (24 April 2007)")
LIST_ITEM (L"\\bu OT learning: corrected HarmonicGrammar (and LinearOT) learning procedure "
	"to the stochastic gradient ascent method applied by @@J\\a\"ger (2003)@ to MaxEnt grammars.")
LIST_ITEM (L"\\bu Scripting: removed a bug that could make selection checking (in command windows) unreliable after a script was run.")
NORMAL (L"##4.5.20# (19 April 2007)")
LIST_ITEM (L"\\bu Scripting: allow assignments like $$pitch = To Pitch... 0 75 600$.")
LIST_ITEM (L"\\bu PitchTier Formula: guard against undefined values.")
NORMAL (L"##4.5.19# (2 April 2007)")
LIST_ITEM (L"\\bu Scripting: allow comments with \"\\# \" and \";\" in forms.")
LIST_ITEM (L"\\bu Windows audio playing: attempt at more compatibility with Vista.")
NORMAL (L"##4.5.18# (30 March 2007)")
LIST_ITEM (L"\\bu Macintosh: better image drawing (more grey values).")
LIST_ITEM (L"\\bu More tabulation commands.")
LIST_ITEM (L"\\bu More SpectrumTier commands.")
LIST_ITEM (L"\\bu Picture window: keyboard shortcut for ##Erase all#.")
NORMAL (L"##4.5.17# (19 March 2007)")
LIST_ITEM (L"\\bu Picture window: can change arrow size.")
LIST_ITEM (L"\\bu Several #List commands.")
LIST_ITEM (L"\\bu Spectrum: To SpectrumTier (peaks).")
NORMAL (L"##4.5.16# (22 February 2007)")
LIST_ITEM (L"\\bu Sound-to-Intensity: made resistant against undefined settings.")
LIST_ITEM (L"\\bu Windows: made Ctrl-. available as a shortcut.")
LIST_ITEM (L"\\bu Linux: made it more likely to find the correct fonts.")
NORMAL (L"##4.5.15# (12 February 2007)")
LIST_ITEM (L"\\bu Windows XP: worked around a bug in Windows XP that could cause Praat to crash "
	"when the user moved the mouse pointer over a file in the Desktop in the file selector. "
	"The workaround is to temporarily disable file info tips when the file selector window is on the screen.")
NORMAL (L"##4.5.14# (5 February 2007)")
LIST_ITEM (L"\\bu Scripting: some new predefined string variables like preferencesDirectory\\$ .")
NORMAL (L"##4.5.13# (3 February 2007)")
LIST_ITEM (L"\\bu For stereo sounds, pitch analysis is based on correlations pooled over channels "
	"(rather than on correlations of the channel average).")
LIST_ITEM (L"\\bu For stereo sounds, spectrogram analysis is based on power density averaged across channels "
	"(rather than on the power density of the channel average).")
LIST_ITEM (L"\\bu Scripting: removed a bug introduced in 4.5.06 that caused some variables not to be substituted.")
NORMAL (L"##4.5.12# (30 January 2007)")
LIST_ITEM (L"\\bu Made cross-correlation pitch analysis as fast as it used to be before 4.5.11.")
NORMAL (L"##4.5.11# (29 January 2007)")
LIST_ITEM (L"\\bu Sound objects can be stereo, for better playback quality "
	"(most analyses will work on the averaged mono signal).")
LIST_ITEM (L"\\bu Macintosh: recording a sound now uses CoreAudio instead of SoundManager, "	
	"for more compatibility with modern recording devices, "
	"and the possibility to record with a sampling frequency of 96 kHz.")
LIST_ITEM (L"\\bu @ManPages allow picture scripts with separate object lists.")
LIST_ITEM (L"\\bu Linux: better scroll bars in object list for Lesstif (Debian).")
LIST_ITEM (L"\\bu Linux: made @@Create Strings as file list...@ work on Reiser.")
LIST_ITEM (L"\\bu @sendpraat scripts correctly wait until sounds have played.")
NORMAL (L"##4.5.08# (20 December 2006)")
LIST_ITEM (L"\\bu ExperimentMFC: can use stereo sounds.")
NORMAL (L"##4.5.07# (16 December 2006)")
LIST_ITEM (L"\\bu Macintosh: playing a sound now uses CoreAudio instead of SoundManager.")
LIST_ITEM (L"\\bu Phonetic symbols: \\\'1primary stress and \\\'1secon\\\'2dary stress.")
NORMAL (L"##4.5.06# (13 December 2006)")
LIST_ITEM (L"\\bu Support for 32-bit floating-point WAV files.")
LIST_ITEM (L"\\bu Scripting: removed several kinds of restrictions on string length.")
LIST_ITEM (L"\\bu SSCP: Draw confidence ellipse: corrected a bug that would sometimes not draw the ellipse when %N was very large.")
NORMAL (L"##4.5.05# (5 December 2006)")
LIST_ITEM (L"\\bu Macintosh scripting: European symbols such as \\o\" and \\e' and \\c, are now allowed in file names in scripts "
	"and in MacRoman-encoded file names sent by other programs through the sendpraat subroutine.")
NORMAL (L"##4.5.04# (1 December 2006)")
LIST_ITEM (L"\\bu @@Sound: Change gender...@: corrected a bug that often caused a female-to-male conversion to sound monotonous.")
NORMAL (L"##4.5.03# (29 November 2006)")
LIST_ITEM (L"\\bu Table: added independent-samples t-test.")
LIST_ITEM (L"\\bu Linux: corrected a bug introduced in 4.5.02 that prevented sounds from playing and other weird things.")
NORMAL (L"##4.5.02# (16 November 2006)")
LIST_ITEM (L"\\bu Corrected yet another bug in the new @@Sound: To TextGrid (silences)...@.")
NORMAL (L"##4.5.01# (28 October 2006)")
LIST_ITEM (L"\\bu Sound window: the pitch drawing method is #Curves, #Speckles, or #Automatic.")
LIST_ITEM (L"\\bu Corrected another bug in the new @@Sound: To TextGrid (silences)...@.")
MAN_END

MAN_BEGIN (L"What was new in 4.5?", L"ppgb", 20061026)
NORMAL (L"##4.5# (26 October 2006)")
NORMAL (L"##4.4.35# (20 October 2006)")
LIST_ITEM (L"\\bu In @ManPages you can now draw pictures.")
NORMAL (L"##4.4.34# (19 October 2006)")
LIST_ITEM (L"\\bu Corrected a bug in the new @@Sound: To TextGrid (silences)...@.")
NORMAL (L"##4.4.33# (4 October 2006)")
LIST_ITEM (L"\\bu Windows: corrected a bug introduced in 4.4.31 that caused Praat to skip the first line of the Buttons file.")
NORMAL (L"##4.4.32# (30 September 2006)")
LIST_ITEM (L"\\bu Scripting: more techniques for @@Scripting 4. Object selection|object selection@.")
LIST_ITEM (L"\\bu Scripting: more support for putting the results of the #Info command into a string variable.")
NORMAL (L"##4.4.31# (23 September 2006)")
LIST_ITEM (L"\\bu Support for @@plug-ins@.")
LIST_ITEM (L"\\bu Split between @@Create Strings as file list...@ and @@Create Strings as directory list...@.")
NORMAL (L"##4.4.30# (28 August 2006)")
LIST_ITEM (L"\\bu Table: Draw ellipse (standard deviation)...")
NORMAL (L"##4.4.29# (21 August 2006)")
LIST_ITEM (L"\\bu Allowed \"European\" symbols in file names and object names.")
NORMAL (L"##4.4.28# (10 August 2006)")
LIST_ITEM (L"\\bu Windows XP: Praat files can finally again be opened by double-clicking and by dragging them onto the Praat icon.")
LIST_ITEM (L"\\bu Scripting (Windows): removed a bug that caused Praat to crash if the script window was closed when a file selector window was open.")
NORMAL (L"##4.4.27# (4 August 2006)")
LIST_ITEM (L"\\bu Table window: corrected vertical scroll bar (on Windows).")
LIST_ITEM (L"\\bu Formulas: invSigmoid.")
LIST_ITEM (L"\\bu Logging: added 'power' (and documented the 'freq' command).")
LIST_ITEM (L"\\bu Removed a bug that caused ##Read two Sounds from stereo file...# not to work in scripts.")
NORMAL (L"##4.4.26# (24 July 2006)")
LIST_ITEM (L"\\bu ##Sound & FormantTier: Filter#: much more accurate.")
NORMAL (L"##4.4.25# (16 July 2006)")
LIST_ITEM (L"\\bu TextGrid reading: don't set first boundary to zero for .wrd label files.")
NORMAL (L"##4.4.24# (19 June 2006)")
LIST_ITEM (L"\\bu Scripting: regular expressions allow replacement with empty string.")
NORMAL (L"##4.4.23# (1 June 2006)")
LIST_ITEM (L"\\bu Table: ignore more white space.")
NORMAL (L"##4.4.22# (30 May 2006)")
LIST_ITEM (L"\\bu Scripting: replacing with regular expression. See @@Formulas 5. String functions@.")
NORMAL (L"##4.4.21# (29 May 2006)")
LIST_ITEM (L"\\bu Made Manipulation objects readable again.")
NORMAL (L"##4.4.20# (3 May 2006)")
LIST_ITEM (L"\\bu Removed limit on number of menus (Praat could crash if the number of open windows was high).")
NORMAL (L"##4.4.19# (28 April 2006)")
LIST_ITEM (L"\\bu Table: ##Get mean#, ##Get standard deviation#, ##Get quantile#.")
NORMAL (L"##4.4.18# (24 April 2006)")
LIST_ITEM (L"\\bu Table: ##View & Edit#: view the contents of a table.")
LIST_ITEM (L"\\bu Table: ##Scatter plot#.")
LIST_ITEM (L"\\bu Scripting: more warnings against missing or extra spaces.")
NORMAL (L"##4.4.17# (19 April 2006)")
LIST_ITEM (L"\\bu Table: #Pool: computing averages and medians of dependent variables "
	"for a selected combination of independent variables.")
LIST_ITEM (L"\\bu Table: #Formula accepts string expressions as well as numeric expressions.")
LIST_ITEM (L"\\bu Table: #Sort can sort by any number of columns.")
LIST_ITEM (L"\\bu Table: ##Create with column names#.")
LIST_ITEM (L"\\bu Table: ##Report mean#.")
LIST_ITEM (L"\\bu Formulas: @@Formulas 7. Attributes of objects|row\\$  and col\\$  attributes@.")
LIST_ITEM (L"\\bu Warning when trying to read data files whose format is newer than the Praat version.")
NORMAL (L"##4.4.16# (1 April 2006)")
LIST_ITEM (L"\\bu Spectrum window: dynamic range setting.")
LIST_ITEM (L"\\bu SoundRecorder: corrected a bug in the Intel Mac edition.")
NORMAL (L"##4.4.15# (30 March 2006)")
LIST_ITEM (L"\\bu Source code even more compatible with 64-bit compilers.")
NORMAL (L"##4.4.14# (29 March 2006)")
LIST_ITEM (L"\\bu Source code more compatible with 64-bit compilers.")
NORMAL (L"##4.4.13# (8 March 2006)")
LIST_ITEM (L"\\bu Table To TableOfReal: better handling of --undefined-- values (are now numeric).")
LIST_ITEM (L"\\bu MacOS X: TextGrid files can be double-clicked to open.")
LIST_ITEM (L"\\bu @@Create Strings as file list...@: now handles up to 1,000,000 files per directory.")
NORMAL (L"##4.4.12# (24 February 2006)")
LIST_ITEM (L"\\bu TextGrid: removed a bug introduced in 4.4.10 that caused Praat to crash when converting an IntervalTier into a TextGrid.")
NORMAL (L"##4.4.11# (23 February 2006)")
LIST_ITEM (L"\\bu Listening experiments: removed a bug that could cause Praat to crash when an ExperimentMFC object was removed.")
NORMAL (L"##4.4.10# (20 February 2006)")
LIST_ITEM (L"\\bu Intel computers: corrected reading and writing of 24-bit sound files (error introduced in 4.4.09).")
LIST_ITEM (L"\\bu Create TextGrid: guard against zero tiers.")
LIST_ITEM (L"\\bu MacOS X: correct visibility of Praat icon.")
LIST_ITEM (L"\\bu MacOS X: correct dropping of Praat files on Praat icon.")
NORMAL (L"##4.4.09# (19 February 2006)")
LIST_ITEM (L"\\bu Macintosh: first Intel Macintosh version.")
LIST_ITEM (L"\\bu Windows: Create Strings from directory list...")
NORMAL (L"##4.4.08# (6 February 2006)")
LIST_ITEM (L"\\bu Much improved cepstral smoothing.")
NORMAL (L"##4.4.07# (2 February 2006)")
LIST_ITEM (L"\\bu More scripting facilities (local variables in procedures, e.g. .x and .text\\$ ).")
LIST_ITEM (L"\\bu Faster formulas.")
NORMAL (L"##4.4.06# (30 January 2006)")
LIST_ITEM (L"\\bu More scripting facilities (Object_xxx [ ], Self.nx, Table_xxx\\$  [ ], better messages).")
LIST_ITEM (L"\\bu Better reading and writing of Xwaves label files.")
NORMAL (L"##4.4.05# (26 January 2006)")
LIST_ITEM (L"\\bu @ExperimentMFC: removed a bug that caused Praat to crash when the Oops button was pressed after the experiment finished.")
LIST_ITEM (L"\\bu TextGrid: an IntervalTier can be written to an Xwaves label file.")
NORMAL (L"##4.4.04# (6 January 2006)")
LIST_ITEM (L"\\bu Windows: Quicktime support (see at 4.4.03).")
NORMAL (L"##4.4.03# (6 January 2006)")
LIST_ITEM (L"\\bu Macintosh: Quicktime support, i.e., \"Read from file\" can now read the audio from several kinds of movie files (.mov, .avi).")
NORMAL (L"##4.4.02# (5 January 2006)")
LIST_ITEM (L"\\bu OT learning: allow the decision strategies of Harmonic Grammar and Linear OT.")
NORMAL (L"##4.4.01# (2 January 2006)")
LIST_ITEM (L"\\bu Picture window: \"Logarithmic marks\" allows reversed axes.")
LIST_ITEM (L"\\bu Manipulation window: removed a bug from \"Shift frequencies\" that caused much too small shifts in semitones.")
LIST_ITEM (L"\\bu TextGrid: \"Remove point...\".")
MAN_END

MAN_BEGIN (L"What was new in 4.4?", L"ppgb", 20051219)
NORMAL (L"##4.4# (19 December 2005)")
NORMAL (L"##4.3.37# (15 December 2005)")
LIST_ITEM (L"\\bu @@Principal component analysis@: now accepts tables with more variables (columns) than cases (rows).")
LIST_ITEM (L"\\bu TIMIT label files: removed a bug that caused Praat to crash for files whose first part was not labelled.")
NORMAL (L"##4.3.36# (11 December 2005)")
LIST_ITEM (L"\\bu Ltas: Average.")
LIST_ITEM (L"\\bu Optimality Theory: compute crucial rankings (select OTGrammar + PairDistribution).")
NORMAL (L"##4.3.35# (8 December 2005)")
LIST_ITEM (L"\\bu @ExperimentMFC: switched off warnings for stereo files.")
NORMAL (L"##4.3.34# (8 December 2005)")
LIST_ITEM (L"\\bu Sound window: the arrow scroll step is settable.")
LIST_ITEM (L"\\bu You can now run multiple listening experiments (@ExperimentMFC) in one \"Run\".")
LIST_ITEM (L"\\bu Formant: Get quantile of bandwidth...")
NORMAL (L"##4.3.33# (6 December 2005)")
LIST_ITEM (L"\\bu Removed three bugs introduced in 4.3.32 in @ExperimentMFC.")
NORMAL (L"##4.3.32# (5 December 2005)")
LIST_ITEM (L"\\bu Many more possibilities in @ExperimentMFC.")
NORMAL (L"##4.3.31# (27 November 2005)")
LIST_ITEM (L"\\bu @@Sound: To Ltas (pitch-corrected)...@")
NORMAL (L"##4.3.30# (18 November 2005)")
LIST_ITEM (L"\\bu TableOfReal: Scatter plot: allows reversed axes.")
NORMAL (L"##4.3.29# (11 November 2005)")
LIST_ITEM (L"\\bu Windows: many more font sizes.")
NORMAL (L"##4.3.28# (7 November 2005)")
LIST_ITEM (L"\\bu Fontless EPS files: corrected character width for Symbol font (depended on SILIPA setting).")
LIST_ITEM (L"\\bu Windows: more reliable detection of home directory.")
NORMAL (L"##4.3.27# (7 October 2005)")
LIST_ITEM (L"\\bu TextGrid & Pitch: draw with text alignment.")
NORMAL (L"##4.3.26# (29 September 2005)")
LIST_ITEM (L"\\bu Macintosh: corrected error introduced in 4.3.25.")
NORMAL (L"##4.3.25# (28 September 2005)")
LIST_ITEM (L"\\bu Macintosh: allowed recording with sample rates of 12 and 64 kHz.")
NORMAL (L"##4.3.24# (26 September 2005)")
LIST_ITEM (L"\\bu Table: Down to TableOfReal...: one column of the Table can be used as the row labels for the TableOfReal, "
	"and the strings in the remaining columns of the Table are replaced with whole numbers assigned in alphabetical order.")
NORMAL (L"##4.3.23# (24 September 2005)")
LIST_ITEM (L"\\bu Read Table from comma-separated file...")
LIST_ITEM (L"\\bu Read Table from tab-separated file...")
LIST_ITEM (L"\\bu Write picture as fontless EPS file: choice between XIPA and SILIPA93.")
LIST_ITEM (L"\\bu Bold IPA symbols in EPS files (fontless SILIPA93 only).")
NORMAL (L"##4.3.22# (8 September 2005)")
LIST_ITEM (L"\\bu Macintosh: variable scroll bar size (finally, 7 years since System 8.5).")
NORMAL (L"##4.3.21# (1 September 2005)")
LIST_ITEM (L"\\bu Macintosh: error message if any of the fonts Times, Helvetica, Courier and Symbol are unavailable at start-up.")
LIST_ITEM (L"\\bu Renamed \"Control\" menu to \"Praat\" on all platforms (as on the Mac), "
	"to reflect the fact that no other programs than Praat have used the Praat shell for five years.")
LIST_ITEM (L"\\bu Script editor: Undo and Redo buttons (only on the Mac for now).")
LIST_ITEM (L"\\bu Manual: corrected a bug that sometimes caused Praat to crash when trying to print.")
NORMAL (L"##4.3.20# (18 August 2005)")
LIST_ITEM (L"\\bu Log files: include name of editor window.")
NORMAL (L"##4.3.19# (20 July 2005)")
LIST_ITEM (L"\\bu Improved buttons in manual.")
LIST_ITEM (L"\\bu TableOfReal: Read from headerless spreadsheet file: allow row and column labels to be 30,000 rather than 100 characters.")
NORMAL (L"##4.3.18# (12 July 2005)")
LIST_ITEM (L"\\bu Glottal source for sound synthesis, corrected and documented.")
NORMAL (L"##4.3.17# (7 July 2005)")
LIST_ITEM (L"\\bu Glottal source for sound synthesis.")
LIST_ITEM (L"\\bu Multi-level Optimality Theory: parallel evaluation and bidirectional learning.")
NORMAL (L"##4.3.16# (22 June 2005)")
LIST_ITEM (L"\\bu Pitch drawing: corrected logarithmic scales.")
NORMAL (L"##4.3.15# (22 June 2005)")
LIST_ITEM (L"\\bu Graphics: better dotted lines in pitch contours; clipped pitch curves in editor windows.")
LIST_ITEM (L"\\bu Pitch analysis: more different units (semitones %re 1 Hz).")
NORMAL (L"##4.3.14# (14 June 2005)")
LIST_ITEM (L"\\bu Scripting: regular expressions.")
LIST_ITEM (L"\\bu Removed a bug that caused Praat to crash if a proposed object name was longer than 200 characters.")
NORMAL (L"##4.3.13# (19 May 2005)")
LIST_ITEM (L"\\bu Macintosh: an option to switch off screen previews in EPS files.")
LIST_ITEM (L"\\bu Sources: compatibility of makefiles with MinGW (Minimalist GNU for Windows).")
NORMAL (L"##4.3.12# (10 May 2005)")
LIST_ITEM (L"\\bu Some more manual tricks.")
NORMAL (L"##4.3.11# (6 May 2005)")
LIST_ITEM (L"\\bu TextGrid editor: show number of intervals.")
NORMAL (L"##4.3.10# (25 April 2005)")
LIST_ITEM (L"\\bu Table: Get logistic regression.")
NORMAL (L"##4.3.08# (19 April 2005)")
LIST_ITEM (L"\\bu OT learning: store history with \"OTGrammar & Strings: Learn from partial outputs...\".")
NORMAL (L"##4.3.07# (31 March 2005)")
LIST_ITEM (L"\\bu Linux: removed a bug that could cause a sound to stop playing.")
NORMAL (L"##4.3.04# (9 March 2005)")
LIST_ITEM (L"\\bu Use SIL Doulos IPA 1993/1996 instead of 1989.")
NORMAL (L"##4.3.03# (2 March 2005)")
LIST_ITEM (L"\\bu TextGrid window: green colouring of matching text.")
LIST_ITEM (L"\\bu Regular expressions can be used in many places.")
LIST_ITEM (L"\\bu Pitch analysis: switched off formant-pulling.")
NORMAL (L"##4.3.02# (16 February 2005)")
LIST_ITEM (L"\\bu TextGrid: Remove boundary at time...")
LIST_ITEM (L"\\bu Scripting: corrected %nowarn.")
LIST_ITEM (L"\\bu Linux: guard against blocking audio device.")
LIST_ITEM (L"\\bu Macintosh: guard against out-of-range audio level meter.")
NORMAL (L"##4.3.01# (9 February 2005)")
LIST_ITEM (L"\\bu Replaced PostScript font SILDoulosIPA with XIPA (adapted for Praat by Rafael Laboissi\\e`re).")
LIST_ITEM (L"\\bu Sound: Set part to zero...")
LIST_ITEM (L"\\bu Pitch: To Sound (sine)...")
LIST_ITEM (L"\\bu Sound & TextGrid: Clone time domain.")
MAN_END

MAN_BEGIN (L"What was new in 4.3?", L"ppgb", 20050126)
ENTRY (L"Praat 4.3, 26 January 2005")
	NORMAL (L"General:")
	LIST_ITEM (L"\\bu `Apply' button in settings windows for menu commands and in script forms.")
	LIST_ITEM (L"\\bu Info window can be saved.")
	LIST_ITEM (L"\\bu Removed 30,000-character limit in Info window.")
	NORMAL (L"Phonetics:")
	LIST_ITEM (L"\\bu Speeded up intensity analysis by a factor of 10 "
		"(by making its time resolution 0.01 ms rather than 0.0001 ms at a sampling frequency of 44 kHz).")
	LIST_ITEM (L"\\bu Speeded up pitch analysis and spectrogram analysis by a factor of two.")
	LIST_ITEM (L"\\bu Sound: To Spectrum... now has a reasonably fast non-FFT version.")
	LIST_ITEM (L"\\bu Calibrated long-term average spectrum (Sound: To Ltas...).")
	LIST_ITEM (L"\\bu Pitch-corrected LTAS analysis.")
	LIST_ITEM (L"\\bu Sound: Scale intensity.")
	LIST_ITEM (L"\\bu PitchTier: To Sound (sine).")
	LIST_ITEM (L"\\bu Better warnings against use of the LPC object.")
	NORMAL (L"Voice:")
	LIST_ITEM (L"\\bu July 9, 2004 (4.2.08): Shimmer measurements: more accurate and less sensitive to additive noise.")
	LIST_ITEM (L"\\bu More extensive voice report: pitch statistics; harmonicity.")
	NORMAL (L"Audio:")
	LIST_ITEM (L"\\bu Reading and opening 24-bit and 32-bit sound files (saving still goes in 16 bits).")
	LIST_ITEM (L"\\bu LongSound: save separate channels.")
	LIST_ITEM (L"\\bu Macintosh: much faster reading of WAV files.")
	NORMAL (L"Listening experiments:")
	LIST_ITEM (L"\\bu Subjects can now respond with keyboard presses.")
	NORMAL (L"Graphics:")
	LIST_ITEM (L"\\bu One can now drag the inner viewport in the Picture window, excluding the margins. "
		"This is nice e.g. for creating square viewports or for drawing a waveform and a spectrogram in close contact.")
	LIST_ITEM (L"\\bu Unix: picture highlighting as on Mac and Windows.")
	LIST_ITEM (L"\\bu More drawing methods for Sound and Ltas (curve, bars, poles, speckles).")
	NORMAL (L"OT learning:")
	LIST_ITEM (L"\\bu Monitor rankings when learning.")
	LIST_ITEM (L"\\bu OTGrammar: Remove harmonically bounded candidates...")
	LIST_ITEM (L"\\bu OTGrammar: Save as headerless spreadsheet file...")
	LIST_ITEM (L"\\bu Metrics grammar: added *Clash, *Lapse, WeightByPosition and *MoraicConsonant.")
	NORMAL (L"Scripting:")
	LIST_ITEM (L"\\bu nowarn, noprogress, nocheck.")
	LIST_ITEM (L"\\bu Line numbers.")
MAN_END

MAN_BEGIN (L"What was new in 4.2?", L"ppgb", 20040304)
ENTRY (L"Praat 4.2, 4 March 2004")
	NORMAL (L"General:")
	LIST_ITEM (L"\\bu July 10, 2003: Open source code (General Public Licence).")
	NORMAL (L"Phonetics:")
	LIST_ITEM (L"\\bu Faster computation of spectrum, spectrogram, and pitch.")
	LIST_ITEM (L"\\bu More precision in numeric libraries.")
	LIST_ITEM (L"\\bu PitchTier: Interpolate quadratically.")
	LIST_ITEM (L"\\bu TextGrids can be saved chronologically (and Praat can read that file again).")
	LIST_ITEM (L"\\bu Sound editor window @@Time step settings...@: \"Automatic\", \"Fixed\", and \"View-dependent\".")
	LIST_ITEM (L"\\bu Sound window: distinguish basic from advanced spectrogram and pitch settings.")
	LIST_ITEM (L"\\bu Read TableOfReal from headerless spreadsheet file...: cells with strings are considered zero.")
	LIST_ITEM (L"\\bu Sound window: introduced time step as advanced setting.")
	LIST_ITEM (L"\\bu Sound window: reintroduced view range as advanced setting.")
	LIST_ITEM (L"\\bu Ltas: Compute trend line, Subtract trend line.")
	NORMAL (L"Audio:")
	LIST_ITEM (L"\\bu Sun workstations: support audio servers.")
	NORMAL (L"Graphics:")
	LIST_ITEM (L"\\bu Better selections in Picture window and editor windows.")
	LIST_ITEM (L"\\bu Picture window: better handling of rectangles and ellipses for reversed axes.")
	LIST_ITEM (L"\\bu Windows: corrected positioning of pictures on clipboard and in metafiles.")
	LIST_ITEM (L"\\bu Windows: EPS files check availability of Times and TimesNewRomanPSMT.")
	LIST_ITEM (L"\\bu Polygon: can now also paint in colour instead of only in grey values.")
	LIST_ITEM (L"\\bu Unlimited number of points for polygons in PostScript (may not work on very old printers).")
	LIST_ITEM (L"\\bu Picture window: line widths on all printers and clipboards are now equal to line widths used on PostScript printers: "
		"a line with a line width of \"1\" will be drawn with a width 3/8 points. This improves the looks of pictures printed "
		"on non-PostScript printers, improves the looks of pictures copied to your wordprocessor when printed, "
		"and changes the looks of pictures copied to your presentation program.")
	NORMAL (L"OT learning:")
	LIST_ITEM (L"\\bu Metrics grammar supports \'impoverished overt forms\', "
		"i.e. without secondary stress even if surface structures do have secondary stress.")
	LIST_ITEM (L"\\bu Support for crucially tied constraints and tied candidates.")
	LIST_ITEM (L"\\bu Support for backtracking in EDCD.")
	LIST_ITEM (L"\\bu Queries for testing grammaticality.")
	NORMAL (L"Scripting:")
	LIST_ITEM (L"\\bu ManPages: script links can receive arguments.")
	LIST_ITEM (L"\\bu ManPages: variable duration of recording.")
	LIST_ITEM (L"\\bu Support for unlimited size of script files in editor window on Windows XP and MacOS X (the Unix editions already had this).")
	LIST_ITEM (L"\\bu Improved the reception of %sendpraat commands on Windows XP.")
MAN_END

MAN_BEGIN (L"What was new in 4.1?", L"ppgb", 20030605)
ENTRY (L"Praat 4.1, 5 June 2003")
	NORMAL (L"General:")
	LIST_ITEM (L"\\bu MacOS X edition.")
	LIST_ITEM (L"\\bu Removed licensing.")
	LIST_ITEM (L"\\bu More than 99 percent of the source code distributed under the General Public Licence.")
	LIST_ITEM (L"\\bu Windows 2000 and XP: put preferences files in home directory.")
	NORMAL (L"Phonetics:")
	LIST_ITEM (L"\\bu Spectrum: the sign of the Fourier transform has changed, to comply with common use "
		"in technology and physics. Old Spectrum files are converted when read.")
	LIST_ITEM (L"\\bu Spectral moments.")
	LIST_ITEM (L"\\bu Many jitter and shimmer measures, also in the Sound editor window.")
	LIST_ITEM (L"\\bu PitchTier: shift or multiply frequencies (also in ManipulationEditor).")
	LIST_ITEM (L"\\bu TextGrid: shift times, scale times.")
	LIST_ITEM (L"\\bu Overlap-add synthesis: reduced buzz in voiceless parts.")
	LIST_ITEM (L"\\bu @@Sound: Change gender...")
	LIST_ITEM (L"\\bu Editors: @@Intro 3.6. Viewing a spectral slice@.")
	LIST_ITEM (L"\\bu Editors: Get spectral power at cursor cross.")
	LIST_ITEM (L"\\bu @@Sound: To PointProcess (periodic, peaks)...@")
	LIST_ITEM (L"\\bu Ltas: merge.")
	NORMAL (L"Listening experiments:")
	LIST_ITEM (L"\\bu Goodness judgments.")
	LIST_ITEM (L"\\bu Multiple ResultsMFC: ##To Table#, so that the whole experiment can go into a single statistics file.")
	LIST_ITEM (L"\\bu Stimulus file path can be relative to directory of experiment file.")
	LIST_ITEM (L"\\bu @ExperimentMFC: multiple substimuli for discrimination tests.")
	NORMAL (L"Statistics:")
	LIST_ITEM (L"\\bu New @Table object for column @statistics: Pearson's %r, Kendall's %\\ta-%b, %t-test.")
	LIST_ITEM (L"\\bu Table: scatter plot.")
	LIST_ITEM (L"\\bu Table: column names as variables.")
	LIST_ITEM (L"\\bu @@T-test@.")
	LIST_ITEM (L"\\bu TableOfReal: Extract rows where column...")
	LIST_ITEM (L"\\bu TableOfReal: Get correlation....")
	LIST_ITEM (L"\\bu @@Correlation: Confidence intervals...")
	LIST_ITEM (L"\\bu @@SSCP: Get diagonality (bartlett)...")
	NORMAL (L"OT learning:")
	LIST_ITEM (L"\\bu Tutorial for bidirectional learning.")
	LIST_ITEM (L"\\bu Random choice between equally violating candidates.")
	LIST_ITEM (L"\\bu More constraints in metrics grammar.")
	LIST_ITEM (L"\\bu Learning in editor.")
	NORMAL (L"Graphics:")
	LIST_ITEM (L"\\bu Printing: hard-coded image interpolation for EPS files and PostScript printers.")
	NORMAL (L"Scripting:")
	LIST_ITEM (L"\\bu New @Formulas tutorial.")
	LIST_ITEM (L"\\bu @Formulas: can use variables without quotes.")
	LIST_ITEM (L"\\bu Formulas for PitchTier, IntensityTier, AmplitudeTier, DurationTier.")
	LIST_ITEM (L"\\bu Refer to any matrices and tables in formulas, e.g. Sound_hello (x) or Table_everything [row, col] "
		"or Table_tokens [i, \"F1\"].")
	LIST_ITEM (L"\\bu Assignment by modification, as with += -= *= /=.")
	LIST_ITEM (L"\\bu New functions: date\\$ (), extractNumber, extractWord\\$ , extractLine\\$ . See @@Formulas 5. String functions@.")
	LIST_ITEM (L"\\bu @@Scripting 5.7. Including other scripts@.")
	LIST_ITEM (L"\\bu String formulas in the calculator.")
	LIST_ITEM (L"\\bu Stopped support of things that had been undocumented for the last four years: "
		"#let, #getnumber, #getstring, #ARGS, #copy, #proc, variables with capitals, and strings in numeric variables; "
		"there are messages about how to modify your old scripts.")
	LIST_ITEM (L"\\bu Disallowed ambiguous expressions like -3\\^ 2.")
MAN_END

MAN_BEGIN (L"What was new in 4.0?", L"ppgb", 20011015)
ENTRY (L"Praat 4.0, 15 October 2001")
	NORMAL (L"Editors:")
	LIST_ITEM (L"\\bu Simplified selection and cursor in editor windows.")
	LIST_ITEM (L"\\bu Spectrogram, pitch contour, formant contour, and intensity available in the "
		"Sound, LongSound, and TextGrid editors.")
	LIST_ITEM (L"\\bu TextGrid editor: additions and improvements.")
	LIST_ITEM (L"\\bu @@Log files@.")
	NORMAL (L"Phonetics library:")
	LIST_ITEM (L"\\bu @ExperimentMFC: multiple-forced-choice listening experiments.")
	LIST_ITEM (L"\\bu @@Sound: To Pitch (ac)...@: pitch contour less dependent on time resolution. "
		"This improves the constancy of the contours in the editors when zooming.")
	LIST_ITEM (L"\\bu TextGrid: additions and improvements.")
	LIST_ITEM (L"\\bu Sounds: Concatenate recoverably. Creates a TextGrid whose interval labels are the original "
		"names of the sounds.")
	LIST_ITEM (L"\\bu Sound & TextGrid: Extract all intervals. The reverse of the previous command.")
	LIST_ITEM (L"\\bu Filterbank analyses, @MelFilter, @BarkFilter and "
		"@FormantFilter, by @@band filtering in the frequency domain@." )
	LIST_ITEM (L"\\bu Cepstrum by David Weenink: @MFCC, @LFCC. "
		"@Cepstrum object is a representation of the %%complex cepstrum%.")
	LIST_ITEM (L"\\bu Intensity: To IntensityTier (peaks, valleys).")
	LIST_ITEM (L"\\bu Replaced Analysis and AnalysisEditor with @Manipulation and @ManipulationEditor.")
	NORMAL (L"Phonology library:")
	LIST_ITEM (L"\\bu PairDistribution: Get percentage correct (maximum likelihood, probability matching).")
	LIST_ITEM (L"\\bu OTGrammar & PairDistribution: Get percentage correct...")
	NORMAL (L"Graphics:")
	LIST_ITEM (L"\\bu Improved spectrogram drawing.")
	LIST_ITEM (L"\\bu @@Special symbols@: h\\a'c\\v^ek.")
	LIST_ITEM (L"\\bu Macintosh: improved screen rendition of rotated text.")
	NORMAL (L"Audio:")
	LIST_ITEM (L"\\bu Macintosh: support for multiple audio input devices (sound cards).")
	NORMAL (L"Statistics and numerics library:")
	LIST_ITEM (L"\\bu More statistics by David Weenink.")
	LIST_ITEM (L"\\bu Improved random numbers and other numerical stuff.")
	LIST_ITEM (L"\\bu @@Regular expressions@.")
	NORMAL (L"Scripting:")
	LIST_ITEM (L"\\bu Formatting in variable substitution, e.g. 'pitch:2' gives two digits after the decimal point.")
	LIST_ITEM (L"\\bu Added ##fixed\\$ # to scripting language for formatting of numbers.")
	NORMAL (L"Documentation:")
	LIST_ITEM (L"\\bu @@Multidimensional scaling@ tutorial.")
	LIST_ITEM (L"\\bu Enabled debugging-at-a-distance.")
MAN_END

MAN_BEGIN (L"What was new in 3.9?", L"ppgb", 20001018)
ENTRY (L"Praat 3.9, 18 October 2000")
	NORMAL (L"Editors:")
	LIST_ITEM (L"\\bu Shift-click and shift-drag extend or shrink selection in editor windows.")
	LIST_ITEM (L"\\bu Grouped editors can have separate zooming and scrolling (FunctionEditor preferences).")
	LIST_ITEM (L"\\bu Cursor follows playing sound in editors; interruption by Escape key moves the cursor.")
	LIST_ITEM (L"\\bu TextGridEditor: optimized for transcribing large corpora: text field, directly movable boundaries, "
		"more visible text in tiers, @SpellingChecker, "
		"type while the sound is playing, complete keyboard navigation, control font size, control text alignment, "
		"shift-click near boundary adds interval to selection.")
	LIST_ITEM (L"\\bu Stereo display in LongSound and TextGrid editors.")
	LIST_ITEM (L"\\bu LongSoundEditor and TextGridEditor: write selection to audio file.")
	LIST_ITEM (L"\\bu SoundEditor: added command \"Extract selection (preserve times)\".")
	LIST_ITEM (L"\\bu IntervalTierEditor, DurationTierEditor.")
	LIST_ITEM (L"\\bu Added many query commands in editors.")
	NORMAL (L"Phonetics library:")
	LIST_ITEM (L"\\bu Sound: To Formant...: sample-rate-independent formant analysis.")
	LIST_ITEM (L"\\bu Sound: To Harmonicity (glottal-to-noise excitation ratio).")
	LIST_ITEM (L"\\bu Pitch: support for ERB units, draw all combinations of line/speckle and linear/logarithmic/semitones/mels/erbs, "
		"optionally with TextGrid, Subtract linear fit.")
	LIST_ITEM (L"\\bu Spectrum: Draw along logarithmic frequency axis.")
	LIST_ITEM (L"\\bu TextGrid:  modification commands, Extract part, Shift to zero, Scale times (with Sound or LongSound).")
	LIST_ITEM (L"\\bu @@Matrix: To TableOfReal@, Draw contour...")
	LIST_ITEM (L"\\bu Concatenate Sound and LongSound objects.")
	LIST_ITEM (L"\\bu File formats: save PitchTier in spreadsheet format, read CGN syntax files (XML version), "
		"text files now completely file-server-safe (independent from Windows/Macintosh/Unix line separators).")
	NORMAL (L"Statistics and numerics library:")
	LIST_ITEM (L"\\bu @@Principal component analysis@.")
	LIST_ITEM (L"\\bu @@Discriminant analysis@.")
	LIST_ITEM (L"\\bu @Polynomial: drawing, @@Roots|root@ finding etc.")
	LIST_ITEM (L"\\bu @@TableOfReal: Draw box plots...@.")
	LIST_ITEM (L"\\bu @@Covariance: To TableOfReal (random sampling)...@.")
	LIST_ITEM (L"\\bu @@SSCP: Get sigma ellipse area...@.")
	LIST_ITEM (L"\\bu Query @DTW for 'weighted distance' of time warp.")
	LIST_ITEM (L"\\bu Distributions: To Strings (exact)...")
	LIST_ITEM (L"\\bu Strings: Randomize.")
	NORMAL (L"Phonology library:")
	LIST_ITEM (L"\\bu OTGrammar: To PairDistribution.")
	NORMAL (L"Graphics:")
	LIST_ITEM (L"\\bu Full support for colour inkjet printers on Windows and Macintosh.")
	LIST_ITEM (L"\\bu Full support for high-resolution colour clipboards and metafiles for "
		"Windows and Macintosh programs that support them (this include MS Word "
		"for Windows, but unfortunately not MS Word for Macintosh).")
	LIST_ITEM (L"\\bu Colour in EPS files.")
	LIST_ITEM (L"\\bu Interpolating grey images, i.e. better zoomed spectrograms.")
	LIST_ITEM (L"\\bu Linux: support for 24-bits screens.")
	NORMAL (L"Audio:")
	LIST_ITEM (L"\\bu Asynchronous sound play.")
	LIST_ITEM (L"\\bu Linux: solved problems with /dev/mixer (\"Cannot read MIC gain.\") on many computers.")
	LIST_ITEM (L"\\bu Added possibility of zero padding for sound playing, "
		"in order to reduce clicks on some Linux and Sun computers.")
	LIST_ITEM (L"\\bu LongSound supports mono and stereo, 8-bit and 16-bit, %\\mu-law and A-law, "
		"big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.")
	LIST_ITEM (L"\\bu \"Read two Sounds from stereo file...\" supports 8-bit and 16-bit, %\\mu-law and A-law, "
		"big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.")
	LIST_ITEM (L"\\bu SoundRecorder writes to 16-bit AIFC, WAV, NeXT/Sun, and NIST mono and stereo files.")
	LIST_ITEM (L"\\bu Sound & LongSound: write part or whole to mono or stereo audio file.")
	LIST_ITEM (L"\\bu Read Sound from raw Alaw file.")
	LIST_ITEM (L"\\bu Artword & Speaker (& Sound) movie: real time on all platforms.")
	NORMAL (L"Scripting:")
	LIST_ITEM (L"\\bu @@Formulas 4. Mathematical functions@: added statistical functions: %\\ci^2, Student T, Fisher F, binomial, "
		"and their inverse functions.")
	LIST_ITEM (L"\\bu Windows: program #praatcon for use as a Unix-style console application.")
	LIST_ITEM (L"\\bu Windows and Unix: Praat can be run with a command-line interface without quitting on errors.")
	LIST_ITEM (L"\\bu Unix & Windows: can use <stdout> as a file name (supports pipes for binary data).")
	LIST_ITEM (L"\\bu @sendpraat now also for Macintosh.")
	LIST_ITEM (L"\\bu @@Scripting 6.7. Sending a message to another program|sendsocket@.")
	LIST_ITEM (L"\\bu @@Read from file...@ recognizes script files if they begin with \"\\# !\".")
	LIST_ITEM (L"\\bu Script links in @ManPages.")
	NORMAL (L"Documentation")
	LIST_ITEM (L"\\bu Tutorials on all subjects available through @Intro.")
MAN_END

MAN_BEGIN (L"What was new in 3.8?", L"ppgb", 19990112)
ENTRY (L"Praat 3.8, 12 January 1999")
	NORMAL (L"Phonetics library")
	LIST_ITEM (L"\\bu New objects: @LongSound (view and label long sound files), with editor; PairDistribution.")
	LIST_ITEM (L"\\bu @@Overlap-add@ manipulation of voiceless intervals, version 2: quality much better now; "
		"target duration is exactly as expected from Duration tier or specified lengthening in @@Sound: Lengthen (overlap-add)...@.")
	LIST_ITEM (L"\\bu Audio: Escape key stops audio playing (on Mac also Command-period).")
	LIST_ITEM (L"\\bu @SoundRecorder: allows multiple recordings without close; Play button; Write buttons; buffer size can be set.")
	LIST_ITEM (L"\\bu Reverse a Sound or a selection of a Sound.")
	LIST_ITEM (L"\\bu @@Sound: Get nearest zero crossing...@.")
	LIST_ITEM (L"\\bu Formant: \"Scatter plot (reversed axes)...\".")
	LIST_ITEM (L"\\bu TextGrid & Pitch: \"Speckle separately...\".")
	LIST_ITEM (L"\\bu \"Extract Sound selection (preserve times)\" in TextGridEditor.")
	LIST_ITEM (L"\\bu More query commands for Matrix, TableOfReal, Spectrum, PointProcess.")
	NORMAL (L"Phonology library")
	LIST_ITEM (L"\\bu 25-page OT learning tutorial.")
	LIST_ITEM (L"\\bu Made the OT learner 14 times as fast.")
	NORMAL (L"Platforms")
	LIST_ITEM (L"\\bu May 23: Windows beta version.")
	LIST_ITEM (L"\\bu April 24: Windows alpha version.")
	NORMAL (L"Files")
	LIST_ITEM (L"\\bu Read more Kay, Sun (.au), and WAV sound files.")
	LIST_ITEM (L"\\bu \"Read Strings from raw text file...\"")
	LIST_ITEM (L"\\bu @@Create Strings as file list...@.")
	LIST_ITEM (L"\\bu \"Read IntervalTier from Xwaves...\"")
	LIST_ITEM (L"\\bu hidden \"Read from old Windows Praat picture file...\"")
	NORMAL (L"Graphics")
	LIST_ITEM (L"\\bu Use colours (instead of only greys) in \"Paint ellipse...\" etc.")
	LIST_ITEM (L"\\bu More true colours (maroon, lime, navy, teal, purple, olive).")
	LIST_ITEM (L"\\bu Direct printing from Macintosh to PostScript printers.")
	LIST_ITEM (L"\\bu Hyperpage printing to PostScript printers and PostScript files.")
	LIST_ITEM (L"\\bu Phonetic symbols: raising sign, lowering sign, script g, corner, ligature, pointing finger.")
	NORMAL (L"Shell")
	LIST_ITEM (L"\\bu November 4: all dialogs are modeless (which is new for Unix and Mac).")
	LIST_ITEM (L"\\bu September 27: @sendpraat for Windows.")
	NORMAL (L"Scripting")
	LIST_ITEM (L"\\bu January 7: scriptable editors.")
	LIST_ITEM (L"\\bu October 7: file I/O in scripts.")
	LIST_ITEM (L"\\bu August 23: script language includes all the important functions for string handling.")
	LIST_ITEM (L"\\bu June 24: string variables in scripts.")
	LIST_ITEM (L"\\bu June 22: faster look-up of script variables.")
	LIST_ITEM (L"\\bu June 22: unlimited number of script variables.")
	LIST_ITEM (L"\\bu April 5: suspended chopping of trailing spaces.")
	LIST_ITEM (L"\\bu March 29: enabled formulas as arguments to dialogs (also interactive).")
MAN_END

MAN_BEGIN (L"What was new in 3.7?", L"ppgb", 19980324)
ENTRY (L"Praat 3.7, 24 March 1998")
	NORMAL (L"Editors:")
	LIST_ITEM (L"\\bu In all FunctionEditors: drag to get a selection.")
	NORMAL (L"Phonetics library:")
	LIST_ITEM (L"\\bu Many new query (#Get) commands for @Sound, @Intensity, @Harmonicity, @Pitch, "
		"@Formant, @Ltas, @PitchTier, @IntensityTier, @DurationTier, #FormantTier.")
	LIST_ITEM (L"\\bu Many new modification commands.")
	LIST_ITEM (L"\\bu Many new interpolations.")
	LIST_ITEM (L"\\bu Sound enhancements: @@Sound: Lengthen (overlap-add)...@, @@Sound: Deepen band modulation...@")
	LIST_ITEM (L"\\bu @@Source-filter synthesis@ tutorial, @@Sound & IntensityTier: Multiply@, "
		"##Sound & FormantTier: Filter#, @@Formant: Formula (frequencies)...@, @@Sound: Pre-emphasize (in-line)...@.")
	NORMAL (L"Labelling")
	LIST_ITEM (L"\\bu TextGrid queries (#Get times and labels in a script).")
	LIST_ITEM (L"\\bu @@TextGrid: Count labels...@.")
	LIST_ITEM (L"\\bu @@PointProcess: To TextGrid (vuv)...@: get voiced/unvoiced information from a point process.")
	LIST_ITEM (L"\\bu IntervalTier to TableOfReal: labels become row labels.")
	LIST_ITEM (L"\\bu TextTier to TableOfReal.")
	NORMAL (L"Numerics and statistics library")
	LIST_ITEM (L"\\bu Multidimensional scaling (Kruskal, INDSCAL, etc).")
	LIST_ITEM (L"\\bu @TableOfReal: Set value, Formula, Remove column, Insert column, Draw as squares, To Matrix.")
	NORMAL (L"Phonology library")
	LIST_ITEM (L"\\bu OT learning: new strategies: weighted symmetric plasticity (uncancelled or all).")
	NORMAL (L"Praat shell")
	LIST_ITEM (L"\\bu First Linux version.")
	LIST_ITEM (L"\\bu Eight new functions like e.g. %hertzToBark in @@Formulas 4. Mathematical functions@.")
	LIST_ITEM (L"\\bu @@Praat script@: procedure arguments; object names.")
	NORMAL (L"Documentation:")
	LIST_ITEM (L"\\bu 230 more man pages (now 630).")
	LIST_ITEM (L"\\bu Hypertext: increased readability of formulas, navigation with keyboard.")
MAN_END

MAN_BEGIN (L"What was new in 3.6?", L"ppgb", 19971027)
ENTRY (L"Praat 3.6, 27 October 1997")
	NORMAL (L"Editors:")
	LIST_ITEM (L"\\bu Intuitive position of B and E buttons on left-handed mice.")
	LIST_ITEM (L"\\bu @SoundEditor: copy %windowed selection to list of objects.")
	LIST_ITEM (L"\\bu @SoundEditor: undo Cut, Paste, Zero.")
	LIST_ITEM (L"\\bu @SpectrumEditor: copy band-filtered spectrum or sound to list of objects.")
	LIST_ITEM (L"\\bu @ManipulationEditor: LPC-based pitch manipulation.")
	NORMAL (L"Objects:")
	LIST_ITEM (L"\\bu Use '-', and '+' in object names.")
	NORMAL (L"Phonetics library")
	LIST_ITEM (L"\\bu LPC-based resynthesis in @ManipulationEditor.")
	LIST_ITEM (L"\\bu @Sound: direct modification without formulas (addition, multiplication, windowing)")
	LIST_ITEM (L"\\bu @Sound: filtering in spectral domain by formula.")
	LIST_ITEM (L"\\bu Create a simple @Pitch object from a @PitchTier (for %F__0_) and a @Pitch (for V/U).")
	LIST_ITEM (L"\\bu Semitones in @PitchTier tables.")
	LIST_ITEM (L"\\bu @PointProcess: transplant time domain from @Sound.")
	LIST_ITEM (L"\\bu Much more...")
	NORMAL (L"Phonology library")
	LIST_ITEM (L"\\bu Computational Optimality Theory. See @@OT learning@.")
	NORMAL (L"Hypertext")
	LIST_ITEM (L"\\bu You can use @ManPages files for creating your own tutorials. "
		"These contains buttons for playing and recording sounds, so you can use this for creating "
		"an interactive IPA sound training course.")
	NORMAL (L"Scripting:")
	LIST_ITEM (L"\\bu Programmable @@Praat script@ language: variables, expressions, control structures, "
		"procedures, complete dialog box, exchange of information with Info window, continuation lines.")
	LIST_ITEM (L"\\bu Use platform-independent relative file paths in @@Praat script@.")
	LIST_ITEM (L"\\bu @ScriptEditor: Run selection.")
	NORMAL (L"Graphics:")
	LIST_ITEM (L"\\bu Rotation and scaling while printing the @@Picture window@.")
	LIST_ITEM (L"\\bu Apart from bold and italic, now also bold-italic (see @@Text styles@).")
	LIST_ITEM (L"\\bu Rounded rectangles.")
	LIST_ITEM (L"\\bu Conversion of millimetres and world coordinates.")
	LIST_ITEM (L"\\bu Measurement of text widths (screen and PostScript).")
	NORMAL (L"Unix:")
	LIST_ITEM (L"\\bu Use the @sendpraat program for sending messages to running Praat programs.")
	NORMAL (L"Mac:")
	LIST_ITEM (L"\\bu Praat looks best with the new and beautiful System 8.")
MAN_END

MAN_BEGIN (L"What was new in 3.5?", L"ppgb", 19970527)
ENTRY (L"Praat 3.5, 27 May 1997")
	NORMAL (L"New editors:")
	LIST_ITEM (L"\\bu #TextGridEditor replaces and extends LabelEditor: edit points as well as intervals.")
	LIST_ITEM (L"\\bu #AnalysisEditor replaces and extends PsolaEditor: view pitch, spectrum, formant, and intensity "
		"analyses in a single window, and allow pitch and duration resynthesis by overlap-add and more (would be undone in 3.9.19).")
	LIST_ITEM (L"\\bu #SpectrumEditor allows you to view and edit spectra.")
	NORMAL (L"Praat shell:")
	LIST_ITEM (L"\\bu ##History mechanism# remembers all the commands that you have chosen, "
		"and allows you to put them into a script.")
	LIST_ITEM (L"\\bu #ScriptEditor allows you to edit and run any Praat script, and to put it under a button.")
	LIST_ITEM (L"\\bu All added and removed buttons are remembered across sessions.")
	LIST_ITEM (L"\\bu #ButtonEditor allows you to make buttons visible or invisible.")
	NORMAL (L"Evaluations:")
	LIST_ITEM (L"\\bu In his 1996 doctoral thesis, Henning Reetz "
		"compared five pitch analysis routines; @@Sound: To Pitch (ac)...@ appeared to make the fewest errors. "
		"H. Reetz (1996): %%Pitch Perception in Speech: a Time Domain Approach%, Studies in Language and Language Use #26, "
		"IFOTT, Amsterdam (ICG Printing, Dordrecht).")
	NORMAL (L"Documentation:")
	LIST_ITEM (L"\\bu 140 more man pages (now 330).")
	LIST_ITEM (L"\\bu Tables and pictures in manual.")
	LIST_ITEM (L"\\bu Printing the entire manual.")
	LIST_ITEM (L"\\bu Logo.")
	NORMAL (L"New types:")
	LIST_ITEM (L"\\bu Labelling & segmentation: #TextGrid, #IntervalTier, #TextTier.")
	LIST_ITEM (L"\\bu Analysis & manipulation: #Analysis.")
	LIST_ITEM (L"\\bu Statistics: #TableOfReal, #Distributions, #Transition")
	NORMAL (L"File formats:")
	LIST_ITEM (L"\\bu Read and write rational numbers in text files.")
	LIST_ITEM (L"\\bu Read 8-bit .au sound files.")
	LIST_ITEM (L"\\bu Read and write raw 8-bit two\'s-complement and offset-binary sound files.")
	NORMAL (L"Audio:")
	LIST_ITEM (L"\\bu 16-bit interactive Sound I/O on Mac.")
	LIST_ITEM (L"\\bu Record sounds at 9.8 kHz on SGI.")
	NORMAL (L"New commands:")
	LIST_ITEM (L"\\bu Two more pitch-analysis routines.")
	LIST_ITEM (L"\\bu Sound to PointProcess: collect all maxima, minima, zero crossings.")
	LIST_ITEM (L"\\bu PointProcess: set calculus.")
	LIST_ITEM (L"\\bu TextGrid: extract time-point information.")
	LIST_ITEM (L"\\bu Compute pitch or formants at given time points.")
	LIST_ITEM (L"\\bu Put pitch, formants etc. in tables en get statistics.")
	LIST_ITEM (L"\\bu Many more...")
	NORMAL (L"Macintosh:")
	LIST_ITEM (L"\\bu 16-bit interactive sound I/O.")
	LIST_ITEM (L"\\bu Fast and interpolating spectrogram drawing.")
	LIST_ITEM (L"\\bu Phonetic Mac screen font included in source code (as a fallback to using SIL Doulos IPA).")
	LIST_ITEM (L"\\bu Keyboard shortcuts, text editor, help under question mark, etc.")
MAN_END

MAN_BEGIN (L"What was new in 3.3?", L"ppgb", 19961006)
ENTRY (L"Praat 3.3, 6 October 1996")
	LIST_ITEM (L"\\bu Documentation: hypertext help browser, including the first 190 man pages.")
	LIST_ITEM (L"\\bu New editors: type #TextTier for labelling times instead of intervals.")
	LIST_ITEM (L"\\bu New actions: #Formant: Viterbi tracker, Statistics menu, Scatter plot.")
	LIST_ITEM (L"\\bu Evaluation: For HNR analysis of speech, the cross-correlation method, "
		"which has a sensitivity of 60 dB and a typical time resolution of 12 milliseconds, "
		"must be considered better than the autocorrelation method, "
		"which has a better sensitivity (80 dB), but a much worse time resolution (30 ms). "
		"For pitch analysis, the autocorrelation method still beats the cross-correlation method "
		"because of its better resistance against noise and echos, "
		"and despite its marginally poorer resolution (15 vs. 12 ms).")
	LIST_ITEM (L"\\bu User preferences are saved across sessions.")
	LIST_ITEM (L"\\bu The phonetic X screen font included in the source code.")
	LIST_ITEM (L"\\bu Xwindows resources included in the source code")
	LIST_ITEM (L"\\bu Graphics: eight colours, small caps, text rotation.")
	LIST_ITEM (L"\\bu File formats: Sun/NexT mu-law files, raw matrix text files, Xwaves mark files.")
	LIST_ITEM (L"\\bu Accelerations: keyboard shortcuts, faster dynamic menu, Shift-OK keeps file selector on screen.")
	LIST_ITEM (L"\\bu Object type changes: #StylPitch and #MarkTier are now called #PitchTier and #TextTier, respectively. "
		"Old files can still be read.")
	LIST_ITEM (L"\\bu Script warning: all times in dialogs are in seconds now: milliseconds have gone.")
MAN_END

MAN_BEGIN (L"What was new in 3.2?", L"ppgb", 19960429)
ENTRY (L"Praat 3.2, 29 April 1996")
	LIST_ITEM (L"\\bu Sound I/O for HPUX, Sun Sparc 5, and Sun Sparc LX.")
	LIST_ITEM (L"\\bu Cross-correlation pitch and HNR analysis.")
	LIST_ITEM (L"\\bu Facilities for generating tables from scripts.")
	LIST_ITEM (L"\\bu Editing and playing stylized pitch contours and point processes.")
	LIST_ITEM (L"\\bu Overlap-add pitch manipulation.")
	LIST_ITEM (L"\\bu Spectral smoothing techniques: cepstrum and LPC.")
	LIST_ITEM (L"\\bu Time-domain pitch analysis with jitter measurement.")
	LIST_ITEM (L"\\bu Read and write Bell-Labs sound files and Kay CSL audio files.")
	LIST_ITEM (L"\\bu Replaced IpaTimes font by free SILDoulos-IPA font, and embedded phonetic font in PostScript picture.")
	LIST_ITEM (L"\\bu Completed main phonetic characters.")
MAN_END

MAN_BEGIN (L"What was new in 3.1?", L"ppgb", 19951205)
ENTRY (L"Praat 3.1, 5 December 1995")
	LIST_ITEM (L"\\bu Add and remove buttons dynamically.")
	LIST_ITEM (L"\\bu DataEditor (Inspect button).")
	LIST_ITEM (L"\\bu Initialization scripts.")
	LIST_ITEM (L"\\bu Logarithmic axes.")
	LIST_ITEM (L"\\bu Call remote ADDA server directly.")
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

ENTRY (L"To do")
	LIST_ITEM (L"\\bu TextGrid & Sound: Extract intervals with margins.")
	LIST_ITEM (L"\\bu Spectrum: draw power, re, im, phase.")
	LIST_ITEM (L"\\bu Formant: To Spectrum (slice)... (combines Formant-to-LPC and LPC-to-Spectrum-slice)")
	LIST_ITEM (L"\\bu Read and/or write Matlab files, MBROLA files, Xwaves files, CHAT files.") // Aix
	LIST_ITEM (L"\\bu Matrix: draw numbers.")
	LIST_ITEM (L"\\bu Fractions with \\bsf{a|b}.")
	LIST_ITEM (L"\\bu Move objects up and down list.")
	LIST_ITEM (L"\\bu Spectrogram cross-correlation.")
	LIST_ITEM (L"\\bu Labels in AIFC file.") // Theo Veenker 19980323
	LIST_ITEM (L"\\bu Improve scrolling and add selection in hyperpages.")
	LIST_ITEM (L"\\bu Segment spectrograph?") // Ton Wempe, Jul 16 1996
	LIST_ITEM (L"\\bu Phoneme-to-articulation conversion??") // Mirjam Ernestus, Jul 1 1996
ENTRY (L"Known bugs in the Windows version")
	LIST_ITEM (L"\\bu Cannot stand infinitesimal zooming in SpectrogramEditor.")
	LIST_ITEM (L"\\bu Clipboards with greys sometimes become black-and-white after use of colour.")
ENTRY (L"Known bugs in the Linux version")
	LIST_ITEM (L"\\bu Sounds shorter than 200 ms do not always play (workaround: add zeroes in prefs).")
	LIST_ITEM (L"\\bu Keyboard shortcuts do not work if NumLock is on.")
*/
 
MAN_BEGIN (L"Acknowledgments", L"ppgb", 20130406)
NORMAL (L"The following people contributed source code to Praat:")
LIST_ITEM (L"Paul Boersma: user interface, graphics, @printing, @@Intro|sound@, "
	"@@Intro 3. Spectral analysis|spectral analysis@, @@Intro 4. Pitch analysis|pitch analysis@, "
	"@@Intro 5. Formant analysis|formant analysis@, @@Intro 6. Intensity analysis|intensity analysis@, "
	"@@Intro 7. Annotation|annotation@, @@Intro 8. Manipulation|speech manipulation@, @@voice|voice report@, "
	"@@ExperimentMFC|listening experiments@, "
	"@@articulatory synthesis@, @@OT learning|optimality-theoretic learning@, "
	"tables, @formulas, @scripting, and adaptation of PortAudio, GLPK, and regular expressions.")
LIST_ITEM (L"David Weenink: "
	"@@feedforward neural networks@, @@principal component analysis@, @@multidimensional scaling@, @@discriminant analysis@, @LPC, "
	"@VowelEditor, "
	"and adaptation of GSL, LAPACK, fftpack, regular expressions, and Espeak.")
LIST_ITEM (L"Stefan de Konink and Franz Brau\\sse: major help in port to GTK.")
LIST_ITEM (L"Tom Naughton: major help in port to Cocoa.")
LIST_ITEM (L"Erez Volk: adaptation of FLAC and MAD.")
LIST_ITEM (L"Ola S\\o\"der: @@kNN classifiers@, @@k-means clustering@.")
LIST_ITEM (L"Rafael Laboissi\\e`re: adaptation of XIPA.")
LIST_ITEM (L"Darryl Purnell created the first version of audio for Praat for Linux.")
NORMAL (L"We included the following freely available software libraries in Praat (sometimes with adaptations):")
LIST_ITEM (L"XIPA: IPA font for Unix by Fukui Rei (GPL).")
LIST_ITEM (L"GSL: GNU Scientific Library by Gerard Jungman and Brian Gough (GPL).")
LIST_ITEM (L"GLPK: GNU Linear Programming Kit by Andrew Makhorin (GPL).")
LIST_ITEM (L"PortAudio: Portable Audio Library by Ross Bencina, Phil Burk, Bjorn Roche, Dominic Mazzoni, Darren Gibbs.")
LIST_ITEM (L"Espeak: text-to-speech synthesizer by Jonathan Duddington (GPL).")
LIST_ITEM (L"MAD: MPEG Audio Decoder by Underbit Technologies (GPL).")
LIST_ITEM (L"FLAC: Free Lossless Audio Codec by Josh Coalson.")
LIST_ITEM (L"fftpack: public domain Fourier transforms by Paul Swarztrauber and Monty.")
LIST_ITEM (L"LAPACK: public domain numeric algorithms by Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., "
	"Courant Institute, Argonne National Lab, and Rice University.")
LIST_ITEM (L"Regular expressions by Henry Spencer, Mark Edel, Christopher Conrad, Eddy De Greef (GPL).")
NORMAL (L"For technical support and advice:")
LIST_ITEM (L"Ton Wempe, Dirk Jan Vet.")
NORMAL (L"For their financial support during the development of Praat:")
LIST_ITEM (L"Netherlands Organization for Scientific Research (NWO) (1996\\--1999).")
LIST_ITEM (L"Nederlandse Taalunie (2006\\--2008).")
LIST_ITEM (L"Talkbank project, Carnegie Mellon / Linguistic Data Consortium (2002\\--2003).")
LIST_ITEM (L"Spoken Dutch Corpus (CGN) (1999\\--2001).")
LIST_ITEM (L"Laboratorium Experimentele OtoRhinoLaryngologie, KU Leuven.")
LIST_ITEM (L"DFG-Projekt Dialektintonation, Universit\\a\"t Freiburg.")
LIST_ITEM (L"Department of Linguistics and Phonetics, Lund University.")
LIST_ITEM (L"Centre for Cognitive Neuroscience, University of Turku.")
LIST_ITEM (L"Linguistics Department, University of Joensuu.")
LIST_ITEM (L"Laboratoire de Sciences Cognitives et Psycholinguistique, Paris.")
LIST_ITEM (L"Department of Linguistics, Northwestern University.")
LIST_ITEM (L"Department of Finnish and General Linguistics, University of Tampere.")
LIST_ITEM (L"Institute for Language and Speech Processing, Paradissos Amaroussiou.")
LIST_ITEM (L"J\\o\"rg Jescheniak, Universit\\a\"t Leipzig.")
LIST_ITEM (L"The Linguistics Teaching Laboratory, Ohio State University.")
LIST_ITEM (L"Linguistics & Cognitive Science, Dartmouth College, Hanover NH.")
LIST_ITEM (L"Cornell Phonetics Lab, Ithaca NY.")
NORMAL (L"Finally:")
LIST_ITEM (L"Daniel Hirst, for managing the Praat Discussion list.")
MAN_END

MAN_BEGIN (L"Praat menu", L"ppgb", 20050822)
INTRO (L"The first menu in the @@Object window@. On MacOS X, this menu is in the main menu bar.")
MAN_END

MAN_BEGIN (L"Copy...", L"ppgb", 20111018)
INTRO (L"One of the fixed buttons in the @@Object window@.")
ENTRY (L"Availability")
NORMAL (L"You can choose this command after selecting one object of any type.")
ENTRY (L"Behaviour")
NORMAL (L"The Object window copies the selected object, and all the data it contains, "
	"to a new object, which will appear at the bottom of the List of Objects.")
ENTRY (L"Example")
NORMAL (L"If you select \"Sound hallo\" and click `Copy...', "
	"a command window will appear, which prompts you for a name; "
	"after you click OK, a new object will appear in the list, bearing that name.")
MAN_END

MAN_BEGIN (L"Draw menu", L"ppgb", 20010417)
INTRO (L"A menu that occurs in the @@Dynamic menu@ for many objects.")
NORMAL (L"This menu contains commands for drawing the object to the @@Picture window@, "
	"which will allow you to print the drawing or to copy it to your word processor.")
MAN_END

MAN_BEGIN (L"Dynamic menu", L"ppgb", 20110131)
INTRO (L"A column of buttons in the right-hand part of the @@Object window@, "
	"plus the #Save menu in the Object window.")
NORMAL (L"If you select one or more @objects in the list, "
	"the possible actions that you can perform with the selected objects "
	"will appear in the dynamic menu. "
	"These actions can include viewing & editing, saving, drawing, "
	"conversions to other types (including analysis and synthesis), and more.")
ENTRY (L"Example of analysis:")
NORMAL (L"Record a Sound, select it, and click on ##To Pitch...#. "
	"This will create a new Pitch object and put it in the list of objects. "
	"You can then edit, write, and draw this Pitch object.")
ENTRY (L"Example of synthesis:")
NORMAL (L"Create a #Speaker, create and edit an #Artword, and click on ##To Sound...#.")
MAN_END

MAN_BEGIN (L"View & Edit", L"ppgb", 20110128)
INTRO (L"A command in the @@Dynamic menu@ of several types of @objects.")
NORMAL (L"This command puts an @@Editors|editor@ window on the screen, which shows the contents of the selected object. "
	"This window will allow your to view and modify the contents of this object.")
MAN_END

MAN_BEGIN (L"Extract visible formant contour", L"ppgb", 20030316)
INTRO (L"One of the commands in the Formant menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (L"See @@Intro 5. Formant analysis@")
MAN_END

MAN_BEGIN (L"Extract visible intensity contour", L"ppgb", 20030316)
INTRO (L"One of the commands in the Intensity menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (L"See @@Intro 6. Intensity analysis@")
MAN_END

MAN_BEGIN (L"Extract visible pitch contour", L"ppgb", 20030316)
INTRO (L"One of the commands in the Pitch menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (L"See @@Intro 4. Pitch analysis@")
MAN_END

MAN_BEGIN (L"Extract visible spectrogram", L"ppgb", 20030316)
INTRO (L"One of the commands in the Spectrogram menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (L"See @@Intro 3. Spectral analysis@")
MAN_END

MAN_BEGIN (L"FAQ (Frequently Asked Questions)", L"ppgb", 20071210)
LIST_ITEM (L"@@FAQ: How to cite Praat")
LIST_ITEM (L"@Unicode")
LIST_ITEM (L"@@FAQ: Formant analysis")
LIST_ITEM (L"@@FAQ: Pitch analysis")
LIST_ITEM (L"@@FAQ: Spectrograms")
LIST_ITEM (L"@@FAQ: Scripts")
/* Hardware */
/* Pitch: octave errors */
MAN_END

MAN_BEGIN (L"FAQ: Formant analysis", L"ppgb", 20030916)
NORMAL (L"#Problem: I get different formant values if I choose to analyse 3 formants "
	"than if I choose to analyse 4 formants.")
NORMAL (L"Solution: the \"number of formants\" in formant analysis determines the "
	"number of peaks with which the %entire spectrum is modelled. For an average "
	"female voice, you should choose to analyse 5 formants in the region up to 5500 Hz, "
	"even if you are interested only in the first three formants.")
NORMAL (L"")
NORMAL (L"#Problem: I often get only 1 formant in a region where I see clearly 2 formants "
	"in the spectrogram.")
NORMAL (L"This occurs mainly in back vowels (F1 and F2 close together) for male voices, "
	"if the \"maximum formant\" is set to the standard of 5500 Hz, which is appropriate "
	"for female voices. Set the \"maximum formant\" down to 5000 Hz. "
	"No, Praat comes without a guarantee: the formant analysis is based on LPC, "
	"and this comes with several assumptions as to what a speech spectrum is like.")
NORMAL (L"")
NORMAL (L"#Question: what algorithm is used for formant analysis?")
NORMAL (L"Answer: see @@Sound: To Formant (burg)...@.")
MAN_END

MAN_BEGIN (L"FAQ: How to cite Praat", L"ppgb", 20100401)
NORMAL (L"#Question: how do I cite Praat in my articles?")
NORMAL (L"Answer: nowadays most journals allow you to cite computer programs and web sites. "
	"The style approved by the American Psychological Association, "
	"and therefore by many journals, is like the following "
	"(change the dates and version number as needed):")
#define xstr(s) str(s)
#define str(s) #s
NORMAL (L"Boersma, Paul & Weenink, David (" xstr(PRAAT_YEAR) "). "
	"Praat: doing phonetics by computer [Computer program]. "
	"Version " xstr(PRAAT_VERSION_STR) ", retrieved " xstr(PRAAT_DAY) " " xstr(PRAAT_MONTH) " " xstr(PRAAT_YEAR) " from http://www.praat.org/")
NORMAL (L"If the journal does not allow you to cite a web site, then try:")
NORMAL (L"Boersma, Paul (2001). Praat, a system for doing phonetics by computer. "
	"%%Glot International% ##5:9/10#, 341-345.")
MAN_END

MAN_BEGIN (L"FAQ: Pitch analysis", L"ppgb", 20060913)
NORMAL (L"#Question: what algorithm is used for pitch analysis?")
NORMAL (L"Answer: see @@Sound: To Pitch (ac)...@. The 1993 article is downloadable from "
	"http://www.fon.hum.uva.nl/paul/")
NORMAL (L"#Question: why does Praat consider my sound voiceless while I hear it as voiced?")
NORMAL (L"There are at least five possibilities. Most of them can be checked by zooming in on the @waveform.")
NORMAL (L"The first possibility is that the pitch has fallen below the @@pitch floor@. For instance, "
	"your pitch floor could be 75 Hz but the English speaker produces creak at the end of the utterance. "
	"Or your pitch floor could be 75 Hz but the Chinese speaker is in the middle of a third tone. "
	"If this happens, it may help to lower the pitch floor to e.g. 40 Hz (@@Pitch settings...@), "
	"although that may also smooth the pitch curve too much in other places.")
NORMAL (L"The second possibility is that the pitch has moved too fast. This could happen at the end of a Chinese fourth tone, "
	"which drops very fast. If this happens, it may help to use the \"optimize for voice analysis\" setting, "
	"(@@Pitch settings...@), although Praat may then hallucinate pitches in other places that you would prefer to consider voiceless.")
NORMAL (L"The third possibility is that the periods are very irregular, as in some pathological voices. "
	"If you want to see a pitch in those cases, it may help to use the \"optimize for voice analysis\" setting "
	"(@@Pitch settings...@). Or it may help to lower the \"voicing threshold\" setting (@@Advanced pitch settings...@) "
	"to 0.25 (instead of the standard 0.45) or so.")
NORMAL (L"The fourth possibility is that there is a lot of background noise, as in a recording on a busy street. "
	"In such a case, it may help to lower the \"voicing threshold\" setting (@@Advanced pitch settings...@) "
	"to 0.25 (instead of the standard 0.45) or so. The disadvantage of lowering this setting is that for non-noisy "
	"recordings, Praat will become too eager to find voicing in some places that you would prefer to consider voiceless; "
	"so make sure to set it back to 0.45 once you have finished analysing the noisy recordings.")
NORMAL (L"The fifth possibility is that the part analysed as voiceless is much less loud than the rest of the sound, "
	"or that the sound contains a loud noise elsewhere. This can be checked by zooming in on the part analysed as voiceless: "
	"if Praat suddenly considers it as voiced, this is a sign that this part is much quieter than the rest. "
	"To make Praat analyse this part as voiced, you can lower the \"silence threshold\" setting to 0.01 "
	"(instead of the standard 0.03) or so. The disadvantage of lowering this setting is that Praat may start to consider "
	"some distant background sounds (and quiet echos, for instance) as voiced.")
NORMAL (L"#Question: why do I get different results for the maximum pitch if...?")
NORMAL (L"If you select a Sound and choose @@Sound: To Pitch...@, the time step will usually "
	"be 0.01 seconds. The resulting @Pitch object will have values for times that are "
	"0.01 seconds apart. If you then click Info or choose ##Get maximum pitch# from the #Query menu, "
	"the result is based on those time points. By contrast, if you choose ##Get maximum pitch# "
	"from the #Pitch menu in the SoundEditor window, the result will be based on the visible points, "
	"of which there tend to be a hundred in the visible window. These different time spacings will "
	"lead to slightly different pitch contours.")
NORMAL (L"If you choose ##Move cursor to maximum pitch#, then choose ##Get pitch# from the "
	"#%Pitch menu, the result will be different again. This is because ##Get maximum pitch# "
	"can do a parabolic interpolation around the maximum, whereas ##Get pitch#, not realizing "
	"that the cursor is at a maximum, does a stupid linear interpolation, which tends to lead to "
	"lower values.")
MAN_END

MAN_BEGIN (L"FAQ: Scripts", L"ppgb", 20130421)
NORMAL (L"#Question: how do I do something to all the files in a directory?")
NORMAL (L"Answer: look at @@Create Strings as file list...@.")
NORMAL (L"")
NORMAL (L"#Question: why doesn't the editor window react to my commands?")
NORMAL (L"Your commands are probably something like:")
CODE (L"do (\"Read from file...\", \"hello.wav\")")
CODE (L"do (\"View & Edit\")")
CODE (L"do (\"Zoom...\", 0.3, 0.5)")
NORMAL (L"Answer: Praat doesn't know it has to send the #Zoom command to the editor "
	"window called ##Sound hello#. There could be several Sound editor windows on your "
	"screen. According to @@Scripting 7.1. Scripting an editor from a shell script@, "
	"you will have to say this explicitly:")
CODE (L"do (\"Read from file...\", \"hello.wav\")")
CODE (L"do (\"View & Edit\")")
CODE (L"editor Sound hello")
CODE (L"do (\"Zoom...\", 0.3, 0.5)")
NORMAL (L"")
NORMAL (L"#Problem: a line like \"Number = 1\" does not work.")
NORMAL (L"Solution: names of variables should start with a lower-case letter.")
NORMAL (L"")
NORMAL (L"#Question: why do names of variables have to start with a lower-case letter? "
	"I would like to do things like \"F0 = Get mean pitch\".")
NORMAL (L"Answer (using the shorthand script syntax): Praat scripts combine button commands with things that only occur "
	"in scripts. Button commands always start with a capital letter, e.g. \"Play\". "
	"Script commands always start with lower case, e.g. \"echo Hello\". "
	"A minimal pair is \"select\", which simulates a mouse click in the object list, "
	"versus \"Select...\", which sets the selection in editor windows. Variable names "
	"that start with a capital letter would be rather ambiguous in assignments, "
	"as in \"x = Get\", where \"Get\" would be a variable, versus \"x = Get mean\", "
	"where \"Get mean\" is a button command. To prevent this, Praat enforces "
	"a rigorous lower-case/upper-case distinction.")
NORMAL (L"")
NORMAL (L"#Question: how do I convert a number into a string?")
NORMAL (L"Answer: a\\$  = string\\$  (a)")
NORMAL (L"#Question: how do I convert a string into a number?")
NORMAL (L"Answer: a = number (a\\$ )")
MAN_END

MAN_BEGIN (L"FAQ: Spectrograms", L"ppgb", 20030916)
NORMAL (L"#Problem: the background is grey instead of white (too little contrast)")
NORMAL (L"Solution: reduce the \"dynamic range\" in the spectrogram settings. The standard value is 50 dB, "
	"which is fine for detecting small things like plosive voicing in well recorded speech. "
	"For gross features like vowel formants, or for noisy speech, you may want to change the dynamic range "
	"to 40 or even 30 dB.")
MAN_END

MAN_BEGIN (L"File menu", L"ppgb", 20021204)
INTRO (L"One of the menus in all @editors, in the @manual, and in the @@Picture window@.")
MAN_END

MAN_BEGIN (L"Filtering", L"ppgb", 20100324)
INTRO (L"This tutorial describes the use of filtering techniques in Praat. "
	"It assumes you are familiar with the @Intro.")
ENTRY (L"Frequency-domain filtering")
NORMAL (L"Modern computer techniques make possible an especially simple batch filtering method: "
	"multiplying the complex spectrum in the frequency domain by any real-valued filter function. "
	"This leads to a zero phase shift for each frequency component. The impulse response is symmetric "
	"in the time domain, which also means that the filter is %acausal: the filtered signal will show components "
	"before they start in the original.")
LIST_ITEM (L"\\bu @@Spectrum: Filter (pass Hann band)...@")
LIST_ITEM (L"\\bu @@Spectrum: Filter (stop Hann band)...@")
LIST_ITEM (L"\\bu @@Sound: Filter (pass Hann band)...@")
LIST_ITEM (L"\\bu @@Sound: Filter (stop Hann band)...@")
LIST_ITEM (L"\\bu @@Sound: Filter (formula)...@")
NORMAL (L"Spectro-temporal:")
LIST_ITEM (L"\\bu @@band filtering in the frequency domain@")
ENTRY (L"Fast time-domain filtering")
NORMAL (L"Some very fast Infinite Impulse Response (IIR) filters can be defined in the time domain. "
	"These include recursive all-pole filters and pre-emphasis. These filters are causal but have non-zero phase shifts. "
	"There are versions that create new Sound objects:")
LIST_ITEM (L"\\bu @@Sound: Filter (one formant)...@")
LIST_ITEM (L"\\bu @@Sound: Filter (pre-emphasis)...@")
LIST_ITEM (L"\\bu @@Sound: Filter (de-emphasis)...@")
NORMAL (L"And there are in-line versions, which modify the existing Sound objects:")
LIST_ITEM (L"\\bu @@Sound: Filter with one formant (in-line)...@")
LIST_ITEM (L"\\bu @@Sound: Pre-emphasize (in-line)...@")
LIST_ITEM (L"\\bu @@Sound: De-emphasize (in-line)...@")
ENTRY (L"Convolution")
NORMAL (L"A Finite Impulse Response (FIR) filter can be described as a sampled sound. "
	"Filtering with such a filter amounts to a %#convolution of the original sound and the filter:")
LIST_ITEM (L"\\bu @@Sounds: Convolve...@")
ENTRY (L"Described elsewhere")
NORMAL (L"Described in the @@Source-filter synthesis@ tutorial:")
LIST_ITEM (L"\\bu @@Sound & Formant: Filter@")
LIST_ITEM (L"\\bu @@Sound & FormantGrid: Filter@")
LIST_ITEM (L"\\bu @@LPC & Sound: Filter...@")
LIST_ITEM (L"\\bu @@LPC & Sound: Filter (inverse)@")
MAN_END

MAN_BEGIN (L"Formants & LPC menu", L"ppgb", 20011107)
INTRO (L"A menu that occurs in the @@Dynamic menu@ for a @Sound.")
NORMAL (L"This menu contains commands for analysing the formant contours of the selected Sound:")
LIST_ITEM (L"@@Sound: To Formant (burg)...")
LIST_ITEM (L"@@Sound: To Formant (keep all)...")
LIST_ITEM (L"@@Sound: To Formant (sl)...")
LIST_ITEM (L"@@Sound: To LPC (autocorrelation)...")
LIST_ITEM (L"@@Sound: To LPC (covariance)...")
LIST_ITEM (L"@@Sound: To LPC (burg)...")
LIST_ITEM (L"@@Sound: To LPC (marple)...")
LIST_ITEM (L"@@Sound: To MFCC...")
MAN_END

MAN_BEGIN (L"Get first formant", L"ppgb", 20011107)
INTRO (L"One of the commands in the @@Query menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN (L"Get pitch", L"ppgb", 20010417)
INTRO (L"One of the commands in the @@Query menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN (L"Get second formant", L"ppgb", 20011107)
INTRO (L"One of the commands in the @@Query menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN (L"Goodies", L"ppgb", 20050822)
INTRO (L"The title of a submenu of the @@Praat menu@.")
MAN_END

MAN_BEGIN (L"Info", L"ppgb", 19980101)
INTRO (L"One of the fixed buttons in the @@Object window@.")
ENTRY (L"Availability")
NORMAL (L"You can choose this command after choosing one object.")
ENTRY (L"Purpose")
NORMAL (L"To get some information about the selected object.")
ENTRY (L"Behaviour")
NORMAL (L"The information will appear in the @@Info window@.")
MAN_END

MAN_BEGIN (L"Info window", L"ppgb", 20030528)
INTRO (L"A text window into which many query commands write their answers.")
NORMAL (L"You can select text from this window and copy it to other places.")
NORMAL (L"In a @@Praat script@, you can bypass the Info window by having a query command "
	"writing directly into a script variable.")
NORMAL (L"Apart from the @Info command, which writes general information about the selected object, "
	"the following more specific commands also write into the Info window:")
MAN_END

MAN_BEGIN (L"Inspect", L"ppgb", 19960904)
INTRO (L"One of the fixed buttons in the @@Object window@.")
NORMAL (L"You can use this command after selecting one object in the list.")
NORMAL (L"The contents of the selected object will become visible in a Data Editor. "
	"You can then view and change the data in the object, "
	"but beware: changing the data directly in this way may render them inconsistent.")
NORMAL (L"Changes that you make to the data with another Editor (e.g., a SoundEditor), "
	"or with the commands under #%Modify, "
	"are immediately reflected in the top-level Data Editor; "
	"any subeditors are destroyed, however, because they may now refer to invalid data.")
NORMAL (L"Changes that you make to the data with a Data Editor, "
	"are immediately reflected in any open type-specific Editors (e.g., a SoundEditor).")
MAN_END

MAN_BEGIN (L"Intro", L"ppgb", 20110131)
INTRO (L"This is an introductory tutorial to Praat, a computer program "
	"with which you can analyse, synthesize, and manipulate speech, "
	"and create high-quality pictures for your articles and thesis. "
	"You are advised to work through all of this tutorial.")
NORMAL (L"You can read this tutorial sequentially with the help of the \"1 >\" and \"< 1\" buttons, "
	"or go to the desired information by clicking on the blue links.")
LIST_ITEM (L"@@Intro 1. How to get a sound@: "
	"@@Intro 1.1. Recording a sound|record@, "
	"@@Intro 1.2. Reading a sound from disk|read@, "
	"@@Intro 1.3. Creating a sound from a formula|formula@.")
LIST_ITEM (L"@@Intro 2. What to do with a sound@: "
	"@@Intro 2.1. Saving a sound to disk|write@, "
	"@@Intro 2.2. Viewing and editing a sound|view@.")
LIST_ITEM (L"@@Intro 3. Spectral analysis")
LIST_ITEM1 (L"spectrograms: @@Intro 3.1. Viewing a spectrogram|view@, "
	"@@Intro 3.2. Configuring the spectrogram|configure@, "
	"@@Intro 3.3. Querying the spectrogram|query@, "
	"@@Intro 3.4. Printing the spectrogram|print@, "
	"@@Intro 3.5. The Spectrogram object|the Spectrogram object@.")
LIST_ITEM1 (L"spectral slices: @@Intro 3.6. Viewing a spectral slice|view@, "
	"@@Intro 3.7. Configuring the spectral slice|configure@, "
	"@@Intro 3.8. The Spectrum object|the Spectrum object@.")
LIST_ITEM (L"@@Intro 4. Pitch analysis")
LIST_ITEM1 (L"pitch contours: @@Intro 4.1. Viewing a pitch contour|view@, "
	"@@Intro 4.2. Configuring the pitch contour|configure@, "
	"@@Intro 4.3. Querying the pitch contour|query@, "
	"@@Intro 4.4. Printing the pitch contour|print@, "
	"@@Intro 4.5. The Pitch object|the Pitch object@.")
LIST_ITEM (L"@@Intro 5. Formant analysis")
LIST_ITEM1 (L"formant contours: @@Intro 5.1. Viewing formant contours|view@, "
	"@@Intro 5.2. Configuring the formant contours|configure@, "
	"@@Intro 5.3. Querying the formant contours|query@, "
	"@@Intro 5.4. The Formant object|the Formant object@.")
LIST_ITEM (L"@@Intro 6. Intensity analysis")
LIST_ITEM1 (L"intensity contours: @@Intro 6.1. Viewing an intensity contour|view@, "
	"@@Intro 6.2. Configuring the intensity contour|configure@, "
	"@@Intro 6.3. Querying the intensity contour|query@, "
	"@@Intro 6.4. The Intensity object|the Intensity object@.")
LIST_ITEM (L"@@Intro 7. Annotation")
LIST_ITEM (L"@@Intro 8. Manipulation@: of "
	"@@Intro 8.1. Manipulation of pitch|pitch@, "
	"@@Intro 8.2. Manipulation of duration|duration@, "
	"@@Intro 8.3. Manipulation of intensity|intensity@, "
	"@@Intro 8.4. Manipulation of formants|formants@.")
NORMAL (L"There are also more specialized tutorials:")
LIST_ITEM (L"Phonetics:")
LIST_ITEM1 (L"\\bu Voice analysis (jitter, shimmer, noise): @Voice")
LIST_ITEM1 (L"\\bu Listening experiments: @@ExperimentMFC@")
LIST_ITEM1 (L"\\bu @@Sound files@")
LIST_ITEM1 (L"\\bu @@Filtering@")
LIST_ITEM1 (L"\\bu @@Source-filter synthesis@")
LIST_ITEM1 (L"\\bu @@Articulatory synthesis@")
LIST_ITEM (L"Learning:")
LIST_ITEM1 (L"\\bu @@Feedforward neural networks@")
LIST_ITEM1 (L"\\bu @@OT learning@")
LIST_ITEM (L"Statistics:")
LIST_ITEM1 (L"\\bu @@Principal component analysis@")
LIST_ITEM1 (L"\\bu @@Multidimensional scaling@")
LIST_ITEM1 (L"\\bu @@Discriminant analysis@")
LIST_ITEM (L"General:")
LIST_ITEM1 (L"\\bu @@Printing@")
LIST_ITEM1 (L"\\bu @@Scripting@")
LIST_ITEM1 (L"\\bu @@Demo window@")
ENTRY (L"The authors")
NORMAL (L"The Praat program was created by Paul Boersma and David Weenink of "
	"the Institute of Phonetics Sciences of the University of Amsterdam. "
	"Home page: ##http://www.praat.org# or ##http://www.fon.hum.uva.nl/praat/#.")
NORMAL (L"For questions and suggestions, mail to the Praat discussion list, "
	"which is reachable from the Praat home page, or directly to ##paul.boersma\\@ uva.nl#.")
MAN_END

MAN_BEGIN (L"Intro 1. How to get a sound", L"ppgb", 20021212)
INTRO (L"Most of the things most people do with Praat start with a sound. "
	"There are at least three ways to get a sound:")
LIST_ITEM (L"@@Intro 1.1. Recording a sound")
LIST_ITEM (L"@@Intro 1.2. Reading a sound from disk")
LIST_ITEM (L"@@Intro 1.3. Creating a sound from a formula")
MAN_END

MAN_BEGIN (L"Intro 1.1. Recording a sound", L"ppgb", 20110128)
#ifdef macintosh
	INTRO (L"To record a speech sound into Praat, you need a computer with a microphone.")
	NORMAL (L"To record from the microphone, perform the following steps:")
#else
	INTRO (L"To record a speech sound into Praat, you need a computer with a microphone. "
		"If you do not have a microphone, try to record from an audio CD instead.")
	NORMAL (L"To record from the microphone (or the CD), perform the following steps:")
#endif
LIST_ITEM (L"1. Choose @@Record mono Sound...@ from the @@New menu@ in the @@Object window@. "
	"A @SoundRecorder window will appear on your screen.")
#if defined (_WIN32)
	LIST_ITEM (L"2. Choose the appropriate input device, namely the microphone, by the following steps. "
		"If there is a small loudspeaker symbol in the Windows Start bar, double click it and you will see the %playing mixer. "
		"If there is no loudspeaker symbol, go to ##Control Panels#, then ##Sounds and Audio Devices#, then #Volume, then #Advanced, "
		"and you will finally see the playing mixer. "
		"Once you see the Windows playing mixer, choose #Properties from the #Option menu, "
		"then click #Recording, then #OK. You now see the %recording mixer, where you can select the microphone "
		"(if you do not like this complicated operation, try Praat on Macintosh or Linux instead of Windows).")
#elif defined (macintosh)
	LIST_ITEM (L"2. In the SoundRecorder window, choose the appropriate input device, e.g. choose ##Internal microphone#.")
#else
	LIST_ITEM (L"2. In the SoundRecorder window, choose the appropriate input device, i.e. choose #Microphone (or #CD, or #Line).")
#endif
#ifdef macintosh
	LIST_ITEM (L"3. Use the #Record and #Stop buttons to record a few seconds of your speech.")
#else
	LIST_ITEM (L"3. Use the #Record and #Stop buttons to record a few seconds of your speech "
		"(or a few seconds of music from your playing CD).")
#endif
LIST_ITEM (L"4. Use the #Play button to hear what you have recorded.")
LIST_ITEM (L"5. Repeat steps 3 and 4 until you are satisfied with your recording.")
LIST_ITEM (L"6. Click the ##Save to list# button. Your recording will now appear in the Object window, "
	"where it will be called \"Sound sound\".")
LIST_ITEM (L"7. You can now close the SoundRecorder window.")
LIST_ITEM (L"8. When you saved your sound to the Object window, some buttons appeared in that window. "
	"These buttons show you what you can do with the sound. Try the #Play and @@View & Edit@ buttons.")
NORMAL (L"For more information on recording, see the @SoundRecorder manual page.")
MAN_END

MAN_BEGIN (L"Intro 1.2. Reading a sound from disk", L"ppgb", 20041126)
INTRO (L"Apart from recording a new sound from a microphone, you could read an existing sound file from your disk.")
NORMAL (L"With @@Read from file...@ from the @@Open menu@, "
	"Praat will be able to read most standard types of sound files, e.g. WAV files. "
	"They will appear as @Sound objects in the Object window. For instance, if you open the file ##hello.wav#, "
	"an object called \"Sound hello\" will appear in the list.")
NORMAL (L"If you do not have a sound file on your disk, you can download a WAV file (or so) from the Internet, "
	"then read that file into Praat with ##Read from file...#.")
MAN_END

MAN_BEGIN (L"Intro 1.3. Creating a sound from a formula", L"ppgb", 20070225)
INTRO (L"If you have no microphone, no sound files on disk, and no access to the Internet, "
	"you could still create a sound with @@Create Sound from formula...@ from the @@New menu@.")
MAN_END

MAN_BEGIN (L"Intro 2. What to do with a sound", L"ppgb", 20110131)
INTRO (L"As soon as you have a @Sound in the @@List of Objects@, "
	"the buttons in the @@Dynamic menu@ (the right-hand part of the @@Object window@) "
	"will show you what you can do with it.")
LIST_ITEM (L"@@Intro 2.1. Saving a sound to disk")
LIST_ITEM (L"@@Intro 2.2. Viewing and editing a sound")
MAN_END

MAN_BEGIN (L"Intro 2.1. Saving a sound to disk", L"ppgb", 20110131)
INTRO (L"There are several ways to write a sound to disk.")
NORMAL (L"First, the @@File menu@ of the @SoundRecorder window contains commands to save the left "
	"channel, the right channel, or both channels of the recorded sound to any of four standard types "
	"of sound files (WAV, AIFC, NeXT/Sun, NIST). These four file types are all equally good for Praat: "
	"Praat will handle them equally well on every computer. The first three of these types will "
	"also be recognized by nearly all other sound-playing programs.")
NORMAL (L"Then, once you have a @Sound object in the @@List of Objects@, "
	"you can save it in several formats with the commands in the @@Save menu@. "
	"Again, the WAV, AIFF, AIFC, NeXT/Sun, and NIST formats are equally fine.")
NORMAL (L"For more information, see the @@Sound files@ tutorial.")
MAN_END

MAN_BEGIN (L"Intro 2.2. Viewing and editing a sound", L"ppgb", 20110212)
NORMAL (L"To see the wave form of a @Sound that is in the list of objects, "
	"select that Sound and click @@View & Edit@. A @SoundEditor window will appear on your screen. "
	"You see a waveform (or two waveforms, if you have a stereo sound) and probably some \"analyses\" below it. "
	"You can zoom in and scroll to see the various parts of the sound in detail. "
	"You can select a part of the sound by dragging with the mouse. "
	"To play a part of the sound, click on any of the rectangles below it. "
	"To move a selected part of the sound to another location, use #Cut and #Paste from the #Edit menu. "
	"You can open sound windows for more than one sound, and then cut, copy, and paste between the sounds, "
	"just as you are used to do with text and pictures in word processing programs.")
NORMAL (L"If your sound file is longer than a couple of minutes, "
	"or if you want to see and listen to both channels of a stereo sound, "
	"you may prefer to open it with @@Open long sound file...@. "
	"This puts a @LongSound object into the list. In this way, most of the sound will stay in the file on disk, "
	"and at most 60 seconds will be read into memory each time you play or view a part of it. "
	"To change these 60 seconds to something else, e.g. 500 seconds, choose ##LongSound prefs...# from the #Preferences submenu.")
MAN_END

MAN_BEGIN (L"Intro 3. Spectral analysis", L"ppgb", 20070905)
INTRO (L"This section describes how you can analyse the spectral content of an existing sound. "
	"You will learn how to use %spectrograms and %%spectral slices%.")
LIST_ITEM (L"@@Intro 3.1. Viewing a spectrogram")
LIST_ITEM (L"@@Intro 3.2. Configuring the spectrogram")
LIST_ITEM (L"@@Intro 3.3. Querying the spectrogram")
LIST_ITEM (L"@@Intro 3.4. Printing the spectrogram")
LIST_ITEM (L"@@Intro 3.5. The Spectrogram object")
LIST_ITEM (L"@@Intro 3.6. Viewing a spectral slice")
LIST_ITEM (L"@@Intro 3.7. Configuring the spectral slice")
LIST_ITEM (L"@@Intro 3.8. The Spectrum object")
MAN_END

MAN_BEGIN (L"Intro 3.1. Viewing a spectrogram", L"ppgb", 20110128)
INTRO (L"To see the spectral content of a sound as a function of time, "
	"select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor or @LongSoundEditor window will appear on your screen. "
	"In the entire bottom half of this window you will see a greyish image, which is called a %spectrogram. "
	"If you do not see it, choose @@Show spectrogram@ from the #Spectrogram menu.")
NORMAL (L"The spectrogram is a @@spectro-temporal representation@ of the sound. "
	"The horizontal direction of the spectrogram represents @time, the vertical direction represents @frequency. "
	"The time scale of the spectrogram is the same as that of the waveform, so the spectrogram reacts "
	"to your zooming and scrolling. "
	"To the left of the spectrogram, you see the frequency scale. The frequency at the bottom of the spectrogram "
	"is usually 0 Hz (hertz, cps, cycles per second), and a common value for the frequency at the top is 5000 Hz.")
NORMAL (L"Darker parts of the spectrogram mean higher energy densities, lighter parts mean lower energy densities. "
	"If the spectrogram has a dark area around a time of 1.2 seconds and a frequency of 4000 Hz, "
	"this means that the sound has lots of energy for those high frequencies at that time. "
	"For many examples of spectrograms of speech sounds, see the textbook by @@Ladefoged (2001)@ and "
	"the reference work by @@Ladefoged & Maddieson (1996)@.")
NORMAL (L"To see what time and frequency a certain part of the spectrogram is associated with, "
	"just click on the spectrogram and you will see the vertical time cursor showing the time above "
	"the waveform and the horizontal frequency cursor showing the frequency to the left of the spectrogram. "
	"This is one of the ways to find the %formant frequencies for vowels, or the main spectral peaks "
	"for fricatives.")
ENTRY (L"Hey, there are white vertical stripes at the edges!")
NORMAL (L"This is normal. Spectral analysis requires an %%analysis window% of a certain duration. "
	"For instance, if Praat wants to know the spectrum at 1.342 seconds, it needs to include information "
	"about the signal in a 10-milliseconds window around this time point, i.e., Praat will use "
	"signal information about all times between 1.337 and 1.347 seconds. At the very edges of the sound, "
	"this information is not available: "
	"if the sound runs from 0 to 1.8 seconds, no spectrum can be computed between 0 and 0.005 "
	"seconds or between 1.795 and 1.800 seconds. Hence the white stripes. If you do not see them "
	"immediately when you open the sound, zoom in on the beginning or end of the sound.")
NORMAL (L"When you zoom in on the middle of the sound (or anywhere not near the edges), the white stripes vanish. "
	"Suddenly you see only the time stretch between 0.45 and 1.35 seconds, for instance. "
	"But Praat did not forget what the signal looks like just outside the edges of this time window. "
	"To display a spectrogram from 0.45 to 1.35 seconds, Praat will use information from the wave form "
	"between 0.445 and 1.355 seconds, and if this is available, you will see no white stripes at the edges of the window.")
ENTRY (L"Hey, it changes when I scroll!")
NORMAL (L"This is normal as well, especially for long windows. If your visible time window is 20 seconds long, "
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
NORMAL (L"The darkness of the spectrogram will also change when you scroll, because the visible part with the most "
	"energy is defined as black. When a very energetic part of the signal scrolls out of view, the spectrogram "
	"will turn darker. The next section will describe a way to switch this off.")
MAN_END

MAN_BEGIN (L"Intro 3.2. Configuring the spectrogram", L"ppgb", 20110128)
NORMAL (L"With @@Spectrogram settings...@ from the #Spectrogram menu, "
	"you can determine how the spectrogram is computed and how it is displayed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (\"factory settings\"), which appear "
	"when you click ##Standards#.")
TAG (L"%%View range% (Hz)")
DEFINITION (L"the range of frequencies to display. The standard is 0 Hz at the bottom and 5000 Hz at the top. "
	"If this maximum frequency is higher than the Nyquist frequency of the Sound "
	"(which is half its sampling frequency), some values in the spectrogram will be zero, and the higher "
	"frequencies will be drawn in white. You can see this if you record a Sound at 44100 Hz and set the "
	"view range from 0 Hz to 25000 Hz.")
TAG (L"%%Window length")
DEFINITION (L"the duration of the analysis window. If this is 0.005 seconds (the standard), "
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
TAG (L"%%Dynamic range% (dB)")
DEFINITION (L"All values that are more than %%Dynamic range% dB below the maximum (perhaps after dynamic compression, "
	"see @@Advanced spectrogram settings...@) "
	"will be drawn in white. Values in-between have appropriate shades of grey. Thus, if the highest "
	"peak in the spectrogram has a height of 30 dB/Hz, and the dynamic range is 50 dB (which is the standard value), "
	"then values below -20 dB/Hz will be drawn in white, and values between -20 dB/Hz and 30 dB/Hz will be drawn "
	"in various shades of grey.")
ENTRY (L"The bandwidth")
NORMAL (L"To see how the window length influences the bandwidth, "
	"first create a 1000-Hz sine wave with @@Create Sound from formula...@ "
	"by typing $$1/2 * sin (2*pi*1000*x)$ as the formula, then click ##View & Edit#. "
	"The spectrogram will show a horizontal black line. "
	"You can now vary the window length in the spectrogram settings and see how the thickness "
	"of the lines varies. The line gets thinner if you raise the window length. "
	"Apparently, if the analysis window comprises more periods of the wave, "
	"the spectrogram can tell us the frequency of the wave with greater precision.")
NORMAL (L"To see this more precisely, create a sum of two sine waves, with frequencies of 1000 and 1200 Hz. "
	"the formula is $$1/4 * sin (2*pi*1000*x) + 1/4 * sin (2*pi*1200*x)$. In the editor, you will see "
	"a single thick band if the analysis window is short (5 ms), and two separate bands if the analysis "
	"window is long (30 ms). Apparently, the frequency resolution gets better with longer analysis windows.")
NORMAL (L"So why don't we always use long analysis windows? The answer is that their time resolution is poor. "
	"To see this, create a sound that consists of two sine waves and two short clicks. The formula is "
	"$$0.02*(sin(2*pi*1000*x)+sin(2*pi*1200*x)) + (col=10000)+(col=10200)$. "
	"If you view this sound, you can see that the two clicks will overlap "
	"in time if the analysis window is long, and that the sine waves overlap in frequency if the "
	"analysis window is short. Apparently, there is a trade-off between time resolution and "
	"frequency resolution. One cannot know both the time and the frequency with great precision.")
ENTRY (L"Advanced settings")
NORMAL (L"The Spectrum menu also contains @@Advanced spectrogram settings...@.")
MAN_END

MAN_BEGIN (L"Advanced spectrogram settings...", L"ppgb", 20120531)
ENTRY (L"Optimization")
TAG (L"%%Number of time steps%")
DEFINITION (L"the maximum number of points along the time window for which Praat has to compute "
	"the spectrum. If your screen is not wider than 1200 pixels, then the standard of 1000 is "
	"appropriate, since there is no point in computing more than one spectrum per one-pixel-wide vertical line. "
	"If you have a really wide screen, you may see improvement if you raise this number to 1500.")
TAG (L"%%Number of frequency steps%")
DEFINITION (L"the maximum number of points along the frequency axis for which Praat has to compute "
	"the spectrum. If your screen is not taller than 768 pixels, then the standard of 250 is "
	"appropriate, since there is no point in computing more than one spectrum per one-pixel-height horizontal line. "
	"If you have a really tall screen, you may see improvement if you raise this number.")
NORMAL (L"For purposes of computation speed, Praat may decide to change the time step and the frequency step. "
	"This is because the time step never needs to be smaller than 1/(8\\Vr\\pi) of the window length, "
	"and the frequency step never needs to be smaller than (\\Vr\\pi)/8 of the inverse of the window length. "
	"For instance, if the window length is 5 ms, "
	"the actual time step will never be less than 5/(8\\Vr\\pi) = 0.353 ms, "
	"and the actual frequency step will never be less than (\\Vr\\pi)/8/0.005 = 44.31 Hz.")
ENTRY (L"Spectrogram analysis settings")
TAG (L"%%Method")
DEFINITION (L"there is currently only one method available in this window for computing a spectrum from "
	"a sound: the Fourier transform.")
TAG (L"%%Window shape")
DEFINITION (L"the shape of the analysis window. To compute the spectrum at, say, 3.850 seconds, "
	"samples that lie close to 3.850 seconds are given more weight than samples further away. "
	"The relative extent to which each sample contributes to the spectrum is given by the window shape. "
	"You can choose from: Gaussian, Square (none, rectangular), Hamming (raised sine-squared), "
	"Bartlett (triangular), Welch (parabolic), and Hanning (sine-squared). "
	"The Gaussian window is superior, as it gives no %sidelobes in your spectrogram (see below); "
	"it analyzes a factor of 2 slower than the other window shapes, "
	"because the analysis is actually performed on twice as many samples per frame.")
ENTRY (L"Sidelobes; anybody wants to win a cake?")
NORMAL (L"The Gaussian window is the only shape that we can consider seriously as a candidate for "
	"the analysis window. To see this, create a 1000-Hz sine wave with @@Create Sound from formula...@ "
	"by typing $$1/2 * sin (2*pi*1000*x)$ as the formula, then click ##View & Edit#. "
	"If the window shape is Gaussian, the spectrogram will show a horizontal black line. "
	"If the window shape is anything else, the spectrogram will show many horizontal grey lines (%sidelobes), "
	"which do not represent anything that is available in the signal. They are artifacts of the "
	"window shapes.")
NORMAL (L"We include these other window shapes only for pedagogical purposes "
	"and because the Hanning and Hamming windows have traditionally been used in other programs before "
	"computers were as fast as they are now (a spectrogram is computed twice as fast "
	"with these other windows). Several other programs still use these inferior window shapes, and you are "
	"likely to run into people who claim that the Gaussian window has disadvantages. "
	"We promise such people a large cake if they can come up with sounds that look better "
	"with Hanning or Hamming windows than with a Gaussian window. An example of the reverse is easy "
	"to find; we have just seen one.")
ENTRY (L"Spectrogram blackness settings")
TAG (L"%%Autoscaling%")
TAG (L"%%Maximum% (dB/Hz)")
DEFINITION (L"all parts of the spectrogram that have a power above %maximum (after preemphasis) "
	"will be drawn in black. The standard maximum is 100 dB/Hz, but if %autoscaling is on (which is the standard), "
	"Praat will use the maximum of the visible part of the spectrogram instead; "
	"this ensures that the window will always look well, but it also means that the blackness "
	"of a certain part of the spectrogram will change as you scroll.")
TAG (L"%%Preemphasis% (dB/octave)")
DEFINITION (L"determines the steepness of a high-pass filter, "
	"i.e., how much the power of higher frequencies will be raised before drawing, as compared to lower frequencies. "
	"Since the spectral slope of human vowels is approximately -6 dB per octave, "
	"the standard value for this setting is +6 dB per octave, "
	"so that the spectrum is flattened and the higher formants look as strong as the lower ones. "
	"When you raise the preemphasis, frequency bands above 1000 Hz will become darker, those below 1000 Hz will become lighter.")
TAG (L"%%Dynamic compression")
DEFINITION (L"determines how much stronger weak spectra should be made before drawing. "
	"Normally, this parameter is between 0 and 1. If it is 0 (the standard value), there is no dynamic compression. "
	"If it is 1, all spectra will be drawn equally strong, "
	"i.e., all of them will contain frequencies that are drawn in black. "
	"If this parameter is 0.4 and the global maximum is at 80 dB, then a spectrum with a maximum at 20 dB "
	"(which will normally be drawn all white if the dynamic range is 50 dB), "
	"will be raised by 0.4 * (80 - 20) = 24 dB, "
	"so that its maximum will be seen at 44 dB (thus making this frame visible).")
MAN_END

MAN_BEGIN (L"Intro 3.3. Querying the spectrogram", L"ppgb", 20030403)
NORMAL (L"If you click anywhere inside the spectrogram, a cursor cross will appear, "
	"and you will see the time and frequency in red at the top and to the left of the window. "
	"To see the time in the Info window, "
	"choose ##Get cursor# from the #Query menu or press the F6 key. "
	"To see the frequency in the Info window, "
	"choose ##Get frequency# from the #Spectrum menu.")
NORMAL (L"To query the power of the spectrogram at the cursor cross, "
	"choose ##Get spectral power at cursor cross# from the #Spectrum menu or press the F9 key. "
	"The Info window will show you the power density, expressed in Pascal^2/Hz.")
MAN_END

MAN_BEGIN (L"Intro 3.4. Printing the spectrogram", L"ppgb", 20070905)
NORMAL (L"To print a spectrogram, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to paint it into the @@Picture window@. "
	"You do this by choosing ##Paint visible spectrogram...# "
	"from the Spectrum menu in the Sound or TextGrid window. "
	"From the File menu in the Picture window, you can then print it, save it to an EPS file, "
	"or copy it to the clipboard (to do Paste in your word processor, for instance).")
MAN_END

MAN_BEGIN (L"Intro 3.5. The Spectrogram object", L"ppgb", 20070905)
NORMAL (L"To do more with spectrograms, you can create a @Spectrogram object in the @@List of Objects@. "
	"You do this either by choosing ##Extract visible spectrogram# "
	"from the Spectrum menu in the Sound or TextGrid window, "
	"or by selecting a Sound object in the list and choosing @@Sound: To Spectrogram...@ from the #Spectrum menu. "
	"In either case, a new Spectrogram object will appear in the list. "
	"To draw this Spectrogram object to the @@Picture window@, "
	"select it and choose the @@Spectrogram: Paint...@ command. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard. "
	"Many other commands are available in the @@dynamic menu@.")
MAN_END

MAN_BEGIN (L"Intro 3.6. Viewing a spectral slice", L"ppgb", 20030316)
INTRO (L"With ##View spectral slice# from the #Spectrum menu in the @SoundEditor and the @TextGridEditor, "
	"you can see the frequency spectrum at the time cursor "
	"or the average frequency spectrum in the time selection.")
ENTRY (L"Spectral slice at the cursor")
NORMAL (L"If you click anywhere in the wave form of the SoundEditor or TextGridEditor windows, "
	"a cursor will appear at that time. If you then choose ##View spectral slice#, "
	"Praat will create a @Spectrum object named %slice in the Objects window and show it in a @SpectrumEditor window. "
	"In this way, you can inspect the frequency contents of the signal around the cursor position.")
ENTRY (L"Spectral slice from a selection")
NORMAL (L"If you drag the mouse through the wave form of the SoundEditor or TextGridEditor windows, "
	"a @@time selection@ will appear. If you then choose ##View spectral slice#, "
	"Praat will again create a @Spectrum object named %slice in the Objects window and show it in a @SpectrumEditor window. "
	"In this way, you can inspect the frequency contents of the signal in the selection.")
MAN_END


MAN_BEGIN (L"Intro 3.7. Configuring the spectral slice", L"ppgb", 20110128)
ENTRY (L"Spectral slice at the cursor")
NORMAL (L"What Praat does precisely, depends on your Spectrogram settings. "
	"Suppose that the %%window length% setting is 0.005 seconds (5 milliseconds). "
	"If the %%window shape% is not Gaussian, Praat will extract the part of the sound "
	"that runs from 2.5 milliseconds before the cursor to 2.5 ms after the cursor. "
	"Praat then multiplies this 5 ms long signal by the window shape, then computes a spectrum "
	"with the method of @@Sound: To Spectrum...@, which is put into the Objects window and opened in an editor window. "
	"If the window shape is Gaussian, Praat will extract a part of the sound "
	"that runs from 5 milliseconds before the cursor to 5 ms after the cursor. The spectrum will then be based "
	"on a `physical' window length of 10 ms, although the `effective' window length is still 5 ms "
	"(see @@Intro 3.2. Configuring the spectrogram@ for details).")
ENTRY (L"Spectral slice from a selection")
NORMAL (L"What Praat does precisely, again depends on the %%window shape% of your Spectrogram settings. "
	"Suppose that your selection is 50 ms long. Praat will extract the entire selection, "
	"then multiply this 50 ms long signal by the window shape, then compute a spectrum, put it into the Objects window and open it an editor window. "
	"This procedure is equivalent to choosing ##Extract windowed selection...# (with a %%relative duration% of 1.0), "
	"followed by ##To Spectrum...# (with %fast switched on), followed by #Edit.")
NORMAL (L"If the window is Gaussian, Praat will still only use the selection, without doubling its duration. "
	"This means that the spectrum that you see in this case will mainly be based on the centre half of the selection, "
	"and the signal near the edges will be largely ignored.")
MAN_END

MAN_BEGIN (L"Intro 3.8. The Spectrum object", L"ppgb", 20030403)
NORMAL (L"To compute a Fourier frequency spectrum of an entire sound, "
	"select a @Sound object and choose @@Sound: To Spectrum...|To Spectrum...@ from the #Spectrum menu. "
	"A new @Spectrum object will appear in the @@List of Objects@. "
	"To view or modify it (or listen to its parts), click @@View & Edit@. "
	"To print it, choose one of the #Draw commands to draw the Spectrum object to the @@Picture window@ first.")
MAN_END

MAN_BEGIN (L"Intro 4. Pitch analysis", L"ppgb", 20070905)
INTRO (L"This section describes how you can analyse the pitch contour of an existing sound.")
LIST_ITEM (L"@@Intro 4.1. Viewing a pitch contour")
LIST_ITEM (L"@@Intro 4.2. Configuring the pitch contour")
LIST_ITEM (L"@@Intro 4.3. Querying the pitch contour")
LIST_ITEM (L"@@Intro 4.4. Printing the pitch contour")
LIST_ITEM (L"@@Intro 4.5. The Pitch object")
MAN_END

MAN_BEGIN (L"Intro 4.1. Viewing a pitch contour", L"ppgb", 20110128)
NORMAL (L"To see the pitch contour of an existing sound as a function of time, "
	"select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor window will appear on your screen. "
	"The bottom half of this window will contain a pitch contour, drawn as a blue line or as a sequence of blue dots. "
	"If you do not see the pitch contour, choose @@Show pitch@ from the #Pitch menu.")
NORMAL (L"To the right of the window, you may see three pitch values, written with blue digits: "
	"at the bottom, you see the floor of the viewable pitch range, perhaps 75 Hz; at the top, "
	"you see the ceiling of the pitch range, perhaps 600 Hz; and somewhere in between, you see the pitch value "
	"at the cursor, or the average pitch in the selection.")
MAN_END

MAN_BEGIN (L"Intro 4.2. Configuring the pitch contour", L"ppgb", 20050830)
NORMAL (L"With @@Pitch settings...@ from the #Pitch menu, "
	"you can determine how the pitch contour is displayed and how it is computed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (\"factory settings\"), which appear "
	"when you click #Standards.")
ENTRY (L"The %%pitch range% setting")
NORMAL (L"This is the most important setting for pitch analysis. The standard range is from 75 to 500 hertz, "
	"which means that the pitch analysis method will only find values between 75 and 500 Hz. "
	"The range that you set here will be shown to the right of the analysis window.")
NORMAL (L"For a male voice, you may want to set the floor to 75 Hz, and the ceiling to 300 Hz; "
	"for a female voice, set the range to 100-500 Hz instead. For creaky voice you will want to set it much "
	"lower than 75 Hz.")
NORMAL (L"Here is why you have to supply these settings. If the pitch floor is 75 Hz, "
	"the pitch analysis method requires a 40-millisecond analysis window, "
	"i.e., in order to measure the F0 at a time of, say, 0.850 seconds, "
	"Praat needs to consider a part of the sound that runs from 0.830 to 0.870 seconds. "
	"These 40 milliseconds correspond to 3 maximum pitch periods (3/75 = 0.040). "
	"If you set the pitch floor down to 25 Hz, the analysis window will grow to 120 milliseconds "
	"(which is again 3 maximum pitch periods), i.e., all times between 0.790 and 0.910 seconds will be considered. "
	"This makes it less easy to see fast F0 changes.")
NORMAL (L"So setting the floor of the pitch range is a technical requirement for the pitch analysis. "
	"If you set it too low, you will miss very fast F0 changes, and if you set it too high, "
	"you will miss very low F0 values. For children's voices you can often use 200 Hz, "
	"although 75 Hz will still give you the same time resolution as you get for the males.")
ENTRY (L"The %units setting")
NORMAL (L"This setting determines the units of the vertical pitch scale. Most people like to see the pitch range "
	"in hertz, but there are several other possibilities.")
ENTRY (L"Advanced settings")
NORMAL (L"The Pitch menu also contains @@Advanced pitch settings...@.")
MAN_END

MAN_BEGIN (L"Time step settings...", L"ppgb", 20031003)
INTRO (L"A command in the #View menu of the @SoundEditor and @TextGridEditor "
	"to determine the time interval between consecutive measurements "
	"of pitch, formants, and intensity.")
ENTRY (L"Automatic time steps")
NORMAL (L"It is recommended that you set the %%Time step strategy% to #Automatic. "
	"In this way, Praat computes just enough pitch, formant, and intensity values to draw "
	"reliable pitch, formant, and intensity contours. In general, Praat will compute 4 values "
	"within an analysis window (\"four times oversampling\").")
NORMAL (L"As described in @@Sound: To Pitch...@, Praat's standard time step for pitch analysis is 0.75 divided by the pitch floor, "
	"e.g., if the pitch floor is 75 Hz, the time step will be 0.01 seconds. "
	"In this way, there will be 4 pitch measurements within an analysis window, which is 3 / (75 Hz) = 40 milliseconds long.")
NORMAL (L"As described in @@Sound: To Formant (burg)...@, Praat's standard time step for formant measurements is the %%Window length% divided by 4, "
	"e.g. if the window length is 0.025 seconds, the time step will be 6.25 milliseconds.")
NORMAL (L"As described in @@Sound: To Intensity...@, Praat's standard time step for intensity measurements is 0.8 divided by the pitch floor, "
	"e.g. if the pitch floor is 75 Hz, the time step will be 10.6666667 milliseconds. "
	"In this way, there will be 4 intensity measurements within an intensity analysis window, "
	"which is 3.2 / (75 Hz) = 42.6666667 milliseconds long.")
ENTRY (L"Fixed time step")
NORMAL (L"You can override the automatic time step by setting the %%Time step strategy% to #Fixed. "
	"The %%Fixed time step% setting then determines the time step that Praat will use: "
	"if you set it to 0.001 seconds, Praat will compute pitch, formant, and intensity values for every millisecond. "
	"Beware that this can slow down the editor window appreciably, because this step is much smaller "
	"than usual values of the automatic time step (see above).")
NORMAL (L"Enlarging the time step to e.g. 0.1 seconds will speed up the editor window "
	"but may render the pitch, formant, and intensity curves less exact (they become %undersampled), "
	"which will influence your measurements and the locations of the pulses.")
NORMAL (L"If there are fewer than 2.0 pitch measurement points per analysis window, "
	"Praat will draw the pitch curve as separate little blue disks "
	"rather than as a continuous blue curve, in order to warn you of the undersampling. "
	"E.g. if the pitch floor is 75 Hz, Praat will draw the pitch curve as disks if the time step is greater than 0.02 seconds.")
ENTRY (L"View-dependent time step")
NORMAL (L"Another way to override the standard time step is by setting the %%Time step strategy% to ##View-dependent#. "
	"The %%Number of time steps per view% setting then determines the time step that Praat will use: "
	"if you set it to 100, Praat will always compute 100 pitch, formant, and intensity values within the view window. "
	"More precisely: if you zoom the view window to 3 seconds, Praat will show you 100 pitch, formant, and intensity points at distances "
	"of 0.03 seconds (or fewer than 100, if you are near the left or right edge of the signal). "
	"As with the %%Fixed time step% setting, Praat will draw the pitch as separate disks in case of undersampling. "
	"You may want to use this setting if you want the pitch curve to be drawn equally fast independently of the degree "
	"of zooming.")
MAN_END

MAN_BEGIN (L"Advanced pitch settings...", L"ppgb", 20110808)
INTRO (L"A command in the #Pitch menu of the @SoundEditor or @TextGridEditor windows. "
	"Before changing the advanced pitch settings, make sure you understand "
	"@@Intro 4.2. Configuring the pitch contour@.")
ENTRY (L"View range different from analysis range")
NORMAL (L"Normally, the range of pitch values that can be seen in the editor window is equal to the range of pitch values "
	"that the analysis algorithm can determine. If you set the analysis range from 75 to 500 Hz, this will be the range "
	"you see in the editor window as well. If the pitch values in the curve happen to be between 350 and 400 Hz, "
	"you may want to zoom in to the 350-400 Hz pitch region. "
	"You will usually do this by changing the pitch range in the @@Pitch settings...@ window. "
	"However, the analysis range will also change in that case, so that the curve itself may change. "
	"If you do not want that, you can change the %%View range% settings "
	"from \"0.0 (= auto)\" \\-- \"0.0 (=auto)\" to something else, perhaps \"350\" \\-- \"400\".")
ENTRY (L"Pitch analysis settings")
NORMAL (L"For information about these, see @@Sound: To Pitch (ac)...@. The standard settings are best in most cases. "
	"For some pathological voices, you will want to set the voicing threshold to much less than the standard of 0.45, "
	"in order to get pitch values even in irregular parts of the signal.")
MAN_END

MAN_BEGIN (L"Intro 4.3. Querying the pitch contour", L"ppgb", 20040614)
NORMAL (L"With @@Get pitch@ from the #Pitch menu in the @SoundEditor or @TextGridEditor, "
	"you get information about the pitch at the cursor or in the selection. "
	"If a cursor is visible in the window, ##Get pitch# writes to the @@Info window@ "
	"the linearly interpolated pitch at that time; "
	"if a time selection is visible inside the window, ##Get pitch# writes to the @@Info window@ "
	"the mean (average) pitch in the visible part of that selection; "
	"otherwise, ##Get pitch# writes the average pitch in the visible part of the sound.")
MAN_END

MAN_BEGIN (L"Intro 4.4. Printing the pitch contour", L"ppgb", 20070905)
NORMAL (L"To print a pitch contour, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to draw it into the @@Picture window@. "
	"You do this by choosing ##Draw visible pitch contour...# "
	"from the Pitch menu in the Sound or TextGrid window. "
	"From the File menu in the Picture window, you can then print it, save it to an EPS file, "
	"or copy it to the clipboard (to do Paste in your word processor, for instance).")
MAN_END

MAN_BEGIN (L"Intro 4.5. The Pitch object", L"ppgb", 20110128)
NORMAL (L"The pitch contour that is visible in the @SoundEditor or @TextGridEditor window, "
	"can be copied as a separate @Pitch object to the @@List of Objects@. To do this, "
	"choose @@Extract visible pitch contour@ from the #Pitch menu.")
NORMAL (L"Another way to get a separate Pitch object is to select a @Sound object in the list "
	"choose @@Sound: To Pitch...@ (preferred) or any of the other methods from the @@Periodicity menu@.")
NORMAL (L"To view and modify the contents of a Pitch object, select it and choose @@View & Edit@. "
	"This creates a @PitchEditor window on your screen.")
NORMAL (L"To save a pitch contour to disk, select the @Pitch object in the list and choose one of the commands in the @@Save menu@.")
NORMAL (L"Later on, you can read the saved file again with @@Read from file...@ from the @@Open menu@.")
NORMAL (L"To draw a @Pitch object to the @@Picture window@, select it and choose any of the commands in the @@Draw menu@. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard for inclusion in your word processor.")
MAN_END

MAN_BEGIN (L"Advanced pulses settings...", L"ppgb", 20110220)
INTRO (L"A command in the #Pulses menu of the @SoundEditor or @TextGridEditor windows. "
	"Before changing the advanced pulses settings, make sure you understand "
	"the @@Voice@ tutorial.")
NORMAL (L"For information about the ##Maximum period factor# setting, see @@PointProcess: Get jitter (local)...@. "
	"The standard setting is best in most cases. "
	"The ##Period floor# and ##Period ceiling# settings derive from the pitch floor and pitch ceiling (@@Pitch settings...@), "
	"according to a formula given in @@Voice 2. Jitter@.")
MAN_END

MAN_BEGIN (L"Intro 5. Formant analysis", L"ppgb", 20030316)
INTRO (L"This section describes how you can analyse the formant contours of an existing sound.")
LIST_ITEM (L"@@Intro 5.1. Viewing formant contours")
LIST_ITEM (L"@@Intro 5.2. Configuring the formant contours")
LIST_ITEM (L"@@Intro 5.3. Querying the formant contours")
LIST_ITEM (L"@@Intro 5.4. The Formant object")
MAN_END

MAN_BEGIN (L"Intro 5.1. Viewing formant contours", L"ppgb", 20110128)
NORMAL (L"To see the formant contours of a sound as functions of time, select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor window will appear on your screen. "
	"The analysis part of this window will contain formant contours, drawn as red speckles. "
	"If you do not see the formant contours, choose @@Show formant@ from the #Formant menu.")
MAN_END

MAN_BEGIN (L"Intro 5.2. Configuring the formant contours", L"ppgb", 20030316)
NORMAL (L"The formant analysis parameters, with you can set with the #Formant menu, are important. "
	"For a female voice, you may want to set the maximum frequency to 5500 Hz; "
	"for a male voice, set it to 5000 Hz instead. "
	"For more information about analysis parameters, see @@Sound: To Formant (burg)...@.")
MAN_END

MAN_BEGIN (L"Intro 5.3. Querying the formant contours", L"ppgb", 20040616)
NORMAL (L"With @@Get first formant@ from the Formant menu in the @SoundEditor or @TextGridEditor, "
	"you get information about the first formant at the cursor or in the selection. "
	"If there is a cursor, ##Get first formant# writes to the @@Info window@ the linearly interpolated first formant at that time. "
	"If there is a true selection, ##Get first formant# writes to the @@Info window@ the mean first formant in the visble part of that selection. "
	"The same goes for @@Get second formant@ and so on.")
MAN_END

MAN_BEGIN (L"Intro 5.4. The Formant object", L"ppgb", 20030316)
NORMAL (L"The formant contours that are visible in the @SoundEditor or @TextGridEditor window, "
	"can be copied as a separate @Formant object to the @@List of Objects@. To do this, "
	"choose @@Extract visible formant contour@ from the Formant menu.")
NORMAL (L"Another way to get a separate Formant object is to select a @Sound object in the list "
	"choose @@Sound: To Formant (burg)...@ (preferred) or any of the other methods "
	"from the @@Formants & LPC menu@.")
ENTRY (L"Saving formant contours to disk")
NORMAL (L"To save formant contours to disk, select the @Formant object in the list and choose one of the commands in the @@Save menu@.")
NORMAL (L"Later on, you can read the saved file again with @@Read from file...@ from the @@Open menu@.")
ENTRY (L"Drawing formant contours")
NORMAL (L"To draw a @Formant object to the @@Picture window@, select it and choose any of the commands in the @@Draw menu@. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard for inclusion in your word processor.")
MAN_END

MAN_BEGIN (L"Intro 6. Intensity analysis", L"ppgb", 20030316)
INTRO (L"This section describes how you can analyse the intensity contour of an existing sound.")
LIST_ITEM1 (L"@@Intro 6.1. Viewing an intensity contour")
LIST_ITEM1 (L"@@Intro 6.2. Configuring the intensity contour")
LIST_ITEM1 (L"@@Intro 6.3. Querying the intensity contour")
LIST_ITEM1 (L"@@Intro 6.4. The Intensity object")
MAN_END

MAN_BEGIN (L"Intro 6.1. Viewing an intensity contour", L"ppgb", 20110128)
INTRO (L"To see the intensity contour of a sound as a function of time, select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor window will appear on your screen. "
	"The analysis part of this window will contain an intensity contour, drawn as a yellow or green line "
	"(choose ##Show intensity# from the #Intensity menu if it is not visible). "
	"This also works in the @TextGridEditor.")
MAN_END

MAN_BEGIN (L"Intro 6.2. Configuring the intensity contour", L"ppgb", 20041123)
INTRO (L"With ##Intensity settings...# from the #Intensity menu, "
	"you can control how the intensity contour is computed and how it is shown.")
ENTRY (L"The view range")
NORMAL (L"By changing these two numbers you can set the vertical scale. "
	"The standard setting is from 50 to 100 dB, but if you are interested in the power of the background noise "
	"you may want to set it to the range from 0 to 100 dB.")
ENTRY (L"The averaging method")
NORMAL (L"Averaging is what occurs if you make a @@time selection@ (rather than setting the cursor at a single time). "
	"The green number in dB to the left or right side of the intensity contour will show the average "
	"intensity in the selection. The same value is what you get when choosing ##Get intensity# from "
	"the #Intensity menu. You can choose any of the three averaging methods "
	"available for @@Intensity: Get mean...@, or choose for the #median value in the selection.")
ENTRY (L"Pitch floor")
NORMAL (L"The intensity curve is %smoothed, since you usually do not want the intensity curve to go up and down with "
	"the intensity variations within a pitch period. To avoid such pitch-synchronous variations, "
	"the intensity at every time point is a weighted average over many neighbouring time points. "
	"The weighting is performed by a Gaussian (`double Kaiser') window that has a duration that is determined by the "
	"##Minimum pitch# setting (see @@Intro 4.2. Configuring the pitch contour@). "
	"To see more detail than usual, raise the minimum pitch; to get more smoothing than usual, lower it. "
	"For more information, see @@Sound: To Intensity...@.")
ENTRY (L"Subtract mean pressure or not?")
NORMAL (L"Many microphones, microphone preamplifiers, or other components in the recording system "
	"can add a constant level to the air pressure (a %%DC offset%). "
	"You can see this at quiet locations in the wave form, "
	"where the average pressure is not zero but the background noise hovers about some non-zero value. "
	"You will usually not want to take this constant pressure seriously, i.e. the intensity curve should "
	"normally not take into account the energy provided by this constant pressure level. "
	"If you switch on the ##Subtract mean pressure# switch (whose standard setting is also on), "
	"the intensity value at a time point is computed by first subtracting the mean pressure around this point, "
	"and then applying the Gaussian window.")
MAN_END

MAN_BEGIN (L"Intro 6.3. Querying the intensity contour", L"ppgb", 20041123)
INTRO (L"To ask for the intensity at the cursor, or the average intensity in the visible part of the selection, "
	"choose ##Get intensity# from the #Intensity menu or press the F11 key. You get the same value in dB that is visible "
	"to the left or right side of the intensity curve, but with more digits.")
MAN_END

MAN_BEGIN (L"Intro 6.4. The Intensity object", L"ppgb", 20041123)
NORMAL (L"To print an intensity contour, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to create an @Intensity object in the @@List of Objects@. "
	"You do this either by choosing ##Extract visible intensity contour# "
	"from the #Intensity menu in the SoundEditor or TextGridEditor window, "
	"or by selecting a Sound object in the list and choosing @@Sound: To Intensity...@. "
	"In either case, a new Intensity object will appear in the list. "
	"To draw the Intensity object to the @@Picture window@, select it and choose ##Draw...#. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard.")
MAN_END

MAN_BEGIN (L"Intro 7. Annotation", L"ppgb", 20110129)
INTRO (L"You can annotate existing @Sound objects and sound files (@LongSound objects).")
NORMAL (L"The labelling data will reside in a @TextGrid object. This object is separate "
	"from the sound, which means that you will often see two objects in the list: a Sound or LongSound, "
	"and a TextGrid.")
ENTRY (L"Creating a TextGrid")
NORMAL (L"You create a new empty TextGrid from the Sound or LongSound with @@Sound: To TextGrid...@ "
	"or @@LongSound: To TextGrid...@ from the #Annotate menu (which shows up in the Objects window if you select a Sound or LongSound). In this way, the time domain "
	"of the @TextGrid will automatically equal that of the sound (if you choose @@Create TextGrid...@ from "
	"the @@New menu@ instead, you will have to supply the time domain by yourself).")
NORMAL (L"When you create a TextGrid, you specify the names of the %tiers. For instance, if you want to segment "
	"the sound into words and into phonemes, you may want to create two tiers and call them "
	"\"words\" and \"phonemes\" (you can easily add, remove, and rename tiers later). "
	"Since both of these tiers are %%interval tiers% (you label the intervals between the word and phoneme "
	"boundaries, not the boundaries themselves), you specify \"phonemes words\" for %%Tier names%, "
	"and you leave the %%Point tiers% empty.")
ENTRY (L"View and edit")
NORMAL (L"You can edit a TextGrid object all by itself, but you will normally want to see "
	"the sound in the editor window as well. To achieve this, you select both the Sound (or LongSound) and "
	"the TextGrid, and click @@View & Edit@. A @TextGridEditor will appear on your screen. "
	"Like the Sound editor, the TextGrid editor will show you a spectrogram, a pitch contour, a formant contour, "
	"and an intensity contour. This editor will allow "
	"you to add, remove, and edit labels, boundaries, and tiers. Under Help in the TextGridEditor, you will "
	"find the @TextGridEditor manual page. You are strongly advised to read it, because it will show you "
	"how you can quickly zoom (drag the mouse), play (click a rectangle), or edit a label (just type).")
ENTRY (L"Save")
NORMAL (L"You will normally write the TextGrid to disk with @@Save as text file...@ "
	"or @@Save as short text file...@. It is true that @@Save as binary file...@ will also work, but the others "
	"give you a file you can read with any text editor.")
NORMAL (L"However you saved it, you can read the TextGrid into Praat later with @@Read from file...@.")
MAN_END

MAN_BEGIN (L"Intro 8. Manipulation", L"ppgb", 20021212)
LIST_ITEM (L"@@Intro 8.1. Manipulation of pitch")
LIST_ITEM (L"@@Intro 8.2. Manipulation of duration")
LIST_ITEM (L"@@Intro 8.3. Manipulation of intensity")
MAN_END

MAN_BEGIN (L"Intro 8.1. Manipulation of pitch", L"ppgb", 20110128)
INTRO (L"To modify the pitch contour of an existing @Sound object, "
	"you select this @Sound and click ##To Manipulation#. "
	"A @Manipulation object will then appear in the list. "
	"You can then click @@View & Edit@ to raise a @ManipulationEditor, "
	"which will show the pitch contour (@PitchTier) as a series of thick dots. "
	"To reduce the number of dots, choose ##Stylize pitch (2 st)# "
	"from the #Pitch menu; it will then be easy to drag the dots "
	"about the time-pitch area.")
NORMAL (L"If you click any of the rectangles "
	"(or choose any of the #Play commands from the #View menu), "
	"you will hear the modified sound. By shift-clicking, you will hear "
	"the original sound.")
NORMAL (L"To get the modified sound as a separate object, "
	"choose ##Publish resynthesis# from the @@File menu@.")
NORMAL (L"If you modify the duration curve as well (see @@Intro 8.2. Manipulation of duration@), "
	"the modified sound will be based on the modified pitch and duration.")
ENTRY (L"Cloning a pitch contour")
NORMAL (L"To use the pitch contour of one Manipulation object as the pitch contour of another Manipulation object, "
	"you first choose ##Extract pitch tier# for the first Manipulation object, "
	"then select the resulting PitchTier object together with the other Manipulation object "
	"(e.g. by a click on the PitchTier and a Command-click on the Manipulation), "
	"and choose ##Replace pitch tier#.")
ENTRY (L"Precise manipulation of pitch")
NORMAL (L"If you know exactly what pitch contour you want, "
	"you can create an empty PitchTier with @@Create PitchTier...@ from the New menu, "
	"then add points with @@PitchTier: Add point...@.")
NORMAL (L"For instance, suppose you want to have a pitch that falls from 350 to 150 Hz in one second. "
	"You create the PitchTier, then add a point at 0 seconds and a frequency of 350 Hz, "
	"and a point at 1 second with a frequency of 150 Hz. "
	"You can put this PitchTier into a Manipulation object in the way described above.")
MAN_END

MAN_BEGIN (L"Intro 8.2. Manipulation of duration", L"ppgb", 20110128)
INTRO (L"You can use Praat to modify the relative durations in an existing sound.")
NORMAL (L"First, you select a @Sound object and click \"To Manipulation\". "
	"A @Manipulation object will then appear in the list. "
	"You can then click @@View & Edit@ to raise a @ManipulationEditor, "
	"which will show an empty @DurationTier. "
	"You can add targets to this tier by choosing \"Add duration point at cursor\" "
	"from the \"Dur\" menu. The targets will show up as green dots, which you can easily drag "
	"about the duration area.")
NORMAL (L"If you click any of the rectangles "
	"(or choose any of the @Play commands from the @View menu), "
	"you will hear the modified sound. By shift-clicking, you will hear "
	"the original sound.")
NORMAL (L"To get the modified sound as a separate object, "
	"choose ##Publish resynthesis# from the @@File menu@.")
NORMAL (L"If you modify the pitch curve as well (see @@Intro 8.1. Manipulation of pitch@), "
	"the modified sound will be based on the modified duration and pitch.")
ENTRY (L"Precise manipulation of duration")
NORMAL (L"If you know exactly the times and relative durations, it is advisable to write a script (see @Scripting). "
	"Suppose, for instance, that you have a 355-ms piece of sound, and you want to shorten the first 85 ms to 70 ms, "
	"and the remaining 270 ms to 200 ms.")
NORMAL (L"In your first 85 ms, your relative duration should be 70/85, "
	"and during the last 270 ms, it should be 200/270. "
	"The DurationTier does linear interpolation, so it can only be approximate these precise times, "
	"but fortunately to any precision you like:")
CODE (L"Create DurationTier... shorten 0 0.085+0.270")
CODE (L"Add point... 0.000 70/85")
CODE (L"Add point... 0.084999 70/85")
CODE (L"Add point... 0.085001 200/270")
CODE (L"Add point... 0.355 200/270")
NORMAL (L"To put this DurationTier back into a Manipulation object, you select the two objects together "
	"(e.g. a click on the DurationTier and a Command-click on the Manipulation), "
	"and choose ##Replace duration tier#.")
MAN_END

MAN_BEGIN (L"Intro 8.3. Manipulation of intensity", L"ppgb", 20110128)
INTRO (L"You can modify the intensity contour of an existing sound.")
NORMAL (L"While the pitch and duration of a sound can be modified with the @ManipulationEditor "
	"(see @@Intro 8.1. Manipulation of pitch@ and @@Intro 8.2. Manipulation of duration@), "
	"the modification of the intensity curve is done in a different way.")
NORMAL (L"You can create an @IntensityTier with the command @@Create IntensityTier...@ "
	"from the @@New menu@. With @@View & Edit@, you can add points to this tier. "
	"You can then \"multiply\" this tier with a sound, by selecting the @Sound and the "
	"IntensityTier together and clicking @@Sound & IntensityTier: Multiply|Multiply@. "
	"The points in the IntensityTier represent relative intensities in dB; "
	"therefore, the sound pressure values in the Sound are multiplied by 10^^(dB/20)^.")
NORMAL (L"Instead of an IntensityTier, you can use an @AmplitudeTier; "
	"when you click #Multiply for a selected Sound and AmplitudeTier, "
	"the sound pressure values in the Sound are directly multiplied by the values in the AmplitudeTier.")
MAN_END

MAN_BEGIN (L"Intro 8.4. Manipulation of formants", L"ppgb", 20010408)
INTRO (L"The manipulation of formant contours cannot be as straightforward as the manipulation "
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

MAN_BEGIN (L"Labelling", L"ppgb", 20010408)
INTRO (L"See @@Intro 7. Annotation@.")
MAN_END

MAN_BEGIN (L"List of Objects", L"ppgb", 19981024)
INTRO (L"A list in the left-hand part of the @@Object window@.")
ENTRY (L"Purpose")
NORMAL (L"If you select one or more @objects in this list, "
	"the possible actions that you can perform with the selected objects "
	"will appear in the @@Dynamic menu@.")
ENTRY (L"How to select objects")
NORMAL (L"To select one object (and deselect all the others), click on the object.")
NORMAL (L"To extend the selection, drag the mouse (Unix, Windows) or use Shift-click (all platforms).")
NORMAL (L"To change the selection of one object (without changing the others), "
	"use Control-click (Unix, Windows) or Command-click (Macintosh).")
MAN_END

MAN_BEGIN (L"New menu", L"ppgb", 20080427)
INTRO (L"The ##New menu# is one of the menus in the @@Object window@. "
	"You use this menu to create new objects from scratch. It contains the following commands:")
LIST_ITEM (L"\\bu @@Record mono Sound...@")
LIST_ITEM (L"\\bu @@Record stereo Sound...@")
LIST_ITEM (L"\\bu @@Create Sound from formula...@")
LIST_ITEM (L"\\bu @@Create Sound from tone complex...")
LIST_ITEM (L"\\bu @@Create Sound from gammatone...")
LIST_ITEM (L"\\bu @@Create Sound from Shepard tone...")
LIST_ITEM (L"\\bu @@Create Matrix...@ (from a formula)")
LIST_ITEM (L"\\bu @@Create simple Matrix...@ (from a formula)")
LIST_ITEM (L"\\bu @@Create empty PointProcess...@")
LIST_ITEM (L"\\bu @@Create Poisson process...@")
LIST_ITEM (L"\\bu @@Create PitchTier...@")
LIST_ITEM (L"\\bu @@Create DurationTier...@")
LIST_ITEM (L"\\bu @@Create IntensityTier...@")
LIST_ITEM (L"\\bu @@Create FormantGrid...@")
LIST_ITEM (L"\\bu @@Create Strings as file list...@")
LIST_ITEM (L"\\bu @@Create TextGrid...@")
LIST_ITEM (L"\\bu @@OT learning@ tutorial")
LIST_ITEM (L"\\bu @@Create tongue-root grammar...@")
NORMAL (L"To create new objects from files on disk, use the @@Open menu@ instead. "
	"Objects can also often be create from other objects, with commands that start with ##To#.")
MAN_END

MAN_BEGIN (L"Object window", L"ppgb", 20030528)
INTRO (L"One of the two main windows in the Praat program.")
ENTRY (L"Subdivision")
LIST_ITEM (L"To the left: the @@List of Objects@.")
LIST_ITEM (L"To the right: the @@Dynamic menu@.")
ENTRY (L"Fixed buttons")
NORMAL (L"The following buttons appear below the List of Objects:")
LIST_ITEM (L"\\bu @@Rename...")
LIST_ITEM (L"\\bu @@Info")
LIST_ITEM (L"\\bu @@Copy...")
LIST_ITEM (L"\\bu @@Remove")
LIST_ITEM (L"\\bu @@Inspect")
ENTRY (L"Menus")
LIST_ITEM (L"The Object window contains several fixed menus: "
	"the #Praat, #New, #Open, and #Help menus. "
	"It also contains the #Save menu, whose contents vary with the kinds of selected objects, "
	"and must, therefore, be considered part of the dynamic menu.")
ENTRY (L"The Praat menu")
LIST_ITEM (L"\\bu (@@Run script...@)")
LIST_ITEM (L"\\bu @@New Praat script@: creates an empty @@ScriptEditor@")
LIST_ITEM (L"\\bu @@Open Praat script...@: creates a @@ScriptEditor@ with a script from disk")
LIST_ITEM (L"\\bu The ##Goodies submenu#: for doing things (like using the Calculator) "
	"that do not create new objects and do not depend on the kinds of selected objects.")
LIST_ITEM (L"\\bu The ##Preferences submenu#: for program-wide preferences, "
	"like audio input and output settings.")
LIST_ITEM (L"\\bu ##Buttons...#: raises a @@ButtonEditor@")
LIST_ITEM (L"\\bu (@@Add menu command...@)")
LIST_ITEM (L"\\bu (@@Add action command...@)")
LIST_ITEM (L"\\bu @@Quit")
ENTRY (L"Other menus")
LIST_ITEM (L"\\bu The @@New menu@: for creating objects from scratch.")
LIST_ITEM (L"\\bu The @@Open menu@: for reading objects from file into memory.")
LIST_ITEM (L"\\bu The @@Save menu@: for writing objects from memory to file.")
LIST_ITEM (L"\\bu The ##Help menu#: for viewing the manual.")
MAN_END

MAN_BEGIN (L"Periodicity menu", L"ppgb", 20010417)
INTRO (L"A menu that occurs in the @@Dynamic menu@ for a @Sound.")
NORMAL (L"This menu contains commands for analysing the pitch contour of the selected Sound:")
LIST_ITEM (L"@@Sound: To Pitch...")
LIST_ITEM (L"@@Sound: To Pitch (ac)...")
LIST_ITEM (L"@@Sound: To Pitch (cc)...")
LIST_ITEM (L"@@Sound: To Harmonicity (cc)...")
LIST_ITEM (L"@@Sound: To Harmonicity (ac)...")
MAN_END

MAN_BEGIN (L"Pitch settings...", L"ppgb", 20030316)
INTRO (L"A command in the Pitch menu of the @SoundEditor and @TextGridEditor windows. "
	"See @@Intro 4.2. Configuring the pitch contour@.")
MAN_END

MAN_BEGIN (L"Play", L"ppgb", 20021212)
INTRO (L"A command that is available if you select a @Sound, @Pitch, or @PointProcess object. "
	"It gives you an acoustic representation of the selected object, if your loudspeakers are on "
	"and you did not \"mute\" your computer sound system.")
NORMAL (L"A Play button is also available in the @SoundRecorder window "
	"and in the @View menu of the @SoundEditor or @TextGridEditor. In the editors, "
	"you will usually play a sound by clicking on any of the rectangles around the data.")
MAN_END

MAN_BEGIN (L"Query", L"ppgb", 20021218)
INTRO (L"Query commands give you information about objects.")
NORMAL (L"Most query commands start with the word #%Get or sometimes the word #%Count. "
	"You will find these commands in two places: under the #Query menu that usually appears if you "
	"select an @@Objects|object@ in the list, and in the #Query menus of the @editors.")
ENTRY (L"Behaviour")
NORMAL (L"If you click a query command, the answer will be written to the @@Info window@.")
ENTRY (L"Scripting")
NORMAL (L"In a script, you can still use query commands to write the information to the Info window "
	"but you can also use any query command to put the information into a variable. "
	"(see @@Scripting 6.3. Query commands@). In such a case, the value will not be written into the Info window.")
ENTRY (L"Query commands in the Praat program")
NORMAL (L"The Praat program contains the following query commands:")
MAN_END

MAN_BEGIN (L"Query menu", L"ppgb", 20010417)
INTRO (L"One of the menus in most @editors.")
MAN_END

MAN_BEGIN (L"Quit", L"ppgb", 20050822)
INTRO (L"One of the commands in the @@Praat menu@.")
ENTRY (L"Purpose")
NORMAL (L"To leave the program.")
ENTRY (L"Behaviour")
NORMAL (L"All @objects not written to a file will be lost. "
	"However, file-based objects (like large lexica) will be saved correctly.")
ENTRY (L"Usage")
NORMAL (L"To save your data to a disk file before quitting, choose a command from the @@Save menu@.")
MAN_END

MAN_BEGIN (L"Read from file...", L"ppgb", 20111018)
INTRO (L"One of the commands in the @@Open menu@.")
ENTRY (L"Purpose")
NORMAL (L"To read one or more @objects from a file on disk.")
ENTRY (L"Behaviour")
NORMAL (L"Many kinds of files are recognized:")
LIST_ITEM (L"1. Text files that are structured as described under @@Save as text file...@; "
	"these can contain an object of any type, or a collection of objects.")
LIST_ITEM (L"2. Files that were produced by @@Save as binary file...@ (any type).")
LIST_ITEM (L"3. Files in a LISP text format (only for object types that can be written to a LISP file).")
LIST_ITEM (L"4. Files that were made recognizable by the libraries built on Praat. "
	"For instance, the phonetics library adds recognizers for many kinds of sound files.")
NORMAL (L"If the file contains more than one object, these objects will appear in the list, "
	"and their names will be the same as the names that they had "
	"when they were saved with ##Save as text file...# or ##Save as binary file...#.")
ENTRY (L"Examples")
LIST_ITEM (L"\\bu If the file contains only one Pitch object and is called \"hallo.pit\", "
	"an object with the name \"Pitch hallo\" will appear in the list of objects. "
	"You may have more objects with the same name.")
LIST_ITEM (L"\\bu If the file contains one object of type Pitch, named \"hallo\", "
	"and one object of type Polygon, named \"kromme\", "
	"there will appear two objects in the list, "
	"called \"Pitch hallo\" and \"Polygon kromme\".")
MAN_END

MAN_BEGIN (L"Open menu", L"ppgb", 20110111)
INTRO (L"One of the menus in the @@Object window@.")
NORMAL (L"With the Open menu, you read one or more @objects from a file on disk into memory. "
	"The resulting object(s) will appear in the @@List of Objects@.")
NORMAL (L"The Open menu contains the command @@Read from file...@, which recognizes most file types, "
	"and perhaps several other commands for reading unrecognizable file types (e.g., raw sound data), "
	"or for interpreting known file types in a different way "
	"(e.g., reading two mono sounds from one stereo sound file):")
MAN_END

MAN_BEGIN (L"Remove", L"ppgb", 20021212)
INTRO (L"One of the fixed buttons in the @@Object window@.")
NORMAL (L"You can choose this command after selecting one or more @objects in the list.")
NORMAL (L"The selected objects will permanently disappear from the list, "
	"and the computer memory that they occupied will be freed.")
NORMAL (L"To save your data before removing, choose a command from the @@Save menu@.")
MAN_END

MAN_BEGIN (L"Rename...", L"ppgb", 20111018)
INTRO (L"One of the fixed buttons in the @@Object window@.")
ENTRY (L"Availability")
NORMAL (L"You can choose this command after selecting one object of any type.")
ENTRY (L"Purpose")
NORMAL (L"You can give the selected object a new name.")
ENTRY (L"Behaviour")
NORMAL (L"If you type special symbols or spaces, the Object window will replace them with underscores.")
MAN_END

MAN_BEGIN (L"Resource fork", L"ppgb", 20030316)
INTRO (L"One of the two %forks of a Macintosh file (the other is the %%data fork%). "
	"If a Macintosh file is moved to another platform directly, the resource fork is lost. "
	"To backup your Macintosh files, use compression, for instance with #DropStuff\\tm.")
MAN_END

MAN_BEGIN (L"Save menu", L"ppgb", 20111018)
INTRO (L"One of the menus in the @@Object window@.")
ENTRY (L"Purpose")
NORMAL (L"With the #Save menu, you write one or more selected @objects from memory to a file on disk. "
	"The data can be read in again with one of the commands in the @@Open menu@ "
	"(most often simply with @@Read from file...@).")
ENTRY (L"Usage: save your work")
NORMAL (L"You will often choose a command from this menu just before clicking the @Remove button "
	"or choosing the @Quit command.")
ENTRY (L"Fixed commands")
NORMAL (L"If no object is selected, the #Save menu is empty. "
	"If any object is selected, it will at least contain the following commands:")
LIST_ITEM (L"\\bu @@Write to console")
LIST_ITEM (L"\\bu @@Save as text file...")
LIST_ITEM (L"\\bu @@Save as short text file...")
LIST_ITEM (L"\\bu @@Save as binary file...")
ENTRY (L"Dynamic commands")
NORMAL (L"Depending on the type of the selected object, the following commands may be available "
	"in the #Save menu:")
MAN_END

MAN_BEGIN (L"Save as binary file...", L"ppgb", 20110129)
INTRO (L"One of the commands in the @@Save menu@.")
ENTRY (L"Availability")
NORMAL (L"You can choose this command after selecting one or more @objects.")
ENTRY (L"Behaviour")
NORMAL (L"The Object window will ask you for a file name. "
	"After you click OK, the objects will be written to a binary file on disk.")
ENTRY (L"Usage")
NORMAL (L"The file can be read again with @@Read from file...@.")
ENTRY (L"File format")
NORMAL (L"These files are in a device-independent binary format, "
	"and can be written and read on any machine.")
MAN_END

MAN_BEGIN (L"Save as short text file...", L"ppgb", 20110129)
INTRO (L"One of the commands in the @@Save menu@.")
ENTRY (L"Availability")
NORMAL (L"You can choose this command after selecting one or more @objects.")
ENTRY (L"Behaviour")
NORMAL (L"The Object window will ask you for a file name. "
	"After you click OK, the objects will be written to a text file on disk.")
ENTRY (L"File format")
NORMAL (L"The format is much shorter than the one described at @@Save as text file...@. "
	"Most of the comments are gone, and there is normally one piece of data per line.")
NORMAL (L"The file can be read again with the all-purpose @@Read from file...@.")
MAN_END

MAN_BEGIN (L"Save as text file...", L"ppgb", 20110129)
INTRO (L"One of the commands in the @@Save menu@.")
ENTRY (L"Availability")
NORMAL (L"You can choose this command after selecting one or more @objects.")
ENTRY (L"Behaviour")
NORMAL (L"The Object window will ask you for a file name. "
	"After you click OK, the objects will be written to a text file on disk.")
ENTRY (L"File format")
NORMAL (L"If you selected a single object, e.g., of type Pitch, "
	"the file will start with the lines:")
CODE (L"File type = \"ooTextFile\"")
CODE (L"Object class = \"Pitch\"")
NORMAL (L"After this, the pitch data will follow.")
LIST_ITEM (L"If you selected more than one object, e.g., `Pitch hallo' and `Polygon kromme', "
	"the file will look like:")
CODE (L"File type = \"ooTextFile\"")
CODE (L"Object class = \"Collection\"")
CODE (L"size = 2")
CODE (L"item []:")
CODE (L"    item [1]:")
CODE (L"        class = \"Pitch\"")
CODE (L"        name = \"hallo\"")
CODE (L"        (pitch data...)")
CODE (L"    item [2]:")
CODE (L"        class = \"Polygon\"")
CODE (L"        name = \"kromme\"")
CODE (L"        (polygon data...)")
NORMAL (L"The file can be read again with @@Read from file...@, "
	"which, by the way, does not need the verbosity of the above example. "
	"The following minimal format will also be read correctly:")
CODE (L"\"ooTextFile\"")
CODE (L"\"Collection\"  2")
CODE (L"\"Pitch\"  \"hallo\"  (pitch data...)")
CODE (L"\"Polygon\"  \"kromme\"  (polygon data...)")
NORMAL (L"Thus, all text that is not a free-standing number and is not enclosed in double quotes or < >, "
	"is considered a comment, as is all text following an exclamation mark (`!') on the same line.")
MAN_END

MAN_BEGIN (L"Segmentation", L"ppgb", 20010408)
INTRO (L"See @@Intro 7. Annotation@.")
MAN_END

MAN_BEGIN (L"Show formant", L"ppgb", 20030316)
INTRO (L"One of the commands in the Formant menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (L"See @@Intro 5. Formant analysis@.")
MAN_END

MAN_BEGIN (L"Show intensity", L"ppgb", 20030316)
INTRO (L"One of the commands in the Intensity menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (L"See @@Intro 6. Intensity analysis@.")
MAN_END

MAN_BEGIN (L"Show pitch", L"ppgb", 20030316)
INTRO (L"One of the commands in the Pitch menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (L"See @@Intro 4. Pitch analysis@.")
MAN_END

MAN_BEGIN (L"Show pulses", L"ppgb", 20030316)
INTRO (L"One of the commands in the Pulses menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (L"See @Voice.")
MAN_END

MAN_BEGIN (L"Show spectrogram", L"ppgb", 20030316)
INTRO (L"One of the commands in the Spectrogram menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (L"See @@Intro 3. Spectral analysis@.")
MAN_END

MAN_BEGIN (L"Source-filter synthesis", L"ppgb", 20050713)
INTRO (L"This tutorial describes how you can do acoustic synthesis with Praat. "
	"It assumes that you are familiar with the @Intro.")
ENTRY (L"1. The source-filter theory of speech production")
NORMAL (L"The source-filter theory (@@Fant (1960)|Fant 1960@) hypothesizes that an acoustic speech signal can be seen "
	"as a %source signal (the glottal source, or noise generated at a constriction in the vocal tract), "
	"%filtered with the resonances in the cavities of the vocal tract downstream from the glottis "
	"or the constriction. The %%Klatt synthesizer% (@@Klatt & Klatt (1990)|Klatt & Klatt 1990@), for instance, "
	"is based on this idea.")
NORMAL (L"In the Praat program, you can create a %source signal from scratch of from an existing "
	"speech signal, and you can create a %filter from scratch or extract it from an existing speech signal. "
	"You can manipulate (change, adapt) both the source and the filter before doing "
	"the actual synthesis, which combines the two.")
LIST_ITEM (L"@@Source-filter synthesis 1. Creating a source from pitch targets")
LIST_ITEM (L"@@Source-filter synthesis 2. Filtering a source")
LIST_ITEM (L"@@Source-filter synthesis 3. The ba-da continuum")
LIST_ITEM (L"@@Source-filter synthesis 4. Using existing sounds")
MAN_END

MAN_BEGIN (L"Source-filter synthesis 1. Creating a source from pitch targets", L"ppgb", 20110128)
INTRO (L"Creating a glottal source signal for speech synthesis involves creating a @PointProcess, "
	"which is a series of time points that should represent the exact moments of glottal closure.")
NORMAL (L"You may want to start with creating a well-defined pitch contour. "
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
	L"Create PitchTier... source 0 0.5\n"
	"Add point... 0.0 150\n"
	"Add point... 0.5 100\n"
	"Draw... 0 0 0 200 yes lines and speckles\n"
	"Text top... no %%An F0 linearly falling between two points\n"
	"One mark left... 100 yes yes no\n"
	"Remove\n"
)
NORMAL (L"You can hear the falling pitch by clicking on the rectangles in the PitchTier window "
	"(or by clicking ##Play pulses#, #Hum, or ##Play sine# in the Objects window).")
NORMAL (L"From this PitchTier, you can create a @PointProcess with @@PitchTier: To PointProcess@. "
	"The resulting PointProcess now represents a series of glottal pulses. To make some parts of this "
	"point process voiceless, you can use @@PointProcess: Remove points between...@. "
	"It is advisable to make the very beginning and end of this point process voiceless, so that the filtered sound "
	"will not start or end abruptly. In the following example, the first and last 20 ms are devoiced, "
	"and a stretch of 70 ms in the middle "
	"is made voiceless as well, perhaps because you want to simulate a voiceless plosive there:")
SCRIPT (4.5, 2.5,
	L"pitchTier = Create PitchTier... source 0 0.5\n"
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
NORMAL (L"Now that we have a glottal point process (a glottal pulse train), the only thing left "
	"is to turn it into a sound by choosing @@PointProcess: To Sound (phonation)...@. "
	"If you use the standard settings of this command (but with ##Adaptation factor# set to 0.6), the result will be a @Sound with "
	"reasonable glottal flow derivatives centred around each of the original pulses in the point process. "
	"You can check this by selecting the Sound and choosing ##View & Edit#:")
SCRIPT (4.5, 2.5,
	L"pitchTier = Create PitchTier... source 0 0.5\n"
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
NORMAL (L"You will also see that the amplitude of the first two glottal wave shapes of every voiced stretch "
	"is (realistically) somewhat smaller than the amplitudes of the following wave shapes; "
	"This is the result of setting ##Adaptation factor# to 0.6.")
NORMAL (L"What you have now is what we call a %%glottal source signal%. It does two things: it contains information on the glottal flow, "
	"and it already takes into account one aspect of the %filter, namely the radiation at the lips. "
	"This combination is standard procedure in acoustic synthesis.")
NORMAL (L"The glottal source signal sounds as a voice without a vocal tract. "
	"The following section describes how you add vocal-tract resonances, i.e. the %filter.")
ENTRY (L"Automation")
NORMAL (L"In a clean Praat script, the procedure described above will look as follows:")
CODE (L"pitchTier = Create PitchTier... source 0 0.5")
CODE (L"Add point... 0.0 150")
CODE (L"Add point... 0.5 100")
CODE (L"pulses = To PointProcess")
CODE (L"Remove points between... 0 0.02")
CODE (L"Remove points between... 0.24 0.31")
CODE (L"Remove points between... 0.48 0.5")
CODE (L"source = To Sound (phonation)... 44100 0.6 0.05 0.7 0.03 3.0 4.0")
CODE (L"select pitchTier")
CODE (L"plus pulses")
CODE (L"Remove")
CODE (L"select source")
MAN_END

MAN_BEGIN (L"Source-filter synthesis 2. Filtering a source", L"ppgb", 20080427)
INTRO (L"Once you have a glottal source signal, you are ready to create a filter that represents "
	"the resonances of the vocal tract, as a function of time. In other words, you create a @FormantGrid object.")
NORMAL (L"For a vowel spoken by an average (i.e. adult female) human voice, tradition assumes five formants in the range "
	"between 0 and 5500 hertz. This number comes from a computation of the formants of a "
	"straight tube, which has resonances at wavelengths of four tube lengths, four thirds of a tube length, "
	"four fifths, and so on. For a straight tube 16 centimetres long, the shortest wavelength is 64 cm, "
	"which, with a sound velocity of 352 m/s, means a resonance frequency of 352/0.64 = 550 hertz. "
	"The other resonances will be at 1650, 2750, 3850, and 4950 hertz.")
NORMAL (L"You can create a @FormantGrid object with @@Create FormantGrid...@ from the #New menu (submenu #Tiers):")
CODE (L"Create FormantGrid... filter 0 0.5 10 550 1100 60 50")
NORMAL (L"This creates a FormantGrid with 10 formants and a single frequency value for each formant: %F__1_ is 550 Hz "
	"and the higher formants are spaced 1100 Hz apart, i.e., they are "
	"1650, 2750, 3850, 4950, 6050, 7150, 8250, 9350, and 10450 hertz; "
	"the ten bandwidths start at 60 Hz and have a spacing of 50 Hz, "
	"i.e., they are 60, 110, 160, 210, 260, 310, 360, 410, 460, and 510 hertz.")
NORMAL (L"You can then create formant contours with @@FormantGrid: Add formant point...@:")
CODE (L"Remove formant points between... 1 0 0.5")
CODE (L"Add formant point... 1 0.00 100")
CODE (L"Add formant point... 1 0.05 700")
CODE (L"Remove formant points between... 2 0 0.5")
CODE (L"Add formant point... 2 0.00 500")
CODE (L"Add formant point... 2 0.05 1100")
NORMAL (L"This example creates a spectral specification whose %F__1_ rises from 100 to 700 hertz during the "
	"first 50 milliseconds (as for any obstruent), and whose %F__2_ rises from 500 to 1100 hertz. "
	"The other eight formants keep their original values, as do the ten bandwidths. "
	"The resulting sound may be a [ba]-like formant transition.")
NORMAL (L"To get the final acoustic result (the sound), you select the glottal source signal "
	"together with the FormantGrid and choose @@Sound & FormantGrid: Filter@.")
NORMAL (L"The resulting sound will have a fairly straight intensity contour. You can change this with "
	"the #Formula command (@@Sound: Formula...@), or by multiplying the source signal or the "
	"acoustic result with an @Intensity or @IntensityTier object.")
MAN_END

MAN_BEGIN (L"Source-filter synthesis 3. The ba-da continuum", L"ppgb", 20091012)
INTRO (L"As an example, we are going to create a male [ba]-[da] continuum in six steps. The acoustic difference "
	"between [ba] and [da] is the initial %F__2_, which is 500 Hz for [ba], and 2500 Hz for [da].")
NORMAL (L"We use the same @PitchTier throughout, to model a falling intonation contour:")
CODE (L"Create PitchTier... f0 0.00 0.50")
CODE (L"Add point... 0.00 150")
CODE (L"Add point... 0.50 100")
NORMAL (L"The first and last 50 milliseconds are voiceless:")
CODE (L"To PointProcess")
CODE (L"Remove points between... 0.00 0.05")
CODE (L"Remove points between... 0.45 0.50")
NORMAL (L"Generate the glottal source signal:")
CODE (L"To Sound (phonation)... 44100 0.6 0.05 0.7 0.03 3.0 4.0")
NORMAL (L"During the labial or coronal closure, the sound is almost silent, so we use an @IntensityTier "
	"that models this:")
CODE (L"Create IntensityTier... intens 0.00 0.50")
CODE (L"Add point... 0.05 60")
CODE (L"Add point... 0.10 80")
NORMAL (L"Generate the source signal:")
CODE (L"#plus Sound f0")
CODE (L"Multiply")
CODE (L"Rename... source")
NORMAL (L"The ten sounds are generated in a loop:")
CODE (L"#for i #from 1 #to 10")
CODE (L"   f2_locus = 500 + (2500/9) * (i - 1) ; variable names start with lower case!")
CODE (L"   Create FormantGrid... filter 0.0 0.5 9 800 1000 60 80")
CODE (L"   Remove formant points between... 1 0.0 0.5")
CODE (L"   Add formant point... 1 0.05 100")
CODE (L"   Add bandwidth point... 1 0.05 50")
CODE (L"   Add formant point... 2 0.05 f2_locus")
CODE (L"   Add bandwidth point... 2 0.05 100")
CODE (L"   #plus Sound source")
CODE (L"   Filter (no scale)")
CODE (L"   Rename... bada'i'")
CODE (L"   #select FormantGrid filter")
CODE (L"   Remove")
CODE (L"#endfor")
NORMAL (L"Clean up:")
CODE (L"#select Sound source")
CODE (L"#plus Sound f0")
CODE (L"#plus IntensityTier intens")
CODE (L"#plus PointProcess f0")
CODE (L"#plus PitchTier f0")
CODE (L"Remove")
NORMAL (L"In this example, filtering was done without automatic scaling, so that "
	"the resulting signals have equal intensities in the areas where they have "
	"equal formants. You will probably want to multiply all these signals with "
	"the same value in order to bring their amplitudes in a suitable range "
	"between -1 and +1 Pascal.")
MAN_END

MAN_BEGIN (L"Source-filter synthesis 4. Using existing sounds", L"ppgb", 20111018)
ENTRY (L"1. How to extract the %filter from an existing speech sound")
NORMAL (L"You can separate source and filter with the help of the technique of %%linear prediction% "
	"(see @@Sound: LPC analysis@). This technique tries to approximate a given frequency spectrum with "
	"a small number of peaks, for which it finds the mid frequencies and the bandwidths. "
	"If we do this for an overlapping sequence of windowed parts of a sound signal "
	"(i.e. a %%short-term analysis%), we get a quasi-stationary approximation of the signal's "
	"spectral characteristics as a function of time, i.e. a smoothed version of the @Spectrogram.")
NORMAL (L"For a speech signal, the peaks are identified with the resonances (%formants) of the vocal tract. "
	"Since the spectrum of a vowel spoken by an average human being falls off with approximately "
	"6 dB per octave, %%pre-emphasis% is applied to the signal before the linear-prediction analysis, "
	"so that the algorithm will not try to match only the lower parts of the spectrum.")
NORMAL (L"For an adult female human voice, tradition assumes five formants in the range "
	"between 0 and 5500 hertz, say at 550, 1650, 2750, 3850, and 4950 hertz. For the linear prediction in "
	"Praat, you will have to implement this 5500-Hz band-limiting by resampling the original "
	"speech signal to 11 kHz. For a male voice, you would use 10 kHz; for a young child, 20 kHz.")
NORMAL (L"To perform the resampling, you use @@Sound: Resample...@: "
	"you select a @Sound object, and click ##Resample...#. "
	"In the rest of this tutorial, I will use the syntax that you would use in a script, "
	"though you will usually do these things by clicking on objects and buttons. Thus:")
CODE (L"#select Sound hallo")
CODE (L"Resample... 11000 50")
NORMAL (L"You can then perform a linear-prediction analysis on the resampled sound "
	"with @@Sound: To LPC (burg)...@:")
CODE (L"#select Sound hallo_11000")
CODE (L"To LPC (burg)... 10 0.025 0.005 50")
NORMAL (L"This says that your analysis is done with 10 linear-prediction parameters "
	"(which will yield at most five formant-bandwidth pairs), with an analysis window "
	"effectively 25 milliseconds long, with time steps of 5 milliseconds (so that the windows "
	"will appreciably overlap), and with a pre-emphasis frequency of 50 Hz (which is the point "
	"above which the sound will be amplified by 6 dB/octave prior to the analysis proper).")
NORMAL (L"As a result, an object called \"LPC hallo\" will appear in the list of objects. "
	"This @LPC object is a time function with 10 %%linear-prediction coefficients% in each %%time frame%. "
	"These coefficients are rather opaque even to the expert (try to view them with @Inspect), "
	"but they are the raw material from which formant and bandwidth values can be computed. "
	"To see the smoothed @Spectrogram associated with the LPC object, choose @@LPC: To Spectrogram...@:")
CODE (L"#select LPC hallo_11000")
CODE (L"To Spectrogram... 20 0 50")
CODE (L"Paint... 0 0 0 0 50 0 0 yes")
NORMAL (L"Note that when drawing this Spectrogram, you will want to set the pre-emphasis to zero "
	"(the fifth 0 in the last line), because pre-emphasis has already been applied in the analysis.")
NORMAL (L"You can get and draw the formant-bandwidth pairs from the LPC object, "
	"with @@LPC: To Formant@ and @@Formant: Speckle...@:")
CODE (L"#select LPC hallo_11000")
CODE (L"To Formant")
CODE (L"Speckle... 0 0 5500 30 yes")
NORMAL (L"Note that in converting the @LPC into a @Formant object, you may have lost some "
	"information about spectral peaks at very low frequencies (below 50 Hz) or at very high "
	"frequencies (near the @@Nyquist frequency@ of 5500 Hz. Such peaks usually try to fit "
	"an overall spectral slope (if the 6 dB/octave model is inappropriate), and are not seen "
	"as related with resonances in the vocal tract, so they are ignored in a formant analysis. "
	"For resynthesis purposes, they might still be important.")
NORMAL (L"Instead of using the intermediate LPC object, you could have done a formant analysis "
	"directly on the original Sound, with @@Sound: To Formant (burg)...@:")
CODE (L"#select Sound hallo")
CODE (L"To Formant (burg)... 0.005 5 5500 0.025 50")
NORMAL (L"A @Formant object has a fixed sampling (time step, frame length), and for every "
	"%%formant frame%, it contains a number of formant-bandwidth pairs.")
NORMAL (L"From a Formant object, you can create a @FormantGrid with @@Formant: Down to FormantGrid@. "
	"A FormantGrid object contains a number of tiers with time-stamped %%formant points% and %%bandwidth points%.")
NORMAL (L"Any of these three types (@LPC, @Formant, and @FormantGrid) can represent the %filter "
	"in source-filter synthesis.")
ENTRY (L"2. How to extract the %source from an existing speech sound")
NORMAL (L"If you are only interested in the %filter characteristics, you can get by with @Formant objects. "
	"To get at the %source signal, however, you need the raw @LPC object: "
	"you select it together with the resampled @Sound, and apply %%inverse filtering%:")
CODE (L"#select Sound hallo_11000")
CODE (L"#plus LPC hallo_11000")
CODE (L"Filter (inverse)")
NORMAL (L"A new Sound named \"hallo_11000\" will appear in the list of objects "
	"(you could rename it to \"source\"). "
	"This is the estimated source signal. Since the LPC analysis was designed to yield a spectrally "
	"flat filter (through the use of pre-emphasis), this source signal represents everything in the "
	"speech signal that cannot be attributed to the resonating cavities. Thus, the \"source signal\" "
	"will consist of the glottal volume-velocity source (with an expected spectral slope of "
	"-12 dB/octave for vowels) and the radiation characteristics at the lips, which cause a "
	"6 dB/octave spectral rise, so that the resulting spectrum of the \"source signal\" "
	"is actually the %derivative of the glottal flow, with an expected spectral slope of -6 dB/octave.")
NORMAL (L"Note that with inverse filtering you cannot measure the actual spectral slope of the source signal. "
	"Even if the actual slope is very different from -6 dB/octave, formant extraction will try to "
	"match the pre-emphasized spectrum. Thus, by choosing a pre-emhasis of -6 dB/octave, "
	"you %impose a slope of -6 dB/octave on the source signal.")
ENTRY (L"3. How to do the synthesis")
NORMAL (L"You can create a new Sound from a source Sound and a filter, in at least four ways.")
NORMAL (L"If your filter is an @LPC object, you select it and the source, and choose @@LPC & Sound: Filter...@:")
CODE (L"#select Sound source")
CODE (L"#plus LPC filter")
CODE (L"Filter... no")
NORMAL (L"If you had computed the source and filter from an LPC analysis, this procedure should give "
	"you back the original Sound, except that windowing has caused 25 milliseconds at the beginning "
	"and end of the signal to be set to zero.")
NORMAL (L"If your filter is a @Formant object, you select it and the source, and choose @@Sound & Formant: Filter@:")
CODE (L"#select Sound source")
CODE (L"#plus Formant filter")
CODE (L"Filter")
NORMAL (L"If you had computed the source and filter from an LPC analysis, this procedure will not generally give "
	"you back the original Sound, because some linear-prediction coefficients will have been ignored "
	"in the conversion to formant-bandwidth pairs.")
NORMAL (L"If your filter is a @FormantGrid object, you select it and the source, and choose @@Sound & FormantGrid: Filter@:")
CODE (L"#select Sound source")
CODE (L"#plus FormantGrid filter")
CODE (L"Filter")
NORMAL (L"Finally, you could just know the %%impulse response% of your filter (in a @Sound object). "
	"You then select both Sound objects, and choose @@Sounds: Convolve...@:")
CODE (L"#select Sound source")
CODE (L"#plus Sound filter")
CODE (L"Convolve... integral zero")
NORMAL (L"As a last step, you may want to bring the resulting sound within the [-1; +1] range:")
CODE (L"Scale peak... 0.99")
ENTRY (L"4. How to manipulate the filter")
NORMAL (L"You can hardly change the values in an @LPC object in a meaningful way: "
	"you would have to manually change its rather opaque data with the help of @Inspect.")
NORMAL (L"A @Formant object can be changed in a friendlier way, with @@Formant: Formula (frequencies)...@ "
	"and @@Formant: Formula (bandwidths)...@. For instance, to multiply all formant frequencies by 0.9, "
	"you do")
CODE (L"#select Formant filter")
CODE (L"Formula (frequencies)... self * 0.9")
NORMAL (L"To add 200 hertz to all values of %F__2_, you do")
CODE (L"Formula (frequencies)... if row = 2 then self + 200 else self fi")
NORMAL (L"A @FormantGrid object can be changed by adding or removing points:")
LIST_ITEM (L"@@FormantGrid: Add formant point...@")
LIST_ITEM (L"@@FormantGrid: Add bandwidth point...@")
LIST_ITEM (L"@@FormantGrid: Remove formant points between...@")
LIST_ITEM (L"@@FormantGrid: Remove bandwidth points between...@")
ENTRY (L"5. How to manipulate the source signal")
NORMAL (L"You can manipulate the source signal in the same way you that would manipulate any sound, "
	"for instance with the @ManipulationEditor.")
MAN_END

MAN_BEGIN (L"Spectrogram settings...", L"ppgb", 20030316)
INTRO (L"A command in the Spectrogram menu of the @SoundEditor and @TextGridEditor windows. "
	"See @@Intro 3.2. Configuring the spectrogram@.")
MAN_END

MAN_BEGIN (L"Technical", L"ppgb", 20120915)
INTRO (L"The title of a submenu of the @@Praat menu@.")
MAN_END

MAN_BEGIN (L"Types of objects", L"ppgb", 20101230)
INTRO (L"Praat contains the following types of objects and @Editors. "
	"For an introduction and tutorials, see @Intro.")
NORMAL (L"General purpose:")
LIST_ITEM (L"\\bu @Matrix: a sampled real-valued function of two variables")
LIST_ITEM (L"\\bu @Polygon")
LIST_ITEM (L"\\bu @PointProcess: a point process (@PointEditor)")
LIST_ITEM (L"\\bu @Sound: a sampled continuous process (@SoundEditor, @SoundRecorder, @@Sound files@)")
LIST_ITEM (L"\\bu @LongSound: a file-based version of a sound (@LongSoundEditor)")
LIST_ITEM (L"\\bu @Strings")
LIST_ITEM (L"\\bu @Distributions, @PairDistribution")
LIST_ITEM (L"\\bu @Table, @TableOfReal")
LIST_ITEM (L"\\bu @Permutation")
LIST_ITEM (L"\\bu @ParamCurve")
NORMAL (L"Periodicity analysis:")
LIST_ITEM (L"\\bu Tutorials:")
LIST_ITEM1 (L"\\bu @@Intro 4. Pitch analysis")
LIST_ITEM1 (L"\\bu @@Intro 6. Intensity analysis")
LIST_ITEM1 (L"\\bu @Voice (jitter, shimmer, noise)")
LIST_ITEM (L"\\bu @Pitch: articulatory fundamental frequency, acoustic periodicity, or perceptual pitch (@PitchEditor)")
LIST_ITEM (L"\\bu @Harmonicity: degree of periodicity")
LIST_ITEM (L"\\bu @Intensity, @IntensityTier: intensity contour")
NORMAL (L"Spectral analysis:")
LIST_ITEM (L"\\bu Tutorials:")
LIST_ITEM1 (L"\\bu @@Intro 3. Spectral analysis")
LIST_ITEM1 (L"\\bu @@Intro 5. Formant analysis")
LIST_ITEM (L"\\bu @Spectrum: complex-valued equally spaced frequency spectrum (@SpectrumEditor)")
LIST_ITEM (L"\\bu @Ltas: long-term average spectrum")
LIST_ITEM (L"\\bu Spectro-temporal: @Spectrogram, @BarkFilter, @MelFilter, @FormantFilter")
LIST_ITEM (L"\\bu @Formant: acoustic formant contours")
LIST_ITEM (L"\\bu @LPC: coefficients of Linear Predictive Coding, as a function of time")
LIST_ITEM (L"\\bu @Cepstrum, @CC, @LFCC, @MFCC (cepstral coefficients)")
LIST_ITEM (L"\\bu @Excitation: excitation pattern of basilar membrane")
LIST_ITEM (L"\\bu @Excitations: an ensemble of #Excitation objects")
LIST_ITEM (L"\\bu @Cochleagram: excitation pattern as a function of time")
NORMAL (L"Labelling and segmentation (see @@Intro 7. Annotation@):")
LIST_ITEM (L"\\bu @TextGrid (@TextGridEditor)")
NORMAL (L"Listening experiments:")
LIST_ITEM (L"\\bu @ExperimentMFC")
NORMAL (L"Manipulation of sound:")
LIST_ITEM (L"\\bu Tutorials:")
LIST_ITEM1 (L"\\bu @@Intro 8.1. Manipulation of pitch")
LIST_ITEM1 (L"\\bu @@Intro 8.2. Manipulation of duration")
LIST_ITEM1 (L"\\bu @@Intro 8.3. Manipulation of intensity")
LIST_ITEM1 (L"\\bu @@Filtering")
LIST_ITEM1 (L"\\bu @@Source-filter synthesis")
LIST_ITEM (L"\\bu @PitchTier (@PitchTierEditor)")
LIST_ITEM (L"\\bu @Manipulation (@ManipulationEditor): @@overlap-add@")
LIST_ITEM (L"\\bu @DurationTier")
LIST_ITEM (L"\\bu @FormantGrid")
NORMAL (L"Articulatory synthesis (see the @@Articulatory synthesis@ tutorial):")
LIST_ITEM (L"\\bu @Speaker: speaker characteristics of a woman, a man, or a child")
LIST_ITEM (L"\\bu #Articulation: snapshot of articulatory specifications (muscle activities)")
LIST_ITEM (L"\\bu @Artword: articulatory target specifications as functions of time")
LIST_ITEM (L"\\bu (@VocalTract: area function)")
NORMAL (L"Neural net package:")
LIST_ITEM (L"\\bu @FFNet: feed-forward neural net")
LIST_ITEM (L"\\bu @Pattern")
LIST_ITEM (L"\\bu @Categories: for classification (#CategoriesEditor)")
NORMAL (L"Numerical and statistical analysis:")
LIST_ITEM (L"\\bu @Eigen: eigenvectors and eigenvalues")
LIST_ITEM (L"\\bu @Polynomial, @Roots, @ChebyshevSeries, @LegendreSeries, @ISpline, @MSpline")
LIST_ITEM (L"\\bu @Covariance: covariance matrix")
LIST_ITEM (L"\\bu @Confusion: confusion matrix")
LIST_ITEM (L"\\bu @@Discriminant analysis@: @Discriminant")
LIST_ITEM (L"\\bu @@Principal component analysis@: @PCA")
LIST_ITEM (L"\\bu @Correlation, @ClassificationTable, @SSCP")
LIST_ITEM (L"\\bu @DTW: dynamic time warping")
NORMAL (L"@@Multidimensional scaling@:")
LIST_ITEM (L"\\bu @Configuration (@Salience)")
LIST_ITEM (L"\\bu @@Kruskal analysis@: @Dissimilarity (@Weight), @Similarity")
LIST_ITEM (L"\\bu @@INDSCAL analysis@: @Distance, @ScalarProduct")
LIST_ITEM (L"\\bu @@Correspondence analysis@: @ContingencyTable")
NORMAL (L"Optimality-theoretic learning (see the @@OT learning@ tutorial)")
LIST_ITEM (L"\\bu @OTGrammar (@OTGrammarEditor)")
NORMAL (L"Bureaucracy")
LIST_ITEM (L"\\bu @WordList, @SpellingChecker")
MAN_END

MAN_BEGIN (L"Unicode", L"ppgb", 20110129)
INTRO (L"Praat is becoming a fully international program: "
	"the texts in Praat's TextGrids, Tables, scripts, or Info window (and elsewhere) can contain many types of characters "
	"(see @@special symbols@). "
	"For this reason, Praat saves its text files in one of two possible formats: ASCII or UTF-16.")
ENTRY (L"ASCII text files")
NORMAL (L"If your TextGrid (or Table, or script, or Info window...) contains only characters that can be encoded as ASCII, "
	"namely the characters !\\\" \\# \\$ \\% &\\' ()*+,-./0123456789:;<=>?\\@  "
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\bs]\\^ \\_ ` abcdefghijklmnopqrstuvwxyz{|}~, "
	"then when you say @@Save as text file...@ or #Save, Praat will write an ASCII text file, "
	"which is a text file in which every character is encoded in a single byte (8 bits). "
	"All programs that can read plain text files can read such files produced by Praat.")
ENTRY (L"UTF-16 text files")
NORMAL (L"If your TextGrid (or Table, or script, or Info window...) contains one or more characters that cannot be encoded as ASCII, "
	"for instance West-European characters such as \\ao\\c,\\e'\\o\"\\ss\\?d, East-European characters such as \\c<\\l/\\o:\\s<\\uo\\z', "
	"or Hebrew characters such as \\?+\\B+\\G+\\M%\\vO\\sU, "
	"then when you say @@Save as text file...@ or #Save, Praat will write an UTF-16 text file, "
	"which is a text file in which every character is encoded in two bytes (and some very rare characters in four bytes). "
	"Many programs can read such text files, for instance NotePad, WordPad, Microsoft Word, and TextWrangler.")
ENTRY (L"What if my other programs cannot read UTF-16 text files?")
NORMAL (L"If you want to export your Table to Microsoft Excel or to SPSS, or if you want your TextGrid file to be read "
	"by somebody else's Perl script, then there will be no problem if your Table contains only ASCII characters (see above). "
	"But if your Table contains any other (i.e. non-ASCII) characters, you may be in trouble, because Praat will write the Table "
	"as an UTF-16 text file, and not all of the programs just mentioned can read such files yet.")
NORMAL (L"What you can do is go to ##Text writing preferences...# in the #Preferences submenu of the #Praat menu, "
	"and there set the output encoding to ##UTF-8#. Praat will from then on save your text files in the UTF-8 format, "
	"which means one byte for every ASCII character and 2 to 4 bytes for every non-ASCII character. "
	"Especially on Linux, many programs understand UTF-8 text and will display the correct characters. "
	"Programs such as SPSS do not understand UTF-8 but will still display ASCII characters correctly; "
	"for instance, the names M\\u\"nchen and Wa\\l/\\e;sa may appear as M\\A~\\:-nchen and Wa\\Ao,\\A\"\\TMsa or so.")
NORMAL (L"If you can get by with West-European characters (on Windows), "
	"then you may choose ##try ISO Latin-1, then UTF-16# for the output encoding. "
	"It is possible (but not guaranteed) that programs like SPSS then display your West-European text correctly. "
	"This trick is of limited use, because it will not work if your operating system is set to a \"codepage\" "
	"differently from ISO Latin-1 (or \"ANSI\"), or if you need East-European or Hebrew characters, or if you want "
	"to share your text files with Macintosh users.")
NORMAL (L"If you already have some UTF-16 text files and you want to convert them to UTF-8 or ISO Latin-1 (the latter only if "
	"they do not contain non-West-European characters), then you can read them into Praat and save them again "
	"(with the appropriate output encoding setting). "
	"Other programs, such a NotePad and TextWrangler, can also do this conversion.")
NORMAL (L"Finally, it is still possible to make sure that all texts are ASCII, e.g. you type the characters \\ss and \\o: "
	"as \\bsss and \\bso: respectively. See @@special symbols@.")
MAN_END

MAN_BEGIN (L"View", L"ppgb", 20010512)
INTRO (L"One of the menus in several @editors and in the @manual.")
MAN_END

MAN_BEGIN (L"Write to console", L"ppgb", 20110129)
INTRO (L"One of the commands in the @@Save menu@.")
NORMAL (L"You can choose this command after selecting one object. "
	"The data that it contains, is written to the Console window "
	"(the terminal window, if you started up Praat from a terminal window), "
	"in the same format as with the @@Save as text file...@ command, "
	"except for the first line, which reads something like:")
CODE (L"Write to console: class Sound,  name \"hallo\"")
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
