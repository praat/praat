/* praat_menuCommands.cpp
 *
 * Copyright (C) 1992-2018 Paul Boersma
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
#include "praat_script.h"
#include "praat_version.h"
#include "GuiP.h"

static OrderedOf <structPraat_Command> theCommands;
void praat_menuCommands_exit_optimizeByLeaking () { theCommands. _ownItems = false; }

void praat_menuCommands_init () {
}

static int compareMenuCommands (const void *void_me, const void *void_thee) {
	Praat_Command me = * (Praat_Command *) void_me, thee = * (Praat_Command *) void_thee;
	if (my window) {
		if (! thy window) return 1;
		int compare = str32cmp (my window.get(), thy window.get());
		if (compare) return compare;
	} else if (thy window) return -1;
	if (my menu) {
		if (! thy menu) return 1;
		int compare = str32cmp (my menu.get(), thy menu.get());
		if (compare) return compare;
	} else if (thy menu) return -1;
	if (my sortingTail < thy sortingTail) return -1;
	return 1;
}

void praat_sortMenuCommands () {
	for (integer i = 1; i <= theCommands.size; i ++) {
		Praat_Command command = theCommands.at [i];
		command -> sortingTail = i;
	}
	qsort (& theCommands.at [1], theCommands.size, sizeof (Praat_Command), compareMenuCommands);
}

static integer lookUpMatchingMenuCommand (conststring32 window, conststring32 menu, conststring32 title) {
/*
 * A menu command is fully specified by its environment (window + menu) and its title.
 */
	for (integer i = 1; i <= theCommands.size; i ++) {
		Praat_Command command = theCommands.at [i];
		conststring32 tryWindow = command -> window.get();
		conststring32 tryMenu = command -> menu.get();
		conststring32 tryTitle = command -> title.get();
		if ((window == tryWindow || (window && tryWindow && str32equ (window, tryWindow))) &&
		    (menu == tryMenu || (menu && tryMenu && str32equ (menu, tryMenu))) &&
		    (title == tryTitle || (title && tryTitle && str32equ (title, tryTitle)))) return i;
	}
	return 0;   // not found
}

static void do_menu (Praat_Command me, uint32 modified) {
	if (my callback == DO_RunTheScriptFromAnyAddedMenuCommand) {
		UiHistory_write (U"\nrunScript: ");
		try {
			DO_RunTheScriptFromAnyAddedMenuCommand (nullptr, 0, nullptr, my script.get(), nullptr, nullptr, false, nullptr);
		} catch (MelderError) {
			Melder_flushError (U"Command \"", my title.get(), U"\" not executed.");
		}
		praat_updateSelection ();
	} else {
		if (my title && ! str32str (my title.get(), U"...")) {
			UiHistory_write (U"\n");
			UiHistory_write (my title.get());
		}
		try {
			my callback (nullptr, 0, nullptr, nullptr, nullptr, my title.get(), modified, nullptr);
		} catch (MelderError) {
			Melder_flushError (U"Command \"", my title.get(), U"\" not executed.");
		}
		praat_updateSelection ();
	}
}

static void gui_button_cb_menu (Praat_Command me, GuiButtonEvent event) {
	do_menu (me, event -> shiftKeyPressed | event -> commandKeyPressed | event -> optionKeyPressed | event -> extraControlKeyPressed);
}

static void gui_cb_menu (Praat_Command me, GuiMenuItemEvent event) {
	bool modified = event -> shiftKeyPressed || event -> commandKeyPressed || event -> optionKeyPressed || event -> extraControlKeyPressed;
	do_menu (me, modified);
}

static GuiMenu windowMenuToWidget (conststring32 window, conststring32 menu) {
	return
		str32equ (window, U"Picture") ? praat_picture_resolveMenu (menu) :
		str32equ (window, U"Objects") ? praat_objects_resolveMenu (menu) : nullptr;
}

