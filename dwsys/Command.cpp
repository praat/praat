/* Command.cpp
 *
 * Copyright (C) 1994-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Command.h"

#pragma mark - class Command

Thing_implement (Command, Thing, 0);

void Command_init (Command me, conststring32 name, Thing boss, Command_Callback execute, Command_Callback undo) {
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

#pragma mark - class CommandHistory

Thing_implement (CommandHistory, Ordered, 0);

void CommandHistory_forth (CommandHistory me) {
	my current ++;
}

void CommandHistory_back (CommandHistory me) {
	my current --;
}

Command CommandHistory_getItem (CommandHistory me) {
	Melder_assert (my current > 0 && my current <= my size);
	return my at [my current];
}

void CommandHistory_insertItem_move (CommandHistory me, autoCommand command) {
	for (integer i = my size; i >= my current + 1; i --)
		my removeItem (i);
	my addItem_move (command.move());
	while (my size > 20)
		my removeItem (1);
	my current = my size;
}

bool CommandHistory_isEmpty (CommandHistory me) {
	return my size == 0;
}

bool CommandHistory_isOffleft (CommandHistory me) {
	return my current == 0;
}

bool CommandHistory_isOffright (CommandHistory me) {
	return my size == 0 || my current == my size + 1;
}

conststring32 CommandHistory_commandName (CommandHistory me, integer offsetFromCurrent) {
	integer pos = my current + offsetFromCurrent;
	return pos >= 1 && pos <= my size ? Thing_getName (my at [pos]) : nullptr;
}

/* End of file Command.cpp */
