/* manual_KlattGrid.cpp
 *
 * Copyright (C) 2009-2014,2023,2025 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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
MAN_PAGES_BEGIN
R"~~~(
################################################################################
"KlattGrid"
© David Weenink 2014-01-17

A KlattGrid represents the source-filter model as a function of time.
It consists of a number of tiers that model aspects of the source and the filter,
and the interaction between source and filter. The KlattGrid implements a superset
of the speech synthesizer described in figure 14 by @@Klatt & Klatt (1990)@.

The following drawing represents a cascade synthesizer with six oral formants, one nasal formant,
one nasal antiformant, one tracheal formant, one tracheal antiformant and six frication formants.
{- 7.0x6.5
	Create KlattGrid: "kg", 0, 1, 6, 1, 1, 6, 1, 1, 1
	Draw synthesizer: "Cascade"
	Remove
}
In the next picture a parallel synthesizer branch is used instead of the cascade one.
{- 7.0x6.5
	Create KlattGrid: "kg", 0, 1, 6, 1, 1, 6, 1, 1, 1
	Draw synthesizer: "Parallel"
	Remove
}
All parameters in the synthesizer are represented by separate tiers.

The source
==========
The source is modelled by the following tiers:

##Pitch
: models fundamental frequency (in Hertz).

##Flutter
: models a kind of “random” variation of the pitch (with a number between zero and one).

##Voicing amplitude
: models the maximum amplitude of the glottal flow (in dB SPL).

##Open phase
: models the open phase of the glottis (with a number between zero and one).
  If the tier is empty, a default of 0.7 will be used.

##Power1#, ##Power2#
: model the shape of the glottal flow function %flow(%t)=%t^^%power1^\-m%t^^%power2^ for 0 \<_ %t \<_ 1.
  To make glottal closure possible, %power2 has to be larger than %power1.
  If the power1 tier is empty, a default value of 3 will be used.
  If the power2 tier is empty, a default of 4 will be used.

##Collision phase
: models the last part of the flow function with an exponential decay function instead of a polynomial one.
  More information about #Power1, #Power2, ##Open phase# and ##Collision phase#
  can be found in the @@PointProcess: To Sound (phonation)...@ manual.

##Spectral tilt
: models the extra number of dB the voicing spectrum should be down at 3000 Hertz.

##Aspiration amplitude
: models the (maximum) amplitude of the noise generated at the glottis (in dB SPL).

##Breathiness amplitude
: models the maximum breathiness noise amplitude during the open phase of the glottis (in dB SPL).
  The amplitude of the breathiness noise is modulated by the glottal flow.

##Double pulsing
: models diplophonia (by a fraction between zero and one). Whenever this parameter is greater than zero,
  alternate pulses are modified. A pulse is modified with this %%single% parameter in %%two% ways: it is %%delayed
  in time% and its amplitude is %%attenuated%.  If the double pulsing value is a maximum and equals one, the time of
  closure of the first peak coincides with the opening time of the second one.

The vocal tract filter
======================
The filter is modelled by a number of @@FormantGrid@'s. For parallel synthesis the formant grids that normally
only contain formant frequency and formant bandwidth tiers, have been extended with amplitude tiers.
Amplitudes values are in dB. The following formant grids can be used:

##Oral formants
: represent the “standard” oral resonances of the vocal tract.

##Nasal formants
: model resonances in the nasal tract. Because the shape of the nasal tract does not vary much
  during the course of an utterance, nasal formants tend to be constant.

##Nasal antiformants
: model dips in the spectrum caused by leakage to the nasal tract.

Interaction between source and filter
=====================================
The interaction between source and filter is modelled by two formant grids.

##Tracheal formants
: model one aspect of the coupling of the trachea with the vocal tract transfer function, namely, by the
  introduction of extra formants (and antiformants) that sometimes distort vowel spectra to a varying degrees.
  According to @@Klatt & Klatt (1990)@, the other effect of tracheal formants is increased losses
  at glottal termination which primarily affect first-formant bandwidths.

##Tracheal antiformants
: model dips in the spectrum caused by the trachea.

##Delta formants
: The values in this grid model the number of hertz that the oral formants and/or bandwidths change during the open phase
  of the glottis. @@Klatt & Klatt (1990)@ distinguish four types of source-filter interactions: an F1 ripple in the
  source waveform, a non-linear interaction between the first formant and the fundamental frequency, a truncation of
  the first formant and tracheal formants and antiformants.

The frication section
=====================
The frication section is modelled with a frication formant grid, with formant frequencies, bandwidths and (separate)
amplitudes (dB), a frication by-pass tier (dB) and an amplitude tier (dB SPL) that governs the frication noise source.

A minimal synthesizer
=====================
The following script produces a minimal voiced sound. The first line creates the standard KlattGrid."
The next two lines define a pitch point, in Hz, and the voicing amplitude, in dB. The last line
creates the sound.
{;
	Create KlattGrid: "kg", 0, 1, 6, 1, 1, 6, 1, 1, 1
	Add pitch point: 0.5, 100
	Add voicing amplitude point: 0.5, 90
	To Sound
}
The following script will produce raw frication noise. Because we do not specify formant amplitudes,
we turn off the formants in the parallel section.
{;
	Create KlattGrid: "kg", 0, 1, 6, 1, 1, 6, 1, 1, 1
	Add frication amplitude point: 0.5 ,80
	Add frication bypass point: 0.5, 0
	To Sound (special): 0, 0, 44100, "yes", "no", "yes", "yes", "yes", "yes",
	... "Powers in tiers", "yes", "yes", "yes",
	... "Cascade", 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, "yes"
}
Changes
=======
In Praat versions before 5.1.05 the values for the %%oral / nasal / tracheal formant amplitudes% and
%%frication bypass amplitude% had to be given in dB SPL;
now they are in real dB's, i.e.
0 dB means no change in amplitude. You can calculate new values from old values as:
`new_value = old_value + 20*log10(2e-5)`.
This means that you have to subtract approximately 94 dB from the old values.

################################################################################
"Create KlattGrid..."
© David Weenink 2008-12-24

A command to create a multitier @@KlattGrid@ speech synthesizer.

################################################################################
"Create KlattGrid from vowel..."
© David Weenink 2023-08-01

Create a new @@KlattGrid@ from the specifications for a vowel.

Settings
========

##Name#
: defines the name that the newly created KlattGrid will get in the list of objects.

##Duration (s)#
: defines the duration of the KlattGrid. The created KlattGrid will have a start time of 0 seconds
and an end time of %%duration% seconds.

##Pitch (Hz)#
: defines the value of the pitch point that will be added at the start of the Klattgrid.

##F1 (Hz)#, ##F2 (Hz)#, ##F3 (Hz), F4 (Hz)##
: define the frequencies in hertz of the first four formants. If a frequency is not positive the formant is not used.

##B1 (Hz)#, ##B2 (Hz)#, ##B3 (Hz)#
: define the bandwidths in hertz of the first three formants

##Bandwidth fraction#
: defines the bandwidths of the fourth and higher formants as a fraction of
  the formant's frequency (i.e. it is the inverse of the formant's %%quality factor%).
  For example if F4 equals 2800 Hz and the "Bandwidth fraction" was chosen as 0.05 then its
  bandwidth will be 0.05*2800 = 140 Hz. The quality factor of this fourth formant is 20.

##Formant frequency interval (Hz)#
: defines the distances between the following formants in hertz. For example, if this values is chosen
  as 1100 Hz and if F4 happens to be 3000 Hz then F5 will be 4100 Hz, F6 will be 5200 Hz, F7 will be 6300 Hz, and so on.
  If the value is not positive these formants will not be created at all. You would typically choose this value
  as 1000 or 1100 Hz for an average male or female voice, respectively.

Examples
========
The following script creates a vowel sound with many formants which will sound like the vowel [a].
The formant frequencies will be: F1 = 800 Hz, F2 = 1200 Hz, F3 = 2300 Hz, F4 = 2800 Hz. The frequencies of the higher
formants will be at intervals of 1000 Hz, starting from F4. Therefore, F5 = 3800 Hz, F6 = 4800 Hz, F7 = 5800 Hz,
and so on. The bandwidths will be B1 = 80 Hz, B2 = 80 Hz, B3 = 100 Hz.
The bandwidths of the fourth and higher formants will be 0.05 times their frequency. Therefore, B4 = 140 Hz,
B5 = 190 Hz, B6 = 240 Hz, B7 = 290 Hz, and so on.
{;
	Create KlattGrid from vowel: "a", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.05, 1000
	To Sound
}
The following script will create a two formant sound which also sounds like [a].
The formant frequencies will be 800 Hz and 1200 Hz.
{;
	Create KlattGrid from vowel: "a", 0.3, 125, 800, 80, 1200, 80, 0, 100, 0, 0.1, 0
	To Sound
}
The following script will create a formant sound which also sounds like [a].
The formant frequencies will be 800 Hz, 1200 Hz, 2200 Hz, 3200 Hz, 4200, and so on, with 1000 Hz separation).
{;
	Create KlattGrid from vowel: "a", 0.3, 125, 800, 80, 1200, 80, 0, 100, 0, 0.05, 1000
	To Sound
}
Because all the frequency points of the corresponding tiers in this KlattGrid are defined at
the start time of the grid, i.e. at time 0.0 seconds, it is easy
to change the characteristics of the vowel sound by adding new points. For example, given one of the [a] sounds above
which were all synthesized with constant pitch we can have a falling pitch with:
{;
	Create KlattGrid from vowel: "a", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.05, 1000
	Add pitch point: 0.3, 100.0
	To Sound
}
An [au] diphthong is also easily made by a simple extension with two oral formant frequency points:
{;
	Create KlattGrid from vowel: "au", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.05, 1000
	Add pitch point: 0.3, 100.0
	Add oral formant frequency point: 1, 0.3, 300
	Add oral formant frequency point: 2, 0.3, 600
	To Sound
}
Formant frequencies and bandwidths for 16 Swedish vowels are presented by
@@Hawks & Miller (1995)@ in their table 1. They further give equations for bandwidths as a function
of formant frequency.
@@Fleisher et al. (2015)@ present in their table 1 values for the first 5 formant frequencies
and bandwidths of the German vowels /a/, /i/ and /\hs/ for speech mode and for singing mode.
In general their bandwidths are lower than as predicted by the Hawks and Miller equations.

################################################################################
"KlattGrid: To Sound (phonation)..."
© David Weenink 2009-01-22

A command to synthesize a Sound from the selected @@KlattGrid@.

Settings
========
##Sampling frequency (Hz)
: the @@sampling frequency@ of the resulting sound.

##Voicing
: switches voicing on or off.

##Flutter
: switches the flutter tier on or off. This will, of course, only have effect if at least one flutter point has been defined in the flutter tier.

##Double pulsing
: switches the double pulsing tier on or off.

##Collision phase
: switches the collision phase tier on or off.

##Spectral tilt
: switches the spectral tilt tier on or off.

##Flow function
: determines what flow function will be used. The flow function is determined by two parameters, %power1 and %power2, as %flow(%t)=%x^^%power1^\-m%x^^%power2^.
  If the option "Powers in tier" is chosen the power1 and power2 tiers will be used for the values of %power1 and %power2.
  The other choices switch the two tiers off and instead fixed values will be used for %power1 and %power2.

##Flow derivative
: determines whether the flow or the flow derivative is used for phonation.

##Aspiration
: determines whether aspiration is included in the synthesis.

##Breathiness
: determines whether breathiness is included in the synthesis.

################################################################################
"KlattGrid: Play special..."
© David Weenink 2009-04-21, Paul Boersma 2023

A command to play part of a @@KlattGrid@.

Settings
========

##Time range (s)
: determines the part of the sound’s time domain that you want to hear.
  If both arguments equal zero, the complete sound will be played.

##Scale peak
: determines whether the peak value of the sound will be set to 0.99. In this way the sound will always play well.

##Voicing
: switches voicing on or off.

##Flutter
: switches the flutter tier on or off. This will, of course, only have effect if at least one flutter point has been defined in the flutter tier.

##Double pulsing
: switches the double pulsing tier on or off.

##Collision phase
: switches the collision phase tier on or off.

##Spectral tilt
: switches the spectral tilt tier on or off.

##Flow function
: determines what flow function will be used. The flow function is determined by two parameters, %power1 and %power2, as %flow(%t)=%x^^%power1^\-m%x^^%power2^.
  If the option "Powers in tier" is chosen the power1 and power2 tiers will be used for the values of %power1 and %power2.
  The other choices switch the two tiers off and instead fixed values will be used for %power1 and %power2.

##Flow derivative
: determines whether the flow or the flow derivative is used for phonation.

##Aspiration
: determines whether aspiration is included in the synthesis.

##Breathiness
: determines whether breathiness is included in the synthesis.

##Model
: switches on either the cascade or the parallel section of the synthesizer.

##Oral formant range#, ##Nasal formant range# ...
: selects the formants to use in the synthesis. Choosing the end of a range smaller than the start of the range switches off the formants.

##Frication bypass
: switches the frication bypass of the frication section on or off.
  The complete frication section can be turned off by also switching off the frication formants.

################################################################################
"KlattGrid: To Sound (special)..."
© David Weenink 2009-04-15, Paul Boersma 2023

A command to synthesize a Sound from the selected @@KlattGrid@.

Settings
========

##Time range (s)
: determines the part of the time domain that you want to save as a sound.
  If both arguments equal zero, the complete sound will be created.

##Sampling frequency (Hz)
: determines the @@sampling frequency@ of the resulting sound.

##Scale peak
: determines whether the peak value of the sound will be set to 0.99. In this way the sound will always play well and can be saved to a file with minimal loss of resolution.

##Voicing
: switches voicing on or off.

##Flutter
: switches the flutter tier on or off. This will, of course, only have effect if at least one flutter point has been defined in the flutter tier.

##Double pulsing
: switches the double pulsing tier on or off.

##Collision phase
: switches the collision phase tier on or off.

##Spectral tilt
: switches the spectral tilt tier on or off.

##Flow function
: determines what flow function will be used. The flow function is determined by two parameters, %power1 and %power2, as %flow(%t)=%x^^%power1^\-m%x^^%power2^.
  If the option "Powers in tier" is chosen the power1 and power2 tiers will be used for the values of %power1 and %power2.
  The other choices switch the two tiers off and instead fixed values will be used for %power1 and %power2.

##Flow derivative
: determines whether the flow or the flow derivative is used for phonation.

##Aspiration
: determines whether aspiration is included in the synthesis.

##Breathiness
: determines whether breathiness is included in the synthesis.

##Model
: switches on either the cascade or the parallel section of the synthesizer.

##Oral formant range#, ##Nasal formant range# ...
: selects the formants to use in the synthesis. Choosing the end of a range smaller than the start of the range switches off the formants.

##Frication bypass
: switches the frication bypass of the frication section on or off.
  The complete frication section can be turned off by also switching off the frication formants.

################################################################################
"KlattGrid: Extract oral formant grid (open phases)..."
© David Weenink 2009-04-21

Extracts the oral formant grid as used in the synthesis, i.e. the resulting grid contains
the informantion from the oral formant grid and the delta formant grid combined during the open phase of the glottis.

################################################################################
"KlattTable"
© David Weenink 2016-06-01

The parameters for the Klatt synthesizer in table format. You can convert it to a @@KlattGrid@, which is easier to view and edit.

################################################################################
"Sound: To KlattGrid (simple)..."
© David Weenink 2009-04-15

Create a @@KlattGrid@ from a @@Sound@.

Algorithm
=========
Oral formants are determined by the method of @@Sound: To Formant (burg)...@, and transformed to a @@FormantGrid@.
Pitch is determined by the method of @@Sound: To Pitch...@, and transformed to a @@PitchTier@.
Intensity is determined by the method of @@Sound: To Intensity...@, and transformed to an @@IntensityTier@.

Next, a KlattGrid is created whose time domain conforms to the sound. Its pitch tier, normal formant grid
and voicing amplitude are replaced by the results from the analyses.

################################################################################
"Sound & KlattGrid: Filter by vocal tract..."
© David Weenink 2009-01-08

Filters the selected sound with the vocal tract part of the selected @@KlattGrid@.
During this filtering the delta formants and bandwidths are %not used.

################################################################################
)~~~"
MAN_PAGES_END
}

/* End of file manual_KlattGrid.cpp */
