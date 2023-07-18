/* manual_tutorials.cpp
 *
 * Copyright (C) 1992-2023 Paul Boersma
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

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"What's new?"
© Paul Boersma 2023

Latest changes in Praat.

##6.3.11# (17 July 2023)
• VowelEditor: repaired a bug by which the VowelEditor wouldn’t start up if an incorrect marks file had previously been specified.
• Mac: use a Cherokee font that distinguishes "ka" and "wo" well.
• OTMulti: vertical constraint names support newlines and disharmonies.
• Text windows are one tab plus 84.5 characters wide.
• Table: ##Get all values in column...#.
• Undocumented alpha support for Praat notebooks.
• Formulas: @`correlation` (%`vec1#`, %`vec2#`), @`vertical$` (for formatting string arrays).
• Scripting: @`readFile#`, @`readFile##`.

##6.3.10# (3 May 2023)
• Windows: repaired a bug by which key strokes would no longer go to the Demo window after a pause window with no text fields was closed.
• Repaired a bug in the VowelEditor by which it would crash when starting up if fewer than 4 numbers had been set in the “Extra formant–bandwidth pairs” field.
• FormantPathEditor improvements.
• Undocumented alpha support for Praat notebooks.
• Scripting: vector literals can be indexed.
• Better interactions between editor scripts and pause windows.

##6.3.09# (2 March 2023)
• SpeechSynthesizer can do eSpeak’s grapheme-to-phoneme conversion in 130 languages.
• FormantPath can now have time stretches that are not aligned to frame boundaries.
• Stabler handling of edge cases in editor scripting.
• Corrected an at least 23-year-old bug by which menu titles could not be greyed out on Windows.

##6.3.08# (10 February 2023)
• Corrected a bug introduced in 6.3.04 by which `word` and `sentence` would not work in new-style script forms.

##6.3.07# (6 February 2023)
• Corrected an old bug that caused ##Matrix: Draw as squares# to crash on a 1\xx1 matrix.
• NoulliGrid window: “NoulliGrid” menu.
• Scripting: faster vectors in arguments to `runScript()`.

##6.3.06# (31 January 2023)
• Corrected a bug introduced in 6.3.04 by which `optionmenu` would not work in new-style script forms.
• Scripting: `min()`, `max()`, `imin()` and `imax()` for vectors.

##6.3.05# (27 January 2023)
• Corrected a bug introduced in 6.3.04 by which some old-style forms could not be read.
• Repaired an old bug by which `chooseReadString$()` could crash on Linux.

##6.3.04# (24 January 2023)
• Playing multiple sounds with #Play now concatenates them without breaks,
and is interruptable by the Escape key.
• Renamed the remaining occurrences of #Preferences to #Settings
	(old scripts will continue to work).
• ##Create Polygon from values...
• ##Sound: To Sound (derivative)...
• Scripting: forms now look more like other commands.
• Scripting: forms can contain `infile`, `outfile`, `folder`, `realvector`,
	`positivevector`, `integervector` and `naturalvector` fields.
• Scripting: in forms and pause forms, you can now set the number of lines in
	`text`, `infile`, `outfile`, `folder`, and vector fields.

##6.3.03# (17 December 2022)
• Sound files: read Kay\re Nasometer\tm files (as separate channels).
• Script window: indent and exdent.
• Scripting: conserve editor environment in `runScript`.

##6.3.02# (29 November 2022)
• Linux: corrected a bug introduced in 6.2.20 by which typing Tab would not start a Sound.
• Sound window: corrected a bug introduced in 6.2.19 by which drawing selection hairs could not be switched off
	(or on) when painting a spectrogram to the Picture window.

##6.3.01# (21 November 2022)
• Fixed a crash introduced in 6.3 when closing an editor window by choosing #Close (or by typing Command-W).

What used to be new?
====================
• @@What was new in 6.3?
• @@What was new in 6.2?
• @@What was new in 6.1?
• @@What was new in 6.0?
• @@What was new in 5.4?
• @@What was new in 5.3?
• @@What was new in 5.2?
• @@What was new in 5.1?
• @@What was new in 5.0?
• @@What was new in 4.6?
• @@What was new in 4.5?
• @@What was new in 4.4?
• @@What was new in 4.3?
• @@What was new in 4.2?
• @@What was new in 4.1?
• @@What was new in 4.0?
• @@What was new in 3.9?
• @@What was new in 3.8?
• @@What was new in 3.7?
• @@What was new in 3.6?
• @@What was new in 3.5?
• @@What was new in 3.3?
• @@What was new in 3.2?
• @@What was new in 3.1?

################################################################################
"What was new in 6.3?"
© Paul Boersma 2022

##6.3# (15 November 2022)
• Number-aware sorting for labels when drawing tables.
• Scripting: @`sort$#` (), @`shuffle$#` (), @`sort_numberAware$#` ().
• Fixed a crash when saving (or copying) a DTW object.
• Scripting: fixed a crash when using `Close` after @`editor`.

##6.2.23# (8 October 2022)
• Fix a crash introduced in August 2020 when in the Sound window you would select a part, then click #sel,
  then select a part, then do #Cut, then click #bak.
• #Rename should leave an object’s ID in the title of the editor window.
• Windows graphics should support a resolution of 690 dots per inch.
• Editor scripting: corrected a bug introduced in 6.2.15 whereby an added editor script would lose
  the editor context if it contained a form, after #Apply was clicked.
• Editor windows (such as the Sound window): showing a dialog window that calls a script
  (e.g. ##Log script 3#, ##Log script 4# or an added command) should remove the previous version of that window.
• Mac editor windows: dialog windows that call scripts should disappear
  when the editor goes away (as on Windows and Linux).

##6.2.22# (6 October 2022)
• Playing cursor in Pitch, PitchTier, KlattGrid and PointProcess windows.
• Corrected a bug introduced in 6.2.15 whereby an added editor script would lose
  the editor context if it contained a form, after #OK was clicked.

##6.2.21# (1 October 2022)
• Corrected a bug introduced in 6.2.15 whereby a sub-window in #Inspect would crash upon clicking #Change.

##6.2.20# (24 September 2022)
• More balanced colours.
• Hierarchical menus in editor windows.
• Sound window scripting: understand `editor$` again in log format (bug since 6.2.15).

##6.2.19# (12 September 2022)
• Visible window queries in #Time menu.
• Correct execution of added Objects window script after running a failing editor script (decades-old bug).
• Made ##Erase first# work again (bug introduced in 6.2.15).

##6.2.18# (2 September 2022)
• SoundRecorder: support for up to 20 (instead of 8) input devices.
• FormantPath window: can now be viewed even without a Sound.
• Windows audio: SoundRecorder prevents high CPU usage when not recording.

##6.2.17# (23 August 2022)
• FormantPath window: more information shown.
• Windows audio: compatible with speakers that do not support mono.
• Editor scripts: made two old commands available again (removed in 6.2.15).

##6.2.16# (18 August 2022)
• FormantPath window: non-editable TextGrid.
• Scripting the Demo window on the Mac: better guarantees that `demoShow` () actually drains on current systems.
• TextGrid works again with LongSound (bug introduced in 6.2.15).
• Editor scripts work again from menus (bug introduced in 6.2.15).

##6.2.15# (14 August 2022)
• Legends in editor windows.
• The menus #Time and #Play are new in editor windows, and the menus #Spectrum and #Formant were renamed to #Spectrogram and #Formants.
• More menu commands in editor windows.
• Rid frames around areas in editor window, except if editable.
• The FormantPath window can include a copy of a TextGrid.
• Restored horizontal and vertical dragging in tier areas (had been absent for two years).
• Removed several small bugs (and probably introduced a couple of new ones) in editor windows.
• Restored the original Praat icon.

##6.2.14# (24 May 2022)
• TextGrid window: repaired a bug introduced in 6.2.10 by which a click in the boundary insertion circle
  would copy the contents of a new interval from another tier (if that other tier stood selected).

##6.2.13# (18 May 2022)
• Playing a sound on the Mac: prevented an audible click at the end of audio output (appending a silence at the end is no longer needed).
• FormantPath window: more flexible visualization.
• New versions of PortAudio and Unicode.
• Removed the sendpraat directive.
• Removed several small interface bugs, and improved manual.

##6.2.12# (17 April 2022)
• @@Table: Get sum...@
• Scripting an editor window can now query analyses that are not visible (e.g. after zooming).

##6.2.11# (8 April 2022)
• Windows: Demo window could hang with use of Chinese keyboard (after #Play).

##6.2.10# (17 March 2022)
• TextGrid window: restored the splitting of labels.
• @@Save Demo Window as PDF file...@ (#Goodies menu).

##6.2.09# (15 February 2022)
• Windows: support for a screen resolution of 204 dpi.

##6.2.08# (5 February 2022)
• Mac: more checks on audio devices when playing sounds.

##6.2.07# (28 January 2022)
• Prevented a rare crash in the TextGrid window.
• Windows: corrected a bug introduced in 6.2.04 by which some file names were unreadable.

##6.2.06# (20 January 2022)
• Mac: prevent Demo window from hanging on copy or paste.

##6.2.05# (5 January 2022)
• Calling Praat from the command line: the switch `--send` sends a script
  to a different already running GUI instance of Praat; this is more flexible than using %sendpraat,
  because `--send` will start a new instance of Praat if Praat is not yet running.

##6.2.04# (18 December 2021)
• Praat now includes eSpeak 1.51, raising the number of supported languages from 100 to 130.
• Scripting: support for `infile`, `outfile` and `folder` fields in forms.
• Calling Praat from the command line: the switch `--open` now adds files to an already running instance of Praat;
  this was already the case on Windows, but it now works in the same way on the Mac and on Linux.
• Scripting on Mac: multiline text fields in forms have scroll bars and behave like other fields.

##6.2.03# (3 December 2021)
• Mac: made Enter key work as expected (i.e. character selection) on Japanese keyboards in multi-line text fields in dialogs and TextGrid window.

##6.2.02# (2 December 2021)
• Fixed a hang if there were negative pitch values in @@Sound: Change gender...@.
• Fixed a crash if the size of a PCA did not match the size of a TableOfReal.

##6.2.01# (17 November 2021)
• Sound window: corrected a bug that caused a thick waveform during or after playing.

################################################################################
"What was new in 6.2?"
© Paul Boersma 2021

##6.2# (15 November 2021)
• TextGrid window: closing box and opening triangle for IPA chart.
• Linux: compatibility with Wayland (rather than just X11).
• Chromebook: surrogate window title replaces the one that Chromebooks won’t show.
• Much longer list of functions in manual.

##6.1.56# (3 November 2021)
• Scripting: @@Record Sound (fixed time)...@ should return an object ID.
• More phonetic symbols in EPS files.

##6.1.55# (25 October 2021)
• TextGrid window: guard against a rare crash on Windows.
• TextGrid: Extract part: guard against producing an empty interval tier.
• Plug-ins: settings windows should not show the file name of the script.
• Scripting: clicking Cancel when browsing an Infile field should not crash.
• Phonetic symbols: a new backslash trigraph for a mid-centralized diacritic (as in [e\x^]), namely `\x^`.

##6.1.54# (9 October 2021)
• PowerCepstrogram: can tabulate cepstral peak prominence values.
• Mac: repaired global Escape key menu shortcut (stops sounds that play from the Objects window).

##6.1.53# (8 September 2021)
• Linux: corrected a bug that caused PDF files to be misaligned
  if the origin was not in the top left corner.

##6.1.52# (25 August 2021)
• FormantPath window: more formant listings and stress listings.
• Sound window: on #Cut or #Paste, all windows that contain that Sound now get ungrouped.
• Corrected a crash when reading Vorbis files on some 32-bit platforms.

##6.1.51# (22 July 2021)
• @@Spectrum: Tabulate (verbose)@ with:  power spectral density, bin width within domain, bin energy, and much more.
• Sound window: ##Get amplitude(s)#.
• Scripting: `date_utc$` (), `date#` (), `date_utc#` ().

##6.1.50# (20 June 2021)
• RealTier: editing, Formula, conversion from and to Matrix and Table and other tiers such as PitchTier,
  DurationTier, IntensityTier and AmplitudeTier.
• Scripting: can now assign multiple objects to a vector.
• Fix crashes in ##Create letter R example# and ##Create KlattTable example# introduced in 6.1.49.

##6.1.49# (12 June 2021)
• @PitchTier, @IntensityTier, @DurationTier and @AmplitudeTier windows: save preferences.
• @Manipulation window: removed unused semitone options.
• @RealTier object: editing; conversions from and to other tiers and from Table and Matrix.

##6.1.48# (2 June 2021)
• TextGrid window: Shift-drag drags multiple boundaries again even if the Shift key is not released.
• Scripting: publishing (#Extract) an object from an editor window returns its ID again.
• Scripting: ##Select...# in an editor window is now prevented from selecting outside the time domain.

##6.1.47# (21 May 2021)
• The Escape key now means Cancel also in a non-pausing pause window.

##6.1.46# (20 May 2021)
• Fixed a bug with line types in the Picture window.
• TextGrid window: shift-click on a boundary again extends the selection.

##6.1.45# (20 May 2021)
• Praat picture files can be drawn with ##Read from file# again.

##6.1.44# (17 May 2021)
• Scripting: queries in editor windows are scriptable again.

##6.1.43# (13 May 2021)
• TextGrid window: characters in the IPA chart grow with the window.
• TextGrid window: seeing the maroon shadow now always means that the boundary will be moved when the mouse is released (problem since 6.1.20 or so).
• Scripting: pause window on the Mac: the Enter key works as designed (bug in 6.1.42).

##6.1.42# (15 April 2021)
• Settings windows: taller text fields for formulas and vectors.
• Mac TextGrid window: cursor moves to end of text after selecting an interval (as on Windows and Linux).

##6.1.41# (25 March 2021)
• TextGrid window: unselected boundaries can again be drawn to the cursor.
• Windows: faster running cursor.
• Multi-line formula fields.
• Scripting: guard against too deep recursive calls to runScript (depth 20).
• EEG: repaired a bug that could cause Praat to crash when computing an ERPTier.

##6.1.40# (27 February 2021)
• Inspect window: corrected a bug that could cause Praat to crash when opening a string vector.
• Sound window: made channel scrolling work again when there are more than 8 channels.
• Sound window: made channel muting by clicking on the loudspeaker symbols work again.

##6.1.39# (8 February 2021)
• Scripting: made string vectors visible in writeFile (), pauseScript () and exitScript ().
• Mac: corrected a bug that could cause About Praat to crash on macOS 10.10.

##6.1.38# (2 January 2021)
• Scripting: string vectors, @`empty$#` (), @`readLinesFromFile$#` (), @`fileNames$#` (), @`folderNames$#` (), @`splitByWhitespace$#` ().
• Sound files: open Ogg Vorbis files and Opus files.

##6.1.37# (16 December 2020)
• TextGrid window: removed a bug that caused Praat to crash when option-clicking or command-clicking a boundary when another boundary was selected on a point tier.
• Record Sound (fixed time): removed a bug that caused Praat to crash with certain combinations of duration and sampling frequency on Windows.
• PowerCepstrogram: more precise integration, so that the cepstral peak prominence is now a continuous function of time.
• TextGrid: Get points (preceded): removed a bug that caused Praat to crash under some edge conditions.
• Scripting: @`tryToWriteFile` () and @`tryToAppendFile` () should not be available in manuals.

##6.1.36# (6 December 2020)
• Scripting: @`tryToWriteFile` (), @`tryToAppendFile` (); formulas can have a size of 1 million.
• Obscured scripts: error messages always disclosed.
• IPA chart: sacrifice laminality symbol for breathiness symbol.

##6.1.35# (29 November 2020)
• Scripting: @`sort#` (), @`shuffle#` (), @`to#` (), @`from_to#` (), @`from_to_by#` (), @`from_to_count#` (), @`between_by#` (), @`between_count#` ().
• Mac: repaired a bug existing since 6.1.04 that caused incorrect line counts in text files with high Unicode characters.

##6.1.34# (25 November 2020)
• Mac: got bold and italic to work on Praat versions compiled with the BigSur API.
• SoundRecorder always a white background, instead of a different background on each platform.

##6.1.33# (19 November 2020)
• Mac: got audio to work on Praat versions compiled with the BigSur API (6.1.32 did not have audio on BigSur).

##6.1.32# (16 November 2020)
• First version for Macs with Apple Silicon chip.
• Mac: corrected a bug that could cause Praat to crash when a Sound window was closed from a script.
• Windows and Linux: made ##Text width...# work correctly from batch.
• Linux: reinstalled Tab shortcut in Sound window.

##6.1.31# (10 November 2020)
• Linux: first GTK-3 version.

##6.1.30# (3 November 2020)
• Windows: prevented a crash that could occur in the Sound window when clicking with the touchpad while dragging with the mouse (or the reverse).

##6.1.29# (27 October 2020)
• Mac: corrected a bug revealed by Xcode 12 since 6.1.22 that could cause Praat to crash when using empty vectors,
  such as when querying a Harmonicity for its mean in regions without frames.

##6.1.28# (20 October 2020)
• Pause forms: text fields can be multi-line.
• Removed a bug that caused Praat to crash when editing a PitchTier.
• Script window: got rid of too many history entries after dragging in the Picture window.

##6.1.27# (13 October 2020)
• Demo window: make sure that the contents of the window become visible
  more often without calling demoShow() or demoWaitForInput().

##6.1.26# (5 October 2020)
• Windows: removed a bug that caused Praat to crash if you clicked in the Sound window,
then dragged the mouse out of that window, then released the mouse button, and then clicked in the Sound window again.
• Linux: removed a bug that caused Praat to crash when clicking Change in the Inspect window.

##6.1.25# (4 October 2020)
• @FormantPath and @FormantPathEditor.
• Windows: implemented vertical scrolling with the mouse wheel in the manual and in the Picture window.
• Linux: removed a bug that caused Praat to crash when double-clicking in the Sound window.
• Linux: removed a bug that caused Praat to crash when raising the About window twice.
• Removed a bug that caused Praat to crash when zooming in
  to a region in the Sound window without samples, when a pitch curve was visible.
• Removed a bug that caused Praat to crash when pasting in
  a region in the Sound window without samples.

##6.1.24# (29 September 2020)
• Linux: got rid of flashing during a running cursor or
  when making a selection in the Sound window or the Picture window.

##6.1.23# (28 September 2020)
• Windows: got rid of flashing during a running cursor or
  when making a selection in the Sound window or the Picture window.
• Linux: corrected a bug that caused the buttons at the top of a manual window
  to be overwritten with text when scrolling.

##6.1.22# (24 September 2020)
• Windows: made the running cursor visible again.
• Corrected a bug introduced in 6.1.17alpha that could cause incorrect line spacing in the Picture window,
  such as in @@TableOfReal: Draw as numbers@.

##6.1.21# (20 September 2020)
• First fully functional version for BigSur.
• Removed a decades-old bug by which an extremum allegedly computed by cubic interpolation
  would actually have been computed by sinc700.

##6.1.20beta# (10 September 2020)
• Second beta version for macOS Big Sur.

##6.1.19beta# (7 September 2020)
• First beta version for macOS Big Sur.

##6.1.18alpha# (1 September 2020)
• Second alpha version for macOS Big Sur.

##6.1.17alpha# (16 August 2020)
• Alpha version for macOS Big Sur.

##6.1.16# (6 June 2020)
• @@Record Sound (fixed time)...@: more reliable choice of input device.
• Mac: notice plugging and unplugging of headphones.

##6.1.15# (20 May 2020)
• Repaired a bug introduced in 6.0.44 that could cause an incorrect (namely, totally constant) ClassificationTable.

##6.1.14# (2 May 2020)
• Repaired a bug in drawing ranges introduced in 6.1.06.

##6.1.13# (19 April 2020)
• Repaired discriminant analysis on i386 processors.
• Scripting: enabled predictability by setting random seeds.
• Automated batch testing on x86_64, i386 and ARMv7 processors.

##6.1.12# (13 April 2020)
• All non-GUI tests can be automated from batch.
• Repaired a bug introduced in 6.0 whereby a KlattGrid-generated Sound would always be peak-scaled.

##6.1.11# (6 April 2020)
• Better messages.
• Overhauled VowelEditor.
• Repaired crashes after creating a Formant from a Spectrum or an Excitation.

##6.1.10# (23 March 2020)
• Faster formant computation.
• More LPC queries.

##6.1.09# (26 January 2020)
• Accept some more kinds of broken WAV files.
• Improved box plot drawing.
• More Network commands.

##6.1.08# (5 December 2019)
• Repaired a bug with automatic alignment for TextGrids (introduced in 6.1.07).

##6.1.07# (26 November 2019)
• Raspberry Pi edition.

##6.1.06# (8 November 2019)
• Chromebook edition.
• Scripting: colour fields now take name, grey value or RGB vector.

##6.1.05# (16 October 2019)
• Repaired a bug introduced in 6.0.44 that could cause rubbish LPC smoothing.
• Repaired a rare crash when dragging a selection on the Mac.

##6.1.04# (28 September 2019)
• @Electroglottography.
• Sound and other windows: ##Widen or shrink selection...#.
• @@KlattGrid: Create from vowel...@.
• Fix rectangle playing bug when selection viewer is on.
• Selection viewer separately visible for Sound window and TextGrid window.
• Scripting: allow comments after parameter list.
• Scripting: fix CR/LF pasting from Microsoft Word.

##6.1.03# (1 September 2019)
• Sound window: ##Widen or shrink selection...#.

##6.1.02# (25 August 2019)
• Repaired @@Sound: Concatenate with overlap...@.
• Mac: Info and script windows: prevent line breaks caused by tab stops.

##6.1.01# (14 August 2019)
• Repaired @@TextGrid: Replace interval texts...@.

################################################################################
"What was new in 6.1?"
© Paul Boersma 2019

##6.1# (13 July 2019)

##6.0.57# (1 July 2019)
• Repaired a crash when an editor window zoomed in to a zero time range.
• Repaired a crash when playing a matrix movie with one row or a constant value.
• Repaired a crash when reconstructing a TableOfReal from a PCA.
• Scripting: allow comments after `else`, `endif`, `endfor`...

##6.0.56# (20 June 2019)
• Windows: file dropping on the Praat icon works for higher-Unicode file names.
• SpellingChecker: Unicode support.

##6.0.55# (13 June 2019)
• Unicode normalization in file names.

##6.0.54# (6 June 2019)
• Removed a bug introduced in 6.0.51 that could cause incorrect axes in Demo window.

##6.0.53# (26 May 2019)
• Much faster playing of short parts of long sounds that need resampling.
• Better handling of broken CSV files.
• 64-bit floating-point WAV files.

##6.0.52# (2 May 2019)
• Removed a bug introduced in 6.0.51 that could cause turning a Discriminant into a Configuration to crash.
• Removed a bug introduced in contour grey drawing in August 2017.

##6.0.51# (29 April 2019)
• Script window: Use Selection for Find.
• Removed a bug introduced in 6.0.41 that could cause Praat to crash after removing an element from a Strings or a row from a TableOfReal.

##6.0.50# (31 March 2019)
• Manual updates, speed, more tests.
• Scripting: @`rowSums#` (), @`columnSums#` (); @`randomGauss##` () finally with four arguments.

##6.0.49# (2 March 2019)
• Removed a bug introduced in 6.0.41 whereby a script could misreport an undefined table value.
• Removed a bug introduced in 6.0.44 whereby an MFCC's maximum frequency could be ignored.
• Pitch: Tabulate candidates.
• SoundSet.

##6.0.48# (17 February 2019)
• Removed a bug introduced in 6.0.44 whereby Praat could crash when drawing a function without any points.
• Removed a bug whereby Praat would not start up on macOS 10.10 (because of required GPU libraries).

##6.0.47# (8 February 2019)
• Removed a bug introduced in 6.0.44 whereby a cepstral peak prominence would be computed incorrectly.

##6.0.46# (3 January 2019)
• Removed a crashing bug in the LongSound window introduced in 6.0.44.
• Removed a bug causing incorrect conversion from Formant to LPC introduced in 6.0.44.
• More space for actions in buttons window.

##6.0.45# (1 January 2019)
• Removed a crashing bug in EditDistanceTable introduced in 6.0.44.
• Removed a crashing bug in KlattGrid drawing introduced in 6.0.44.
• Removed a bug introduced in 6.0.44 whereby old HMM files were read incorrectly.

##6.0.44# (31 December 2018)
• Bug fixes and speed.

##6.0.43# (8 September 2018)
• Removed a very old bug that could create invalid TextGrids during forced alignment.
• Windows: repaired a bug introduced in 6.0.41
  that caused a crash in several places (such as when changing the sample rate in the SoundRecorder)
  when Praat had been started with a double click.
• Windows: repaired a bug introduced in 6.0.41 that caused Praat to crash when exiting.

##6.0.42# (15 August 2018)
• Repaired a bug introduced in 6.0.41 that caused a crash in pause windows.

##6.0.41# (6 August 2018)
• Interoperability: the Windows edition can now communicate with other software in UTF-8.

##6.0.40# (11 May 2018)
• Scripting: $$@@Scripting 4.3. Querying objects|`selected#`@$.
• Table window: cell selection.
• Unicode-savvy regular expressions.

##6.0.39# (3 April 2018)
• More string matching (case-insensitive, word-level).

##6.0.38# (29 March 2018)
• Concordance: @@TextGrid: Tabulate occurrences...@.
• Some @@Independent Component Analysis on EEG@.
• Removed many small bugs.

##6.0.37# (3 February 2018)
• Graphics: fixed crashing bug with \\bsun trigraph.

##6.0.36# (11 November 2017)
• Many more languages for SpeechSynthesizer and automatic alignment (new eSpeak).

##6.0.35# (16 October 2017)
• Fixed a bug introduced in 6.0.23 by which ##Remove right boundary# would sometimes incorrectly refuse.

##6.0.34# (10 October 2017)
• Scripting: arrays in menu commands in scripts.
• #Inspect: made a button visible that had disappeared in 6.0.33.

##6.0.33# (26 September 2017)
• @@Pitch: Subtract linear fit...@: correct a bug that prevented computation in the ERB domain.
• Scripting: made `object[xx].xmin` work again.
• Removed a bug in @@Polynomial: Evaluate...@ introduced in 6.0.32.

##6.0.32# (16 September 2017)
• Table: allow spaces in column names.
• Settings windows no longer replace your visible expressions with their resulting values.
• Scripting: formulas using the tilde.
• Optimality Theory learning: corrected a bug introduced in 6.0.30 that could cause the editor to crash on the Mac.
• EEG: corrected a bug that could cause an infinite loop when doing principal component analysis.
• Scripting: faster interpreter.

##6.0.31# (21 August 2017)
• Scripting: more vectors and matrices.
• Numerics: faster and more precise sums, means, standard deviations.

##6.0.30# (22 July 2017)
• Removed a bug that caused an incorrect title for a PitchTier or PointProcess window.
• Removed a bug that caused Praat to crash when doing a linear regression on a Table with no rows.
• Scripting: `object[]`, @@Scripting 5.7. Vectors and matrices|vectors@.
• Graphics: better text drawing details.
• Linux: possibility to compile Praat without a GUI but with graphics file output.

##6.0.29# (24 May 2017)
• Sound window: channel muting.
• Linux: support for Chinese, Japanese, Korean, Indic, Arabic and Hebrew characters in TextGrids and elsewhere.

##6.0.28# (23 March 2017)
• Scripting: @`demoPeekInput` () for animations in combination with `sleep` ().

##6.0.27# (18 March 2017)
• TextGrid: fewer error messages in concatenation of multiple TextGrids.
• Scripting: @`sleep` () to pause Praat temporarily: useful for animations in combination with @`demoShow` ().

##6.0.26# (2 March 2017)
• Mac: more corrections in @`demoShow` () and `blankWhilePlaying`.
• PraatBarren: better error message when an attempt is made to run PraatBarren interactively.

##6.0.25# (11 February 2017)
• Mac: made @`demoShow` () and `blankWhilePlaying` compatible with MacOS 10.12 Sierra.
• Mac SoundRecorder: more sampling frequencies, on behalf of external USB microphones.

##6.0.24# (23 January 2017)
• Fixed a bug introduced in 6.0.23 by which ##Remove right boundary# would choose the wrong tier.
• TextGrid window: click to insert a phonetic symbol from an IPA chart.

##6.0.23# (12 December 2016)
• Linux: fixed a bug that caused Praat to crash when playing a sound of more than 7 channels.
• Change Gender: fixed a bug introduced in 6.0.22 by which the pitch range factor could not be 0.
• Improvements in the manual and in texts.

##6.0.22# (15 November 2016)
• Scripting: correct error messages for expressions like: `5 + “hello”`
• Command line: the --open option works correctly in the GUI if you open multiple files.

##6.0.21# (25 September 2016)
• ExperimentMFC: you can now specify font sizes and response keys for goodness judgments.
• Table: when drawing ellipses, a font size of 0 can now be used to prevent drawing the labels.
• Mac: dragging selections repaired for System 10.11.6 (but we advise to upgrade to 10.12).
• Mac: re-enabled Return key for confirming some dialog boxes.

##6.0.20# (3 September 2016)
• Can open UTF-8 text files with Byte Order Mark.
• Scripting improvement: function names can now be used as names of indexed variables.

##6.0.19# (13 June 2016)
• Mac: dragging selections repaired for System 10.11.5.

##6.0.18# (23 May 2016)
• Windows: better dotted lines.
• TextGrid window: again better automatic alignment.

##6.0.17# (21 April 2016)
• TextGrid window: better automatic alignment.

##6.0.16# (5 April 2016)
• Scripting: “hashes”: variables can now be indexed with strings rather than only with numbers.
• TextGrid window: fewer out-of-order messages in automatic alignment.

##6.0.15# (21 March 2016)
• TextGrid window: removed a bug whereby Praat could do automatic alignment only on sounds sampled at 44.1 kHz.
• TextGrid window: improved the location of the final boundary in automatic alignment.
• Table window: added a preference for the interpretation of graphical style symbols (`%#^_@`).

##6.0.14# (11 February 2016)
• Linux: corrected a bug by which the Tab short-cut for playing a sound would not work if the NumLock key was on.
• Mac 64-bit: corrected a bug that could cause Praat to crash if the Tab key was pressed in a dialog window.

##6.0.13# (31 January 2016)
• Windows: corrected a bug that could cause Praat to crash when closing a script window.
• Mac 64-bit: progress bars with movies.

##6.0.12# (24 January 2016)
• Windows 32-bit: corrected a bug that could cause Praat to crash when closing the Sound window after popping up dialog boxes.
• Mac 64-bit: better `demoShow` ().
• Mac 64-bit: working Matrix movies and articulatory synthesizer movies.

##6.0.11# (18 January 2016)
• Corrected a bug that could cause Praat to crash when drawing a Spectrum with undefined values.
• Mac 64-bit: corrected a bug by which some text in the Sound window would sometimes not be drawn.
• Mac 64-bit demo window: improved the working of `demoShow` ().

##6.0.10# (8 January 2016)
• Corrected a bug that would cause Praat to crash when starting an ExperimentMFC window.
• Mac 64-bit: corrected a bug that would cause black selections in the Sound window.

##6.0.09# (1 January 2016)
• Windows 64-bit: corrected a bug that could cause Praat to crash when closing the Sound window after popping up dialog boxes.

##6.0.08# (5 December 2015)
• Windows command line: the `--a` option can be used when redirecting the output to pipes and files.
• Linux command line: the `--run` option is not needed in the Terminal when redirecting output
  (unless you redirect standard output, standard input %and standard error).

##6.0.07# (30 November 2015)
• Mac 64-bit: repaired dragging selections (which got broken in the El Capitan fix in 6.0.06).

##6.0.06# (29 November 2015)
• Mac 64-bit: correct early Info window updates and @`demoShow` () in El Capitan.
• Manual updates.
• Corrected some small memory leaks.

##6.0.05# (8 November 2015)
• A manual page that describes @@TextGrid file formats@.
• Corrected a bug that prevented some KlattGrid tiers from being edited.
• 32-bit Linux: correct use of PulseAudio (note: 6.0.04 on Debian already had this correct).

##6.0.04# (1 November 2015)
• Corrected a bug that caused an incorrect number in FFNet files;
  unreadable FFNet files can be sent to the Praat authors for repair.

##6.0.03# (31 October 2015)
• Corrected a bug that caused Praat to crash when the Soundrecorder window was closed.
• Scripting: when calling Praat from the command line, added the `--run` option to force script execution.

##6.0.02# (30 October 2015)
• Corrected the redrawing of analyses in the Sound window after a change in the sound.

##6.0.01# (29 October 2015)
• Windows: fixed a bug that caused Praat to crash at start-up in some Windows versions.

################################################################################
"What was new in 6.0?"
© Paul Boersma 2015

##6.0# (28 October 2015)
• Linux: use PulseAudio as the server for playing sound.
• Windows: removed `Praatcon.exe`, because Praat.exe can now be used from the command line.
• Praat can now open files from the command line on all platforms.
• The API change that requires raising the major Praat version to 6:
  no longer any need for `Praatcon.exe`, because Praat itself can now run a script from the command line on all platforms.
• See @@Scripting 6.9. Calling from the command line@ for all command line options.
• SpeechSynthesizer: corrected a potentially crashing bug.
• Mac Retina graphics: made function drawing (e.g. in the TextGrid window) twice as fast.
• LongSound: corrected playing of more than 2 channels.

##5.4.22# (8 October 2015)
• Sound files: added support for MP3 files with ID3 version 2 headers.
• @@Table: Line graph where...@: removed a bug that caused nothing to be drawn if the number of columns was larger than the number of rows and
the column index was larger than the number of rows.
• Mac: corrected horizontal spacing in graphical text on El Capitán.
• Mac: corrected a bug that would cause Praat to crash if a PNG file could not be saved (from a script).
• Windows: enabled PNG saving from Praatcon.

##5.4.21# (29 September 2015)
• Corrected a bug introduced in 5.4.20 that could cause the Sound window to crash.
• Repaired a very old hang with pulses in the Sound window.

##5.4.20# (26 September 2015)
• Corrected the graphical representation of numbers with exponents (wrong since 5.4.10).
• Windows: prevented flashing console window in @`runScript` ().

##5.4.19# (16 September 2015)
• LongSound: corrected saving of 8-bit files as 16-bit.

##5.4.18# (7 September 2015)
• Regular expressions: if the pattern is illegal, you'll now get an error message instead of an empty string.
• LongSound: removed a bug introduced in 1999 that could play long stereo sounds much too fast if they had unusual sampling frequencies.
  This bug has become more noticeable on the Mac since January 2015.
• Guard better against sound files that contain 0 samples.
• Scripting: faster look-up of variables.

##5.4.17# (20 August 2015)
• Scripting: removed a bug that could cause Praat to crash when calling `writeFile` () with an empty text.

##5.4.16# (16 August 2015)
• Mac 64-bit: removed a bug by which graphical text passed over the right edge in TextGrid intervals.
• Mac 64-bit: removed a bug that allowed the general #Zoom and #Close commands to be accessible from a script.

##5.4.15# (1 August 2015)
• Introduced several command line options that make it easier to call Praat from a web server.
• 64-bit Mac: removed a bug introduced in 5.4.11 that caused incorrect spacing of vertical text in picture files.

##5.4.14# (24 July 2015)
• Windows and 32-bit Mac: removed a bug introduced in 5.4.12 that could cause weird behavior (even crashing) when
  pressing the Tab key in the script window or in the manual.

##5.4.13# (22 July 2015)
• The Demo window can now save itself to a PNG or PDF file.
• Windows: removed a bug introduced in 5.4.10 whereby Praatcon would write garbled text to the console.

##5.4.12# (10 July 2015)
• Windows: removed a bug introduced in 5.4.10 whereby SpeechSynthesizer would only pronounce the first letter of the text.

##5.4.11# (8 July 2015)
• Windows: removed a bug introduced in 5.4.10 whereby the file selection window could try to open a file with a garbled name.
• Removed a bug whereby the PointProcess window would crash when showing a perfectly silent sound.

##5.4.10# (27 June 2015)
• Removed a bug introduced in version 4.5.09 (January 2007) that caused incorrect
  upsampling of stereo sounds when the upsampling factor was exactly 2. This bug has caused
  incorrect playing of stereo sounds with a sampling frequency of 22050 Hz on the Mac since January 2015.
• Removed a bug introduced in 2011 that could cause Praat to crash when you scrolled the LongSound window.
• @@TextGrid: Count intervals where...@ and @@TextGrid: Count points where...@.

##5.4.09# (1 June 2015)
• Linux: the tab key can be used to play sounds on more computers.
• Windows: TextGrid files with non-BMP characters are now read correctly.
• Windows: files with names that contain non-BMP characters are now saved correctly.
• Updated manual.

##5.4.08# (24 March 2015)
• Sound window: removed a bug that caused a crash in ##Editor info#.

##5.4.07# (22 March 2015)
• TextGrid window: ##Add interval# now does the same thing as ##Add boundary# if a time stretch is selected.
• Linux: better redrawing in TextGrid window.

##5.4.06# (21 February 2015)
• Windows: repaired a bug that could cause Praat to crash if a metafile resolution was 200 dpi.

##5.4.05# (13 February 2015)
• Better support for big integer numbers on Windows XP.
• Sound window: guarded against empty view ranges for the intensity curve.

##5.4.04# (28 December 2014)
• Windows audio playback: if the sound has more channels than the audio hardware, distribute them evenly.

##5.4.03# (18 December 2014)
• TextGrid reading: Praat now corrects some incomplete TextGrid files created by others.
• Better support for text files larger than 2 GB.

##5.4.02# (26 November 2014)
• Mac: repaired a bug by which quote characters typed into the script window could become curly instead of straight.

##5.4.01# (9 November 2014)
• @MelSpectrogram, @BarkSpectrogram.
• Linux: removed a bug that could cause too many flashes when scrolling a Sound window.
• Mac: repaired a bug that could cause a crash in the @VowelEditor window.

################################################################################
"What was new in 5.4?"
© Paul Boersma 2014

##5.4# (4 October 2014)

##5.3.87# (3 October 2014)
• Windows scripting: prevented incorrect handling of relative paths after the use of @`chooseReadFile$`.
• Windows: repaired a bug that could cause Praat to crash if a metafile resolution was 180 or 1200 dpi.

##5.3.86# (28 September 2014)
• Linux audio: reverted to old version of PortAudio because of race problems in the Alsa–PulseAudio cooperation.

##5.3.85# (19 September 2014)
• Mac audio: circumvented a bug in PortAudio by which playback of sounds with sampling frequencies
  below 44100 Hz could be distorted on some USB headsets.

##5.3.84# (26 August 2014)
• Manipulation: repaired a bug that could cause Praat to crash when playing a manipulation of a Sound created by the @SpeechSynthesizer.

##5.3.83# (16 August 2014)
• TextGrid window: repaired a bug with automatic alignment that could cause an analysis tier to go out of order.
• Linux audio: created a second workaround that reduces even more the chances of a freeze that is due to a potential deadlock
  in the collaboration between Alsa and PulseAudio that can occur when the playback of a sound is cancelled.
• Smoother communication with Phon.
• Windows: repaired a memory leak when saving PNG files.

##5.3.82# (26 July 2014)
• Linux and Mac audio playback: if the sound has more channels than the audio hardware, distribute them evenly.
• Pause forms: more consistent appearance of the Revert button.
• Scripting: @`pauseScript` () function.

##5.3.81# (2 July 2014)
• EEG: can work with status %numbers instead of only with status %bits.
• Windows: repaired a bug that could cause Praat to crash if there was a 96-dpi printer.

##5.3.80# (29 June 2014)
• Praat preferences: choice between Chinese and Japanese style for Han characters.

##5.3.79# (21 June 2014)
• Can now play sounds over more than two channels.
• Asynchronous play in scripts (see @@Demo window@).
• EEG: blue-to-red colour scale for scalp distributions.

##5.3.78# (12 June 2014)
• Multithreading can now speed up pitch analysis by a factor of 4 or so,
  depending on the number of cores in your processor.
• Linux: can now open and save Photo objects (from PNG files)
  and use @@Insert picture from file...@.
• Open WAV files that are in the “extensible” format (previously “unsupported format -2”).
• Windows: support for dropping more than one file on the Praat icon.
• Scripting: can now use the #editor command with an object’s ID instead of only with its name.
• Windows: removed a bug that sometimes disallowed saving more than one JPEG file.
• Linux audio: created a workaround that reduces the chances of a freeze that is due to a potential deadlock
  in the collaboration between Alsa and PulseAudio that can occur when the playback of a sound is cancelled.

##5.3.77# (18 May 2014)
• EEG: more facilities for EDF+ files.

##5.3.76# (8 May 2014)
• One can determine the size of “speckles” (filled circles) with ##Speckle size...# in the #Pen menu.
  Speckles are used in drawing Formant, PitchTier, and several other kinds of objects.

##5.3.75# (30 April 2014)
• Linux Matrix graphics bug fix: corrected working of @@Matrix: Draw cells...@.
• Scripting bug fix: ability to use x and y as indexed variables.
• PowerCepstrogram bug fix: made old version of Paint command available again for scripts.

##5.3.74# (24 April 2014)
• EEG: more interpretation of triggers in EDF+ files.

##5.3.73# (21 April 2014)
• EEG: understand more EGI/NetStation files.

##5.3.72# (17 April 2014)
• Windows: repaired a bug that caused two black edges in PNG files.
• Windows: repaired a bug that could cause Praat to crash if a metafile resolution was 360 dpi.
• Linux: repaired a bug that caused Praat to crash when cutting or pasting a sound in the Sound window.

##5.3.71# (9 April 2014)
• Windows: brought more unity in the style of Chinese characters.

##5.3.70# (2 April 2014)
• Added some query commands for DurationTier objects.
• Repaired a bug that caused Praat not to run as a console app.

##5.3.69# (28 March 2014)
• Picture window: can save to 300-dpi and 600-dpi PNG files.
• Graphics: sub-pixel precision line drawing on Mac and Linux.
• Repaired a bug that could show spurious buttons in the Objects window if a plug-in created objects.

##5.3.68# (20 March 2014)
• Mac: corrected a bug introduced in 5.3.67 that could cause crashes when drawing a spectrogram.
• Mac and Linux: @@Create Strings as file list...@ handles broken symbolic links more leniently.

##5.3.67# (19 March 2014)
• Corrected a bug that would create strange PNG files if the selection did not touch the upper left corner of the Picture window.
• Mac: can save the Picture window to PNG file.
• EEG: understand trigger letters in BDF/EDF files.

##5.3.66# (9 March 2014)
• Windows and Linux: can save the Picture window to PNG file.
• Windows: opening, modifying and saving PNG, TIFF or JPEG files (the Photo object, as on the Mac).

##5.3.65# (27 February 2014)
• Scripting language: removed some bugs from @`runScript`.
• Linux: can save the Picture window to PDF file.

##5.3.64# (12 February 2014)
• Scripting language: @`writeInfo`, @`procedure`, @`exitScript`, @`runScript`: all with colons.
• 64-bit Mac graphics: better highlighting and unhighlighting of selection.
• 64-bit Mac graphics: full screen.

##5.3.63# (24 January 2014)
• Scripting language: easier menu command invocation using the colon “`:`”.
• 64-bit Mac graphics: better handling of any absence of Doulos SIL or Charis SIL.
• Windows scripting: can now use “`~`” in file names to refer to the home directory, as on Mac and Linux.

##5.3.62# (2 January 2014)
• 64-bit Mac: removed a bug introduced in 5.3.61 that could cause text containing “ff” to become invisible.

##5.3.61# (1 January 2014)
• EEG: understand status registers that contain text.
• KlattGrid: removed a bug introduced in May 2009 that could make Praat crash after editing an oral formant grid.

##5.3.60# (8 December 2013)
• Mac 64-bit: implemented swiping (to scroll with the trackpad) and pinching (to zoom with the trackpad).
• Scripting: @`backslashTrigraphsToUnicode` () and @`unicodeToBackslashTrigraphs` ().

##5.3.59# (20 November 2013)
• EEG: faster reading of BDF and EDF files.
• Batch scripting: made @`appendInfo` () write to the console in the same way as #`print`.
• Removed a bug introduced in 5.3.57 whereby some Praat text files could not be read.

##5.3.58# (17 November 2013)
• EEG: support for 16-bit (next to 24-bit) BDF files and for 16-bit (next to 8-bit) statuses.
• Mac: 64-bit beta version.

##5.3.57# (27 October 2013)
• Mac: opening, modifying and saving image files (the Photo object).
• Mac 64-bit: some small improvements in the user interface.

##5.3.56# (15 September 2013)
• Mac: 64-bit alpha version.
• Linux: improved selecting in the Picture window.

##5.3.55# (2 September 2013)
• Corrected a bug introduced in 5.3.54 by which you couldn't select a file for saving.

##5.3.54# (1 September 2013)
• Sound window: removed a bug introduced in 5.3.42 by which you couldn't ask for an odd number of poles in Formant Settings
  (by e.g. specifying “5.5” for the number of formants).
• Linux: improved dragging of selections in the Picture window and the Sound window.

##5.3.53# (9 July 2013)
• Table: more drawing commands.

##5.3.52# (12 June 2013)
• Scripting: editor windows understand #`do` and #`do$`.

##5.3.51# (30 May 2013)
• Sound window: ##Extract selected sound for overlap...#.

##5.3.49# (13 May 2013)
• TextGrid window: alignment of the sound and the annotation in an interval, via Espeak.
• Scripting: repaired a bug introduced in 5.3.32 that could cause very slow running of scripts.

##5.3.48# (1 May 2013)
• Scripting: variable-substitution-free object selection and file writing.
• Scripting: @`selectObject` and @`removeObject` can select or remove multiple objects at a time.

##5.3.47# (23 April 2013)
• OTGrammar: included Giorgio Magri's (2012) update rule (weighted all up, high down).

##5.3.46# (21 April 2013)
• Scripting: variable-substitution-free procedure calls.
• Linux: made the Save menu compatible with Ubuntu 12.04.

##5.3.45# (15 April 2013)
• More parts of the manual reflect variable-substitution-free scripting.

##5.3.44# (7 April 2013)
• @@Create Sound as pure tone...@.
• First steps towards variable-substitution-free scripting: the #`do`, #`do$`, @`writeInfo` and @`appendInfo` functions.

##5.3.43# (27 March 2013)
• Read and write stereo Kay sound files.
• Phonetic symbols \\ts, \\tS and \\ap.
• Network: ##Normalize weights...#, ##List weights...#, ##Weights down to Table...#.

##5.3.42# (2 March 2013)
• Repaired some minor bugs regarding synchronization between multiple windows.

##5.3.41# (9 February 2013)
• Linux: repaired a bug that could cause Praat to crash when closing the Info window or a script window.

##5.3.40# (2 February 2013)
• Windows: better handling (in the LongSound window) of sounds that are too long to play.
• Mac: corrected a bug that caused Praat to crash when closing the Strings window.
• Linux: made audio output work again on 32-bit Linux.

##5.3.39# (6 January 2013)
• @@Formant & Spectrogram: To IntensityTier...@.

##5.3.38# (4 January 2013)
• ExperimentMFC: repaired a bug introduced in 5.3.36 that caused sound to fail to play completely if the screen did not blank.

##5.3.37# (2 January 2013)
• ExperimentMFC: after screen blanking, reaction times count from when the response buttons appear.

##5.3.36# (1 January 2013)
• ExperimentMFC: made it possible to blank the screen while the sound is playing.

##5.3.35# (8 December 2012)
• SpeechSynthesizer: made it possible to have more than one at a time.
• Linux: corrected a bug that caused incorrect alignment of the buttons in the Objects window.

##5.3.34# (21 November 2012)
• Windows: corrected a bug that caused incorrect alignment of the buttons in the Objects window.
• The new @@Sound: Combine to stereo@ can now work with Sounds of different durations, as the old could.
• Corrected a bug that caused Praat to crash when creating a SpeechSynthesizer.

##5.3.33# (20 November 2012)
• ##Sound: Combine to stereo# can now combine any number of Sounds into a new Sound whose number of channels
  is the sum of the numbers of channels of the original Sounds.
• ERP: Down to Sound.

##5.3.32# (17 October 2012)
• Sound window: corrected the working of the ##by window# scaling option.

##5.3.31# (10 October 2012)
• ERP: Down to Table...
• Linux: corrected a bug that could cause Praat to crash after closing a file selector window for saving.

##5.3.30# (6 October 2012)
• Circumvented a rare Windows bug that could cause Praat to start to write 1.5 as 1,5 in some countries
  after opening a strange directory.
• Windows: corrected a bug introduced in 5.3.24 that could cause Praat to crash when quitting.
• Windows: corrected a bug introduced in 5.3.24 that could cause the Objects window to look weird after resizing.

##5.3.29# (30 September 2012)
• @EEG: Draw scalp..., including gray legend.
• Made the new Sound scaling options available for LongSounds.

##5.3.28# (28 September 2012)
• Corrected a bug introduced in 5.3.24 that made Praat crash on OSX 10.5.

##5.3.27# (27 September 2012)
• Corrected a couple of small bugs.

##5.3.26# (26 September 2012)
• Corrected a bug introduced in 5.3.24 that prevented Praat from running in batch mode.

##5.3.25# (26 September 2012)
• EEG: view of scalp distribution takes scaling options into account.
• Linux: swipable Sound window and manual.
• Linux: corrected a bug introduced in 5.3.24 whereby TextGrids could not be edited.

##5.3.24# (24 September 2012)
• More kinds of vertical scaling in e.g. Sound and EEG windows.
• Rewritten user interface (may contain some bugs).

##5.3.23# (7 August 2012)
• Mac: removed a bug introduced in September 2011 that could cause incorrect behaviour of regular expressions since July 2012.

##5.3.22# (21 July 2012)
• Linux: removed a bug introduced in 5.3.21 that could cause Praat to crash when opening files.
• Neural networks: correct writing and reading of learning settings.

##5.3.21# (10 July 2012)
• Linux: better folder choice in file selector window for opening files.
• Repaired a bug that caused Praat to crash when opening a ManPages file on some platforms.

##5.3.20# (5 July 2012)
• @EEG: ##Replace TextGrid#.

##5.3.19# (24 June 2012)

##5.3.18# (15 June 2012)
• Corrected a bug in @@Sound: Change gender...@ that caused a part of the sound not to be changed.

##5.3.17# (12 June 2012)
• @EEG window: extent autoscaling by window.
• ERPTier: Remove events between...

##5.3.16# (23 May 2012)

##5.3.15# (10 May 2012)
• Improvements in clipboards, PDF and EPS files.

##5.3.14# (28 April 2012)
• Linux: Tab shortcut for playing the selection.
• EPS files: higher minimum image resolution for spectrograms (300 instead of 106 dpi).

##5.3.13# (11 April 2012)
• @EEG: Extract part...

##5.3.12# (5 April 2012)
• Praat picture file: allow larger function drawings (up to 10^9 instead of 10^6 points).
• Linux: better audio compatibility with Ubuntu 11.10 (support for unusual sampling frequencies when playing LongSounds).

##5.3.11# (27 March 2012)
• @EEG: a selected ERPTier can extract events on the basis of a column of a selected Table.

##5.3.10# (12 March 2012)
• @EEG: Concatenate.

##5.3.09# (10 March 2012)
• Better text-to-speech.

##5.3.08# (5 March 2012)
• Removed a bug introduced in 5.3.07 that could cause Praat to crash when viewing a LongSound.

##5.3.07# (4 March 2012)
• Praat can now save Sounds as 24-bit and 32-bit WAV files.

##5.3.06# (28 February 2012)

##5.3.05# (19 February 2012)
• SpeechSynthesizer (#New menu \\-> #Sound): text-to-speech.
• @EEG: better scalp distribution drawing for both 32-channel and 64-channel recordings.

##5.3.04# (12 January 2012)
• EEG: draw scalp distribution.
• Linux: better audio compatibility with Ubuntu 11.10 (support for unusual sampling frequencies when playing Sounds).

##5.3.03# (21 November 2011)
• EEG: filtering, editing, more viewing.

##5.3.02# (7 November 2011)
• Corrected a bug introduced in 5.3.01 that could cause the PitchTier or PointProcess window to crash.
• Corrected a bug that could cause the Info window to freeze after an error message in a script window.

##5.3.01# (1 November 2011)
• Macintosh and Windows: better window positioning if the Dock or Task Bar is on the left or right.
• IPA symbol: you can now use \\bs.f for the half-length sign (\\.f).
• EEG window.

################################################################################
"What was new in 5.3?"
© Paul Boersma 2011

##5.3# (15 October 2011)

##5.2.46# (7 October 2011)
• Corrected the same very old bug as in 5.2.44, but now also for opening and saving files.
• Many better messages.

##5.2.45# (29 September 2011)
• Spectrum window: ##Move cursor to nearest peak#.
• @@Table: Save as comma-separated file...@.
• Windows: you can now change the font size of the script window.
• Windows scripting: the Info window now updates while you are writing to it.
• Windows: error messages now stay at the front so that you never have to click away
  an invisible message window anymore.

##5.2.44# (23 September 2011)
• Corrected a very old bug in the history mechanism (the button title wouldn't always show up).

##5.2.43# (21 September 2011)
• Linux: you can now change the font size of the script window.
• Corrected a bug that could cause Praat to crash when opening an already open file in the script window.

##5.2.42# (18 September 2011)
• Corrected a bug introduced in 5.2.36 that prevented formulas from working on FormantGrid objects.

##5.2.41# (17 September 2011)
• Improved dashed-dotted lines in EPS files.
• Corrected a bug introduced in 5.2.36 that caused Praat to crash when running an ExperimentMFC.

##5.2.40# (11 September 2011)
• Corrected a bug in the 64-bit Windows edition that led to an incorrect location for the preferences files.

##5.2.39# (10 September 2011)
• 64-bit edition for Windows.
• Corrected a bug that caused Praat to crash instead of saying “undefined” when reporting the shimmer of a sound with fewer than 3 pulses.

##5.2.38# (6 September 2011)
• Corrected several bugs that were introduced in 5.2.36 in the Inspect window.

##5.2.37# (2 September 2011)
• Graphics: added the Chinese phonetic symbols \\id and \\ir. See @@Phonetic symbols: vowels@.
• Corrected a bug introduced in 5.2.36 that caused Praat to crash when querying formant quantiles.

##5.2.36# (30 August 2011)
• Graphics: added superscript diacritics as single symbols: \\^h (\\bs\\^ h), \\^j (\\bs\\^ j),
and many more; also \\_u (\\bs\\_ u). See @@Phonetic symbols: diacritics@.
• Praat fully converted to C++ (this may initially cause some bugs, but will be more reliable in the end).

##5.2.35# (5 August 2011)
• Corrected the menus of the Sound and TextGrid windows.

##5.2.34# (3 August 2011)
• @@Insert picture from file...@ now works on Windows (as well as on the Mac).
• Corrected a bug that could cause Praat to crash when playing a LongSound.

##5.2.33# (29 July 2011)
• Improved the reliability of sound playing and several other things.
• Improved several error messages.

##5.2.32# (22 July 2011)
• Corrected several problems with error messages.
• Corrected a bug that could cause slow execution of scripts with forms.

##5.2.31# (21 July 2011)
• Corrected a bug that caused an incorrect window size in the VowelEditor.
• Corrected a bug that caused incorrect error messages when opening a FLAC file as a LongSound.
• Sound window: corrected a bug that could cause Praat to crash when zooming in on the left or right edge.

##5.2.30# (18 July 2011)
• Corrected a bug introduced in 5.2.29 whereby the list of object actions could stay empty after an error message in a script.
• Corrected a bug in Klatt synthesis whereby the generation of a sound could be refused.

##5.2.29# (12 July 2011)
• More accurate error checking, due to Praat's conversion to C++ (last C-only version was 5.2.17).

##5.2.28# (28 June 2011)
• Corrected some Polygon bugs.

##5.2.27# (19 June 2011)
• @Polygon: ##Draw (closed)...#.
• @@PointProcess: To Sound (phonation)...@:
corrected a bug that could cause Praat to crash if pulses lay outside the time domain.

##5.2.26# (24 May 2011)
• Corrected a bug that could cause Praat to crash on some platforms when reading a Praat binary file.
• @ExperimentMFC: corrected a bug that caused Praat to crash if an experiment contained zero trials.
• Corrected a bug that caused Praat to crash when merging multiple IntervalTiers.

##5.2.25# (11 May 2011)
• OT learning: corrected a crashing bug from ##Get candidate...#.

##5.2.24# (10 May 2011)
• Ability to open WAV files that contain incorrect information about the number of samples.
• Removed an old bug that could cause Praat to fail to read a chronological TextGrid text file.

##5.2.23# (1 May 2011)
• Removed a bug introduced recently that could cause Praat to crash when working with derivatives of TableOfReal (such as Distributions).

##5.2.22# (14 April 2011)
• @ExperimentMFC: reaction times for key presses.
• Linux: more reliable start-up on German systems.

##5.2.21# (29 March 2011)
• Scripting: removed a crashing bug introduced for colour names in 5.2.20.

##5.2.20# (25 March 2011)
• Scripting: removed a large memory leak for indexed variables.
• Scripting: removed a small memory leak for colour names.
• Support for very long file paths on the Mac.

##5.2.19# (16 March 2011)
• @ExperimentMFC: corrected a bug introduced in 5.2.18 that could cause Praat to crash when extracting results from an incomplete experiment.

##5.2.18# (9 March 2011)
• @ExperimentMFC: incomplete experiments can nevertheless output their incomplete results.

##5.2.17# (2 March 2011)
• Better names for Table opening and saving commands.
• @ExperimentMFC: reaction times for mouse clicks.
• Linux/GTK: corrected triple clicks in ExperimentMFC.

##5.2.16# (20 February 2011)
• Better support for WAV files with special “chunks” in them.
• Manual: documentation of explicit formulas for @@Voice 2. Jitter|jitter measurements@.

##5.2.15# (11 February 2011)
• @@Sounds: Concatenate with overlap...@.

##5.2.14# (8 February 2011)
• Repaired crashing bug in Manipulation window introduced in 5.2.13.

##5.2.13# (7 February 2011)
• Renamed #Write commands to #Save commands.
• Scripting: allow pause forms without #Stop button (see @@Scripting 6.6. Controlling the user@).
• GTK: correct behaviour of default buttons.

##5.2.12# (28 January 2011)
• Renamed #Edit buttons to ##View & Edit#.
• Better visibility of dragged things on Linux.

##5.2.11# (18 January 2011)
• Better visibility of dragged things (sound selection, tier points, TextGrid boundaries) on Windows and Linux.

##5.2.10# (11 January 2011)
• Renamed #Read and #Write menus to #Open and #Save.
• Sound: use of ##Formula (part)...# can speed up formulas appreciably.

##5.2.09# (9 January 2011)
• Much improved scripting tutorial.
• Listening experiments can now show pictures instead of just texts (on the Mac).
• EPS files can now be many miles wide instead of just 55 inches.

##5.2.08# (1 January 2011)
• Improved manual.
• Improved memory allocation on 32-bit platforms:
less probability of crashes when you approach the 2 GB memory limit gradually,
and a new “low on memory; save your work now” warning.
• Removed IntervalTier and TextTier datatypes from Praat (old scripts that use them will continue to work).

##5.2.07# (24 December 2010)
• Support for reading and writing multi-channel sound files (i.e. above two-channel stereo).

##5.2.06# (18 December 2010)
• Picture window: a new (the fourth) line type, namely dashed-dotted.
• Support for analysing and drawing multi-channel sounds (i.e. above two-channel stereo).
• Can read some EEG files (BioSemi 24-bit BDF) as a Sound and a TextGrid object.
• Linux: compatibility with computers without English language support.
• Macintosh: support for high-plane (i.e. very uncommon) Unicode characters in file names (as already existed on Unix and Windows).

##5.2.05# (4 December 2010)
• Regular expressions: better Unicode support.
• Scripting window: command ##Convert to C string#.

##5.2.04# (27 November 2010)
• Scripting: allow directory (folder) selector windows; see @@Scripting 6.6. Controlling the user@.

##5.2.03# (19 November 2010)
• Scripting: support for string arrays.

##5.2.02# (17 November 2010)
• TextGrid window: corrected a bug that caused Praat to crash (instead of doing nothing) when you tried to add boundaries
if the selection ran from the penultimate interval to the end of the TextGrid.
• Scripting: support for arrays with multiple indexes.
• Linux: made spectrogram drawing compatible with Ubuntu 10.10.
• Linux: made sound more easily available on Ubuntu 10.10.

##5.2.01# (4 November 2010)
• Scripting: support for numeric @@Scripting 5.6. Arrays and dictionaries|arrays@.

################################################################################
"What was new in 5.2?"
© Paul Boersma 2010

##5.2# (29 October 2010)

##5.1.45# (26 October 2010)
• Linux/GTK: allow Praat to run without an X display.
• Sounds are played synchronously in scripts run from ManPages with \\bsSC.

##5.1.44# (4 October 2010)
• Linux/GTK: visibility of ExperimentMFC window.
• Linux/GTK: keyboard shortcuts.

##5.1.43# (4 August 2010)
• Scripting: support for stand-alone programs; see @@Scripting 9. Turning a script into a stand-alone program@.
• Table: allow drawing of ellipses even if irrelevant columns contain undefined data.
• Linux/GTK: correct resizing of Table window.
• Linux/GTK: prevented multiple storing of Picture window selection in scripting history.

##5.1.42# (26 July 2010)
• Scripting: allow file selector windows; see @@Scripting 6.6. Controlling the user@.
• Linux: multiple file selection.

##5.1.41# (15 July 2010)
• OTGrammar: ##Compare candidates...#
• GTK: support for the Doulos SIL and Charis SIL fonts.
• GTK: working vowel editor.
• Vowel editor: repaired memory leak.
//• GTK: Picture window: front when drawing.
//• GTK: correct scrolling in manuals.

##5.1.40# (13 July 2010)
• GTK: working Demo window (drawing, clicks, keys).
• GTK: pause forms.
• GTK: manual pages: receive a white background, for compatibility with dark-background themes.
• GTK: in settings windows, better alignment of labels to radio groups.
• GTK: rotated text.

##5.1.39# (10 July 2010)
• GTK beta version.
• Linux: made @@Sound: Record (fixed time)...@ work correctly (the sampling frequency was wrong).
• GTK: list of objects: multiple selection, working Rename button.
• GTK: running a script no longer deselects all objects at the start.
• GTK: working Buttons editor.
• GTK: correctly laid out settings windows.

##5.1.38# (2 July 2010)
• Linux: made @@Sound: Record fixed time...@ work correctly (the sampling frequency was wrong).
• Mac: repaired a bug introduced in 5.1.35 that could cause Praat to crash
  if neither Doulos SIL nor Charis SIL were installed.
• Mac: correct live scrolling in Picture window and DataEditor.

##5.1.37# (23 June 2010)
• PitchTier window and similar windows: can drag multiple points simultaneously.
• Table: %t-tests report the number of degrees of freedom.
• GTK: correct progress bar again.
• GTK: correct behaviour of Synth menu in manipulation window.

##5.1.36# (18 June 2010)
• Mac: live scrolling.
• Demo window: the Demo window can now run from the command line.
• Motif: corrected a bug introduced in 5.1.33 whereby things in the Picture window could be placed incorrectly.
• GTK: script window accepts Unicode again.

##5.1.35# (10 June 2010)
• TextGrid window: removed a very old bug that could lead to reversed intervals
  and to crashes when you inserted a boundary after using Shift-arrow and Command-arrow.
• Graphics: Praat now uses Doulos SIL instead of Charis SIL if your font setting is “Times”
  and the font is nonbold and nonitalic, because Doulos SIL matches Times New Roman better.
• kNN: made Ola Söder's k-nearest-neighbours classification compatible with Windows.

##5.1.34# (31 May 2010)
• Sound window: corrected a bug that caused Praat to crash if the analysis window was shorter than 2 samples.
• GTK: scrolling in the Info window and script window.
• GTK: script editor: implemented searching, replacing, and change dialogs.

##5.1.33# (24 May 2010)
• GTK alpha version.
• Abolished resolution independence: the Sound window now looks the same on all platforms,
  and the Demo window has the same relative font size on your screen and on the video projector.
• GTK: support for asynchronous audio output.
• GTK: sound plays once rather than three times in Sound and other windows.
• GTK: can click more than once in the manual.
• GTK: correct pink selections in Sound and other windows.
• GTK: correct dragging in TextGrid, Manipulation, and tier windows.
• GTK: a working TextGrid window.
• GTK: no automatic triple click in the manual (and many other windows).
• GTK: moving cursor while sound is playing in Sound and other windows.
• GTK: correct colours of the rectangles in Sound and other windows.
• GTK: a working Group button.
• GTK: correct font menus, font size menus, colour menus, line type menus.
• GTK: scrolling in the manual (and many other windows).
• GTK: erase old texts in manual windows.
• GTK: made Picture window come back when drawing.
• GTK: Info window is fronted automatically.
• GTK: support sendpraat.

##5.1.32# (30 April 2010)
• Scripting: command ##Expand include files#.
• Scripting: accept lines that start with non-breaking spaces
  (as may occur in scripts copied from the web manual).
• Sound files: accept MP3 files with extension written in capitals.
• Linux audio recording: corrected input choice (microphone, line).

##5.1.31# (4 April 2010)
• @@Sounds: Convolve...@, @@Sounds: Cross-correlate...@, @@Sound: Autocorrelate...@, with full documentation.
• More query commands for IntensityTier.

##5.1.30# (25 March 2010)
• Scripting: `createFolder` () can now work with absolute paths.
• PointProcess: made it impossible to add a point where there is already a point.

##5.1.29# (11 March 2010)
• Full support for unicode values above 0xFFFF on Macintosh.

##5.1.28# (10 March 2010)
• TextGrid window: removed a recently introduced bug that could cause Praat to crash when inserting a boundary.

##5.1.27# (7 March 2010)
• Table: Wilcoxon rank sum test.
• Logistic regression: corrected a bug by which a boundary would sometimes not be drawn.

##5.1.26# (25 February 2010)
• Experimental GTK version with the help of Franz Brauße.
• Corrected a bug that could cause Praat to crash if sound playing failed twice.

##5.1.25# (20 January 2010)
• Script window: the new command ##Reopen from disk# allows you to edit the script with an external editor.
• Script window: removed a bug that could cause Praat to crash
  if you did ##Find again# before having done any #Find.

##5.1.24# (15 January 2010)
• Formulas run 10 to 20 percent faster.
• Macintosh: support for forward delete key and Command-\\` .

##5.1.23# (1 January 2010)
• Allowed multiple files to be selected with e.g. @@Read from file...@.
• Demo window: guarded against handling the Demo window from two scripts at a time.

##5.1.22# (15 December 2009)
• Picture window: millions of @@colour|colours@ instead of just twelve.
• Sound window: ##Move frequency cursor to...#

##5.1.21# (30 November 2009)
• @@Sound: Draw where...@
• @@Matrix: Draw contours...@ and @@LogisticRegression: Draw boundary...@ support reversed axes.
• Sound window: ##Move frequency cursor to...#.

##5.1.20# (26 October 2009)
• Editor windows: repaired the Synchronized Zoom And Scroll preference.

##5.1.19# (21 October 2009)
• Table: Randomize rows
• Tables: Append (vertically)
• Scripting: corrected a bug that could cause Praat to crash if the name of a field in a form contained a colon.
• Windows: corrected arc drawing.

##5.1.18# (9 October 2009)
• The @@Demo window@ is less often automatically moved to the front
  (in order to allow it to pop up other editor windows).
• @@DTW & TextGrid: To TextGrid (warp times)@: corrected a bug that could
  lead to an incorrect end time of the last interval in new IntervalTiers.

##5.1.17# (22 September 2009)
• Made more stereo movies readable.
• Editor windows now have a ##Zoom Back# button.

##5.1.16# (17 September 2009)
• Macintosh: corrected a bug that caused incorrect phonetic symbols if Charis SIL was available but SIL Doulos IPA93 was not.

##5.1.15# (30 August 2009)
• Corrected a bug in @@Sound: Change gender...@ introduced in 5.1.14.

##5.1.14# (27 August 2009)
• Windows: corrected a bug introduced in 5.1.13 that caused Praat to crash during tab navigation.
• Made @@Sound: Change gender...@ compatible with elephant calls (i.e. very low F0).

##5.1.13# (21 August 2009)
• Script window: #Find and #Replace.
• Picture window (and therefore Demo window!): @@Insert picture from file...@ (MacOS 10.4 and up).
• @@Demo window@: full screen (on the Mac).
• Scripting: faster object selection (scripts no longer slow down when there are many objects in the list).
• Scripting: @`variableExists` ().
• Macintosh: PDF clipboard (MacOS 10.4 and up).

##5.1.12# (4 August 2009)
• Macintosh: the Picture window can save to PDF file (you need MacOS 10.4 or up).
• Macintosh: corrected a bug that caused Praat to crash at start-up on MacOS 10.3.

##5.1.11# (19 July 2009)

##5.1.10# (8 July 2009)
• Corrected a bug that could cause Praat to crash if the Demo window was closed after an #execute.
• OTGrammar & PairDistribution: added ##Get minimum number correct...#.

##5.1.09# (28 June 2009)
• Made East-European Roman characters available in EPS files.

##5.1.08# (21 June 2009)
• Removed a bug introduced in 5.1.07 that could cause strange pictures in manual.
• Macintosh: execute @sendpraat messages immediately instead of waiting for the user to click the jumping Praat icon.

##5.1.07# (12 May 2009)
• Demo window: navigation by arrow keys also on Windows.
• Demo window: no longer crashes on Linux.

##5.1.06# (11 May 2009)
• Demo window.

##5.1.05# (7 May 2009)
• KlattGrid update.

##5.1.04# (4 April 2009)
• Corrected a bug that could cause a missing text character in EPS files produced by a version of Praat running in batch.
• Corrected a bug that could cause high values in a Matrix computed from a Pitch.

##5.1.03# (21 March 2009)
//• OT learning: the ##Random up, highest down# update rule.
• ExperimentMFC: corrected a bug introduced in 5.0.36 that caused Praat to crash if a sound file did not exist.
• Articulatory synthesis: corrected a bug that could cause Praat to crash when copying Artwords.
• Macintosh: corrected a bug that could cause poor text alignment in picture.

##5.1.02# (9 March 2009)
• Allow pause forms without fields.
• The value “undefined” is disallowed from all fields in command windows
  except @@Table: Set numeric value...@ and @@TableOfReal: Set value...@.
• @@TextGrid: List...@ and @@TextGrid: Down to Table...@.
• OT learning: Giorgio Magri’s ##Weighted all up, highest down# update rule.

##5.1.01# (26 February 2009)
• Corrected several bugs in Klatt synthesis.

################################################################################
"What was new in 5.1?"
© Paul Boersma 2009-01-31

##5.1# (31 January 2009)
• Editors for Klatt synthesis.
• Corrected many bugs.

##5.0.47# (21 January 2009)
• Extensive pause windows: @@Scripting 6.6. Controlling the user@.

##5.0.46# (7 January 2009)
• More Klatt synthesizer.
• First pause form support.
• Renewed CategoriesEditor.
• Repaired several memory leaks.

##5.0.45# (29 December 2008)
• Bug fixes in Klatt synthesizer.

##5.0.44# (24 December 2008)
• David's Klatt synthesizer: @KlattGrid.

##5.0.43# (9 December 2008)
• Scripting tutorial: local variables in procedures.

##5.0.42# (26 November 2008)
• Removed a bug that could cause Praat to crash when drawing pictures in the manual window.
• Removed a bug that could cause Praat to crash when drawing multi-line text.

##5.0.41# (23 November 2008)
• ExperimentMFC: allow multiple lines in all texts.
• Regular expressions: removed a bug that could cause Praat to hang when using “`.*`”.
• Table: Draw ellipses: removed a bug that could cause Praat to crash if some cells were undefined.

##5.0.40# (10 November 2008)
• Improved reading and writing of text files (faster, bigger).

##5.0.39# (1 November 2008)
• praatcon -a (for sending ANSI encoding when redirected)

##5.0.38# (28 October 2008)
• @FormantGrid: To Formant...

##5.0.36# (20 October 2008)
• @ExperimentMFC: accepts nonstandard sound files.

##5.0.35# (5 October 2008)
• ##Scale times by...# and ##Scale times to...# (Modify menu).

##5.0.34# (22 September 2008)
• ##Shift times by...# and ##Shift times to...# (Modify menu).
• Sound: @@Combine to stereo@ works even if the two mono sounds have different durations or time domains.

##5.0.33# (9 September 2008)
• Windows: prevented warning messages about “Widget type”.

##5.0.32# (12 August 2008)
• Contributed by Ola Söder: kNN classifiers and k-means clustering.
• Made UTF-16-encoded chronological TextGrid files readable.

##5.0.31# (6 August 2008)
• Macintosh: corrected a bug introduced in 5.0.30 that caused Praat to crash when you pressed the Tab key
in a window without text fields.

##5.0.30# (22 July 2008)
• Macintosh and Windows: tab navigation.

##5.0.29# (8 July 2008)
• OTMulti: can record history.
• Picture window: corrected text in Praat picture files.

##5.0.28# (3 July 2008)
• Windows: audio output uses DirectX (next to Multi-Media Extensions).

##5.0.27# (28 June 2008)
• @@Phonetic symbols@: breve (a\\N^).
• Annotation: improved some SpellingChecker commands.
• Table: can now set string values that contain spaces.

##5.0.26# (15 June 2008)
• Windows: sound recording no longer makes your laptop's fan spin.
• Windows: no longer any 64 MB limit on recorded sounds.
• Linux: audio input and output uses Alsa (next to OSS).

##5.0.25# (31 May 2008)
• OT learning: added decision strategy ExponentialMaximumEntropy.

##5.0.24# (14 May 2008)
• Linux: corrected a bug at start-up.

##5.0.23# (9 May 2008)
• Corrected a bug that could cause Praat to crash when you edited an Artword that you had read from a file.

##5.0.22# (26 April 2008)
• Editing formant contours: @FormantGrid.

##5.0.21# (22 April 2008)
• Annotating with Cyrillic, Arabic, Chinese, Korean characters, and many more (on Macintosh and Windows).

##5.0.20# (8 April 2008)
• @ExperimentMFC: prevented the OK key from working if no response (and goodness choice) had been made.
• OT learning: sped up learning from partial outputs by a factor of five or more.

##5.0.19# (4 April 2008)
• TextGrid window: corrected a bug introduced in 5.0.17 that could cause Praat to crash when handling point tiers.

##5.0.18# (31 March 2008)
• Manipulation window: corrected a bug introduced in 5.0.17 that caused Praat to crash when adding the first duration point.
• Sound: added ##Extract all channels#.
• OT learning: added @@OTGrammar & PairDistribution: Find positive weights...@.
• ExperimentMFC: corrected a bug that caused Praat to crash when the second of multiple experiments
  referred to non-existing sound files.

##5.0.17# (29 March 2008)
• Sped up vowel editor by a large factor.
• OT learning: corrected Exponential HG update rule from OT-GLA to HG-GLA.
• OT learning: shift Exponential HG average constraint weight to zero after every learning step.

##5.0.16# (25 March 2008)
• Macintosh: returned to old compiler because of incompatibility with MacOS X 10.4.

##5.0.15# (21 March 2008)
• Windows: more reliable dropping of files on the Praat icon when Praat is already running.

##5.0.14# (20 March 2008)
• David's vowel editor (New \\-> Sound).
• Formulas: corrected scaling of sinc function.

##5.0.13# (18 March 2008)
• Corrected drawing of histograms.
• TextGrid window: selected part of the TextGrid can be extracted and saved.
• TextGrid: more complete conversion between backslash trigraphs and Unicode.
• Windows: more reliable dropping of files on the Praat icon when Praat is not running yet.
• Formulas: sinc function.

##5.0.12# (12 March 2008)
• Bigger ligature symbol (k\\lip).

##5.0.11# (7 March 2008)
• Corrected saving of new binary Manipulation files (you can send any unreadable Manipulation files to Paul Boersma for correction).

##5.0.10# (27 February 2008)
• Added the characters \\d- and \\D-.
• Windows: made ##praatcon.exe# compatible with Unicode command lines.

##5.0.09# (16 February 2008)
• Windows: corrected a bug by which Praat would not open files that were dragged on the Praat icon
if the names of these files or their directory paths contained non-ASCII characters.
• Linux: ignore the Mod2 key, because of its unpredictable assignment.

##5.0.08# (10 February 2008)
• Corrected the minus sign (\bs-m = “−”).

##5.0.07# (8 February 2008)
• Object IDs are visible in editor windows.

##5.0.06# (31 January 2008)
• Corrected a bug that caused ##Find again# in the TextGrid window not to work.
• Macintosh: made Praat work correctly on 10.3 computers with missing fonts.

##5.0.05# (19 January 2008)
• All Matrix, Sound, Spectrogram, Pitch, Ltas, and Spectrum objects (and more) are now in 52-bit relative precision (instead of the earlier 23 bits).
• Corrected a bug that could lead to “Unknown opcode (0)” messages when drawing large sounds (more than 16 million samples) in the Picture window.
• Macintosh: solved around a bug in the C library that could cause incorrect representation of non-ASCII characters (in the OTGrammar window).

##5.0.04# (12 January 2008)
• Windows: corrected a bug introduced in 5.0.03 that caused Praat to crash if you pressed a key in the Sound window.
• Macintosh: some cosmetic corrections.

##5.0.03# (9 January 2008)
• Scripting: guard against opening the same file more than once.
• Table: possibility of regarding a column as a distribution (#Generate menu).
• Macintosh: corrected line colours.

##5.0.02# (27 December 2007)
• TextGrid window: corrected the drawing of numbers to the right of the tiers.
• Corrected a bug that caused Praat to crash when doing ##SpectrumTier: List#.

##5.0.01# (18 December 2007)
• Corrected a bug that could cause Praat to crash when redrawing the sound or TextGrid window.

################################################################################
"What was new in 5.0?"
© Paul Boersma 2007-12-10

##5.0# (10 December 2007)
• Corrected many bugs.
• Display font sizes in points rather than pixels.

##4.6.41# (9 December 2007)
• Windows: corrected a bug that could cause listening experiments not to run when the directory path included non-ASCII characters;
  the same bug could (under comparable circumstances) cause scripted menu commands not to work.
• Corrected a bug that could cause null bytes in data files when the text output encoding preference was ##try ISO Latin-1, then UTF-16#.

##4.6.40# (3 December 2007)
• Corrected some minor bugs.

##4.6.39# (1 December 2007)
• Manual: corrected a bug that could cause Praat to crash when viewing certain manual pages with pictures.
• Scripting: corrected a bug that could cause Praat to crash when a long string was used as an argument to a procedure.

##4.6.38# (19 November 2007)
• More extensive @@logistic regression@.

##4.6.37# (15 November 2007)
• Object list shows numbers.
• Macintosh: corrected saving of non-ASCII text settings.

##4.6.36# (2 November 2007)
• Sound and TextGrid windows: direct drawing of intensity, formants, pulses, and TextGrid.
• Regular expressions: corrected handling of newlines on Windows.
• Scripting: improved positioning of settings windows for script commands in editors on Windows.

##4.6.35# (22 October 2007)
• Windows and Linux: better positioning of form windows of editor scripts.
• Macintosh: OTMulti learning window more compatible with non-ASCII characters.

##4.6.34# (18 October 2007)
• Corrected a bug introduced in September that could cause Praat to crash when starting up
  if the user had explicitly made action commands visible or invisible in an earlier session.

##4.6.33# (16 October 2007)
• Corrected a bug introduced in September that caused Praat to crash when a PointProcess window was opened without a Sound.
• Macintosh: objects with non-ASCII names show up correctly in the list.

##4.6.32# (14 October 2007)
• Unicode support for names of objects.
• Linux: first Unicode support (in window titles).
• Windows scripting: corrected a bug that caused weird characters in Paste History.

##4.6.31# (8 October 2007)
• TextGrid window: made Save command available again for TextGrid windows without a sound.
• Corrected a bug that caused binary Collection files with objects with names with non-ASCII characters to be unreadable.

##4.6.30# (3 October 2007)
• OTMulti: added an evaluate command in the Objects window, so that paced learning becomes scriptable.
• Macintosh: worked around a feature of a system library that could cause Praat to crash when reading a Collection text file
  that contained objects with non-ASCII names.

##4.6.29# (1 October 2007)
• OT learning: leak and constraint in OTMulti.
• Support for saving Table, TableOfReal, Strings, OTGrammar and OTMulti in Unicode.

##4.6.28# (1 October 2007)
• OT learning: positive constraint satisfactions in OTMulti tableaus.
• Corrected a bug that could cause Praat to crash when reading a non-UTF-8 text file
  when the ##Text reading preference# had been set to UTF-8.

##4.6.27# (29 September 2007)
• Corrected redrawing of text with non-ASCII characters.

##4.6.26# (29 September 2007)
• Corrected reading of high UTF-8 codes.

##4.6.25# (26 September 2007)
• @ExperimentMFC: can set font size for response buttons.

##4.6.24# (24 September 2007)
• Czech, Polish, Croatian, and Hungarian characters such as \c< \uo \t< \e; \s' \l/ \c\' \o: (see @@Special symbols@).
• Some support for Hebrew characters such as \?+ \B+ \sU (see @@Special symbols@).

##4.6.23# (22 September 2007)
• Corrected a bug introduced in 4.6.13 that caused crashes in text handling (mainly on Linux).
• Info commands in editors.

##4.6.22# (17 September 2007)
• Phonetic symbols: added the nonsyllabicity diacritic (a\\nv).
• Macintosh: worked around a feature of a system library that could cause strange behaviour of forms in scripts with non-ASCII characters.

##4.6.21# (5 September 2007)
• Sound and TextGrid windows: direct drawing of selected sound to the picture window.

##4.6.20# (2 September 2007)
• Introduced direct drawing of spectrogram and pitch to the Praat picture window from Sound windows and TextGrid windows.
• Corrected a bug introduced in 4.6.13 by which Inspect did not show all data in a TextGrid.

##4.6.19# (31 August 2007)
• Macintosh: worked around a bug in a system library that caused Praat to crash (since version 4.6.13)
  when removing a boundary from a TextGrid interval in MacOS X 10.3 or earlier.

##4.6.18# (28 August 2007)
• Sound: Filter (formula): now works in the same way as the other filter commands
  (without adding an empty space at the end), and on stereo sounds.

##4.6.17# (25 August 2007)
• Windows: improved rotated text, also for copy-paste and printing.
• Windows: phonetic characters on the screen now require the Charis SIL or Doulos SIL font.
• Picture settings report (mainly for script writers).
• Corrected a bug that could cause Praat to crash when closing a manual page that had been read from a file.

##4.6.16# (22 August 2007)
• Macintosh: corrected a bug introduced in 4.6.13 that could cause Praat to crash when drawing a spectrogram in MacOS X 10.3 or earlier.

##4.6.15# (21 August 2007)
• Corrected a bug introduced in 4.6.14 that prevented the use of Helvetica in the Picture window.
• Corrected a bug in ##Read Table from table file...#, introduced in 4.6.13.

##4.6.14# (20 August 2007)
• Corrected a bug introduced in 4.6.13 that prevented any other font than Palatino in the Picture window.
• Macintosh: corrected height of subscripts and superscripts (broken in 4.6.13).

##4.6.13# (16 August 2007)
• TextGrid: corrected reading of chronological files.
• Macintosh: text looks better (and rotated text is now readable on Intel Macs).
• Macintosh: phonetic characters on the screen now require the Charis SIL or Doulos SIL font.

##4.6.12# (27 July 2007)
• OTGrammar bug fix: leak and constraint plasticity correctly written into OTGrammar text files.

##4.6.11# (25 July 2007)
• OTGrammar: introduced %%constraint plasticity% for slowing down or halting the speed with which constraints are reranked.
• OTGrammar: introduced %leak for implementing forgetful learning of correlations.
• OTGrammar: positive constraint satisfactions are drawn as “+” in tableaus.

##4.6.10# (22 July 2007)
• Improved reading of UTF-16 data files.
• Improved error messages when reading text files (line numbers are mentioned).
• @@Table: Get group mean (Student t)...@.

##4.6.09# (24 June 2007)
• Corrected a bug introduced in 4.6.07 that caused a crash when reading Collections.
• Corrected a bug introduced in 4.6.07 that caused incorrect Open buttons in Inspect.
• How come 4.6.07 introduced those bugs? Because of large changes in the Praat source code as a result of the transition to Unicode.

##4.6.08# (22 June 2007)
• Windows: worked around a “feature” of the C library that caused 3-byte line-breaks in the buttons file.
• Windows: returned to smaller font in script window.
• OT learning: corrected a bug in PositiveHG.

##4.6.07# (20 June 2007)
• Sound files: MP3 as LongSound (implemented by Erez Volk).
• Scripting: Unicode support for strings and script window (Mac and Windows only).

##4.6.06# (4 June 2007)
• Script window: corrected a bug introduced in 4.6.05 that could cause incorrect symbols in saved files.

##4.6.05# (2 June 2007)
• Sound files: reading MP3 audio files (implemented by Erez Volk).

##4.6.04# (29 May 2007)
• OT learning: added decision strategy PositiveHG.

##4.6.03# (24 May 2007)
• Spectral slices have better names.

##4.6.02# (17 May 2007)
• Sound files: saving FLAC audio files (implemented by Erez Volk).

##4.6.01# (16 May 2007)
• Removed a bug that caused downsampling (and therefore formant measurements)
  to be incorrect for stereo sounds.

################################################################################
"What was new in 4.6?"
© Paul Boersma 2007

##4.6# (12 May 2007)

##4.5.26# (8 May 2007)
• Sound files: reading FLAC audio files (implemented by Erez Volk).

##4.5.25# (7 May 2007)
• @@Table: Rows to columns...@
• @@Table: Collapse rows...@ (renamed from #Pool).
• @@Table: Formula (column range)...@
• OT learning: OTGrammar window shows harmonies.

##4.5.24# (27 April 2007)
• OT learning: added decision strategy MaximumEntropy;
  this has the same harmony determination method as Harmonic Grammar (include the additive constraint noise),
  but there is some more variability, in that every candidate gets a relative probability of exp(harmony).

##4.5.23# (26 April 2007)
• Macintosh: much smaller sizes (in kilobytes) of spectrograms for printing and clipboard;
  this improves the compatibility with other programs such as Microsoft Word for large spectrograms.

##4.5.22# (25 April 2007)
• Macintosh: improved drawing of spectrograms for printing and clipboard
  (this was crippled in 4.5.18, but now it is better than before 4.5.18).

##4.5.21# (24 April 2007)
• OT learning: corrected HarmonicGrammar (and LinearOT) learning procedure
  to the stochastic gradient ascent method applied by @@Jäger (2003)@ to MaxEnt grammars.
• Scripting: removed a bug that could make selection checking (in command windows) unreliable after a script was run.

##4.5.20# (19 April 2007)
• Scripting: allow assignments like `pitch = To Pitch... 0 75 600`.
• PitchTier Formula: guard against undefined values.

##4.5.19# (2 April 2007)
• Scripting: allow comments with “`#`” and “`;`” in forms.
• Windows audio playing: attempt at more compatibility with Vista.

##4.5.18# (30 March 2007)
• Macintosh: better image drawing (more grey values).
• More tabulation commands.
• More SpectrumTier commands.
• Picture window: keyboard shortcut for @@Erase all@.

##4.5.17# (19 March 2007)
• Picture window: can change arrow size.
• Several #List commands.
• @@Spectrum: To SpectrumTier (peaks)@.

##4.5.16# (22 February 2007)
• Sound-to-Intensity: made resistant against undefined settings.
• Windows: made Ctrl-. available as a shortcut.
• Linux: made it more likely to find the correct fonts.

##4.5.15# (12 February 2007)
• Windows XP: worked around a bug in Windows XP that could cause Praat to crash
  when the user moved the mouse pointer over a file in the Desktop in the file selector.
  The workaround is to temporarily disable file info tips when the file selector window is on the screen.

##4.5.14# (5 February 2007)
• Scripting: some new predefined string variables like `preferencesDirectory$`.

##4.5.13# (3 February 2007)
• For stereo sounds, pitch analysis is based on correlations pooled over channels
  (rather than on correlations of the channel average).
• For stereo sounds, spectrogram analysis is based on power density averaged across channels
  (rather than on the power density of the channel average).
• Scripting: removed a bug introduced in 4.5.06 that caused some variables not to be substituted.

##4.5.12# (30 January 2007)
• Made cross-correlation pitch analysis as fast as it used to be before 4.5.11.

##4.5.11# (29 January 2007)
• Sound objects can be stereo, for better playback quality
  (most analyses will work on the averaged mono signal).
• Macintosh: recording a sound now uses CoreAudio instead of SoundManager, "
  for more compatibility with modern recording devices,
  and the possibility to record with a sampling frequency of 96 kHz.
• @ManPages allow picture scripts with separate object lists.
• Linux: better scroll bars in object list for Lesstif (Debian).
• Linux: made @@Create Strings as file list...@ work on Reiser.
• @sendpraat scripts correctly wait until sounds have played.

##4.5.08# (20 December 2006)
• ExperimentMFC: can use stereo sounds.

##4.5.07# (16 December 2006)
• Macintosh: playing a sound now uses CoreAudio instead of SoundManager.
• Phonetic symbols: \'1primary stress and \'1secon\'2dary stress.

##4.5.06# (13 December 2006)
• Support for 32-bit floating-point WAV files.
• Scripting: removed several kinds of restrictions on string length.
• SSCP: Draw confidence ellipse: corrected a bug that would sometimes not draw the ellipse when %N was very large.

##4.5.05# (5 December 2006)
• Macintosh scripting: European symbols such as ö and é and ç are now allowed in file names in scripts
  and in MacRoman-encoded file names sent by other programs through the sendpraat subroutine.

##4.5.04# (1 December 2006)
• @@Sound: Change gender...@: corrected a bug that often caused a female-to-male conversion to sound monotonous.

##4.5.03# (29 November 2006)
• Table: added independent-samples t-test.
• Linux: corrected a bug introduced in 4.5.02 that prevented sounds from playing and other weird things.

##4.5.02# (16 November 2006)
• Corrected yet another bug in the new @@Sound: To TextGrid (silences)...@.

##4.5.01# (28 October 2006)
• Sound window: the pitch drawing method is #Curves, #Speckles, or #Automatic.
• Corrected another bug in the new @@Sound: To TextGrid (silences)...@.

################################################################################
"What was new in 4.5?"
© Paul Boersma 2006

##4.5# (26 October 2006)

##4.4.35# (20 October 2006)
• In @ManPages you can now draw pictures.

##4.4.34# (19 October 2006)
• Corrected a bug in the new @@Sound: To TextGrid (silences)...@.

##4.4.33# (4 October 2006)
• Windows: corrected a bug introduced in 4.4.31 that caused Praat to skip the first line of the Buttons file.

##4.4.32# (30 September 2006)
• Scripting: more techniques for @@Scripting 4. Object selection|object selection@.
• Scripting: more support for putting the results of the #Info command into a string variable.

##4.4.31# (23 September 2006)
• Support for @@plug-ins@.
• Split between @@Create Strings as file list...@ and @@Create Strings as folder list...@.

##4.4.30# (28 August 2006)
• Table: Draw ellipse (standard deviation)...

##4.4.29# (21 August 2006)
• Allowed “European” symbols in file names and object names.

##4.4.28# (10 August 2006)
• Windows XP: Praat files can finally again be opened by double-clicking and by dragging them onto the Praat icon.
• Scripting (Windows): removed a bug that caused Praat to crash if the script window was closed when a file selector window was open.

##4.4.27# (4 August 2006)
• Table window: corrected vertical scroll bar (on Windows).
• Formulas: invSigmoid.
• Logging: added `'power'` (and documented the `'freq'` command).
• Removed a bug that caused @@Read two Sounds from stereo file...@ not to work in scripts.

##4.4.26# (24 July 2006)
• @@Sound & FormantTier: Filter@: much more accurate.

##4.4.25# (16 July 2006)
• TextGrid reading: don't set first boundary to zero for .wrd label files.

##4.4.24# (19 June 2006)
• Scripting: regular expressions allow replacement with empty string.

##4.4.23# (1 June 2006)
• Table: ignore more white space.

##4.4.22# (30 May 2006)
• Scripting: replacing with regular expression. See @@Formulas 6. String functions@.

##4.4.21# (29 May 2006)
• Made Manipulation objects readable again.

##4.4.20# (3 May 2006)
• Removed limit on number of menus (Praat could crash if the number of open windows was high).

##4.4.19# (28 April 2006)
• @@Table: Get mean...@, @@Table: Get standard deviation...@, @@Table: Get quantile...@.

##4.4.18# (24 April 2006)
• Table: ##View & Edit#: view the contents of a table.
• @@Table: Scatter plot...@.
• Scripting: more warnings against missing or extra spaces.

##4.4.17# (19 April 2006)
• @@Table: Collapse rows...|Table: Pool...@: computing averages and medians of dependent variables
for a selected combination of independent variables.
• @@Table: Formula...@ accepts string expressions as well as numeric expressions.
• @@Table: Sort...@ can sort by any number of columns.
• @@Table: Create with column names...@.
• @@Table: Report mean...@.
• Formulas: @@Formulas 8. Attributes of objects|`row$` and `col$` attributes@.
• Warning when trying to read data files whose format is newer than the Praat version.

##4.4.16# (1 April 2006)
• Spectrum window: dynamic range setting.
• SoundRecorder: corrected a bug in the Intel Mac edition.

##4.4.15# (30 March 2006)
• Source code even more compatible with 64-bit compilers.

##4.4.14# (29 March 2006)
• Source code more compatible with 64-bit compilers.

##4.4.13# (8 March 2006)
• @@Table: To TableOfReal...@: better handling of --undefined-- values (are now numeric).
• MacOS X: TextGrid files can be double-clicked to open.
• @@Create Strings as file list...@: now handles up to 1,000,000 files per directory.

##4.4.12# (24 February 2006)
• TextGrid: removed a bug introduced in 4.4.10 that caused Praat to crash when converting an IntervalTier into a TextGrid.

##4.4.11# (23 February 2006)
• Listening experiments: removed a bug that could cause Praat to crash when an ExperimentMFC object was removed.

##4.4.10# (20 February 2006)
• Intel computers: corrected reading and writing of 24-bit sound files (error introduced in 4.4.09).
• Create TextGrid: guard against zero tiers.
• MacOS X: correct visibility of Praat icon.
• MacOS X: correct dropping of Praat files on Praat icon.

##4.4.09# (19 February 2006)
• Macintosh: first Intel Macintosh version.
• Windows: @@Create Strings from folder list...@.

##4.4.08# (6 February 2006)
• Much improved cepstral smoothing.

##4.4.07# (2 February 2006)
• More scripting facilities (local variables in procedures, e.g. `.x` and `.text$`).
• Faster formulas.

##4.4.06# (30 January 2006)
• More scripting facilities (`Object_xxx [ ]`, `Self.nx`, `Table_xxx$ [ ]`, better messages).
• Better reading and writing of Xwaves label files.

##4.4.05# (26 January 2006)
• @ExperimentMFC: removed a bug that caused Praat to crash when the Oops button was pressed after the experiment finished.
• TextGrid: an IntervalTier can be written to an Xwaves label file.

##4.4.04# (6 January 2006)
• Windows: Quicktime support (see at 4.4.03).

##4.4.03# (6 January 2006)
• Macintosh: Quicktime support, i.e., @@Read from file...@ can now read the audio from several kinds of movie files (.mov, .avi).

##4.4.02# (5 January 2006)
• OT learning: allow the decision strategies of Harmonic Grammar and Linear OT.

##4.4.01# (2 January 2006)
• Picture window: @@Logarithmic marks...@ allows reversed axes.
• Manipulation window: removed a bug from ##Shift frequencies...# that caused much too small shifts in semitones.
• @@TextGrid: Remove point...@.

################################################################################
"What was new in 4.4?"
© Paul Boersma 2005

##4.4# (19 December 2005)

##4.3.37# (15 December 2005)
• @@Principal component analysis@: now accepts tables with more variables (columns) than cases (rows).
• TIMIT label files: removed a bug that caused Praat to crash for files whose first part was not labelled.

##4.3.36# (11 December 2005)
• Ltas: Average.
• Optimality Theory: compute crucial rankings (select OTGrammar + PairDistribution).

##4.3.35# (8 December 2005)
• @ExperimentMFC: switched off warnings for stereo files.

##4.3.34# (8 December 2005)
• Sound window: the arrow scroll step is settable.
• You can now run multiple listening experiments (@ExperimentMFC) in one #Run.
• @@Formant: Get quantile of bandwidth...@.

##4.3.33# (6 December 2005)
• Removed three bugs introduced in 4.3.32 in @ExperimentMFC.

##4.3.32# (5 December 2005)
• Many more possibilities in @ExperimentMFC.

##4.3.31# (27 November 2005)
• @@Sound: To Ltas (pitch-corrected)...@

##4.3.30# (18 November 2005)
• TableOfReal: Scatter plot: allows reversed axes.

##4.3.29# (11 November 2005)
• Windows: many more font sizes.

##4.3.28# (7 November 2005)
• Fontless EPS files: corrected character width for Symbol font (depended on SILIPA setting).
• Windows: more reliable detection of home directory.

##4.3.27# (7 October 2005)
• TextGrid & Pitch: draw with text alignment.

##4.3.26# (29 September 2005)
• Macintosh: corrected error introduced in 4.3.25.

##4.3.25# (28 September 2005)
• Macintosh: allowed recording with sampling frequencies of 12 and 64 kHz.

##4.3.24# (26 September 2005)
• @@Table: Down to TableOfReal...@: one column of the Table can be used as the row labels for the TableOfReal,
  and the strings in the remaining columns of the Table are replaced with whole numbers assigned in alphabetical order.

##4.3.23# (24 September 2005)
• @@Read Table from comma-separated file...@
• @@Read Table from tab-separated file...@
• Write picture as fontless EPS file: choice between XIPA and SILIPA93.
• Bold IPA symbols in EPS files (fontless SILIPA93 only).

##4.3.22# (8 September 2005)
• Macintosh: variable scroll bar size (finally, 7 years since System 8.5).

##4.3.21# (1 September 2005)
• Macintosh: error message if any of the fonts Times, Helvetica, Courier and Symbol are unavailable at start-up.
• Renamed Control menu to “Praat” on all platforms (as on the Mac),
to reflect the fact that no other programs than Praat have used the Praat shell for five years.
• Script editor: Undo and Redo buttons (only on the Mac for now).
• Manual: corrected a bug that sometimes caused Praat to crash when trying to print.

##4.3.20# (18 August 2005)
• Log files: include name of editor window.

##4.3.19# (20 July 2005)
• Improved buttons in manual.
• @@Read TableOfReal from headerless spreadsheet file...@: allow row and column labels to be 30,000 rather than 100 characters.

##4.3.18# (12 July 2005)
• Glottal source for sound synthesis, corrected and documented.

##4.3.17# (7 July 2005)
• Glottal source for sound synthesis.
• Multi-level Optimality Theory: parallel evaluation and bidirectional learning.

##4.3.16# (22 June 2005)
• Pitch drawing: corrected logarithmic scales.

##4.3.15# (22 June 2005)
• Graphics: better dotted lines in pitch contours; clipped pitch curves in editor windows.
• Pitch analysis: more different units (semitones %re 1 Hz).

##4.3.14# (14 June 2005)
• Scripting: regular expressions.
• Removed a bug that caused Praat to crash if a proposed object name was longer than 200 characters.

##4.3.13# (19 May 2005)
• Macintosh: an option to switch off screen previews in EPS files.
• Sources: compatibility of makefiles with MinGW (Minimalist GNU for Windows).

##4.3.12# (10 May 2005)
• Some more manual tricks.

##4.3.11# (6 May 2005)
• TextGrid editor: show number of intervals.

##4.3.10# (25 April 2005)
• @@Table: Get logistic regression...@.

##4.3.08# (19 April 2005)
• OT learning: store history with @@OTGrammar & Strings: Learn from partial outputs...@.

##4.3.07# (31 March 2005)
• Linux: removed a bug that could cause a sound to stop playing.

##4.3.04# (9 March 2005)
• Use SIL Doulos IPA 1993/1996 instead of 1989.

##4.3.03# (2 March 2005)
• TextGrid window: green colouring of matching text.
• Regular expressions can be used in many places.
• Pitch analysis: switched off formant-pulling.

##4.3.02# (16 February 2005)
• @@TextGrid: Remove boundary at time...@
• Scripting: corrected %nowarn.
• Linux: guard against blocking audio device.
• Macintosh: guard against out-of-range audio level meter.

##4.3.01# (9 February 2005)
• Replaced PostScript font SILDoulosIPA with XIPA (adapted for Praat by Rafael Laboissière).
• @@Sound: Set part to zero...@
• @@Pitch: To Sound (sine)...@
• @@Sound & TextGrid: Clone time domain@.

################################################################################
"What was new in 4.3?"
© Paul Boersma 2005

Praat 4.3, 26 January 2005
==========================

General:
• #Apply button in settings windows for menu commands and in script forms.
• Info window can be saved.
• Removed 30,000-character limit in Info window.

Phonetics:
• Speeded up intensity analysis by a factor of 10
  (by making its time resolution 0.01 ms rather than 0.0001 ms at a sampling frequency of 44 kHz).
• Speeded up pitch analysis and spectrogram analysis by a factor of two.
• Sound: To Spectrum... now has a reasonably fast non-FFT version.
• Calibrated long-term average spectrum (Sound: To Ltas...).
• Pitch-corrected LTAS analysis.
• Sound: Scale intensity.
• PitchTier: To Sound (sine).
• Better warnings against use of the LPC object.

Voice:
• July 9, 2004 (4.2.08): Shimmer measurements: more accurate and less sensitive to additive noise.
• More extensive voice report: pitch statistics; harmonicity.

Audio:
• Reading and opening 24-bit and 32-bit sound files (saving still goes in 16 bits).
• LongSound: save separate channels.
• Macintosh: much faster reading of WAV files.

Listening experiments:
• Subjects can now respond with keyboard presses.

Graphics:
• One can now drag the inner viewport in the Picture window, excluding the margins.
  This is nice e.g. for creating square viewports or for drawing a waveform and a spectrogram in close contact.
• Unix: picture highlighting as on Mac and Windows.
• More drawing methods for Sound and Ltas (curve, bars, poles, speckles).

OT learning:
• Monitor rankings when learning.
• OTGrammar: Remove harmonically bounded candidates...
• OTGrammar: Save as headerless spreadsheet file...
• Metrics grammar: added *Clash, *Lapse, WeightByPosition and *MoraicConsonant.

Scripting:
• nowarn, noprogress, nocheck.
• Line numbers.

################################################################################
"What was new in 4.2?"
© Paul Boersma 2004

Praat 4.2, 4 March 2004
=======================

General:
• July 10, 2003: Open source code (@@General Public Licence@).

Phonetics:
• Faster computation of spectrum, spectrogram, and pitch.
• More precision in numeric libraries.
• PitchTier: Interpolate quadratically.
• TextGrids can be saved chronologically (and Praat can read that file again).
• Sound editor window @@Time step settings...@: #Automatic, #Fixed, and ##View-dependent#.
• Sound window: distinguish basic from advanced spectrogram and pitch settings.
• Read TableOfReal from headerless spreadsheet file...: cells with strings are considered zero.
• Sound window: introduced time step as advanced setting.
• Sound window: reintroduced view range as advanced setting.
• Ltas: Compute trend line, Subtract trend line.

Audio:
• Sun workstations: support audio servers.

Graphics:
• Better selections in Picture window and editor windows.
• Picture window: better handling of rectangles and ellipses for reversed axes.
• Windows: corrected positioning of pictures on clipboard and in metafiles.
• Windows: EPS files check availability of Times and TimesNewRomanPSMT.
• Polygon: can now also paint in colour instead of only in grey values.
• Unlimited number of points for polygons in PostScript (may not work on very old printers).
• Picture window: line widths on all printers and clipboards are now equal to line widths used on PostScript printers:
  a line with a line width of “1” will be drawn with a width 3/8 points. This improves the looks of pictures printed
  on non-PostScript printers, improves the looks of pictures copied to your wordprocessor when printed,
  and changes the looks of pictures copied to your presentation program.

OT learning:
• Metrics grammar supports “impoverished overt forms”,
  i.e. without secondary stress even if surface structures do have secondary stress.
• Support for crucially tied constraints and tied candidates.
• Support for backtracking in EDCD.
• Queries for testing grammaticality.

Scripting:
• ManPages: script links can receive arguments.
• ManPages: variable duration of recording.
• Support for unlimited size of script files in editor window on Windows XP and MacOS X (the Unix editions already had this).
• Improved the reception of %sendpraat commands on Windows XP.

################################################################################
"What was new in 4.1?"
© Paul Boersma 2003

Praat 4.1, 5 June 2003
======================

General:
• MacOS X edition.
• Removed licensing.
• More than 99 percent of the source code distributed under the @@General Public Licence@.
• Windows 2000 and XP: put preferences files in home directory.

Phonetics:
• Spectrum: the sign of the Fourier transform has changed, to comply with common use
  in technology and physics. Old Spectrum files are converted when read.
• Spectral moments.
• Many jitter and shimmer measures, also in the Sound editor window.
• PitchTier: shift or multiply frequencies (also in ManipulationEditor).
• TextGrid: shift times, scale times.
• Overlap-add synthesis: reduced buzz in voiceless parts.
• @@Sound: Change gender...
• Editors: @@Intro 3.6. Viewing a spectral slice@.
• Editors: ##Get spectral power at cursor cross#.
• @@Sound: To PointProcess (periodic, peaks)...@
• Ltas: merge.

Listening experiments:
• Goodness judgments.
• Multiple @@ResultsMFC: To Table@, so that the whole experiment can go into a single statistics file.
• Stimulus file path can be relative to directory of experiment file.
• @ExperimentMFC: multiple substimuli for discrimination tests.

Statistics:
• New @Table object for column @statistics: Pearson's %r, Kendall's %\\ta-%b, %t-test.
• Table: scatter plot.
• Table: column names as variables.
• @@T-test@.
• TableOfReal: Extract rows where column...
• TableOfReal: Get correlation....
• @@Correlation: Confidence intervals...
• @@SSCP: Get diagonality (bartlett)...

OT learning:
• Tutorial for bidirectional learning.
• Random choice between equally violating candidates.
• More constraints in metrics grammar.
• Learning in editor.

Graphics:
• Printing: hard-coded image interpolation for EPS files and PostScript printers.

Scripting:
• New @Formulas tutorial.
• @Formulas: can use variables without quotes.
• Formulas for PitchTier, IntensityTier, AmplitudeTier, DurationTier.
• Refer to any matrices and tables in formulas, e.g. `Sound_hello (x)` or `Table_everything [row, col]`
  or `Table_tokens [i, “F1”]`.
• Assignment by modification, as with += -= *= /=.
• New functions: @`date$` (), @`extractNumber`, @`extractWord$`, @`extractLine$`. See @@Formulas 6. String functions@.
• @@Scripting 5.8. Including other scripts@.
• String formulas in the calculator.
• Stopped support of things that had been undocumented for the last four years:
  #let, #getnumber, #getstring, #ARGS, #copy, #proc, variables with capitals, and strings in numeric variables;
  there are messages about how to modify your old scripts.
• Disallowed ambiguous expressions like -3\\^ 2.

################################################################################
"What was new in 4.0?"
© Paul Boersma 2001

Praat 4.0, 15 October 2001
==========================

Editors:
• Simplified selection and cursor in editor windows.
• Spectrogram, pitch contour, formant contour, and intensity available in the
  Sound, LongSound, and TextGrid editors.
• TextGrid editor: additions and improvements.
• @@Log files@.

Phonetics library:
• @ExperimentMFC: multiple-forced-choice listening experiments.
• @@Sound: To Pitch (ac)...@: pitch contour less dependent on time resolution.
  This improves the constancy of the contours in the editors when zooming.
• TextGrid: additions and improvements.
• Sounds: Concatenate recoverably. Creates a TextGrid whose interval labels are the original
  names of the sounds.
• Sound & TextGrid: Extract all intervals. The reverse of the previous command.
• Filterbank analyses, @MelFilter, @BarkFilter and
  @FormantFilter, by @@band filtering in the frequency domain@." )
• Cepstrum by David Weenink: @MFCC, @LFCC.
  @Cepstrum object is a representation of the %%complex cepstrum%.
• Intensity: To IntensityTier (peaks, valleys).
• Replaced Analysis and AnalysisEditor with @Manipulation and @ManipulationEditor.

Phonology library:
• PairDistribution: Get percentage correct (maximum likelihood, probability matching).
• OTGrammar & PairDistribution: Get percentage correct...

Graphics:
• Improved spectrogram drawing.
• @@Special symbols@: háček.
• Macintosh: improved screen rendition of rotated text.

Audio:
• Macintosh: support for multiple audio input devices (sound cards).

Statistics and numerics library:
• More statistics by David Weenink.
• Improved random numbers and other numerical stuff.
• @@Regular expressions@.

Scripting:
• Formatting in variable substitution, e.g. 'pitch:2' gives two digits after the decimal point.
• Added @`fixed$` () to scripting language for formatting of numbers.

Documentation:
• @@Multidimensional scaling@ tutorial.
• Enabled debugging-at-a-distance.

################################################################################
"What was new in 3.9?"
© Paul Boersma 2000

Praat 3.9, 18 October 2000
==========================

Editors:
• Shift-click and shift-drag extend or shrink selection in editor windows.
• Grouped editors can have separate zooming and scrolling (FunctionEditor preferences).
• Cursor follows playing sound in editors; interruption by Escape key moves the cursor.
• TextGridEditor: optimized for transcribing large corpora: text field, directly movable boundaries,
  more visible text in tiers, @SpellingChecker,
  type while the sound is playing, complete keyboard navigation, control font size, control text alignment,
  shift-click near boundary adds interval to selection.
• Stereo display in LongSound and TextGrid editors.
• LongSoundEditor and TextGridEditor: write selection to audio file.
• SoundEditor: added command ##Extract selection (preserve times)#.
• IntervalTierEditor, DurationTierEditor.
• Added many query commands in editors.

Phonetics library:
• @@Sound: To Formant...@: sample-rate-independent formant analysis.
• @@Sound: To Harmonicity...@ (glottal-to-noise excitation ratio).
• Pitch: support for ERB units, draw all combinations of line/speckle and linear/logarithmic/semitones/mels/erbs,
  optionally with TextGrid, Subtract linear fit.
• Spectrum: Draw along logarithmic frequency axis.
• TextGrid:  modification commands, Extract part, Shift to zero, Scale times (with Sound or LongSound).
• @@Matrix: To TableOfReal@, @@Matrix: Draw contour...@.
• Concatenate Sound and LongSound objects.
• File formats: save PitchTier in spreadsheet format, read CGN syntax files (XML version),
  text files now completely file-server-safe (independent from Windows/Macintosh/Unix line separators).

Statistics and numerics library:
• @@Principal component analysis@.
• @@Discriminant analysis@.
• @Polynomial: drawing, @@Roots|root@ finding etc.
• @@TableOfReal: Draw box plots...@.
• @@Covariance: To TableOfReal (random sampling)...@.
• @@SSCP: Get sigma ellipse area...@.
• Query @DTW for “weighted distance” of time warp.
• @@Distributions: To Strings (exact)...@
• @@Strings: Randomize@.

Phonology library:
• @@OTGrammar: To PairDistribution@.

Graphics:
• Full support for colour inkjet printers on Windows and Macintosh.
• Full support for high-resolution colour clipboards and metafiles for
  Windows and Macintosh programs that support them (this include MS Word
  for Windows, but unfortunately not MS Word for Macintosh).
• Colour in EPS files.
• Interpolating grey images, i.e. better zoomed spectrograms.
• Linux: support for 24-bits screens.

Audio:
• Asynchronous sound play.
• Linux: solved problems with /dev/mixer (“Cannot read MIC gain.”) on many computers.
• Added possibility of zero padding for sound playing,
  in order to reduce clicks on some Linux and Sun computers.
• LongSound supports mono and stereo, 8-bit and 16-bit, %\\mu-law and A-law,
  big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.
• @@Read two Sounds from stereo file...@ supports 8-bit and 16-bit, %\\mu-law and A-law,
  big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.
• SoundRecorder writes to 16-bit AIFC, WAV, NeXT/Sun, and NIST mono and stereo files.
• Sound & LongSound: write part or whole to mono or stereo audio file.
• Read Sound from raw Alaw file.
• Artword & Speaker (& Sound) movie: real time on all platforms.

Scripting:
• @@Formulas 5. Mathematical functions@: added statistical functions: %\\ci^2, Student T, Fisher F, binomial,
  and their inverse functions.
• Windows: program #`praatcon` for use as a Unix-style console application.
• Windows and Unix: Praat can be run with a command-line interface without quitting on errors.
• Unix & Windows: can use <stdout> as a file name (supports pipes for binary data).
• @sendpraat now also for Macintosh.
• @@Scripting 6.7. Sending a message to another program|sendsocket@.
• @@Read from file...@ recognizes script files if they begin with “`#!`”.
• Script links in @ManPages.

Documentation
• Tutorials on all subjects available through @Intro.

################################################################################
"What was new in 3.8?"
© Paul Boersma 1999

Praat 3.8, 12 January 1999
==========================

Phonetics library
• New objects: @LongSound (view and label long sound files), with editor; PairDistribution.
• @@Overlap-add@ manipulation of voiceless intervals, version 2: quality much better now;
target duration is exactly as expected from Duration tier or specified lengthening in @@Sound: Lengthen (overlap-add)...@.
• Audio: Escape key stops audio playing (on Mac also Command-period).
• @SoundRecorder: allows multiple recordings without close; Play button; Write buttons; buffer size can be set.
• Reverse a Sound or a selection of a Sound.
• @@Sound: Get nearest zero crossing...@.
• @@Formant: Scatter plot (reversed axes)...@.
• @@TextGrid & Pitch: Speckle separately...@.
• ##Extract Sound selection (preserve times)# in TextGridEditor.
• More query commands for Matrix, TableOfReal, Spectrum, PointProcess.

Phonology library
• 25-page OT learning tutorial.
• Made the OT learner 14 times as fast.

Platforms
• May 23: Windows beta version.
• April 24: Windows alpha version.

Files
• Read more Kay, Sun (.au), and WAV sound files.
• @@Read Strings from raw text file...@
• @@Create Strings as file list...@.
• @@Read IntervalTier from Xwaves...@
• hidden ##Read from old Windows Praat picture file...#

Graphics
• Use colours (instead of only greys) in ##Paint ellipse...# etc.
• More true colours (maroon, lime, navy, teal, purple, olive).
• Direct printing from Macintosh to PostScript printers.
• Hyperpage printing to PostScript printers and PostScript files.
• Phonetic symbols: raising sign, lowering sign, script g, corner, ligature, pointing finger.

Shell
• November 4: all dialogs are modeless (which is new for Unix and Mac).
• September 27: @sendpraat for Windows.

Scripting
• January 7: scriptable editors.
• October 7: file I/O in scripts.
• August 23: script language includes all the important functions for string handling.
• June 24: string variables in scripts.
• June 22: faster look-up of script variables.
• June 22: unlimited number of script variables.
• April 5: suspended chopping of trailing spaces.
• March 29: enabled formulas as arguments to dialogs (also interactive).

################################################################################
"What was new in 3.7?"
© Paul Boersma 1998

Praat 3.7, 24 March 1998
========================

Editors:
• In all FunctionEditors: drag to get a selection.

Phonetics library:
• Many new query (#Get) commands for @Sound, @Intensity, @Harmonicity, @Pitch,
@Formant, @Ltas, @PitchTier, @IntensityTier, @DurationTier, #FormantTier.
• Many new modification commands.
• Many new interpolations.
• Sound enhancements: @@Sound: Lengthen (overlap-add)...@, @@Sound: Deepen band modulation...@
• @@Source-filter synthesis@ tutorial, @@Sound & IntensityTier: Multiply@,
  @@Sound & FormantTier: Filter@, @@Formant: Formula (frequencies)...@, @@Sound: Pre-emphasize (in-place)...@.

Labelling
• TextGrid queries (#Get times and labels in a script).
• @@TextGrid: Count labels...@.
• @@PointProcess: To TextGrid (vuv)...@: get voiced/unvoiced information from a point process.
• @@IntervalTier: To TableOfReal...@: labels become row labels.
• @@TextTier: To TableOfReal@.

Numerics and statistics library
• Multidimensional scaling (Kruskal, INDSCAL, etc).
• @TableOfReal: @@TableOfReal: ||Set value...@, @@TableOfReal: ||Formula...@,
  @@TableOfReal: ||Remove column...@, @@TableOfReal: ||Insert column...@,
  @@TableOfReal: ||Draw as squares...@, @@TableOfReal: ||To Matrix@.

Phonology library
• OT learning: new strategies: weighted symmetric plasticity (uncancelled or all).

Praat shell
• First Linux version.
• Eight new functions like e.g. @`hertzToBark` in @@Formulas 5. Mathematical functions@.
• @@Praat script@: procedure arguments; object names.

Documentation:
• 230 more man pages (now 630).
• Hypertext: increased readability of formulas, navigation with keyboard.

################################################################################
"What was new in 3.6?"
© Paul Boersma 1997

Praat 3.6, 27 October 1997
==========================

Editors:
• Intuitive position of B and E buttons on left-handed mice.
• @SoundEditor: copy %windowed selection to list of objects.
• @SoundEditor: undo Cut, Paste, Zero.
• @SpectrumEditor: copy band-filtered spectrum or sound to list of objects.
• @ManipulationEditor: LPC-based pitch manipulation.

Objects:
• Use “-” and “+” in object names.

Phonetics library
• LPC-based resynthesis in @ManipulationEditor.
• @Sound: direct modification without formulas (addition, multiplication, windowing)
• @Sound: filtering in spectral domain by formula.
• Create a simple @Pitch object from a @PitchTier (for %F__0_) and a @Pitch (for V/U).
• Semitones in @PitchTier tables.
• @PointProcess: transplant time domain from @Sound.
• Much more...

Phonology library
• Computational Optimality Theory. See @@OT learning@.

Hypertext
• You can use @ManPages files for creating your own tutorials.
  These contains buttons for playing and recording sounds, so you can use this for creating
  an interactive IPA sound training course.

Scripting:
• Programmable @@Praat script@ language: variables, expressions, control structures,
  procedures, complete dialog box, exchange of information with Info window, continuation lines.
• Use platform-independent relative file paths in @@Praat script@.
• @ScriptEditor: Run selection.

Graphics:
• Rotation and scaling while printing the @@Picture window@.
• Apart from bold and italic, now also bold-italic (see @@Text styles@).
• Rounded rectangles.
• Conversion of millimetres and world coordinates.
• Measurement of text widths (screen and PostScript).

Unix:
• Use the @sendpraat program for sending messages to running Praat programs.

Mac:
• Praat looks best with the new and beautiful System 8.

################################################################################
"What was new in 3.5?"
© Paul Boersma 1997

Praat 3.5, 27 May 1997
======================

New editors:
• @TextGridEditor replaces and extends LabelEditor: edit points as well as intervals.
• @@ManipulationEditor|AnalysisEditor@ replaces and extends PsolaEditor: view pitch, spectrum, formant, and intensity
analyses in a single window, and allow pitch and duration resynthesis by overlap-add and more (would be undone in 3.9.19).
• @SpectrumEditor allows you to view and edit spectra.

Praat shell:
• @@History mechanism@ remembers all the commands that you have chosen,
and allows you to put them into a script.
• @ScriptEditor allows you to edit and run any Praat script, and to put it under a button.
• All added and removed buttons are remembered across sessions.
• #ButtonEditor allows you to make buttons visible or invisible.

Evaluations:
• In his 1996 doctoral thesis, Henning Reetz
compared five pitch analysis routines; @@Sound: To Pitch (ac)...@ appeared to make the fewest errors.
H. Reetz (1996): %%Pitch Perception in Speech: a Time Domain Approach%, Studies in Language and Language Use #26,
IFOTT, Amsterdam (ICG Printing, Dordrecht).

Documentation:
• 140 more man pages (now 330).
• Tables and pictures in manual.
• Printing the entire manual.
• Logo.

New types:
• Labelling & segmentation: @TextGrid, @IntervalTier, @TextTier.
• Analysis & manipulation: @@Manipulation|Analysis@.
• Statistics: @TableOfReal, @Distributions, @Transition

File formats:
• Read and write rational numbers in text files.
• Read 8-bit .au sound files.
• Read and write raw 8-bit two\'s-complement and offset-binary sound files.

Audio:
• 16-bit interactive Sound I/O on Mac.
• Record sounds at 9.8 kHz on SGI.

New commands:
• Two more pitch-analysis routines.
• Sound to PointProcess: collect all maxima, minima, zero crossings.
• PointProcess: set calculus.
• TextGrid: extract time-point information.
• Compute pitch or formants at given time points.
• Put pitch, formants etc. in tables en get statistics.
• Many more...

Macintosh:
• 16-bit interactive sound I/O.
• Fast and interpolating spectrogram drawing.
• Phonetic Mac screen font included in source code (as a fallback to using SIL Doulos IPA).
• Keyboard shortcuts, text editor, help under question mark, etc.

################################################################################
"What was new in 3.3?"
© Paul Boersma 1996

Praat 3.3, 6 October 1996
=========================
• Documentation: hypertext help browser, including the first 190 man pages.
• New editors: type #TextTier for labelling times instead of intervals.
• New actions: #Formant: Viterbi tracker, Statistics menu, Scatter plot.
• Evaluation: For HNR analysis of speech, the cross-correlation method,
  which has a sensitivity of 60 dB and a typical time resolution of 12 milliseconds,
  must be considered better than the autocorrelation method,
  which has a better sensitivity (80 dB), but a much worse time resolution (30 ms).
  For pitch analysis, the autocorrelation method still beats the cross-correlation method
  because of its better resistance against noise and echos,
  and despite its marginally poorer resolution (15 vs. 12 ms).
• User preferences are saved across sessions.
• The phonetic X screen font included in the source code.
• Xwindows resources included in the source code
• Graphics: eight colours, small caps, text rotation.
• File formats: Sun/NexT mu-law files, raw matrix text files, Xwaves mark files.
• Accelerations: keyboard shortcuts, faster dynamic menu, Shift-OK keeps file selector on screen.
• Object type changes: #StylPitch and #MarkTier are now called @PitchTier and @TextTier, respectively.
  Old files can still be read.
• Script warning: all times in dialogs are in seconds now: milliseconds have gone.

################################################################################
"What was new in 3.2?"
© Paul Boersma 1996

Praat 3.2, 29 April 1996
========================
• Sound I/O for HPUX, Sun Sparc 5, and Sun Sparc LX.
• Cross-correlation pitch and HNR analysis.
• Facilities for generating tables from scripts.
• Editing and playing stylized pitch contours and point processes.
• Overlap-add pitch manipulation.
• Spectral smoothing techniques: cepstrum and LPC.
• Time-domain pitch analysis with jitter measurement.
• Read and write Bell-Labs sound files and Kay CSL audio files.
• Replaced IpaTimes font by free SILDoulos-IPA font, and embedded phonetic font in PostScript picture.
• Completed main phonetic characters.

################################################################################
"What was new in 3.1?"
© Paul Boersma 1995

Praat 3.1, 5 December 1995
==========================
• Add and remove buttons dynamically.
• DataEditor (Inspect button).
• Initialization scripts.
• Logarithmic axes.
• Call remote ADDA server directly.

################################################################################
)~~~"
MAN_PAGES_END

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
	• TextGrid & Sound: Extract intervals with margins.")
	• Spectrum: draw power, re, im, phase.")
	• Formant: To Spectrum (slice)... (combines Formant-to-LPC and LPC-to-Spectrum-slice)")
	• Read and/or write Matlab files, MBROLA files, Xwaves files, CHAT files.") // Aix
	• Matrix: draw numbers.")
	• Fractions with \\bsf{a|b}.")
	• Move objects up and down list.")
	• Spectrogram cross-correlation.")
	• Labels in AIFC file.") // Theo Veenker 19980323
	• Improve scrolling and add selection in hyperpages.")
	• Segment spectrograph?") // Ton Wempe, Jul 16 1996
	• Phoneme-to-articulation conversion??") // Mirjam Ernestus, Jul 1 1996
ENTRY (U"Known bugs in the Windows version")
	• Cannot stand infinitesimal zooming in SpectrogramEditor.")
*/

