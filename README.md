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
- `praatXXXX_mac.dmg`: disk image with universal executable for (64-bit) Intel and Apple Silicon Macs (Cocoa)
- `praatXXXX_xcodeproj.zip`: zipped Xcode project file for the universal (64-bit) edition (Cocoa)
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
- `praatXXXX_rpi_armv7.tar.gz`: gzipped tarred executable for 32-bit Linux on the Raspberry Pi 4B (GTK 2 or 3)
- `praatXXXX_chrome64.tar.gz`: gzipped tarred executable for 64-bit Linux on Chromebooks (GTK 2 or 3)
- `praatXXXX_linux64barren.tar.gz`: gzipped tarred executable for 64-bit Linux, without GUI or graphics
- `praatXXXX_linux64nogui.tar.gz`: gzipped tarred executable for 64-bit Linux, without GUI but with graphics (Cairo and Pango)
- `praatXXXX_linux64.tar.gz`: gzipped tarred executable for 64-bit Linux (GTK 2 or 3)
- `praatXXXX_linux32.tar.gz`: gzipped tarred executable for 32-bit Linux (GTK 2)
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
the folders `fon` and `sys` should be visible within these folders.
If you want to build Praat's 64-bit edition, type

    cd ~/praats
    cp makefiles/makefile.defs.mingw64 ./makefile.defs

or if you want to build Praat's 32-bit edition, type

    cd ~/praats32
    cp makefiles/makefile.defs.mingw32 ./makefile.defs

Then type `make` to build `Praat.exe`
(use `make -j12` to speed this up, i.e. to use 12 processors in parallel).

### 3.2. Compiling for Macintosh

Extract the *praatXXXX_xcodeproj.zip* file from the [latest release](https://github.com/praat/praat/releases)
into the directory that contains `sys`, `fon`, `dwtools` and so on.
Then open the project `praat.xcodeproj` in Xcode 12 and choose Build or Run for the target `praat_mac`.
You can compile with the 11.0 SDK, which will work as far back as macOS 10.7, which is our deployment target.

If you get an error message like “Code Signing Identity xxx does not match any valid, non-expired,
code-signing certificate in your keychain”, then select the target `praat_mac`, go to Info → Build,
and switch “Code Signing Identity” to “Don’t Code Sign”,
or sign with your own certificate if you have one as a registered Apple developer.

If you get lots of errors saying “Expected unqualified-id” or “Unknown type name NSString”,
then you may have to switch the Type of some .cpp file from “C++ Source” to “Objective-C++ Source”
(under “Identity and Type” in the righthand sidebar).

If you want to build Praat as a library instead of as an executable,
try the target `praat_mac64_a` (static) or `praat_mac64_so` (dynamic).

**Notarization.** If you want others to be able to use your Mac app,
you will probably have to not only *sign* the executable, but also *notarize* it. To this end,
do Xcode (version 12) -> Product -> Archive -> Distribute App -> Developer ID -> Upload ->
Automatically manage signing -> Upload -> ...wait... (“Package Approved”) ...wait...
(“Ready to distribute”) -> Export Notarized App). If your Praat.app was built into
`~/builds/mac_products/Configuration64`, then you can save the notarized
`Praat.app` in `~/builds/mac_products`, then drag it in the Finder to
`~/builds/mac_products/Configuration64`, overwriting the non-notarized
Praat.app that was already there. If on the way you receive an error
“App Store Connect Operation Error -- You must first sign the relevant contracts online”,
you have to log in to `developer.apple.com` and do Review Agreement -> Agree;
you may then also have to go to App Store Connect, log in, then Agreements, Tax, and Banking
-> View and Agree to Terms (even if you have no paid apps).

### 3.3. Compiling on Linux and other Unixes

To set up the required system libraries, install some graphics and sound packages:

    sudo apt-get install libgtk-3-dev
    sudo apt-get install libasound2-dev
    sudo apt-get install libpulse-dev
    sudo apt-get install libjack-dev

