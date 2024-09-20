/*
	These things have to be #defines so that they can be used in an automated `praat.plist`.

	So please don't try to move these #defines to a Melder_setAppVersion() in main.cpp, for instance.

	To achieve the automation, go to `Build Settings` -> `Packaging` in the Praat Xcode project,
	and then set `Preprocess Info.plist File` to `Yes`
	and `Info.plist Preprocessor Prefix File` to `sys/praat_version.h`.
*/
#define PRAAT_VERSION_STR 6.4.20
#define PRAAT_VERSION_NUM 6420
#define PRAAT_YEAR 2024
#define PRAAT_MONTH September
#define PRAAT_DAY 18