MAN_BEGIN (U"FLAC BSD 3-clause license", U"ppgb", 20210823)
NORMAL (U"The Praat source code contains a copy of the FLAC software (see @Acknowledgments). "
	"Here is the FLAC license text:")
CODE (U"libFLAC - Free Lossless Audio Codec library")
CODE (U"Copyright (C) 2000-2009  Josh Coalson")
CODE (U"Copyright (C) 2011-2018  Xiph.Org Foundation")
CODE (U"")
CODE (U"Redistribution and use in source and binary forms, with or without "
	"modification, are permitted provided that the following conditions "
	"are met:")
CODE (U"")
CODE (U"- Redistributions of source code must retain the above copyright "
	"notice, this list of conditions and the following disclaimer.")
CODE (U"")
CODE (U"- Redistributions in binary form must reproduce the above copyright "
	"notice, this list of conditions and the following disclaimer in the "
	"documentation and/or other materials provided with the distribution.")
CODE (U"")
CODE (U"- Neither the name of the Xiph.org Foundation nor the names of its "
	"contributors may be used to endorse or promote products derived from "
	"this software without specific prior written permission.")
CODE (U"")
CODE (U"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
	"``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
	"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
	"A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR "
	"CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
	"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
	"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
	"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
	"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
	"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
	"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")