To set up your source tree for Linux, go to Praat's sources directory (where the folders `fon` and `sys` are)
and type one of the four following commands:

    # on Ubuntu command line
    cp makefiles/makefile.defs.linux.pulse ./makefile.defs

    # on Ubuntu command line
    cp makefiles/makefile.defs.linux.pulse_static ./makefile.defs

    # on Chromebook command line
    cp makefiles/makefile.defs.chrome64 ./makefile.defs

    # on Raspberry Pi command line
    cp makefiles/makefile.defs.linux.rpi ./makefile.defs

To build the Praat executable, type `make` or `make -j12`.
If your Unix isn’t Linux, you may have to edit the library names in the makefile
(you may need pthread, gtk-3, gdk-3, atk-1.0, pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0,
cairo-gobject, cairo, gio-2.0, pango-1.0, freetype, fontconfig, gobject-2.0, gmodule-2.0, gthread-2.0, rt, glib-2.0, asound, jack).

When compiling Praat on an external supercomputer or so, you will not have sound.
If you do have `libgtk-3-dev` (and its dependencies), do

    cp makefiles/makefile.defs.linux.silent ./makefile.defs

Then type `make` or `make -j12` to build the program. If your Unix isn’t Linux,
you may have to edit the library names in the makefile (you may need pthread, gtk-3, gdk-3, atk-1.0,
pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0, cairo-gobject, cairo, gio-2.0, pango-1.0, freetype, fontconfig, gobject-2.0,
gmodule-2.0, gthread-2.0, rt, glib-2.0).

When compiling Praat for use as a server for commands from your web pages, you may not need sound or a GUI. Do

    cp makefiles/makefile.defs.linux.nogui ./makefile.defs

