/* praat_library.cpp
 *
 * Copyright (C) 2016,2018,2021,2024 Paul Boersma
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

#include "praatP.h"

void praat_library_createC (bool isInHeaderFile, bool includeCreateAPI, bool includeReadAPI, bool includeSaveAPI,
	bool includeQueryAPI, bool includeModifyAPI, bool includeToAPI,
	bool includeRecordAPI, bool includePlayAPI,
	bool includeDrawAPI, bool includeHelpAPI, bool includeWindowAPI,
	bool /* includeDemoAPI */)
{
	try {
		MelderInfo_open ();
		MelderInfo_writeLine (U"/* praatlib.h");
		MelderInfo_writeLine (U" *");
		MelderInfo_writeLine (U" * Copyright (C) 2016-2018 Paul Boersma and David Weenink");
		MelderInfo_writeLine (U" *");
		MelderInfo_writeLine (U" * This code is free software; you can redistribute it and/or modify");
		MelderInfo_writeLine (U" * it under the terms of the GNU General Public License as published by");
		MelderInfo_writeLine (U" * the Free Software Foundation; either version 3 of the License, or (at");
		MelderInfo_writeLine (U" * your option) any later version.");
		MelderInfo_writeLine (U" *");
		MelderInfo_writeLine (U" * This code is distributed in the hope that it will be useful, but");
		MelderInfo_writeLine (U" * WITHOUT ANY WARRANTY; without even the implied warranty of");
		MelderInfo_writeLine (U" * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
		MelderInfo_writeLine (U" * See the GNU General Public License for more details.");
		MelderInfo_writeLine (U" *");
		MelderInfo_writeLine (U" * You should have received a copy of the GNU General Public License");
		MelderInfo_writeLine (U" * along with this work. If not, see <http://www.gnu.org/licenses/>.");
		MelderInfo_writeLine (U" */");

		MelderInfo_writeLine (U"/* C API, version ", Melder_appMonthSTR(), U" ", Melder_appDay(), U", ", Melder_appYear(), U" */");

		praat_menuCommands_writeC (true, includeCreateAPI, includeReadAPI,
			includeRecordAPI, includePlayAPI, includeDrawAPI, includeHelpAPI, includeWindowAPI);
		praat_actions_writeC (true, includeSaveAPI,
			includeQueryAPI, includeModifyAPI, includeToAPI,
			includePlayAPI, includeDrawAPI, includeHelpAPI, includeWindowAPI);
		MelderInfo_close ();
	} catch (MelderError) {
		Melder_throw (U"C library not created.");
	}
}

/* End of file praat_statistics.cpp */