MAN_END

MAN_BEGIN (U"Ogg Vorbis BSD 3-clause license", U"ppgb", 20201227)
NORMAL (U"The Praat source code contains a copy of the Ogg Vorbis software (see @Acknowledgments). "
	"Here is the Ogg Vorbis license text:")
CODE (U"Copyright (c) 2002-2020 Xiph.org Foundation")
CODE (U"")
CODE (U"Redistribution and use in source and binary forms, with or without "
	"modification, are permitted provided that the following conditions "
	"are met:")
CODE (U"")
CODE (U"- Redistributions of source code must retain the above copyright "
	"notice, this list of conditions and the following disclaimer.")
CODE (U"")
CODE (U"- Redistributions in binary form must reproduce the above copyright "
	"notice, this list of conditions and the following disclaimer in the "
	"documentation and/or other materials provided with the distribution.")
CODE (U"")
CODE (U"- Neither the name of the Xiph.org Foundation nor the names of its "
	"contributors may be used to endorse or promote products derived from "
	"this software without specific prior written permission.")
CODE (U"")
CODE (U"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
	"``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
	"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
	"A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR "
	"CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
	"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
	"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
	"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
	"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
	"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
	"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")
MAN_END

MAN_BEGIN (U"Opus BSD 3-clause license", U"ppgb", 20210105)
NORMAL (U"The Praat source code contains a copy of the Opus software (see @Acknowledgments). "
	"Here is the Opus license text:")