GuiMenuItem praat_addMenuCommand_ (conststring32 window, conststring32 menu, conststring32 title /* cattable */,
	conststring32 after, uint32 flags, UiCallback callback, conststring32 nameOfCallback)
{
	integer position;
	int depth = flags, key = 0;
	bool unhidable = false, hidden = false, noApi = false, forceApi = false;
	int deprecationYear = 0;
	uint32 guiFlags = 0;
	if (flags > 7) {
		depth = ((flags & praat_DEPTH_7) >> 16);
		unhidable = (flags & praat_UNHIDABLE) != 0;
		hidden = (flags & praat_HIDDEN) != 0 && ! unhidable;
		key = flags & 0x000000FF;
		noApi = (flags & praat_NO_API) != 0;
		forceApi = (flags & praat_FORCE_API) != 0;
		deprecationYear = (flags & praat_DEPRECATED) == praat_DEPRECATED ? 2000 + (flags >> 24) : 0;
		guiFlags = key ? flags & (0x000000FF | GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_BUTTON_STATE_MASK) : flags & GuiMenu_BUTTON_STATE_MASK;
	}
	if (callback && ! title) {
		Melder_flushError (U"praat_addMenuCommand: command with callback has no title. Window \"", window, U"\", menu \"", menu, U"\".");
		return nullptr;
	}

	/*
	 * Determine the position of the new command.
	 */
	if (after && after [0] != U'*') {   // search for existing command with same selection
		integer found = lookUpMatchingMenuCommand (window, menu, after);
		if (found) {
			position = found + 1;   // after 'after'
		} else {
			Melder_flushError (U"praat_addMenuCommand: the command \"", title, U"\" cannot be put after \"", after, U"\",\n"
				U"in the menu \"", menu, U"\" in the window \"", window, U"\"\n"
				U"because the latter command does not exist.");
			return nullptr;
		}
	} else {
		position = theCommands.size + 1;   // at end
	}

	/*
	 * Make new command.
	 */
	autoPraat_Command command = Thing_new (Praat_Command);

	command -> window = Melder_dup_f (window);
	command -> menu = Melder_dup_f (menu);
	command -> title = Melder_dup_f (title);
	trace (U"insert new command \"", title, U"\"");
	command -> depth = depth;
	command -> callback = callback;   // null for a separator or cascade button
	command -> nameOfCallback = nameOfCallback;
	command -> executable = !! callback;
	command -> script = autostring32();
	command -> hidden = hidden;
	command -> unhidable = unhidable;
	command -> deprecationYear = deprecationYear;
	command -> noApi = noApi;
	command -> forceApi = forceApi;

	if (! theCurrentPraatApplication -> batch) {
		GuiMenu parentMenu = nullptr;

		/* WHERE TO PUT IT?
		 * Determine parent menu widget.
		 * This is not going to fail:
		 * if 'depth' is inappropriate, the alleged subitem will be put in the top menu.
		 */
		if (depth == 0) {
			/*
			 * Put the new command in the window's top menu.
			 */
			parentMenu = windowMenuToWidget (window, menu);
		} else {
			/*
			 * Put the new command in a submenu.
			 * The supermenu to put the new command in is the first menu that we find when going up.
			 */
			for (integer parentPosition = position - 1; parentPosition > 0; parentPosition --) {
				Praat_Command parentCommand = theCommands.at [parentPosition];
				if (parentCommand -> depth == depth - 1) {
					/*
					 * We found the supermenu.
					 */
					if (! parentCommand -> callback && parentCommand -> title && parentCommand -> title [0] != U'-') {
						if (! parentCommand -> button)
							Melder_fatal (U"No button for ", window, U"/", menu, U"/", title, U".");
						Thing_cast (GuiMenuItem, parentButton_as_GuiMenuItem, parentCommand -> button);
						parentMenu = parentButton_as_GuiMenuItem -> d_menu;
					}
					break;
				}
			}
			if (! parentMenu) parentMenu = windowMenuToWidget (window, menu);   // fallback: put the command in the window's top menu
		}
		if (! parentMenu) {
			trace (U"WARNING: no parent menu for ", window, U"/", menu, U"/", title, U".");
			return nullptr;
		}

		/*
		 * WHAT TO PUT THERE?
		 */

		if (! title || title [0] == U'-') {
			trace (U"insert the command as a separator");
			command -> button = GuiMenu_addSeparator (parentMenu);
			Melder_assert (command -> button);
		} else if (! callback) {
			trace (U"insert the command as a submenu");
			command -> button = GuiMenu_createInMenu (parentMenu, title, 0) -> d_menuItem.get();
			Melder_assert (command -> button);
		} else {
			trace (U"insert the command as a normal menu item");
			command -> button = GuiMenu_addItem (parentMenu, title, guiFlags, gui_cb_menu, command.get());
			Melder_assert (command -> button);
		}
		if (hidden) GuiThing_hide (command -> button);
	}
	Thing_cast (GuiMenuItem, button_as_GuiMenuItem, command -> button);
	theCommands. addItemAtPosition_move (command.move(), position);
	return button_as_GuiMenuItem;
}

