/* main_Praat.h
 *
 * Copyright (C) 2024 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */


/*
	These things have to be #defines so that they can be used in an automated `praat.plist`.
	To achieve this automation, go to `Build Settings` -> `Packaging` in the Praat Xcode project,
	and then set `Preprocess Info.plist File` to `Yes`
	and `Info.plist Preprocessor Prefix File` to `sys/praat_version.h`.
*/
#define PRAAT_NAME  Praat
#define PRAAT_VERSION_STR  6.4.21alpha3
#define PRAAT_VERSION_NUM  6420
#define PRAAT_YEAR  2024
#define PRAAT_MONTH  9
#define PRAAT_DAY  20

/* End of file main_Praat.h */
