/* manual_tutorials.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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

MAN_BEGIN ("What's new?", "ppgb", 20051219)
INTRO ("Latest changes in P\\s{RAAT}.")
NORMAL ("##4.4# (December 19, 2005)")
NORMAL ("##4.3.37# (December 15, 2005)")
LIST_ITEM ("\\bu @@Principal component analysis@: now accepts tables with more variables (columns) than cases (rows).")
LIST_ITEM ("\\bu TIMIT label files: removed a bug that caused Praat to crash for files whose first part was not labelled.")
NORMAL ("##4.3.36# (December 11, 2005)")
LIST_ITEM ("\\bu Ltas: Average.")
LIST_ITEM ("\\bu Optimality Theory: compute crucial rankings (select OTGrammar + PairDistribution).")
NORMAL ("##4.3.35# (December 8, 2005)")
LIST_ITEM ("\\bu @ExperimentMFC: switched off warnings for stereo files.")
NORMAL ("##4.3.34# (December 8, 2005)")
LIST_ITEM ("\\bu Sound window: the arrow scroll step is settable.")
LIST_ITEM ("\\bu You can now run multiple listening experiments (@ExperimentMFC) in one \"Run\".")
LIST_ITEM ("\\bu Formant: Get quantile of bandwidth...")
NORMAL ("##4.3.33# (December 6, 2005)")
LIST_ITEM ("\\bu Removed three bugs introduced in 4.3.32 in @ExperimentMFC.")
NORMAL ("##4.3.32# (December 5, 2005)")
LIST_ITEM ("\\bu Many more possibilities in @ExperimentMFC.")
NORMAL ("##4.3.31# (November 27, 2005)")
LIST_ITEM ("\\bu @@Sound: To Ltas (pitch-corrected)...@")
NORMAL ("##4.3.30# (November 18, 2005)")
LIST_ITEM ("\\bu TableOfReal: Scatter plot: allows reversed axes.")
NORMAL ("##4.3.29# (November 11, 2005)")
LIST_ITEM ("\\bu Windows: many more font sizes.")
NORMAL ("##4.3.28# (November 7, 2005)")
LIST_ITEM ("\\bu Fontless EPS files: corrected character width for Symbol font (depended on SILIPA setting).")
LIST_ITEM ("\\bu Windows: more reliable detection of home directory.")
NORMAL ("##4.3.27# (October 7, 2005)")
LIST_ITEM ("\\bu TextGrid & Pitch: draw with text alignment.")
NORMAL ("##4.3.26# (September 29, 2005)")
LIST_ITEM ("\\bu Macintosh: corrected error introduced in 4.3.25.")
NORMAL ("##4.3.25# (September 28, 2005)")
LIST_ITEM ("\\bu Macintosh: allowed recording with sample rates of 12 and 64 kHz.")
NORMAL ("##4.3.24# (September 26, 2005)")
LIST_ITEM ("\\bu Table: Down to TableOfReal...: one column of the Table can be used as the row labels for the TableOfReal, "
	"and the strings in the remaining columns of the Table are replaced with whole numbers assigned in alphabetical order.")
NORMAL ("##4.3.23# (September 24, 2005)")
LIST_ITEM ("\\bu Read Table from comma-separated file...")
LIST_ITEM ("\\bu Read Table from tab-separated file...")
LIST_ITEM ("\\bu Write picture as fontless EPS file: choice between XIPA and SILIPA93.")
LIST_ITEM ("\\bu Bold IPA symbols in EPS files (fontless SILIPA93 only).")
NORMAL ("##4.3.22# (September 8, 2005)")
LIST_ITEM ("\\bu Macintosh: variable scroll bar size (finally, 7 years since System 8.5).")
NORMAL ("##4.3.21# (September 1, 2005)")
LIST_ITEM ("\\bu Macintosh: error message if any of the fonts Times, Helvetica, Courier and Symbol are unavailable at start-up.")
LIST_ITEM ("\\bu Renamed \"Control\" menu to \"Praat\" on all systems (as on the Mac), "
	"to reflect the fact that no other programs than Praat have used the Praat shell for five years.")
LIST_ITEM ("\\bu Script editor: Undo and Redo buttons (only on the Mac for now).")
LIST_ITEM ("\\bu Manual: corrected a bug that sometimes caused Praat to crash when trying to print.")
NORMAL ("##4.3.20# (August 18, 2005)")
LIST_ITEM ("\\bu Log files: include name of editor window.")
NORMAL ("##4.3.19# (July 20, 2005)")
LIST_ITEM ("\\bu Improved buttons in manual.")
LIST_ITEM ("\\bu TableOfReal: Read from headerless spreadsheet file: allow row and column labels to be 30,000 rather than 100 characters.")
NORMAL ("##4.3.18# (July 12, 2005)")
LIST_ITEM ("\\bu Glottal source for sound synthesis, corrected and documented.")
NORMAL ("##4.3.17# (July 7, 2005)")
LIST_ITEM ("\\bu Glottal source for sound synthesis.")
LIST_ITEM ("\\bu Multi-level Optimality Theory: parallel evaluation and bidirectional learning.")
NORMAL ("##4.3.16# (June 22, 2005)")
LIST_ITEM ("\\bu Pitch drawing: corrected logarithmic scales.")
NORMAL ("##4.3.15# (June 22, 2005)")
LIST_ITEM ("\\bu Graphics: better dotted lines in pitch contours; clipped pitch curves in editor windows.")
LIST_ITEM ("\\bu Pitch analysis: more different units (semitones %re 1 Hz).")
NORMAL ("##4.3.14# (June 14, 2005)")
LIST_ITEM ("\\bu Scripting: regular expressions.")
LIST_ITEM ("\\bu Removed a bug that caused Praat to crash if a proposed object name was longer than 200 characters.")
NORMAL ("##4.3.13# (May 19, 2005)")
LIST_ITEM ("\\bu Macintosh: an option to switch off screen previews in EPS files.")
LIST_ITEM ("\\bu Sources: compatibility of makefiles with MinGW (Minimalist GNU for Windows).")
NORMAL ("##4.3.12# (May 10, 2005)")
LIST_ITEM ("\\bu Some more manual tricks.")
NORMAL ("##4.3.11# (May 6, 2005)")
LIST_ITEM ("\\bu TextGrid editor: show number of intervals.")
NORMAL ("##4.3.10# (April 25, 2005)")
LIST_ITEM ("\\bu Table: Get logistic regression.")
NORMAL ("##4.3.08# (April 19, 2005)")
LIST_ITEM ("\\bu OT learning: store history with \"OTGrammar & Strings: Learn from partial outputs...\".")
NORMAL ("##4.3.07# (March 31, 2005)")
LIST_ITEM ("\\bu Linux: removed a bug that could cause a sound to stop playing.")
NORMAL ("##4.3.04# (March 9, 2005)")
LIST_ITEM ("\\bu Use SIL Doulos IPA 1993/1996 instead of 1989.")
NORMAL ("##4.3.03# (March 2, 2005)")
LIST_ITEM ("\\bu TextGrid window: green colouring of matching text.")
LIST_ITEM ("\\bu Regular expressions can be used in many places.")
LIST_ITEM ("\\bu Pitch analysis: switched off formant-pulling.")
NORMAL ("##4.3.02# (February 16, 2005)")
LIST_ITEM ("\\bu TextGrid: Remove boundary at time...")
LIST_ITEM ("\\bu Scripting: corrected %nowarn.")
LIST_ITEM ("\\bu Linux: guard against blocking audio device.")
LIST_ITEM ("\\bu Macintosh: guard against out-of-range audio level meter.")
NORMAL ("##4.3.01# (February 9, 2005)")
LIST_ITEM ("\\bu Replaced PostScript font SILDoulosIPA with XIPA (adapted for Praat by Rafael Laboissi\\e`re).")
LIST_ITEM ("\\bu Sound: Set part to zero...")
LIST_ITEM ("\\bu Pitch: To Sound (sine)...")
LIST_ITEM ("\\bu Sound & TextGrid: Clone time domain.")
NORMAL ("##4.3# (January 26, 2005)")
NORMAL ("##4.2.35# (January 24, 2005)")
LIST_ITEM ("\\bu OT learning: save in spreadsheet format.")
NORMAL ("##4.2.34# (January 12, 2005)")
LIST_ITEM ("\\bu TextGrid editor: better visibility of yellow lines.")
NORMAL ("##4.2.33# (January 11, 2005)")
LIST_ITEM ("\\bu MacOS X: work around bug in StuffIt Expander 9.0.1.")
NORMAL ("##4.2.32# (January 1, 2005)")
LIST_ITEM ("\\bu Scripting: removed small bug that did not allow spaces before \"form\" statement.")
NORMAL ("##4.2.31# (December 13, 2004)")
LIST_ITEM ("\\bu OT learning: made progress window show rankings on Windows.")
LIST_ITEM ("\\bu MacOS 9 scripting: allowed recursive \"execute\".")
NORMAL ("##4.2.29# (December 4, 2004)")
LIST_ITEM ("\\bu Scripting: allow multiple open forms with Apply buttons, including from commands added to menus.")
NORMAL ("##4.2.28# (December 3, 2004)")
LIST_ITEM ("\\bu OT learning: corrected violations of *Lapse constraint (three unstressed syllables at the edges as well).")
NORMAL ("##4.2.27# (December 2, 2004)")
LIST_ITEM ("\\bu Removed a bug introduced recently that could cause Praat to crash "
	"when selecting a small part to the left or right of the pitch curve in the sound window.")
NORMAL ("##4.2.26# (December 1, 2004)")
LIST_ITEM ("\\bu Much updated manual.")
NORMAL ("##4.2.24# (November 24, 2004)")
LIST_ITEM ("\\bu Improved manual about spectra.")
LIST_ITEM ("\\bu Single @@Sound: To Spectrum...@.")
NORMAL ("##4.2.23# (November 17, 2004)")
LIST_ITEM ("\\bu Much faster reading of 16-bit sound files.")
LIST_ITEM ("\\bu `Apply' button in settings windows for menu commands and in script forms.")
NORMAL ("##4.2.22# (November 15, 2004)")
LIST_ITEM ("\\bu LongSound: save separate channels.")
LIST_ITEM ("\\bu Macintosh: much faster reading of WAV files.")
NORMAL ("##4.2.21# (November 4, 2004)")
LIST_ITEM ("\\bu Calibrated long-term average spectrum (Sound: To Ltas...).")
LIST_ITEM ("\\bu Removed a bug introduced in 4.2.19 that caused \"Sound: To Harmonicity\" not to work at all.")
LIST_ITEM ("\\bu Scripting: nowarn, noprogress, nocheck.")
NORMAL ("##4.2.20# (October 24, 2004)")
LIST_ITEM ("\\bu Speeded up intensity analysis by a factor of 10 "
	"(by making its time resolution 0.01 ms rather than 0.0001 ms at a sampling frequency of 44 kHz).")
LIST_ITEM ("\\bu Improved query commands for Intensity and Ltas.")
LIST_ITEM ("\\bu Removed 30,000-character limit in Info window.")
LIST_ITEM ("\\bu Removed a bug introduced in 4.2.19 that caused Praat to crash in TextGrid: Draw.")
NORMAL ("##4.2.19# (October 18, 2004)")
LIST_ITEM ("\\bu Speeded up pitch analysis and spectrogram analysis by a factor of two.")
LIST_ITEM ("\\bu Windows: removed a bug that slowed Praat down after several hours of work or "
	"after a very long analysis with the progress window.")
NORMAL ("##4.2.18# (October 5, 2004)")
LIST_ITEM ("\\bu PitchTier: To Sound (sine).")
LIST_ITEM ("\\bu Removed a bug that caused too many warnings in ManPages that call scripts.")
NORMAL ("##4.2.17# (September 14, 2004)")
LIST_ITEM ("\\bu Info window: can save.")
LIST_ITEM ("\\bu Script editor: line numbers.")
LIST_ITEM ("\\bu Unix: picture highlighting as on Mac and Windows.")
LIST_ITEM ("\\bu Optimality Theory: monitor rankings when learning.")
NORMAL ("##4.2.16# (September 5, 2004)")
LIST_ITEM ("\\bu One can now drag the inner viewport in the Picture window, excluding the margins. "
	"This is nice e.g. for creating square viewports or for drawing a waveform and a spectrogram in close contact.")
NORMAL ("##4.2.14# (August 20, 2004)")
LIST_ITEM ("\\bu Windows: \"Write to fontless EPS file...\" now correctly references SILDoulosIPA rather than SILDoulosIPA-Regular.")
NORMAL ("##4.2.13# (August 12, 2004)")
LIST_ITEM ("\\bu ExperimentMFC: removed a bug that caused Praat to crash if the carrier-before phrase "
	"was longer than the carrier-after phrase.")
NORMAL ("##4.2.11# (August 11, 2004)")
LIST_ITEM ("\\bu Optimality Theory: added WeightByPosition and *MoraicConsonant, to learn coda weights.")
NORMAL ("##4.2.10# (August 10, 2004)")
LIST_ITEM ("\\bu Optimality Theory: Remove harmonically bounded candidates.")
LIST_ITEM ("\\bu Linux: removed the same bug as in 4.1.13, but now for scripts.")
NORMAL ("##4.2.09# (July 28, 2004)")
LIST_ITEM ("\\bu Command line scripting: better handling of spaces.")
LIST_ITEM ("\\bu Optimality Theory: added *Clash and *Lapse.")
NORMAL ("##4.2.08# (July 9, 2004)")
LIST_ITEM ("\\bu @ExperimentMFC: subjects can now respond with keyboard presses.")
LIST_ITEM ("\\bu Shimmer measurements: more accurate and less sensitive to additive noise.")
NORMAL ("##4.2.07# (June 16, 2004)")
LIST_ITEM ("\\bu Sound editor: queries/reports/listings sometimes report on the whole visible part (and then say so).")
LIST_ITEM ("\\bu SSCP: Draw sigma ellipse: allow reversed axes.")
LIST_ITEM ("\\bu Scripting: corrected readability of voice report.")
NORMAL ("##4.2.06# (May 14, 2004)")
LIST_ITEM ("\\bu Reading and opening 24-bit and 32-bit sound files (saving still goes in 16 bits).")
LIST_ITEM ("\\bu Sound: Scale intensity.")
NORMAL ("##4.2.05# (May 13, 2004)")
LIST_ITEM ("\\bu More extensive voice report (pitch statistics; harmonicity).")
LIST_ITEM ("\\bu Pitch-corrected LTAS analysis.")
LIST_ITEM ("\\bu Much faster Fourier transform (non-FFT version).")
LIST_ITEM ("\\bu More drawing methods for Sound and Ltas (curve, bars, poles, speckles).")
LIST_ITEM ("\\bu More TableOfReal extraction commands.")
LIST_ITEM ("\\bu Removed bugs from the orthographic representation of vowels in the Peterson & Barney table.")
LIST_ITEM ("\\bu Removed a bug introduced in 4.2.01 that caused a lack of redrawing in the button editor window.")
NORMAL ("##4.2.04# (April 21, 2004)")
LIST_ITEM ("\\bu Removed a bug introduced in 4.2.01 that caused a lack of redrawing in the PointProcess "
	"and LongSound editor windows.")
LIST_ITEM ("\\bu Sound: To Spectrum (dft)")
NORMAL ("##4.2.03# (April 20, 2004)")
LIST_ITEM ("\\bu MacOS X and Linux: removed a bug introduced in 4.2.01 by which ##Create Strings from file list...# "
	"did not work with a wildcard (\"*\").")
NORMAL ("##4.2.01# (April 7, 2004)")
LIST_ITEM ("\\bu Improved frequency precision in spectrogram drawing.")
LIST_ITEM ("\\bu Less flashing in sound window.")
LIST_ITEM ("\\bu Better warnings against use of the LPC object.")
ENTRY ("Praat 4.2, March 4, 2004")
	NORMAL ("General:")
	LIST_ITEM ("\\bu July 10, 2003: Open source code (General Public Licence).")
	NORMAL ("Phonetics:")
	LIST_ITEM ("\\bu Faster computation of spectrum, spectrogram, and pitch.")
	LIST_ITEM ("\\bu More precision in numeric libraries.")
	LIST_ITEM ("\\bu PitchTier: Interpolate quadratically.")
	LIST_ITEM ("\\bu TextGrids can be saved chronologically (and Praat can read that file again).")
	LIST_ITEM ("\\bu Sound editor window @@Time step settings...@: \"Automatic\", \"Fixed\", and \"View-dependent\".")
	LIST_ITEM ("\\bu Sound window: distinguish basic from advanced spectrogram and pitch settings.")
	LIST_ITEM ("\\bu Read TableOfReal from headerless spreadsheet file...: cells with strings are considered zero.")
	LIST_ITEM ("\\bu Sound window: introduced time step as advanced setting.")
	LIST_ITEM ("\\bu Sound window: reintroduced view range as advanced setting.")
	LIST_ITEM ("\\bu Ltas: Compute trend line, Subtract trend line.")
	NORMAL ("Audio:")
	LIST_ITEM ("\\bu Sun workstations: support audio servers.")
	NORMAL ("Graphics:")
	LIST_ITEM ("\\bu Better selections in Picture window and editor windows.")
	LIST_ITEM ("\\bu Picture window: better handling of rectangles and ellipses for reversed axes.")
	LIST_ITEM ("\\bu Windows: corrected positioning of pictures on clipboard and in metafiles.")
	LIST_ITEM ("\\bu Windows: EPS files check availability of Times and TimesNewRomanPSMT.")
	LIST_ITEM ("\\bu Polygon: can now also paint in colour instead of only in grey values.")
	LIST_ITEM ("\\bu Unlimited number of points for polygons in PostScript (may not work on very old printers).")
	LIST_ITEM ("\\bu Picture window: line widths on all printers and clipboards are now equal to line widths used on PostScript printers: "
		"a line with a line width of \"1\" will be drawn with a width 3/8 points. This improves the looks of pictures printed "
		"on non-PostScript printers, improves the looks of pictures copied to your wordprocessor when printed, "
		"and changes the looks of pictures copied to your presentation program.")
	NORMAL ("OT learning:")
	LIST_ITEM ("\\bu Metrics grammar supports \'impoverished overt forms\', "
		"i.e. without secondary stress even if surface structures do have secondary stress.")
	LIST_ITEM ("\\bu Support for crucially tied constraints and tied candidates.")
	LIST_ITEM ("\\bu Support for backtracking in EDCD.")
	LIST_ITEM ("\\bu Queries for testing grammaticality.")
	NORMAL ("Scripting:")
	LIST_ITEM ("\\bu ManPages: script links can receive arguments.")
	LIST_ITEM ("\\bu ManPages: variable duration of recording.")
	LIST_ITEM ("\\bu Support for unlimited size of script files in editor window on Windows XP and MacOS X (the Unix editions already had this).")
	LIST_ITEM ("\\bu Improved the reception of %sendpraat commands on Windows XP.")
ENTRY ("Praat 4.1, June 5, 2003")
	NORMAL ("General:")
	LIST_ITEM ("\\bu MacOS X edition.")
	LIST_ITEM ("\\bu Removed licensing.")
	LIST_ITEM ("\\bu More than 99 percent of the source code distributed under the General Public Licence.")
	LIST_ITEM ("\\bu Windows 2000 and XP: put preferences files in home directory.")
	NORMAL ("Phonetics:")
	LIST_ITEM ("\\bu Spectrum: the sign of the Fourier transform has changed, to comply with common use "
		"in technology and physics. Old Spectrum files are converted when read.")
	LIST_ITEM ("\\bu Spectral moments.")
	LIST_ITEM ("\\bu Many jitter and shimmer measures, also in the Sound editor window.")
	LIST_ITEM ("\\bu PitchTier: shift or multiply frequencies (also in ManipulationEditor).")
	LIST_ITEM ("\\bu TextGrid: shift times, scale times.")
	LIST_ITEM ("\\bu PSOLA synthesis: reduced buzz in voiceless parts.")
	LIST_ITEM ("\\bu @@Sound: Change gender...")
	LIST_ITEM ("\\bu Editors: @@Intro 3.5. Viewing a spectral slice@.")
	LIST_ITEM ("\\bu Editors: Get spectral power at cursor cross.")
	LIST_ITEM ("\\bu @@Sound: To PointProcess (periodic, peaks)...@")
	LIST_ITEM ("\\bu Ltas: merge.")
	NORMAL ("Listening experiments:")
	LIST_ITEM ("\\bu Goodness judgments.")
	LIST_ITEM ("\\bu Multiple ResultsMFC: ##To Table#, so that the whole experiment can go into a single statistics file.")
	LIST_ITEM ("\\bu Stimulus file path can be relative to directory of experiment file.")
	LIST_ITEM ("\\bu @ExperimentMFC: multiple substimuli for discrimination tests.")
	NORMAL ("Statistics:")
	LIST_ITEM ("\\bu New @Table object for column @statistics: Pearson's %r, Kendall's %\\ta-%b, %t-test.")
	LIST_ITEM ("\\bu Table: scatter plot.")
	LIST_ITEM ("\\bu Table: column names as variables.")
	LIST_ITEM ("\\bu @@T-test@.")
	LIST_ITEM ("\\bu TableOfReal: Extract rows where column...")
	LIST_ITEM ("\\bu TableOfReal: Get correlation....")
	LIST_ITEM ("\\bu @@Correlation: Confidence intervals...")
	LIST_ITEM ("\\bu @@SSCP: Get diagonality (bartlett)...")
	NORMAL ("OT learning:")
	LIST_ITEM ("\\bu Tutorial for bidirectional learning.")
	LIST_ITEM ("\\bu Random choice between equally violating candidates.")
	LIST_ITEM ("\\bu More constraints in metrics grammar.")
	LIST_ITEM ("\\bu Learning in editor.")
	NORMAL ("Graphics:")
	LIST_ITEM ("\\bu Printing: hard-coded image interpolation for EPS files and PostScript printers.")
	NORMAL ("Scripting:")
	LIST_ITEM ("\\bu New @Formulas tutorial.")
	LIST_ITEM ("\\bu @Formulas: can use variables without quotes.")
	LIST_ITEM ("\\bu Formulas for PitchTier, IntensityTier, AmplitudeTier, DurationTier.")
	LIST_ITEM ("\\bu Refer to any matrices and tables in formulas, e.g. Sound_hello (x) or Table_everything [row, col] "
		"or Table_tokens [i, \"F1\"].")
	LIST_ITEM ("\\bu Assignment by modification, as with += -= *= /=.")
	LIST_ITEM ("\\bu New functions: date\\$ (), extractNumber, extractWord\\$ , extractLine\\$ . See @@Formulas 5. String functions@.")
	LIST_ITEM ("\\bu @@Scripting 5.7. Including other scripts@.")
	LIST_ITEM ("\\bu String formulas in the calculator.")
	LIST_ITEM ("\\bu Stopped support of things that had been undocumented for the last four years: "
		"#let, #getnumber, #getstring, #ARGS, #copy, #proc, variables with capitals, and strings in numeric variables; "
		"there are messages about how to modify your old scripts.")
	LIST_ITEM ("\\bu Disallowed ambiguous expressions like -3\\^ 2.")
ENTRY ("Praat 4.0, October 15, 2001")
	NORMAL ("Editors:")
	LIST_ITEM ("\\bu Simplified selection and cursor in editor windows.")
	LIST_ITEM ("\\bu Spectrogram, pitch contour, formant contour, and intensity available in the "
		"Sound, LongSound, and TextGrid editors.")
	LIST_ITEM ("\\bu TextGrid editor: additions and improvements.")
	LIST_ITEM ("\\bu @@Log files@.")
	NORMAL ("Phonetics library:")
	LIST_ITEM ("\\bu @ExperimentMFC: multiple-forced-choice listening experiments.")
	LIST_ITEM ("\\bu @@Sound: To Pitch (ac)...@: pitch contour less dependent on time resolution. "
		"This improves the constancy of the contours in the editors when zooming.")
	LIST_ITEM ("\\bu TextGrid: additions and improvements.")
	LIST_ITEM ("\\bu Sounds: Concatenate recoverably. Creates a TextGrid whose interval labels are the original "
		"names of the sounds.")
	LIST_ITEM ("\\bu Sound & TextGrid: Extract all intervals. The reverse of the previous command.")
	LIST_ITEM ("\\bu Filterbank analyses, @MelFilter, @BarkFilter and "
		"@FormantFilter, by @@band filtering in the frequency domain@." )
	LIST_ITEM ("\\bu Cepstrum by David Weenink: @MFCC, @LFCC. "
		"@Cepstrum object is a representation of the %%complex cepstrum%.")
	LIST_ITEM ("\\bu Intensity: To IntensityTier (peaks, valleys).")
	LIST_ITEM ("\\bu Replaced Analysis and AnalysisEditor with @Manipulation and @ManipulationEditor.")
	NORMAL ("Phonology library:")
	LIST_ITEM ("\\bu PairDistribution: Get percentage correct (maximum likelihood, probability matching).")
	LIST_ITEM ("\\bu OTGrammar & PairDistribution: Get percentage correct...")
	NORMAL ("Graphics:")
	LIST_ITEM ("\\bu Improved spectrogram drawing.")
	LIST_ITEM ("\\bu @@Special symbols@: h\\a'c\\v^ek.")
	LIST_ITEM ("\\bu Macintosh: improved screen rendition of rotated text.")
	NORMAL ("Audio:")
	LIST_ITEM ("\\bu Macintosh: support for multiple audio input devices (sound cards).")
	NORMAL ("Statistics and numerics library:")
	LIST_ITEM ("\\bu More statistics by David Weenink.")
	LIST_ITEM ("\\bu Improved random numbers and other numerical stuff.")
	LIST_ITEM ("\\bu @@Regular expressions@.")
	NORMAL ("Scripting:")
	LIST_ITEM ("\\bu Formatting in variable substitution, e.g. 'pitch:2' gives two digits after the decimal point.")
	LIST_ITEM ("\\bu Added ##fixed\\$ # to scripting language for formatting of numbers.")
	NORMAL ("Documentation:")
	LIST_ITEM ("\\bu @@Multidimensional scaling@ tutorial.")
	LIST_ITEM ("\\bu Enabled debugging-at-a-distance.")
ENTRY ("Praat 3.9, October 18, 2000")
	NORMAL ("Editors:")
	LIST_ITEM ("\\bu Shift-click and shift-drag extend or shrink selection in editor windows.")
	LIST_ITEM ("\\bu Grouped editors can have separate zooming and scrolling (FunctionEditor preferences).")
	LIST_ITEM ("\\bu Cursor follows playing sound in editors; interruption by Escape key moves the cursor.")
	LIST_ITEM ("\\bu TextGridEditor: optimized for transcribing large corpora: text field, directly movable boundaries, "
		"more visible text in tiers, @SpellingChecker, "
		"type while the sound is playing, complete keyboard navigation, control font size, control text alignment, "
		"shift-click near boundary adds interval to selection.")
	LIST_ITEM ("\\bu Stereo display in LongSound and TextGrid editors.")
	LIST_ITEM ("\\bu LongSoundEditor and TextGridEditor: write selection to audio file.")
	LIST_ITEM ("\\bu SoundEditor: added command \"Extract selection (preserve times)\".")
	LIST_ITEM ("\\bu IntervalTierEditor, DurationTierEditor.")
	LIST_ITEM ("\\bu Added many query commands in editors.")
	NORMAL ("Phonetics library:")
	LIST_ITEM ("\\bu Sound: To Formant...: sample-rate-independent formant analysis.")
	LIST_ITEM ("\\bu Sound: To Harmonicity (glottal-to-noise excitation ratio).")
	LIST_ITEM ("\\bu Pitch: support for ERB units, draw all combinations of line/speckle and linear/logarithmic/semitones/mels/erbs, "
		"optionally with TextGrid, Subtract linear fit.")
	LIST_ITEM ("\\bu Spectrum: Draw along logarithmic frequency axis.")
	LIST_ITEM ("\\bu TextGrid:  modification commands, Extract part, Shift to zero, Scale times (with Sound or LongSound).")
	LIST_ITEM ("\\bu @@Matrix: To TableOfReal@, Draw contour...")
	LIST_ITEM ("\\bu Concatenate Sound and LongSound objects.")
	LIST_ITEM ("\\bu File formats: save PitchTier in spreadsheet format, read CGN syntax files (XML version), "
		"text files now completely file-server-safe (independent from Windows/Macintosh/Unix line separators).")
	NORMAL ("Statistics and numerics library:")
	LIST_ITEM ("\\bu @@Principal component analysis@.")
	LIST_ITEM ("\\bu @@Discriminant analysis@.")
	LIST_ITEM ("\\bu @Polynomial: drawing, @@Roots|root@ finding etc.")
	LIST_ITEM ("\\bu @@TableOfReal: Draw box plots...@.")
	LIST_ITEM ("\\bu @@Covariance: To TableOfReal (random sampling)...@.")
	LIST_ITEM ("\\bu @@SSCP: Get sigma ellipse area...@.")
	LIST_ITEM ("\\bu Query @DTW for 'weighted distance' of time warp.")
	LIST_ITEM ("\\bu Distributions: To Strings (exact)...")
	LIST_ITEM ("\\bu Strings: Randomize.")
	NORMAL ("Phonology library:")
	LIST_ITEM ("\\bu OTGrammar: To PairDistribution.")
	NORMAL ("Graphics:")
	LIST_ITEM ("\\bu Full support for colour inkjet printers on Windows and Macintosh.")
	LIST_ITEM ("\\bu Full support for high-resolution colour clipboards and metafiles for "
		"Windows and Macintosh programs that support them (this include MS Word "
		"for Windows, but unfortunately not MS Word for Macintosh).")
	LIST_ITEM ("\\bu Colour in EPS files.")
	LIST_ITEM ("\\bu Interpolating grey images, i.e. better zoomed spectrograms.")
	LIST_ITEM ("\\bu Linux: support for 24-bits screens.")
	NORMAL ("Audio:")
	LIST_ITEM ("\\bu Asynchronous sound play.")
	LIST_ITEM ("\\bu Linux: solved problems with /dev/mixer (\"Cannot read MIC gain.\") on many computers.")
	LIST_ITEM ("\\bu Added possibility of zero padding for sound playing, "
		"in order to reduce clicks on some Linux and Sun computers.")
	LIST_ITEM ("\\bu LongSound supports mono and stereo, 8-bit and 16-bit, %\\mu-law and A-law, "
		"big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.")
	LIST_ITEM ("\\bu \"Read two Sounds from stereo file...\" supports 8-bit and 16-bit, %\\mu-law and A-law, "
		"big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.")
	LIST_ITEM ("\\bu SoundRecorder writes to 16-bit AIFC, WAV, NeXT/Sun, and NIST mono and stereo files.")
	LIST_ITEM ("\\bu Sound & LongSound: write part or whole to mono or stereo audio file.")
	LIST_ITEM ("\\bu Read Sound from raw Alaw file.")
	LIST_ITEM ("\\bu Artword & Speaker (& Sound) movie: real time on all systems.")
	NORMAL ("Scripting:")
	LIST_ITEM ("\\bu @@Formulas 4. Mathematical functions@: added statistical functions: %\\ci^2, Student T, Fisher F, binomial, "
		"and their inverse functions.")
	LIST_ITEM ("\\bu Windows: program #praatcon for use as a Unix-style console application.")
	LIST_ITEM ("\\bu Windows and Unix: Praat can be run with a command-line interface without quitting on errors.")
	LIST_ITEM ("\\bu Unix & Windows: can use <stdout> as a file name (supports pipes for binary data).")
	LIST_ITEM ("\\bu @sendpraat now also for Macintosh.")
	LIST_ITEM ("\\bu @@Scripting 6.7. Sending a message to another program|sendsocket@.")
	LIST_ITEM ("\\bu @@Read from file...@ recognizes script files if they begin with \"\\# !\".")
	LIST_ITEM ("\\bu Script links in @ManPages.")
	NORMAL ("Documentation")
	LIST_ITEM ("\\bu Tutorials on all subjects available through @Intro.")
ENTRY ("Praat 3.8, January 12, 1999")
	NORMAL ("Phonetics library")
	LIST_ITEM ("\\bu New objects: @LongSound (view and label long sound files), with editor; PairDistribution.")
	LIST_ITEM ("\\bu @PSOLA manipulation of voiceless intervals, version 2: quality much better now; "
		"target duration is exactly as expected from Duration tier or specified lengthening in @@Sound: Lengthen (PSOLA)...@.")
	LIST_ITEM ("\\bu Audio: Escape key stops audio playing (on Mac also Command-period).")
	LIST_ITEM ("\\bu @SoundRecorder: allows multiple recordings without close; Play button; Write buttons; buffer size can be set.")
	LIST_ITEM ("\\bu Reverse a Sound or a selection of a Sound.")
	LIST_ITEM ("\\bu @@Sound: Get nearest zero crossing...@.")
	LIST_ITEM ("\\bu Formant: \"Scatter plot (reversed axes)...\".")
	LIST_ITEM ("\\bu TextGrid & Pitch: \"Speckle separately...\".")
	LIST_ITEM ("\\bu \"Extract Sound selection (preserve times)\" in TextGridEditor.")
	LIST_ITEM ("\\bu More query commands for Matrix, TableOfReal, Spectrum, PointProcess.")
	NORMAL ("Phonology library")
	LIST_ITEM ("\\bu 25-page OT learning tutorial.")
	LIST_ITEM ("\\bu Made the OT learner 14 times as fast.")
	NORMAL ("Systems")
	LIST_ITEM ("\\bu May 23: Windows beta version.")
	LIST_ITEM ("\\bu April 24: Windows alpha version.")
	NORMAL ("Files")
	LIST_ITEM ("\\bu Read more Kay, Sun (.au), and WAV sound files.")
	LIST_ITEM ("\\bu \"Read Strings from raw text file...\"")
	LIST_ITEM ("\\bu @@Create Strings as file list...@.")
	LIST_ITEM ("\\bu \"Read IntervalTier from Xwaves...\"")
	LIST_ITEM ("\\bu hidden \"Read from old Windows Praat picture file...\"")
	NORMAL ("Graphics")
	LIST_ITEM ("\\bu Use colours (instead of only greys) in \"Paint ellipse...\" etc.")
	LIST_ITEM ("\\bu More true colours (maroon, lime, navy, teal, purple, olive).")
	LIST_ITEM ("\\bu Direct printing from Macintosh to PostScript printers.")
	LIST_ITEM ("\\bu Hyperpage printing to PostScript printers and PostScript files.")
	LIST_ITEM ("\\bu Phonetic symbols: raising sign, lowering sign, script g, corner, ligature, pointing finger.")
	NORMAL ("Shell")
	LIST_ITEM ("\\bu November 4: all dialogs are modeless (which is new for Unix and Mac).")
	LIST_ITEM ("\\bu September 27: @sendpraat for Windows.")
	NORMAL ("Scripting")
	LIST_ITEM ("\\bu January 7: scriptable editors.")
	LIST_ITEM ("\\bu October 7: file I/O in scripts.")
	LIST_ITEM ("\\bu August 23: script language includes all the important functions for string handling.")
	LIST_ITEM ("\\bu June 24: string variables in scripts.")
	LIST_ITEM ("\\bu June 22: faster look-up of script variables.")
	LIST_ITEM ("\\bu June 22: unlimited number of script variables.")
	LIST_ITEM ("\\bu April 5: suspended chopping of trailing spaces.")
	LIST_ITEM ("\\bu March 29: enabled formulas as arguments to dialogs (also interactive).")
ENTRY ("Praat 3.7, March 24, 1998")
	NORMAL ("Editors:")
	LIST_ITEM ("\\bu In all FunctionEditors: drag to get a selection.")
	NORMAL ("Phonetics library:")
	LIST_ITEM ("\\bu Many new query (#Get) commands for @Sound, @Intensity, @Harmonicity, @Pitch, "
		"@Formant, @Ltas, @PitchTier, @IntensityTier, @DurationTier, @FormantTier.")
	LIST_ITEM ("\\bu Many new modification commands.")
	LIST_ITEM ("\\bu Many new interpolations.")
	LIST_ITEM ("\\bu Sound enhancements: @@Sound: Lengthen (PSOLA)...@, @@Sound: Deepen band modulation...@")
	LIST_ITEM ("\\bu @@Source-filter synthesis@ tutorial, @@Sound & IntensityTier: Multiply@, "
		"@@Sound & FormantTier: Filter@, @@Formant: Formula (frequencies)...@, @@Sound: Pre-emphasize (in-line)...@.")
	NORMAL ("Labelling")
	LIST_ITEM ("\\bu TextGrid queries (#Get times and labels in a script).")
	LIST_ITEM ("\\bu @@TextGrid: Count labels...@.")
	LIST_ITEM ("\\bu @@PointProcess: To TextGrid (vuv)...@: get voiced/unvoiced information from a point process.")
	LIST_ITEM ("\\bu IntervalTier to TableOfReal: labels become row labels.")
	LIST_ITEM ("\\bu TextTier to TableOfReal.")
	NORMAL ("Numerics and statistics library")
	LIST_ITEM ("\\bu Multidimensional scaling (Kruskal, INDSCAL, etc).")
	LIST_ITEM ("\\bu @TableOfReal: Set value, Formula, Remove column, Insert column, Draw as squares, To Matrix.")
	NORMAL ("Phonology library")
	LIST_ITEM ("\\bu OT learning: new strategies: weighted symmetric plasticity (uncancelled or all).")
	NORMAL ("Praat shell")
	LIST_ITEM ("\\bu First Linux version.")
	LIST_ITEM ("\\bu Eight new functions like e.g. %hertzToBark in @@Formulas 4. Mathematical functions@.")
	LIST_ITEM ("\\bu @@Praat script@: procedure arguments; object names.")
	NORMAL ("Documentation:")
	LIST_ITEM ("\\bu 230 more man pages (now 630).")
	LIST_ITEM ("\\bu Hypertext: increased readability of formulas, navigation with keyboard.")
ENTRY ("Praat 3.6, October 27, 1997")
	NORMAL ("Editors:")
	LIST_ITEM ("\\bu Intuitive position of B and E buttons on left-handed mice.")
	LIST_ITEM ("\\bu @SoundEditor: copy %windowed selection to list of objects.")
	LIST_ITEM ("\\bu @SoundEditor: undo Cut, Paste, Zero.")
	LIST_ITEM ("\\bu @SpectrumEditor: copy band-filtered spectrum or sound to list of objects.")
	LIST_ITEM ("\\bu @ManipulationEditor: LPC-based pitch manipulation.")
	NORMAL ("Objects:")
	LIST_ITEM ("\\bu Use '-', and '+' in object names.")
	NORMAL ("Phonetics library")
	LIST_ITEM ("\\bu LPC-based resynthesis in @ManipulationEditor.")
	LIST_ITEM ("\\bu @Sound: direct modification without formulas (addition, multiplication, windowing)")
	LIST_ITEM ("\\bu @Sound: filtering in spectral domain by formula.")
	LIST_ITEM ("\\bu Create a simple @Pitch object from a @PitchTier (for %F__0_) and a @Pitch (for V/U).")
	LIST_ITEM ("\\bu Semitones in @PitchTier tables.")
	LIST_ITEM ("\\bu @PointProcess: transplant time domain from @Sound.")
	LIST_ITEM ("\\bu Much more...")
	NORMAL ("Phonology library")
	LIST_ITEM ("\\bu Computational Optimality Theory. See @@OT learning@.")
	NORMAL ("Hypertext")
	LIST_ITEM ("\\bu You can use @ManPages files for creating your own tutorials. "
		"These contains buttons for playing and recording sounds, so you can use this for creating "
		"an interactive IPA sound training course.")
	NORMAL ("Scripting:")
	LIST_ITEM ("\\bu Programmable @@Praat script@ language: variables, expressions, control structures, "
		"procedures, complete dialog box, exchange of information with Info window, continuation lines.")
	LIST_ITEM ("\\bu Use system-independent relative file paths in @@Praat script@.")
	LIST_ITEM ("\\bu @ScriptEditor: Run selection.")
	NORMAL ("Graphics:")
	LIST_ITEM ("\\bu Rotation and scaling while printing the @@Picture window@.")
	LIST_ITEM ("\\bu Apart from bold and italic, now also bold-italic (see @@Text styles@).")
	LIST_ITEM ("\\bu Rounded rectangles.")
	LIST_ITEM ("\\bu Conversion of millimetres and world coordinates.")
	LIST_ITEM ("\\bu Measurement of text widths (screen and PostScript).")
	NORMAL ("Unix:")
	LIST_ITEM ("\\bu Use the @sendpraat program for sending messages to running Praat programs.")
	NORMAL ("Mac:")
	LIST_ITEM ("\\bu Praat looks best with the new and beautiful System 8.")
ENTRY ("Praat 3.5, May 27, 1997")
	NORMAL ("New editors:")
	LIST_ITEM ("\\bu #TextGridEditor replaces and extends LabelEditor: edit points as well as intervals.")
	LIST_ITEM ("\\bu #AnalysisEditor replaces and extends PsolaEditor: view pitch, spectrum, formant, and intensity "
		"analyses in a single window, and allow pitch and duration resynthesis by #PSOLA and more (would be undone in 3.9.19).")
	LIST_ITEM ("\\bu #SpectrumEditor allows you to view and edit spectra.")
	NORMAL ("Praat shell:")
	LIST_ITEM ("\\bu ##History mechanism# remembers all the commands that you have chosen, "
		"and allows you to put them into a script.")
	LIST_ITEM ("\\bu #ScriptEditor allows you to edit and run any Praat script, and to put it under a button.")
	LIST_ITEM ("\\bu All added and removed buttons are remembered across sessions.")
	LIST_ITEM ("\\bu #ButtonEditor allows you to make buttons visible or invisible.")
	NORMAL ("Evaluations:")
	LIST_ITEM ("\\bu In his 1996 doctoral thesis, Henning Reetz "
		"compared five pitch analysis routines; @@Sound: To Pitch (ac)...@ appeared to make the fewest errors. "
		"H. Reetz (1996): %%Pitch Perception in Speech: a Time Domain Approach%, Studies in Language and Language Use #26, "
		"IFOTT, Amsterdam (ICG Printing, Dordrecht).")
	NORMAL ("Documentation:")
	LIST_ITEM ("\\bu 140 more man pages (now 330).")
	LIST_ITEM ("\\bu Tables and pictures in manual.")
	LIST_ITEM ("\\bu Printing the entire manual.")
	LIST_ITEM ("\\bu Logo.")
	NORMAL ("New classes:")
	LIST_ITEM ("\\bu Labelling & segmentation: #TextGrid, #IntervalTier, #TextTier.")
	LIST_ITEM ("\\bu Analysis & manipulation: #Analysis.")
	LIST_ITEM ("\\bu Statistics: #TableOfReal, #Distributions, #Transition")
	NORMAL ("File formats:")
	LIST_ITEM ("\\bu Read and write rational numbers in text files.")
	LIST_ITEM ("\\bu Read 8-bit .au sound files.")
	LIST_ITEM ("\\bu Read and write raw 8-bit two\'s-complement and offset-binary sound files.")
	NORMAL ("Audio:")
	LIST_ITEM ("\\bu 16-bit interactive Sound I/O on Mac.")
	LIST_ITEM ("\\bu Record sounds at 9.8 kHz on SGI.")
	NORMAL ("New commands:")
	LIST_ITEM ("\\bu Two more pitch-analysis routines.")
	LIST_ITEM ("\\bu Sound to PointProcess: collect all maxima, minima, zero crossings.")
	LIST_ITEM ("\\bu PointProcess: set calculus.")
	LIST_ITEM ("\\bu TextGrid: extract time-point information.")
	LIST_ITEM ("\\bu Compute pitch or formants at given time points.")
	LIST_ITEM ("\\bu Put pitch, formants etc. in tables en get statistics.")
	LIST_ITEM ("\\bu Many more...")
	NORMAL ("Macintosh:")
	LIST_ITEM ("\\bu 16-bit interactive sound I/O.")
	LIST_ITEM ("\\bu Fast and interpolating spectrogram drawing.")
	LIST_ITEM ("\\bu Phonetic Mac screen font included in source code (as a fallback to using SIL Doulos IPA).")
	LIST_ITEM ("\\bu Keyboard shortcuts, text editor, help under question mark, etc.")
ENTRY ("Praat 3.3, October 6, 1996")
	LIST_ITEM ("\\bu Documentation: hypertext help browser, including the first 190 man pages.")
	LIST_ITEM ("\\bu New editors: class #TextTier for labelling times instead of intervals.")
	LIST_ITEM ("\\bu New actions: #Formant: Viterbi tracker, Statistics menu, Scatter plot.")
	LIST_ITEM ("\\bu Evaluation: For HNR analysis of speech, the cross-correlation method, "
		"which has a sensitivity of 60 dB and a typical time resolution of 12 milliseconds, "
		"must be considered better than the autocorrelation method, "
		"which has a better sensitivity (80 dB), but a much worse time resolution (30 ms). "
		"For pitch analysis, the autocorrelation method still beats the cross-correlation method "
		"because of its better resistance against noise and echos, "
		"and despite its marginally poorer resolution (15 vs. 12 ms).")
	LIST_ITEM ("\\bu User preferences are saved across sessions.")
	LIST_ITEM ("\\bu The phonetic X screen font included in the source code.")
	LIST_ITEM ("\\bu Xwindows resources included in the source code")
	LIST_ITEM ("\\bu Graphics: eight colours, small caps, text rotation.")
	LIST_ITEM ("\\bu File formats: Sun/NexT mu-law files, raw matrix text files, Xwaves mark files.")
	LIST_ITEM ("\\bu Accelerations: keyboard shortcuts, faster dynamic menu, Shift-OK keeps file selector on screen.")
	LIST_ITEM ("\\bu Class changes: #StylPitch and #MarkTier are now called #PitchTier and #TextTier, respectively. "
		"Old files can still be read.")
	LIST_ITEM ("\\bu Script warning: all times in dialogs are in seconds now: milliseconds have gone.")
ENTRY ("Praat 3.2, April 29, 1996")
	LIST_ITEM ("\\bu Sound I/O for HPUX, Sun Sparc 5, and Sun Sparc LX.")
	LIST_ITEM ("\\bu Cross-correlation pitch and HNR analysis.")
	LIST_ITEM ("\\bu Facilities for generating tables from scripts.")
	LIST_ITEM ("\\bu Editing and playing stylized pitch contours and point processes.")
	LIST_ITEM ("\\bu PSOLA pitch manipulation.")
	LIST_ITEM ("\\bu Spectral smoothing techniques: cepstrum and LPC.")
	LIST_ITEM ("\\bu Time-domain pitch analysis with jitter measurement.")
	LIST_ITEM ("\\bu Read and write Bell-Labs sound files and Kay CSL audio files.")
	LIST_ITEM ("\\bu Replaced IpaTimes font by free SILDoulos-IPA font, and embedded phonetic font in PostScript picture.")
	LIST_ITEM ("\\bu Completed main phonetic characters.")
ENTRY ("Praat 3.1, December 5, 1995")
	LIST_ITEM ("\\bu Add and remove buttons dynamically.")
	LIST_ITEM ("\\bu DataEditor (Inspect button).")
	LIST_ITEM ("\\bu Initialization scripts.")
	LIST_ITEM ("\\bu Logarithmic axes.")
	LIST_ITEM ("\\bu Call remote ADDA server directly.")
ENTRY ("To do")
	LIST_ITEM ("\\bu TextGrid & Sound: Extract intervals with margins.")
	LIST_ITEM ("\\bu Spectrum: draw power, re, im, phase.")
	LIST_ITEM ("\\bu Formant: To Spectrum (slice)... (combines Formant-to-LPC and LPC-to-Spectrum-slice)")
	LIST_ITEM ("\\bu Read and/or write Matlab files, MBROLA files, Xwaves files, CHAT files.") /* Aix */
	LIST_ITEM ("\\bu Matrix: draw numbers.")
	LIST_ITEM ("\\bu Fractions with \\bsf{a|b}.")
	LIST_ITEM ("\\bu Move objects up and down list.")
	LIST_ITEM ("\\bu Spectrogram cross-correlation.")
	LIST_ITEM ("\\bu Labels in AIFC file.") /* Theo Veenker 19980323 */
	LIST_ITEM ("\\bu Improve scrolling and add selection in hyperpages.")
	LIST_ITEM ("\\bu Segment spectrograph?") /* Ton Wempe, Jul 16 1996 */
	LIST_ITEM ("\\bu Phoneme-to-articulation conversion??") /* Mirjam Ernestus, Jul 1 1996 */