void praat_addMenuCommandScript (conststring32 window, conststring32 menu, conststring32 title,
	conststring32 after, integer depth, conststring32 script)
{
	try {
		Melder_assert (window && menu && title && after && script);
		if (script [0] != U'\0' && title [0] == U'\0')
			Melder_throw (U"Command with script has no title. Window \"", window, U"\", menu \"", menu, U"\".");

		/*
		 * Determine the position of the new command.
		 */
		integer position;
		if (str32len (after) && after [0] != U'*') {   // search for existing command with same selection
			integer found = lookUpMatchingMenuCommand (window, menu, after);
			if (found) {
				position = found + 1;   // after 'after'
			} else {
				/*Melder_throw (U"The menu command \"", title, U"\" cannot be put after \"", after, U"\",\n"
					U"in the menu \"", menu, "\" in the window \"", window, U"\"\n"
					U"because the latter command does not exist.");*/
				position = theCommands.size + 1;   // default: at end
			}
		} else {
			position = theCommands.size + 1;   // at end
		}

		/*
		 * Make new command.
		 */
		autoPraat_Command command = Thing_new (Praat_Command);
		command -> window = Melder_dup_f (window);
		command -> menu = Melder_dup_f (menu);
		command -> title = ( title [0] != U'\0' ? Melder_dup_f (title) : autostring32() );   // allow old-fashioned untitled separators
		command -> depth = depth;
		command -> callback = ( script [0] != U'\0' ? DO_RunTheScriptFromAnyAddedMenuCommand : nullptr );   // null for a separator or cascade button
		command -> executable = ( script [0] != U'\0' );
		command -> noApi = true;
		if (script [0] == U'\0') {
			command -> script = Melder_dup_f (U"");   // empty string, which will be needed to signal origin
		} else {
			structMelderFile file { };
			Melder_relativePathToFile (script, & file);
			command -> script = Melder_dup_f (Melder_fileToPath (& file));
		}
		command -> after = ( after [0] != U'\0' ? Melder_dup_f (after) : autostring32() );
		if (praatP.phase >= praat_READING_BUTTONS) {
			static integer uniqueID = 0;
			command -> uniqueID = ++ uniqueID;
		}

		if (! theCurrentPraatApplication -> batch) {
			GuiMenu parentMenu = nullptr;

			/* WHERE TO PUT IT?
			 * Determine parent menu widget.
			 * This is not going to fail:
			 * if 'depth' is inappropriate, the alleged subitem will be put in the top menu.
			 */
			if (depth == 0) {
				parentMenu = windowMenuToWidget (window, menu);   // not a subitem: in the top menu
			} else {
				for (integer parentPosition = position - 1; parentPosition > 0; parentPosition --) {
					Praat_Command parentCommand = theCommands.at [parentPosition];
					if (parentCommand -> depth == depth - 1) {
						if (! parentCommand -> callback && parentCommand -> title && parentCommand -> title [0] != U'-') {
							if (! parentCommand -> button)
								Melder_fatal (U"No button for ", window, U"/", menu, U"/", title, U".");
							Melder_assert (parentCommand -> button -> classInfo == classGuiMenuItem);
							parentMenu = (static_cast <GuiMenuItem> (parentCommand -> button)) -> d_menu;
						}
						break;
					}
				}
				if (! parentMenu) parentMenu = windowMenuToWidget (window, menu);   // fallback: a subitem without a menu title
			}
			if (parentMenu) {
				/* WHAT TO PUT THERE?
				 */
				if (title [0] == U'\0' || title [0] == U'-') {
					command -> button = GuiMenu_addSeparator (parentMenu);
				} else if (script [0] == '\0') {
					command -> button = GuiMenu_createInMenu (parentMenu, title, 0) -> d_menuItem.get();
				} else {
					command -> button = GuiMenu_addItem (parentMenu, title, 0, gui_cb_menu, command.get());
				}
			}
		}
		theCommands. addItemAtPosition_move (command.move(), position);

		if (praatP.phase >= praat_HANDLING_EVENTS) praat_sortMenuCommands ();
	} catch (MelderError) {
		Melder_throw (U"Script menu command not added.");
	}
}

