#ifndef _melder_app_h_
#define _melder_app_h_
/* melder_app.h
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

void Melder_setAppName (conststring32 title);
conststring32 Melder_upperCaseAppName();
conststring32 Melder_lowerCaseAppName();

void Melder_setAppVersion (conststring32 versionText, integer versionNumber);
conststring32 Melder_appVersionSTR();
integer Melder_appVersion();

void Melder_setAppDate (integer year, integer month, integer day);
integer Melder_appYear();
integer Melder_appMonth();
conststring32 Melder_appMonthSTR();
integer Melder_appDay();

void Melder_setAppContactAddress (conststring32 firstPartOfEmailAddress, conststring32 secondPartOfEmailAdress);
		// we don't do a single email address with a "@" in it,
		// because some simplistic malware checkers would then think that our binary contains malware, which it doesn't
conststring32 Melder_appContactAddress();

/* End of file melder_app.h */
#endif