ENTRY ("Known bugs in all editions")
	LIST_ITEM ("\\bu (small) No navigation by tabs.")
	LIST_ITEM ("\\bu (small) Tabs in texts should be considered spaces.")
ENTRY ("Known bugs in the Macintosh version")
	LIST_ITEM ("\\bu (small) Pause window modal.")
	LIST_ITEM ("\\bu (small) Cascade buttons grey after suspend+resume during progress window.")
	LIST_ITEM ("\\bu Movie window cannot be closed.")
ENTRY ("Known bugs in the Windows version")
	LIST_ITEM ("\\bu Cannot stand infinitesimal zooming in SpectrogramEditor.")
	LIST_ITEM ("\\bu Clipboards with greys sometimes become black-and-white after use of colour.")
ENTRY ("Known bugs in the Unix versions")
	LIST_ITEM ("\\bu (small) Motif messaging windows should have no parent and be modeless.")
ENTRY ("Known bugs in the Linux version")
	LIST_ITEM ("\\bu Sounds shorter than 200 ms do not always play (workaround: add zeroes in prefs).")
	LIST_ITEM ("\\bu Keyboard shortcuts do not work if NumLock is on.")
	LIST_ITEM ("\\bu Progress window does not always disappear.")
ENTRY ("Known bugs in the Solaris version")
	LIST_ITEM ("\\bu (serious for some) File names run out of the window in some Motif versions.")
