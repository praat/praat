ESPEAK_NG_READ_ME.TXT

djmw 20171029

Espeak-ng Version 1.49.3-dev

The Espeak-ng program and its library are the successor of espeak (espeak was maintained by Jonathan Duddington). Espeak-ng is a fork maintained by Reece H. Dunn. We have cloned the git repository:
cd espeak-work
git clone https://github.com/espeak-ng/espeak-ng.git

We use the espeak-data directory as a supply for the data the
synthesizer needs. The synthesizer needs the location of this directory to work correctly. The synthesizer's
version and the espeak-data version have to match.
This is not acceptable in Praat since we don't want these potential mismatches between the internal version
of the synthesizer and the external espeak-data directory to occur at all.
We have therefore "removed" espeak's dependency on the external espeak-data directory by moving all
the data to memory.
This means that some of the espeak code had to be modified a little bit to accomplish this.


***** (only once)

Clone the git  repository 
./autogen.sh
CC=gcc CFLAGS="-Werror=missing-prototypes -Werror=implicit -Wreturn-type -Wunused -Wunused-parameter -Wuninitialized" ./configure --prefix=/usr

Now we can be up-to-date by pulling.

**** 

make

Compile all code and dictonaries.

**** 
We have replaced the file io based on fopen, fclose, fgets etc... with our own io (see espeak_ng_io.cpp)

We inserted a number of expilicy casts:
static_cast<espeak_ng_STATUS> (errno)

Adapted some of the header files.

#include "espeak_ng.h"
#include "speak_lib.h"
#include "encoding.h"
#include "ucd.h"

#undef INCLUDE_MBROLA
#undef PLATFORM_POSIX
#undef PLATFORM_WINDOWS
#undef USE_NANOSLEEP
#define DATA_FROM_SOURCECODE_FILES

****

More details can be found in the "espeak_ng_data_to_code.praat" file.



