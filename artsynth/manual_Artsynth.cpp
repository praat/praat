/* manual_Artsynth.cpp
 *
 * Copyright (C) 1992-2005,2007,2010,2011,2014-2017,2020 Paul Boersma
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

void manual_Artsynth_init (ManPages me);
void manual_Artsynth_init (ManPages me) {

MAN_BEGIN (U"Articulatory synthesis", U"ppgb", 20111028)
INTRO (U"This is a description of the articulatory synthesis package in Praat. "
	"For a detailed description of the physics and mathematics behind the model, "
	"see @@Boersma (1998)@, chapters 2 and 3. "
	"For examples of how to synthesize utterances, consult http://www.fon.hum.uva.nl/paul/diss/ch5/.")
ENTRY (U"How to start (after reading the @Intro)")
NORMAL (U"We are going to have the synthesizer say [əpə]. We need a @Speaker and an @Artword object.")
NORMAL (U"1. Create a speaker with @@Create Speaker...@ from the @@New menu@.")
NORMAL (U"2. Create an articulation word of 0.5 seconds with @@Create Artword...@.")
NORMAL (U"3. Edit the Artword by selecting it and clicking ##View & Edit#.")
NORMAL (U"4. To set the glottis to a position suitable for phonation, use the ArtwordEditor to "
	"set the %Interarytenoid activity to 0.5 throughout the utterance. You set two targets: "
	"0.5 at a time of 0 seconds, and 0.5 at a time of 0.5 seconds.")
NORMAL (U"5. To prevent air escaping from the nose, close the nasopharyngeal port "
	"by setting the %LevatorPalatini activity to 1.0 throughout the utterance.")
NORMAL (U"6. To generate the lung pressure needed for phonation, you set the %Lungs activity at 0 seconds to 0.2, "
	"and at 0.1 seconds to 0.")
NORMAL (U"7. To force a jaw movement that closes the lips, set the %Masseter activity at 0.25 seconds to 0.7, "
	"and the %OrbicularisOris activity at 0.25 seconds to 0.2.")
NORMAL (U"8. Select the Speaker and the Artword and click #Movie; "
	"you will see a closing-and-opening gesture of the mouth.")
NORMAL (U"9. Select the Speaker and the Artword and click ##To Sound...# "
	"(see @@Artword & Speaker: To Sound...@).")
NORMAL (U"10. Just click #OK; the synthesis starts.")
NORMAL (U"11. If you are sitting at a 1997 computer, this will last for 5 minutes or so; at a 2010 computer, 6 seconds. "
	"If this is too slow for you, click #Interrupt. "
	"Otherwise, you can watch the vibrating vocal cords "
	"and the changing vocal-tract shape.")
NORMAL (U"12. You can play, view, and analyse the resulting @Sound as you would any other. "
	"You can see and hear a sound movie if you select the Speaker, "
	"the Artword, and the Sound, and click #Play.")
MAN_END

MAN_BEGIN (U"Artword", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. See @@Articulatory synthesis@.")
NORMAL (U"An object of class Artword object represents the activities of several speech muscles as functions of time.")
ENTRY (U"Artword commands")
LIST_ITEM (U"• @@Create Artword...@: creates an Artword with relaxed muscles")
LIST_ITEM (U"• @@Artword & Speaker: To Sound...@: articulatory synthesis")
MAN_END

MAN_BEGIN (U"Artword & Speaker: To Sound...", U"ppgb", 20040331)
INTRO (U"A command to synthesize a @Sound object from the selected @Speaker and the selected @Artword.")
NORMAL (U"This is the command that performs the actual articulatory synthesis. "
	"See @@Articulatory synthesis@.")
ENTRY (U"Settings")
TAG (U"##Sampling frequency (Hz)")
DEFINITION (U"the number of times per second that the equilibrium widths and lengths and the tensions of the muscles "
	"are recomputed from the Artword. This will also be the sampling frequency of the resulting sound and "
	"of the optional resulting tube widths, air pressures, and air velocities. The standard value is 22050 Hz.")
TAG (U"##Oversampling")
DEFINITION (U"the number of times that the aerodynamic quantities and the state of the tube walls "
	"will be recomputed during each sample period. The standard value is 25.")
TAG (U"##Width 1#, ##Width 2#, ##Width 3#")
DEFINITION (U"the numbers (see below) of the tubes whose widths you want to monitor. "
	"E.g., if ##Width 1# is 36, the synthesizer will create a Sound object named $$width36$, "
	"which contains the width of tube 36 (the lower glottis) as a function of time, expressed in metres. "
	"To prevent the creation of a $$width$ object, specify $$0$ (the standard value).")
TAG (U"##Pressure 1#, ##Pressure 2#, ##Pressure 3#")
DEFINITION (U"the numbers (see below) of the tubes whose air pressures you want to monitor. "
	"E.g., if ##Pressure 3# is 37, the synthesizer will create a Sound object named $$pressure37$, "
	"which contains the air pressure of tube 37 (the upper glottis) as a function of time, expressed in Pascal. "
	"To prevent the creation of a $$pressure$ object, specify $$0$ (the standard value).")
TAG (U"##Velocity 1#, ##Velocity 2#, ##Velocity 3")
DEFINITION (U"the numbers (see below) of the tubes whose air velocities you want to monitor. "
	"E.g., if ##Velocity 1# is 60, the synthesizer will create a Sound object named $velocity60, "
	"which contains the air velocity of tube 60 (in the mouth) as a function of time, expressed in metres per second. "
	"To prevent the creation of a $velocity object, specify $0 (the standard value).")
ENTRY (U"Stability")
NORMAL (U"The internal sampling frequency for the aerodynamics is the specified ##Sampling frequency#, "
	"multiplied by the specified #Oversampling. With the standard settings, this is 22050 times 25 = 550750 Hz.")
NORMAL (U"To ensure the stability of the synthesis, this internal sampling frequency should not be less than the "
	"velocity of sound (353 m/s) divided by the length of the shortest tube. For the standard #Female, "
	"#Male, and #Child speakers, the shortest tube is the upper glottis, which has a length "
	"of 0.7, 1.0, and 0.3 millimetres, respectively. The minimum internal sampling frequencies, therefore, "
	"are 504286, 353000, and 1176667 hertz, respectively.")
ENTRY (U"Time resolution")
NORMAL (U"To capture the microscopic pressure changes in the glottis, you will want maximum time resolution. "
	"For a female speaker, you could set ##Sampling frequency# to 550750 Hz, and #Oversampling to 1.")
ENTRY (U"Tube numbers")
NORMAL (U"Here are the tube numbers that you can use for the #Width, #Pressure, and #Velocity settings:")
LIST_ITEM (U"1..23: lungs (from bottom to top)")
LIST_ITEM (U"24..29: bronchi (from bottom to top)")
LIST_ITEM (U"30..35: trachea (from bottom to top)")
LIST_ITEM (U"36: lower glottis")
LIST_ITEM (U"37: upper glottis (not for a one-mass model)")
LIST_ITEM (U"38..49: pharynx (from bottom to top)")
LIST_ITEM (U"50..51: nasopharyngeal branching")
LIST_ITEM (U"52..64: mouth (from back to front)")
LIST_ITEM (U"65..78: nose (from back to front)")
LIST_ITEM (U"79..86: conus elasticus (only for a 10-mass model)")
LIST_ITEM (U"87..89: glottal shunt between the arytenoids (from bottom to top)")
NORMAL (U"Some structural properties:")
LIST_ITEM (U"• Tube 1 is closed at the bottom.")
LIST_ITEM (U"• Tubes 64 (lips) and 78 (nostrils) radiate into the air.")
LIST_ITEM (U"• The nasopharyngeal branch is at tubes 50, 51, and 65. They are constrained to have equal lengths.")
LIST_ITEM (U"• For a one-mass model of the vocal cords, tube 36 is connected to 38.")
LIST_ITEM (U"• For a 10-mass model, tubes 32..35 are replaced with 79..86, so that "
	"tube 31 is connected to 79, and 86 is connected to 36.")
LIST_ITEM (U"• A glottal shunt will be implemented if the speaker's $$shunt.Dx$ attribute is not zero. "
	"A branch is then made from tubes 34 and 35 (or 85 and 86) to 87, "
	"and from tube 89 to 38 and 39.")
MAN_END

MAN_BEGIN (U"Create Artword...", U"ppgb", 20101212)
INTRO (U"A command to create an @Artword object with all muscle activities set to zero. "
	"See @@Articulatory synthesis@.")
ENTRY (U"Settings")
TAG (U"##Name")
DEFINITION (U"the name that you give to the created object. The standard name is $$hallo$, "
	"but you should give it a more sensible name, possibly something that represents the utterance "
	"that it is supposed to generate.")
TAG (U"##Duration (seconds)")
DEFINITION (U"the duration of the resulting Artword. Should be as long as the utterance that "
	"you want to generate with it. The standard value is 1 second.")
MAN_END

MAN_BEGIN (U"Create Speaker...", U"ppgb", 20101212)
INTRO (U"A command to create a @Speaker object. See @@Articulatory synthesis@.")
ENTRY (U"Settings")
TAG (U"##Name")
DEFINITION (U"the name that you give to the created object. The standard name is \"speaker\", "
	"but if you work with multiple Speaker objects, give them sensible names to reduce confusion.")
TAG (U"##Kind of speaker")
DEFINITION (U"Choose from #Female, #Male, or #Child. The only difference is a relative size.")
TAG (U"##Number of tubes in glottis")
DEFINITION (U"Choose from #1, #2, or #10. See @@Artword & Speaker: To Sound...@ for details.")
MAN_END

MAN_BEGIN (U"Create Vocal Tract from phone...", U"ppgb", 19960908)
INTRO (U"A way to create a @VocalTract object.")
ENTRY (U"Purpose")
NORMAL (U"to translate a phone symbol like [a], [u], etc., into a vocal-tract area function.")
ENTRY (U"Behaviour")
NORMAL (U"The resulting VocalTract will appear in the list of objects, "
	"with the same name as the phone.")
ENTRY (U"Algorithm")
NORMAL (U"The area function of the resulting VocalTract is taken from the Russian speaker from @@Fant (1960)@.")
MAN_END

MAN_BEGIN (U"Speaker", U"ppgb", 19980201)
INTRO (U"One of the @@types of objects@ in Praat. See @@Articulatory synthesis@.")
ENTRY (U"Speaker commands")
LIST_ITEM (U"• @@Create Speaker...")
LIST_ITEM (U"• @@Artword & Speaker: To Sound...@: articulatory synthesis")
MAN_END

MAN_BEGIN (U"VocalTract", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"A VocalTract object represents the area function of the vocal tract, expressed in m^2, "
	"running from the glottis to the lips.")
MAN_END

MAN_BEGIN (U"VocalTract: Formula...", U"ppgb", 20021206)
INTRO (U"A command for changing the data in all selected @VocalTract objects.")
NORMAL (U"See the @Formulas tutorial for examples and explanations.")
MAN_END

}

/* End of file manual_Artsynth.cpp */