ENTRY ("Known bugs in the HP version")
	LIST_ITEM ("\\bu (serious for some) Sound recording should be from audioserver instead of local.")
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
*/
 
MAN_BEGIN ("Acknowledgments", "ppgb", 20050208)
NORMAL ("For supplying source code:")
LIST_ITEM ("GPL IPA fonts: Fukui Rei (XIPA) & Rafael Laboissi\\e`re (adaptation to Praat).")
LIST_ITEM ("IPA fonts: Summer Institute of Linguistics (Doulos1989, until January 2005).")
LIST_ITEM ("Linux audio: Darryl Purnell, Pretoria.")
LIST_ITEM ("GPL Gnu Scientific Library: Gerard Jungman, Brian Gough.")
LIST_ITEM ("Public domain Fourier transforms: Paul Swarztrauber (fftpack), Monty.")
LIST_ITEM ("Public domain LAPACK: Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., "
	"Courant Institute, Argonne National Lab, and Rice University.")
NORMAL ("For technical support and advice:")
LIST_ITEM ("Ton Wempe.")
NORMAL ("For their financial support during the development of Praat:")
LIST_ITEM ("Netherlands Organization for Scientific Research (NWO).")
LIST_ITEM ("Talkbank project, Carnegie Mellon / Linguistic Data Consortium.")
LIST_ITEM ("Spoken Dutch Corpus (CGN).")
LIST_ITEM ("Laboratorium Experimentele OtoRhinoLaryngologie, KU Leuven.")
LIST_ITEM ("DFG-Projekt Dialektintonation, Universit\\a\"t Freiburg.")
LIST_ITEM ("Department of Linguistics and Phonetics, Lund University.")
LIST_ITEM ("Centre for Cognitive Neuroscience, University of Turku.")
LIST_ITEM ("Linguistics Department, University of Joensuu.")
LIST_ITEM ("Laboratoire de Sciences Cognitives et Psycholinguistique, Paris.")
LIST_ITEM ("Department of Linguistics, Northwestern University.")
LIST_ITEM ("Department of Finnish and General Linguistics, University of Tampere.")
LIST_ITEM ("Institute for Language and Speech Processing, Paradissos Amaroussiou.")
LIST_ITEM ("J\\o\"rg Jescheniak, Universit\\a\"t Leipzig.")
LIST_ITEM ("The Linguistics Teaching Laboratory, Ohio State University.")
LIST_ITEM ("Linguistics & Cognitive Science, Dartmouth College, Hanover NH.")
LIST_ITEM ("Cornell Phonetics Lab, Ithaca NY.")
MAN_END

MAN_BEGIN ("Praat menu", "ppgb", 20050822)
INTRO ("The first menu in the @@Object window@. On MacOS X, this menu is in the main menu bar.")
MAN_END

MAN_BEGIN ("Copy...", "ppgb", 19960904)
INTRO ("One of the fixed buttons in the @@Object window@.")
ENTRY ("Availability")
NORMAL ("You can choose this command after selecting one object of any class.")
ENTRY ("Behaviour")
NORMAL ("The Object window copies the selected object, and all the data it contains, "
	"to a new object, which will appear at the bottom of the List of Objects.")
ENTRY ("Example")
NORMAL ("If you select \"Sound hallo\" and click `Copy...', "
	"a dialog will appear, which prompts you for a name; "
	"after you click OK, a new object will appear in the list, bearing that name.")
MAN_END

MAN_BEGIN ("Draw menu", "ppgb", 20010417)
INTRO ("A menu that occurs in the @@Dynamic menu@ for many objects.")
NORMAL ("This menu contains commands for drawing the object to the @@Picture window@, "
	"which will allow you to print the drawing or to copy it to your word processor.")
MAN_END

MAN_BEGIN ("Dynamic menu", "ppgb", 19981024)
INTRO ("A column of buttons in the right-hand part of the @@Object window@, "
	"plus the #Write menu in the Object window.")
NORMAL ("If you select one or more @objects in the list, "
	"the possible actions that you can perform with the selected objects "
	"will appear in the dynamic menu. "
	"These actions can include editing, writing, drawing, "
	"conversions to other types (including analysis and synthesis), and more.")
ENTRY ("Example of analysis:")
NORMAL ("Record a Sound, select it, and click on ##To Pitch...#. "
	"This will create a new Pitch object and put it in the list of objects. "
	"You can then edit, write, and draw this Pitch object.")
ENTRY ("Example of synthesis:")
NORMAL ("Create a #Speaker, create and edit an #Artword, and click on ##To Sound...#.")
MAN_END

MAN_BEGIN ("Edit", "ppgb", 20010414)
INTRO ("A command in the @@Dynamic menu@ of several types of @objects.")
NORMAL ("This command puts an @@Editors|editor@ window on the screen, which shows the contents of the selected object. "
	"This window will allow your to view and modify the contents of this object.")
MAN_END

MAN_BEGIN ("Extract visible formant contour", "ppgb", 20030316)
INTRO ("One of the commands in the Formant menu of the @SoundEditor and the @TextGridEditor.")
NORMAL ("See @@Intro 5. Formant analysis@")
MAN_END

MAN_BEGIN ("Extract visible intensity contour", "ppgb", 20030316)
INTRO ("One of the commands in the Intensity menu of the @SoundEditor and the @TextGridEditor.")
NORMAL ("See @@Intro 6. Intensity analysis@")
MAN_END

MAN_BEGIN ("Extract visible pitch contour", "ppgb", 20030316)
INTRO ("One of the commands in the Pitch menu of the @SoundEditor and the @TextGridEditor.")
NORMAL ("See @@Intro 4. Pitch analysis@")
MAN_END

MAN_BEGIN ("Extract visible spectrogram", "ppgb", 20030316)
INTRO ("One of the commands in the Spectrogram menu of the @SoundEditor and the @TextGridEditor.")
NORMAL ("See @@Intro 3. Spectral analysis@")
MAN_END

MAN_BEGIN ("FAQ (Frequently Asked Questions)", "ppgb", 20050227)
LIST_ITEM ("@@FAQ: How to cite Praat")
LIST_ITEM ("@@FAQ: Formant analysis")
LIST_ITEM ("@@FAQ: Pitch analysis")
LIST_ITEM ("@@FAQ: Spectrograms")
LIST_ITEM ("@@FAQ: Scripts")
/* Hardware */
/* Pitch: octave errors */
MAN_END

MAN_BEGIN ("FAQ: Formant analysis", "ppgb", 20030916)
NORMAL ("#Problem: I get different formant values if I choose to analyse 3 formants "
	"than if I choose to analyse 4 formants.")
NORMAL ("Solution: the \"number of formants\" in formant analysis determines the "
	"number of peaks with which the %entire spectrum is modelled. For an average "
	"female voice, you should choose to analyse 5 formants in the region up to 5500 Hz, "
	"even if you are interested only in the first three formants.")
NORMAL ("")
NORMAL ("#Problem: I often get only 1 formant in a region where I see clearly 2 formants "
	"in the spectrogram.")
NORMAL ("This occurs mainly in back vowels (F1 and F2 close together) for male voices, "
	"if the \"maximum formant\" is set to the standard of 5500 Hz, which is appropriate "
	"for female voices. Set the \"maximum formant\" down to 5000 Hz. "
	"No, Praat comes without a guarantee: the formant analysis is based on LPC, "
	"and this comes with several assumptions as to what a speech spectrum is like.")
NORMAL ("")
NORMAL ("#Question: what algorithm is used for formant analysis?")
NORMAL ("Answer: see @@Sound: To Formant (burg)...@.")
MAN_END

MAN_BEGIN ("FAQ: How to cite Praat", "ppgb", 20050227)
NORMAL ("#Question: how do I cite Praat in my articles?")
NORMAL ("Answer: nowadays most journals allow you to cite computer programs and web sites. "
	"The style approved by the American Psychological Association, "
	"and therefore by many journals, is like the following "
	"(change the dates and version number as needed):")
NORMAL ("Boersma, Paul & Weenink, David (" PRAAT_YEAR "). "
	"Praat: doing phonetics by computer (Version " PRAAT_VERSION ") [Computer program]. "
	"Retrieved " PRAAT_MONTH " " PRAAT_DAY ", " PRAAT_YEAR ", from http://www.praat.org/")
NORMAL ("If the journal does not allow you to cite a web site, then try:")
NORMAL ("Boersma, Paul (2001). Praat, a system for doing phonetics by computer. "
	"%%Glot International% ##5:9/10#, 341-345.")
MAN_END

MAN_BEGIN ("FAQ: Pitch analysis", "ppgb", 20021219)
NORMAL ("#Question: what algorithm is used for pitch analysis?")
NORMAL ("Answer: see @@Sound: To Pitch (ac)...@. The 1993 article is downloadable from "
	"http://www.fon.hum.uva.nl/paul/")
NORMAL ("#Question: why do I get different results for the maximum pitch if...?")
NORMAL ("If you select a Sound and choose @@Sound: To Pitch...@, the time step will usually "
	"be 0.01 seconds. The resulting @Pitch object will have values for times that are "
	"0.01 seconds apart. If you then click Info or choose ##Get maximum pitch# from the #Query menu, "
	"the result is based on those time points. By contrast, if you choose ##Get maximum pitch# "
	"from the #Pitch menu in the SoundEditor window, the result will be based on the visible points, "
	"of which there tend to be a hundred in the visible window. These different time spacings will "
	"lead to slightly different pitch contours.")
NORMAL ("If you choose ##Move cursor to maximum pitch#, then choose ##Get pitch# from the "
	"#%Pitch menu, the result will be different again. This is because ##Get maximum pitch# "
	"can do a parabolic interpolation around the maximum, whereas ##Get pitch#, not realizing "
	"that the cursor is at a maximum, does a stupid linear interpolation, which tends to lead to "
	"lower values.")
MAN_END

MAN_BEGIN ("FAQ: Scripts", "ppgb", 20040222)
NORMAL ("#Question: how do I do something to all the files in a directory?")
NORMAL ("Answer: look at @@Create Strings as file list...@.")
NORMAL ("")
NORMAL ("#Question: why doesn't the editor window react to my commands?")
NORMAL ("Your commands are probably something like:")
CODE ("Read from file... hello.wav")
CODE ("Edit")
CODE ("Zoom... 0.3 0.5")
NORMAL ("Answer: Praat doesn't know it has to send the #Zoom command to the editor "
	"window called ##Sound hello#. There could be several Sound editor windows on your "
	"screen. According to @@Scripting 7.1. Scripting an editor from a shell script@, "
	"you will have to say this explicitly:")
CODE ("Read from file... hello.wav")
CODE ("Edit")
CODE ("editor Sound hello")
CODE ("Zoom... 0.3 0.5")
NORMAL ("")
NORMAL ("#Problem: a line like \"Number = 1\" does not work.")
NORMAL ("Solution: names of variables should start with a lower-case letter.")
NORMAL ("")
NORMAL ("#Question: why do names of variables have to start with a lower-case letter? "
	"I would like to do things like \"F0 = Get mean pitch\".")
NORMAL ("Answer: Praat scripts combine button commands with things that only occur "
	"in scripts. Button commands always start with a capital letter, e.g. \"Play\". "
	"Script command always start with lower case, e.g. \"echo Hello\". "
	"A minimal pair is \"select\", which simulates a mouse click in the object list, "
	"versus \"Select...\", which sets the selection in editor windows. Variable names "
	"that start with a capital letter would be rather ambiguous in assignments, "
	"as in \"x = Get\", where \"Get\" would be a variable, versus \"x = Get mean\", "
	"where \"Get mean\" is a button command. To prevent this, Praat enforces "
	"a rigorous lower-case/upper-case distinction.")
NORMAL ("")
NORMAL ("#Question: how do I convert a number into a string?")
NORMAL ("Answer: a\\$  = \"'a'\"")
NORMAL ("#Question: how do I convert a string into a number?")
NORMAL ("Answer: a = 'a\\$ '")
MAN_END

MAN_BEGIN ("FAQ: Spectrograms", "ppgb", 20030916)
NORMAL ("#Problem: the background is grey instead of white (too little contrast)")
NORMAL ("Solution: reduce the \"dynamic range\" in the spectrogram settings. The standard value is 50 dB, "
	"which is fine for detecting small things like plosive voicing in well recorded speech. "
	"For gross features like vowel formants, or for noisy speech, you may want to change the dynamic range "
	"to 40 or even 30 dB.")
MAN_END

MAN_BEGIN ("File menu", "ppgb", 20021204)
INTRO ("One of the menus in all @editors, in the @manual, and in the @@Picture window@.")
MAN_END

MAN_BEGIN ("Filtering", "ppgb", 20030316)
INTRO ("This tutorial describes the use of filtering techniques in P\\s{RAAT}. "
	"It assumes you are familiar with the @Intro.")
ENTRY ("Frequency-domain filtering")
NORMAL ("Modern computer techniques make possible an especially simple batch filtering method: "
	"multiplying the complex spectrum in the frequency domain by any real-valued filter function. "
	"This leads to a zero phase shift for each frequency component. The impulse response is symmetric "
	"in the time domain, which also means that the filter is %acausal: the filtered signal will show components "
	"before they start in the original.")
