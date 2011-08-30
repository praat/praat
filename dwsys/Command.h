#ifndef _Command_h_
#define _Command_h_
/* Command.h
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
 djmw 19950711
 djmw 20020812 GPL header
 djmw 20110306 Latest modification
*/

#include "Thing.h"
#include "Collection.h"

Thing_define (Command, Thing) {
	// new data:
	public:
		Any data;
		int (*execute) (I);
		int (*undo) (I);
};

void Command_init (I, const wchar_t *name, Any data, int (*execute)(Any), int (*undo)(Any));
	
int Command_do (I);

int Command_undo (I);

Thing_define (CommandHistory, Ordered) {
	// new data:
	public:
		long current;
};

/* Active data structure. 'current' is position of the cursor in the list */
/* Queries and insertions are at the current position */
/* Invariants: */
/*	0 <= current <= size + 1; */

CommandHistory CommandHistory_create (long maximumCapacity);

void CommandHistory_forth (I);
/* Precondition: ! offright */
/* my current++; */

void CommandHistory_back (I);
/* Precondition: ! offleft */
/* my current--; */

Any CommandHistory_getItem (I);
/* return (pointer to) my item[my current]; */

void CommandHistory_insertItem (I, Any item);
/* 1. forget about item[ current+1..size ] */
/* 2. insert item after current. */
/* 3. current = size */

int CommandHistory_empty (I);
/*	return my size == 0; */

int CommandHistory_offleft (I);
/*	return my current == 0; */

int CommandHistory_offright (I);
/*	return my size == 0 || my current == my size + 1; */

wchar_t *CommandHistory_commandName (I, long offsetFromCurrent);
/* offsetFromCurrent may be zero, positive or negative. */
/* References outside the list will return NULL. */

#endif /* _Command_h_ */