void praat_hideMenuCommand (conststring32 window, conststring32 menu, conststring32 title) {
	if (theCurrentPraatApplication -> batch || ! window || ! menu || ! title) return;
	integer found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return;
	Praat_Command command = theCommands.at [found];
	if (! command -> hidden && ! command -> unhidable) {
		command -> hidden = true;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) GuiThing_hide (command -> button);
	}
}

void praat_showMenuCommand (conststring32 window, conststring32 menu, conststring32 title) {
	if (theCurrentPraatApplication -> batch || ! window || ! menu || ! title) return;
	integer found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return;
	Praat_Command command = theCommands.at [found];
	if (command -> hidden) {
		command -> hidden = false;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) GuiThing_show (command -> button);
	}
}

void praat_saveAddedMenuCommands (MelderString *buffer) {
	/*
		The procedure as it is now, runs in M*N time,
		where M is the number of added commands and N is the total number of commands.
		Sorting first instead runs in N log N time and will therefore be faster if M >> log N ≈ 10.
	*/
	integer maxID = 0;
	for (integer i = 1; i <= theCommands.size; i ++) {
		Praat_Command command = theCommands.at [i];
		if (command -> uniqueID > maxID)
			maxID = command -> uniqueID;
	}
	for (integer id = 1; id <= maxID; id ++)   // sorted
		for (integer i = 1; i <= theCommands.size; i ++) {
			Praat_Command me = theCommands.at [i];
			if (my uniqueID == id && ! my hidden && my window && my menu && my title) {
				MelderString_append (buffer, U"Add menu command... \"", my window.get(), U"\" \"", my menu.get(), U"\" \"", my title.get(), U"\" \"",
					( my after ? my after.get() : U"" ), U"\" ", my depth, U" ", ( my script ? my script.get() : U"" ), U"\n");
				break;
			}
		}
}

void praat_saveToggledMenuCommands (MelderString *buffer) {
	for (integer i = 1; i <= theCommands.size; i ++) {
		Praat_Command me = theCommands.at [i];
		if (my toggled && my window && my menu && my title && ! my uniqueID && ! my script)
			MelderString_append (buffer, my hidden ? U"Hide" : U"Show", U" menu command... \"",
				my window.get(), U"\" \"", my menu.get(), U"\" ", my title.get(), U"\n");
	}
}

/***** FIXED BUTTONS *****/