which creates the executable `praat_nogui`. If you don't need graphics (e.g. PNG files) either
(i.e. you need only Praat's computation), you can create an even lighter edition:

    cp makefiles/makefile.defs.linux.barren ./makefile.defs

which creates the executable `praat_barren`. Then type `make` or `make -j12` to build the program.
If your Unix isn’t Linux, you may have to edit the library names in the makefile.

## 4. Compiling the source code on all platforms simultaneously

The easiest way to develop your Praat clone on all platforms simultaneously,
is to edit the source code on a Mac (with Xcode)
and use Parallels Desktop for Windows and Linux building and testing.
This is how Praat is currently developed.
Editing takes place in Xcode,
after which building Praat involves no more than typing Command-B into Xcode
(or Command-R to build and run)
or `praat-build` into a Windows or Linux terminal (or `praat-run` to build and run).

### 4.1. MacOS development set-up

Your source code folders, such as `fon` and `sys`, will reside in a folder like `/Users/yourname/Praats/src`,
where you also put `praat.xcodeproj`, as described above in 3.2.
On Paul’s 2018 MacBook Pro with Xcode 12.2, building Praat with Command-B or Command-R,
after cleaning the build folder with Shift-Command-K,
takes 120 seconds for the x86_64 part and 110 seconds for the ARM64 part (optimization level O3).

### 4.2. Windows development set-up

Under Parallels Desktop 15 or later, install Windows 10. In Windows 10, install Cygwin,
and create a `praats` folder, as described above in 3.1.

There are two options for your source tree: either it resides on the MacOS disk
(which you will mount from Windows anyway), or it resides on the Windows disk.
Compiling Praat for Windows on the MacOS disk takes 13 minutes (optimization level O3),
whereas compiling Praat on the Windows disk takes only 4 minutes and 20 seconds.
So we go with installing the source tree under the Cygwin home folder, as follows.

You need to get the source from the MacOS disk, so you have to mount the MacOS disk
from Cygwin. This is easy: in Parallels Desktop, choose `Windows 10` -> `Configure`,
then `Options`, then `Sharing`, then `Share Mac`, and set `Share folders` to `Home folder only`
(if this scares you, then use `Custom Folders` instead).
Your MacOS home folder (i.e. `/Users/yourname`) is now visible anywhere on Windows 10
as the `Z` drive (or so), and from the `Cygwin64 Terminal` you can access it as `/cygdrive/z`.

When developing Praat for Windows, you just edit your files in Xcode;
do not forget to save them (as you do e.g. by building in Xcode).
Then, just as you use Command-B and Command-R in Xcode,
you will be able to type `praat-build` (which only builds) or `praat-run` (which builds and runs)
into your `Cygwin64 Terminal`. To accomplish this,
add the following definitions into `/home/yourname/.profile` in your Cygwin home folder,
so that the `bash` shell will automatically execute them whenever you start your `Cygwin64 Terminal`:

    # in Cygwin:~/.profile
    PRAAT_SOURCES="/cygdrive/z/Praats/src"
    PRAAT_EXCLUDES='--exclude="*.xcodeproj" --exclude="Icon*" --exclude=".*" --exclude="*kanweg*"'
    alias praat-build="( cd ~/praats &&\
        rsync -rptvz $PRAAT_SOURCES/ $PRAAT_EXCLUDES . &&\
        cp makefiles/makefile.defs.mingw64 makefile.defs &&\
        make -j12 )"
    alias praat="~/praats/Praat.exe"
    alias praat-run="praat-build && praat"

This also defines `praat` for running Praat without first rebuilding it.
The cycle from editing Praat on the Mac to running the new version on Windows therefore takes only two steps:

1. edit and save the source code in Xcode on your Mac;
2. type `praat-run` on your Windows 10 (under Parallels Desktop on your Mac).

If you also want to develop the 32-bit edition, you add to `.profile`:

    # in Cygwin:~/.profile
    alias praat32-build="( cd ~/praats32 &&\
        rsync -rptvz $PRAAT_SOURCES/ $PRAAT_EXCLUDES . &&\
        cp makefiles/makefile.defs.mingw32 makefile.defs &&\
        make -j12 )"
    alias praat32="~/praats32/Praat.exe"
    alias praat32-run="praat32-build && praat32"

### 4.3. Linux development set-up

Under Parallels Desktop 15 or later, install Ubuntu 18.04, and create
a folder `praats` in your home folder, as described above in 3.3.

In Parallels Desktop, choose `Ubuntu 18.04` -> `Configure`,
then `Options`, then `Sharing`, then `Share Mac`, and set `Share folders` to `Home folder only`
(or use `Custom Folders` instead).
Your MacOS home folder (i.e. `/Users/yourname`) is now visible on the Ubuntu 18.04 desktop
as `Home`, and from the `Terminal` you can access it as `/media/psf/Home`.

When developing Praat for Linux, you just edit and save your files in Xcode.
You will be able to type `praat-build` (which only builds) or `praat-run` (which builds and runs)
into your `Terminal` after you add the following definitions into
`/home/parallels/.bash_aliases` in your Ubuntu 18.04 home folder
(this will be run automatically by `.bashrc` whenever you start a `Terminal` window,
assuming that it uses the `bash` shell):

    # in Ubuntu:~/.bash_aliases
    PRAAT_SOURCES="/media/psf/Home/Praats/src"
    PRAAT_EXCLUDES='--exclude="*.xcodeproj" --exclude="Icon*" --exclude=".*" --exclude="*kanweg*"'
    alias praat-build="( cd ~/praats &&\
        rsync -rptvz $PRAAT_SOURCES/ $PRAAT_EXCLUDES . &&\
        cp makefiles/makefile.defs.linux.pulse makefile.defs &&\
        make -j12 )"
    alias praat="~/praats/praat"
    alias praat-run="praat-build && praat"
    alias praatt-build="( cd ~/praatst &&\
        rsync -rptvz $PRAAT_SOURCES/ $PRAAT_EXCLUDES . &&\
        cp makefiles/makefile.defs.linux.pulse_static makefile.defs &&\
        make -j12 )"
    alias praatt="~/praatst/praatt"
    alias praatt-run="praatt-build && praatt"

Building Praat this way takes 2 minutes and 10 seconds (optimization level O3).

To build `praat_barren`, create a folder `praatsb`, and define

    # in Ubuntu:~/.bash_aliases
    alias praatb-build="( cd ~/praatsb &&\
        rsync -rptvz $PRAAT_SOURCES/ $PRAAT_EXCLUDES . &&\
        cp makefiles/makefile.defs.linux.barren makefile.defs &&\
        make -j12 )"
    alias praatb="~/praatsb/praat_barren"
    alias praatb-run="praatb-build && praatb"

You test `praat_barren` briefly by typing

    # on Ubuntu command line
    praatb --version

To build `praat_nogui`, create a folder `praatsn`, and define

    # in Ubuntu:~/.bash_aliases
    alias praatn-build="( cd ~/praatsn &&\
        rsync -rptvz $PRAAT_SOURCES/ $PRAAT_EXCLUDES . &&\
        cp makefiles/makefile.defs.linux.nogui makefile.defs &&\
        make -j12 )"
    alias praatn="~/praatsn/praat_nogui"
    alias praatn-run="praatn-build && praatn"

You test `praat_nogui` briefly by typing

    # on Ubuntu command line
    praatn --version

To build Praat for Chrome64 (64-bit Intel Chromebooks only),
create a folder `praatc`, and define

    # in Ubuntu:~/.bash_aliases
    alias praatc-build="( cd ~/praatsc &&\
        rsync -rptvz $PRAAT_SOURCES/ $PRAAT_EXCLUDES . &&\
        cp makefiles/makefile.defs.chrome64 makefile.defs &&\
        make -j12 )"
    alias praatc="~/praatsc/praat"
    alias praatc-run="praatc-build && praat"

To test Praat for Chrome64, you can just run it on Ubuntu 18.04 by typing `praatc`,
or you transfer it to a Chromebook for the real test.

### 4.4. Chromebook development set-up

Parallels Desktop 15 has no emulator for Chrome, so the choice is between
building Praat on a Chromebook directly or building Praat on Ubuntu 18.04.
On a 2019 HP Chromebook with Intel processor, building Praat takes
a forbidding 27 minutes.

So we choose to build Praat on Ubuntu 18.04 (under Parallels Desktop on the Mac),
because building the Intel Chrome64 edition on Ubuntu 18.04 takes only
2 minutes and 10 seconds. If you have the Linux set-up described in 4.3,
you can do this with the `bc` command.

Next, you need a way to get the executable `praat` from Mac/Ubuntu to your Chromebook.
The distributors of Praat do this via an intermediary university computer;
let’s call this computer-in-the-middle `fon.hum.uva.nl`
(not coincidentally, that’s the name of the computer that hosts `praat.org`).
If you have an account on that computer (say it’s called `yourname`),
then you can access that account with `ssh`, and it is best to do that without 
typing your password each time. To accomplish this, type

    # on Ubuntu command line
    ssh-keygen

on your Ubuntu 18.04. This gives you a file `~/.ssh/id_rsa.pub` on your Ubuntu 18.04,
which contains your public `ssh` key. You should append the contents of this `id_rsa.pub`
to the file `~/.ssh/authorized_keys` on your intermediary computer. From that moment on,
your intermediary computer will accept `rsync -e ssh` calls from your Ubuntu 18.04.
On the intermediary computer, create a folder `~/builds`, and a folder `chrome64` inside that.
If you now define

    # in Ubuntu:~/.bash_aliases
    praatc-put="rsync -tpvz ~/praatsc/praat yourname@fon.hum.uva.nl:~/builds/chrome64"
    praatc-mid="praatc-build && praatc-put"

you can build and send Praat for Chrome to the intermediary computer by just typing

    # on Ubuntu command line
    praatc-mid

On your Chromebook, start up Linux (see the Chromebook download page for details),
create a directory `~/praats` there, and define the following:

    # in Chromebook:~/.bash_aliases
    alias praat-get="( cd ~/praats &&\
        rsync -tpvz yourname@fon.hum.uva.nl:~/builds/chrome64/praat . )"
    alias praat="~/praats/praat"
    alias praat-run="praat-get && praat"

From then on, you can use

    # on Chromebook command line
    praat-run

to fetch Praat from the intermediary computer and run it.

The cycle from editing Praat on the Mac to running it on your Chromebook therefore takes only three steps:

1. edit and save the source code in Xcode on your Mac;
2. type `praatc-mid` on your Ubuntu (under Parallels Desktop on your Mac);
3. type `praat-run` on your Chromebook.

For edits in a `cpp` file (no changes in header files), this whole cycle can be performed within 15 seconds.

### 4.5. Raspberry Pi development set-up

One could perhaps create the Raspberry Pi edition by cross-compiling on Ubuntu 18.04.
If any reader of these lines has precise instructions, we would like to know about it
(the main problem is how to install the GTK etc libraries in the Raspberry Pi toolchain,
or how to get `dpkg` under Ubuntu-buster to actually find `armhf` libraries).

Till then, you build on the Raspberry Pi itself. Your could do that via an intermediary computer
(analogously to what we described above for Chromebook), but you can also do it directly
if you include your Raspberry Pi in the same local network as your Mac and switch on SSH
on your Raspberry Pi (via Raspberry ->  `Preferences` -> `Raspberry Pi Configuration`
-> `Interfaces` -> `SSH` -> `Enable`. You add your Mac’s public SSH key to your Raspberry Pi with

    # on Mac command line
    ssh-keygen   # only if you have no SSH key yet
    ssh-copy-id pi@192.168.1.2   # or whatever your Pi’s static IP address is

On your Raspberry Pi, you create a folder `~/praats`,
after which you can push the sources from your Mac to your Raspberry Pi with

    # in Mac:~/.bash_profile
    PRAAT_SOURCES="~/Praats/src"
    PRAAT_EXCLUDES='--exclude="*.xcodeproj" --exclude="Icon*" --exclude=".*" --exclude="*kanweg*"'
    alias praats-putpi="rsync -rptvz -e ssh $PRAAT_EXCLUDES \
        $PRAAT_SOURCES/ pi@192.168.1.2:~/praats"

On the Raspberry Pi, you define

    # in RaspberryPi:~/.bash_aliases
    alias praat-build="( cd ~/praats &&\
        cp makefiles/makefile.defs.linux.rpi makefile.defs &&\
        make -j4 )"
    alias praat="~/praats/praat"
    alias praat-run="praat-build && praat"

after which you can build and run Praat with

    # on Raspberry Pi command line
    praat-run

Thus, the cycle from editing Praat on the Mac to running it on your Raspberry Pi therefore takes three steps:

1. edit and save the source code in Xcode on your Mac;
2. type `praats-putpi` on your Mac;
3. type `praat-run` on your Raspberry Pi, perhaps via `ssh -X pi@192.168.1.2` in your Mac terminal.

From clean sources this takes around 19 minutes (on a Raspberry Pi 4B),
but if no header files change, then it can be done in approximately 20 seconds.

### 4.6. Distributing Praat

If you want to distribute your version of Praat, you can do so on GitHub and/or on a website
(at least, that’s how the main authors do it). Both of these venues require that you have
all the executables in one place. The guide below refers to the creation of packages
for all platforms for Praat version 9.9.99, although your version number will be different.
The packages will be collected in the directory `~/Praats/www` on the Mac.

If you follow the location mentioned in the `.xcodeproj` file, the Mac binary will reside
in a place like `~/builds/mac_products/Configuration64`.

After notarizing the Mac binary (see above under 3.2),
you include the executable in a `.dmg` disk image, with the following commands:

    # on Mac command line
    PRAAT_WWW=~/Praats/www
    PRAAT_VERSION=9999
    cd ~/builds/mac_products/Configuration64
    hdiutil create -fs HFS+ -ov -srcfolder Praat.app -volname Praat_${PRAAT_VERSION} praat_${PRAAT_VERSION}.dmg
    hdiutil convert -ov -format UDZO -o ${PRAAT_WWW}/praat${PRAAT_VERSION}_mac.dmg praat_${PRAAT_VERSION}.dmg
    rm praat_${PRAAT_VERSION}.dmg

You also need to distribute the `.xcodeproj` file, which is actually a folder, so that you have to zip it:

    # on Mac command line
    PRAAT_SOURCES="~/Praats/src
    cd $PRAAT_SOURCES
    zip -r $PRAAT_WWW/praat$(PRAAT_VERSION)_xcodeproj.zip praat.xcodeproj

The Windows executables have to be sent from your Cygwin terminal to your Mac.
It is easiest to do this without a version number (so that you have to supply the number only once),
so you send them to the intermediate Mac folders `~/builds/win64` and `~/builds/win32`.
On Cygwin you can define:

    # in Cygwin:~/.profile
    alias praat-dist="praat-build && rsync -t ~/praats/Praat.exe /cygdrive/z/builds/win64" 
    alias praat32-dist="praat32-build && rsync -t ~/praats32/Praat.exe /cygdrive/z/builds/win32"

so that you can “upload” the two executables to the Mac with

    # on Cygwin command line
    praat-dist
    praat32-dist

The four Linux executables have to be sent from your Ubuntu terminal to your Mac,
namely to the folders `~/builds/linux64` (which will contain `praat`, `praat_barren` and
`praat_nogui`) and `~/builds/chrome64` (which will contain only `praat`).
On Ubuntu you can define

    # in Ubuntu:~/.bash_aliases
    alias praat-dist="praat-build && rsync -t ~/praats/praat /media/psf/Home/builds/linux64"
    alias praatt-dist="praatt-build && rsync -t ~/praatst/praat_static /media/psf/Home/builds/linux64"
    alias praatb-dist="praatb-build && rsync -t ~/praatsb/praat_barren /media/psf/Home/builds/linux64"
    alias praatn-dist="praatn-build && rsync -t ~/praatsn/praat_nogui /media/psf/Home/builds/linux64"
    alias praatc-dist="praatc-build && rsync -t ~/praatsc/praat /media/psf/Home/builds/chrome64"

so that you can “upload” the five executables to the Mac with

    # on Ubuntu command line
    praat-dist
    praatt-dist
    praatb-dist
    praatn-dist
    praatc-dist

You can fetch the Raspberry Pi edition directly from your Raspberry Pi:

    # on Mac command line
    rsync -tpvz pi@192.168.1.2:~/praats/praat ~/builds/rpi_armv7

When the folders under `~/builds`, namely `win64`, `win32`, `linux64`, `chrome64` and `rpi_armv7`
all contain enough new executables (there should be 1, 1, 3, 1 and 1, respectively),
you can issue the following commands to create the packages and install them in `~/Praats/www`:

    # on Mac command line
    zip $PRAAT_WWW/praat$(PRAAT_VERSION)_win64.zip ~/builds/win64/Praat.exe
    zip $PRAAT_WWW/praat$(PRAAT_VERSION)_win32.zip ~/builds/win32/Praat.exe
    ( cd ~/builds/linux64 &&\
      tar cvf praat$(PRAAT_VERSION)_linux64.tar praat &&\
      gzip praat$(PRAAT_VERSION)_linux64.tar &&\
      mv praat$(PRAAT_VERSION)_linux64.tar.gz $PRAAT_WWW )
    ( cd ~/builds/linux64 &&\
      tar cvf praat$(PRAAT_VERSION)_linux64static.tar praat_static &&\
      gzip praat$(PRAAT_VERSION)_linux64static.tar &&\
      mv praat$(PRAAT_VERSION)_linux64static.tar.gz $PRAAT_WWW )
    ( cd ~/builds/linux64 &&\
      tar cvf praat$(PRAAT_VERSION)_linux64barren.tar praat_barren &&\
      gzip praat$(PRAAT_VERSION)_linux64barren.tar &&\
      mv praat$(PRAAT_VERSION)_linux64barren.tar.gz $PRAAT_WWW )
    ( cd ~/builds/linux64 &&\
      tar cvf praat$(PRAAT_VERSION)_linux64nogui.tar praat_nogui &&\
      gzip praat$(PRAAT_VERSION)_linux64nogui.tar &&\
      mv praat$(PRAAT_VERSION)_linux64nogui.tar.gz $PRAAT_WWW )
    ( cd ~/builds/chrome64 &&\
      tar cvf praat$(PRAAT_VERSION)_chrome64.tar praat &&\
      gzip praat$(PRAAT_VERSION)_chrome64.tar &&\
      mv praat$(PRAAT_VERSION)_chrome64.tar.gz $PRAAT_WWW )
    ( cd ~/builds/rpi_armv7 &&\
      tar cvf praat$(PRAAT_VERSION)_rpi_armv7.tar praat &&\
      gzip praat$(PRAAT_VERSION)_rpi_armv7.tar &&\
      mv praat$(PRAAT_VERSION)_rpi_armv7.tar.gz $PRAAT_WWW )

Finally, you can update your website and/or create a new release on GitHub.