CODE (U"Copyright (c) 2001-2011 Xiph.Org, Skype Limited, Octasic,")
CODE (U"                        Jean-Marc Valin, Timothy B. Terriberry,")
CODE (U"                        CSIRO, Gregory Maxwell, Mark Borgerding,")
CODE (U"                        Erik de Castro Lopo")
CODE (U"")
CODE (U"Redistribution and use in source and binary forms, with or without "
	"modification, are permitted provided that the following conditions "
	"are met:")
CODE (U"")
CODE (U"- Redistributions of source code must retain the above copyright "
	"notice, this list of conditions and the following disclaimer.")
CODE (U"")
CODE (U"- Redistributions in binary form must reproduce the above copyright "
	"notice, this list of conditions and the following disclaimer in the "
	"documentation and/or other materials provided with the distribution.")
CODE (U"")
CODE (U"- Neither the name of Internet Society, IETF or IETF Trust, nor the "
	"names of specific contributors may be used to endorse or promote "
	"products derived from this software without specific prior written "
	"permission.")
CODE (U"")
CODE (U"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
	"``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
	"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
	"A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR "
	"CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
	"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
	"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
	"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
	"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
	"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
	"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")
CODE (U"")
CODE (U"Opus is subject to the royalty-free patent licenses which are "
	"specified at:")