void praat_addFixedButtonCommand_ (GuiForm parent, conststring32 title, UiCallback callback, conststring32 nameOfCallback, int x, int y) {
	autoPraat_Command me = Thing_new (Praat_Command);
	my window = Melder_dup_f (U"Objects");
	my title = Melder_dup_f (title);
	my callback = callback;
	my nameOfCallback = nameOfCallback;
	my unhidable = true;
	my noApi = ( str32equ (title, U"Inspect") );
	if (theCurrentPraatApplication -> batch) {
		my button = nullptr;
	} else {
		GuiThing button = my button = GuiButton_create (parent, x, x + 82, -y - Gui_PUSHBUTTON_HEIGHT, -y,
			title, gui_button_cb_menu, me.get(), 0);
		GuiThing_setSensitive (button, false);
		GuiThing_show (button);
	}
	my executable = false;
	theCommands. addItemAtPosition_move (me.move(), 0);
}

void praat_sensitivizeFixedButtonCommand (conststring32 title, bool sensitive) {
	Praat_Command commandFound = nullptr;
	for (integer i = 1; i <= theCommands.size; i ++) {
		Praat_Command command = theCommands.at [i];
		if (str32equ (command -> title.get(), title)) {
			commandFound = command;
			break;
		}
	}
	if (! commandFound) Melder_fatal (U"Unkown fixed button <<", title, U">>");
	commandFound -> executable = sensitive;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding)
		GuiThing_setSensitive (commandFound -> button, sensitive);
}

int praat_doMenuCommand (conststring32 title, conststring32 arguments, Interpreter interpreter) {
	Praat_Command commandFound = nullptr;
	for (integer i = 1; i <= theCommands.size; i ++) {
		Praat_Command command = theCommands.at [i];
		if (command -> executable && str32equ (command -> title.get(), title) &&
			(str32equ (command -> window.get(), U"Objects") || str32equ (command -> window.get(), U"Picture")))
		{
			commandFound = command;
			break;
		}
	}
	if (! commandFound) return 0;
	commandFound -> callback (nullptr, 0, nullptr, arguments, interpreter, title, false, nullptr);
	return 1;
}

int praat_doMenuCommand (conststring32 title, integer narg, Stackel args, Interpreter interpreter) {
	Praat_Command commandFound = nullptr;
	for (integer i = 1; i <= theCommands.size; i ++) {
		Praat_Command command = theCommands.at [i];
		if (command -> executable && str32equ (command -> title.get(), title) &&
			(str32equ (command -> window.get(), U"Objects") || str32equ (command -> window.get(), U"Picture")))
		{
			commandFound = command;
			break;
		}
	}
	if (! commandFound) return 0;
	commandFound -> callback (nullptr, narg, args, nullptr, interpreter, title, false, nullptr);
	return 1;
}

integer praat_getNumberOfMenuCommands () { return theCommands.size; }

Praat_Command praat_getMenuCommand (integer i)
	{ return i < 1 || i > theCommands.size ? nullptr : theCommands.at [i]; }

void praat_addCommandsToEditor (Editor me) {
	conststring32 windowName = my classInfo -> className;
	for (integer i = 1; i <= theCommands.size; i ++) {
		Praat_Command command = theCommands.at [i];
		if (str32equ (command -> window.get(), windowName)) {
			Editor_addCommandScript (me, command -> menu.get(), command -> title.get(), 0, command -> script.get());
		}
	}
}

static bool commandIsToBeIncluded (Praat_Command command, bool deprecated, bool includeCreateAPI, bool includeReadAPI,
	bool includeRecordAPI, bool includePlayAPI, bool includeDrawAPI, bool includeHelpAPI, bool includeWindowAPI)
{
	bool obsolete = ( deprecated && (command -> deprecationYear < PRAAT_YEAR - 10 || command -> deprecationYear < 2017) );
	bool hiddenByDefault = ( command -> hidden != command -> toggled );
	bool explicitlyHidden = hiddenByDefault && ! deprecated;
	bool hidden = explicitlyHidden || ! command -> callback || command -> noApi || obsolete ||
		(! includeWindowAPI && Melder_nequ (command -> nameOfCallback, U"WINDOW_", 7)) ||
		(! includeHelpAPI && Melder_nequ (command -> nameOfCallback, U"HELP_", 5)) ||
		(! includeDrawAPI && Melder_nequ (command -> nameOfCallback, U"GRAPHICS_", 9)) ||
		(! includePlayAPI && Melder_nequ (command -> nameOfCallback, U"PLAY_", 5)) ||
		(! includeRecordAPI && Melder_nequ (command -> nameOfCallback, U"RECORD_", 7)) ||
		(! includeReadAPI && Melder_nequ (command -> nameOfCallback, U"READ_", 5)) ||
		(! includeReadAPI && Melder_nequ (command -> nameOfCallback, U"READ1_", 6)) ||
		(! includeCreateAPI && Melder_nequ (command -> nameOfCallback, U"NEW1_", 5));
	return command -> forceApi || ! hidden;
}

