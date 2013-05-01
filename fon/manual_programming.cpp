/* manual_programming.cpp
 *
 * Copyright (C) 1992-2010 Paul Boersma
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

MAN_BEGIN (L"Programming with Praat", L"ppgb", 20130429)
INTRO (L"You can extend the functionality of the Praat program "
	"by adding modules written in C or C++ to it. All of Praat's source code "
	"is available under the General Public Licence.")
ENTRY (L"1. Warning")
NORMAL (L"Before trying the task of learning how to write Praat extensions in C or C++, "
	"you should be well aware of the possibilities of @scripting. "
	"Many built-in commands in Praat have started their "
	"lives as Praat scripts, and scripts are easier to write than extensions in C or C++. "
	"If you have a set of scripts, you can distribute them as a @@plug-ins|plug-in@.")
ENTRY (L"2. Getting the existing source code")
NORMAL (L"You obtain the Praat source code via ##www.praat.org#, in a file with a name like "
	"##praat5347_sources.zip# or ##praat5347_sources.tar.gz# (depending on the Praat version), and unpack this by double-clicking. "
	"The result will be a set of directories "
	"called #kar, #num, #external (with #GSL, #glpk, #FLAC, #mp3, #portaudio and #espeak in it), "
	"#sys, #dwsys, #stat, #fon, #dwtools, #LPC, #FFNet, #gram, #artsynth, #EEG, #contrib, #main, #makefiles, and #test, "
	"plus a makefile and an Xcode project for Macintosh.")
ENTRY (L"3. Building Praat on Macintosh")
NORMAL (L"Open ##praat.xcodeproj# in Xcode and choose #Build and #Run. For more details see the download page.")
ENTRY (L"4. Building Praat on Windows")
NORMAL (L"Praat for Windows is compiled with MinGW. See the download page for instructions.")
ENTRY (L"5. Building Praat on Linux")
NORMAL (L"To compile and link Praat on Linux, you go to the directory that "
	"contains the source directories and the makefile, and copy a ##makefile.defs# "
	"file from the #makefiles directory:")
CODE (L"> cp makefiles/makefile.defs.linux ./makefile.defs")
NORMAL (L"You have to have installed ##libgtk2.0-dev# (and its dependencies) and ##libasound2-dev#.")
NORMAL (L"On other Unixes, you do the same, but the file ##makefile.defs# may require some editing after this, "
	"because Silicon Graphics Irix, Sparc Solaris and HPUX may use different libraries or have them in different locations than Linux.")
ENTRY (L"6. Extending Praat")
NORMAL (L"You can edit ##main/main_Praat.cpp#. "
	"This example shows you how to create a very simple program with all the functionality "
	"of the Praat program, and a single bit more (namely an additional command in the New menu):")
CODE (L"\\# include \"praat.h\"")
CODE (L"")
CODE (L"DIRECT (HelloFromJane)")
CODE1 (L"Melder_information (L\"Hello, I am Jane.\");")
CODE (L"END")
CODE (L"")
CODE (L"int main (int argc, char **argv) {")
CODE1 (L"praat_init (\"Praat_Jane\", argc, argv);")
CODE1 (L"INCLUDE_LIBRARY (praat_uvafon_init)")
CODE1 (L"praat_addMenuCommand (L\"Objects\", L\"New\", L\"Hello from Jane...\", NULL, 0, DO_HelloFromJane);")
CODE1 (L"praat_run ();")
CODE1 (L"return 0;")
CODE (L"}")
ENTRY (L"7. Learning how to program")
NORMAL (L"To see how objects are defined, take a look at ##sys/Thing.h#, ##sys/Data.h#, "
	"##sys/oo.h#, the ##XXX_def.h# files in the #fon directory, and the corresponding "
	"##XXX.cpp# files in the #fon directory. To see how commands show up on the buttons "
	"in the fixed and dynamic menus, take a look at the large interface description file "
	"##fon/praat_Fon.cpp#.")
ENTRY (L"8. Using the Praat shell only")
NORMAL (L"For building the Praat shell (the Objects and Picture windows) only, you need only the code in the eight directories "
	"#kar, #GSL, #num, ##external/{FLAC,MP3,portaudio}#, #sys, and #dwsys. You delete the inclusion of praat_uvafon_init from #main. "
	"You will be able to build a Praat shell, i.e. an Objects and a Picture window, "
	"which has no knowledge of the world, i.e., which does not know any objects "
	"that can be included in the list of objects. You could use this Praat shell "
	"for modelling your own world and defining your own classes of objects. For advanced programmers only.")
MAN_END

}

/* End of file manual_programming.cpp */