CODE (U"")
CODE (U"Xiph.Org Foundation:")
CODE (U"https://datatracker.ietf.org/ipr/1524/")
CODE (U"")
CODE (U"Microsoft Corporation:")
CODE (U"https://datatracker.ietf.org/ipr/1914/")
CODE (U"")
CODE (U"Broadcom Corporation:")
CODE (U"https://datatracker.ietf.org/ipr/1526/")
MAN_END

MAN_BEGIN (U"Skype Limited BSD 3-clause license", U"ppgb", 20220102)
NORMAL (U"The Praat source code contains a copy of the SILK software (see @Acknowledgments). "
	"Here is the Skype Limited license text:")
CODE (U"Copyright (c) 2006-2011 Skype Limited. All rights reserved.")
CODE (U"")
CODE (U"Redistribution and use in source and binary forms, with or without "
	"modification, are permitted provided that the following conditions "
	"are met:")
CODE (U"")
CODE (U"- Redistributions of source code must retain the above copyright "
	"notice, this list of conditions and the following disclaimer.")
CODE (U"")
CODE (U"- Redistributions in binary form must reproduce the above copyright "
	"notice, this list of conditions and the following disclaimer in the "
	"documentation and/or other materials provided with the distribution.")
CODE (U"")
CODE (U"- Neither the name of Internet Society, IETF or IETF Trust, nor the names of specific "
	"contributors, may be used to endorse or promote products derived from "
	"this software without specific prior written permission.")
CODE (U"")
CODE (U"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
	"``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
	"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
	"A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR "
	"CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
	"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
	"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
	"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
	"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
	"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
	"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")
MAN_END

MAN_BEGIN (U"Unicode Inc. license agreement", U"ppgb", 20220515)
NORMAL (U"The Praat source code contains a copy of the Unicode Character Database, "
	"as well as derived software (see @Acknowledgments). "
	"Here is the Unicode Inc. license text:")
CODE (U"UNICODE, INC. LICENSE AGREEMENT - DATA FILES AND SOFTWARE")
CODE (U"")
CODE (U"See Terms of Use <https://www.unicode.org/copyright.html> "
	"for definitions of Unicode Inc.’s Data Files and Software.")
CODE (U"")
CODE (U"NOTICE TO USER: Carefully read the following legal agreement.")
CODE (U"BY DOWNLOADING, INSTALLING, COPYING OR OTHERWISE USING UNICODE INC.'S "
	"DATA FILES (\"DATA FILES\"), AND/OR SOFTWARE (\"SOFTWARE\"), "
	"YOU UNEQUIVOCALLY ACCEPT, AND AGREE TO BE BOUND BY, ALL OF THE "
	"TERMS AND CONDITIONS OF THIS AGREEMENT.")
CODE (U"IF YOU DO NOT AGREE, DO NOT DOWNLOAD, INSTALL, COPY, DISTRIBUTE OR USE "
	"THE DATA FILES OR SOFTWARE.")
CODE (U"")
CODE (U"COPYRIGHT AND PERMISSION NOTICE")
CODE (U"")
CODE (U"Copyright © 1991-2022 Unicode, Inc. All rights reserved.")
CODE (U"Distributed under the Terms of Use in https://www.unicode.org/copyright.html.")
CODE (U"")
CODE (U"Permission is hereby granted, free of charge, to any person obtaining "
	"a copy of the Unicode data files and any associated documentation "
	"(the \"Data Files\") or Unicode software and any associated documentation "
	"(the \"Software\") to deal in the Data Files or Software "
	"without restriction, including without limitation the rights to use, "
	"copy, modify, merge, publish, distribute, and/or sell copies of "
	"the Data Files or Software, and to permit persons to whom the Data Files "
	"or Software are furnished to do so, provided that either")
CODE (U"(a) this copyright and permission notice appear with all copies "
	"of the Data Files or Software, or")
CODE (U"(b) this copyright and permission notice appear in associated Documentation.")
CODE (U"")
CODE (U"THE DATA FILES AND SOFTWARE ARE PROVIDED \"AS IS\", WITHOUT WARRANTY OF "
	"ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE "
	"WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND "
	"NONINFRINGEMENT OF THIRD PARTY RIGHTS.")
CODE (U"IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS "
	"NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL "
	"DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, "
	"DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER "
	"TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR "
	"PERFORMANCE OF THE DATA FILES OR SOFTWARE.")
CODE (U"")
CODE (U"Except as contained in this notice, the name of a copyright holder "
	"shall not be used in advertising or otherwise to promote the sale, "
	"use or other dealings in these Data Files or Software without prior "
	"written authorization of the copyright holder.")
MAN_END

MAN_BEGIN (U"Acknowledgments", U"ppgb", 20220926)
NORMAL (U"The following people contributed source code to Praat:")
LIST_ITEM (U"Paul Boersma: user interface, graphics, @printing, @@Intro|sound@, "
	"@@Intro 3. Spectral analysis|spectral analysis@, @@Intro 4. Pitch analysis|pitch analysis@, "
	"@@Intro 5. Formant analysis|formant analysis@, @@Intro 6. Intensity analysis|intensity analysis@, "
	"@@Intro 7. Annotation|annotation@, @@Intro 8. Manipulation|speech manipulation@, @@voice|voice report@, "
	"@@ExperimentMFC|listening experiments@, "
	"@@articulatory synthesis@, @@OT learning|optimality-theoretic learning@, "
	"tables, @formulas, @scripting, and adaptation of PortAudio, GLPK, regular expressions, and Opus.")
LIST_ITEM (U"David Weenink: "
	"@@feedforward neural networks@, @@principal component analysis@, @@multidimensional scaling@, @@discriminant analysis@, @LPC, "
	"@VowelEditor, "
	"and adaptation of GSL, LAPACK, fftpack, regular expressions, Espeak, Ogg Vorbis, and Opus.")
LIST_ITEM (U"Stefan de Konink and Franz Brauße: major help in port to GTK.")
LIST_ITEM (U"Tom Naughton: major help in port to Cocoa.")
LIST_ITEM (U"Erez Volk: adaptation of FLAC and MAD.")
LIST_ITEM (U"Ola Söder: kNN classifiers, k-means clustering.")
LIST_ITEM (U"Rafael Laboissière: adaptation of XIPA, audio bug fixes for Linux.")
LIST_ITEM (U"Darryl Purnell created the first version of audio for Praat for Linux.")
NORMAL (U"We included the following freely available software libraries in Praat (sometimes with adaptations):")
LIST_ITEM (U"XIPA: IPA font for Unix by Fukui Rei (GPL).")
LIST_ITEM (U"GSL: GNU Scientific Library by Gerard Jungman and Brian Gough (GPL 3 or later).")
LIST_ITEM (U"GLPK: GNU Linear Programming Kit by Andrew Makhorin (GPL 3 or later); "
	"contains AMD software by the same author (LGPL 2.1 or later).")
LIST_ITEM (U"PortAudio: Portable Audio Library by Ross Bencina, Phil Burk, Bjorn Roche, Dominic Mazzoni, Darren Gibbs, "
	"version 19.7.0 of April 2021 (CC-BY-like license).")
LIST_ITEM (U"Espeak: text-to-speech synthesizer by Jonathan Duddington and Reece Dunn (GPL 3 or later).")
LIST_ITEM (U"MAD: MPEG Audio Decoder by Underbit Technologies (GPL 2 or later).")
LIST_ITEM (U"FLAC: Free Lossless Audio Codec by Josh Coalson and Xiph.Org, version 1.3.3 (@@FLAC BSD 3-clause license@).")
LIST_ITEM (U"Ogg Vorbis: audio compression by Christopher Montgomery (@@Ogg Vorbis BSD 3-clause license@).")
LIST_ITEM (U"Opus: audio compression by Jean-Marc Valin, Gregory Maxwell, Christopher Montgomery, Timothy Terriberry, "
	"Koen Vos, Andrew Allen and others (@@Opus BSD 3-clause license@).")
LIST_ITEM (U"SILK: audio compression by Skype Limited (@@Skype Limited BSD 3-clause license@).")
LIST_ITEM (U"fftpack: public-domain Fourier transforms by Paul Swarztrauber, translated to C by Christopher Montgomery.")
LIST_ITEM (U"@LAPACK: public-domain numeric algorithms by Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., "
	"Courant Institute, Argonne National Lab, and Rice University, "
	"C edition by Peng Du, Keith Seymour and Julie Langdou, version 3.2.1 of June 2009.")
LIST_ITEM (U"Regular expressions by Henry Spencer, Mark Edel, Christopher Conrad, Eddy De Greef (GPL 2 or later).")
LIST_ITEM (U"Unicode Character Database by Unicode Inc., version 14.0 of September 2021 (@@Unicode Inc. license agreement@)")
NORMAL (U"Most of the source code of Praat is distributed under the General Public License, version 2 or later. "
	"However, as Praat includes the above software written by others, "
	"the whole of Praat is distributed under the General Public License, version 3 or later.")
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
LIST_ITEM (U"Daniel Hirst and Daniel McCloy, for managing the Praat Users List.")
LIST_ITEM (U"Rafael Laboissière and Andreas Tille, for maintaining the Debian package.")
LIST_ITEM (U"Jason Bacon and Adriaan de Groot, for maintaining the FreeBSD port.")
LIST_ITEM (U"José Joaquín Atria and Ingmar Steiner, for setting up the source-code repository on GitHub.")
LIST_ITEM (U"Hundreds of Praat users, for sending suggestions and notifying us of problems and thus helping us to improve Praat.")
MAN_END

MAN_BEGIN (U"License", U"ppgb", 20211016)
NORMAL (U"Praat is free software distributed under the @@General Public License, version 3@ or higher. "
	"See @Acknowledgments for details on the licenses of software libraries by others "
	"that are included in Praat.")
MAN_END

MAN_BEGIN (U"General Public License, version 3", U"ppgb", 20211026)
NORMAL (U"This is the license under which Praat as a whole is distributed.")
CODE (U"                    GNU GENERAL PUBLIC LICENSE")
CODE (U"                       Version 3, 29 June 2007")
CODE (U"")
CODE (U" Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>")
CODE (U" Everyone is permitted to copy and distribute verbatim copies")
CODE (U" of this license document, but changing it is not allowed.")
CODE (U"")
CODE (U"                            Preamble")
CODE (U"")
CODE (U"The GNU General Public License is a free, copyleft license for "
	"software and other kinds of works.")
CODE (U"")
CODE (U"The licenses for most software and other practical works are designed "
	"to take away your freedom to share and change the works.  By contrast, "
	"the GNU General Public License is intended to guarantee your freedom to "
	"share and change all versions of a program--to make sure it remains free "
	"software for all its users.  We, the Free Software Foundation, use the "
	"GNU General Public License for most of our software; it applies also to "
	"any other work released this way by its authors.  You can apply it to "
	"your programs, too.")
CODE (U"")
CODE (U"When we speak of free software, we are referring to freedom, not "
	"price.  Our General Public Licenses are designed to make sure that you "
	"have the freedom to distribute copies of free software (and charge for "
	"them if you wish), that you receive source code or can get it if you "
	"want it, that you can change the software or use pieces of it in new "
	"free programs, and that you know you can do these things.")
CODE (U"")
CODE (U"To protect your rights, we need to prevent others from denying you "
	"these rights or asking you to surrender the rights.  Therefore, you have "
	"certain responsibilities if you distribute copies of the software, or if "
	"you modify it: responsibilities to respect the freedom of others.")
CODE (U"")
CODE (U"For example, if you distribute copies of such a program, whether "
	"gratis or for a fee, you must pass on to the recipients the same "
	"freedoms that you received.  You must make sure that they, too, receive "
	"or can get the source code.  And you must show them these terms so they "
	"know their rights.")
CODE (U"")
CODE (U"Developers that use the GNU GPL protect your rights with two steps: "
	"(1) assert copyright on the software, and (2) offer you this License "
	"giving you legal permission to copy, distribute and/or modify it.")
CODE (U"")
CODE (U"For the developers' and authors' protection, the GPL clearly explains "
	"that there is no warranty for this free software.  For both users' and "
	"authors' sake, the GPL requires that modified versions be marked as "
	"changed, so that their problems will not be attributed erroneously to "
	"authors of previous versions.")
CODE (U"")
CODE (U"Some devices are designed to deny users access to install or run "
	"modified versions of the software inside them, although the manufacturer "
	"can do so.  This is fundamentally incompatible with the aim of "
	"protecting users' freedom to change the software.  The systematic "
	"pattern of such abuse occurs in the area of products for individuals to "
	"use, which is precisely where it is most unacceptable.  Therefore, we "
	"have designed this version of the GPL to prohibit the practice for those "
	"products.  If such problems arise substantially in other domains, we "
	"stand ready to extend this provision to those domains in future versions "
	"of the GPL, as needed to protect the freedom of users.")
CODE (U"")
CODE (U"Finally, every program is threatened constantly by software patents. "
	"States should not allow patents to restrict development and use of "
	"software on general-purpose computers, but in those that do, we wish to "
	"avoid the special danger that patents applied to a free program could "
	"make it effectively proprietary.  To prevent this, the GPL assures that "
	"patents cannot be used to render the program non-free.")
CODE (U"")
CODE (U"The precise terms and conditions for copying, distribution and "
	"modification follow.")
CODE (U"")
CODE (U"                       TERMS AND CONDITIONS")
CODE (U"")
CODE (U"0. Definitions.")
CODE (U"")
CODE (U"“This License” refers to version 3 of the GNU General Public License.")
CODE (U"")
CODE (U"“Copyright” also means copyright-like laws that apply to other kinds of "
	"works, such as semiconductor masks.")
CODE (U"")
CODE (U"“The Program” refers to any copyrightable work licensed under this "
	"License.  Each licensee is addressed as “you”.  “Licensees” and "
	"“recipients” may be individuals or organizations.")
CODE (U"")
CODE (U"To “modify” a work means to copy from or adapt all or part of the work "
	"in a fashion requiring copyright permission, other than the making of an "
	"exact copy.  The resulting work is called a “modified version” of the "
	"earlier work or a work “based on” the earlier work.")
CODE (U"")
CODE (U"A “covered work” means either the unmodified Program or a work based "
	"on the Program.")
CODE (U"")
CODE (U"To “propagate” a work means to do anything with it that, without "
	"permission, would make you directly or secondarily liable for "
	"infringement under applicable copyright law, except executing it on a "
	"computer or modifying a private copy.  Propagation includes copying, "
	"distribution (with or without modification), making available to the "
	"public, and in some countries other activities as well.")
CODE (U"")
CODE (U"To “convey” a work means any kind of propagation that enables other "
	"parties to make or receive copies.  Mere interaction with a user through "
	"a computer network, with no transfer of a copy, is not conveying.")
CODE (U"")
CODE (U"An interactive user interface displays “Appropriate Legal Notices” "
	"to the extent that it includes a convenient and prominently visible "
	"feature that (1) displays an appropriate copyright notice, and (2) "
	"tells the user that there is no warranty for the work (except to the "
	"extent that warranties are provided), that licensees may convey the "
	"work under this License, and how to view a copy of this License.  If "
	"the interface presents a list of user commands or options, such as a "
	"menu, a prominent item in the list meets this criterion.")
CODE (U"")
CODE (U"1. Source Code.")
CODE (U"")
CODE (U"The “source code” for a work means the preferred form of the work "
	"for making modifications to it.  “Object code” means any non-source "
	"form of a work.")
CODE (U"")
CODE (U"A “Standard Interface” means an interface that either is an official "
	"standard defined by a recognized standards body, or, in the case of "
	"interfaces specified for a particular programming language, one that "
	"is widely used among developers working in that language.")
CODE (U"")
CODE (U"The “System Libraries” of an executable work include anything, other "
	"than the work as a whole, that (a) is included in the normal form of "
	"packaging a Major Component, but which is not part of that Major "
	"Component, and (b) serves only to enable use of the work with that "
	"Major Component, or to implement a Standard Interface for which an "
	"implementation is available to the public in source code form.  A "
	"“Major Component”, in this context, means a major essential component "
	"(kernel, window system, and so on) of the specific operating system "
	"(if any) on which the executable work runs, or a compiler used to "
	"produce the work, or an object code interpreter used to run it.")