static bool commandHasFileNameArgument (Praat_Command command) {
	bool hasFileNameArgument =
		Melder_nequ (command -> nameOfCallback, U"READ1_", 6) ||
		Melder_nequ (command -> nameOfCallback, U"SAVE_", 5)
	;
	return hasFileNameArgument;
}

static conststring32 getReturnType (Praat_Command command) {
	const conststring32 returnType =
		Melder_nequ (command -> nameOfCallback, U"NEW1_", 5) ? U"PraatObject" :
		Melder_nequ (command -> nameOfCallback, U"READ1_", 6) ? U"PraatObject" :
		Melder_nequ (command -> nameOfCallback, U"REAL_", 5) ? U"double" :
		Melder_nequ (command -> nameOfCallback, U"INTEGER_", 8) ? U"int64_t" :
		Melder_nequ (command -> nameOfCallback, U"STRING_", 7) ? U"char *" :
		Melder_nequ (command -> nameOfCallback, U"REPORT_", 7) ? U"char *" :
		Melder_nequ (command -> nameOfCallback, U"LIST_", 5) ? U"char *" :
		Melder_nequ (command -> nameOfCallback, U"INFO_", 5) ? U"char *" :
		Melder_nequ (command -> nameOfCallback, U"HINT_", 5) ? U"char *" :
		U"void";
	return returnType;
}

void praat_menuCommands_writeC (bool isInHeaderFile, bool includeCreateAPI, bool includeReadAPI,
	bool includeRecordAPI, bool includePlayAPI, bool includeDrawAPI, bool includeHelpAPI, bool includeWindowAPI)
{
	integer numberOfApiMenuCommands = 0;
	for (integer i = 1; i <= theCommands.size; i ++) {
		Praat_Command command = theCommands.at [i];
		bool deprecated = ( command -> deprecationYear > 0 );
		if (! commandIsToBeIncluded (command, deprecated, includeCreateAPI, includeReadAPI,
			includeRecordAPI, includePlayAPI, includeDrawAPI, includeHelpAPI, includeWindowAPI)) continue;
		MelderInfo_writeLine (U"\n/* Menu command \"", command -> title.get(), U"\"",
			deprecated ? U", deprecated " : U"", deprecated ? Melder_integer (command -> deprecationYear) : U"",
			U" */");
		conststring32 returnType = getReturnType (command);
		MelderInfo_writeLine (returnType, U" Praat", str32chr (command -> nameOfCallback, U'_'), U" (");
		bool isDirect = ! str32str (command -> title.get(), U"...");
		if (isDirect) {
		} else {
			command -> callback (nullptr, -1, nullptr, nullptr, nullptr, nullptr, false, nullptr);
		}
		if (commandHasFileNameArgument (command)) {
			MelderInfo_writeLine (U"\tconst char *fileName");
		}
		MelderInfo_write (U")");
		if (isInHeaderFile) {
			MelderInfo_writeLine (U";");
		} else {
			MelderInfo_writeLine (U" {");
			MelderInfo_writeLine (U"}");
		}
		numberOfApiMenuCommands += 1;
	}
}

/* End of file praat_menuCommands.cpp */
