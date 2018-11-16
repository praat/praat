#ifndef _Command_h_
#define _Command_h_
/* Command.h
 *
 * Copyright (C) 1994-2018 David Weenink, 2015 Paul Boersma
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

#include "Thing.h"
#include "Collection.h"


#pragma mark - class Command

Thing_declare (Command);

typedef MelderCallback<int, structCommand> Command_Callback;

Thing_define (Command, Thing) {
	Thing boss;
	Command_Callback execute;
	Command_Callback undo;
};

void Command_init (Command me, conststring32 name, Thing boss, Command_Callback execute, Command_Callback undo);

int Command_do (Command me);

int Command_undo (Command me);


#pragma mark - class CommandHistory

Collection_define (CommandHistory, OrderedOf, Command) {
	integer current;
};

/* Active data structure. 'current' is position of the cursor in the list */
/* Queries and insertions are at the current position */
/* Invariants: */
/*	0 <= current <= size + 1; */

void CommandHistory_forth (CommandHistory me);
/* Precondition: ! offright */
/* my current++; */

void CommandHistory_back (CommandHistory me);
/* Precondition: ! offleft */
/* my current--; */

Command CommandHistory_getItem (CommandHistory me);
/* return (pointer to) my item[my current]; */

void CommandHistory_insertItem_move (CommandHistory me, autoCommand command);
/* 1. forget about item[ current+1..size ] */
/* 2. insert item after current. */
/* 3. current = size */

bool CommandHistory_isEmpty (CommandHistory me);
/*	return my size == 0; */

bool CommandHistory_isOffleft (CommandHistory me);
/*	return my current == 0; */

bool CommandHistory_isOffright (CommandHistory me);
/*	return my size == 0 || my current == my size + 1; */

conststring32 CommandHistory_commandName (CommandHistory me, integer offsetFromCurrent);
/* offsetFromCurrent may be zero, positive or negative. */
/* References outside the list will return nullptr. */

#endif /* _Command_h_ */
