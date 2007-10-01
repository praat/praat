/* manual_programming.c
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

void manual_programming_init (ManPages me);
void manual_programming_init (ManPages me) {

MAN_BEGIN (L"Programming with Praat", L"ppgb", 20070930)
INTRO (L"You can extend the functionality of the P\\s{RAAT} program "
	"by adding modules written in C to it. All of P\\s{RAAT}'s source code "
	"is available under the General Public Licence.")
ENTRY (L"1. Warning")
NORMAL (L"Before trying the task of learning how to write P\\s{RAAT} extensions in C, "
	"you should be well aware of the possibilities of @scripting. "
	"Many built-in commands in P\\s{RAAT} have started their "
	"lives as P\\s{RAAT} scripts, and scripts are easier to write than extensions in C.")
ENTRY (L"2. Getting the existing source code")
NORMAL (L"You obtain the P\\s{RAAT} source code via ##www.praat.org#, in a file with a name like "
	"##praat4627_sources.tar.gz# (depending on the P\\s{RAAT} version), and unpack this with #gunzip and ##tar xvf# "
	"(on Unix), or ##StuffIt^\\re Expander^\\tm# (on Macintosh), "
	"or ##Aladdin^\\re Expander^\\tm# (on Windows). The result will be a set of directories "
	"called #kar, #GSL, #FLAC, #sys, #dwsys, #fon, #dwtools, #LPC, #FFNet, #artsynth, #main, and #makefiles, "
	"plus a makefile, a Codewarrior project for Windows, and an Xcode project for Macintosh.")
ENTRY (L"3. Building P\\s{RAAT} on Unix")
NORMAL (L"To compile and link P\\s{RAAT} on Unix, you go to the directory that "
	"contains the source directories and the makefile, and copy a ##makefile.defs# "
	"file from the #makefiles directory. On Linux, for instance, you type")
CODE (L"> cp makefiles/makefile.defs.linux.dynamic ./makefile.defs")
NORMAL (L"The file ##makefile.defs# may require some editing after this, "
	"since the libraries in your Linux distribution may be different from mine, or your Motif library (##Xm.a# "
	"or ##Xm.so#) is in a different location. On Silicon Graphics Irix, you will use "
	"##makefile.defs.sgi#; on Sparc Solaris, you will probably use "
	"##makefile.defs.solaris.cde#; on HPUX, you will use ##makefile.defs.hpux#; "
	"on Intel Solaris, ##makefile.defs.solaris.cde.up.ac#.")
NORMAL (L"P\\s{RAAT} is preferably linked with a real Motif version (Lesstif gives some minor problems), "
	"i.e., there must be a library ##Xm.so# or ##Xm.a# and an #Xm include directory. "
	"There exists a free Motif library for Linux "
	"(##openmotif-devel-2.1.30-1_ICS.i386.rpm# from ICT); "
	"on SGI, Solaris, and HPUX, Motif libraries are included with the system, "
	"and the include directory comes with the development package.")
ENTRY (L"4. Building P\\s{RAAT} on Macintosh")
NORMAL (L"Open ##praat.xcodeproj# in Xcode and choose Build and Go.")
ENTRY (L"5. Building P\\s{RAAT} on Windows")
NORMAL (L"Open ##praat.mcp# in CodeWarrior (version 9.0 or higher), choose the target "
	"##praat_win#, and choose Make or Run.")
NORMAL (L"P\\s{RAAT} may compile under Visual C++ as well.")
ENTRY (L"6. Extending P\\s{RAAT}")
NORMAL (L"You can edit ##main/main_Praat.c#. "
	"This example shows you how to create a very simple program with all the functionality "
	"of the P\\s{RAAT} program, and a single bit more:")
CODE (L"\\# include \"praat.h\"")
CODE (L"")
CODE (L"DIRECT (HelloFromJane)")
CODE1 (L"Melder_information1 (L\"Hello, I am Jane.\");")
CODE (L"END")
CODE (L"")
CODE (L"void main (int argc, char **argv) {")
CODE1 (L"praat_init (\"Praat_Jane\", argc, argv);")
CODE1 (L"INCLUDE_LIBRARY (praat_uvafon_init)")
CODE1 (L"praat_addMenuCommand (\"Objects\", \"New\", \"Hello from Jane...\", NULL, 0, DO_HelloFromJane);")
CODE1 (L"praat_run ();")
CODE1 (L"return 0;")
CODE (L"}")
ENTRY (L"7. Learning how to program")
NORMAL (L"To see how objects are defined, take a look at ##sys/Thing.h#, ##sys/Data.h#, "
	"##sys/oo.h#, the ##XXX_def.h# files in the #fon directory, and the corresponding "
	"##XXX.c# files in the #fon directory. To see how commands show up on the buttons "
	"in the fixed and dynamic menus, take a look at the large interface description file "
	"##fon/praat_Fon.c#.")
ENTRY (L"8. Using the P\\s{RAAT} shell only")
NORMAL (L"For building the P\\s{RAAT} shell (the Objects and Picture windows) only, you need only the code in the five directories "
	"#kar, #GSL, #FLAC, #sys, and #dwsys. You delete the inclusion of praat_uvafon_init from #main. "
	"You will be able to build a P\\s{RAAT} shell, i.e. an Objects and a Picture window, "
	"which has no knowledge of the world, i.e., which does not know any objects "
	"that can be included in the list of objects. You could use this P\\s{RAAT} shell "
	"for modelling your own world and defining your own classes of objects. For advanced programmers only.")
MAN_END

}

/* End of file manual_Prog.c */
