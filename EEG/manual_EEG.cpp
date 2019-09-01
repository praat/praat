/* manual_EEG.cpp
 *
 * Copyright (C) 2012,2015 Paul Boersma
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

void manual_EEG_init (ManPages me);
void manual_EEG_init (ManPages me) {

MAN_BEGIN (U"EEG", U"ppgb", 20180329)
INTRO (U"EEG means electro-encephalography: brain potentials recorded via e.g. 32 or 64 electrodes on the scalp. "
	"In Praat, an EEG object looks like a combination of a Sound object with e.g. 32 or 64 channels "
	"and a TextGrid object that marks the events.")
ENTRY (U"1. How to get an EEG object in Praat")
NORMAL (U"You typically create an EEG object in Praat by opening a BDF/EDF file with @@Read from file...@. "
	"Praat tries to read the whole file into memory, so you may want to work with a 64-bit edition of Praat "
	"if you want to avoid \"out of memory\" messages.")
NORMAL (U"After you do ##Read from file...#, an EEG object will appear in the list of objects.")
ENTRY (U"2. How to look into an EEG object")
NORMAL (U"Once you have an EEG object in the list, you can click ##View & Edit# to look into it. "
	"You will typically see the first 8 channels, but you scroll to the other channels by clicking on the up and down arrows. "
	"You can scroll and zoom in the same way as in a Sound window.")
NORMAL (U"The channel names that you see are often A1, A2, ... A32, B1, B2, ... B32, C1, C2, ... C32, and so on. These represent the cap electrodes. "
	"If the number of cap electrodes is 32, though, the channel names are Fp1, AF3, ... Cz, "
	"and if it is 64, the channel names are Fp1, AF7, ... O2. You can change these names with "
	"##Set channel name...# from the #Modify menu.")
NORMAL (U"Below the cap electrodes you may see a number of channels for the external electrodes. "
	"These are typically named EXG1, EXG2, ... EXG8, but you can change these names with ##Edit external electrode names...# "
	"from the #Modify menu.")
NORMAL (U"Below the external electrodes you may see a number of special channels, "
	"perhaps named GSR1, GSR2, Erg1, Erg2, Resp, Plet, Temp, and Status. "
	"These represent recordings from other sensors than the electrodes.")
NORMAL (U"The Status channel is special. It is the 8-bit digital signal that you fed to your EEG apparatus. "
	"When reading the BDF file, Praat extracts all 8 bits from this status signal, "
	"and these 8 bits are shown in the EEG window as TextGrid interval tiers named S1, S2, ... S8. "
	"These bits are the place to mark events. For instance, in our lab we use S8 to mark any event, "
	"S5 to mark a \"standard\" event, and S4 to mark a \"deviant\" event.")
ENTRY (U"3. What to do to an EEG object")
NORMAL (U"The raw EEG signal that you have read from a file is typically quite noisy and may have a very large "
	"DC (direct-current) component as compared to the small vibrations that you are interested in. "
	"There are several ways to clean it up.")
NORMAL (U"##Subtracting a reference signal.# The voltage on the whole scalp may rise and fall in time. "
	"You are unlikely to be interested in these global movements; instead, you are likely to be interested only "
	"in the voltage movements on the brain-influenced part of the scalp (the cap electrodes) "
	"with respect to the voltage movements on the non-brain-influenced part of the scalp (the external electrodes). "
	"If you have external electrode recordings that include the left and right mastoids, you can use these channels as a %%reference signal%, i.e., "
	"you can use ##Subtract reference...# to subtract the average of the two mastoid channels from all electrode channels (including the external electrodes themselves). "
	"If you have no such recordings, you can still regard the average of a range of electrode channels as a reference signal, "
	"and use ##Subtract mean channel...# to subtract this from each electrode channel.")
NORMAL (U"##Detrending.# With #Detrend, you subtract from each electrode channel a line in such a way that the first sample and the last sample become zero. "
	"Detrending and reference subtraction can be performed in either order.")
NORMAL (U"##Filtering.# With ##Filter...#, you band-pass filter each electrode channel. Filtering should be done after detrending, but "
	"filtering and reference subtraction can be performed in either order.")
ENTRY (U"4. How to do an ERP analysis")
NORMAL (U"An ERP is an Event-Related Potential. Events are marked somewhere in S1, S2, ... S8. In the above example, "
	"we extract all the \"deviant\" events by doing ##To ERPTier...#, setting ##From time# to -0.11 seconds, "
	"##To Time# to 0.39 seconds, and ##Marker bit# to 4. This way, an ERPTier object appears in the list, containing "
	"the parts of the EEG signal that lie in a time window of [-0.11, 0.39] seconds around each event on the S4 (deviant) tier. "
	"Thus, if S4 contains 150 events, the resulting ERPTier will contain 150 pieces of the original EEG signal (without the extra sensors).")
NORMAL (U"You are typically interested in the part of each ERP in the ERPTier after the event. "
	"That is, you are probably interested in how this part compares to the part not influenced by the event, "
	"i.e. the part before the event. To make this comparison easier, you can do ##Subtract baseline...#: "
	"the waveform of the ERP will be moved up or down in such a way that the average amplitude of the part between the specified times becomes zero; "
	"the times you will typically fill in here are the starting time of the ERP (a negative number) and the time of the event (0.0).")
NORMAL (U"After you subtract the baseline, the part after the event contains the interesting part of the ERP: "
	"even its sign (positive or negative) is meaningful now. Some ERPs, however, will contain very large positive or negative peaks "
	"because of eye movements and other disturbing signals. To remove those ERPs from the ERPTier, "
	"do ##Reject artefacts...#, while specifying the amount by which you allow the amplitude of an ERP to go above or below zero.")
NORMAL (U"Once you have an ERPTier, you can extract each of the 150 ERPs from it with ##Extract ERP...#. "
	"It is perhaps more interesting to compute the average of all those 150 ERPs with ##To ERP (mean)#. "
	"These commands put a new ERP object in the list.")
NORMAL (U"Once you have an ERP object, you can look into it with ##View & Edit#. "
	"If you want to see in the ERP window the scalp distribution at the time of the cursor, or the average scalp distribution in the selected time stretch, "
	"you have to switch on ##Show selection viewer# in the #Preferences window (available from the #File menu).")
ENTRY (U"See also")
LIST_ITEM (U"\\bu @@Independent Component Analysis on EEG@")
MAN_END

MAN_BEGIN (U"Independent Component Analysis on EEG", U"ppgb", 20180502)
INTRO (U"Independent Component Analysis (ICA) is often used to improve @EEG signals. "
	"See @@blind source separation@ for the algorithm.")
ENTRY (U"1. Selecting your channels")
NORMAL (U"You typically will not want to do ICA on all 80 channels of your signal, "
	"some of which may not even be EEG channels. To create a smaller EEG with e.g. only the 64 scalp channels, "
	"use ##EEG: Extract channels...#. From here on we will assume that your reduced EEG has 64 channels.")
ENTRY (U"2. How to get a MixingMatrix")
NORMAL (U"Once you have your reduced EEG, you can start to do ICA on it. "
	"This starts by creating a @MixingMatrix: select your EEG object and choose ##To MixingMatrix...#. "
	"The resulting MixingMatrix has one row for each of your 64 EEG channels, and columns called \"ic1\" through \"ic64\".")
ENTRY (U"3. How to see the independent components")
NORMAL (U"Select your EEG and your MixingMatrix together and choose ##Unmix#. "
	"The resulting ICA-EEG will have 64 channels called \"ic1\" through \"ic64\".")
ENTRY (U"4. Just checking back")
NORMAL (U"If you select your ICA-EEG together with your MixingMatrix and choose ##Mix#, "
	"the resulting EEG should be very similar to your original 64-channel EEG.")
MAN_END

}

/* End of file manual_EEG.cpp */
