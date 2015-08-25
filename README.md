# Praat: doing phonetics by computer

Most information on how the program works is in its manual and on http://www.praat.org.

### 1. Compiling the source code

You need the Praat source code only in the following cases:

1. you want to extend Praat’s functionality by adding C or C++ code to it; or
2. you want to understand or reuse Praat’s source code; or
3. you want to compile Praat for a computer for which we do not provide binary executables, e.g. Linux for non-Intel computers, FreeBSD, HP-UX, SGI, or SPARC Solaris.

Before trying to dive into Praat’s source code, you should be familiar with the working of the Praat program and with writing Praat scripts. The Praat program can be downloaded from http://www.praat.org.

##### 1.1. License

All of the code is available under the [GNU General Public License](http://www.fon.hum.uva.nl/praat/GNU_General_Public_License.txt). Of course, any improvements are welcomed by the authors.

##### 1.2. Downloading the archive

To download the latest source code of Praat,
click on one of the *zip* or *tar.gz* archives at the latest release or at any later change.

##### 1.3. Unpacking the archive

On most computers you can unpack the *zip* file by double-clicking. If you prefer to try the *tar.gz* file instead, drop it on *StuffIt Expander* (on Windows), double-click it (on Macintosh), or use `gunzip` and `tar xvf` (on Unix).

##### 1.4. Steps to take if you want to extend Praat

First make sure that the source code can be compiled as is. Then add your own buttons by editing `main/main_Praat.cpp` or `fon/praat_Fon.cpp`. Consult the manual page on [Programming](http://www.fon.hum.uva.nl/praat/manual/Programming_with_Praat.html).

##### 1.5. The programming language

Most of the source code is written in C++, but some parts are written in C.
The code requires that your compiler supports C99 and C++11 (for e.g. `char32_t`).

##### 1.6. Compiling on Macintosh

Extract the *xcodeproj64.zip* or *xcodeproj32.zip* file from the latest release (depending on whether you want to compile the 64-bit or the 32-bit edition) into the directory that contains `sys`, `fon`, `dwtools` and so on. Then open the project `praat32.xcodeproj` or `praat64.xcodeproj` in Xcode and choose Build or Run. The project contains the target `praat_mac` (for MacOS X, on Intel processors).

If you get an error message like “Code Signing Identity xxx does not match any valid, non-expired, code-signing certificate in your keychain”, then select the target `praat_mac`, go to Info → Build, and switch “Code Signing Identity” to “Don’t Code Sign”, or sign with your own certificate if you have one as a registered Apple developer.

If you get lots of errors saying “Expected unqualified-id” or “Unknown type name NSString”, then you may have to switch the Type of some .cpp file from “C++ Source” to “Objective-C++ Source” (under “Identity and Type” in the righthand sidebar).

##### 1.7. Compiling on Linux and other Unixes

Install `libgtk2.0-dev` (and its dependencies) and `libasound2-dev`. Then go to the sources directory and type

    cp makefiles/makefile.defs.linux.alsa ./makefile.defs
    mv external/portaudio external/portaudio2014
    mv external/portaudio2007 external/portaudio

Then type `make` to build the program. One may have to `kill jackd` or `artsd` to get audio to function. If your Unix isn’t Linux, you may have to edit the library names in the makefile (you may need pthread, gtk-x11-2.0, gdk-x11-2.0, atk-1.0, pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0, cairo, gio-2.0, pango-1.0, freetype, fontconfig, gobject-2.0, gmodule-2.0, gthread-2.0, rt, glib-2.0, asound).

When compiling Praat on an external supercomputer or so, you will not have sound. If you do have `libgtk2.0-dev` (and its dependencies), do

    cp makefiles/makefile.defs.linux.silent ./makefile.defs

Then type `make` to build the program. If your Unix isn’t Linux, you may have to edit the library names in the makefile (you may need pthread, gtk-x11-2.0, gdk-x11-2.0, atk-1.0, pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0, cairo, gio-2.0, pango-1.0, freetype, fontconfig, gobject-2.0, gmodule-2.0, gthread-2.0, rt, glib-2.0).

##### 1.8. Compiling for Windows

Use the [MinGW](http://www.mingw.org) compiler, perhaps on a Mac or Linux computer, to avoid any chance for viruses. You can find toolchains for 32 and 64 bits [here](http://sourceforge.net/projects/mingw-w64/files/) (look for Automated Builds). Install the GDI+ headers and the GDI+ library ([32-bit](http://www.fon.hum.uva.nl/praat/libgdiplus.a-32.zip); for 64-bit Windows just extract a GDI+ DLL from somewhere). Then copy the file `makefiles/makefile.defs.mingw32` or `makefiles/makefile.defs.mingw64` to the sources directory (i.e. where `makefile` is) and rename it to `makefile.defs`. Then go to the sources directory and type `make`.

If you get error messages about `__stosb` and the like, get rid of `Data` in `winnt.h` and/or `intrin-impl.h` and/or `intrin_mac.h` (rename to `DataXXX` or so), because of name clash with `sys/Data.h`.

### 2. Binary executables

The meaning of the names of binary files uploaded on GitHub is as follows:

##### 2.1. Windows binaries
- `praatXXXX_win64.zip`: zipped executable for 64-bit Windows (XP and higher)
- `praatXXXX_win32.zip`: zipped executable for 32-bit Windows (XP and higher)
- `praatconXXXX_win64.zip`: zipped executable for 64-bit Windows, console edition
- `praatconXXXX_win32.zip`: zipped executable for 32-bit Windows, console edition
- `praatconXXXX_win32sit.exe`: self-extracting StuffIt archive with executable for 32-bit Windows, console edition
- `praatXXXX_win98.zip`: zipped executable for Windows 98
- `praatXXXX_win98sit.exe`: self-extracting StuffIt archive with executable for Windows 98

##### 2.2. Mac binaries
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

##### 2.3. Unix binaries
- `praatXXXX_linux64.tar.gz`: gzipped tarred executable for 64-bit Linux (GTK)
- `praatXXXX_linux32.tar.gz`: gzipped tarred executable for 32-bit Linux (GTK)
- `praatXXXX_linux_motif64.tar.gz`: gzipped tarred executable for 64-bit Linux (Motif)
- `praatXXXX_linux_motif32.tar.gz`: gzipped tarred executable for 32-bit Linux (Motif)
- `praatXXXX_solaris.tar.gz`: gzipped tarred executable for Solaris