LIST_ITEM ("\\bu @@Spectrum: Filter (pass Hann band)...@")
LIST_ITEM ("\\bu @@Spectrum: Filter (stop Hann band)...@")
LIST_ITEM ("\\bu @@Sound: Filter (pass Hann band)...@")
LIST_ITEM ("\\bu @@Sound: Filter (stop Hann band)...@")
LIST_ITEM ("\\bu @@Sound: Filter (formula)...@")
NORMAL ("Spectro-temporal:")
LIST_ITEM ("\\bu @@band filtering in the frequency domain@")
ENTRY ("Fast time-domain filtering")
NORMAL ("Some very fast Infinite Impulse Response (IIR) filters can be defined in the time domain. "
	"These include recursive all-pole filters and pre-emphasis. These filters are causal but have non-zero phase shifts. "
	"There are versions that create new Sound objects:")
LIST_ITEM ("\\bu @@Sound: Filter (one formant)...@")
LIST_ITEM ("\\bu @@Sound: Filter (pre-emphasis)...@")
LIST_ITEM ("\\bu @@Sound: Filter (de-emphasis)...@")
NORMAL ("And there are in-line versions, which modify the existing Sound objects:")
LIST_ITEM ("\\bu @@Sound: Filter with one formant (in-line)...@")
LIST_ITEM ("\\bu @@Sound: Pre-emphasize (in-line)...@")
LIST_ITEM ("\\bu @@Sound: De-emphasize (in-line)...@")
ENTRY ("Convolution")
NORMAL ("A Finite Impulse Response (FIR) filter can be described as a sampled sound. "
	"Filtering with such a filter amounts to a %#convolution of the original sound and the filter:")
LIST_ITEM ("\\bu @@Sounds: Convolve@")
ENTRY ("Described elsewhere")
NORMAL ("Described in the @@Source-filter synthesis@ tutorial:")
LIST_ITEM ("\\bu @@Sound & Formant: Filter@")
LIST_ITEM ("\\bu @@Sound & FormantTier: Filter@")
LIST_ITEM ("\\bu @@LPC & Sound: Filter...@")
LIST_ITEM ("\\bu @@LPC & Sound: Filter (inverse)@")
MAN_END

MAN_BEGIN ("Formants & LPC menu", "ppgb", 20011107)
INTRO ("A menu that occurs in the @@Dynamic menu@ for a @Sound.")
NORMAL ("This menu contains commands for analysing the formant contours of the selected Sound:")
LIST_ITEM ("@@Sound: To Formant (burg)...")
LIST_ITEM ("@@Sound: To Formant (keep all)...")
LIST_ITEM ("@@Sound: To Formant (sl)...")
LIST_ITEM ("@@Sound: To LPC (autocorrelation)...")
LIST_ITEM ("@@Sound: To LPC (covariance)...")
LIST_ITEM ("@@Sound: To LPC (burg)...")
LIST_ITEM ("@@Sound: To LPC (marple)...")
LIST_ITEM ("@@Sound: To MFCC...")
MAN_END