CODE (U"")
CODE (U"The “Corresponding Source” for a work in object code form means all "
	"the source code needed to generate, install, and (for an executable "
	"work) run the object code and to modify the work, including scripts to "
	"control those activities.  However, it does not include the work's "
	"System Libraries, or general-purpose tools or generally available free "
	"programs which are used unmodified in performing those activities but "
	"which are not part of the work.  For example, Corresponding Source "
	"includes interface definition files associated with source files for "
	"the work, and the source code for shared libraries and dynamically "
	"linked subprograms that the work is specifically designed to require, "
	"such as by intimate data communication or control flow between those "
	"subprograms and other parts of the work.")
CODE (U"")
CODE (U"The Corresponding Source need not include anything that users "
	"can regenerate automatically from other parts of the Corresponding "
	"Source.")
CODE (U"")
CODE (U"The Corresponding Source for a work in source code form is that "
	"same work.")
CODE (U"")
CODE (U"2. Basic Permissions.")
CODE (U"")
CODE (U"All rights granted under this License are granted for the term of "
	"copyright on the Program, and are irrevocable provided the stated "
	"conditions are met.  This License explicitly affirms your unlimited "
	"permission to run the unmodified Program.  The output from running a "
	"covered work is covered by this License only if the output, given its "
	"content, constitutes a covered work.  This License acknowledges your "
	"rights of fair use or other equivalent, as provided by copyright law.")
CODE (U"")
CODE (U"You may make, run and propagate covered works that you do not "
	"convey, without conditions so long as your license otherwise remains "
	"in force.  You may convey covered works to others for the sole purpose "
	"of having them make modifications exclusively for you, or provide you "
	"with facilities for running those works, provided that you comply with "
	"the terms of this License in conveying all material for which you do "
	"not control copyright.  Those thus making or running the covered works "
	"for you must do so exclusively on your behalf, under your direction "
	"and control, on terms that prohibit them from making any copies of "
	"your copyrighted material outside their relationship with you.")
CODE (U"")
CODE (U"Conveying under any other circumstances is permitted solely under "
	"the conditions stated below.  Sublicensing is not allowed; section 10 "
	"makes it unnecessary.")
CODE (U"")
CODE (U"3. Protecting Users’ Legal Rights From Anti-Circumvention Law.")
CODE (U"")
CODE (U"No covered work shall be deemed part of an effective technological "
	"measure under any applicable law fulfilling obligations under article "
	"11 of the WIPO copyright treaty adopted on 20 December 1996, or "
	"similar laws prohibiting or restricting circumvention of such "
	"measures.")
CODE (U"")
CODE (U"When you convey a covered work, you waive any legal power to forbid "
	"circumvention of technological measures to the extent such circumvention "
	"is effected by exercising rights under this License with respect to "
	"the covered work, and you disclaim any intention to limit operation or "
	"modification of the work as a means of enforcing, against the work's "
	"users, your or third parties' legal rights to forbid circumvention of "
	"technological measures.")
CODE (U"")
CODE (U"4. Conveying Verbatim Copies.")
CODE (U"")
CODE (U"You may convey verbatim copies of the Program's source code as you "
	"receive it, in any medium, provided that you conspicuously and "
	"appropriately publish on each copy an appropriate copyright notice; "
	"keep intact all notices stating that this License and any "
	"non-permissive terms added in accord with section 7 apply to the code; "
	"keep intact all notices of the absence of any warranty; and give all "
	"recipients a copy of this License along with the Program.")
CODE (U"")
CODE (U"You may charge any price or no price for each copy that you convey, "
	"and you may offer support or warranty protection for a fee.")
CODE (U"")
CODE (U"5. Conveying Modified Source Versions.")
CODE (U"")
CODE (U"You may convey a work based on the Program, or the modifications to "
	"produce it from the Program, in the form of source code under the "
	"terms of section 4, provided that you also meet all of these conditions:")
CODE (U"")
	CODE1 (U"a) The work must carry prominent notices stating that you modified "
		"it, and giving a relevant date.")
	CODE1 (U"")
	CODE1 (U"b) The work must carry prominent notices stating that it is "
		"released under this License and any conditions added under section "
		"7.  This requirement modifies the requirement in section 4 to "
		"“keep intact all notices”.")
	CODE1 (U"")
	CODE1 (U"c) You must license the entire work, as a whole, under this "
		"License to anyone who comes into possession of a copy.  This "
		"License will therefore apply, along with any applicable section 7 "
		"additional terms, to the whole of the work, and all its parts, "
		"regardless of how they are packaged.  This License gives no "
		"permission to license the work in any other way, but it does not "
		"invalidate such permission if you have separately received it.")
	CODE1 (U"")
	CODE1 (U"d) If the work has interactive user interfaces, each must display "
		"Appropriate Legal Notices; however, if the Program has interactive "
		"interfaces that do not display Appropriate Legal Notices, your "
		"work need not make them do so.")
CODE (U"")
CODE (U"A compilation of a covered work with other separate and independent "
	"works, which are not by their nature extensions of the covered work, "
	"and which are not combined with it such as to form a larger program, "
	"in or on a volume of a storage or distribution medium, is called an "
	"“aggregate” if the compilation and its resulting copyright are not "
	"used to limit the access or legal rights of the compilation's users "
	"beyond what the individual works permit.  Inclusion of a covered work "
	"in an aggregate does not cause this License to apply to the other "
	"parts of the aggregate.")
CODE (U"")
CODE (U"6. Conveying Non-Source Forms.")
CODE (U"")
CODE (U"You may convey a covered work in object code form under the terms "
	"of sections 4 and 5, provided that you also convey the "
	"machine-readable Corresponding Source under the terms of this License, "
	"in one of these ways:")
CODE (U"")
	CODE1 (U"a) Convey the object code in, or embodied in, a physical product "
		"(including a physical distribution medium), accompanied by the "
		"Corresponding Source fixed on a durable physical medium "
		"customarily used for software interchange.")
	CODE1 (U"")
	CODE1 (U"b) Convey the object code in, or embodied in, a physical product "
		"(including a physical distribution medium), accompanied by a "
		"written offer, valid for at least three years and valid for as "
		"long as you offer spare parts or customer support for that product "
		"model, to give anyone who possesses the object code either (1) a "
		"copy of the Corresponding Source for all the software in the "
		"product that is covered by this License, on a durable physical "
		"medium customarily used for software interchange, for a price no "
		"more than your reasonable cost of physically performing this "
		"conveying of source, or (2) access to copy the "
		"Corresponding Source from a network server at no charge.")
	CODE1 (U"")
	CODE1 (U"c) Convey individual copies of the object code with a copy of the "
		"written offer to provide the Corresponding Source.  This "
		"alternative is allowed only occasionally and noncommercially, and "
		"only if you received the object code with such an offer, in accord "
		"with subsection 6b.")
	CODE1 (U"")
	CODE1 (U"d) Convey the object code by offering access from a designated "
		"place (gratis or for a charge), and offer equivalent access to the "
		"Corresponding Source in the same way through the same place at no "
		"further charge.  You need not require recipients to copy the "
		"Corresponding Source along with the object code.  If the place to "
		"copy the object code is a network server, the Corresponding Source "
		"may be on a different server (operated by you or a third party) "
		"that supports equivalent copying facilities, provided you maintain "
		"clear directions next to the object code saying where to find the "
		"Corresponding Source.  Regardless of what server hosts the "
		"Corresponding Source, you remain obligated to ensure that it is "
		"available for as long as needed to satisfy these requirements.")
	CODE1 (U"")
	CODE1 (U"e) Convey the object code using peer-to-peer transmission, provided "
		"you inform other peers where the object code and Corresponding "
		"Source of the work are being offered to the general public at no "
		"charge under subsection 6d.")
CODE (U"")
CODE (U"A separable portion of the object code, whose source code is excluded "
	"from the Corresponding Source as a System Library, need not be "
	"included in conveying the object code work.")
CODE (U"")
CODE (U"A “User Product” is either (1) a “consumer product”, which means any "
	"tangible personal property which is normally used for personal, family, "
	"or household purposes, or (2) anything designed or sold for incorporation "
	"into a dwelling.  In determining whether a product is a consumer product, "
	"doubtful cases shall be resolved in favor of coverage.  For a particular "
	"product received by a particular user, “normally used” refers to a "
	"typical or common use of that class of product, regardless of the status "
	"of the particular user or of the way in which the particular user "
	"actually uses, or expects or is expected to use, the product.  A product "
	"is a consumer product regardless of whether the product has substantial "
	"commercial, industrial or non-consumer uses, unless such uses represent "
	"the only significant mode of use of the product.")
CODE (U"")
CODE (U"“Installation Information” for a User Product means any methods, "
	"procedures, authorization keys, or other information required to install "
	"and execute modified versions of a covered work in that User Product from "
	"a modified version of its Corresponding Source.  The information must "
	"suffice to ensure that the continued functioning of the modified object "
	"code is in no case prevented or interfered with solely because "
	"modification has been made.")
CODE (U"")
CODE (U"If you convey an object code work under this section in, or with, or "
	"specifically for use in, a User Product, and the conveying occurs as "
	"part of a transaction in which the right of possession and use of the "
	"User Product is transferred to the recipient in perpetuity or for a "
	"fixed term (regardless of how the transaction is characterized), the "
	"Corresponding Source conveyed under this section must be accompanied "
	"by the Installation Information.  But this requirement does not apply "
	"if neither you nor any third party retains the ability to install "
	"modified object code on the User Product (for example, the work has "
	"been installed in ROM).")
CODE (U"")
CODE (U"The requirement to provide Installation Information does not include a "
	"requirement to continue to provide support service, warranty, or updates "
	"for a work that has been modified or installed by the recipient, or for "
	"the User Product in which it has been modified or installed.  Access to a "
	"network may be denied when the modification itself materially and "
	"adversely affects the operation of the network or violates the rules and "
	"protocols for communication across the network.")
CODE (U"")
CODE (U"Corresponding Source conveyed, and Installation Information provided, "
	"in accord with this section must be in a format that is publicly "
	"documented (and with an implementation available to the public in "
	"source code form), and must require no special password or key for "
	"unpacking, reading or copying.")
CODE (U"")
CODE (U"7. Additional Terms.")
CODE (U"")
CODE (U"“Additional permissions” are terms that supplement the terms of this "
	"License by making exceptions from one or more of its conditions. "
	"Additional permissions that are applicable to the entire Program shall "
	"be treated as though they were included in this License, to the extent "
	"that they are valid under applicable law.  If additional permissions "
	"apply only to part of the Program, that part may be used separately "
	"under those permissions, but the entire Program remains governed by "
	"this License without regard to the additional permissions.")
CODE (U"")
CODE (U"When you convey a copy of a covered work, you may at your option "
	"remove any additional permissions from that copy, or from any part of "
	"it.  (Additional permissions may be written to require their own "
	"removal in certain cases when you modify the work.)  You may place "
	"additional permissions on material, added by you to a covered work, "
	"for which you have or can give appropriate copyright permission.")
CODE (U"")
CODE (U"Notwithstanding any other provision of this License, for material you "
	"add to a covered work, you may (if authorized by the copyright holders of "
	"that material) supplement the terms of this License with terms:")
CODE (U"")
	CODE1 (U"a) Disclaiming warranty or limiting liability differently from the "
		"terms of sections 15 and 16 of this License; or")
	CODE1 (U"")
	CODE1 (U"b) Requiring preservation of specified reasonable legal notices or "
		"author attributions in that material or in the Appropriate Legal "
		"Notices displayed by works containing it; or")
	CODE1 (U"")
	CODE1 (U"c) Prohibiting misrepresentation of the origin of that material, or "
		"requiring that modified versions of such material be marked in "
		"reasonable ways as different from the original version; or")
	CODE1 (U"")
	CODE1 (U"d) Limiting the use for publicity purposes of names of licensors or "
		"authors of the material; or")
	CODE1 (U"")
	CODE1 (U"e) Declining to grant rights under trademark law for use of some "
		"trade names, trademarks, or service marks; or")
	CODE1 (U"")
	CODE1 (U"f) Requiring indemnification of licensors and authors of that "
		"material by anyone who conveys the material (or modified versions of "
		"it) with contractual assumptions of liability to the recipient, for "
		"any liability that these contractual assumptions directly impose on "
		"those licensors and authors.")
CODE (U"")
CODE (U"All other non-permissive additional terms are considered “further "
	"restrictions” within the meaning of section 10.  If the Program as you "
	"received it, or any part of it, contains a notice stating that it is "
	"governed by this License along with a term that is a further "
	"restriction, you may remove that term.  If a license document contains "
	"a further restriction but permits relicensing or conveying under this "
	"License, you may add to a covered work material governed by the terms "
	"of that license document, provided that the further restriction does "
	"not survive such relicensing or conveying.")
CODE (U"")
CODE (U"If you add terms to a covered work in accord with this section, you "
	"must place, in the relevant source files, a statement of the "
	"additional terms that apply to those files, or a notice indicating "
	"where to find the applicable terms.")
CODE (U"")
CODE (U"Additional terms, permissive or non-permissive, may be stated in the "
	"form of a separately written license, or stated as exceptions; "
	"the above requirements apply either way.")
CODE (U"")
CODE (U"8. Termination.")
CODE (U"")
CODE (U"You may not propagate or modify a covered work except as expressly "
	"provided under this License.  Any attempt otherwise to propagate or "
	"modify it is void, and will automatically terminate your rights under "
	"this License (including any patent licenses granted under the third "
	"paragraph of section 11).")
CODE (U"")
CODE (U"However, if you cease all violation of this License, then your "
	"license from a particular copyright holder is reinstated (a) "
	"provisionally, unless and until the copyright holder explicitly and "
	"finally terminates your license, and (b) permanently, if the copyright "
	"holder fails to notify you of the violation by some reasonable means "
	"prior to 60 days after the cessation.")
CODE (U"")
CODE (U"Moreover, your license from a particular copyright holder is "
	"reinstated permanently if the copyright holder notifies you of the "
	"violation by some reasonable means, this is the first time you have "
	"received notice of violation of this License (for any work) from that "
	"copyright holder, and you cure the violation prior to 30 days after "
	"your receipt of the notice.")
CODE (U"")
CODE (U"Termination of your rights under this section does not terminate the "
	"licenses of parties who have received copies or rights from you under "
	"this License.  If your rights have been terminated and not permanently "
	"reinstated, you do not qualify to receive new licenses for the same "
	"material under section 10.")
CODE (U"")
CODE (U"9. Acceptance Not Required for Having Copies.")
CODE (U"")
CODE (U"You are not required to accept this License in order to receive or "
	"run a copy of the Program.  Ancillary propagation of a covered work "
	"occurring solely as a consequence of using peer-to-peer transmission "
	"to receive a copy likewise does not require acceptance.  However, "
	"nothing other than this License grants you permission to propagate or "
	"modify any covered work.  These actions infringe copyright if you do "
	"not accept this License.  Therefore, by modifying or propagating a "
	"covered work, you indicate your acceptance of this License to do so.")
CODE (U"")
CODE (U"10. Automatic Licensing of Downstream Recipients.")
CODE (U"")
CODE (U"Each time you convey a covered work, the recipient automatically "
	"receives a license from the original licensors, to run, modify and "
	"propagate that work, subject to this License.  You are not responsible "
	"for enforcing compliance by third parties with this License.")
CODE (U"")
CODE (U"An “entity transaction” is a transaction transferring control of an "
	"organization, or substantially all assets of one, or subdividing an "
	"organization, or merging organizations.  If propagation of a covered "
	"work results from an entity transaction, each party to that "
	"transaction who receives a copy of the work also receives whatever "
	"licenses to the work the party’s predecessor in interest had or could "
	"give under the previous paragraph, plus a right to possession of the "
	"Corresponding Source of the work from the predecessor in interest, if "
	"the predecessor has it or can get it with reasonable efforts.")
CODE (U"")
CODE (U"You may not impose any further restrictions on the exercise of the "
	"rights granted or affirmed under this License.  For example, you may "
	"not impose a license fee, royalty, or other charge for exercise of "
	"rights granted under this License, and you may not initiate litigation "
	"(including a cross-claim or counterclaim in a lawsuit) alleging that "
	"any patent claim is infringed by making, using, selling, offering for "
	"sale, or importing the Program or any portion of it.")
CODE (U"")
CODE (U"11. Patents.")
CODE (U"")
CODE (U"A “contributor” is a copyright holder who authorizes use under this "
	"License of the Program or a work on which the Program is based.  The "
	"work thus licensed is called the contributor’s “contributor version”.")
CODE (U"")
CODE (U"A contributor’s “essential patent claims” are all patent claims "
	"owned or controlled by the contributor, whether already acquired or "
	"hereafter acquired, that would be infringed by some manner, permitted "
	"by this License, of making, using, or selling its contributor version, "
	"but do not include claims that would be infringed only as a "
	"consequence of further modification of the contributor version.  For "
	"purposes of this definition, “control” includes the right to grant "
	"patent sublicenses in a manner consistent with the requirements of "
	"this License.")
CODE (U"")
CODE (U"Each contributor grants you a non-exclusive, worldwide, royalty-free "
	"patent license under the contributor's essential patent claims, to "
	"make, use, sell, offer for sale, import and otherwise run, modify and "
	"propagate the contents of its contributor version.")
CODE (U"")
CODE (U"In the following three paragraphs, a “patent license” is any express "
	"agreement or commitment, however denominated, not to enforce a patent "
	"(such as an express permission to practice a patent or covenant not to "
	"sue for patent infringement).  To “grant” such a patent license to a "
	"party means to make such an agreement or commitment not to enforce a "
	"patent against the party.")
CODE (U"")
CODE (U"If you convey a covered work, knowingly relying on a patent license, "
	"and the Corresponding Source of the work is not available for anyone "
	"to copy, free of charge and under the terms of this License, through a "
	"publicly available network server or other readily accessible means, "
	"then you must either (1) cause the Corresponding Source to be so "
	"available, or (2) arrange to deprive yourself of the benefit of the "
	"patent license for this particular work, or (3) arrange, in a manner "
	"consistent with the requirements of this License, to extend the patent "
	"license to downstream recipients.  “Knowingly relying” means you have "
	"actual knowledge that, but for the patent license, your conveying the "
	"covered work in a country, or your recipient’s use of the covered work "
	"in a country, would infringe one or more identifiable patents in that "
	"country that you have reason to believe are valid.")
CODE (U"")
CODE (U"If, pursuant to or in connection with a single transaction or "
	"arrangement, you convey, or propagate by procuring conveyance of, a "
	"covered work, and grant a patent license to some of the parties "
	"receiving the covered work authorizing them to use, propagate, modify "
	"or convey a specific copy of the covered work, then the patent license "
	"you grant is automatically extended to all recipients of the covered "
	"work and works based on it.")
CODE (U"")
CODE (U"A patent license is “discriminatory” if it does not include within "
	"the scope of its coverage, prohibits the exercise of, or is "
	"conditioned on the non-exercise of one or more of the rights that are "
	"specifically granted under this License.  You may not convey a covered "
	"work if you are a party to an arrangement with a third party that is "
	"in the business of distributing software, under which you make payment "
	"to the third party based on the extent of your activity of conveying "
	"the work, and under which the third party grants, to any of the "
	"parties who would receive the covered work from you, a discriminatory "
	"patent license (a) in connection with copies of the covered work "
	"conveyed by you (or copies made from those copies), or (b) primarily "
	"for and in connection with specific products or compilations that "
	"contain the covered work, unless you entered into that arrangement, "
	"or that patent license was granted, prior to 28 March 2007.")
CODE (U"")
CODE (U"Nothing in this License shall be construed as excluding or limiting "
	"any implied license or other defenses to infringement that may "
	"otherwise be available to you under applicable patent law.")
CODE (U"")
CODE (U"12. No Surrender of Others' Freedom.")
CODE (U"")
CODE (U"If conditions are imposed on you (whether by court order, agreement or "
	"otherwise) that contradict the conditions of this License, they do not "
	"excuse you from the conditions of this License.  If you cannot convey a "
	"covered work so as to satisfy simultaneously your obligations under this "
	"License and any other pertinent obligations, then as a consequence you may "
	"not convey it at all.  For example, if you agree to terms that obligate you "
	"to collect a royalty for further conveying from those to whom you convey "
	"the Program, the only way you could satisfy both those terms and this "
	"License would be to refrain entirely from conveying the Program.")
CODE (U"")
CODE (U"13. Use with the GNU Affero General Public License.")
CODE (U"")
CODE (U"Notwithstanding any other provision of this License, you have "
	"permission to link or combine any covered work with a work licensed "
	"under version 3 of the GNU Affero General Public License into a single "
	"combined work, and to convey the resulting work.  The terms of this "
	"License will continue to apply to the part which is the covered work, "
	"but the special requirements of the GNU Affero General Public License, "
	"section 13, concerning interaction through a network will apply to the "
	"combination as such.")
CODE (U"")
CODE (U"14. Revised Versions of this License.")
CODE (U"")
CODE (U"The Free Software Foundation may publish revised and/or new versions of "
	"the GNU General Public License from time to time.  Such new versions will "
	"be similar in spirit to the present version, but may differ in detail to "
	"address new problems or concerns.")
CODE (U"")
CODE (U"Each version is given a distinguishing version number.  If the "
	"Program specifies that a certain numbered version of the GNU General "
	"Public License “or any later version” applies to it, you have the "
	"option of following the terms and conditions either of that numbered "
	"version or of any later version published by the Free Software "
	"Foundation.  If the Program does not specify a version number of the "
	"GNU General Public License, you may choose any version ever published "
	"by the Free Software Foundation.")
CODE (U"")
CODE (U"If the Program specifies that a proxy can decide which future "
	"versions of the GNU General Public License can be used, that proxy's "
	"public statement of acceptance of a version permanently authorizes you "
	"to choose that version for the Program.")
CODE (U"")
CODE (U"Later license versions may give you additional or different "
	"permissions.  However, no additional obligations are imposed on any "
	"author or copyright holder as a result of your choosing to follow a "
	"later version.")
CODE (U"")
CODE (U"15. Disclaimer of Warranty.")
CODE (U"")
CODE (U"THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY "
	"APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT "
	"HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY "
	"OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, "
	"THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR "
	"PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM "
	"IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF "
	"ALL NECESSARY SERVICING, REPAIR OR CORRECTION.")
CODE (U"")
CODE (U"16. Limitation of Liability.")
CODE (U"")
CODE (U"IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING "
	"WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS "
	"THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY "
	"GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE "
	"USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF "
	"DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD "
	"PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), "
	"EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF "
	"SUCH DAMAGES.")
CODE (U"")
CODE (U"17. Interpretation of Sections 15 and 16.")
CODE (U"")
CODE (U"If the disclaimer of warranty and limitation of liability provided "
	"above cannot be given local legal effect according to their terms, "
	"reviewing courts shall apply local law that most closely approximates "
	"an absolute waiver of all civil liability in connection with the "
	"Program, unless a warranty or assumption of liability accompanies a "
	"copy of the Program in return for a fee.")
CODE (U"")
CODE (U"                     END OF TERMS AND CONDITIONS")
CODE (U"")
CODE (U"            How to Apply These Terms to Your New Programs")
CODE (U"")
CODE (U"If you develop a new program, and you want it to be of the greatest "
	"possible use to the public, the best way to achieve this is to make it "
	"free software which everyone can redistribute and change under these terms.")
CODE (U"")
CODE (U"To do so, attach the following notices to the program.  It is safest "
	"to attach them to the start of each source file to most effectively "
	"state the exclusion of warranty; and each file should have at least "
	"the “copyright” line and a pointer to where the full notice is found.")
CODE (U"")
	CODE1 (U"##<one line to give the program’s name and a brief idea of what it does.>")
	CODE1 (U"##Copyright (C) <year>  <name of author>")
	CODE1 (U"")
	CODE1 (U"##This program is free software: you can redistribute it and/or modify "
		"it under the terms of the GNU General Public License as published by "
		"the Free Software Foundation, either version 3 of the License, or "
		"(at your option) any later version.")
	CODE1 (U"")
	CODE1 (U"##This program is distributed in the hope that it will be useful, "
		"but WITHOUT ANY WARRANTY; without even the implied warranty of "
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
		"GNU General Public License for more details.")
	CODE1 (U"")
	CODE1 (U"##You should have received a copy of the GNU General Public License "
		"along with this program.  If not, see <https://www.gnu.org/licenses/>.")
	CODE1 (U"")
	CODE1 (U"Also add information on how to contact you by electronic and paper mail.")
CODE (U"")
CODE (U"If the program does terminal interaction, make it output a short "
	"notice like this when it starts in an interactive mode:")
CODE (U"")
	CODE1 (U"##<program>  Copyright (C) <year>  <name of author>")
	CODE1 (U"##This program comes with ABSOLUTELY NO WARRANTY; for details type ‘show w’.")
	CODE1 (U"##This is free software, and you are welcome to redistribute it")
	CODE1 (U"##under certain conditions; type ‘show c’ for details.")
	CODE1 (U"")
	CODE1 (U"The hypothetical commands ‘show w’ and ‘show c’ should show the appropriate "
		"parts of the General Public License.  Of course, your program’s commands "
		"might be different; for a GUI interface, you would use an “about box”.")
CODE (U"")
CODE (U"You should also get your employer (if you work as a programmer) or school, "
	"if any, to sign a “copyright disclaimer” for the program, if necessary. "
	"For more information on this, and how to apply and follow the GNU GPL, see "
	"<https://www.gnu.org/licenses/>.")
CODE (U"")
CODE (U"The GNU General Public License does not permit incorporating your program "
	"into proprietary programs.  If your program is a subroutine library, you "
	"may consider it more useful to permit linking proprietary applications with "
	"the library.  If this is what you want to do, use the GNU Lesser General "
	"Public License instead of this License.  But first, please read "
	"<https://www.gnu.org/licenses/why-not-lgpl.html>.")
MAN_END

MAN_BEGIN (U"Privacy and security", U"ppgb", 20221109)
INTRO (U"Praat is an “isolated” app. You download it from praat.org, "
	"then record sounds into Praat (all in RAM) or open a sound file, "
	"then analyse or manipulate that sound. The only way in which your results "
	"are saved to disk (as e.g. a Pitch file, a TextGrid file, or a sound file), "
	"is when you explicitly choose one of the #Save or #Export commands "
	"from Praat’s menus; Praat will not by itself save any data files to disk "
	"or send any information anywhere. When you create a picture in the Picture window, "
	"the only way to move that picture anywhere else is if you save it explicitly "
	"to a picture file (e.g. PNG) or if you Copy–Paste it to e.g. a text editing "
	"app such as e.g. Microsoft Word; Praat will not by itself save any picture to disk "
	"or to the clipboard or send any information anywhere. "
	"Praat will run just fine on your computer if it does not have Internet access, "
	"and in fact Praat cannot even notice whether you are in a network or not. "
	"Praat works entirely stand-alone.")
ENTRY (U"Praat does not call home")
NORMAL (U"When you are using Praat, you can be assured that Praat does not attempt to send any of your data "
	"or pictures or settings to the Praat team. "
	"In fact, Praat never accesses the Internet, not even to @@checking for updates|check for updates@.")
ENTRY (U"No telemetry")
NORMAL (U"Praat does not send anything to the Praat team while you are using Praat:")
LIST_ITEM (U"\\bu No surveillance")
LIST_ITEM (U"\\bu No tracking")
LIST_ITEM (U"\\bu No Google Analytics")
LIST_ITEM (U"\\bu In general, no spying or data mining by the Praat team")
ENTRY (U"What does Praat save to disk without asking you?")
NORMAL (U"Praat will save your preferences to your own disk on your own computer, "
	"in a folder of your own, when you close Praat. "
	"This includes the settings in your Sound window (e.g. your last chosen Pitch range), "
	"so that your Sound windows will look the same after you start Praat up again. "
	"The goal of this is to provide a continuous user experience, and is what you probably expect, "
	"because most apps that you use on your computer work this way.")
ENTRY (U"What we do measure")
NORMAL (U"As mentioned above, Praat does no telemetry, i.e. it does not send us anything while you are using Praat. "
	"We %do% receive some information, though, when %you contact %us. This happens when you download "
	"a new Praat version for your computer. We log the Praat downloads, so that we can potentially count "
	"how often which edition and which version of Praat is downloaded.")
ENTRY (U"Wouldn’t telemetry be useful for the quality of Praat?")
NORMAL (U"Companies that use telemetry tend to justify that by arguing that gathering information on how their app is used "
	"is useful for improving the quality of their app (by collecting error messages), "
	"or to know which features are rarely used, so that those features can be removed.")
