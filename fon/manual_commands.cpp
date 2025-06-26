/* manual_commands.cpp
 *
 * Copyright (C) 1992-2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

void manual_commands_init (ManPages me);
void manual_commands_init (ManPages me) {

MAN_PAGES_BEGIN R"~~~(
################################################################################
"fixed menu commands"   (into 2023 “Fixed menu commands“)
© Paul Boersma 1997,2005,2012,2023

The commands in the fixed menus of the @@Objects window@ (Praat, New, Open, Help, Goodies,
Settings, and Technical) and the @@Picture window@ (File, Edit, Margins, World, Select, Pen, Font, Help).

These commands are always clickable (if not hidden) and scriptable (if not added).

################################################################################
"action commands"   (into 2023 “Action commands”)
© Paul Boersma 1997,2002,2011,2023

The commands in the @@Dynamic menu@ of the @@Objects window@.

These commands are available only if the right kinds of objects are selected.
They are shown in a list in the right half of the Objects window,
or in the @@Save menu@ if they start with ##Save as# or ##Append to#.

################################################################################
"Add action command..."
© Paul Boersma 1997,2005,2006,2023

One of the hidden commands in the Praat menu of the @@Objects window@.
With this command, you add a button to the dynamic menu in the right half of the Objects window.

Settings
========
See @@Add to dynamic menu...@.

Usage
=====
You can use this command in your @@initialization script@ or in @@plug-ins@,
just like @@Add menu command...@.

Normally, however, if you want to add a command to the dynamic menu,
you would use the command @@Add to dynamic menu...@ of the @ScriptEditor instead,
which has the exact same list of parameters.

################################################################################
"Add menu command..."
© Paul Boersma 1997,2005,2006,2023

One of the hidden commands in the Praat menu of the @@Objects window@.
With this command, you add a button to any of the fixed menus in the Objects window,
the Picture window, or any type of editor window.

Settings
========
See @@Add to fixed menu...@ (for the Objects window or the Picture window)
or @@Add to menu...@ (for editor windows).

Usage
=====
You can use this command in your @@initialization script@ or in @@plug-ins@,
just like @@Add action command...@.

Normally, however, if you want to add a command to a fixed menu,
you would use the command @@Add to fixed menu...@ of the @ScriptEditor instead,
which has the exact same list of parameters.

################################################################################
"Add to dynamic menu..."
© Paul Boersma 1998,2006,2011,2014,2023

A command in the File menu of the @ScriptEditor.

With this command, you add a button to the dynamic menu in the @@Objects window@.
This button will only be visible if the specified combination of objects is selected.
Clicking the button will invoke the specified @@Praat script@.

Settings
========
##Class 1
: the name of the class of the object to be selected.
  For instance, if a button should only appear if the user selects a Sound, this would be “Sound”.

##Number 1
: the number of objects of %class1 that have to be selected. For most built-in commands, this number is unspecified (0);
  e.g., the user can choose #Draw... or ##To Spectrum# regardless of whether she selected 1, 2, 3, or more Sound objects.
  If the number of selected objects is different from %number1, the button will be visible but insensitive.

##Class 2
: the name of the class of the second object to be selected, different from %class1.
  Normally the empty string (“”).

##Number 2
: the number of selected objects of %class2.

##Class 3
: the name of the class of the third object to be selected, different from %class1 and %class2.
  Normally the empty string (“”).

##Number 3
: the number of selected objects of %class3.

##Command
: the title of the new command button (or label, or submenu title).
  To get a separator line instead of a command text (only in a submenu),
  you specify a unique string that starts with a hyphen (“-”); the @ButtonEditor may contain some examples of this.
  If the command starts with ##Save as# or ##Append to#, it will be placed in the @@Save menu@.

##After command
: a button title in the dynamic menu or submenu where you want your new button.
  If you specify the empty string (“”), your button will be put at the bottom.
  You can specify a push button, a label (subheader), or a cascade button (submenu title) here.

##Depth
: 0 if you want your button in the main menu, 1 if you want it in a submenu.

##Script
: the full path name of the script to invoke. If you saved the script you are editing,
  its name will already have been filled in here.
  If you do not specify a script, you will get a separating label or cascading menu title instead,
  depending on the %depth of the following command.

Example
=======
If one object of class Sound is selected, you want a new submenu called “Filters”
after the #Convolve button, containing your new commands “Autocorrelation” and “Band filter...”,
separated by a horizontal separator line:
{;
	Add to dynamic menu: "Sound", 0, "", 0, "", 0, "Filters -",
	... "Convolve", 0, ""
	Add to dynamic menu: "Sound", 1, "", 0, "", 0, "Autocorrelation",
	... "Filters -", 1, "/u/praats/demo/autocorrelation.praat"
	Add to dynamic menu: "Sound", 0, "", 0, "", 0, "-- band filter --",
	... "Autocorrelation", 1, ""
	Add to dynamic menu: "Sound", 1, "", 0, "", 0, "Band filter...",
	... "-- band filter --", 1, "/u/praats/demo/bandFilter.praat"
}
Note that “Filters -” will be a submenu title, %because it is followed by subcommands (%depth 1).
Note that %number1 is 1 only for the executable buttons; for the cascade button and the separator line,
this number is ignored.

Usage convention
================
Please adhere to the convention that command that take arguments, such as “Band filter...” above, end in three dots.

Using this command in a script
==============================
To add a dynamic button from a script (perhaps your @@initialization script@ or a @@plug-ins|plug-in@),
use the hidden shell command @@Add action command...@ instead.

################################################################################
"Add to fixed menu..."
© Paul Boersma 1997,2002,2005,2006,2012,2014,2023

A command in the File menu of the @ScriptEditor.

With this command, you add a button to any fixed menu in the @@Objects window@ or in the @@Picture window@.
Clicking the added button will invoke the specified @@Praat script@.

Settings
========
#Window
: the name of the window (“Objects” or “Picture”) that contains the menu that you want to change.

#Menu
: the title of the menu that you want to change. If #Window is “Objects”, you can specify
  the Praat, New, Open, Help, Goodies, Settings, or Technical menu (for the Save menu,
  which depends on the objects selected,
  you would use @@Add to dynamic menu...@ instead). If #Window is “Picture”, you can specify
  the File, Edit, Margins, World, Select, Pen, Font, or Help menu.

#Command
: the title of the new menu button. To get a separator line instead of a command text,
  you specify a unique string that starts with a hyphen (“-”); the @ButtonEditor contains many examples of this.

##After command
: a button title in the menu or submenu after which you want your new button to be inserted.
  If you specify the empty string (“”), your button will be put in the main menu.

#Depth
: 0 if you want your button in the main menu, 1 if you want it in a submenu.

#Script
: the full path name of the script to invoke. If you saved the script you are editing,
  its name will already have been filled in here. If you do not specify a script,
  you will get a cascading menu title instead.

Example 1
=========
In the Matrix submenu of the @@New menu@, you want a separator line followed by the command #Peaks:
{;
	Add to fixed menu: "Objects", "New", "-- peaks --", "Create simple Matrix...", 1, ""
	Add to fixed menu: "Objects", "New", "Peaks", "-- peaks --", 1, "/u/praats/demo/peaks.praat"
}
Example 2
=========
In the @@New menu@, you want a submenu called “Demo”, with a subitem titled “Lorenz...”:
{;
	Add to fixed menu: "Objects", "New", "Demo", "", 0, ""
	Add to fixed menu: "Objects", "New", "Lorenz...", "Demo", 1, "/u/praats/demo/lorenz.praat"
}
Usage convention
================
Please adhere to the convention that commands that take arguments, such as “Lorenz...” above, end in three dots.

Using this command in a script
==============================
To add a fixed button from a script (perhaps your @@initialization script@ or a @@plug-ins|plug-in@),
use the hidden shell command @@Add menu command...@ instead. It has the exact same list of parameters.

################################################################################
"Add to menu..."
© Paul Boersma 2023

A command in the File menu of a @ScriptEditor that was created in an editor (e.g. SoundEditor) window.

With this command, you add a button to any fixed menu in an editor window.
Clicking the added button will invoke the specified @@Praat script@.

Settings
========
#Window
: the name of the type of window (e.g. “TextGridEditor”) that contains the menu that you want to change.

#Menu
: the title of the menu that you want to change. If #Window is “SoundEditor”, you can specify
  the File, Edit, Time, Play, Sound, Analyses, Spectrogram, Pitch, Intensity, Formants, or Pulses menu.

#Command
: the title of the new menu button. To get a separator line instead of a command text,
  you specify a unique string that starts with a hyphen (“-”).
  To get a command with a rectangle to its right (like e.g. ##Save sound to disk:# in the File menu
  of the SoundEditor), have the command end in a colon (“:”).
  To get a command with a rectangle to its right and a separator line above it,
  you can use e.g. `- Save sound to disk:` as a shorthand.

##After command
: not used; always keep this empty (i.e. an empty string).

#Depth
: 0 if you want your button left-aligned in the menu, 1 if you want it indented
  (as e.g. the command ##Save selected sound as WAV file...# in the File menu of the SoundEditor).

#Script
: the full path name of the script to invoke. If you saved the script you are editing,
  its name will already have been filled in here. If you do not specify a script,
  you will get a separator line instead.

Usage convention
================
Please adhere to the convention that commands that take arguments, such as “Lorenz...” above, end in three dots.

Using this command in a script
==============================
To add a fixed button from a script (perhaps your @@initialization script@ or a @@plug-ins|plug-in@),
use the hidden shell command @@Add menu command...@ instead. It has the exact same list of parameters.

################################################################################
"ButtonEditor"
© Paul Boersma 1998,2003,2005,2006,2023

An editor for viewing, hiding, showing, removing, and executing the commands
in the fixed menus, editor menus and dynamic menus of the Praat program.
To open the button editor, choose ##Buttons...#
from the Settings menu in the Praat menu of the @@Objects window@.

What the button editor shows
============================
The button editor gives a list of:
1. The five fixed buttons.
2. The built-in and added @@fixed menu commands@, lexicographically sorted by window and menu name.
3. The added editor menu commands, lexicographically sorted by editor and menu name.
4. The built-in and added @@action commands@, sorted by the names of the selected objects.

Visibility of built-in commands
===============================
Most built-in commands are visible by default, but some are hidden by default (see @@hidden commands@).
The button editor shows these commands as “shown” or “hidden”, respectively.
You can change the visibility of a command by clicking on the blue “shown” or “hidden” text;
this text will then be replaced with “HIDDEN” or “SHOWN”, with capitals to signal their non-standard settings.
These changes will be remembered in the @@buttons file@ across sessions of your program.
To return to the standard settings, click the blue “HIDDEN” or “SHOWN” texts again.

Some built-in commands cannot be hidden. They are marked as “unhidable”.
The most notable example is the ##Buttons...# button
(a failure to make the ##Commands...# command unhidable in Microsoft Word
causes some computer viruses to be very hard to remove...).

Added commands
==============
Commands that you have added to the fixed or dynamic menus (probably with @@Add to fixed menu...@ or
@@Add to dynamic menu...@ in the @ScriptEditor),
are marked as “ADDED”. They are remembered in the @@buttons file@.
You can change the availability of these commands by clicking on the blue “ADDED” text, which will then
be replaced with “REMOVED”. After this, the added command will no longer be remembered in the @@buttons file@.
To make the command available again, click the blue “REMOVED” text again, before leaving the program.

Start-up commands
=================
Commands that were added in an @@initialization script@ or @@plug-ins|plug-in@ (with @@Add menu command...@ or
@@Add action command...@) are marked as “START-UP”.
They are %not remembered in the @@buttons file@.
You can change the visibility of these commands by clicking on the blue “START-UP” text, which will then
be replaced with “HIDDEN”. This setting %will be remembered in the @@buttons file@.
To make the command visible again, click the blue “HIDDEN” text again.

Executing commands
==================
The button editor allows you to choose hidden commands without first making them visible in the fixed or dynamic menus.

The editor shows all the executable commands in blue. These include:
1. The fixed #Remove button, if one or more objects are selected in the @@List of Objects@.
2. The other fixed buttons, if exactly one object is selected.
3. All of the fixed menu commands, hidden or not, and “removed” or not.
4. Those action commands that match the currently selected objects with respect to class and number.

To execute any of these blue commands, just click on it.

################################################################################
"buttons file"   (into 2002 “Buttons file”)
© Paul Boersma 1997,2002,2007,2015,2020

The file into which changes in the availability and visibility of commands in the fixed
and dynamic menus are recorded.

The buttons file is written to disk when you quit Praat,
and it is read again when you start Praat the next time. It is a simple @@Praat script@ that you can read
(but should not edit) with any text editor.

Adding buttons
==============
To add a command to a fixed or dynamic menu, you typically use the @ScriptEditor.

Removing buttons
================
To remove an added command from a fixed or dynamic menu, you typically use the @ButtonEditor.

Hiding and showing buttons
================
To hide a built-in command from a fixed or dynamic menu, or to make a hidden command visible,
you typically use the @ButtonEditor.

Where is the buttons file?
==========================
The buttons file is in your Praat @@preferences folder@.

On Windows the file is called #`Buttons5.ini`,
for instance `C:\Users\Miep\Praat\Buttons5.ini`.

On MacOS it is called #`Buttons5`,
for instance `/Users/miep/Library/Preferences/Praat Prefs/Buttons5`.

On Linux it is called #`buttons5`,
for instance `/home/miep/.praat-dir/buttons5`.

################################################################################
"preferences folder"
© Paul Boersma 2007,2010,2015(“preferences directory”),2020

The Praat preferences folder is the folder where Praat saves the @@preferences file@ and the @@buttons file@,
and where you can install @@plug-ins@ and save the preferences of your scripts (in your subfolder of the #apps subfolder).
If the preferences folder does not exist, it will automatically be created when you start Praat.

Windows
=======
If you are user #Miep, your Praat preferences folder will be `C:\Users\Miep\Praat` in Praat 6.xx,
and `C:\Users\Miep\AppData\Roaming\Praat` in Praat 7.xx.

To see what’s in there, open a File Explorer window, switch on `View` \-> `Show` \-> `Hidden items`,
and navigate to `Local Disk` \-> `Users` \-> %`your name` \-> `AppData` \-> `Roaming` \-> `Praat`.

Macintosh
=========
If you are user #miep, your Praat preferences folder will be `/Users/miep/Library/Preferences/Praat Prefs` in Praat 6.xx,
and `/Users/miep/Library/Application Support/Praat` in Praat 7.xx.

To see what’s in there, go to the Finder, choose Go \-> Library, and navigate to `Application Support` \-> `Praat`.

Linux
=====
If your home folder is #`/home/miep/`, your Praat preferences folder will be `/home/miep/.praat-dir` in Praat 6.xx,
and `/home/miep/.config/praat` in Praat 7.xx.

To see what’s in there, open a Terminal window and type `ls -al ~/.config/praat`.

(If the environment variable `XDG_CONFIG_HOME` is set, then the `praat` folder will be there instead.)

If your app isn’t called Praat, but e.g. PraatForHospitals (Praat’s extra safe edition available from version 7 on),
then the preferences folder isn’t called `Praat` or `praat`, but `PraatForHospitals` or `praat_for_hospitals` instead.

################################################################################
"preferences file"   (into 2003 “Preferences file”)
© Paul Boersma 1997,2002,2003,2007,2020,2023

The file into which some of your preferences are saved across your sessions with Praat.
For instance, if you change the font used by the Picture window to Palatino and quit Praat,
the Picture-window font will still be Palatino when you start Praat again.

The preferences file is written to disk when you quit Praat,
and it is read when you start Praat. It is a simple text file that you can read
(but should not edit) with any text editor.

Where is the preferences file?
==============================
The preferences file is in your Praat @@preferences folder@.

On Windows it is called #`Preferences5.ini`,
for instance `C:\Users\Miep\Praat\Preferences5.ini`.

On Macintosh it is called #`Prefs5`,
for instance `/Users/miep/Library/Preferences/Praat Prefs/Prefs5`.

On Linux the file is called #`prefs5`,
for instance `/home/miep/.praat-dir/prefs5`.

################################################################################
"initialization script"   (through 2004 “Initialization script”)
© Paul Boersma 1997,2004,2006,2014,2015,2020,2023

Your initialization script is a normal @@Praat script@ that is run as soon as you start Praat.

On Unix or macOS, you create an initialization script by creating a file named `praat-startUp`
in the folder `/usr/local`,
or putting a file `.praat-user-startUp` or `praat-user-startUp` in your home folder
(if you rename the Praat executable, these names have to change as well).

If you have more than one of these files, they are run in the above order.

On Windows, you create an initialization script by creating a file named
`praat-user-startUp` in your home folder,
which could be `C:\Users\Miep` if you are Miep.

If you have both of these files, they are run in the above order.

Example
=======
If you like to be greeted by your husband when Praat starts up,
you could put the following lines in your initialization script:
{;
	if windows
		Read from file: "C:\Users\Miep\helloMiep.wav"
	elsif macintosh
		Read from file: "/Users/miep/helloMiep.wav"
	else
		Read from file: "/home/miep/helloMiep.wav"
	endif
	Play
	Remove
}
What not to use an initialization script for
============================================
You could set preferences like the default font in your initialization script,
but these will be automatically remembered between invocations of Praat anyway (in your @@preferences file@),
so this would often be superfluous, or indeed confusing.

For installing sets of menu commands at start-up you will probably prefer to use @@plug-ins@
rather than a single start-up file.

################################################################################
"plug-ins"   (into 2006 “plugins”)
© Paul Boersma 2006,2007,2014,2015,2020,2023

Experienced Praat script writers can distribute their product as a plug-in to Praat.

The Praat plug-in mechanism
===========================
When Praat starts up, it will execute all Praat scripts called #`setup.praat`
that reside in folders whose name starts with #`plugin_` and that themselves reside in
your Praat @@preferences folder@.

How to write a Praat plug-in
============================
Suppose that you have a set of Praat scripts specialized in the analysis and synthesis of vocalizations of guinea pigs,
and that these scripts are called `analyseQueak.praat` and `createQueak.praat` (“queak” is what guinea pigs tend to say).
With the @ScriptEditor, you have put the script `analyseQueak.praat` in the dynamic menu that
appears if the user selects a Sound object, and you have put the script `createQueak.praat` in the @@New menu@.
Only the latter script requires the user to supply some settings in a form, so the two menu commands
are ##Analyse queak# (without dots) and ##Create queak...# (with three dots), respectively.
Suppose now that you want to distribute those two commands to other guinea pig vocalization researchers.

What you do is that you create a Praat script called `setup.praat` (in the same folder as the two other scripts),
that contains the following two lines:
{;
	\@{Add action command:} "Sound", 1, "", 0, "", 0, "Analyse queak", "", 0, "analyseQueak.praat"
	\@{Add menu command:} "Objects", "New", "Create queak...", "", 0, "createQueak.praat"
}
(If you ran this script, Praat would install those two commands in the correct menus, and remember them in the @@buttons file@;
but you are now going to install them in a different way.)

You now put the three scripts in a new folder called %`plugin_Queak`,
and put this folder in your Praat preferences folder. If you are on Windows,
you will now have a folder called something like `C:\Users\Your Name\Praat\plugin_Queak`.

If you now start up Praat, Praat will automatically execute the script
`C:\Users\Your Name\Praat\plugin_Queak\setup.praat`
and thereby install the two buttons. The two buttons will %not be remembered in the buttons file,
but they will be installed at every Praat start-up. De-installation involves removing (or renaming) the `plugin_Queak` folder.

To distribute the Queak plug-in among your colleague guinea pig researchers, you can use any installer program to put
the `plugin_Queak` folder into the user’s Praat preferences folder; or you could ask those colleagues to move
the `plugin_Queak` folder there by hand.

The structure of your plug-in folder
====================================
In the example `setup.praat` file above, the names of the scripts `analyseQueak.praat` and `createQueak.praat`
occur without any folder information. This works because Praat regards these file names as being relative to the folder
where `setup.praat` is located. If your plug-in is much larger than two scripts, you may want to put subdirectories into
the folder `plugin_Queak`. For instance, if you put `analyseQueak.praat` into the subfolder `analysis`,
your line in the `setup.praat` script would look as follows:
{;
	\@{Add action command:} "Sound", 1, "", 0, "", 0, "Analyse queak", "", 0, "analysis/analyseQueak.praat"
}
The forward slash (“/”) in this example makes your plug-in platform-independent: it will work unchanged
on Windows, Macintosh, and Unix.

Nothing prevents you from adding data files to your plug-in. For instance, your `plugin_Queak` folder
could contain a subfolder `sounds` full of guinea pig recordings, and you could make them available in the New or Open menu.

Using a plug-in for site-wide customization
===========================================
If your local guinea pig research group shares a number of Praat scripts, these can be made available to everybody
in the following way:
1. Create a script that adds buttons to the fixed and dynamic menus, using the commands
	@@Add menu command...@ and @@Add action command...@. This script could be a slightly edited copy of someone’s
	@@buttons file@.
2. Put this script where everybody can see it,
	for instance in `U:\MaldenGuineaPigResearchButtons.praat`, where `U` is your shared computer.
3. Create a file `setup.praat` that contains only the following line:
`
		runScript: "U:\MaldenGuineaPigResearchButtons.praat"
`
4. Put the `setup.praat` file in a new folder called `plugin_MaldenGuineaPigResearch`,
	and distribute this folder among your local colleagues.

This procedure allows all members of the group to automatically enjoy all the later changes in your
custom command set.

################################################################################
"plugins"
© Paul Boersma 2006

See @@plug-ins@.

################################################################################
"hidden commands"   (into 2023 “Hidden commands”)
© Paul Boersma 1997,2003,2005,2006,2011

Some commands in Praat's fixed and dynamic menus are hidden by default.
You can still call hidden commands from scripts, run them by clicking on them in a @ButtonEditor,
or make them visible with the help of the @ButtonEditor.

To hide commands that are visible by default, use the @ButtonEditor.

What commands are hidden by default?
====================================
Commands that are expected to be of very limited use, are hidden by default. Examples are:
1. The commands @@Add menu command...@, ##Hide menu command...#, ##Show menu command...#,
  @@Add action command...@, ##Hide action command...#, and ##Show action command...# in the #Praat menu
  of the @@Objects window@. These are used in the @@buttons file@ and could be used by an @@initialization script@ or a @@plug-ins|plug-in@
  as well; in an interactive session, however, the functionality of these commands is part of the
  @ScriptEditor and the @ButtonEditor.
2. The command ##Read from old Praat picture file...# in the #File menu of the @@Picture window@.
  For reading a file format that was in use before May, 1995.
3. In the Praat program, the action ##Sound: Save as Sesam file...#. Writes a file format in common use
  in the Netherlands on Vax machines. In the Dutch phonetics departments, the plugs were pulled from the Vaxes in 1994.
4. In the Praat program, the action ##Sound: To Cochleagram (edb)...#. Needed by one person in 1994.
  An interesting, but undocumented procedure (De Boer's gammatone filter bank plus Meddis & Hewitt's
  synapse model), which does not create a normally interpretable Cochleagram object.

################################################################################
)~~~" MAN_PAGES_END

}

/* End of file manual_commands.cpp */
