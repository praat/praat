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
or clone ("fork") the praat/praat repository at any later change.

### 1.3. Steps to take if you want to extend Praat

First make sure that the source code can be compiled as is.
Then add your own buttons by editing `main/main_Praat.cpp` or `fon/praat_Fon.cpp`.
Consult the manual page on [Programming](http://www.fon.hum.uva.nl/praat/manual/Programming_with_Praat.html).

### 1.4. The programming language

Most of the source code is written in C++, but some parts are written in C.
The code requires that your compiler supports C99 and C++11 (for e.g. `char32_t` and rvalue references).

### 1.5. Compiling for Windows

Install Cygwin (on a 64-bit computer),
and under Cygwin install the Devel packages i686-w64-mingw32 (for 32-bit targets)
and/or x86_64-w64-mingw32 (for 64-bit targets).
Move the Praat sources directory somewhere in your `/home/yourname` tree.
Go to this sources directory (i.e. where `makefile` is) and type

    cp makefiles/makefile.defs.mingw32 ./makefile.defs

if you want to build Praat's 32-bit edition, or

    cp makefiles/makefile.defs.mingw64 ./makefile.defs

if you want to build Praat's 64-bit edition. Then type `make` to build `Praat.exe`
(use `make -j4` to speed this up, i.e. to use 4 processors in parallel).

Cross-compiling for Windows: use the [MinGW](http://www.mingw.org) compiler, perhaps on a Mac or Linux computer.
You can find toolchains for 32 and 64 bits
[here](http://sourceforge.net/projects/mingw-w64/files/) (look for Automated Builds).
Install the GDI+ headers and the GDI+ library ([32-bit](http://www.fon.hum.uva.nl/praat/libgdiplus.a-32.zip);
for 64-bit Windows just extract a GDI+ DLL from somewhere).
Then copy the file `makefiles/makefile.defs.darmin32` or `makefiles/makefile.defs.darmin64`
to the sources directory and rename it to `makefile.defs`.
Then type `make`.

### 1.6. Compiling for Macintosh

Extract the *xcodeproj64.zip* or *xcodeproj32.zip* file from the latest release
(depending on whether you want to compile the 64-bit or the 32-bit edition) into the directory that contains
`sys`, `fon`, `dwtools` and so on. Then open the project `praat32.xcodeproj` or `praat64.xcodeproj` in Xcode
and choose Build or Run. The project contains the target `praat_mac` (for MacOS X, on Intel processors).

If you get an error message like “Code Signing Identity xxx does not match any valid, non-expired,
code-signing certificate in your keychain”, then select the target `praat_mac`, go to Info → Build,
and switch “Code Signing Identity” to “Don’t Code Sign”,
or sign with your own certificate if you have one as a registered Apple developer.

If you get lots of errors saying “Expected unqualified-id” or “Unknown type name NSString”,
then you may have to switch the Type of some .cpp file from “C++ Source” to “Objective-C++ Source”
(under “Identity and Type” in the righthand sidebar).

### 1.7. Compiling on Linux and other Unixes

Install `libgtk2.0-dev` (and its dependencies) and `libasound2-dev`. Then go to the sources directory and type

    cp makefiles/makefile.defs.linux.alsa ./makefile.defs
    mv external/portaudio external/portaudio2014
    mv external/portaudio2007 external/portaudio

Then type `make` to build the program. You may have to `kill jackd` or `artsd` to get audio to function.
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

## 2. Binary executables

While the [Praat website](http://www.praat.org) contains the latest executable for all platforms that we support
(or used to support), the releases on GitHub contain many older executables as well.

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
- `praatXXXX_mac32.dmg`: disk image with executable for 32-bit Intel Macs (Carbon)
- `praatXXXX_xcodeproj64.zip`: zipped Xcode project file for the 64-bit edition (Cocoa)
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

## 3. Linux distributions

### 3.1 Debian

Praat is [included](https://packages.debian.org/source/sid/praat) in
the Debian distribution.  For installing it, just do:

    aptitude install praat

The Debian package (since version 5.3.56-1) includes a `praat-open-files`
command that allows opening files from the command line inside a Praat
process that is running already in the system.

Recent upstream versions of the `praat` package backported to the stable
and testing distributions of Debian are available in an
[APT repository](http://praat.cefala.org).