NORMAL (U"We are skeptical. If we, as Praat developers, have made a programming error, "
	"then we hope that an “assertion” will help solve the issue. "
	"An assertion is a place in our code where Praat will crash if a certain assumption "
	"is not met. A message window will pop up in Praat that says that Praat will crash, together "
	"with the request to send some relevant information by email to us, the developers of Praat. "
	"If you do send this crash information on to us (you can read it, as it is normal English without secrets), "
	"we will then virtually always find out (sometimes with some more help from you, "
	"such as the sound file or script that caused the crash) "
	"what was wrong, and correct the mistake, so that our programming error (“bug”) no longer occurs "
	"in the next version of Praat. We will also build an automatable test that checks, for all future "
	"versions of Praat, that the bug does not reappear. In this way, every Praat version tends to be more stable and correct "
	"than the previous version. We believe that this practice minimizes the problems with Praat sufficiently, "
	"and no automated reporting of error messages and crash messages is necessary.")
NORMAL (U"As for the removal of obsolete features, we are just very conservative. "
	"Typically, file types from the 1980s and 1990s can typically still be opened in the 2020s, "
	"and old Praat scripts should continue to run for at least 15 years after we marked a language feature "
	"as “deprecated” or “obsolete” (and removed it from the manual). "
	"This has not prevented us from also being able to open file types invented in the 2020s "
	"or to have a modern scripting language that supports vectors, matrices and string arrays, "
	"and backward compatibility hardly hampers the continual modernization of Praat.")
ENTRY (U"Praat scripts and plug-ins")
NORMAL (U"As with R scripts, Python scripts, and quite generally any kinds of scripts from any source, "
	"you should consider Praat scripts written by others, such as plug-ins that you download, "
	"as separate apps with their own privacy and security issues. Use a script or plug-in only "
	"if you completely trust that script or plug-in and its creators.")
MAN_END

MAN_BEGIN (U"Checking for updates", U"ppgb", 20220217)
INTRO (U"Updates for Praat are available from `www.praat.org`.")
NORMAL (U"Your current version (if you are reading this from the manual inside the Praat program, rather than from the website) is " stringize(PRAAT_VERSION_STR)
	", from " stringize(PRAAT_MONTH) " " stringize(PRAAT_DAY) ", " stringize(PRAAT_YEAR) ". "
	"Given that we tend to release new Praat versions once or twice a month, "
	"you can probably guess whether it would be worth your while to have a look at `www.praat.org` "
	"to see what is new, and perhaps download a new version.")
NORMAL (U"Praat improves continually, and old features will almost always continue to work, "
	"so there should never be a reason to continue to work with older versions.")
ENTRY (U"Why no automatic update checking?")
NORMAL (U"Many apps automatically check for updates when you start them up. "
	"This means that the owners of such an app are capable of recording which users use their app when, "
	"which is information that can potentially harm your privacy, for instance when a government "
	"or legal investigation demands that the app owners provide them with access to such information. "
	"The Praat team wants to stay far away from the possibility of such situations occurring, "
	"even if you may be convinced that usage of the Praat program cannot be regarded by anybody "
	"as being anything other than perfectly innocent. For this resason, the Praat program "
	"will never contact the Praat team and, more generally, "
	"will never attempt to access the Internet by itself. For more information, see @@Privacy and security@.")
MAN_END

MAN_PAGES_BEGIN R"~~~(
"Reporting a problem"
© Paul Boersma 2022-01-16

Anything that you consider incorrect behaviour of Praat (a “bug”) can be reported
to the authors by email (`paul.boersma@uva.nl`). This includes any crashes.

Questions about how to use Praat for your specific cases
can be posed to the Praat User List (`https://groups.io/g/Praat-Users-List`).
This includes any questions on why your self-written Praat script does something unexpected.
)~~~" MAN_PAGES_END

MAN_BEGIN (U"Praat menu", U"ppgb", 20161227)
INTRO (U"The first menu in the @@Objects window@. "
	"In macOS, this menu is in the main menu bar along the top of the screen.")
MAN_END

MAN_BEGIN (U"Analyses menu", U"ppgb", 20221202)
INTRO (U"A menu in the @SoundEditor or @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Copy...", U"ppgb", 20111018)
INTRO (U"One of the fixed buttons in the @@Objects window@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one object of any type.")
ENTRY (U"Behaviour")
NORMAL (U"The Objects window copies the selected object, and all the data it contains, "
	"to a new object, which will appear at the bottom of the List of Objects.")
ENTRY (U"Example")
NORMAL (U"If you select “Sound hallo” and click ##Copy...#, "
	"a command window will appear, which prompts you for a name; "
	"after you click #OK, a new object will appear in the list, bearing that name.")
MAN_END

MAN_BEGIN (U"Draw menu", U"ppgb", 20010417)
INTRO (U"A menu that occurs in the @@Dynamic menu@ for many objects.")
NORMAL (U"This menu contains commands for drawing the object to the @@Picture window@, "
	"which will allow you to print the drawing or to copy it to your word processor.")
MAN_END

MAN_BEGIN (U"Dynamic menu", U"ppgb", 20110131)  // 2023-06-08
INTRO (U"A column of buttons in the right-hand part of the @@Objects window@, "
	"plus the @@Save menu@ in the Objects window.")
NORMAL (U"If you select one or more @objects in the list, "
	"the possible actions that you can perform with the selected objects "
	"will appear in the dynamic menu. "
	"These actions can include viewing & editing, saving, drawing, "
	"conversions to other types (including analysis and synthesis), and more.")
ENTRY (U"Example of analysis:")
NORMAL (U"Record a Sound, select it, and click ##To Pitch...#. "
	"This will create a new Pitch object and put it in the list of objects. "
	"You can then edit, write, and draw this Pitch object.")
ENTRY (U"Example of synthesis:")
NORMAL (U"Create a Speaker, create and edit an Artword, and click ##To Sound...#.")
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
NORMAL (U"Solution: the “number of formants” in formant analysis determines the "
	"number of peaks with which the %entire spectrum is modelled. For an average "
	"female voice, you should choose to analyse 5 formants in the region up to 5500 Hz, "
	"even if you are interested only in the first three formants.")
NORMAL (U"")
NORMAL (U"#Problem: I often get only 1 formant in a region where I see clearly 2 formants "
	"in the spectrogram.")
