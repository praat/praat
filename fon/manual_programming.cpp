/* manual_programming.cpp
 *
 * Copyright (C) 1992-2010,2015 Paul Boersma
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

void manual_programming_init (ManPages me);
void manual_programming_init (ManPages me) {

MAN_BEGIN (U"Programming with Praat", U"ppgb", 20151028)
INTRO (U"You can extend the functionality of the Praat program "
	"by adding modules written in C or C++ to it. All of Praat's source code "
	"is available under the General Public Licence.")
ENTRY (U"1. Warning")
NORMAL (U"Before trying the task of learning how to write Praat extensions in C or C++, "
	"you should be well aware of the possibilities of @scripting. "
	"Many built-in commands in Praat have started their "
	"lives as Praat scripts, and scripts are easier to write than extensions in C or C++. "
	"If you have a set of scripts, you can distribute them as a @@plug-ins|plug-in@.")
ENTRY (U"2. Getting the existing source code")
NORMAL (U"You obtain the Praat source code from GitHub (https://github.com/praat), in a file with a name like "
	"##praat5423_sources.zip# or ##praat5423_sources.tar.gz# (depending on the Praat version), and unpack this by double-clicking. "
	"The result will be a set of directories "
	"called #kar, #num, #external (with #GSL, #glpk, #FLAC, #mp3, #portaudio and #espeak in it), "
	"#sys, #dwsys, #stat, #fon, #dwtools, #LPC, #FFNet, #gram, #artsynth, #EEG, #contrib, #main, #makefiles, #test, and #dwtest, "
	"plus a makefile and an Xcode project for Macintosh.")
ENTRY (U"3. Building Praat")
NORMAL (U"Consult the README file on GitHub for directions to compile and link Praat for your platform.")
ENTRY (U"4. Extending Praat")
NORMAL (U"To start extending Praat’s functionality, you can edit ##main/main_Praat.cpp#. "
	"This example shows you how to create a very simple program with all the functionality "
	"of the Praat program, and a single bit more (namely an additional command in the New menu):")
CODE (U"\\# include \"praat.h\"")
CODE (U"")
CODE (U"DIRECT (HelloFromJane)")
CODE1 (U"Melder_information (U\"Hello, I am Jane.\");")
CODE (U"END")
CODE (U"")
CODE (U"int main (int argc, char **argv) {")
CODE1 (U"praat_init (U\"Praat_Jane\", argc, argv);")
CODE1 (U"INCLUDE_LIBRARY (praat_uvafon_init)")
CODE1 (U"praat_addMenuCommand (U\"Objects\", U\"New\", U\"Hello from Jane...\", NULL, 0, DO_HelloFromJane);")
CODE1 (U"praat_run ();")
CODE1 (U"return 0;")
CODE (U"}")
ENTRY (U"5. Learning how to program")
NORMAL (U"To see how objects are defined, take a look at ##sys/Thing.h#, ##sys/Daata.h#, "
	"##sys/oo.h#, the ##XXX_def.h# files in the #fon directory, and the corresponding "
	"##XXX.cpp# files in the #fon directory. To see how commands show up on the buttons "
	"in the fixed and dynamic menus, take a look at the large interface description file "
	"##fon/praat_Fon.cpp#.")
ENTRY (U"6. Using the Praat shell only")
NORMAL (U"For building the Praat shell (the Objects and Picture windows) only, you need only the code in the eight directories "
	"#kar, #GSL, #num, ##external/{FLAC,MP3,portaudio}#, #sys, and #dwsys. You delete the inclusion of praat_uvafon_init from #main. "
	"You will be able to build a Praat shell, i.e. an Objects and a Picture window, "
	"which has no knowledge of the world, i.e., which does not know any objects "
	"that can be included in the list of objects. You could use this Praat shell "
	"for modelling your own world and defining your own classes of objects. For advanced programmers only.")
MAN_END

}

/* End of file manual_programming.cpp */
