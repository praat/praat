/* manual_Artsynth.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2002/12/06
 */

#include "ManPagesM.h"

void manual_Artsynth_init (ManPages me);
void manual_Artsynth_init (ManPages me) {

MAN_BEGIN ("Articulatory synthesis", "ppgb", 20030316)
INTRO ("This is a description of the articulatory synthesis package in P\\s{RAAT}. "
	"For a detailed description of the physics and mathematics behind the model, "
	"see @@Boersma (1998)@, chapters 2 and 3. "
	"For examples of how to synthesize utterances, consult http://www.fon.hum.uva.nl/paul/diss/ch5/.")
ENTRY ("How to start (after reading the @Intro)")
NORMAL ("We are going to have the synthesizer say [apa]. We need a @Speaker and an @Artword object.")
NORMAL ("1. Create a speaker with @@Create Speaker...@ from the @@New menu@.")
NORMAL ("2. Create an articulation word of 0.5 seconds with @@Create Artword...@.")
NORMAL ("3. Edit the Artword by selecting it and clicking #Edit.")
NORMAL ("4. To set the glottis to a position suitable for phonation, use the ArtwordEditor to "
	"set the %Interarytenoid activity to 0.5 throughout the utterance. You set two targets: "
	"0.5 at a time of 0 seconds, and 0.5 at a time of 0.5 seconds.")
NORMAL ("5. To prevent air escaping from the nose, close the nasopharyngeal port "
	"by setting the %LevatorPalatini activity to 1.0 throughout the utterance.")
NORMAL ("6. To generate the lung pressure needed for phonation, you set the %Lungs activity at 0 seconds to 0.2, "
	"and at 0.1 seconds to 0.")
NORMAL ("7. To force a jaw movement that closes the lips, set the %Masseter activity at 0.25 seconds to 0.7, "
	"and the %OrbicularisOris activity at 0.25 seconds to 0.2.")
NORMAL ("8. Select the Speaker and the Artword and click #Movie; "
	"you will see a closing-and-opening gesture of the mouth.")
NORMAL ("9. Select the Speaker and the Artword and click ##To Sound...# "
	"(see @@Artword & Speaker: To Sound...@).")
NORMAL ("10. Just click %OK; the synthesis starts.")
NORMAL ("11. If you are sitting at a 1997 computer, this will last for 5 minutes or so. "
	"If this is too slow for you, click %Interrupt. "
	"Otherwise, you can watch the vibrating vocal cords "
	"and the changing vocal-tract shape.")
NORMAL ("12. You can play, view, and analyse the resulting @Sound as you would any other. "
	"You can see and hear a sound movie if you select the Speaker, "
	"the Artword, and the Sound, and click #Play.")
MAN_END

MAN_BEGIN ("Artword", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. See @@Articulatory synthesis@.")
NORMAL ("An object of class Artword object represents the activities of several speech muscles as functions of time.")
ENTRY ("Artword commands")
LIST_ITEM ("\\bu @@Create Artword...@: creates an Artword with relaxed muscles")
LIST_ITEM ("\\bu @@Artword & Speaker: To Sound...@: articulatory synthesis")
MAN_END

MAN_BEGIN ("Artword & Speaker: To Sound...", "ppgb", 20040331)
INTRO ("A command to synthesize a @Sound object from the selected @Speaker and the selected @Artword.")
NORMAL ("This is the command that performs the actual articulatory synthesis. "
	"See @@Articulatory synthesis@.")
ENTRY ("Arguments")
TAG ("%%Sampling frequency% (Hz)")
DEFINITION ("the number of times per second that the equilibrium widths and lengths and the tensions of the muscles "
	"are recomputed from the Artword. This will also be the sampling frequency of the resulting sound and "
	"of the optional resulting tube widths, air pressures, and air velocities. The standard value is 22050 Hz.")
TAG ("%Oversampling")
DEFINITION ("the number of times that the aerodynamic quantities and the state of the tube walls "
	"will be recomputed during each sample period. The standard value is 25.")
TAG ("%%Width 1%, %%Width 2%, %%Width 3")
DEFINITION ("the numbers (see below) of the tubes whose widths you want to monitor. "
	"E.g., if %%Width 1% is 36, the synthesizer will create a Sound object named \"width36\", "
	"which contains the width of tube 36 (the lower glottis) as a function of time, expressed in metres. "
	"To prevent the creation of a \"width\" object, specify \"0\" (the standard value).")
TAG ("%%Pressure 1%, %%Pressure 2%, %%Pressure 3")
DEFINITION ("the numbers (see below) of the tubes whose air pressures you want to monitor. "
	"E.g., if %%Pressure 3% is 37, the synthesizer will create a Sound object named \"pressure37\", "
	"which contains the air pressure of tube 37 (the upper glottis) as a function of time, expressed in Pascal. "
	"To prevent the creation of a \"pressure\" object, specify \"0\" (the standard value).")
TAG ("%%Velocity 1%, %%Velocity 2%, %%Velocity 3")
DEFINITION ("the numbers (see below) of the tubes whose air velocities you want to monitor. "
	"E.g., if %%Velocity 1% is 60, the synthesizer will create a Sound object named \"velocity60\", "
	"which contains the air velocity of tube 60 (in the mouth) as a function of time, expressed in metres per second. "
	"To prevent the creation of a \"velocity\" object, specify \"0\" (the standard value).")
ENTRY ("Stability")
NORMAL ("The internal sampling frequency for the aerodynamics is the specified %%sampling rate%, "
	"multiplied by the specified %oversampling. With the standard settings, this is 22050 times 25 = 550750 Hz.")
NORMAL ("To ensure the stability of the synthesis, this internal sampling frequency should not be less than the "
	"velocity of sound (353 m/s) divided by the length of the shortest tube. For the standard \"Female\", "
	"\"Male\", and \"Child\" speakers, the shortest tube is the upper glottis, which has a length "
	"of 0.7, 1.0, and 0.3 millimetres, respectively. The minimum internal sampling frequencies, therefore, "
	"are 504286, 353000, and 1176667 Hertz, respectively.")
ENTRY ("Time resolution")
NORMAL ("To capture the microscopic pressure changes in the glottis, you will want maximum time resolution. "
	"For a female speaker, you could set %%sampling frequency% to 550750 Hz, and %oversampling to 1.")
ENTRY ("Tube numbers")
NORMAL ("Here are the tube numbers that you can use for the %width, %pressure, and %velocity arguments:")
LIST_ITEM ("1..23: lungs (from bottom to top)")
LIST_ITEM ("24..29: bronchi (from bottom to top)")
LIST_ITEM ("30..35: trachea (from bottom to top)")
LIST_ITEM ("36: lower glottis")
LIST_ITEM ("37: upper glottis (not for a one-mass model)")
LIST_ITEM ("38..49: pharynx (from bottom to top)")
LIST_ITEM ("50..51: nasopharyngeal branching")
LIST_ITEM ("52..64: mouth (from back to front)")
LIST_ITEM ("65..78: nose (from back to front)")
LIST_ITEM ("79..86: conus elasticus (only for a 10-mass model)")
LIST_ITEM ("87..89: glottal shunt between the arytenoids (from bottom to top)")
NORMAL ("Some structural properties:")
LIST_ITEM ("\\bu Tube 1 is closed at the bottom.")
LIST_ITEM ("\\bu Tubes 64 (lips) and 78 (nostrils) radiate into the air.")
LIST_ITEM ("\\bu The nasopharyngeal branch is at tubes 50, 51, and 65. They are constrained to have equal lengths.")
LIST_ITEM ("\\bu For a one-mass model of the vocal cords, tube 36 is connected to 38.")
LIST_ITEM ("\\bu For a 10-mass model, tubes 32..35 are replaced with 79..86, so that "
	"tube 31 is connected to 79, and 86 is connected to 36.")
LIST_ITEM ("\\bu A glottal shunt will be implemented if the speaker's \"shunt.Dx\" attribute is not zero. "
	"A branch is then made from tubes 34 and 35 (or 85 and 86) to 87, "
	"and from tube 89 to 38 and 39.")
MAN_END

MAN_BEGIN ("Create Artword...", "ppgb", 20030916)
INTRO ("A command to create an @Artword object with all muscle activities set to zero. "
	"See @@Articulatory synthesis@.")
ENTRY ("Arguments")
TAG ("%Name")
DEFINITION ("the name that you give to the created object. The standard name is \"speaker\", "
	"but if you work with multiple Speaker objects, give them sensible names to reduce confusion.")
MAN_END

MAN_BEGIN ("Create Speaker...", "ppgb", 20030916)
INTRO ("A command to create a @Speaker object. See @@Articulatory synthesis@.")
ENTRY ("Arguments")
TAG ("%Name")
DEFINITION ("the name that you give to the created object. The standard name is \"artword\", "
	"but you should give it a more sensible name, possibly something that represents the utterance "
	"that it is supposed to generate.")
TAG ("%Duration (seconds)")
DEFINITION ("the duration of the resulting Artword. Should be as long as the utterance that "
	"you want to generate with it. The standard value is 1 second.")
MAN_END

MAN_BEGIN ("Create Vocal Tract from phone...", "ppgb", 19960908)
INTRO ("A way to create a @VocalTract object.")
ENTRY ("Purpose")
NORMAL ("to translate a phone symbol like [a], [u], etc., into a vocal-tract area function.")
ENTRY ("Behaviour")
NORMAL ("The resulting VocalTract will appear in the list of objects, "
	"with the same name as the phone.")
ENTRY ("Algorithm")
NORMAL ("The area function of the resulting VocalTract is taken from the Russian speaker from @@Fant (1960)@.")
MAN_END

MAN_BEGIN ("Fant (1960)", "ppgb", 19980201)
DEFINITION ("Gunnar Fant (1960): %%Acoustic Theory of Speech Production%. Mouton, The Hague.")
MAN_END

MAN_BEGIN ("Flanagan & Landgraf (1968)", "ppgb", 19980201)
NORMAL ("James L. Flanagan & L.L. Landgraf (1968): \"Self-oscillating source for vocal-tract synthesizers\", "
	"%%IEEE Transactions on Audio and Electroacoustics% ##AU-16#: 57-64. "
	"Reprinted in: James L. Flanagan & Lawrence R. Rabiner (eds.) (1973): "
	"%%Speech Synthesis%, Dowden, Hutchinson & Ross, Stroudsburg.")
NORMAL ("The authors show that if we model each vocal cord as a single mass-spring system, "
	"the vocal cords will vibrate passively as a result of the interaction with a glottal airflow.")
MAN_END

MAN_BEGIN ("Ishizaka & Flanagan (1972)", "ppgb", 19980201)
NORMAL ("Kenzo Ishizaka & James L. Flanagan (1972): \"Synthesis of voiced sounds from a two-mass model "
	"of the vocal cords\", %%Bell System Technical Journal% #51: 1233-1268. "
	"Reprinted in: James L. Flanagan & Lawrence R.ÊRabiner (eds.) (1973): "
	"%%Speech Synthesis%, Dowden, Hutchinson & Ross, Stroudsburg.")
NORMAL ("The authors show that if we model each vocal cord as two coupled mass-spring systems, "
	"the passive vibration that results from the interaction with a glottal airflow "
	"will show more realistic behaviour than with the one-mass model of @@Flanagan & Landgraf (1968)@, "
	"at least for a male speaker.")
MAN_END

MAN_BEGIN ("Speaker", "ppgb", 19980201)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. See @@Articulatory synthesis@.")
ENTRY ("Speaker commands")
LIST_ITEM ("\\bu @@Create Speaker...")
LIST_ITEM ("\\bu @@Artword & Speaker: To Sound...@: articulatory synthesis")
MAN_END

MAN_BEGIN ("VocalTract", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("A VocalTract object represents the area function of the vocal tract, expressed in m^2, "
	"running from the glottis to the lips.")
MAN_END

MAN_BEGIN ("VocalTract: Formula...", "ppgb", 20021206)
INTRO ("A command for changing the data in all selected @VocalTract objects.")
NORMAL ("See the @Formulas tutorial for examples and explanations.")
MAN_END

}

/* End of file manual_Artsynth.c */
