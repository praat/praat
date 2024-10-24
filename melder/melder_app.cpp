/* melder_app.cpp
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

#include "melder.h"

static autostring32 Melder_toUpperCamelCase (conststring32 string) {
	autostring32 result (Melder_length (string) + 1);
	char32 *q = & result [0];
	bool nextLetterShouldBeUpperCase = true;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		if (nextLetterShouldBeUpperCase) {
			*q ++ = Melder_toUpperCase (*p);
			nextLetterShouldBeUpperCase = false;
		} else if (*p == U'_') {
			nextLetterShouldBeUpperCase = true;
		} else
			*q ++ = *p;
	}
	*q = U'\0';   // closing null byte
	return result;
}

static autostring32 Melder_toLowerCamelCase (conststring32 string) {
	autostring32 result (Melder_length (string) + 1);
	char32 *q = & result [0];
	bool nextLetterShouldBeUpperCase = false;
	bool nextLetterShouldBeLowerCase = true;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		if (nextLetterShouldBeUpperCase) {
			*q ++ = Melder_toUpperCase (*p);
			nextLetterShouldBeUpperCase = false;
		} else if (nextLetterShouldBeLowerCase) {
			*q ++ = Melder_toLowerCase (*p);
			nextLetterShouldBeLowerCase = false;
		} else if (*p == U'_') {
			nextLetterShouldBeUpperCase = true;
		} else
			*q ++ = *p;
	}
	*q = U'\0';   // closing null byte
	return result;
}

static autostring32 Melder_toLowerSnakeCase (conststring32 string) {
	autostring32 result (2 * Melder_length (string) + 1);   // make room for extra underscores
	char32 *q = & result [0];
	bool nextLetterShouldBeLowerCase = true;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		if (nextLetterShouldBeLowerCase) {
			*q ++ = Melder_toLowerCase (*p);
			nextLetterShouldBeLowerCase = false;
		} else if (Melder_isUpperCaseLetter (*p) ) {
			*q ++ = U'_';
			*q ++ = Melder_toLowerCase (*p);
			nextLetterShouldBeLowerCase = true;
		} else
			*q ++ = *p;
	}
	*q = U'\0';   // closing null byte
	return result;
}

static autostring32 Melder_toUpperSnakeCase (conststring32 string) {
	autostring32 result = Melder_toLowerSnakeCase (string);
	bool nextLetterShouldBeUpperCase = true;
	for (char32 *p = & result [0]; *p != U'\0'; p ++) {
		if (nextLetterShouldBeUpperCase) {
			*p = Melder_toUpperCase (*p);
			nextLetterShouldBeUpperCase = false;
		} else if (*p == U'_') {
			nextLetterShouldBeUpperCase = true;
		}
	}
	return result;
}

static autostring32 theUpperCaseAppName, theLowerCaseAppName;
void Melder_setAppName (conststring32 appName) {
	theUpperCaseAppName = Melder_toUpperCamelCase (appName);
	theLowerCaseAppName = Melder_toLowerSnakeCase (appName);
}
conststring32 Melder_upperCaseAppName() {
	return theUpperCaseAppName.get();
}
conststring32 Melder_lowerCaseAppName() {
	return theLowerCaseAppName.get();
}

static autostring32 theAppVersionText;
static integer theAppVersionNumber;
void Melder_setAppVersion (conststring32 versionText, integer versionNumber) {
	theAppVersionText = Melder_dup (versionText);
	theAppVersionNumber = versionNumber;
}
conststring32 Melder_appVersionSTR() {
	return theAppVersionText.get();
}
integer Melder_appVersion() {
	return theAppVersionNumber;
}

static integer theAppYear, theAppMonth, theAppDay;
void Melder_setAppDate (integer year, integer month, integer day) {
	theAppYear = year;
	theAppMonth = month;
	theAppDay = day;
}
integer Melder_appYear() {
	return theAppYear;
}
integer Melder_appMonth() {
	return theAppMonth;
}
conststring32 Melder_appMonthSTR() {
	static conststring32 monthNames [12] = {
		U"January", U"February", U"March", U"April", U"May", U"June",
		U"July", U"August", U"September", U"October", U"November", U"December"
	};
	Melder_assert (theAppMonth > 0 && theAppMonth <= 12);
	return monthNames [theAppMonth-1];
}
integer Melder_appDay() {
	return theAppDay;
}

static autostring32 theAppAuthorContactAddress;
void Melder_setAppAuthorContactAddress (conststring32 firstPartOfEmailAddress, conststring32 secondPartOfEmailAdress) {
	theAppAuthorContactAddress = Melder_dup (Melder_cat (firstPartOfEmailAddress, U"@", secondPartOfEmailAdress));
}
conststring32 Melder_appAuthorContactAddress() {
	return theAppAuthorContactAddress.get();
}

/* End of file melder_app.cpp */
