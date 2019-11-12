# Praat: doing phonetics by computer

Most information on how the program works is in its manual and on http://praat.org,
which is also where you can download the latest binary executables from.

## 1. Binary executables

While the [Praat website](http://praat.org) contains the latest executable for all platforms that we support
(or used to support), the [releases on GitHub](https://github.com/praat/praat/releases) contain many older executables as well.

The meaning of the names of binary files available on GitHub is as follows:

### 1.1. Windows binaries
- `praatXXXX_win64.zip`: zipped executable for 64-bit Windows (XP and higher)
- `praatXXXX_win32.zip`: zipped executable for 32-bit Windows (XP and higher)
- `praatconXXXX_win64.zip`: zipped executable for 64-bit Windows, console edition
- `praatconXXXX_win32.zip`: zipped executable for 32-bit Windows, console edition
- `praatconXXXX_win32sit.exe`: self-extracting StuffIt archive with executable for 32-bit Windows, console edition
- `praatXXXX_win98.zip`: zipped executable for Windows 98
- `praatXXXX_win98sit.exe`: self-extracting StuffIt archive with executable for Windows 98

### 1.2. Mac binaries
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

### 1.3. Unix binaries
- `praatXXXX_chrome64.tar.gz`: gzipped tarred executable for 64-bit Linux on Chromebooks (GTK)
- `praatXXXX_linux64.tar.gz`: gzipped tarred executable for 64-bit Linux (GTK)
- `praatXXXX_linux32.tar.gz`: gzipped tarred executable for 32-bit Linux (GTK)
- `praatXXXX_linux_motif64.tar.gz`: gzipped tarred executable for 64-bit Linux (Motif)
- `praatXXXX_linux_motif32.tar.gz`: gzipped tarred executable for 32-bit Linux (Motif)
- `praatXXXX_solaris.tar.gz`: gzipped tarred executable for Solaris

## 2. Compiling the source code

You need the Praat source code only in the following cases:

1. you want to extend Praat’s functionality by adding C or C++ code to it; or
2. you want to understand or reuse Praat’s source code; or
3. you want to compile Praat for a computer for which we do not provide binary executables,
e.g. Linux for some non-Intel computers, FreeBSD, HP-UX, SGI, or SPARC Solaris.

Before trying to dive into Praat’s source code, you should be familiar with the working of the Praat program
and with writing Praat scripts. The Praat program can be downloaded from http://www.praat.org.

### 2.1. License

All of the code is available on GitHub under the
[GNU General Public License](http://www.fon.hum.uva.nl/praat/GNU_General_Public_License.txt).
Of course, any improvements are welcomed by the authors.

### 2.2. Downloading the archive

To download the latest source code of Praat from GitHub,
click on the *zip* or *tar.gz* archive at the latest release,
or fork ("clone") the praat/praat repository at any later change.

### 2.3. Steps to take if you want to extend Praat

First make sure that the source code can be compiled as is.
Then add your own buttons by editing `main/main_Praat.cpp` or `fon/praat_Fon.cpp`.
Consult the manual page on [Programming](http://www.fon.hum.uva.nl/praat/manual/Programming_with_Praat.html).

### 2.4. The programming language

Most of the source code is written in C++, but some parts are written in C.
The code requires that your compiler supports C99 and C++17.

## 3. Compiling the source code on one platform

### 3.1. Compiling for Windows

To compile Praat's Windows edition on a 64-bit Windows computer,
install Cygwin on that computer,
and under Cygwin install the Devel packages x86_64-w64-mingw32 (for 64-bit targets)
and/or i686-w64-mingw32 (for 32-bit targets).
Move the Praat sources directory somewhere in your `/home/yourname` tree,
e.g. as `/home/yourname/praats` and/or `/home/yourname/praats32`;
the folders `fon` and `sys` shoudl be visible within these folders.
If you want to build Praat's 64-bit edition, type

    cd ~/praats
    cp makefiles/makefile.defs.mingw64 ./makefile.defs

or if you want to build Praat's 32-bit edition, type

    cd ~/praats32
    cp makefiles/makefile.defs.mingw32 ./makefile.defs

Then type `make` to build `Praat.exe`
(use `make -j12` to speed this up, i.e. to use 12 processors in parallel).

### 3.2. Compiling for Macintosh

Extract the *praatXXXX_xcodeproj64.zip* file from the [latest release](https://github.com/praat/praat/releases)
into the directory that contains
`sys`, `fon`, `dwtools` and so on. Then open the project `praat64.xcodeproj` in Xcode
and choose Build or Run for the target `praat_mac64`.
Note that on Mojave or Catalina you may have to copy the 10.13 SDK into your Xcode app,
because Praat will have problems with the Dark Mode if you compile with the 10.15 SDK.

If you get an error message like “Code Signing Identity xxx does not match any valid, non-expired,
code-signing certificate in your keychain”, then select the target `praat_mac64`, go to Info → Build,
and switch “Code Signing Identity” to “Don’t Code Sign”,
or sign with your own certificate if you have one as a registered Apple developer.

If you get lots of errors saying “Expected unqualified-id” or “Unknown type name NSString”,
then you may have to switch the Type of some .cpp file from “C++ Source” to “Objective-C++ Source”
(under “Identity and Type” in the righthand sidebar).

If you want to build Praat as a library instead of as an executable,
try the target `praat_mac64_a` (static) or `praat_mac64_so` (dynamic).

### 3.3. Compiling on Linux and other Unixes

To set up the required system libraries, install some graphics and sound packages:

	sudo apt-get install libgtk2.0-dev
	sudo apt-get install libasound2-dev
	sudo apt-get install libpulse-dev
	sudo apt-get install libjack-dev

To set up your source tree for Linux, go to Praat's sources directory (where the folders `fon` and `sys` are) and type

    cp makefiles/makefile.defs.linux.pulse ./makefile.defs

To build the Praat executable, type `make`.
If your Unix isn’t Linux, you may have to edit the library names in the makefile
(you may need pthread, gtk-x11-2.0, gdk-x11-2.0, atk-1.0, pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0,
cairo, gio-2.0, pango-1.0, freetype, fontconfig, gobject-2.0, gmodule-2.0, gthread-2.0, rt, glib-2.0, asound, jack).

When compiling Praat on an external supercomputer or so, you will not have sound.
If you do have `libgtk2.0-dev` (and its dependencies), do

    cp makefiles/makefile.defs.linux.silent ./makefile.defs

Then type `make` to build the program. If your Unix isn’t Linux,
you may have to edit the library names in the makefile (you may need pthread, gtk-x11-2.0, gdk-x11-2.0, atk-1.0,
pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0, cairo, gio-2.0, pango-1.0, freetype, fontconfig, gobject-2.0,
gmodule-2.0, gthread-2.0, rt, glib-2.0).

When compiling Praat for use as a server for commands from your web pages, you may not need sound or a GUI. Do

    cp makefiles/makefile.defs.linux.nogui ./makefile.defs

which creates the executable `praat_nogui`. If you don't need graphics (e.g. PNG files) either
(i.e. you need only Praat's computation), you can create an even lighter edition:

    cp makefiles/makefile.defs.linux.barren ./makefile.defs

which creates the executable `praat_barren`. Then type `make` to build the program. If your Unix isn’t Linux,
you may have to edit the library names in the makefile.

## 4. Compiling the source code on all platforms simultaneously

The easiest way to develop your Praat clone on all platforms simultaneously,
is to edit the source code on a Mac (with Xcode)
and use Parallels Desktop for Windows and Linux building and testing.
This is how Praat is currently developed.
Editing takes place in Xcode,
after which building Praat involves no more then typing Command-B into Xcode
(or Command-R to build and run)
or `b` into a Windows or Linux terminal (or `r` to build and run).

### 4.1. MacOS set-up

Your source code folders, such as `fon` and `sys`, will reside in a folder like `/Users/yourname/Praats/src`,
where you also put `praat64.xcodeproj`, as described above in 3.2.
On Paul’s 2018 MacBook Pro with Xcode 11.2, building Praat with Command-B or Command-R,
after cleaning the build folder with Shift-Command-K, takes 3 minutes.

### 4.2. Windows set-up

Under Parallels Desktop 15 or later, install Windows 10. In Windows 10, install Cygwin,
and create a `praats` folder, as described above in 3.1.

There are two options for your source tree: either it resides on the MacOS disk
(which you will mount from Windows anyway), or it resides on the Windows disk.
Compiling Praat for Windows on the MacOS disk takes 13 minutes,
whereas compiling Praat on the Windows disk takes only 4 minutes and 20 seconds.
So we go with installing the source tree under the Cygwin home folder, as follows.

You need to get the source from the MacOS disk, so you have to mount the MacOS disk
from Cygwin. This is easy: in Parallels Desktop, choose `Windows 10` -> `Configure`,
then `Options`, then `Sharing`, then `Share Mac`, and set `Share folders` to `Home folder only`
(if this scares you, then use `Custom Folders` instead).
Your MacOS home folder (i.e. `/Users/yourname`) is now visible anywhere on Windows 10
as the `Z` drive (or so), and from the `Cygwin64 Terminal` you can access it as `/cygdrive/z`.

The `Cygwin64 Terminal` uses the `bash` shell, so it will be good to define

    MAC_SOURCE="/cygdrive/z/Praats/src"

in `/home/yourname/.bashrc` and/or `/home/yourname/.bash_profile` in your Cygwin home folder.

When developing Praat for Windows, you just edit your files in Xcode;
do not forget to save them (as you do e.g. by building in Xcode).
Then, just as you use Command-B and Command-R in Xcode,
you will be able to type `b` (Build) or `r` (Build & Run) into your `Cygwin64 Terminal`
after you add the following definitions into `/home/yourname/.bashrc` and/or `/home/yourname/.bash_profile`:

    PRAAT_EXCLUDES='--exclude="*.xcodeproj" --exclude="Icon*" --exclude=".*"'
    alias b="  cd ~/praats   ; rsync -rptvz $MAC_SOURCE/ $PRAAT_EXCLUDES . ;\
        cp makefiles/makefile.defs.mingw64 makefile.defs ; make -j12"
    alias b32="cd ~/praats32 ; rsync -rptvz $MAC_SOURCE/ $PRAAT_EXCLUDES . ;\
        cp makefiles/makefile.defs.mingw32 makefile.defs ; make -j12"
    alias p="~/praats/Praat.exe"
    alias p32="~/praats32/Praat.exe"
    alias r="b ; p"
    alias r32="b32 ; p32"

This also defines `p` for running Praat without first rebuilding it.

### 4.3. Linux set-up

Under Parallels Desktop 15 or later, install Ubuntu 18.04, and create
a folder `praats` in your home folder, as described above in 3.3.

In Parallels Desktop, choose `Ubuntu 18.04` -> `Configure`,
then `Options`, then `Sharing`, then `Share Mac`, and set `Share folders` to `Home folder only`
(or use `Custom Folders` instead).
Your MacOS home folder (i.e. `/Users/yourname`) is now visible on the Ubuntu 18.04 desktop
as `Home`, and from the `Terminal` you can access it as `/media/psf/Home`.

The `Terminal` uses the `bash` shell, so it will be good to define

    MAC_SOURCE="/media/psf/Home/Praats/src"

in `/home/parallels/.bash_aliases` in your Ubuntu 18.04 home folder
(this will be run automatically by `.bashrc` whenever you start a `Terminal` window).

When developing Praat for Linux, you just edit and save your files in Xcode.
You will be able to type `b` (Build) or `p` (run Praat) or `r` (Build & Run) into your `Terminal`
after you add the following definitions into `/home/parallels/.bash_aliases`:

    PRAAT_EXCLUDES='--exclude="*.xcodeproj" --exclude="Icon*" --exclude=".*"'
    alias b="cd ~/praats ; rsync -rptvz $MAC_SOURCE/ $PRAAT_EXCLUDES . ;\
        cp makefiles/makefile.defs.linux.pulse makefile.defs ; make -j12"
    alias p="~/praats/praat"
    alias r="b ; p"

To build `praat_barren`, create a folder `praatsb`, and to your `.bash_aliases` add

    alias bb="cd ~/praatsb ; rsync -rptvz $MAC_SOURCE/ $PRAAT_EXCLUDES . ;\
        cp makefiles/makefile.defs.linux.barren makefile.defs ; make -j12"
    alias pb="~/praatsb/praat_barren"
    alias rb="bb ; pb"

You test `praat_barren` briefly by typing

    pb --version

To build `praat_nogui`, create a folder `praatsn`, and to your `.bash_aliases` add

    alias bn="cd ~/praatsn ; rsync -rptvz $MAC_SOURCE/ $PRAAT_EXCLUDES . ;\
        cp makefiles/makefile.defs.linux.nogui makefile.defs ; make -j12"
    alias pn="~/praatsn/praat_nogui"
    alias rn="bn ; pn"

You test `praat_nogui` briefly by typing

    pn --version

To build `praat` for Chrome64 (64-bit Intel Chromebooks only),
create a folder `praatc`, and to your `.bash_aliases` add

    alias bc="cd ~/praatsc ; rsync -rptvz $MAC_SOURCE/ $PRAAT_EXCLUDES . ;\
        cp makefiles/makefile.defs.chrome64 makefile.defs ; make -j12"
    alias pc="~/praatsc/praat"
    alias rc="bc ; pc"

To test `praat` for Chrome64, you can just run it on Ubuntu 18.04 by typing `pc`,
or you transfer it to a Chromebook for the real test.
