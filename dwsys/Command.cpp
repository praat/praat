/* Command.cpp
 *
 * Copyright (C) 1994-2011, 2015 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 19950710
 djmw 20020812 GPL header
 djmw 20071007 wchar
 djmw 20110304 Thing_new
*/

#include "Command.h"

Thing_implement (Command, Thing, 0);

void Command_init (Command me, const char32 *name, Thing boss, Command_Callback execute, Command_Callback undo) {
	Melder_assert (execute && undo);
	Thing_setName (me, name);
	my boss = boss;
	my execute = execute;
	my undo = undo;
}

int Command_do (Command me) {
	return my execute (me);
}

int Command_undo (Command me) {
	return my undo (me);
}

Thing_implement (CommandHistory, Ordered, 0);

autoCommandHistory CommandHistory_create (long maximumCapacity) {
	try {
		autoCommandHistory me = Thing_new (CommandHistory);
		Collection_init (me.peek(), classCommand, maximumCapacity);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Command not created.");
	}
}

void CommandHistory_forth (CommandHistory me) {
	my current++;
}

void CommandHistory_back (CommandHistory me) {
	my current--;
}

Command CommandHistory_getItem (CommandHistory me) {
	Melder_assert (my current > 0 && my current <= my size);
	return static_cast<Command> (my item [my current]);
}

void CommandHistory_insertItem (CommandHistory me, Command command)
{
	Melder_assert (command && (Thing_isa (command, my itemClass) || my itemClass == nullptr));
	if (my current < my size) {
		for (long i = my current + 1; i <= my size; i++) {
			forget ( ( (Command *) my item) [i]);
		}
		my size = my current;
	}
	if (my size >= my _capacity) {
		Collection_removeItem (me, 1);
	}
	my item[++my size] = command;
	my current = my size;
}

int CommandHistory_empty (CommandHistory me) {
	return my size == 0;
}

int CommandHistory_offleft (CommandHistory me) {
	return my current == 0;
}

int CommandHistory_offright (CommandHistory me) {
	return my size == 0 || my current == my size + 1;
}

char32 *CommandHistory_commandName (CommandHistory me, long offsetFromCurrent) {
	long pos = my current + offsetFromCurrent;
	return pos >= 1 && pos <= my size ? Thing_getName ( (Thing) my item[pos]) : nullptr;
}

/* End of file Command.cpp */