NORMAL (U"This occurs mainly in back vowels (F1 and F2 close together) for male voices, "
	"if the “maximum formant” is set to the standard of 5500 Hz, which is appropriate "
	"for female voices. Set the “maximum formant” down to 5000 Hz. "
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
NORMAL (U"Boersma, Paul & Weenink, David (" stringize(PRAAT_YEAR) "). "
	"Praat: doing phonetics by computer [Computer program]. "
	"Version " stringize(PRAAT_VERSION_STR) ", retrieved " stringize(PRAAT_DAY) " " stringize(PRAAT_MONTH) " " stringize(PRAAT_YEAR) " from http://www.praat.org/")
NORMAL (U"If the journal does not allow you to cite a web site, then try:")
NORMAL (U"Boersma, Paul (2001). Praat, a system for doing phonetics by computer. "
	"%%Glot International% ##5:9/10#, 341-345.")
NORMAL (U"This paper can be downloaded from Boersma's website.")
MAN_END

MAN_BEGIN (U"FAQ: Pitch analysis", U"ppgb", 20221202)
NORMAL (U"#Question: what algorithm is used for pitch analysis?")
NORMAL (U"Answer: see @@Sound: To Pitch (ac)...@. The 1993 article is downloadable from "
	"https://www.fon.hum.uva.nl/paul/")
NORMAL (U"#Question: why does Praat consider my sound voiceless while I hear it as voiced?")
NORMAL (U"There are at least five possibilities. Most of them can be checked by zooming in on the @waveform.")
NORMAL (U"The first possibility is that the pitch has fallen below the @@pitch floor@. For instance, "
	"your pitch floor could be 75 Hz but the English speaker produces creak at the end of the utterance. "
	"Or your pitch floor could be 75 Hz but the Chinese speaker is in the middle of a third tone. "
	"If this happens, it may help to lower the pitch floor to e.g. 40 Hz (@@Pitch settings...@), "
	"although that may also smooth the pitch curve too much in other places.")
NORMAL (U"The second possibility is that the pitch has moved too fast. This could happen at the end of a Chinese fourth tone, "
	"which drops very fast. If this happens, it may help to use the ##optimize for voice analysis# setting, "
	"(@@Pitch settings...@), although Praat may then hallucinate pitches in other places that you would prefer to consider voiceless.")
NORMAL (U"The third possibility is that the periods are very irregular, as in some pathological voices. "
	"If you want to see a pitch in those cases, it may help to use the ##optimize for voice analysis# setting "
	"(@@Pitch settings...@). Or it may help to lower the ##voicing threshold# setting (@@Advanced pitch settings...@) "
	"to 0.25 (instead of the standard 0.45) or so.")
NORMAL (U"The fourth possibility is that there is a lot of background noise, as in a recording on a busy street. "
	"In such a case, it may help to lower the ##voicing threshold# setting (@@Advanced pitch settings...@) "
	"to 0.25 (instead of the standard 0.45) or so. The disadvantage of lowering this setting is that for non-noisy "
	"recordings, Praat will become too eager to find voicing in some places that you would prefer to consider voiceless; "
	"so make sure to set it back to 0.45 once you have finished analysing the noisy recordings.")
NORMAL (U"The fifth possibility is that the part analysed as voiceless is much less loud than the rest of the sound, "
	"or that the sound contains a loud noise elsewhere. This can be checked by zooming in on the part analysed as voiceless: "
	"if Praat suddenly considers it as voiced, this is a sign that this part is much quieter than the rest. "
	"To make Praat analyse this part as voiced, you can lower the ##silence threshold# setting to 0.01 "
	"(instead of the standard 0.03) or so. The disadvantage of lowering this setting is that Praat may start to consider "
	"some distant background sounds (and quiet echos, for instance) as voiced.")
NORMAL (U"#Question: why do I get different results for the maximum pitch if...?")
NORMAL (U"If you select a Sound and choose @@Sound: To Pitch...@, the time step will usually "
	"be 0.01 seconds. The resulting @Pitch object will have values for times that are "
	"0.01 seconds apart. If you then click Info or choose ##Get maximum pitch# from the @@Query submenu@, "
	"the result is based on those time points. By contrast, if you choose ##Get maximum pitch# "
	"from the @@Pitch menu@ in the SoundEditor window, the result will be based on the visible points, "
	"of which there tend to be a hundred in the visible window. These different time spacings will "
	"lead to slightly different pitch contours.")
NORMAL (U"If you choose ##Move cursor to maximum pitch#, then choose ##Get pitch# from the "
	"@@Pitch menu@, the result will be different again. This is because ##Get maximum pitch# "
	"can do a parabolic interpolation around the maximum, whereas ##Get pitch#, not realizing "
	"that the cursor is at a maximum, does a stupid linear interpolation, which tends to lead to "
	"lower values.")
MAN_END

MAN_BEGIN (U"FAQ: Scripts", U"ppgb", 20230201)
NORMAL (U"#Question: how do I do something to all the files in a directory?")
NORMAL (U"Answer: look at `fileNames$#` () or @@Create Strings as file list...@.")
NORMAL (U"")
NORMAL (U"#Question: why doesn't the editor window react to my commands?")
NORMAL (U"Your commands are probably something like:")
CODE (U"Read from file: “hello.wav”")
CODE (U"View & Edit")
CODE (U"Zoom: 0.3, 0.5")
NORMAL (U"Answer: Praat doesn’t know it has to send the #Zoom command to the editor "
	"window called ##14. Sound hello#. There could be several Sound editor windows on your "
	"screen. According to @@Scripting 7.1. Scripting an editor from a shell script@, "
	"you will have to say this explicitly:")
CODE (U"Read from file: “hello.wav”")
CODE (U"View & Edit")
CODE (U"editor: “Sound hello”")
CODE (U"Zoom: 0.3, 0.5")
NORMAL (U"")
NORMAL (U"#Problem: a line like “%`Number = 1`” does not work.")
NORMAL (U"Solution: names of variables should start with a lower-case letter.")
NORMAL (U"")
NORMAL (U"#Question: why do names of variables have to start with a lower-case letter? "
	"I would like to do things like “`F0 = Get mean pitch`”.")
NORMAL (U"Answer: Praat scripts combine button commands with things that only occur "
	"in scripts. Button commands always start with a capital letter, e.g. `Play` (there is a button #Play). "
	"Script commands always start with lower case, e.g. `writeInfoLine: “Hello”` "
	"($writeInfoLine is a built-in function). "
	"A minimal pair is “$select”, which simulates a mouse click in the object list, "
	"versus “`Select...`”, which sets the selection in editor windows. If we allowed initial capitals, "
	"variable names would become rather ambiguous in assignments, "
	"as in “`x = Get`”, where “%`Get`” would be a variable, versus “`x = Get mean`”, "
	"where ##Get mean# is a button command. To prevent this confusion, Praat enforces "
	"a rigorous lower-case/upper-case distinction.")
NORMAL (U"")
NORMAL (U"#Question: how do I convert a number into a string?")
NORMAL (U"Answer: `a$ = string$ (a)`")
NORMAL (U"#Question: how do I convert a string into a number?")
NORMAL (U"Answer: `a = number (a$)`")
MAN_END

MAN_BEGIN (U"FAQ: Spectrograms", U"ppgb", 20030916)
NORMAL (U"#Problem: the background is grey instead of white (too little contrast)")
NORMAL (U"Solution: reduce the ##Dynamic range# in the spectrogram settings. The standard value is 50 dB, "
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
NORMAL (U"And there are in-place versions, which modify the existing Sound objects:")
LIST_ITEM (U"• @@Sound: Filter with one formant (in-place)...@")
LIST_ITEM (U"• @@Sound: Pre-emphasize (in-place)...@")
LIST_ITEM (U"• @@Sound: De-emphasize (in-place)...@")
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

MAN_BEGIN (U"Formants menu", U"ppgb", 20221202)
INTRO (U"A menu in the @SoundEditor or @TextGridEditor.")
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

MAN_BEGIN (U"Get first formant", U"ppgb", 20221202)
INTRO (U"One of the query commands in the @@Formants menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Get pitch", U"ppgb", 20221202)
INTRO (U"One of the query commands in the @@Pitch menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Get second formant", U"ppgb", 20221202)
INTRO (U"One of the query commands in the @@Formants menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Goodies", U"ppgb", 20050822)
INTRO (U"The title of a submenu of the @@Praat menu@.")
MAN_END

MAN_BEGIN (U"Info", U"ppgb", 19980101)
INTRO (U"One of the fixed buttons in the @@Objects window@.")
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
INTRO (U"One of the fixed buttons in the @@Objects window@.")
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

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Intro"
© Paul Boersma, 2011-01-31

This is an introductory tutorial to Praat, a computer program
with which you can analyse, synthesize, and manipulate speech,
and create high-quality pictures for your articles and thesis.
You are advised to work through all of this tutorial.

You can read this tutorial sequentially with the help of the “##1 >#” and “##< 1#” buttons,
or go to the desired information by clicking on the blue links.

, @@Intro 1. How to get a sound@:
	@@Intro 1.1. Recording a sound|record@,
	@@Intro 1.2. Reading a sound from disk|read@,
	@@Intro 1.3. Creating a sound from a formula|formula@.
, @@Intro 2. What to do with a sound@:
	@@Intro 2.1. Saving a sound to disk|write@,
	@@Intro 2.2. Viewing and editing a sound|view@.
, @@Intro 3. Spectral analysis
	, spectrograms: @@Intro 3.1. Viewing a spectrogram|view@,
		@@Intro 3.2. Configuring the spectrogram|configure@,
		@@Intro 3.3. Querying the spectrogram|query@,
		@@Intro 3.4. Printing the spectrogram|print@,
		@@Intro 3.5. The Spectrogram object|the Spectrogram object@.
	, spectral slices: @@Intro 3.6. Viewing a spectral slice|view@,
		@@Intro 3.7. Configuring the spectral slice|configure@,
		@@Intro 3.8. The Spectrum object|the Spectrum object@.
, @@Intro 4. Pitch analysis
	, pitch contours: @@Intro 4.1. Viewing a pitch contour|view@,
		@@Intro 4.2. Configuring the pitch contour|configure@,
		@@Intro 4.3. Querying the pitch contour|query@,
		@@Intro 4.4. Printing the pitch contour|print@,
		@@Intro 4.5. The Pitch object|the Pitch object@.
, @@Intro 5. Formant analysis
	, formant contours: @@Intro 5.1. Viewing formant contours|view@,
		@@Intro 5.2. Configuring the formant contours|configure@,
		@@Intro 5.3. Querying the formant contours|query@,
		@@Intro 5.4. The Formant object|the Formant object@.
, @@Intro 6. Intensity analysis
	, intensity contours: @@Intro 6.1. Viewing an intensity contour|view@,
		@@Intro 6.2. Configuring the intensity contour|configure@,
		@@Intro 6.3. Querying the intensity contour|query@,
		@@Intro 6.4. The Intensity object|the Intensity object@.
, @@Intro 7. Annotation
, @@Intro 8. Manipulation@: of
	@@Intro 8.1. Manipulation of pitch|pitch@,
	@@Intro 8.2. Manipulation of duration|duration@,
	@@Intro 8.3. Manipulation of intensity|intensity@,
	@@Intro 8.4. Manipulation of formants|formants@.

There are also more specialized tutorials:
, Phonetics:
	• Voice analysis (jitter, shimmer, noise): @Voice
	• Listening experiments: @@ExperimentMFC@
	• @@Sound files@
	• @@Filtering@
	• @@Source-filter synthesis@
	• @@Articulatory synthesis@
, Learning:
	• @@Feedforward neural networks@
	• @@OT learning@
, Statistics:
	• @@Principal component analysis@
	• @@Multidimensional scaling@
	• @@Discriminant analysis@
, General:
	• @@Scripting@
	• @@Demo window@
	• @@Printing@

The authors
===========

The Praat program was created by Paul Boersma and David Weenink of
the Institute of Phonetics Sciences of the University of Amsterdam.
Home page: #`http://www.praat.org` or #`https://www.fon.hum.uva.nl/praat/`.

For questions and suggestions, mail to the Praat discussion list,
which is reachable from the Praat home page, or directly to #`paul.boersma@uva.nl`.

################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Intro 1. How to get a sound", U"ppgb", 20021212)
INTRO (U"Most of the things most people do with Praat start with a sound. "
	"There are at least three ways to get a sound:")
LIST_ITEM (U"@@Intro 1.1. Recording a sound")
LIST_ITEM (U"@@Intro 1.2. Reading a sound from disk")
LIST_ITEM (U"@@Intro 1.3. Creating a sound from a formula")
MAN_END

MAN_BEGIN (U"Intro 1.1. Recording a sound", U"ppgb", 20201120)
INTRO (U"To record a speech sound into Praat, you need a computer with a microphone.")
NORMAL (U"To record from the microphone, perform the following steps:")
LIST_ITEM (U"1. Choose @@Record mono Sound...@ from the @@New menu@ in the @@Objects window@. "
	"A @SoundRecorder window will appear on your screen.")
LIST_ITEM (U"2. On the left in the SoundRecorder window, choose the appropriate input device, e.g. choose ##Internal microphone#. "
	"(On Windows, instead right-click the loudspeaker symbol in the Start bar; see @SoundRecorder for more details.)")
LIST_ITEM (U"3. Use the #Record and #Stop buttons to record a few seconds of your speech.")
LIST_ITEM (U"4. Use the #Play button to hear what you have recorded.")
LIST_ITEM (U"5. Repeat steps 3 and 4 until you are satisfied with your recording.")
LIST_ITEM (U"6. Click the ##Save to list# button. Your recording will now appear in the Objects window, "
	"where it will be called “Sound sound”.")
LIST_ITEM (U"7. You can now close the SoundRecorder window.")
LIST_ITEM (U"8. When you saved your sound to the Objects window, some buttons appeared in that window. "
	"These buttons show you what you can do with the sound. Try the #Play and @@View & Edit@ buttons.")
NORMAL (U"For more information on recording, see the @SoundRecorder manual page.")
MAN_END

MAN_BEGIN (U"Intro 1.2. Reading a sound from disk", U"ppgb", 20041126)
INTRO (U"Apart from recording a new sound from a microphone, you could read an existing sound file from your disk.")
NORMAL (U"With @@Read from file...@ from the @@Open menu@, "
	"Praat will be able to read most standard types of sound files, e.g. WAV files. "
	"They will appear as @Sound objects in the Objects window. For instance, if you open the file ##hello.wav#, "
	"an object called “Sound hello” will appear in the list.")
NORMAL (U"If you do not have a sound file on your disk, you can download a WAV file (or so) from the Internet, "
	"then read that file into Praat with ##Read from file...#.")
MAN_END

MAN_BEGIN (U"Intro 1.3. Creating a sound from a formula", U"ppgb", 20070225)
INTRO (U"If you have no microphone, no sound files on disk, and no access to the Internet, "
	"you could still create a sound with @@Create Sound from formula...@ from the @@New menu@.")
MAN_END

MAN_BEGIN (U"Intro 2. What to do with a sound", U"ppgb", 20110131)
INTRO (U"As soon as you have a @Sound in the @@List of Objects@, "
	"the buttons in the @@Dynamic menu@ (the right-hand part of the @@Objects window@) "
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

MAN_BEGIN (U"Intro 2.2. Viewing and editing a sound", U"ppgb", 20230202)   // 2023-06-08
NORMAL (U"To see the wave form of a @Sound that is in the list of objects, "
	"select that Sound and click @@View & Edit@. A @SoundEditor window will appear on your screen. "
	"You see a waveform (or two waveforms, if you have a stereo sound) and probably some “analyses” below it. "
	"You can zoom in and scroll to see the various parts of the sound in detail. "
	"You can select a part of the sound by dragging with the mouse. "
	"To play a part of the sound, click in any of the rectangles below it. "
	"To move a selected part of the sound to another location, use #Cut and #Paste from the Edit menu. "
	"You can open sound windows for more than one sound, and then cut, copy, and paste between the sounds, "
	"just as you are used to doing with text and pictures in word processing programs.")
NORMAL (U"If your sound file is longer than a couple of minutes, "
	"or if you want to see and listen to both channels of a stereo sound, "
	"you may prefer to open it with @@Open long sound file...@. "
	"This puts a @LongSound object into the list. In this way, most of the sound will stay in the file on disk, "
	"and at most 60 seconds will be read into memory each time you play or view a part of it. "
	"To change these 60 seconds to something else, e.g. 500 seconds, "
	"choose ##LongSound settings...# from the #Settings submenu of the Praat menu.")
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

MAN_BEGIN (U"Intro 3.1. Viewing a spectrogram", U"ppgb", 20110128)  // 2023-06-08
INTRO (U"To see the spectral content of a sound as a function of time, "
	"select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor or @LongSoundEditor window will appear on your screen. "
	"In the entire bottom half of this window you will see a greyish image, which is called a %spectrogram. "
	"If you do not see it, choose @@Show spectrogram@ from the Spectrogram menu.")
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
	"just click in the spectrogram and you will see the vertical time cursor showing the time above "
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

MAN_BEGIN (U"Intro 3.2. Configuring the spectrogram", U"ppgb", 20220907)
NORMAL (U"With @@Spectrogram settings...@ from the #Spectrogram menu, "
	"you can determine how the spectrogram is computed and how it is displayed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (\"factory settings\"), which appear "
	"when you click ##Standards#.")
TERM (U"%%View range% (Hz)")
DEFINITION (U"the range of frequencies to display. The standard is 0 Hz at the bottom and 5000 Hz at the top. "
	"If this maximum frequency is higher than the Nyquist frequency of the Sound "
	"(which is half its sampling frequency), some values in the spectrogram will be zero, and the higher "
	"frequencies will be drawn in white. You can see this if you record a Sound at 44100 Hz and set the "
	"view range from 0 Hz to 25000 Hz.")
TERM (U"%%Window length")
DEFINITION (U"the duration of the analysis window. If this is 0.005 seconds (the standard), "
	"Praat uses for each frame the part of the sound that lies between 0.0025 seconds before "
	"and 0.0025 seconds after the centre of that frame "
	"(for Gaussian windows, Praat actually uses a bit more than that). "
	"The window length determines the %bandwidth of the spectral analysis, "
	"i.e. the width of the horizontal line in the spectrogram of a pure sine wave (see below). "
	"For a Gaussian window, the -3 dB bandwidth is 2*sqrt(6*ln(2))/(\\pi*%%Window length%), "
	"or 1.2982804 / %%Window length%. "
	"To get a “broad-band” spectrogram (bandwidth 260 Hz), keep the standard window length of 5 ms; "
	"to get a “narrow-band” spectrogram (bandwidth 43 Hz), set it to 30 ms (0.03 seconds). "
	"The other window shapes give slightly different values.")
TERM (U"%%Dynamic range% (dB)")
DEFINITION (U"All values that are more than %%Dynamic range% dB below the maximum (perhaps after dynamic compression, "
	"see @@Advanced spectrogram settings...@) "
	"will be drawn in white. Values in-between have appropriate shades of grey. Thus, if the highest "
	"peak in the spectrogram has a height of 30 dB/Hz, and the dynamic range is 50 dB (which is the standard value), "
	"then values below -20 dB/Hz will be drawn in white, and values between -20 dB/Hz and 30 dB/Hz will be drawn "
	"in various shades of grey.")
ENTRY (U"The bandwidth")
NORMAL (U"To see how the window length influences the bandwidth, "
	"first create a 1000-Hz sine wave with @@Create Sound from formula...@ "
	"by typing `1/2 * sin (2*pi*1000*x)` as the formula, then click ##View & Edit#. "
	"The spectrogram will show a horizontal black line. "
	"You can now vary the window length in the spectrogram settings and see how the thickness "
	"of the lines varies. The line gets thinner if you raise the window length. "
	"Apparently, if the analysis window comprises more periods of the wave, "
	"the spectrogram can tell us the frequency of the wave with greater precision.")
NORMAL (U"To see this more precisely, create a sum of two sine waves, with frequencies of 1000 and 1200 Hz. "
	"the formula is `1/4 * sin (2*pi*1000*x) + 1/4 * sin (2*pi*1200*x)`. In the editor, you will see "
	"a single thick band if the analysis window is short (5 ms), and two separate bands if the analysis "
	"window is long (30 ms). Apparently, the frequency resolution gets better with longer analysis windows.")
NORMAL (U"So why don't we always use long analysis windows? The answer is that their time resolution is poor. "
	"To see this, create a sound that consists of two sine waves and two short clicks. The formula is "
	"`0.02*(sin(2*pi*1000*x)+sin(2*pi*1200*x)) + (col=10000)+(col=10200)`. "
	"If you view this sound, you can see that the two clicks will overlap "
	"in time if the analysis window is long, and that the sine waves overlap in frequency if the "
	"analysis window is short. Apparently, there is a trade-off between time resolution and "
	"frequency resolution. One cannot know both the time and the frequency with great precision.")
ENTRY (U"Advanced settings")
NORMAL (U"The Spectrogram menu also contains @@Advanced spectrogram settings...@.")
MAN_END

MAN_BEGIN (U"Advanced spectrogram settings...", U"ppgb", 20120531)
ENTRY (U"Optimization")
TERM (U"%%Number of time steps%")
DEFINITION (U"the maximum number of points along the time window for which Praat has to compute "
	"the spectrum. If your screen is not wider than 1200 pixels, then the standard of 1000 is "
	"appropriate, since there is no point in computing more than one spectrum per one-pixel-wide vertical line. "
	"If you have a really wide screen, you may see improvement if you raise this number to 1500.")
TERM (U"%%Number of frequency steps%")
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
TERM (U"%%Method")
DEFINITION (U"there is currently only one method available in this window for computing a spectrum from "
	"a sound: the Fourier transform.")
TERM (U"%%Window shape")
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
	"by typing `1/2 * sin (2*pi*1000*x)` as the formula, then click ##View & Edit#. "
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
TERM (U"%%Autoscaling%")
TERM (U"%%Maximum% (dB/Hz)")
DEFINITION (U"all parts of the spectrogram that have a power above %maximum (after preemphasis) "
	"will be drawn in black. The standard maximum is 100 dB/Hz, but if %autoscaling is on (which is the standard), "
	"Praat will use the maximum of the visible part of the spectrogram instead; "
	"this ensures that the window will always look well, but it also means that the blackness "
	"of a certain part of the spectrogram will change as you scroll.")
TERM (U"%%Preemphasis% (dB/octave)")
DEFINITION (U"determines the steepness of a high-pass filter, "
	"i.e., how much the power of higher frequencies will be raised before drawing, as compared to lower frequencies. "
	"Since the spectral slope of human vowels is approximately -6 dB per octave, "
	"the standard value for this setting is +6 dB per octave, "
	"so that the spectrum is flattened and the higher formants look as strong as the lower ones. "
	"When you raise the preemphasis, frequency bands above 1000 Hz will become darker, those below 1000 Hz will become lighter.")
TERM (U"%%Dynamic compression")
DEFINITION (U"determines how much stronger weak spectra should be made before drawing. "
	"Normally, this parameter is between 0 and 1. If it is 0 (the standard value), there is no dynamic compression. "
	"If it is 1, all spectra will be drawn equally strong, "
	"i.e., all of them will contain frequencies that are drawn in black. "
	"If this parameter is 0.4 and the global maximum is at 80 dB, then a spectrum with a maximum at 20 dB "
	"(which will normally be drawn all white if the dynamic range is 50 dB), "
	"will be raised by 0.4 * (80 - 20) = 24 dB, "
	"so that its maximum will be seen at 44 dB (thus making this frame visible).")
MAN_END

MAN_BEGIN (U"Intro 3.3. Querying the spectrogram", U"ppgb", 20221202)
NORMAL (U"If you click anywhere inside the spectrogram, a cursor cross will appear, "
	"and you will see the time and frequency in red at the top and to the left of the window. "
	"To see the time in the Info window, "
	"choose ##Get cursor# from the #Spectrogram menu or press the F6 key. "
	"To see the frequency in the Info window, "
	"choose ##Get frequency# from the #Spectrogram menu.")
NORMAL (U"To query the power of the spectrogram at the cursor cross, "
	"choose ##Get spectral power at cursor cross# from the #Spectrum menu or press the F9 key. "
	"The Info window will show you the power density, expressed in Pascal^2/Hz.")
MAN_END

MAN_BEGIN (U"Intro 3.4. Printing the spectrogram", U"ppgb", 20220907)
NORMAL (U"To print a spectrogram, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to paint it into the @@Picture window@. "
	"You do this by choosing ##Paint visible spectrogram...# "
	"from the Spectrogram menu in the Sound or TextGrid window. "
	"From the File menu in the Picture window, you can then print it, save it to an EPS file, "
	"or copy it to the clipboard (to do Paste in your word processor, for instance).")
MAN_END

MAN_BEGIN (U"Intro 3.5. The Spectrogram object", U"ppgb", 20220907)
NORMAL (U"To do more with spectrograms, you can create a @Spectrogram object in the @@List of Objects@. "
	"You do this either by choosing ##Extract visible spectrogram# "
	"from the Spectrogram menu in the Sound or TextGrid window, "
	"or by selecting a Sound object in the list and choosing @@Sound: To Spectrogram...@ from the #Spectrum menu. "
	"In either case, a new Spectrogram object will appear in the list. "
	"To draw this Spectrogram object to the @@Picture window@, "
	"select it and choose the @@Spectrogram: Paint...@ command. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard. "
	"Many other commands are available in the @@dynamic menu@.")
MAN_END

MAN_BEGIN (U"Intro 3.6. Viewing a spectral slice", U"ppgb", 20220907)
INTRO (U"With ##View spectral slice# from the #Spectrogram menu in the @SoundEditor and the @TextGridEditor, "
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
	"on a “physical” window length of 10 ms, although the “effective” window length is still 5 ms "
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

MAN_BEGIN (U"Intro 4.2. Configuring the pitch contour", U"ppgb", 20190331)
NORMAL (U"With @@Pitch settings...@ from the #Pitch menu, "
	"you can determine how the pitch contour is displayed and how it is computed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (\"factory settings\"), which appear "
	"when you click #Standards.")
ENTRY (U"The %%pitch range% setting")
NORMAL (U"This is the most important setting for pitch analysis. The standard range is from 75 to 500 hertz, "
	"which means that the pitch analysis method will only find values between 75 and 500 Hz. "
	"The range that you set here will be shown to the right of the analysis window.")
NORMAL (U"You should set the range to values appropriate for your speaker, "
	"but a problem is that speakers vary enormously in their pitch ranges. "
	"For many low-pitched (e.g. average male) voices, you may want to set the floor to 75 Hz, and the ceiling to 300 Hz; "
	"for many high-pitched (e.g. average female) voices, a range of 100-500 Hz may instead be appropriate. "
	"On the high side, some children can reach almost 2000 Hz when yelling; "
	"on the low side, creaky voice can go as low as 40 Hz; "
	"for the speakers you are investigating, you may therefore want to experiment with this setting.")
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
	"although 75 Hz will still give you the same time resolution as you get for low-pitched voices.")
ENTRY (U"The %units setting")
NORMAL (U"This setting determines the units of the vertical pitch scale. Most people like to see the pitch range "
	"in hertz, but there are several other possibilities.")
ENTRY (U"Advanced settings")
NORMAL (U"The Pitch menu also contains @@Advanced pitch settings...@.")
MAN_END

MAN_BEGIN (U"Time step settings...", U"ppgb", 20220814 /*20031003*/)
INTRO (U"A command in the #Analysis menu of the @SoundEditor and @TextGridEditor "
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
	"The weighting is performed by a Gaussian (“double Kaiser”) window that has a duration that is determined by the "
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

MAN_BEGIN (U"Intro 8.1. Manipulation of pitch", U"ppgb", 20220814)
INTRO (U"To modify the pitch contour of an existing @Sound object, "
	"you select this @Sound and click ##To Manipulation#. "
	"A @Manipulation object will then appear in the list. "
	"You can then click @@View & Edit@ to raise a @ManipulationEditor, "
	"which will show the pitch contour (@PitchTier) as a series of thick blue dots. "
	"To reduce the number of dots, choose ##Stylize pitch (2 st)# "
	"from the #Pitch menu; it will then be easy to drag the dots "
	"around the time–pitch area (press the Option key to restrict dragging to vertical, "
	"and the Command key to restrict dragging to horizontal; mnemonics: Uption, Come Later).")
NORMAL (U"If you click any of the rectangles "
	"(or choose any of the commands from the #Play menu), "
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
	"(e.g. by a click on the PitchTier and a @@Command-click@ on the Manipulation), "
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

MAN_BEGIN (U"Intro 8.2. Manipulation of duration", U"ppgb", 20200901)
INTRO (U"You can use Praat to modify the relative durations in an existing sound.")
NORMAL (U"First, you select a @Sound object and click \"To Manipulation\". "
	"A @Manipulation object will then appear in the list. "
	"You can then click @@View & Edit@ to raise a @ManipulationEditor, "
	"which will show an empty @DurationTier. "
	"You can add targets to this tier by choosing \"Add duration point at cursor\" "
	"from the \"Dur\" menu. The targets will show up as blue dots, which you can easily drag "
	"around the duration area.")
NORMAL (U"If you click any of the rectangles "
	"(or choose any of the commands from the @Play menu), "
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
	"The DurationTier does linear interpolation, so it can only approximate these precise times, "
	"but fortunately to any precision you like:")
CODE (U"Create DurationTier: \"shorten\", 0, 0.085 + 0.270")
CODE (U"Add point: 0.000, 70/85")
CODE (U"Add point: 0.084999, 70/85")
CODE (U"Add point: 0.085001, 200/270")
CODE (U"Add point: 0.355, 200/270")
NORMAL (U"To put this DurationTier back into a Manipulation object, you select the two objects together "
	"(e.g. a click on the DurationTier and a @@Command-click@ on the Manipulation), "
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

MAN_BEGIN (U"List of Objects", U"ppgb", 20210228)
INTRO (U"A list in the left-hand part of the @@Objects window@.")
ENTRY (U"Purpose")
NORMAL (U"If you select one or more @objects in this list, "
	"the possible actions that you can perform with the selected objects "
	"will appear in the @@Dynamic menu@.")
ENTRY (U"How to select objects")
NORMAL (U"To select one object (and deselect all the others), click on the object.")
NORMAL (U"To extend the selection, drag the mouse or use Shift-click.")
NORMAL (U"To change the selection of one object (without changing the others), use @@Command-click@.")
MAN_END

MAN_BEGIN (U"New menu", U"ppgb", 20161013)
INTRO (U"The ##New menu# is one of the menus in the @@Objects window@. "
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

MAN_BEGIN (U"Objects window", U"ppgb", 20230325)
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
LIST_ITEM (U"The Objects window contains several fixed menus: "
	"the #Praat, #New, #Open, and #Help menus. "
	"It also contains the @@Save menu@, whose contents vary with the kinds of selected objects, "
	"and must, therefore, be considered part of the dynamic menu.")
ENTRY (U"The Praat menu")
LIST_ITEM (U"• @@New Praat script@: creates an empty @@ScriptEditor@")
LIST_ITEM (U"• @@New Praat notebook@: creates an empty @@NotebookEditor@")
LIST_ITEM (U"• @@Open Praat script...@: creates a @@ScriptEditor@ with a script from disk")
LIST_ITEM (U"• @@Open Praat notebook...@: creates a @@NotebookEditor@ with a notebook from disk")
LIST_ITEM (U"• The ##Goodies submenu#: for doing things (like using the Calculator) "
	"that do not create new objects and do not depend on the kinds of selected objects.")
LIST_ITEM (U"• The ##Settings submenu#: for program-wide preferences, "
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

MAN_BEGIN (U"Pitch menu", U"ppgb", 20221202)
INTRO (U"A menu in the @SoundEditor or @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Pitch settings...", U"ppgb", 20030316)
INTRO (U"A command in the Pitch menu of the @SoundEditor and @TextGridEditor windows. "
	"See @@Intro 4.2. Configuring the pitch contour@.")
MAN_END

MAN_BEGIN (U"Play", U"ppgb", /*20021212*/ 20220814)
INTRO (U"A command that is available if you select a @Sound, @Pitch, or @PointProcess object. "
	"It gives you an acoustic representation of the selected object, if your loudspeakers are on "
	"and you did not \"mute\" your computer sound system.")
NORMAL (U"A Play button is also available in the @SoundRecorder window "
	"and in the @Play menu of the @SoundEditor or @TextGridEditor. In the editors, "
	"you will usually play a sound by clicking on any of the rectangles around the data.")
MAN_END

MAN_BEGIN (U"Query submenu", U"ppgb", 20221202)
INTRO (U"A submenu that appears in the @@Objects window@ if you select an object of almost any type.")
INTRO (U"Query commands give you information about objects.")
NORMAL (U"Most query commands start with the word #%Get or sometimes the word #%Count. "
	"You will find these commands in two places: under the @@Query submenu@ that usually appears if you "
	"select an @@Objects|object@ in the list, and in the query parts of several menus of the @editors "
	"(such as the @@Pitch menu@, the @@Formants menu@ or the @@Spectrogram menu@.")
ENTRY (U"Behaviour")
NORMAL (U"If you click a query command, the answer will be written to the @@Info window@.")
ENTRY (U"Scripting")
NORMAL (U"In a script, you can still use query commands to write the information to the Info window "
	"but you can also use any query command to put the information into a variable. "
	"(see @@Scripting 6.3. Query commands@). In such a case, the value will not be written into the Info window.")
ENTRY (U"Some pages that link here")
ENTRY (U"Query commands in the Praat program")
LIST_ITEM (U"@@FAQ: Pitch analysis")
LIST_ITEM (U"@@Formulas 8. Attributes of objects")
LIST_ITEM (U"@@OT learning 7. Learning from overt forms")
LIST_ITEM (U"@@Script for listing F0 statistics")
LIST_ITEM (U"@@Scripting 3.3. Numeric queries")
LIST_ITEM (U"@@Scripting 3.5. String queries")
LIST_ITEM (U"@@Scripting 6.2. Writing to the Info window")
LIST_ITEM (U"@@time domain")
LIST_ITEM (U"@@Voice 6. Automating voice analysis with a script")
NORMAL (U"The Praat program contains at least the following query commands:")
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
INTRO (U"One of the menus in the @@Objects window@.")
NORMAL (U"With the Open menu, you read one or more @objects from a file on disk into memory. "
	"The resulting object(s) will appear in the @@List of Objects@.")
NORMAL (U"The Open menu contains the command @@Read from file...@, which recognizes most file types, "
	"and perhaps several other commands for reading unrecognizable file types (e.g., raw sound data), "
	"or for interpreting known file types in a different way "
	"(e.g., reading two mono sounds from one stereo sound file):")
MAN_END

MAN_BEGIN (U"Remove", U"ppgb", 20021212)
INTRO (U"One of the fixed buttons in the @@Objects window@.")
NORMAL (U"You can choose this command after selecting one or more @objects in the list.")
NORMAL (U"The selected objects will permanently disappear from the list, "
	"and the computer memory that they occupied will be freed.")
NORMAL (U"To save your data before removing, choose a command from the @@Save menu@.")
MAN_END

MAN_BEGIN (U"Rename...", U"ppgb", 20111018)
INTRO (U"One of the fixed buttons in the @@Objects window@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one object of any type.")
ENTRY (U"Purpose")
NORMAL (U"You can give the selected object a new name.")
ENTRY (U"Behaviour")
NORMAL (U"If you type special symbols or spaces, the Objects window will replace them with underscores.")
MAN_END

MAN_BEGIN (U"Save menu", U"ppgb", 20211015)
INTRO (U"One of the menus in the @@Objects window@.")
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
NORMAL (U"The Objects window will ask you for a file name. "
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
NORMAL (U"The Objects window will ask you for a file name. "
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
NORMAL (U"The Objects window will ask you for a file name. "
	"After you click OK, the objects will be written to a text file on disk.")
ENTRY (U"File format")
NORMAL (U"If you selected a single object, e.g., of type Pitch, "
	"the file will start with the lines:")
CODE (U"File type = \"ooTextFile\"")
CODE (U"Object class = \"Pitch\"")
NORMAL (U"After this, the pitch data will follow.")
LIST_ITEM (U"If you selected more than one object, e.g., “Pitch hallo” and “Polygon kromme”, "
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
	"is considered a comment, as is all text following an exclamation mark (“!”) on the same line.")
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
	"(I call this PitchTier “empty” because it does not contain any pitch information yet); "
	"you may want to name the PitchTier “source” and have it start at 0 seconds and end at 0.5 seconds. "
	"Once the PitchTier exists and is selected, you can ##View & Edit# it to add pitch points (pitch targets) to it at certain times "
	"(or you choose @@PitchTier: Add point...@ from the #Modify menu repeatedly). "
	"You could add a pitch point of 150 Hz at time 0.0 and a pitch point of 100 Hz at time 0.5. "
	"In the PitchTier window, you can see that the pitch curve falls linearly "
	"from 150 to 100 Hz during its time domain:")
SCRIPT (4.5, 2.5,
	U"Create PitchTier: “source”, 0, 0.5\n"
	"Add point: 0.0, 150\n"
	"Add point: 0.5, 100\n"
	"Draw: 0, 0, 0, 200, “yes”, “lines and speckles”\n"
	"Text top: “no”, ~%%An F0 linearly falling between two points\n"
	"One mark left: 100, “yes”, “yes”, “no”, “”\n"
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
	U"pitchTier = Create PitchTier: “source”, 0, 0.5\n"
	"Add point: 0.0, 150\n"
	"Add point: 0.5, 100\n"
	"pulses = To PointProcess\n"
	"Remove points between: 0, 0.02\n"
	"Remove points between: 0.24, 0.31\n"
	"Remove points between: 0.48, 0.5\n"
	"Draw: 0, 0, “yes”\n"
	"Text top: “no”, ~%%The times of the glottal pulses\n"
	"One mark bottom: 0.24, “yes”, “yes”, “no”, “”\n"
	"One mark bottom: 0.31, “yes”, “yes”, “no”, “”\n"
	"removeObject: pitchTier, pulses\n"
)
NORMAL (U"Now that we have a glottal point process (a glottal pulse train), the only thing left "
	"is to turn it into a sound by choosing @@PointProcess: To Sound (phonation)...@. "
	"If you use the standard settings of this command (but with ##Adaptation factor# set to 0.6), the result will be a @Sound with "
	"reasonable glottal flow derivatives centred around each of the original pulses in the point process. "
	"You can check this by selecting the Sound and choosing ##View & Edit#:")
SCRIPT (4.5, 2.5,
	U"pitchTier = Create PitchTier: “source”, 0, 0.5\n"
	"Add point: 0.0, 150\n"
	"Add point: 0.5, 100\n"
	"pulses = To PointProcess\n"
	"Remove points between: 0, 0.02\n"
	"Remove points between: 0.24, 0.31\n"
	"Remove points between: 0.48, 0.5\n"
	"source = To Sound (phonation): 44100, 0.6, 0.05, 0.7, 0.03, 3.0, 4.0\n"
	"Draw: 0, 0, 0, 0, “yes”, “curve”\n"
	"Text top: “no”, ~%%The source waveform\n"
	"One mark bottom: 0.24, “yes”, “yes”, “no”, “”\n"
	"One mark bottom: 0.31, “yes”, “yes”, “no”, “”\n"
	"removeObject: pitchTier, pulses, source\n"
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
CODE (U"pitchTier = Create PitchTier: “source”, 0, 0.5")
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
CODE (U"Create FormantGrid: “filter”, 0, 0.5, 10, 550, 1100, 60, 50")
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
CODE (U"Create PitchTier: “f0”, 0.00, 0.50")
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
CODE (U"Create IntensityTier: “intens”, 0.00, 0.50")
CODE (U"Add point: 0.05, 60")
CODE (U"Add point: 0.10, 80")
NORMAL (U"Generate the source signal:")
CODE (U"plusObject: “Sound f0”")
CODE (U"Multiply")
CODE (U"Rename: “source”")
NORMAL (U"The ten sounds are generated in a loop:")
CODE (U"for i from 1 to 10")
	CODE1 (U"f2_locus = 500 + (2500/9) * (i - 1) ; variable names start with lower case!")
	CODE1 (U"Create FormantGrid: “filter”, 0.0, 0.5, 9, 800, 1000, 60, 80")
	CODE1 (U"Remove formant points between: 1, 0.0, 0.5")
	CODE1 (U"Add formant point: 1, 0.05, 100")
	CODE1 (U"Add bandwidth point: 1, 0.05, 50")
	CODE1 (U"Add formant point: 2, 0.05, f2_locus")
	CODE1 (U"Add bandwidth point: 2, 0.05, 100")
	CODE1 (U"plusObject: “Sound source”")
	CODE1 (U"Filter (no scale)")
	CODE1 (U"Rename: “bada” + string$ (i)")
	CODE1 (U"removeObject: “FormantGrid filter”")
CODE (U"endfor")
NORMAL (U"Clean up:")
CODE (U"removeObject: “Sound source”, “Sound f0”, “IntensityTier intens”,")
CODE (U"... “PointProcess f0”, “PitchTier f0”")
NORMAL (U"In this example, filtering was done without automatic scaling, so that "
	"the resulting signals have equal intensities in the areas where they have "
	"equal formants. You will probably want to multiply all these signals with "
	"the same value in order to bring their amplitudes in a suitable range "
	"between -1 and +1 Pascal.")
MAN_END

MAN_BEGIN (U"Source-filter synthesis 4. Using existing sounds", U"ppgb", 20170828)
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
CODE (U"selectObject: “Sound hallo”")
CODE (U"Resample: 11000, 50")
NORMAL (U"You can then perform a linear-prediction analysis on the resampled sound "
	"with @@Sound: To LPC (burg)...@:")
CODE (U"selectObject: “Sound hallo_11000”")
CODE (U"To LPC (burg): 10, 0.025, 0.005, 50")
NORMAL (U"This says that your analysis is done with 10 linear-prediction parameters "
	"(which will yield at most five formant-bandwidth pairs), with an analysis window "
	"effectively 25 milliseconds long, with time steps of 5 milliseconds (so that the windows "
	"will appreciably overlap), and with a pre-emphasis frequency of 50 Hz (which is the point "
	"above which the sound will be amplified by 6 dB/octave prior to the analysis proper).")
NORMAL (U"As a result, an object called “LPC hallo” will appear in the list of objects. "
	"This @LPC object is a time function with 10 %%linear-prediction coefficients% in each %%time frame%. "
	"These coefficients are rather opaque even to the expert (try to view them with @Inspect), "
	"but they are the raw material from which formant and bandwidth values can be computed. "
	"To see the smoothed @Spectrogram associated with the LPC object, choose @@LPC: To Spectrogram...@:")
CODE (U"selectObject: “LPC hallo_11000”")
CODE (U"To Spectrogram: 20, 0, 50")
CODE (U"Paint: 0, 0, 0, 0, 50, 0, 0, “yes”")
NORMAL (U"Note that when drawing this Spectrogram, you will want to set the pre-emphasis to zero "
	"(the fifth 0 in the last line), because pre-emphasis has already been applied in the analysis.")
NORMAL (U"You can get and draw the formant-bandwidth pairs from the LPC object, "
	"with @@LPC: To Formant@ and @@Formant: Speckle...@:")
CODE (U"selectObject: “LPC hallo_11000”")
CODE (U"To Formant")
CODE (U"Speckle: 0, 0, 5500, 30, “yes”")
NORMAL (U"Note that in converting the @LPC into a @Formant object, you may have lost some "
	"information about spectral peaks at very low frequencies (below 50 Hz) or at very high "
	"frequencies (near the @@Nyquist frequency@ of 5500 Hz. Such peaks usually try to fit "
	"an overall spectral slope (if the 6 dB/octave model is inappropriate), and are not seen "
	"as related with resonances in the vocal tract, so they are ignored in a formant analysis. "
	"For resynthesis purposes, they might still be important.")
NORMAL (U"Instead of using the intermediate LPC object, you could have done a formant analysis "
	"directly on the original Sound, with @@Sound: To Formant (burg)...@:")
CODE (U"selectObject: “Sound hallo”")
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
CODE (U"selectObject: “Sound hallo_11000”, “LPC hallo_11000”")
CODE (U"Filter (inverse)")
NORMAL (U"A new Sound named `hallo_11000` will appear in the list of objects "
	"(you could rename it to `source`). "
	"This is the estimated source signal. Since the LPC analysis was designed to yield a spectrally "
	"flat filter (through the use of pre-emphasis), this source signal represents everything in the "
	"speech signal that cannot be attributed to the resonating cavities. Thus, the “source signal” "
	"will consist of the glottal volume-velocity source (with an expected spectral slope of "
	"-12 dB/octave for vowels) and the radiation characteristics at the lips, which cause a "
	"6 dB/octave spectral rise, so that the resulting spectrum of the “source signal” "
	"is actually the %derivative of the glottal flow, with an expected spectral slope of -6 dB/octave.")
NORMAL (U"Note that with inverse filtering you cannot measure the actual spectral slope of the source signal. "
	"Even if the actual slope is very different from -6 dB/octave, formant extraction will try to "
	"match the pre-emphasized spectrum. Thus, by choosing a pre-emhasis of -6 dB/octave, "
	"you %impose a slope of -6 dB/octave on the source signal.")
ENTRY (U"3. How to do the synthesis")
NORMAL (U"You can create a new Sound from a source Sound and a filter, in at least four ways.")
NORMAL (U"If your filter is an @LPC object, you select it and the source, and choose @@LPC & Sound: Filter...@:")
CODE (U"selectObject: “Sound source”, “LPC filter”")
CODE (U"Filter: “no”")
NORMAL (U"If you had computed the source and filter from an LPC analysis, this procedure should give "
	"you back the original Sound, except that windowing has caused 25 milliseconds at the beginning "
	"and end of the signal to be set to zero.")
NORMAL (U"If your filter is a @Formant object, you select it and the source, and choose @@Sound & Formant: Filter@:")
CODE (U"selectObject: “Sound source”, “Formant filter”")
CODE (U"Filter")
NORMAL (U"If you had computed the source and filter from an LPC analysis, this procedure will not generally give "
	"you back the original Sound, because some linear-prediction coefficients will have been ignored "
	"in the conversion to formant-bandwidth pairs.")
NORMAL (U"If your filter is a @FormantGrid object, you select it and the source, and choose @@Sound & FormantGrid: Filter@:")
CODE (U"selectObject: “Sound source”, “FormantGrid filter”")
CODE (U"Filter")
NORMAL (U"Finally, you could just know the %%impulse response% of your filter (in a @Sound object). "
	"You then select both Sound objects, and choose @@Sounds: Convolve...@:")
CODE (U"selectObject: “Sound source”, “Sound filter”")
CODE (U"Convolve: “integral”, “zero”")
NORMAL (U"As a last step, you may want to bring the resulting sound within the [-1; +1] range:")
CODE (U"Scale peak: 0.99")
ENTRY (U"4. How to manipulate the filter")
NORMAL (U"You can hardly change the values in an @LPC object in a meaningful way: "
	"you would have to manually change its rather opaque data with the help of @Inspect.")
NORMAL (U"A @Formant object can be changed in a friendlier way, with @@Formant: Formula (frequencies)...@ "
	"and @@Formant: Formula (bandwidths)...@. For instance, to multiply all formant frequencies by 0.9, "
	"you do")
CODE (U"selectObject: “Formant filter”")
CODE (U"Formula (frequencies): “self * 0.9”")
NORMAL (U"To add 200 hertz to all values of %F__2_, you do")
CODE (U"Formula (frequencies): ~ if row = 2 then self + 200 else self fi")
NORMAL (U"A @FormantGrid object can be changed by adding or removing points:")
LIST_ITEM (U"@@FormantGrid: Add formant point...@")
LIST_ITEM (U"@@FormantGrid: Add bandwidth point...@")
LIST_ITEM (U"@@FormantGrid: Remove formant points between...@")
LIST_ITEM (U"@@FormantGrid: Remove bandwidth points between...@")
ENTRY (U"5. How to manipulate the source signal")
NORMAL (U"You can manipulate the source signal in the same way you that would manipulate any sound, "
	"for instance with the @ManipulationEditor.")
MAN_END

MAN_BEGIN (U"Spectrogram menu", U"ppgb", 20221202)
INTRO (U"A menu in the @SoundEditor or @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Spectrogram settings...", U"ppgb", 20221202)
INTRO (U"A command in the @@Spectrogram menu@ of the @SoundEditor and @TextGridEditor windows. "
	"See @@Intro 3.2. Configuring the spectrogram@.")
MAN_END

MAN_BEGIN (U"Technical", U"ppgb", 20120915)
INTRO (U"The title of a submenu of the @@Praat menu@.")
MAN_END

MAN_BEGIN (U"Types of objects", U"ppgb", 20190928)
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
LIST_ITEM (U"• @Electroglottogram")
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

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Unicode"
© Paul Boersma 2023

Praat aims at being a fully international program:
the texts in Praat’s TextGrids, Tables, scripts, or Info window (and elsewhere) can contain many types of characters
(see @@special symbols@).
For this reason, Praat saves its text files in one of two possible formats: UTF-8 (including ASCII) or UTF-16.

ASCII text files
================

If your TextGrid (or Table, or script, or Info window...) contains only characters that can be encoded as ASCII,
namely the characters `!\"#$%&'()*+,-./0123456789:;<=>?@
ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`` abcdefghijklmnopqrstuvwxyz{|}~`,
then when you say @@Save as text file...@ or #Save, Praat will write an ASCII text file,
which is a text file in which every character is encoded in a single byte (8 bits).
All programs that can read plain text files can read such files produced by Praat.

UTF-16 text files
=================

If your TextGrid (or Table, or script, or Info window...) contains one or more characters that cannot be encoded as ASCII,
for instance West-European characters such as \ao\c,\e'\o"\ss\?d, East-European characters such as \c<\l/\o:\s<\uo\z',
or Hebrew characters such as \?+\B+\G+\M%\vO\sU,
then when you say @@Save as text file...@ or #Save, Praat will write an UTF-16 text file,
which is a text file in which every character is encoded in two bytes (and some very rare characters in four bytes).
Many programs can read such text files, for instance NotePad, WordPad, Microsoft Word, and TextWrangler.

What if my other programs cannot read UTF-16 text files?
========================================================

If you want to export your Table to Microsoft Excel or to SPSS, or if you want your TextGrid file to be read
by somebody else's Perl script, then there will be no problem if your Table contains only ASCII characters (see above).
But if your Table contains any other (i.e. non-ASCII) characters, you may be in trouble, because Praat will write the Table
as an UTF-16 text file, and not all of the programs just mentioned can read such files yet.

What you can do is go to ##Text writing settings...# in the #Settings submenu of the #Praat menu,
and there set the output encoding to ##UTF-8#. Praat will from then on save your text files in the UTF-8 format,
which means one byte for every ASCII character and 2 to 4 bytes for every non-ASCII character.
Especially on Linux, many programs understand UTF-8 text and will display the correct characters.
Programs such as SPSS do not understand UTF-8 but will still display ASCII characters correctly;
for instance, the names München and Wałęsa may appear as M\A~\:-nchen and Wa\Ao,\A"\TMsa or so.

If you can get by with West-European characters (on Windows),
then you may choose ##try ISO Latin-1, then UTF-16# for the output encoding.
It is possible (but not guaranteed) that programs like SPSS then display your West-European text correctly.
This trick is of limited use, because it will not work if your operating system is set to a “codepage”
differently from ISO Latin-1 (or “ANSI”), or if you need East-European or Hebrew characters, or if you want
to share your text files with Macintosh users.

If you already have some UTF-16 text files and you want to convert them to UTF-8 or ISO Latin-1 (the latter only if
they do not contain non-West-European characters), then you can read them into Praat and save them again
(with the appropriate output encoding setting).
Other programs, such a NotePad and TextWrangler, can also do this conversion.

Finally, it is still possible to make sure that all texts are ASCII, e.g. you type the characters ß and ő
as “`\ss`” and “`\o:`” respectively. See @@special symbols@.

################################################################################
"Play"
© Paul Boersma 2022

One of the menus in several @editors and in the @manual.
################################################################################
)~~~"
MAN_PAGES_END

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