MAN_BEGIN ("Get first formant", "ppgb", 20011107)
INTRO ("One of the commands in the @@Query menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN ("Get pitch", "ppgb", 20010417)
INTRO ("One of the commands in the @@Query menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN ("Get second formant", "ppgb", 20011107)
INTRO ("One of the commands in the @@Query menu@ of the @SoundEditor and the @TextGridEditor.")
MAN_END

MAN_BEGIN ("Goodies", "ppgb", 20050822)
INTRO ("The title of a submenu of the @@Praat menu@.")
MAN_END

MAN_BEGIN ("Info", "ppgb", 19980101)
INTRO ("One of the fixed buttons in the @@Object window@.")
ENTRY ("Availability")
NORMAL ("You can choose this command after choosing one object.")
ENTRY ("Purpose")
NORMAL ("To get some information about the selected object.")
ENTRY ("Behaviour")
NORMAL ("The information will appear in the @@Info window@.")
MAN_END

MAN_BEGIN ("Info window", "ppgb", 20030528)
INTRO ("A text window into which many query commands write their answers.")
NORMAL ("You can select text from this window and copy it to other places.")
NORMAL ("In a @@Praat script@, you can bypass the Info window by having a query command "
	"writing directly into a script variable.")
NORMAL ("Apart from the @Info command, which writes general information about the selected object, "
	"the following more specific commands also write into the Info window:")
MAN_END

MAN_BEGIN ("Inspect", "ppgb", 19960904)
INTRO ("One of the fixed buttons in the @@Object window@.")
NORMAL ("You can use this command after selecting one object in the list.")
NORMAL ("The contents of the selected object will become visible in a Data Editor. "
	"You can then view and change the data in the object, "
	"but beware: changing the data directly in this way may render them inconsistent.")
NORMAL ("Changes that you make to the data with another Editor (e.g., a SoundEditor), "
	"or with the commands under #%Modify, "
	"are immediately reflected in the top-level Data Editor; "
	"any subeditors are destroyed, however, because they may now refer to invalid data.")
NORMAL ("Changes that you make to the data with a Data Editor, "
	"are immediately reflected in any open type-specific Editors (e.g., a SoundEditor).")
MAN_END

MAN_BEGIN ("Intro", "ppgb", 20041028)
INTRO ("This is an introductory tutorial to P\\s{RAAT}, a computer program "
	"with which you can analyse, synthesize, and manipulate speech, "
	"and create high-quality pictures for your articles and thesis. "
	"You are advised to work through all of this tutorial.")
NORMAL ("You can read this tutorial sequentially with the help of the \"1 >\" and \"< 1\" buttons, "
	"or go to the desired information by clicking on the blue links.")
LIST_ITEM ("@@Intro 1. How to get a sound@: "
	"@@Intro 1.1. Recording a sound|record@, "
	"@@Intro 1.2. Reading a sound from disk|read@, "
	"@@Intro 1.3. Creating a sound from a formula|formula@.")
LIST_ITEM ("@@Intro 2. What to do with a sound@: "
	"@@Intro 2.1. Writing a sound to disk|write@, "
	"@@Intro 2.2. Viewing and editing a sound|view@.")
LIST_ITEM ("@@Intro 3. Spectral analysis")
LIST_ITEM1 ("spectrograms: @@Intro 3.1. Viewing a spectrogram|view@, "
	"@@Intro 3.2. Configuring the spectrogram|configure@, "
	"@@Intro 3.3. Querying the spectrogram|query@, "
	"@@Intro 3.4. The Spectrogram object|the Spectrogram object@.")
LIST_ITEM1 ("spectral slices: @@Intro 3.5. Viewing a spectral slice|view@, "
	"@@Intro 3.6. Configuring the spectral slice|configure@, "
	"@@Intro 3.7. The Spectrum object|the Spectrum object@.")
LIST_ITEM ("@@Intro 4. Pitch analysis")
LIST_ITEM1 ("pitch contours: @@Intro 4.1. Viewing a pitch contour|view@, "
	"@@Intro 4.2. Configuring the pitch contour|configure@, "
	"@@Intro 4.3. Querying the pitch contour|query@, "
	"@@Intro 4.4. The Pitch object|the Pitch object@.")
LIST_ITEM ("@@Intro 5. Formant analysis")
LIST_ITEM1 ("formant contours: @@Intro 5.1. Viewing formant contours|view@, "
	"@@Intro 5.2. Configuring the formant contours|configure@, "
	"@@Intro 5.3. Querying the formant contours|query@, "
	"@@Intro 5.4. The Formant object|the Formant object@.")
LIST_ITEM ("@@Intro 6. Intensity analysis")
LIST_ITEM1 ("intensity contours: @@Intro 6.1. Viewing an intensity contour|view@, "
	"@@Intro 6.2. Configuring the intensity contour|configure@, "
	"@@Intro 6.3. Querying the intensity contour|query@, "
	"@@Intro 6.4. The Intensity object|the Intensity object@.")
LIST_ITEM ("@@Intro 7. Annotation")
LIST_ITEM ("@@Intro 8. Manipulation@: of "
	"@@Intro 8.1. Manipulation of pitch|pitch@, "
	"@@Intro 8.2. Manipulation of duration|duration@, "
	"@@Intro 8.3. Manipulation of intensity|intensity@, "
	"@@Intro 8.4. Manipulation of formants|formants@.")
NORMAL ("There are also more specialized tutorials:")
LIST_ITEM ("Phonetics:")
LIST_ITEM1 ("\\bu Voice analysis (jitter, shimmer, noise): @Voice")
LIST_ITEM1 ("\\bu Listening experiments: @@ExperimentMFC@")
LIST_ITEM1 ("\\bu @@Sound files@")
LIST_ITEM1 ("\\bu @@Filtering@")
LIST_ITEM1 ("\\bu @@Source-filter synthesis@")
LIST_ITEM1 ("\\bu @@Articulatory synthesis@")
LIST_ITEM ("Learning:")
LIST_ITEM1 ("\\bu @@Feedforward neural networks@")
LIST_ITEM1 ("\\bu @@OT learning@")
LIST_ITEM ("Statistics:")
LIST_ITEM1 ("\\bu @@Principal component analysis@")
LIST_ITEM1 ("\\bu @@Multidimensional scaling@")
LIST_ITEM1 ("\\bu @@Discriminant analysis@")
LIST_ITEM ("General:")
LIST_ITEM1 ("\\bu @@Printing@")
LIST_ITEM1 ("\\bu @@Scripting@")
ENTRY ("The authors")
NORMAL ("The Praat program was created by Paul Boersma and David Weenink of "
	"the Institute of Phonetics Sciences of the University of Amsterdam. "
	"Home page: ##http://www.praat.org# or ##http://www.fon.hum.uva.nl/praat/#.")
NORMAL ("For questions and suggestions, mail to the Praat discussion list, "
	"which is reachable from the Praat home page, or directly to ##paul.boersma\\@ uva.nl#.")
MAN_END

MAN_BEGIN ("Intro 1. How to get a sound", "ppgb", 20021212)
INTRO ("Most of the things most people do with Praat start with a sound. "
	"There are at least three ways to get a sound:")
LIST_ITEM ("@@Intro 1.1. Recording a sound")
LIST_ITEM ("@@Intro 1.2. Reading a sound from disk")
LIST_ITEM ("@@Intro 1.3. Creating a sound from a formula")
MAN_END

MAN_BEGIN ("Intro 1.1. Recording a sound", "ppgb", 20041126)
#ifdef __MACH__
	INTRO ("To record a speech sound into Praat, you need a computer with a microphone.")
	NORMAL ("To record from the microphone, perform the following steps:")
#else
	INTRO ("To record a speech sound into Praat, you need a computer with a microphone. "
		"If you do not have a microphone, try to record from an audio CD instead.")
	NORMAL ("To record from the microphone (or the CD), perform the following steps:")
#endif
LIST_ITEM ("1. Choose @@Record mono Sound...@ from the @@New menu@ in the @@Object window@. "
	"A @SoundRecorder window will appear on your screen.")
#if defined (_WIN32)
	LIST_ITEM ("2. Choose the appropriate input device, namely the microphone, by the following steps. "
		"If there is a small loudspeaker symbol in the Windows Start bar, double click it and you will see the %playing mixer. "
		"If there is no loudspeaker symbol, go to ##Control Panels#, then ##Sounds and Audio Devices#, then #Volume, then #Advanced, "
		"and you will finally see the playing mixer. "
		"Once you see the Windows playing mixer, choose #Properties from the #Option menu, "
		"then click #Recording, then #OK. You now see the %recording mixer, where you can select the microphone "
		"(if you do not like this complicated operation, try Praat on Macintosh or Linux instead of Windows).")
#elif defined (__MACH__)
	LIST_ITEM ("2. In the SoundRecorder window, choose the appropriate input device, e.g. choose ##Internal microphone#.")
#else
	LIST_ITEM ("2. In the SoundRecorder window, choose the appropriate input device, i.e. choose #Microphone (or #CD, or #Line).")
#endif
#ifdef __MACH__
	LIST_ITEM ("3. Use the #Record and #Stop buttons to record a few seconds of your speech.")
#else
	LIST_ITEM ("3. Use the #Record and #Stop buttons to record a few seconds of your speech "
		"(or a few seconds of music from your playing CD).")
#endif
LIST_ITEM ("4. Use the #Play button to hear what you have recorded.")
LIST_ITEM ("5. Repeat steps 3 and 4 until you are satisfied with your recording.")
LIST_ITEM ("6. Click the ##Save to list# button. Your recording will now appear in the Object window, "
	"where it will be called \"Sound sound\".")
LIST_ITEM ("7. You can now close the SoundRecorder window.")
LIST_ITEM ("8. When you saved your sound to the Object window, some buttons appeared in that window. "
	"These buttons show you what you can do with the sound. Try the #Play and @Edit buttons.")
NORMAL ("For more information on recording, see the @SoundRecorder manual page.")
MAN_END

MAN_BEGIN ("Intro 1.2. Reading a sound from disk", "ppgb", 20041126)
INTRO ("Apart from recording a new sound from a microphone, you could read an existing sound file from your disk.")
NORMAL ("With @@Read from file...@ from the Read menu, "
	"P\\s{RAAT} will be able to read most standard types of sound files, e.g. WAV files. "
	"They will appear as @Sound objects in the Object window. For instance, if you open the file ##hello.wav#, "
	"an object called \"Sound hello\" will appear in the list.")
NORMAL ("If you do not have a sound file on your disk, you can download a WAV file (or so) from the Internet, "
	"then read that file into Praat with ##Read from file...#.")
MAN_END

MAN_BEGIN ("Intro 1.3. Creating a sound from a formula", "ppgb", 20041126)
INTRO ("If you have no microphone, no sound files on disk, and no access to the Internet, "
	"you could still create a sound from a formula with @@Create Sound...@ from the @@New menu@.")
MAN_END

MAN_BEGIN ("Intro 2. What to do with a sound", "ppgb", 20041126)
INTRO ("As soon as you have a @Sound in the @@List of Objects@, "
	"the buttons in the @@Dynamic menu@ (the right-hand part of the @@Object window@) "
	"will show you what you can do with it.")
LIST_ITEM ("@@Intro 2.1. Writing a sound to disk")
LIST_ITEM ("@@Intro 2.2. Viewing and editing a sound")
MAN_END

MAN_BEGIN ("Intro 2.1. Writing a sound to disk", "ppgb", 20041126)
INTRO ("There are several ways to write a sound to disk.")
NORMAL ("First, the @@File menu@ of the @SoundRecorder window contains commands to save the left "
	"channel, the right channel, or both channels of the recorded sound to any of four standard types "
	"of sound files (WAV, AIFC, NeXT/Sun, NIST). These four file types are all equally good for P\\s{RAAT}: "
	"P\\s{RAAT} will handle them equally well on every computer. The first three of these types will "
	"also be recognized by nearly all other sound-playing programs.")
NORMAL ("Then, once you have a @Sound object in the @@List of Objects@, "
	"you can save it in several formats with the commands in the @@Write menu@. "
	"Again, the WAV, AIFF, AIFC, NeXT/Sun, and NIST formats are equally fine.")
NORMAL ("For more information, see the @@Sound files@ tutorial.")
MAN_END

MAN_BEGIN ("Intro 2.2. Viewing and editing a sound", "ppgb", 20051218)
NORMAL ("To see the wave form of a @Sound that is in the list of objects, "
	"select that Sound and click @Edit. A @SoundEditor window will appear on your screen. "
	"You can zoom in and scroll to see the various parts of the sound in detail. "
	"You can select a part of the sound by dragging with the mouse. "
	"To play a part of the sound, click on any of the rectangles below it. "
	"To move a selected part of the sound to another location, use #Cut and #Paste from the #Edit menu. "
	"You can open sound windows for more than one sound, and then cut, copy, and paste between the sounds, "
	"just as you are used to do with text and pictures in word processing programs.")
NORMAL ("If your sound file is longer than a couple of minutes, "
	"or if you want to see and listen to both channels of a stereo sound, "
	"you may prefer to open it with @@Open long sound file...@. "
	"This puts a @LongSound object into the list. In this way, most of the sound will stay in the file on disk, "
	"and at most 60 seconds will be read into memory each time you play or view a part of it. "
	"To change these 60 seconds to something else, e.g. 500 seconds, choose ##LongSound prefs...# from the #Preferences submenu.")
MAN_END

MAN_BEGIN ("Intro 3. Spectral analysis", "ppgb", 20030316)
INTRO ("This section describes how you can analyse the spectral content of an existing sound. "
	"You will learn how to use %spectrograms and %%spectral slices%.")
LIST_ITEM ("@@Intro 3.1. Viewing a spectrogram")
LIST_ITEM ("@@Intro 3.2. Configuring the spectrogram")
LIST_ITEM ("@@Intro 3.3. Querying the spectrogram")
LIST_ITEM ("@@Intro 3.4. The Spectrogram object")
LIST_ITEM ("@@Intro 3.5. Viewing a spectral slice")
LIST_ITEM ("@@Intro 3.6. Configuring the spectral slice")
LIST_ITEM ("@@Intro 3.7. The Spectrum object")
MAN_END

MAN_BEGIN ("Intro 3.1. Viewing a spectrogram", "ppgb", 20030513)
INTRO ("To see the spectral content of a sound as a function of time, "
	"select a @Sound or @LongSound object and choose @Edit. "
	"A @SoundEditor or @LongSoundEditor window will appear on your screen. "
	"In the entire bottom half of this window you will see a greyish image, which is called a %spectrogram. "
	"If you do not see it, choose @@Show spectrogram@ from the #Spectrogram menu.")
NORMAL ("The spectrogram is a @@spectro-temporal representation@ of the sound. "
	"The horizontal direction of the spectrogram represents @time, the vertical direction represents @frequency. "
	"The time scale of the spectrogram is the same as that of the waveform, so the spectrogram reacts "
	"to your zooming and scrolling. "
	"To the left of the spectrogram, you see the frequency scale. The frequency at the bottom of the spectrogram "
	"is usually 0 Hz (Hertz, cps, cycles per second), and a common value for the frequency at the top is 5000 Hz.")
NORMAL ("Darker parts of the spectrogram mean higher energy densities, lighter parts mean lower energy densities. "
	"If the spectrogram has a dark area around a time of 1.2 seconds and a frequency of 4000 Hz, "
	"this means that the sound has lots of energy for those high frequencies at that time. "
	"For many examples of spectrograms of speech sounds, see the textbook by @@Ladefoged (2001)@ and "
	"the reference work by @@Ladefoged & Maddieson (1996)@.")
NORMAL ("To see what time and frequency a certain part of the spectrogram is associated with, "
	"just click on the spectrogram and you will see the vertical time cursor showing the time above "
	"the waveform and the horizontal frequency cursor showing the frequency to the left of the spectrogram. "
	"This is one of the ways to find the %formant frequencies for vowels, or the main spectral peaks "
	"for fricatives.")
ENTRY ("Hey, there are white vertical stripes at the edges!")
NORMAL ("This is normal. Spectral analysis requires an %%analysis window% of a certain duration. "
	"For instance, if P\\s{RAAT} wants to know the spectrum at 1.342 seconds, it needs to include information "
	"about the signal in a 10-milliseconds window around this time point, i.e., P\\s{RAAT} will use "
	"signal information about all times between 1.337 and 1.347 seconds. At the very edges of the sound, "
	"this information is not available: "
	"if the sound runs from 0 to 1.8 seconds, no spectrum can be computed between 0 and 0.005 "
	"seconds or between 1.795 and 1.800 seconds. Hence the white stripes. If you do not see them "
	"immediately when you open the sound, zoom in on the beginning or end of the sound.")
NORMAL ("When you zoom in on the middle of the sound (or anywhere not near the edges), the white stripes vanish. "
	"Suddenly you see only the time stretch between 0.45 and 1.35 seconds, for instance. "
	"But P\\s{RAAT} did not forget what the signal looks like just outside the edges of this time window. "
	"To display a spectrogram from 0.45 to 1.35 seconds, P\\s{RAAT} will use information from the wave form "
	"between 0.445 and 1.355 seconds, and if this is available, you will see no white stripes at the edges of the window.")
ENTRY ("Hey, it changes when I scroll!")
NORMAL ("This is normal as well, especially for long windows. If your visible time window is 20 seconds long, "
	"and the window takes up 1000 screen pixels horizontally, "
	"then you might think that every one-pixel-wide vertical line should represent the spectrum of 20 milliseconds of sound. "
	"But for reasons of computation speed, P\\s{RAAT} will only show the spectrum of the part of the sound "
	"that lies around the centre of those 20 milliseconds, "
	"not the average or sum of all the spectra in those 20 milliseconds. "
	"This %undersampling of the underlying spectrogram is different from what happens in the drawing of the wave form, "
	"where a vertical black line connects the minimum and maximum amplitude of all the samples that fall inside a "
	"screen pixel. We cannot do something similar for spectrograms. And since scrolling goes by fixed time steps "
	"(namely, 5 percent of the duration of the visible window), rather than by a whole number of screen pixels, "
	"the centres of the pixels will fall in different parts of the spectrogram with each scroll. "
	"Hence the apparent changes. If your visible window is shorter than a couple of seconds, "
	"the scrolling spectrogram will appear much smoother.")
NORMAL ("The darkness of the spectrogram will also change when you scroll, because the visible part with the most "
	"energy is defined as black. When a very energetic part of the signal scrolls out of view, the spectrogram "
	"will turn darker. The next section will describe a way to switch this off.")
MAN_END

MAN_BEGIN ("Intro 3.2. Configuring the spectrogram", "ppgb", 20030917)
NORMAL ("With @@Spectrogram settings...@ from the #Spectrogram menu, "
	"you can determine how the spectrogram is computed and how it is displayed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (\"factory settings\"), which appear "
	"when you click ##Standards#.")
TAG ("%%View range% (Hz)")
DEFINITION ("the range of frequencies to display. The standard is 0 Hz at the bottom and 5000 Hz at the top. "
	"If this maximum frequency is higher than the Nyquist frequency of the Sound "
	"(which is half its sampling frequency), some values in the spectrogram will be zero, and the higher "
	"frequencies will be drawn in white. You can see this if you record a Sound at 44100 Hz and set the "
	"view range from 0 Hz to 25000 Hz.")
TAG ("%%Window length")
DEFINITION ("the duration of the analysis window. If this is 0.005 seconds (the standard), "
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
TAG ("%%Dynamic range% (dB)")
DEFINITION ("All values that are more than %%Dynamic range% dB below the maximum (perhaps after dynamic compression, "
	"see @@Advanced spectrogram settings...@) "
	"will be drawn in white. Values in-between have appropriate shades of grey. Thus, if the highest "
	"peak in the spectrogram has a height of 30 dB/Hz, and the dynamic range is 50 dB (which is the standard value), "
	"then values below -20 dB/Hz will be drawn in white, and values between -20 dB/Hz and 30 dB/Hz will be drawn "
	"in various shades of grey.")
ENTRY ("The bandwidth")
NORMAL ("To see how the window length influences the bandwidth, "
	"first create a 1000-Hz sine wave with @@Create Sound...@ "
	"by typing $$1/2 * sin (2*pi*1000*x)$ as the formula, then click #Edit. "
	"The spectrogram will show a horizontal black line. "
	"You can now vary the window length in the spectrogram settings and see how the thickness "
	"of the lines varies. The line gets thinner if you raise the window length. "
	"Apparently, if the analysis window comprises more periods of the wave, "
	"the spectrogram can tell us the frequency of the wave with greater precision.")
NORMAL ("To see this more precisely, create a sum of two sine waves, with frequencies of 1000 and 1200 Hz. "
	"the formula is $$1/4 * sin (2*pi*1000*x) + 1/4 * sin (2*pi*1200*x)$. In the editor, you will see "
	"a single thick band if the analysis window is short (5 ms), and two separate bands if the analysis "
	"window is long (30 ms). Apparently, the frequency resolution gets better with longer analysis windows.")
NORMAL ("So why don't we always use long analysis windows? The answer is that their time resolution is poor. "
	"To see this, create a sound that consists of two sine waves and two short clicks. The formula is "
	"$$0.02*(sin(2*pi*1000*x)+sin(2*pi*1200*x)) + (col=10000)+(col=10200)$. "
	"If you view this sound, you can see that the two clicks will overlap "
	"in time if the analysis window is long, and that the sine waves overlap in frequency if the "
	"analysis window is short. Apparently, there is a trade-off between time resolution and "
	"frequency resolution. One cannot know both the time and the frequency with great precision.")
ENTRY ("Advanced settings")
NORMAL ("The Spectrum menu also contains @@Advanced spectrogram settings...@.")
MAN_END

MAN_BEGIN ("Advanced spectrogram settings...", "ppgb", 20030917)
ENTRY ("Optimization")
TAG ("%%Number of time steps%")
DEFINITION ("the maximum number of points along the time window for which P\\s{RAAT} has to compute "
	"the spectrum. If your screen is not wider than 1200 pixels, then the standard of 1000 is "
	"appropriate, since there is no point in computing more than one spectrum per one-pixel-wide vertical line. "
	"If you have a really wide screen, you may see improvement if you raise this number to 1500.")
TAG ("%%Number of frequency steps%")
DEFINITION ("the maximum number of points along the frequency axis for which P\\s{RAAT} has to compute "
	"the spectrum. If your screen is not taller than 768 pixels, then the standard of 250 is "
	"appropriate, since there is no point in computing more than one spectrum per one-pixel-height horizontal line. "
	"If you have a really tall screen, you may see improvement if you raise this number.")
NORMAL ("For purposes of computation speed, Praat may decide to change the time step and the frequency step. "
	"This is because the time step never needs to be smaller than 1/(8\\Vr\\pi) of the window length, "
	"and the frequency step never needs to be smaller than (\\Vr\\pi)/8 of the inverse of the window length. "
	"For instance, if the window length is 30 ms, "
	"the actual time step will never be less than 30/(8\\Vr\\pi) = 2.116 ms. "
	"And if the window length is 5 ms, "
	"the actual frequency step will never be less than (\\Vr\\pi)/8/0.005 = 44.32 Hz.")
ENTRY ("Spectrogram analysis settings")
TAG ("%%Method")
DEFINITION ("there is currently only one method available in this window for computing a spectrum from "
	"a sound: the Fourier transform.")
TAG ("%%Window shape")
DEFINITION ("the shape of the analysis window. To compute the spectrum at, say, 3.850 seconds, "
	"samples that lie close to 3.850 seconds are given more weight than samples further away. "
	"The relative extent to which each sample contributes to the spectrum is given by the window shape. "
	"You can choose from: Gaussian, Square (none, rectangular), Hamming (raised sine-squared), "
	"Bartlett (triangular), Welch (parabolic), and Hanning (sine-squared). "
	"The Gaussian window is superior, as it gives no %sidelobes in your spectrogram (see below); "
	"it analyzes a factor of 2 slower than the other window shapes, "
	"because the analysis is actually performed on twice as many samples per frame.")
ENTRY ("Sidelobes; anybody wants to win a cake?")
NORMAL ("The Gaussian window is the only shape that we can consider seriously as a candidate for "
	"the analysis window. To see this, create a 1000-Hz sine wave with @@Create Sound...@ "
	"by typing $$1/2 * sin (2*pi*1000*x)$ as the formula, then click #Edit. "
	"If the window shape is Gaussian, the spectrogram will show a horizontal black line. "
	"If the window shape is anything else, the spectrogram will show many horizontal grey lines (%sidelobes), "
	"which do not represent anything that is available in the signal. They are artifacts of the "
	"window shapes.")
NORMAL ("We include these other window shapes only for pedagogical purposes "
	"and because the Hanning and Hamming windows have traditionally been used in other programs before "
	"computers were as fast as they are now (a spectrogram is computed twice as fast "
	"with these other windows). Several other programs still use these inferior window shapes, and you are "
	"likely to run into people who claim that the Gaussian window has disadvantages. "
	"We promise such people a large cake if they can come up with sounds that look better "
	"with Hanning or Hamming windows than with a Gaussian window. An example of the reverse is easy "
	"to find; we have just seen one.")
ENTRY ("Spectrogram blackness settings")
TAG ("%%Autoscaling%")
TAG ("%%Maximum% (dB/Hz)")
DEFINITION ("all parts of the spectrogram that have a power above %maximum (after preemphasis) "
	"will be drawn in black. The standard maximum is 100 dB/Hz, but if %autoscaling is on (which is the standard), "
	"P\\s{RAAT} will use the maximum of the visible part of the spectrogram instead; "
	"this ensures that the window will always look well, but it also means that the blackness "
	"of a certain part of the spectrogram will change as you scroll.")
TAG ("%%Preemphasis% (dB/octave)")
DEFINITION ("determines the steepness of a high-pass filter, "
	"i.e., how much the power of higher frequencies will be raised before drawing, as compared to lower frequencies. "
	"Since the spectral slope of human vowels is approximately -6 dB per octave, "
	"the standard value for this setting is +6 dB per octave, "
	"so that the spectrum is flattened and the higher formants look as strong as the lower ones. "
	"When you raise the preemphasis, frequency bands above 1000 Hz will become darker, those below 1000 Hz will become lighter.")
TAG ("%%Dynamic compression")
DEFINITION ("determines how much stronger weak spectra should be made before drawing. "
	"Normally, this parameter is between 0 and 1. If it is 0 (the standard value), there is no dynamic compression. "
	"If it is 1, all spectra will be drawn equally strong, "
	"i.e., all of them will contain frequencies that are drawn in black. "
	"If this parameter is 0.4 and the global maximum is at 80 dB, then a spectrum with a maximum at 20 dB "
	"(which will normally be drawn all white if the dynamic range is 50 dB), "
	"will be raised by 0.4 * (80 - 20) = 24 dB, "
	"so that its maximum will be seen at 44 dB (thus making this frame visible).")
MAN_END

MAN_BEGIN ("Intro 3.3. Querying the spectrogram", "ppgb", 20030403)
NORMAL ("If you click anywhere inside the spectrogram, a cursor cross will appear, "
	"and you will see the time and frequency in red at the top and to the left of the window. "
	"To see the time in the Info window, "
	"choose ##Get cursor# from the #Query menu or press the F6 key. "
	"To see the frequency in the Info window, "
	"choose ##Get frequency# from the #Spectrum menu.")
NORMAL ("To query the power of the spectrogram at the cursor cross, "
	"choose ##Get spectral power at cursor cross# from the #Spectrum menu or press the F9 key. "
	"The Info window will show you the power density, expressed in Pascal^2/Hz.")
MAN_END

MAN_BEGIN ("Intro 3.4. The Spectrogram object", "ppgb", 20030317)
NORMAL ("To print a spectrogram, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to create a @Spectrogram object in the @@List of Objects@. "
	"You do this either by choosing ##Extract visible spectrogram# "
	"from the Spectrum menu in the SoundEditor or TextGridEditor window, "
	"or by selecting a Sound object in the list and choosing @@Sound: To Spectrogram...@ from the #Spectrum menu. "
	"In either case, a new Spectrogram object will appear in the list. "
	"To draw this Spectrogram object to the @@Picture window@, "
	"select it and choose the @@Spectrogram: Paint...@ command. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard.")
MAN_END

MAN_BEGIN ("Intro 3.5. Viewing a spectral slice", "ppgb", 20030316)
INTRO ("With ##View spectral slice# from the #Spectrum menu in the @SoundEditor and the @TextGridEditor, "
	"you can see the frequency spectrum at the time cursor "
	"or the average frequency spectrum in the time selection.")
ENTRY ("Spectral slice at the cursor")
NORMAL ("If you click anywhere in the wave form of the SoundEditor or TextGridEditor windows, "
	"a cursor will appear at that time. If you then choose ##View spectral slice#, "
	"P\\s{RAAT} will create a @Spectrum object named %slice in the Objects window and show it in a @SpectrumEditor window. "
	"In this way, you can inspect the frequency contents of the signal around the cursor position.")
ENTRY ("Spectral slice from a selection")
NORMAL ("If you drag the mouse through the wave form of the SoundEditor or TextGridEditor windows, "
	"a @@time selection@ will appear. If you then choose ##View spectral slice#, "
	"P\\s{RAAT} will again create a @Spectrum object named %slice in the Objects window and show it in a @SpectrumEditor window. "
	"In this way, you can inspect the frequency contents of the signal in the selection.")
MAN_END


MAN_BEGIN ("Intro 3.6. Configuring the spectral slice", "ppgb", 20030316)
ENTRY ("Spectral slice at the cursor")
NORMAL ("What P\\s{RAAT} does precisely, depends on your Spectrogram settings. "
	"Suppose that the %%window length% setting is 0.005 seconds (5 milliseconds). "
	"If the %%window shape% is not Gaussian, P\\s{RAAT} will extract the part of the sound "
	"that runs from 2.5 milliseconds before the cursor to 2.5 ms after the cursor. "
	"P\\s{RAAT} then multiplies this 5 ms long signal by the window shape, then computes a spectrum "
	"with the method of @@Sound: To Spectrum...@, which is put into the Objects window and opened in an editor window. "
	"If the window shape is Gaussian, P\\s{RAAT} will extract a part of the sound "
	"that runs from 5 milliseconds before the cursor to 5 ms after the cursor. The spectrum will then be based "
	"on a `physical' window length of 10 ms, although the `effective' window length is still 5 ms "
	"(see @@Intro 3.2. Configuring the spectrogram@ for details).")
ENTRY ("Spectral slice from a selection")
NORMAL ("What P\\s{RAAT} does precisely, again depends on the %%window shape% of your Spectrogram settings. "
	"Suppose that your selection is 50 ms long. P\\s{RAAT} will extract the entire selection, "
	"then multiply this 50 ms long signal by the window shape, then compute a spectrum, put it into the Objects window and open it an editor window. "
	"This procedure is equivalent to choosing ##Extract windowed selection...# (with a %%relative duration% of 1.0), "
	"followed by ##To Spectrum...# (with %fast switched on), followed by #Edit.")
NORMAL ("If the window is Gaussian, P\\s{RAAT} will still only use the selection, without doubling its duration. "
	"This means that the spectrum that you see in this case will mainly be based on the centre half of the selection, "
	"and the signal near the edges will be largely ignored.")
MAN_END

MAN_BEGIN ("Intro 3.7. The Spectrum object", "ppgb", 20030403)
NORMAL ("To compute a Fourier frequency spectrum of an entire sound, "
	"select a @Sound object and choose @@Sound: To Spectrum...|To Spectrum...@ from the #Spectrum menu. "
	"A new @Spectrum object will appear in the @@List of Objects@. "
	"To view or modify it (or listen to its parts), click @Edit. "
	"To print it, choose one of the #Draw commands to draw the Spectrum object to the @@Picture window@ first.")
MAN_END

MAN_BEGIN ("Intro 4. Pitch analysis", "ppgb", 20030316)
INTRO ("This section describes how you can analyse the pitch contour of an existing sound.")
LIST_ITEM ("@@Intro 4.1. Viewing a pitch contour")
LIST_ITEM ("@@Intro 4.2. Configuring the pitch contour")
LIST_ITEM ("@@Intro 4.3. Querying the pitch contour")
LIST_ITEM ("@@Intro 4.4. The Pitch object")
MAN_END

MAN_BEGIN ("Intro 4.1. Viewing a pitch contour", "ppgb", 20030521)
NORMAL ("To see the pitch contour of an existing sound as a function of time, "
	"select a @Sound or @LongSound object and choose @Edit. "
	"A @SoundEditor window will appear on your screen. "
	"The bottom half of this window will contain a pitch contour, drawn as a blue line or as a sequence of blue dots. "
	"If you do not see the pitch contour, choose @@Show pitch@ from the #Pitch menu.")
NORMAL ("To the right of the window, you may see three pitch values, written with blue digits: "
	"at the bottom, you see the floor of the viewable pitch range, perhaps 75 Hz; at the top, "
	"you see the ceiling of the pitch range, perhaps 600 Hz; and somewhere in between, you see the pitch value "
	"at the cursor, or the average pitch in the selection.")
MAN_END

MAN_BEGIN ("Intro 4.2. Configuring the pitch contour", "ppgb", 20050830)
NORMAL ("With @@Pitch settings...@ from the #Pitch menu, "
	"you can determine how the pitch contour is displayed and how it is computed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (\"factory settings\"), which appear "
	"when you click #Standards.")
ENTRY ("The %%pitch range% setting")
NORMAL ("This is the most important setting for pitch analysis. The standard range is from 75 to 500 Hertz, "
	"which means that the pitch analysis method will only find values between 75 and 500 Hz. "
	"The range that you set here will be shown to the right of the analysis window.")
NORMAL ("For a male voice, you may want to set the floor to 75 Hz, and the ceiling to 300 Hz; "
	"for a female voice, set the range to 100-500 Hz instead. For creaky voice you will want to set it much "
	"lower than 75 Hz.")
NORMAL ("Here is why you have to supply these settings. If the pitch floor is 75 Hz, "
	"the pitch analysis method requires a 40-millisecond analysis window, "
	"i.e., in order to measure the F0 at a time of, say, 0.850 seconds, "
	"P\\s{RAAT} needs to consider a part of the sound that runs from 0.830 to 0.870 seconds. "
	"These 40 milliseconds correspond to 3 maximum pitch periods (3/75 = 0.040). "
	"If you set the pitch floor down to 25 Hz, the analysis window will grow to 120 milliseconds "
	"(which is again 3 maximum pitch periods), i.e., all times between 0.790 and 0.910 seconds will be considered. "
	"This makes it less easy to see fast F0 changes.")
NORMAL ("So setting the floor of the pitch range is a technical requirement for the pitch analysis. "
	"If you set it too low, you will miss very fast F0 changes, and if you set it too high, "
	"you will miss very low F0 values. For children's voices you can often use 200 Hz, "
	"although 75 Hz will still give you the same time resolution as you get for the males.")
ENTRY ("The %units setting")
NORMAL ("This setting determines the units of the vertical pitch scale. Most people like to see the pitch range "
	"in Hertz, but there are several other possibilities.")
ENTRY ("Advanced settings")
NORMAL ("The Pitch menu also contains @@Advanced pitch settings...@.")
MAN_END

MAN_BEGIN ("Time step settings...", "ppgb", 20031003)
INTRO ("A command in the #View menu of the @SoundEditor and @TextGridEditor "
	"to determine the time interval between consecutive measurements "
	"of pitch, formants, and intensity.")
ENTRY ("Automatic time steps")
NORMAL ("It is recommended that you set the %%Time step strategy% to #Automatic. "
	"In this way, Praat computes just enough pitch, formant, and intensity values to draw "
	"reliable pitch, formant, and intensity contours. In general, Praat will compute 4 values "
	"within an analysis window (\"four times oversampling\").")
NORMAL ("As described in @@Sound: To Pitch...@, Praat's standard time step for pitch analysis is 0.75 divided by the pitch floor, "
	"e.g., if the pitch floor is 75 Hz, the time step will be 0.01 seconds. "
	"In this way, there will be 4 pitch measurements within an analysis window, which is 3 / (75 Hz) = 40 milliseconds long.")
NORMAL ("As described in @@Sound: To Formant (burg)...@, Praat's standard time step for formant measurements is the %%Window length% divided by 4, "
	"e.g. if the window length is 0.025 seconds, the time step will be 6.25 milliseconds.")
NORMAL ("As described in @@Sound: To Intensity...@, Praat's standard time step for intensity measurements is 0.8 divided by the pitch floor, "
	"e.g. if the pitch floor is 75 Hz, the time step will be 10.6666667 milliseconds. "
	"In this way, there will be 4 intensity measurements within an intensity analysis window, "
	"which is 3.2 / (75 Hz) = 42.6666667 milliseconds long.")
ENTRY ("Fixed time step")
NORMAL ("You can override the automatic time step by setting the %%Time step strategy% to #Fixed. "
	"The %%Fixed time step% setting then determines the time step that Praat will use: "
	"if you set it to 0.001 seconds, Praat will compute pitch, formant, and intensity values for every millisecond. "
	"Beware that this can slow down the editor window appreciably, because this step is much smaller "
	"than usual values of the automatic time step (see above).")
NORMAL ("Enlarging the time step to e.g. 0.1 seconds will speed up the editor window "
	"but may render the pitch, formant, and intensity curves less exact (they become %undersampled), "
	"which will influence your measurements and the locations of the pulses.")
NORMAL ("If there are fewer than 2.0 pitch measurement points per analysis window, "
	"Praat will draw the pitch curve as separate little blue disks "
	"rather than as a continuous blue curve, in order to warn you of the undersampling. "
	"E.g. if the pitch floor is 75 Hz, Praat will draw the pitch curve as disks if the time step is greater than 0.02 seconds.")
ENTRY ("View-dependent time step")
NORMAL ("Another way to override the standard time step is by setting the %%Time step strategy% to ##View-dependent#. "
	"The %%Number of time steps per view% setting then determines the time step that Praat will use: "
	"if you set it to 100, Praat will always compute 100 pitch, formant, and intensity values within the view window. "
	"More precisely: if you zoom the view window to 3 seconds, Praat will show you 100 pitch, formant, and intensity points at distances "
	"of 0.03 seconds (or fewer than 100, if you are near the left or right edge of the signal). "
	"As with the %%Fixed time step% setting, Praat will draw the pitch as separate disks in case of undersampling. "
	"You may want to use this setting if you want the pitch curve to be drawn equally fast independently of the degree "
	"of zooming.")
MAN_END

MAN_BEGIN ("Advanced pitch settings...", "ppgb", 20030916)
INTRO ("A command in the #Pitch menu of the @SoundEditor or @TextGridEditor windows. "
	"Before changing the advanced pitch settings, make sure you understand "
	"@@Intro 4.2. Configuring the pitch contour@.")
ENTRY ("View range different from analysis range")
NORMAL ("Normally, the range of pitch values that can be seen in the editor window is equal to the range of pitch values "
	"that the analysis algorithm can determine. If you set the analysis range from 75 to 500 Hz, this will be the range "
	"you see in the editor window as well. If the pitch values in the curve happen to be between 350 and 400 Hz, "
	"you may want to zoom in to the 350-400 Hz pitch region. "
	"You will usually do this by changing the pitch range in the @@Pitch settings...@ dialog. "
	"However, the analysis range will also change in that case, so that the curve itself may change. "
	"If you do not want that, you can change the %%View range% settings "
	"from \"0.0 (= auto)\" \\-- \"0.0 (=auto)\" to something else, perhaps \"350\" \\-- \"400\".")
ENTRY ("Pitch analysis settings")
NORMAL ("For information about these, see @@Sound: To Pitch (ac)...@. The standard settings are best in most cases. "
	"For some pathological voices, you will want to set the voicing threshold to much less than the standard of 0.45, "
	"in order to get pitch values even in irregular parts of the signal.")
MAN_END

MAN_BEGIN ("Intro 4.3. Querying the pitch contour", "ppgb", 20040614)
NORMAL ("With @@Get pitch@ from the #Pitch menu in the @SoundEditor or @TextGridEditor, "
	"you get information about the pitch at the cursor or in the selection. "
	"If a cursor is visible in the window, ##Get pitch# writes to the @@Info window@ "
	"the linearly interpolated pitch at that time; "
	"if a time selection is visible inside the window, ##Get pitch# writes to the @@Info window@ "
	"the mean (average) pitch in the visible part of that selection; "
	"otherwise, ##Get pitch# writes the average pitch in the visible part of the sound.")
MAN_END

MAN_BEGIN ("Intro 4.4. The Pitch object", "ppgb", 20030316)
NORMAL ("The pitch contour that is visible in the @SoundEditor or @TextGridEditor window, "
	"can be copied as a separate @Pitch object to the @@List of Objects@. To do this, "
	"choose @@Extract visible pitch contour@ from the #Pitch menu.")
NORMAL ("Another way to get a separate Pitch object is to select a @Sound object in the list "
	"choose @@Sound: To Pitch...@ (preferred) or any of the other methods from the @@Periodicity menu@.")
NORMAL ("To view and modify the contents of a Pitch object, select it and choose @Edit. "
	"This creates a @PitchEditor window on your screen.")
NORMAL ("To save a pitch contour to disk, select the @Pitch object in the list and choose one of the commands in the @@Write menu@.")
NORMAL ("Later on, you can read the saved file again with @@Read from file...@ from the @@Read menu@.")
NORMAL ("To draw a @Pitch object to the @@Picture window@, select it and choose any of the commands in the @@Draw menu@. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard for inclusion in your word processor.")
MAN_END

MAN_BEGIN ("Intro 5. Formant analysis", "ppgb", 20030316)
INTRO ("This section describes how you can analyse the formant contours of an existing sound.")
LIST_ITEM ("@@Intro 5.1. Viewing formant contours")
LIST_ITEM ("@@Intro 5.2. Configuring the formant contours")
LIST_ITEM ("@@Intro 5.3. Querying the formant contours")
LIST_ITEM ("@@Intro 5.4. The Formant object")
MAN_END

MAN_BEGIN ("Intro 5.1. Viewing formant contours", "ppgb", 20030316)
NORMAL ("To see the formant contours of a sound as functions of time, select a @Sound or @LongSound object and choose @Edit. "
	"A @SoundEditor window will appear on your screen. "
	"The analysis part of this window will contain formant contours, drawn as red speckles. "
	"If you do not see the formant contours, choose @@Show formant@ from the #Formant menu.")
MAN_END

MAN_BEGIN ("Intro 5.2. Configuring the formant contours", "ppgb", 20030316)
NORMAL ("The formant analysis parameters, with you can set with the #Formant menu, are important. "
	"For a female voice, you may want to set the maximum frequency to 5500 Hz; "
	"for a male voice, set it to 5000 Hz instead. "
	"For more information about analysis parameters, see @@Sound: To Formant (burg)...@.")
MAN_END

MAN_BEGIN ("Intro 5.3. Querying the formant contours", "ppgb", 20040616)
NORMAL ("With @@Get first formant@ from the Formant menu in the @SoundEditor or @TextGridEditor, "
	"you get information about the first formant at the cursor or in the selection. "
	"If there is a cursor, ##Get first formant# writes to the @@Info window@ the linearly interpolated first formant at that time. "
	"If there is a true selection, ##Get first formant# writes to the @@Info window@ the mean first formant in the visble part of that selection. "
	"The same goes for @@Get second formant@ and so on.")
MAN_END

MAN_BEGIN ("Intro 5.4. The Formant object", "ppgb", 20030316)
NORMAL ("The formant contours that are visible in the @SoundEditor or @TextGridEditor window, "
	"can be copied as a separate @Formant object to the @@List of Objects@. To do this, "
	"choose @@Extract visible formant contour@ from the Formant menu.")
NORMAL ("Another way to get a separate Formant object is to select a @Sound object in the list "
	"choose @@Sound: To Formant (burg)...@ (preferred) or any of the other methods "
	"from the @@Formants & LPC menu@.")
ENTRY ("Saving formant contours to disk")
NORMAL ("To save formant contours to disk, select the @Formant object in the list and choose one of the commands in the @@Write menu@.")
NORMAL ("Later on, you can read the saved file again with @@Read from file...@ from the @@Read menu@.")
ENTRY ("Drawing formant contours")
NORMAL ("To draw a @Formant object to the @@Picture window@, select it and choose any of the commands in the @@Draw menu@. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard for inclusion in your word processor.")
MAN_END

MAN_BEGIN ("Intro 6. Intensity analysis", "ppgb", 20030316)
INTRO ("This section describes how you can analyse the intensity contour of an existing sound.")
LIST_ITEM1 ("@@Intro 6.1. Viewing an intensity contour")
LIST_ITEM1 ("@@Intro 6.2. Configuring the intensity contour")
LIST_ITEM1 ("@@Intro 6.3. Querying the intensity contour")
LIST_ITEM1 ("@@Intro 6.4. The Intensity object")
MAN_END

MAN_BEGIN ("Intro 6.1. Viewing an intensity contour", "ppgb", 20041123)
INTRO ("To see the intensity contour of a sound as a function of time, select a @Sound or @LongSound object and choose @Edit. "
	"A @SoundEditor window will appear on your screen. "
	"The analysis part of this window will contain an intensity contour, drawn as a yellow or green line "
	"(choose ##Show intensity# from the #Intensity menu if it is not visible). "
	"This also works in the @TextGridEditor.")
MAN_END

MAN_BEGIN ("Intro 6.2. Configuring the intensity contour", "ppgb", 20041123)
INTRO ("With ##Intensity settings...# from the #Intensity menu, "
	"you can control how the intensity contour is computed and how it is shown.")
ENTRY ("The view range")
NORMAL ("By changing these two numbers you can set the vertical scale. "
	"The standard setting is from 50 to 100 dB, but if you are interested in the power of the background noise "
	"you may want to set it to the range from 0 to 100 dB.")
ENTRY ("The averaging method")
NORMAL ("Averaging is what occurs if you make a @@time selection@ (rather than setting the cursor at a single time). "
	"The green number in dB to the left or right side of the intensity contour will show the average "
	"intensity in the selection. The same value is what you get when choosing ##Get intensity# from "
	"the #Intensity menu. You can choose any of the three averaging methods "
	"available for @@Intensity: Get mean...@, or choose for the #median value in the selection.")
ENTRY ("Pitch floor")
NORMAL ("The intensity curve is %smoothed, since you usually do not want the intensity curve to go up and down with "
	"the intensity variations within a pitch period. To avoid such pitch-synchronous variations, "
	"the intensity at every time point is a weighted average over many neighbouring time points. "
	"The weighting is performed by a Gaussian (`double Kaiser') window that has a duration that is determined by the "
	"##Minimum pitch# setting (see @@Intro 4.2. Configuring the pitch contour@). "
	"To see more detail than usual, raise the minimum pitch; to get more smoothing than usual, lower it. "
	"For more information, see @@Sound: To Intensity...@.")
ENTRY ("Subtract mean pressure or not?")
NORMAL ("Many microphones, microphone preamplifiers, or other components in the recording system "
	"can add a constant level to the air pressure (a %%DC offset%). "
	"You can see this at quiet locations in the wave form, "
	"where the average pressure is not zero but the background noise hovers about some non-zero value. "
	"You will usually not want to take this constant pressure seriously, i.e. the intensity curve should "
	"normally not take into account the energy provided by this constant pressure level. "
	"If you switch on the ##Subtract mean pressure# switch (whose standard setting is also on), "
	"the intensity value at a time point is computed by first subtracting the mean pressure around this point, "
	"and then applying the Gaussian window.")
MAN_END

MAN_BEGIN ("Intro 6.3. Querying the intensity contour", "ppgb", 20041123)
INTRO ("To ask for the intensity at the cursor, or the average intensity in the visible part of the selection, "
	"choose ##Get intensity# from the #Intensity menu or press the F11 key. You get the same value in dB that is visible "
	"to the left or right side of the intensity curve, but with more digits.")
MAN_END

MAN_BEGIN ("Intro 6.4. The Intensity object", "ppgb", 20041123)
NORMAL ("To print an intensity contour, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to create an @Intensity object in the @@List of Objects@. "
	"You do this either by choosing ##Extract visible intensity contour# "
	"from the #Intensity menu in the SoundEditor or TextGridEditor window, "
	"or by selecting a Sound object in the list and choosing @@Sound: To Intensity...@. "
	"In either case, a new Intensity object will appear in the list. "
	"To draw the Intensity object to the @@Picture window@, select it and choose ##Draw...#. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard.")
MAN_END

MAN_BEGIN ("Intro 7. Annotation", "ppgb", 20030113)
INTRO ("You can annotate existing @Sound objects and sound files (@LongSound objects).")
NORMAL ("The labelling data will reside in a @TextGrid object. This object is separate "
	"from the sound, which means that you will often see two objects in the list: a Sound or LongSound, "
	"and a TextGrid.")
ENTRY ("Creating a TextGrid")
NORMAL ("You create a new empty TextGrid from the Sound or LongSound with @@Sound: To TextGrid...@ "
	"or @@LongSound: To TextGrid...@ from the #Annotate menu. In this way, the time domain "
	"of the @TextGrid will automatically equal that of the sound (if you choose @@Create TextGrid...@ from "
	"the @@New menu@ instead, you will have to supply the time domain by yourself).")
NORMAL ("When you create a TextGrid, you specify the names of the %tiers. For instance, if you want to segment "
	"the sound into words and into phonemes, you may want to create two tiers and call them "
	"\"words\" and \"phonemes\" (you can easily add, remove, and rename tiers later). "
	"Since both of these tiers are %%interval tiers% (you label the intervals between the word and phoneme "
	"boundaries, not the boundaries themselves), you specify \"phonemes words\" for %%Tier names%, "
	"and you leave the %%Point tiers% empty.")
ENTRY ("View and edit")
NORMAL ("You can edit a TextGrid object all by itself, but you will normally want to see "
	"the sound in the editor window as well. To achieve this, you select both the Sound (or LongSound) and "
	"the TextGrid, and click @Edit. A @TextGridEditor will appear on your screen. "
	"Like the Sound editor, the TextGrid editor will show you a spectrogram, a pitch contour, a formant contour, "
	"and an intensity contour. This editor will allow "
	"you to add, remove, and edit labels, boundaries, and tiers. Under Help in the TextGridEditor, you will "
	"find the @TextGridEditor manual page. You are strongly advised to read it, because it will show you "
	"how you can quickly zoom (drag the mouse), play (click a rectangle), or edit a label (just type).")
ENTRY ("Save")
NORMAL ("You will normally write the TextGrid to disk with @@Write to text file...@ "
	"or @@Write to short text file...@. It is true that @@Write to binary file...@ will also work, but the others "
	"give you a file you can read with any text editor.")
NORMAL ("However you saved it, you can read the TextGrid into Praat later with @@Read from file...@.")
MAN_END

MAN_BEGIN ("Intro 8. Manipulation", "ppgb", 20021212)
LIST_ITEM ("@@Intro 8.1. Manipulation of pitch")
LIST_ITEM ("@@Intro 8.2. Manipulation of duration")
LIST_ITEM ("@@Intro 8.3. Manipulation of intensity")
MAN_END

MAN_BEGIN ("Intro 8.1. Manipulation of pitch", "ppgb", 20030309)
INTRO ("To modify the pitch contour of an existing @Sound object, "
	"you select this @Sound and click ##To Manipulation#. "
	"A @Manipulation object will then appear in the list. "
	"You can then click @Edit to raise a @ManipulationEditor, "
	"which will show the pitch contour (@PitchTier) as a series of thick dots. "
	"To reduce the number of dots, choose ##Stylize pitch (2 st)# "
	"from the #Pitch menu; it will then be easy to drag the dots "
	"about the time-pitch area.")
NORMAL ("If you click any of the rectangles "
	"(or choose any of the #Play commands from the #View menu), "
	"you will hear the modified sound. By shift-clicking, you will hear "
	"the original sound.")
NORMAL ("To get the modified sound as a separate object, "
	"choose ##Publish resynthesis# from the @@File menu@.")
NORMAL ("If you modify the duration curve as well (see @@Intro 8.2. Manipulation of duration@), "
	"the modified sound will be based on the modified pitch and duration.")
ENTRY ("Cloning a pitch contour")
NORMAL ("To use the pitch contour of one Manipulation object as the pitch contour of another Manipulation object, "
	"you first choose ##Extract pitch tier# for the first Manipulation object, "
	"then select the resulting PitchTier object together with the other Manipulation object "
	"(e.g. by a click on the PitchTier and a Command-click on the Manipulation), "
	"and choose ##Replace pitch tier#.")
ENTRY ("Precise manipulation of pitch")
NORMAL ("If you know exactly what pitch contour you want, "
	"you can create an empty PitchTier with @@Create PitchTier...@ from the New menu, "
	"then add points with @@PitchTier: Add point...@.")
NORMAL ("For instance, suppose you want to have a pitch that falls from 350 to 150 Hz in one second. "
	"You create the PitchTier, then add a point at 0 seconds and a frequency of 350 Hz, "
	"and a point at 1 second with a frequency of 150 Hz. "
	"You can put this PitchTier into a Manipulation object in the way described above.")
MAN_END

MAN_BEGIN ("Intro 8.2. Manipulation of duration", "ppgb", 20030316)
INTRO ("You can use P\\s{RAAT} to modify the relative durations in an existing sound.")
NORMAL ("First, you select a @Sound object and click \"To Manipulation\". "
	"A @Manipulation object will then appear in the list. "
	"You can then click @Edit to raise a @ManipulationEditor, "
	"which will show an empty @DurationTier. "
	"You can add targets to this tier by choosing \"Add duration point at cursor\" "
	"from the \"Dur\" menu. The targets will show up as green dots, which you can easily drag "
	"about the duration area.")
NORMAL ("If you click any of the rectangles "
	"(or choose any of the @Play commands from the @View menu), "
	"you will hear the modified sound. By shift-clicking, you will hear "
	"the original sound.")
NORMAL ("To get the modified sound as a separate object, "
	"choose ##Publish resynthesis# from the @@File menu@.")
NORMAL ("If you modify the pitch curve as well (see @@Intro 8.1. Manipulation of pitch@), "
	"the modified sound will be based on the modified duration and pitch.")
ENTRY ("Precise manipulation of duration")
NORMAL ("If you know exactly the times and relative durations, it is advisable to write a script (see @Scripting). "
	"Suppose, for instance, that you have a 355-ms piece of sound, and you want to shorten the first 85 ms to 70 ms, "
	"and the remaining 270 ms to 200 ms.")
NORMAL ("In your first 85 ms, your relative duration should be 70/85, "
	"and during the last 270 ms, it should be 200/270. "
	"The DurationTier does linear interpolation, so it can only be approximate these precise times, "
	"but fortunately to any precision you like:")
CODE ("Create DurationTier... shorten 0 0.085+0.270")
CODE ("Add point... 0.000 70/85")
CODE ("Add point... 0.084999 70/85")
CODE ("Add point... 0.085001 200/270")
CODE ("Add point... 0.0355 200/270")
NORMAL ("To put this DurationTier back into a Manipulation object, you select the two objects together "
	"(e.g. a click on the DurationTier and a Command-click on the Manipulation), "
	"and choose ##Replace duration tier#.")
MAN_END

MAN_BEGIN ("Intro 8.3. Manipulation of intensity", "ppgb", 20030316)
INTRO ("You can modify the intensity contour of an existing sound.")
NORMAL ("While the pitch and duration of a sound can be modified with the @ManipulationEditor "
	"(see @@Intro 8.1. Manipulation of pitch@ and @@Intro 8.2. Manipulation of duration@), "
	"the modification of the intensity curve is done in a different way.")
NORMAL ("You can create an @IntensityTier with the command @@Create IntensityTier...@ "
	"from the @@New menu@. With @Edit, you can add points to this tier. "
	"You can then multiply this tier with a sound, by selecting the @Sound and the "
	"IntensityTier together and clicking @@Sound & IntensityTier: Multiply|Multiply@. "
	"The points in the IntensityTier thus represent %relative intensities in dB.")
MAN_END

MAN_BEGIN ("Intro 8.4. Manipulation of formants", "ppgb", 20010408)
INTRO ("The manipulation of formant contours cannot be as straightforward as the manipulation "
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

MAN_BEGIN ("Labelling", "ppgb", 20010408)
INTRO ("See @@Intro 7. Annotation@.")
MAN_END

MAN_BEGIN ("List of Objects", "ppgb", 19981024)
INTRO ("A list in the left-hand part of the @@Object window@.")
ENTRY ("Purpose")
NORMAL ("If you select one or more @objects in this list, "
	"the possible actions that you can perform with the selected objects "
	"will appear in the @@Dynamic menu@.")
ENTRY ("How to select objects")
NORMAL ("To select one object (and deselect all the others), click on the object.")
NORMAL ("To extend the selection, drag the mouse (Unix, Windows) or use Shift-click (all systems).")
NORMAL ("To change the selection of one object (without changing the others), "
	"use Control-click (Unix, Windows) or Command-click (Macintosh).")
MAN_END

MAN_BEGIN ("Look and feel", "ppgb", 20030528)
INTRO ("This page is about how you see the Praat shell and work with it in an interactive session.")
ENTRY ("The Praat shell on Macintosh^\\re")
NORMAL ("Most of the %look is Macintosh: rounded buttons, a menu bar at the top. "
	"Some of the look is Motif^\\tm: a menu bar in every window.")
NORMAL ("Most of the %feel is Macintosh: only one window active simultaneously.")
ENTRY ("The Praat shell on Windows^\\re 95 and NT")
NORMAL ("Most of the look and feel is Windows, though all windows are separate overlapping windows, "
	"instead of children of a large application window.")
ENTRY ("The Praat shell on the X Window system")
NORMAL ("From every computer with OSF/Motif, you can run the Praat shell on every X server in the world. "
	"If your program is %praat, you just type:")
CODE ("praat -motif")
NORMAL ("With this command, you will always get the same old Motif look and feel.")
NORMAL ("Many systems, however, have their own favoured look and feel. "
	"For this reason, the Praat shell recognizes several client computers: if you just type")
CODE ("praat")
NORMAL ("(or double-click on the %praat icon), "
	"the Praat shell assumes that the X server matches the client computer.")
NORMAL ("The following client computers are recognized: SGI, Sun, HP. On other computers (Linux, IBM RS6000), "
	"typing $praat will give the same look and feel as typing $$praat -motif$.")
ENTRY ("The Praat shell on SGI")
NORMAL ("On a Silicon Graphics^\\re Iris^\\tm computer, typing")
CODE ("praat")
NORMAL ("or double-clicking the %praat icon, will give you either of the following look-and-feel styles:")
LIST_ITEM ("\\bu the native SGI \"Indigo Magic\" style, with lighting buttons and user-controllable "
	"color schemes, if available;")
LIST_ITEM ("\\bu the plain Motif style, if color schemes are not available (to find this out, "
	"the Praat shell checks the existence of the color-scheme browser %%/usr/sbin/schemebr% "
	"on the client side).")
NORMAL ("You can force the Praat shell to use the SGI style with")
CODE ("praat -sgi")
NORMAL ("or the Motif style with")
CODE ("praat -motif")
ENTRY ("The Praat shell on Sun")
NORMAL ("On a Sun computer, typing")
CODE ("praat")
NORMAL ("will give you the look and feel of Sun4, Solaris, or the Common Desktop Environment (CDE). "
	"These styles use non-standard fonts for their menus, buttons, and texts. Whether CDE is "
	"available, is determined by checking the existence of %%/usr/dt/bin/dtaction%.")
NORMAL ("You can force any of these styles with $$praat -solaris$, $$praat -sun4$, or $$praat -cde$.")
ENTRY ("The Praat shell on HP")
NORMAL ("On a Hewlett-Packard computer, typing")
CODE ("praat")
NORMAL ("will give you the look and feel of the HP version of the Common Desktop Environment. "
	"If you do not have that, try $$praat -motif$.")
ENTRY ("Running the Praat shell across a network")
NORMAL ("Like most X programs, you can run the Praat program across a network, "
	"with the executable running on the %client computer, "
	"and yourself sitting at a computer that acts as an %%X server%. For this to work, "
	"you will have to take care of two things:")
LIST_ITEM ("\\bu Tell the client computer to use the server %display. If you use %telnet "
	"(but not %rlogin), this will be done automatically for you, because %telnet "
	"propagates the DISPLAY environment variable. "
	"Otherwise, if you are sitting at %%indigo3.uni-franeker.nl%, "
	"you type the following line to your remote login shell:")
CODE ("    setenv DISPLAY indigo3.uni-franeker.nl:0")
LIST_ITEM ("\\bu Your X server should grant the client computer access to its display. "
	"This may sometimes be automatic (e.g., if the server allows the whole world to connect to it). "
	"Otherwise, if the client is %%distant1.uni-harderwijk.nl%, you type the following line to your "
	"local shell:")
CODE ("    xhost +distant1.uni-harderwijk.nl")
NORMAL ("Finally, you type $$praat -motif$ to your remote login shell, "
	"and the two windows will appear on your screen.")
NORMAL ("If you run the Praat shell from an SGI client across a network on an X server "
	"(a PC or Sun, for example), "
	"the normal $praat command will probably work and give you the native SGI look and feel. "
	"This works because the SGI style only uses fonts that are available everywhere.")
NORMAL ("If your X server is a Sun with Solaris, you can probably use the Motif, and Solaris or CDE styles.")
NORMAL ("Likewise, if you run %praat from a different machine on a HP X server, you may try $$praat -hp$.")
NORMAL ("If you run %praat from a Sun client on an X server, you will probably use $$praat -motif$.")
ENTRY ("Resource files?")
NORMAL ("Many X Windows applications work with %%client-side% resource files, "
	"which describe the fonts etc. that they need. "
	"However, as the fonts are server-dependent, changing to a different X server "
	"will often require you to change the resource file %%on the client side%, "
	"unless you only use fonts that are available on all systems (like Helvetica and Times). "
	"However, your application may not adhere to any native look-and-feel strategy then. "
	"Incorporating all resource files into the application, as done in the Praat shell, "
	"solves this problem. Locally, you just type $praat, and across a network you use "
	"one of the explicit look-and-feel options.")
NORMAL ("In the Praat shell, the user-controllability of window sizes and fonts, "
	"which is sometimes expressed by editing resource files, "
	"is implemented by a hidden mechanism, which remembers across sessions "
	"the changes made by the user while the application is running.")
MAN_END

MAN_BEGIN ("New menu", "ppgb", 20021212)
INTRO ("The ##New menu# is one of the menus in the @@Object window@. "
	"You use this menu to create new objects from scratch. It contains the following commands:")
LIST_ITEM ("\\bu @@Record mono Sound...@")
LIST_ITEM ("\\bu @@Record stereo Sound...@")
LIST_ITEM ("\\bu @@Create Sound...@ (from a formula)")
LIST_ITEM ("\\bu @@Create Sound from tone complex...")
LIST_ITEM ("\\bu @@Create Sound from gamma-tone...")
LIST_ITEM ("\\bu @@Create Sound from Shepard tone...")
LIST_ITEM ("\\bu @@Create Matrix...@ (from a formula)")
LIST_ITEM ("\\bu @@Create simple Matrix...@ (from a formula)")
LIST_ITEM ("\\bu @@Create empty PointProcess...@")
LIST_ITEM ("\\bu @@Create Poisson process...@")
LIST_ITEM ("\\bu @@Create PitchTier...@")
LIST_ITEM ("\\bu @@Create DurationTier...@")
LIST_ITEM ("\\bu @@Create IntensityTier...@")
LIST_ITEM ("\\bu @@Create FormantTier...@")
LIST_ITEM ("\\bu @@Create Strings as file list...@")
LIST_ITEM ("\\bu @@Create TextGrid...@")
LIST_ITEM ("\\bu @@OT learning@ tutorial")
LIST_ITEM ("\\bu @@Create tongue-root grammar...@")
NORMAL ("To create new objects from files on disk, use the @@Read menu@ instead. "
	"Objects can also often be create from other objects, with commands that start with ##To#.")
MAN_END

MAN_BEGIN ("Object window", "ppgb", 20030528)
INTRO ("One of the two main windows in the P\\s{RAAT} program.")
ENTRY ("Subdivision")
LIST_ITEM ("To the left: the @@List of Objects@.")
LIST_ITEM ("To the right: the @@Dynamic menu@.")
ENTRY ("Fixed buttons")
NORMAL ("The following buttons appear below the List of Objects:")
LIST_ITEM ("\\bu @@Rename...")
LIST_ITEM ("\\bu @@Info")
LIST_ITEM ("\\bu @@Copy...")
LIST_ITEM ("\\bu @@Remove")
LIST_ITEM ("\\bu @@Inspect")
ENTRY ("Menus")
LIST_ITEM ("The Object window contains several fixed menus: "
	"the #Praat, #New, #Read, and #Help menus. "
	"It also contains the #Write menu, whose contents vary with the kinds of selected objects, "
	"and must, therefore, be considered part of the dynamic menu.")
ENTRY ("The Praat menu")
LIST_ITEM ("\\bu (@@Run script...@)")
LIST_ITEM ("\\bu @@New Praat script@: creates an empty @@ScriptEditor@")
LIST_ITEM ("\\bu @@Open Praat script...@: creates a @@ScriptEditor@ with a script from disk")
LIST_ITEM ("\\bu The ##Goodies submenu#: for doing things (like using the Calculator) "
	"that do not create new objects and do not depend on the kinds of selected objects.")
LIST_ITEM ("\\bu The ##Preferences submenu#: for program-wide preferences, "
	"like audio input and output settings.")
LIST_ITEM ("\\bu ##Buttons...#: raises a @@ButtonEditor@")
LIST_ITEM ("\\bu (@@Add menu command...@)")
LIST_ITEM ("\\bu (@@Add action command...@)")
LIST_ITEM ("\\bu @@Quit")
ENTRY ("Other menus")
LIST_ITEM ("\\bu The @@New menu@: for creating objects from scratch.")
LIST_ITEM ("\\bu The @@Read menu@: for reading objects from file into memory.")
LIST_ITEM ("\\bu The @@Write menu@: for writing objects from memory to file.")
LIST_ITEM ("\\bu The ##Help menu#: for viewing the manual.")
MAN_END

MAN_BEGIN ("Periodicity menu", "ppgb", 20010417)
INTRO ("A menu that occurs in the @@Dynamic menu@ for a @Sound.")
NORMAL ("This menu contains commands for analysing the pitch contour of the selected Sound:")
LIST_ITEM ("@@Sound: To Pitch...")
LIST_ITEM ("@@Sound: To Pitch (ac)...")
LIST_ITEM ("@@Sound: To Pitch (cc)...")
LIST_ITEM ("@@Sound: To Harmonicity (cc)...")
LIST_ITEM ("@@Sound: To Harmonicity (ac)...")
MAN_END

MAN_BEGIN ("Pitch settings...", "ppgb", 20030316)
INTRO ("A command in the Pitch menu of the @SoundEditor and @TextGridEditor windows. "
	"See @@Intro 4.2. Configuring the pitch contour@.")
MAN_END

MAN_BEGIN ("Play", "ppgb", 20021212)
INTRO ("A command that is available if you select a @Sound, @Pitch, or @PointProcess object. "
	"It gives you an acoustic representation of the selected object, if your loudspeakers are on "
	"and you did not \"mute\" your computer sound system.")
NORMAL ("A Play button is also available in the @SoundRecorder window "
	"and in the @View menu of the @SoundEditor or @TextGridEditor. In the editors, "
	"you will usually play a sound by clicking on any of the rectangles around the data.")
MAN_END

MAN_BEGIN ("Query", "ppgb", 20021218)
INTRO ("Query commands give you information about objects.")
NORMAL ("Most query commands start with the word #%Get or sometimes the word #%Count. "
	"You will find these commands in two places: under the #Query menu that usually appears if you "
	"select an @@Objects|object@ in the list, and in the #Query menus of the @editors.")
ENTRY ("Behaviour")
NORMAL ("If you click a query command, the answer will be written to the @@Info window@.")
ENTRY ("Scripting")
NORMAL ("In a script, you can still use query commands to write the information to the Info window "
	"but you can also use any query command to put the information into a variable. "
	"(see @@Scripting 6.4. Query commands@). In such a case, the value will not be written into the Info window.")
ENTRY ("Query commands in the Praat program")
NORMAL ("The Praat program contains the following query commands:")
MAN_END

MAN_BEGIN ("Query menu", "ppgb", 20010417)
INTRO ("One of the menus in most @editors.")
MAN_END

MAN_BEGIN ("Quit", "ppgb", 20050822)
INTRO ("One of the commands in the @@Praat menu@.")
ENTRY ("Purpose")
NORMAL ("To leave the program.")
ENTRY ("Behaviour")
NORMAL ("All @objects not written to a file will be lost. "
	"However, file-based objects (like large lexica) will be saved correctly.")
ENTRY ("Usage")
NORMAL ("To save your data to a disk file before quitting, choose a command from the @@Write menu@.")
MAN_END

MAN_BEGIN ("Read from file...", "ppgb", 19970911)
INTRO ("One of the commands in the @@Read menu@.")
ENTRY ("Purpose")
NORMAL ("To read one or more @objects from a file on disk.")
ENTRY ("Behaviour")
NORMAL ("Many kinds of files are recognized:")
LIST_ITEM ("1. Text files that are structured as described under @@Write to text file...@; "
	"these can contain an object of any class, or a collection of objects.")
LIST_ITEM ("2. Files that were produced by @@Write to binary file...@ (any class).")
LIST_ITEM ("3. Files in a LISP text format (only for classes that can be written to a LISP file).")
LIST_ITEM ("4. Files that were made recognizable by the libraries built on Praat. "
	"For instance, the phonetics library adds recognizers for many kinds of sound files.")
NORMAL ("If the file contains more than one object, these objects will appear in the list, "
	"and their names will be the same as the names that they had "
	"when they were saved with ##Write to text file...# or ##Write to binary file...#.")
ENTRY ("Examples")
LIST_ITEM ("\\bu If the file contains only one Pitch object and is called \"hallo.pit\", "
	"an object with the name \"Pitch hallo\" will appear in the list of objects. "
	"You may have more objects with the same name.")
LIST_ITEM ("\\bu If the file contains one object of class Pitch, named \"hallo\", "
	"and one object of class Polygon, named \"kromme\", "
	"there will appear two objects in the list, "
	"called \"Pitch hallo\" and \"Polygon kromme\".")
MAN_END

MAN_BEGIN ("Read menu", "ppgb", 19970911)
INTRO ("One of the menus in the @@Object window@.")
NORMAL ("With the Read menu, you read one or more @objects from a file on disk into memory. "
	"The resulting object(s) will appear in the @@List of Objects@.")
NORMAL ("The Read menu contains the command @@Read from file...@, which recognizes most file types, "
	"and perhaps several other commands for reading unrecognizable file types (e.g., raw sound data), "
	"or for interpreting known file types in a different way "
	"(e.g., reading two sounds from a stereo sound file, which is normally read as mono):")
MAN_END

MAN_BEGIN ("Remove", "ppgb", 20021212)
INTRO ("One of the fixed buttons in the @@Object window@.")
NORMAL ("You can choose this command after selecting one or more @objects in the list.")
NORMAL ("The selected objects will permanently disappear from the list, "
	"and the computer memory that they occupied will be freed.")
NORMAL ("To save your data before removing, choose a command from the @@Write menu@.")
MAN_END

MAN_BEGIN ("Rename...", "ppgb", 19960904)
INTRO ("One of the fixed buttons in the @@Object window@.")
ENTRY ("Availability")
NORMAL ("You can choose this command after selecting one object of any class.")
ENTRY ("Purpose")
NORMAL ("You can give the selected object a new name.")
ENTRY ("Behaviour")
NORMAL ("If you type special symbols or spaces, the Object window will replace them with underscores.")
MAN_END

MAN_BEGIN ("Resource fork", "ppgb", 20030316)
INTRO ("One of the two %forks of a Macintosh file (the other is the %%data fork%). "
	"If a Macintosh file is moved to another system directly, the resource fork is lost. "
	"To backup your Macintosh files, use compression, for instance with #DropStuff\\tm.")
MAN_END

MAN_BEGIN ("Segmentation", "ppgb", 20010408)
INTRO ("See @@Intro 7. Annotation@.")
MAN_END

MAN_BEGIN ("Show formant", "ppgb", 20030316)
INTRO ("One of the commands in the Formant menu of the @SoundEditor and the @TextGridEditor.")
NORMAL ("See @@Intro 5. Formant analysis@.")
MAN_END

MAN_BEGIN ("Show intensity", "ppgb", 20030316)
INTRO ("One of the commands in the Intensity menu of the @SoundEditor and the @TextGridEditor.")
NORMAL ("See @@Intro 6. Intensity analysis@.")
MAN_END

MAN_BEGIN ("Show pitch", "ppgb", 20030316)
INTRO ("One of the commands in the Pitch menu of the @SoundEditor and the @TextGridEditor.")
NORMAL ("See @@Intro 4. Pitch analysis@.")
MAN_END

MAN_BEGIN ("Show pulses", "ppgb", 20030316)
INTRO ("One of the commands in the Pulses menu of the @SoundEditor and the @TextGridEditor.")
NORMAL ("See @Voice.")
MAN_END

MAN_BEGIN ("Show spectrogram", "ppgb", 20030316)
INTRO ("One of the commands in the Spectrogram menu of the @SoundEditor and the @TextGridEditor.")
NORMAL ("See @@Intro 3. Spectral analysis@.")
MAN_END

MAN_BEGIN ("Source-filter synthesis", "ppgb", 20050713)
INTRO ("This tutorial describes how you can do acoustic synthesis with P\\s{RAAT}. "
	"It assumes that you are familiar with the @Intro.")
ENTRY ("1. The source-filter theory of speech production")
NORMAL ("The source-filter theory (@@Fant (1960)|Fant 1960@) hypothesizes that an acoustic speech signal can be seen "
	"as a %source signal (the glottal source, or noise generated at a constriction in the vocal tract), "
	"%filtered with the resonances in the cavities of the vocal tract downstream from the glottis "
	"or the constriction. The %%Klatt synthesizer% (@@Klatt & Klatt (1990)|Klatt & Klatt 1990@), for instance, "
	"is based on this idea.")
NORMAL ("In the Praat program, you can create a %source signal from scratch of from an existing "
	"speech signal, and you can create a %filter from scrtach or extract it from an existing speech signal. "
	"You can manipulate (change, adapt) both the source and the filter before doing "
	"the actual synthesis, which combines the two.")
LIST_ITEM ("@@Source-filter synthesis 1. Creating a source from pitch targets")
LIST_ITEM ("@@Source-filter synthesis 2. Filtering a source")
LIST_ITEM ("@@Source-filter synthesis 3. The ba-da continuum")
LIST_ITEM ("@@Source-filter synthesis 4. Using existing sounds")
MAN_END

MAN_BEGIN ("Source-filter synthesis 1. Creating a source from pitch targets", "ppgb", 20050713)
INTRO ("Creating a glottal source signal for speech synthesis involves creating a @PointProcess, "
	"which is a series of time points that should represent the exact moments of glottal closure.")
NORMAL ("You may want to start with creating a well-defined pitch contour. "
	"Suppose you want to create a sound with a duration of half a second with a pitch that falls from 300 to 200 Hz "
	"during that time. You first create an empty @PitchTier by choosing @@Create PitchTier...@ from the #New menu "
	"(I call this PitchTier \"empty\" because it does not contain any pitch information yet); "
	"you may want to name the PitchTier \"source\" and have it start at 0 seconds and end at 0.5 seconds. "
	"Once the PitchTier exists and is selected, you can choose @@PitchTier: Add point...@ from the #Modify menu repeatedly "
	"to add pitch points (pitch targets) at certain times. You would add a pitch point of 300 Hz at time 0.0 "
	"and a pitch point of 200 Hz at time 1.0. This whole mouse-clicking procedure can be abbreviated as follows "
	"(if you want to automate this procedure in a script, it would look exactly like this):")
CODE ("Create PitchTier... source 0 0.5")
CODE ("Add point... 0 300")
CODE ("Add point... 0.5 200")
NORMAL ("If you #Draw or #Edit this PitchTier, you can see that the pitch curve falls linearly "
	"from 300 to 200 Hz during its time domain. You can hear the falling pitch by choosing "
	"##Play pulses#, #Hum, or ##Play sine#.")
NORMAL ("From this PitchTier, you can create a @PointProcess with @@PitchTier: To PointProcess@. "
	"The resulting PointProcess now represents a series of glottal pulses. To make some parts of this "
	"point process voiceless, you can use @@PointProcess: Remove points between...@. "
	"It is advisable to make the very beginning and end of this point process voiceless, so that the filtered sound "
	"will not start or end abruptly. For instance:")
CODE ("Remove points between... 0 0.02")
CODE ("Remove points between... 0.24 0.31")
CODE ("Remove points between... 0.48 0.5")
NORMAL ("In this example, the first and last 20 ms are devoiced, and a stretch of 70 ms in the middle "
	"is made voiceless as well, perhaps because you want to simulate a voiceless plosive there.")
NORMAL ("Now that we have a glottal point process (a glottal pulse train), the only thing left "
	"is to turn it into a sound by choosing @@PointProcess: To Sound (phonation)...@. "
	"If you use the standard settings of this command, the result will be a @Sound with "
	"reasonable glottal flow derivatives centred around each of the original pulses in the point process. "
	"You can check this by selecting the Sound and choosing #Edit. "
	"You will also see that the amplitude of the first two glottal wave shapes of every voiced stretch "
	"is (realistically) somewhat smaller than the amplitudes of the following wave shapes.")
NORMAL ("What you have now is what we call a %%glottal source signal%. It does two things: it contains information on the glottal flow, "
	"and it already takes into account one aspect of the %filter, namely the radiation at the lips. "
	"This combination is standard procedure in acoustic synthesis.")
NORMAL ("The glottal source signal sounds as a voice without a vocal tract. "
	"The following section describes how you add vocal-tract resonances, i.e. the %filter.")
MAN_END

MAN_BEGIN ("Source-filter synthesis 2. Filtering a source", "ppgb", 20050713)
INTRO ("Once you have a glottal source signal, you are ready to create a filter that represents "
	"the resonances of the vocal tract, as a function of time. In other words, you create a @FormantTier object.")
NORMAL ("For a vowel spoken by an average (i.e. adult female) human voice, tradition assumes five formants in the range "
	"between 0 and 5500 Hertz. This number comes from a computation of the formants of a "
	"straight tube, which has resonances at wavelengths of four tube lengths, four thirds of a tube length, "
	"four fifths, and so on. For a straight tube 16 centimetres long, the shortest wavelength is 64 cm, "
	"which, with a sound velocity of 352 m/s, means a resonance frequency of 352/0.64 = 550 Hertz. "
	"The other resonances will be at 1650, 2750, 3850, and 4950 Hertz.")
NORMAL ("You can create a @FormantTier object with @@Create FormantTier...@, "
	"and add some points to it with @@FormantTier: Add point...@:")
CODE ("Create FormantTier... filter 0 0.5")
CODE ("Add point... 0.00 100 50 500 100 2750 200 3850 300 4950 400 6050 500 7150 600 8250 700 9350 800")
CODE ("Add point... 0.05 700 50 1100 100 2750 200 3850 300 4950 400 6050 500 7150 600 8250 700 9350 800")
NORMAL ("This example creates a spectral specification whose %F__1_ rises from 100 to 700 Hertz during the "
	"first 50 milliseconds (as for any obstruent), and whose %F__2_ rises from 500 to 1100 Hertz. "
	"This may be a [ba]-like formant transition.")
NORMAL ("To get the final acoustic result (the sound), you select the glottal source signal "
	"together with the FormantTier and choose @@Sound & FormantTier: Filter@.")
NORMAL ("The resulting sound will have a fairly straight intensity contour. You can change this with "
	"the #Formula command (@@Sound: Formula...@), or by multiplying the source signal or the "
	"acoustic result with an @Intensity or @IntensityTier object.")
MAN_END

MAN_BEGIN ("Source-filter synthesis 3. The ba-da continuum", "ppgb", 20050713)
INTRO ("As an example, we are going to create a male [ba]-[da] continuum in six steps. The acoustic difference "
	"between [ba] and [da] is the initial %F__2_, which is 500 Hz for [ba], and 2500 Hz for [da].")
NORMAL ("We use the same @PitchTier throughout, to model a falling intonation contour:")
CODE ("Create PitchTier... f0 0.00 0.50")
CODE ("Add point... 0.00 150")
CODE ("Add point... 0.50 100")
NORMAL ("The first and last 50 milliseconds are voiceless:")
CODE ("To PointProcess")
CODE ("Remove points between... 0.00 0.05")
CODE ("Remove points between... 0.45 0.50")
NORMAL ("Generate the glottal source signal:")
CODE ("To Sound (phonation)... 44100 0.6 0.05 0.7 0.03 3.0 4.0")
NORMAL ("During the labial or coronal closure, the sound is almost silent, so we use an @IntensityTier "
	"that models this:")
CODE ("Create IntensityTier... intens 0.00 0.50")
CODE ("Add point... 0.05 60")
CODE ("Add point... 0.10 80")
NORMAL ("Generate the source signal:")
CODE ("#plus Sound f0")
CODE ("Multiply")
CODE ("Rename... source")
NORMAL ("The ten sounds are generated in a loop:")
CODE ("#for i #from 1 #to 10")
CODE ("   f2_locus = 500 + (2500/9) * (i - 1) ; variable names start with lower case!")
CODE ("   Create FormantTier... filter 0.00 0.50")
CODE ("   Add point... 0.05   100 50 'f2_locus' 100")
CODE ("      ... 3000 300 4000 400 5000 500 6000 600 7000 700 8000 800 9000 900")
CODE ("   Add point... 0.10   700 50 1100 100")
CODE ("      ... 3000 300 4000 400 5000 500 6000 600 7000 700 8000 800 9000 900")
CODE ("   #plus Sound source")
CODE ("   Filter (no scale)")
CODE ("   Rename... bada'i'")
CODE ("   #select FormantTier filter")
CODE ("   Remove")
CODE ("#endfor")
NORMAL ("Clean up:")
CODE ("#select Sound source")
CODE ("#plus Sound f0")
CODE ("#plus IntensityTier intens")
CODE ("#plus PointProcess f0")
CODE ("#plus PitchTier f0")
CODE ("Remove")
NORMAL ("In this example, filtering was done without automatic scaling, so that "
	"the resulting signals have equal intensities in the areas where they have "
	"equal formants. You will probably want to multiply all these signals with "
	"the same value in order to bring their amplitudes in a suitable range "
	"between -1 and +1 Pascal.")
MAN_END

MAN_BEGIN ("Source-filter synthesis 4. Using existing sounds", "ppgb", 20050713)
ENTRY ("1. How to extract the %filter from an existing speech sound")
NORMAL ("You can separate source and filter with the help of the technique of %%linear prediction% "
	"(see @@Sound: LPC analysis@). This technique tries to approximate a given frequency spectrum with "
	"a small number of peaks, for which it finds the mid frequencies and the bandwidths. "
	"If we do this for an overlapping sequence of windowed parts of a sound signal "
	"(i.e. a %%short-term analysis%), we get a quasi-stationary approximation of the signal's "
	"spectral characteristics as a function of time, i.e. a smoothed version of the @Spectrogram.")
NORMAL ("For a speech signal, the peaks are identified with the resonances (%formants) of the vocal tract. "
	"Since the spectrum of a vowel spoken by an average human being falls off with approximately "
	"6 dB per octave, %%pre-emphasis% is applied to the signal before the linear-prediction analysis, "
	"so that the algorithm will not try to match only the lower parts of the spectrum.")
NORMAL ("For an adult female human voice, tradition assumes five formants in the range "
	"between 0 and 5500 Hertz, say at 550, 1650, 2750, 3850, and 4950 Hertz. For the linear prediction in "
	"Praat, you will have to implement this 5500-Hz band-limiting by resampling the original "
	"speech signal to 11 kHz. For a male voice, you would use 10 kHz; for a young child, 20 kHz.")
NORMAL ("To perform the resampling, you use @@Sound: Resample...@: "
	"you select a @Sound object, and click ##Resample...#. "
	"In the rest of this tutorial, I will use the syntax that you would use in a script, "
	"though you will usually do these things by clicking on objects and buttons. Thus:")
CODE ("#select Sound hallo")
CODE ("Resample... 11000 50")
NORMAL ("You can then perform a linear-prediction analysis on the resampled sound "
	"with @@Sound: To LPC (burg)...@:")
CODE ("#select Sound hallo_11000")
CODE ("To LPC (burg)... 10 0.025 0.005 50")
NORMAL ("This says that your analysis is done with 10 linear-prediction parameters "
	"(which will yield at most five formant-bandwidth pairs), with an analysis window "
	"effectively 25 milliseconds long, with time steps of 5 milliseconds (so that the windows "
	"will appreciably overlap), and with a pre-emphasis frequency of 50 Hz (which is the point "
	"above which the sound will be amplified by 6 dB/octave prior to the analysis proper).")
NORMAL ("As a result, an object called \"LPC hallo\" will appear in the list of objects. "
	"This @LPC object is a time function with 10 %%linear-prediction coefficients% in each %%time frame%. "
	"These coefficients are rather opaque even to the expert (try to view them with @Inspect), "
	"but they are the raw material from which formant and bandwidth values can be computed. "
	"To see the smoothed @Spectrogram associated with the LPC object, choose @@LPC: To Spectrogram...@:")
CODE ("#select LPC hallo_11000")
CODE ("To Spectrogram... 20 0 50")
CODE ("Paint... 0 0 0 0 50 0 0 yes")
NORMAL ("Note that when drawing this Spectrogram, you will want to set the pre-emphasis to zero "
	"(the fifth 0 in the last line), because pre-emphasis has already been applied in the analysis.")
NORMAL ("You can get and draw the formant-bandwidth pairs from the LPC object, "
	"with @@LPC: To Formant@ and @@Formant: Speckle...@:")
CODE ("#select LPC hallo_11000")
CODE ("To Formant")
CODE ("Speckle... 0 0 5500 30 yes")
NORMAL ("Note that in converting the @LPC into a @Formant object, you may have lost some "
	"information about spectral peaks at very low frequencies (below 50 Hz) or at very high "
	"frequencies (near the @@Nyquist frequency@ of 5500 Hz. Such peaks usually try to fit "
	"an overall spectral slope (if the 6 dB/octave model is inappropriate), and are not seen "
	"as related with resonances in the vocal tract, so they are ignored in a formant analysis. "
	"For resynthesis purposes, they might still be important.")
NORMAL ("Instead of using the intermediate LPC object, you could have done a formant analysis "
	"directly on the original Sound, with @@Sound: To Formant (burg)...@:")
CODE ("#select Sound hallo")
CODE ("To Formant (burg)... 0.005 5 5500 0.025 50")
NORMAL ("A @Formant object has a fixed sampling (time step, frame length), and for every "
	"%%formant frame%, it contains a number of formant-bandwidth pairs.")
NORMAL ("From a Formant object, you can create a @FormantTier with @@Formant: Down to FormantTier@. "
	"A FormantTier object contains a number of time-stamped %%formant points%, "
	"each with a number of formant-bandwidth pairs.")
NORMAL ("Any of these three classes (@LPC, @Formant, and @FormantTier) can represent the %filter "
	"in source-filter synthesis.")
ENTRY ("2. How to extract the %source from an existing speech sound")
NORMAL ("If you are only interested in the %filter characteristics, you can get by with @Formant objects. "
	"To get at the %source signal, however, you need the raw @LPC object: "
	"you select it together with the resampled @Sound, and apply %%inverse filtering%:")
CODE ("#select Sound hallo_11000")
CODE ("#plus LPC hallo_11000")
CODE ("Filter (inverse)")
NORMAL ("A new Sound named \"hallo_11000\" will appear in the list of objects "
	"(you could rename it to \"source\"). "
	"This is the estimated source signal. Since the LPC analysis was designed to yield a spectrally "
	"flat filter (through the use of pre-emphasis), this source signal represents everything in the "
	"speech signal that cannot be attributed to the resonating cavities. Thus, the \"source signal\" "
	"will consist of the glottal volume-velocity source (with an expected spectral slope of "
	"-12 dB/octave for vowels) and the radiation characteristics at the lips, which cause a "
	"6 dB/octave spectral rise, so that the resulting spectrum of the \"source signal\" "
	"is actually the %derivative of the glottal flow, with an expected spectral slope of -6 dB/octave.")
NORMAL ("Note that with inverse filtering you cannot measure the actual spectral slope of the source signal. "
	"Even if the actual slope is very different from -6 dB/octave, formant extraction will try to "
	"match the pre-emphasized spectrum. Thus, by choosing a pre-emhasis of -6 dB/octave, "
	"you %impose a slope of -6 dB/octave on the source signal.")
ENTRY ("3. How to do the synthesis")
NORMAL ("You can create a new Sound from a source Sound and a filter, in at least four ways.")
NORMAL ("If your filter is an @LPC object, you select it and the source, and choose @@LPC & Sound: Filter...@:")
CODE ("#select Sound source")
CODE ("#plus LPC filter")
CODE ("Filter... no")
NORMAL ("If you had computed the source and filter from an LPC analysis, this procedure should give "
	"you back the original Sound, except that windowing has caused 25 milliseconds at the beginning "
	"and end of the signal to be set to zero.")
NORMAL ("If your filter is a @Formant object, you select it and the source, and choose @@Sound & Formant: Filter@:")
CODE ("#select Sound source")
CODE ("#plus Formant filter")
CODE ("Filter")
NORMAL ("If you had computed the source and filter from an LPC analysis, this procedure will not generally give "
	"you back the original Sound, because some linear-prediction coefficients will have been ignored "
	"in the conversion to formant-bandwidth pairs.")
NORMAL ("If your filter is a @FormantTier object, you select it and the source, and choose @@Sound & FormantTier: Filter@:")
CODE ("#select Sound source")
CODE ("#plus FormantTier filter")
CODE ("Filter")
NORMAL ("Finally, you could just know the %%impulse response% of your filter (in a @Sound object). "
	"You then select both Sound objects, and choose @@Sounds: Convolve@:")
CODE ("#select Sound source")
CODE ("#plus Sound filter")
CODE ("Convolve")
ENTRY ("4. How to manipulate the filter")
NORMAL ("You can hardly change the values in an @LPC object in a meaningful way: "
	"you would have to manually change its rather opaque data with the help of @Inspect.")
NORMAL ("A @Formant object can be changed in a friendlier way, with @@Formant: Formula (frequencies)...@ "
	"and @@Formant: Formula (bandwidths)...@. For instance, to multiply all formant frequencies by 0.9, "
	"you do")
CODE ("#select Formant filter")
CODE ("Formula (frequencies)... self * 0.9")
NORMAL ("To add 200 Hertz to all values of %F__2_, you do")
CODE ("Formula (frequencies)... if row = 2 then self + 200 else self fi")
NORMAL ("A @FormantTier object can be changed by adding or removing points:")
LIST_ITEM ("@@FormantTier: Add point...@")
LIST_ITEM ("@@Remove point...@")
LIST_ITEM ("@@Remove point near...@")
LIST_ITEM ("@@Remove points between...@")
ENTRY ("5. How to manipulate the source signal")
NORMAL ("You can manipulate the source signal in the same way you that would manipulate any sound, "
	"for instance with the @ManipulationEditor.")
MAN_END

MAN_BEGIN ("Spectrogram settings...", "ppgb", 20030316)
INTRO ("A command in the Spectrogram menu of the @SoundEditor and @TextGridEditor windows. "
	"See @@Intro 3.2. Configuring the spectrogram@.")
MAN_END

MAN_BEGIN ("Types of objects", "ppgb", 20050713)
INTRO ("P\\s{RAAT} contains the following types of objects and @Editors. "
	"For an introduction and tutorials, see @Intro.")
NORMAL ("General purpose:")
LIST_ITEM ("\\bu @Matrix: a sampled real-valued function of two variables")
LIST_ITEM ("\\bu @Polygon")
LIST_ITEM ("\\bu @PointProcess: a point process (@PointEditor)")
LIST_ITEM ("\\bu @Sound: a sampled continuous process (@SoundEditor, @SoundRecorder, @@Sound files@)")
LIST_ITEM ("\\bu @LongSound: a file-based version of a sound (@LongSoundEditor)")
LIST_ITEM ("\\bu @Strings")
LIST_ITEM ("\\bu @Distributions, @PairDistribution")
LIST_ITEM ("\\bu @Table, @TableOfReal")
LIST_ITEM ("\\bu @Permutation")
LIST_ITEM ("\\bu @ParamCurve")
NORMAL ("Periodicity analysis:")
LIST_ITEM ("\\bu Tutorials:")
LIST_ITEM1 ("\\bu @@Intro 4. Pitch analysis")
LIST_ITEM1 ("\\bu @@Intro 6. Intensity analysis")
LIST_ITEM1 ("\\bu @Voice (jitter, shimmer, noise)")
LIST_ITEM ("\\bu @Pitch: articulatory fundamental frequency, acoustic periodicity, or perceptual pitch (@PitchEditor)")
LIST_ITEM ("\\bu @Harmonicity: degree of periodicity")
LIST_ITEM ("\\bu @Intensity, @IntensityTier: intensity contour")
NORMAL ("Spectral analysis:")
LIST_ITEM ("\\bu Tutorials:")
LIST_ITEM1 ("\\bu @@Intro 3. Spectral analysis")
LIST_ITEM1 ("\\bu @@Intro 5. Formant analysis")
LIST_ITEM ("\\bu @Spectrum: complex-valued equally spaced frequency spectrum (@SpectrumEditor)")
LIST_ITEM ("\\bu @Ltas: long-term average spectrum")
LIST_ITEM ("\\bu Spectro-temporal: @Spectrogram, @BarkFilter, @MelFilter, @FormantFilter")
LIST_ITEM ("\\bu @Formant: acoustic formant contours")
LIST_ITEM ("\\bu @LPC: coefficients of Linear Predictive Coding, as a function of time")
LIST_ITEM ("\\bu @Cepstrum, @CC, @LFCC, @MFCC (cepstral coefficients)")
LIST_ITEM ("\\bu @Excitation: excitation pattern of basilar membrane")
LIST_ITEM ("\\bu @Excitations: an ensemble of #Excitation objects")
LIST_ITEM ("\\bu @Cochleagram: excitation pattern as a function of time")
NORMAL ("Labelling and segmentation (see @@Intro 7. Annotation@):")
LIST_ITEM ("\\bu @TextGrid (@TextGridEditor), @IntervalTier, @TextTier")
NORMAL ("Listening experiments:")
LIST_ITEM ("\\bu @ExperimentMFC")
NORMAL ("Manipulation of sound:")
LIST_ITEM ("\\bu Tutorials:")
LIST_ITEM1 ("\\bu @@Intro 8.1. Manipulation of pitch")
LIST_ITEM1 ("\\bu @@Intro 8.2. Manipulation of duration")
LIST_ITEM1 ("\\bu @@Intro 8.3. Manipulation of intensity")
LIST_ITEM1 ("\\bu @@Filtering")
LIST_ITEM1 ("\\bu @@Source-filter synthesis")
LIST_ITEM ("\\bu @PitchTier (@PitchTierEditor)")
LIST_ITEM ("\\bu @Manipulation (@ManipulationEditor): @PSOLA")
LIST_ITEM ("\\bu @DurationTier")
LIST_ITEM ("\\bu @FormantTier")
NORMAL ("Articulatory synthesis (see the @@Articulatory synthesis@ tutorial):")
LIST_ITEM ("\\bu @Speaker: speaker characteristics of a woman, a man, or a child")
LIST_ITEM ("\\bu #Articulation: snapshot of articulatory specifications (muscle activities)")
LIST_ITEM ("\\bu @Artword: articulatory target specifications as functions of time")
LIST_ITEM ("\\bu (@VocalTract: area function)")
NORMAL ("Neural net package:")
LIST_ITEM ("\\bu @FFNet: feed-forward neural net")
LIST_ITEM ("\\bu @Pattern")
LIST_ITEM ("\\bu @Categories: for classification (#CategoriesEditor)")
NORMAL ("Numerical and statistical analysis:")
LIST_ITEM ("\\bu @Eigen: eigenvectors and eigenvalues")
LIST_ITEM ("\\bu @Polynomial, @Roots, @ChebyshevSeries, @LegendreSeries, @ISpline, @MSpline")
LIST_ITEM ("\\bu @Covariance: covariance matrix")
LIST_ITEM ("\\bu @Confusion: confusion matrix")
LIST_ITEM ("\\bu @@Discriminant analysis@: @Discriminant")
LIST_ITEM ("\\bu @@Principal component analysis@: @PCA")
LIST_ITEM ("\\bu @Correlation, @ClassificationTable, @SSCP")
LIST_ITEM ("\\bu @DTW: dynamic time warping")
NORMAL ("@@Multidimensional scaling@:")
LIST_ITEM ("\\bu @Configuration (@Salience)")
LIST_ITEM ("\\bu @@Kruskal analysis@: @Dissimilarity (@Weight), @Similarity")
LIST_ITEM ("\\bu @@INDSCAL analysis@: @Distance, @ScalarProduct")
LIST_ITEM ("\\bu @@Correspondence analysis@: @ContingencyTable")
NORMAL ("Optimality-theoretic learning (see the @@OT learning@ tutorial)")
LIST_ITEM ("\\bu @OTGrammar (@OTGrammarEditor)")
NORMAL ("Bureaucracy")
LIST_ITEM ("\\bu @WordList, @SpellingChecker")
MAN_END

MAN_BEGIN ("View", "ppgb", 20010512)
INTRO ("One of the menus in several @editors and in the @manual.")
MAN_END

MAN_BEGIN ("Write menu", "ppgb", 19970911)
INTRO ("One of the menus in the @@Object window@.")
ENTRY ("Purpose")
NORMAL ("With the Write menu, you write one or more selected @objects from memory to a file on disk. "
	"The data can be read in again with one of the commands in the @@Read menu@ "
	"(most often simply with @@Read from file...@).")
ENTRY ("Usage: save your work")
NORMAL ("You will often choose a command from this menu just before clicking the @Remove button "
	"or choosing the @Quit command.")
ENTRY ("Fixed commands")
NORMAL ("If no object is selected, the Write menu is empty. "
	"If any object is selected, it will at least contain the following commands:")
LIST_ITEM ("\\bu @@Write to console")
LIST_ITEM ("\\bu @@Write to text file...")
LIST_ITEM ("\\bu ##Write to short text file...")
LIST_ITEM ("\\bu @@Write to binary file...")
ENTRY ("Dynamic commands")
NORMAL ("Depending on the class of the selected object, the following commands may be available "
	"in the Write menu:")
MAN_END

MAN_BEGIN ("Write to binary file...", "ppgb", 19970911)
INTRO ("One of the commands in the @@Write menu@.")
ENTRY ("Availability")
NORMAL ("You can choose this command after selecting one or more @objects.")
ENTRY ("Behaviour")
NORMAL ("The Object window will ask you for a file name. "
	"After you click OK, the objects will be written to a binary file on disk.")
ENTRY ("Usage")
NORMAL ("The file can be read again with @@Read from file...@.")
ENTRY ("File format")
NORMAL ("These files are in a device-independent binary format, "
	"and can be written and read on any machine.")
MAN_END

MAN_BEGIN ("Write to console", "ppgb", 19960904)
INTRO ("One of the commands in the @@Write menu@.")
NORMAL ("You can choose this command after selecting one object. "
	"The data that it contains, is written to the Console window, "
	"in the same format as with the @@Write to text file...@ command, "
	"except for the first line, which reads something like:")
CODE ("Write to console: class Sound,  \"name hallo\"")
MAN_END

MAN_BEGIN ("Write to short text file...", "ppgb", 19981124)
INTRO ("One of the commands in the @@Write menu@.")
ENTRY ("Availability")
NORMAL ("You can choose this command after selecting one or more @objects.")
ENTRY ("Behaviour")
NORMAL ("The Object window will ask you for a file name. "
	"After you click OK, the objects will be written to a text file on disk.")
ENTRY ("File format")
NORMAL ("The format is much shorter than the one described at @@Write to text file...@. "
	"Most of the comments are gone, and there is normally one piece of data per line.")
NORMAL ("The file can be read again with the all-purpose @@Read from file...@.")
MAN_END

MAN_BEGIN ("Write to text file...", "ppgb", 19970911)
INTRO ("One of the commands in the @@Write menu@.")
ENTRY ("Availability")
NORMAL ("You can choose this command after selecting one or more @objects.")
ENTRY ("Behaviour")
NORMAL ("The Object window will ask you for a file name. "
	"After you click OK, the objects will be written to a text file on disk.")
ENTRY ("File format")
NORMAL ("The format is the same as used by @@Write to console@, "
	"except for the first two lines:")
LIST_ITEM ("\\bu if you selected a single object, e.g., of class Pitch, "
	"the file will start with the lines:")
CODE ("File type = \"ooTextFile\"")
CODE ("Class = \"Pitch\"")
LIST_ITEM ("\\bu if you selected more than one object, e.g., `Pitch hallo' and `Polygon kromme', "
	"the file will look like:")
CODE ("File type = \"ooTextFile\"")
CODE ("Class = \"Collection\"")
CODE ("size = 2")
CODE ("item []:")
CODE ("    item [1]:")
CODE ("        class = \"Pitch\"")
CODE ("        name = \"hallo\"")
CODE ("        (pitch data...)")
CODE ("    item [2]:")
CODE ("        class = \"Polygon\"")
CODE ("        name = \"kromme\"")
CODE ("        (polygon data...)")
NORMAL ("The file can be read again with @@Read from file...@, "
	"which, by the way, does not need the verbosity of the above example. "
	"The following minimal format will also be read correctly:")
CODE ("\"ooTextFile\"")
CODE ("\"Collection\"  2")
CODE ("\"Pitch\"  \"hallo\"  (pitch data...)")
CODE ("\"Polygon\"  \"kromme\"  (polygon data...)")
NORMAL ("Thus, all text that is not a free-standing number and is not enclosed in double quotes or < >, "
	"is considered a comment, as is all text following an exclamation mark (`!') on the same line.")
MAN_END

MAN_BEGIN ("Numerics library", "ppgb", 20010410)
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

/* End of file manual_tutorials.c */
