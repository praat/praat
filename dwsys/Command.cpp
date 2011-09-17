/* Command.cpp
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20071007 wchar_t
 djmw 20110304 Thing_new
*/

#include "Command.h"

Thing_implement (Command, Thing, 0);

void Command_init (I, const wchar_t *name, Any data, int (*execute) (Any), int (*undo) (Any)) {
	iam (Command);
	Melder_assert (execute && undo);
	Thing_setName (me, name);
	my data = data;
	my execute = execute;
	my undo = undo;
}

int Command_do (I) {
	iam (Command);
	return my execute (me);
}

int Command_undo (I) {
	iam (Command);
	return my undo (me);
}

Thing_implement (CommandHistory, Ordered, 0);

CommandHistory CommandHistory_create (long maximumCapacity) {
	try {
		autoCommandHistory me = Thing_new (CommandHistory);
		Collection_init (me.peek(), classCommand, maximumCapacity);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Command not created.");
	}
}

void CommandHistory_forth (I) {
	iam (CommandHistory);
	my current++;
}

void CommandHistory_back (I) {
	iam (CommandHistory);
	my current--;
}

Any CommandHistory_getItem (I) {
	iam (CommandHistory);
	Melder_assert (my current > 0 && my current <= my size);
	return my item[my current];
}

void CommandHistory_insertItem (I, Any data) {
	iam (CommandHistory);

	Melder_assert (data && (Thing_member ( (Thing) data, my itemClass) || my itemClass == NULL));
	if (my current < my size) {
		for (long i = my current + 1; i <= my size; i++) {
			forget ( ( (Command *) my item) [i]);
		}
		my size = my current;
	}
	if (my size >= my _capacity) {
		Collection_removeItem (me, 1);
	}
	my item[++my size] = data;
	my current = my size;
}

int CommandHistory_empty (I) {
	iam (CommandHistory);
	return my size == 0;
}

int CommandHistory_offleft (I) {
	iam (CommandHistory);
	return my current == 0;
}

int CommandHistory_offright (I) {
	iam (CommandHistory);
	return my size == 0 || my current == my size + 1;
}

wchar_t *CommandHistory_commandName (I, long offsetFromCurrent) {
	iam (CommandHistory);
	long pos = my current + offsetFromCurrent;

	return pos >= 1 && pos <= my size ? Thing_getName ( (Thing) my item[pos]) : NULL;
}

/* End of file Command.cpp */
