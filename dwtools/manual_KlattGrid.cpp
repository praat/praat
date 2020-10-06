/* manual_KlattGrid.cpp
 *
 * Copyright (C) 2009-2014 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ManPagesM.h"

void manual_KlattGrid (ManPages me);
void manual_KlattGrid (ManPages me)
{

MAN_BEGIN (U"KlattGrid", U"djmw", 20140117)
INTRO (U"A KlattGrid represents the source-filter model as a function of time. It consists of a number of tiers that model aspects of the source and the filter, and the interaction between source and filter. The KlattGrid implements a superset of the speech synthesizer described in figure 14 in the @@Klatt & Klatt (1990)@ article.")
NORMAL (U"The following drawing represents a cascade synthesizer with six oral formants, one nasal formant, "
	"one nasal antiformant, one tracheal formant, one tracheal antiformant and six frication formants. ")
SCRIPT (7.0, 6.5,
	U"Create KlattGrid: \"kg\", 0, 1, 6, 1, 1, 6, 1, 1, 1\n"
	"Draw synthesizer: \"Cascade\"\n"
	"Remove\n")
NORMAL (U"In the next picture a parallel synthesizer branch is used instead of the cascade one.")
SCRIPT (7.0, 6.5,
	U"Create KlattGrid: \"kg\", 0, 1, 6, 1, 1, 6, 1, 1, 1\n"
	"Draw synthesizer... Parallel\n"
	"Remove\n")
NORMAL (U"All parameters in the synthesizer are represented by separate tiers.")
ENTRY (U"The source")
NORMAL (U"The source is modelled by the following tiers:")
TAG (U"##Pitch")
DEFINITION (U"models fundamental frequency (in Hertz).")
TAG (U"##Flutter")
DEFINITION (U"models a kind of \"random\" variation of the pitch (with a number between zero and one). ")
TAG (U"##Voicing amplitude")
DEFINITION (U"models the maximum amplitude of the glottal flow (in dB SPL).")
TAG (U"##Open phase")
DEFINITION (U"models the open phase of the glottis (with a number between zero and one). If the tier is empty a default of 0.7 "
	"will be used.")
TAG (U"##Power1#, ##Power2#")
DEFINITION (U"model the form of the glottal flow function flow(%t)=%t^^%%power1%^-%t^^%%power2%^ for 0\\<_ t \\<_ 1. "
	"To make glottal closure possible, %power2 has to be larger than %power1. If the power1 tier is empty, a default "
	"value of 3 will be used. If the power2 tier is empty, a default of 4 will be used.")
TAG (U"##Collision phase")
DEFINITION (U"models the last part of the flow function with an exponential decay function instead of a polynomial one. "
	"More information about #Power1, #Power2, ##Open phase# and ##Collision phase# can be found in the @@PointProcess: To Sound (phonation)...@ manual.")
TAG (U"##Spectral tilt")
DEFINITION (U"models the extra number of dB the voicing spectrum should be down at 3000 Hertz.")
TAG (U"##Aspiration amplitude")
DEFINITION (U"models the (maximum) amplitude of the noise generated at the glottis (in dB SPL). ")
TAG (U"##Breathiness amplitude")
DEFINITION (U"models the maximum breathiness noise amplitude during the open phase of the glottis (in dB SPL). "
	"The amplitude of the breathiness noise is modulated by the glottal flow.")
TAG (U"##Double pulsing")
DEFINITION (U"models diplophonia (by a fraction between zero and one). Whenever this parameter is greater than zero, "
	"alternate pulses are modified. A pulse is modified with this %%single% parameter in %%two% ways: it is %%delayed "
	"in time% and its amplitude is %%attenuated%.  If the double pulsing value is a maximum and equals one, the time of "
	"closure of the first peak coincides with the opening time of the second one. ")
ENTRY (U"The vocal tract filter")
NORMAL (U"The filter is modelled by a number of @@FormantGrid@'s. For parallel synthesis the formant grids that normally "
	"only contain formant frequency and formant bandwidth tiers, have been extended with amplitude tiers. Amplitudes values are in dB. The following formant grids can be used:")
TAG (U"##Oral formants")
DEFINITION (U"represent the \"standard\" oral resonances of the vocal tract. ")
TAG (U"##Nasal formants")
DEFINITION (U"model resonances in the nasal tract. Because the form of the nasal tract does not vary much during the course of an utterance, nasal formants tend to be constant. ")
TAG (U"##Nasal antiformants")
DEFINITION (U"model dips in the spectrum caused by leakage to the nasal tract.")
ENTRY (U"Interaction between source and filter")
NORMAL (U"The interaction between source and filter is modelled by two formant grids.")
TAG (U"##Tracheal formants")
DEFINITION (U"model one aspect of the coupling of the trachea with the vocal tract transfer function, namely, by the "
	"introduction of extra formants (and antiformants) that sometimes distort vowel spectra to a varying degrees. "
	"According to @@Klatt & Klatt (1990)@, the other effect being increased losses at glottal termination which primarily affect first-formant bandwidths.")
TAG (U"##Tracheal antiformants")
DEFINITION (U"model dips in the spectrum caused by the trachea.")
TAG (U"##Delta formants")
DEFINITION (U"The values in this grid model the number of hertz that the oral formants and/or bandwidths change during the open phase "
	"of the glottis. @@Klatt & Klatt (1990)@ distinguish four types of source-filter interactions: an F1 ripple in the "
	"source waveform, a non-linear interaction between the first formant and the fundamental frequency, a truncation of "
	"the first formant and tracheal formants and antiformants. ")
ENTRY (U"The frication section")
NORMAL (U"The frication section is modelled with a frication formant grid, with formant frequencies, bandwidths and (separate) "
	"amplitudes (dB), a frication by-pass tier (dB) and an amplitude tier (dB SPL) that governs the frication noise source.")
ENTRY (U"A minimal synthesizer")
NORMAL (U"The following script produces a minimal voiced sound. The first line creates the standard KlattGrid."
	"The next two lines define a pitch point, in Hz, and the voicing amplitude, in dB. The last line "
	"creates the sound.")
CODE (U"Create KlattGrid: \"kg\", 0, 1, 6, 1, 1, 6, 1, 1, 1")
CODE (U"Add pitch point: 0.5, 100")
CODE (U"Add voicing amplitude point: 0.5, 90")
CODE (U"To Sound")
NORMAL (U"The following script will produce raw frication noise. Because we do not specify formant amplitudes, "
	"we turn off the formants in the parallel section.")
CODE (U"Create KlattGrid: \"kg\", 0, 1, 6, 1, 1, 6, 1, 1, 1")
CODE (U"Add frication amplitude point: 0.5 ,80")
CODE (U"Add frication bypass point: 0.5, 0")
CODE (U"To Sound (special): 0, 0, 44100, \"yes\", \"no\", \"yes\", \"yes\", \"yes\", \"yes\",")
CODE (U"... \"Powers in tiers\", \"yes\", \"yes\", \"yes\",")
CODE (U"... \"Cascade\", 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, \"yes\"")
ENTRY (U"Changes")
NORMAL (U"In praat versions before 5.1.05 the values for the %%oral / nasal / tracheal formant amplitudes% and"
	" %%frication bypass amplitude% had to be given in dB SPL; "
	" now they are in real dB's, i.e."
	" 0 dB means no change in amplitude. You can calculate new values from old values as:\n"
	"new_value = old_value + 20*log10(2e-5). This means that you have to subtract approximately 94 dB from the old values.")
MAN_END

MAN_BEGIN (U"Create KlattGrid...", U"djmw", 20081224)
INTRO (U"A command to create a multitier @@KlattGrid@ speech synthesizer.")
MAN_END

MAN_BEGIN (U"Create KlattGrid from vowel...", U"djmw", 20191007)
INTRO (U"Create a new @@KlattGrid@ from the specifications for a vowel.")
ENTRY (U"Settings")
TAG (U"##Name#")
DEFINITION (U"defines the name that the newly created KlattGrid will get in the list of objects.")
TAG (U"##Duration (s)#")
DEFINITION (U"defines the duration of the KlattGrid. The created KlattGrid will have a start time of 0 seconds "
	"and an end time of %%duration% seconds. ")
TAG (U"##Pitch (Hz)#")
DEFINITION (U"defines the value of the pitch point that will be added at the start of the Klattgrid.")
TAG (U"##F1 (Hz)#, ##F2 (Hz)#, ##F3 (Hz), F4 (Hz)##")
DEFINITION (U"define the frequencies in hertz of the first four formants. If a frequency is not positive the formant is not used.")
TAG (U"##B1 (Hz)#, ##B2 (Hz)#, ##B3 (Hz)#")
DEFINITION (U"define the bandwidths in hertz of the first three formants")
TAG (U"##Bandwidth fraction#")
DEFINITION (U"defines the bandwidths of the fourth and higher formants as a fraction of the formant's frequency (i.e. it is the inverse of the formant's %%quality factor%). "
	"For example if F4 equals 2800 Hz and the \"Bandwidth fraction\" was chosen as 0.05 then its "
	"bandwidth will be 0.05*2800 = 140 Hz. The quality factor of this fourth formant is 20.")
TAG (U"##Formant frequency interval (Hz)#")
DEFINITION (U"defines the distances between the following formants in hertz. For example, if this values is chosen "
	"as 1100 Hz and if F4 happens to be 3000 Hz then F5 will be 4100 Hz, F6 will be 5200 Hz, F7 will be 6300 Hz, etc."
	"If the value is not positive these formants will not be created at all. You would typically choose this value "
	"as 1000 / 1100 Hz for a male / female voice." )
ENTRY (U"Examples")
NORMAL (U"The following script creates a vowel sound with many formants wich will sound like the vowel /a/. "
	"The formant frequencies will be: F1 = 800 Hz, F2 = 1200 Hz, F3 = 2300 Hz, F4 = 2800 Hz. The frequencies of the higher "
	"formants will be at intervals of 1000 Hz, starting from F4. Therefore, F5 = 3800 Hz, F6 = 4800 Hz, F7 = 5800 Hz, "
	"and so on. The bandwidths will be B1 = 80 Hz, B2 = 80 Hz, B3 = 100 Hz. "
	"The bandwidths of the fourth and higher formants will be 0.05 times their frequency. Therefore, B4 = 140 Hz, "
	"B5 = 190 Hz, B6 = 240 Hz, B7 = 290 Hz, and so on. ")
CODE (U"Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.05, 1000")
CODE (U"To Sound")
NORMAL (U"The following script will create a two formant sound which also sounds like an /a/."
	"The formant frequencies will be 800 Hz and 1200 Hz.")
CODE (U"Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 0, 100, 0, 0.1, 0")
CODE (U"To Sound")
NORMAL (U"The following script will create a formant sound which also sounds like an /a/."
	" The formant frequencies will be 800 Hz, 1200 Hz, 2200 Hz, 3200 Hz, 4200, and so on, with 1000 Hz separation).")
CODE (U"Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 0, 100, 0, 0.05, 1000")
CODE (U"To Sound")
NORMAL (U"Because all the frequency points of the corresponding tiers in this KlattGrid are defined at "
	"the start time of the grid, i.e. at time 0.0 seconds, it is easy "
	"to change the characteristics of the vowel sound by adding new points. For example, given one of the /a/ sounds above "
	"which were all synthesized with constant pitch we can have a falling pitch with:")
CODE (U"Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.05, 1000")
CODE (U"Add pitch point: 0.3, 100.0")
CODE (U"To Sound")
NORMAL (U"An /au/ diphthong is also easily made by a simple extension with two oral formant frequency points:")
CODE (U"Create KlattGrid from vowel: \"au\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.05, 1000")
CODE (U"Add pitch point: 0.3, 100.0")
CODE (U"Add oral formant frequncy point: 1, 0.3, 300")
CODE (U"Add oral formant frequncy point: 2, 0.3, 600")
CODE (U"To Sound")
NORMAL (U"Formant frequencies and bandwidths for 16 Swedish vowels are presented by "
	"@@Hawks & Miller (1995)@ in their table 1. They further give equations for bandwidths as a function "
	"of formant frequency.\n"
	"@@Fleisher et al. (2015)@ present in their table 1 values for the first 5 formant frequencies and bandwidths of the German vowels /a/, /i/ and /\\hs/ for speech mode and for singing mode. In general their bandwidths are lower than as predicted by the Hawks and Miller equations.")
MAN_END

#define PhonationGrid_to_Sound_COMMON_PARAMETERS_HELP \
TAG (U"##Sampling frequency (Hz)") \
DEFINITION (U"the @@sampling frequency@ of the resulting sound.") \
TAG (U"##Voicing") \
DEFINITION (U"switches voicing on or off.") \
TAG (U"##Flutter") \
DEFINITION (U"switches the flutter tier on or off. This will, of course, only have effect if at least one flutter point has been defined in the flutter tier.") \
TAG (U"##Double pulsing") \
DEFINITION (U"switches the double pulsing tier on or off.") \
TAG (U"##Collision phase") \
DEFINITION (U"switches the collision phase tier on or off.") \
TAG (U"##Spectral tilt") \
DEFINITION (U"switches the spectral tilt tier on or off.") \
TAG (U"##Flow function") \
DEFINITION (U"determines which flow function will be used. The flow function is determined by two parameters, %%power1% and %%power2% as %%flow(t)=x^^power1^-x^^power2^%. " \
	"If the option \"Powers in tier\" is chosen the power1 and power2 tiers will be used for the values of %%power1% and %%power2%. The other choices switch the two tiers off and instead fixed values will be used for %%power1% and %%power2%.") \
TAG (U"##Flow derivative") \
DEFINITION (U"determines whether the flow or the flow derivative is used for phonation. ") \
TAG (U"##Aspiration") \
DEFINITION (U"determines whether aspiration is included in the synthesis.") \
TAG (U"##Breathiness") \
DEFINITION (U"determines whether breathiness is included in the synthesis.")

MAN_BEGIN (U"KlattGrid: To Sound (phonation)...", U"djmw", 20090122)
INTRO (U"A command to synthesize a Sound from the selected @@KlattGrid@.")
ENTRY (U"Settings")
PhonationGrid_to_Sound_COMMON_PARAMETERS_HELP
MAN_END

MAN_BEGIN (U"KlattGrid: Play special...", U"djmw", 20090421)
INTRO (U"A command to play part of a @@KlattGrid@.")
ENTRY (U"Settings")
TAG (U"##Time range (s)")
DEFINITION (U"determines the part of the sound's domain that you want to hear. If both argument equal zero the complete sound is played. ")
TAG (U"##Sampling frequency (Hz)")
DEFINITION (U"determines the @@sampling frequency@ of the resulting sound.")
TAG (U"##Scale peak")
DEFINITION (U"determines whether the peak value of the sound will be set to 0.99. In this way the sound will always play well.")
PhonationGrid_to_Sound_COMMON_PARAMETERS_HELP
TAG (U"##Model")
DEFINITION (U"switches on either the cascade or the parallel section of the synthesizer.")
TAG (U"##Oral formant range#, ##Nasal formant range# ...")
DEFINITION (U"selects the formants to use in the synthesis. Choosing the end of a range smaller than the start of the range switches off the formants. ")
TAG (U"##Frication bypass")
DEFINITION (U"switches the frication bypass of the frication section on or off. "
	"The complete frication section can be turned off by also switching off the frication formants.")
MAN_END

MAN_BEGIN (U"KlattGrid: To Sound (special)...", U"djmw", 20090415)
INTRO (U"A command to synthesize a Sound from the selected @@KlattGrid@.")
ENTRY (U"Settings")
TAG (U"##Time range (s)")
DEFINITION (U"determines the part of the domain that you want to save as a sound. If both argument equal zero the complete sound is created. ")
TAG (U"##Sampling frequency (Hz)")
DEFINITION (U"determines the @@sampling frequency@ of the resulting sound.")
TAG (U"##Scale peak")
DEFINITION (U"determines whether the peak value of the sound will be set to 0.99. In this way the sound will always play well and can be saved to a file with minimal loss of resolution.")
PhonationGrid_to_Sound_COMMON_PARAMETERS_HELP
TAG (U"##Model")
DEFINITION (U"switches on either the cascade or the parallel section of the synthesizer.")
TAG (U"##Oral formant range#, ##Nasal formant range# ...")
DEFINITION (U"selects the formants to use in the synthesis. Choosing the end of a range smaller than the start of the range switches off the formants. ")
TAG (U"##Frication bypass")
DEFINITION (U"switches the frication bypass of the frication section on or off. "
	"The complete frication section can be turned off by also switching off the frication formants.")
MAN_END

MAN_BEGIN (U"KlattGrid: Extract oral formant grid (open phases)...", U"djmw", 20090421)
INTRO (U"Extracts the oral formant grid as used in the synthesis, i.e. the resulting grid contains the informantion from the oral formant grid and the delta formant grid combined during the open phase of the glottis. ")
MAN_END

MAN_BEGIN (U"KlattTable", U"djmw", 20160601)
INTRO (U"The parameters for the Klatt synthesizer in table format. You can convert it to a @@KlattGrid@ which is easier to view and edit.")
MAN_END

MAN_BEGIN (U"Sound: To KlattGrid (simple)...", U"djmw", 20090415)
INTRO (U"Create a @@KlattGrid@ from a @@Sound@.")
ENTRY (U"Algorithm")
NORMAL (U"Oral formants are determined by the @@Sound: To Formant (burg)...@ method and transformed to a @@FormantGrid@. "
	"Pitch is determined by the @@Sound: To Pitch...@ method and transformed to a @@PitchTier@. "
	"With @@Sound: To Intensity...@ we determine intensity and convert it to an @@IntensityTier@.")
NORMAL (U"Next a KlattGrid is created whose time domain conforms to the sound. Its pitch tier, normal formant grid "
	"and its voicing amplitude are replaced by the results from the analyses.")
MAN_END

MAN_BEGIN (U"Sound & KlattGrid: Filter by vocal tract...", U"djmw", 20090108)
INTRO (U"Filters the selected sound with the vocal tract part of the selected @@KlattGrid@. During this filtering the delta formants and bandwidths are %%not% used. ")
MAN_END

}

/* End of file manual_KlattGrid.cpp */
