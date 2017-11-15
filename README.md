# Praat: doing phonetics by computer

Most information on how the program works is in its manual and on http://www.praat.org,
which is also where you can download the latest binary executables from.

## 1. Compiling the source code

You need the Praat source code only in the following cases:

1. you want to extend Praat’s functionality by adding C or C++ code to it; or
2. you want to understand or reuse Praat’s source code; or
3. you want to compile Praat for a computer for which we do not provide binary executables,
e.g. Linux for non-Intel computers, FreeBSD, HP-UX, SGI, or SPARC Solaris.

Before trying to dive into Praat’s source code, you should be familiar with the working of the Praat program
and with writing Praat scripts. The Praat program can be downloaded from http://www.praat.org.

### 1.1. License

All of the code is available on GitHub under the
[GNU General Public License](http://www.fon.hum.uva.nl/praat/GNU_General_Public_License.txt).
Of course, any improvements are welcomed by the authors.

### 1.2. Downloading the archive

To download the latest source code of Praat from GitHub,
click on the *zip* or *tar.gz* archive at the latest release,
or fork ("clone") the praat/praat repository at any later change.

### 1.3. Steps to take if you want to extend Praat

First make sure that the source code can be compiled as is.
Then add your own buttons by editing `main/main_Praat.cpp` or `fon/praat_Fon.cpp`.
Consult the manual page on [Programming](http://www.fon.hum.uva.nl/praat/manual/Programming_with_Praat.html).

### 1.4. The programming language

Most of the source code is written in C++, but some parts are written in C.
The code requires that your compiler supports C99 and C++11 (for e.g. `char32_t` and rvalue references).

### 1.5. Compiling for Windows

To compile Praat's Windows edition on a 64-bit Windows computer,
install Cygwin on that computer,
and under Cygwin install the Devel packages i686-w64-mingw32 (for 32-bit targets)
and/or x86_64-w64-mingw32 (for 64-bit targets).
Move the Praat sources directory somewhere in your `/home/yourname` tree.
Go into this sources directory (where the folders `fon` and `sys` are).
Then if you want to build Praat's 32-bit edition, type

    cp makefiles/makefile.defs.mingw32 ./makefile.defs

or if you want to build Praat's 64-bit edition, type

    cp makefiles/makefile.defs.mingw64 ./makefile.defs

Then type `make` to build `Praat.exe`
(use `make -j4` to speed this up, i.e. to use 4 processors in parallel).

Cross-compiling for Windows on a Mac or Linux computer is slightly more difficult, but not impossible.
You can use the [MinGW](http://www.mingw.org) compiler,
for which you can find 32- and 64-bit toolchains
[here](http://sourceforge.net/projects/mingw-w64/files/) (look for Automated Builds).
Install the GDI+ headers and the GDI+ library ([32-bit](http://www.fon.hum.uva.nl/praat/libgdiplus.a-32.zip);
for 64-bit Windows just extract a GDI+ DLL from somewhere).
Then copy the file `makefiles/makefile.defs.darmin32` or `makefiles/makefile.defs.darmin64`
to the sources directory and rename it to `makefile.defs`.
Then type `make`.

### 1.6. Compiling for Macintosh

Extract the *praatXXXX_xcodeproj64.zip* file from the [latest release](https://github.com/praat/praat/releases)
into the directory that contains
`sys`, `fon`, `dwtools` and so on. Then open the project `praat64.xcodeproj` in Xcode
and choose Build or Run for the target `praat_mac64`.

If you get an error message like “Code Signing Identity xxx does not match any valid, non-expired,
code-signing certificate in your keychain”, then select the target `praat_mac64`, go to Info → Build,
and switch “Code Signing Identity” to “Don’t Code Sign”,
or sign with your own certificate if you have one as a registered Apple developer.

If you get lots of errors saying “Expected unqualified-id” or “Unknown type name NSString”,
then you may have to switch the Type of some .cpp file from “C++ Source” to “Objective-C++ Source”
(under “Identity and Type” in the righthand sidebar).

If you want to build Praat as a library instead of as an executable,
try the target `praat_mac64_a` (static) or `praat_mac64_so` (dynamic).

### 1.7. Compiling on Linux and other Unixes

To set up the required system libraries, install some graphics and sound packages:

	sudo apt-get install libgtk2.0-dev
	sudo apt-get install libasound2-dev
	sudo apt-get install libpulse-dev

To set up your source tree for Linux, go to Praat's sources directory (where the folders `fon` and `sys` are) and type

    cp makefiles/makefile.defs.linux.pulse ./makefile.defs

To build the Praat executable, type `make`.
If your Unix isn’t Linux, you may have to edit the library names in the makefile
(you may need pthread, gtk-x11-2.0, gdk-x11-2.0, atk-1.0, pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0,
cairo, gio-2.0, pango-1.0, freetype, fontconfig, gobject-2.0, gmodule-2.0, gthread-2.0, rt, glib-2.0, asound).

When compiling Praat on an external supercomputer or so, you will not have sound.
If you do have `libgtk2.0-dev` (and its dependencies), do

    cp makefiles/makefile.defs.linux.silent ./makefile.defs

Then type `make` to build the program. If your Unix isn’t Linux,
you may have to edit the library names in the makefile (you may need pthread, gtk-x11-2.0, gdk-x11-2.0, atk-1.0,
pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0, cairo, gio-2.0, pango-1.0, freetype, fontconfig, gobject-2.0,
gmodule-2.0, gthread-2.0, rt, glib-2.0).

When compiling Praat for use as a server for commands from your web pages, you may not need sound or a GUI. Do

    cp makefiles/makefile.defs.linux.nogui ./makefile.defs

which creates the executable `praat_nogui`. If you don't need graphics (.e.g PNG files) either
(i.e. you need only Praat's computation), you can create an even lighter edition:

    cp makefiles/makefile.defs.linux.barren ./makefile.defs

which creates the executable `praat_barren`. Then type `make` to build the program. If your Unix isn’t Linux,
you may have to edit the library names in the makefile.

## 2. Binary executables

While the [Praat website](http://www.praat.org) contains the latest executable for all platforms that we support
(or used to support), the [releases on GitHub](https://github.com/praat/praat/releases) contain many older executables as well.

The meaning of the names of binary files available on GitHub is as follows:

### 2.1. Windows binaries
- `praatXXXX_win64.zip`: zipped executable for 64-bit Windows (XP and higher)
- `praatXXXX_win32.zip`: zipped executable for 32-bit Windows (XP and higher)
- `praatconXXXX_win64.zip`: zipped executable for 64-bit Windows, console edition
- `praatconXXXX_win32.zip`: zipped executable for 32-bit Windows, console edition
- `praatconXXXX_win32sit.exe`: self-extracting StuffIt archive with executable for 32-bit Windows, console edition
- `praatXXXX_win98.zip`: zipped executable for Windows 98
- `praatXXXX_win98sit.exe`: self-extracting StuffIt archive with executable for Windows 98

### 2.2. Mac binaries
- `praatXXXX_mac64.dmg`: disk image with executable for 64-bit Intel Macs (Cocoa)
- `praatXXXX_xcodeproj64.zip`: zipped Xcode project file for the 64-bit edition (Cocoa)
- `praatXXXX_mac32.dmg`: disk image with executable for 32-bit Intel Macs (Carbon)
- `praatXXXX_xcodeproj32.zip`: zipped Xcode project file for the 32-bit edition (Carbon)
- `praatXXXX_macU.dmg`: disk image with universal executable for (32-bit) PPC and Intel Macs (Carbon)
- `praatXXXX_macU.sit`: StuffIt archive with universal executable for (32-bit) PPC and Intel Macs (Carbon)
- `praatXXXX_macU.zip`: zipped universal executable for (32-bit) PPC and Intel Macs (Carbon)
- `praatXXXX_macX.zip`: zipped executable for MacOS X (PPC)
- `praatXXXX_mac9.sit`: StuffIt archive with executable for MacOS 9
- `praatXXXX_mac9.zip`: zipped executable for MacOS 9
- `praatXXXX_mac7.sit`: StuffIt archive with executable for MacOS 7

### 2.3. Unix binaries
- `praatXXXX_linux64.tar.gz`: gzipped tarred executable for 64-bit Linux (GTK)
- `praatXXXX_linux32.tar.gz`: gzipped tarred executable for 32-bit Linux (GTK)
- `praatXXXX_linux_motif64.tar.gz`: gzipped tarred executable for 64-bit Linux (Motif)
- `praatXXXX_linux_motif32.tar.gz`: gzipped tarred executable for 32-bit Linux (Motif)
- `praatXXXX_solaris.tar.gz`: gzipped tarred executable for Solaris

