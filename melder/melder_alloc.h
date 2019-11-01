#ifndef _melder_alloc_h_
#define _melder_alloc_h_
/* melder_alloc.h
 *
 * Copyright (C) 1992-2019 Paul Boersma
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

/* These functions call malloc, free, realloc, and calloc. */
/* If out of memory, the non-f versions throw an error message (like "Out of memory"); */
/* the f versions open up a rainy day fund or crash Praat. */
/* These functions also maintain a count of the total number of blocks allocated. */

void Melder_alloc_init ();   // to be called around program start-up
void Melder_message_init ();   // to be called around program start-up
void * _Melder_malloc (int64 size);
#define Melder_malloc(type,numberOfElements)  (type *) _Melder_malloc ((numberOfElements) * (int64) sizeof (type))
void * _Melder_malloc_f (int64 size);
#define Melder_malloc_f(type,numberOfElements)  (type *) _Melder_malloc_f ((numberOfElements) * (int64) sizeof (type))
void * Melder_realloc (void *pointer, int64 size);
void * Melder_realloc_f (void *pointer, int64 size);
void * _Melder_calloc (int64 numberOfElements, int64 elementSize);
#define Melder_calloc(type,numberOfElements)  (type *) _Melder_calloc (numberOfElements, sizeof (type))
void * _Melder_calloc_f (int64 numberOfElements, int64 elementSize);
#define Melder_calloc_f(type,numberOfElements)  (type *) _Melder_calloc_f (numberOfElements, sizeof (type))

#define Melder_free(pointer)  _Melder_free ((void **) & (pointer))
void _Melder_free (void **pointer) noexcept;
/*
	Preconditions:
		none (*pointer may be null).
	Postconditions:
		*pointer == nullptr;
*/

int64 Melder_allocationCount ();
/*
	Returns the total number of successful calls to
	Melder_malloc, Melder_realloc (if 'ptr' is null), and Melder_calloc,
	since the start of the process. Mainly for debugging purposes.
*/

int64 Melder_deallocationCount ();
/*
	Returns the total number of successful calls to Melder_free,
	since the start of the process. Mainly for debugging purposes.
*/

int64 Melder_allocationSize ();
/*
	Returns the total number of bytes allocated in calls to
	Melder_malloc, Melder_realloc (if moved), and Melder_calloc,
	since the start of the process. Mainly for debugging purposes.
*/

int64 Melder_reallocationsInSituCount ();
int64 Melder_movingReallocationsCount ();

/* End of file melder_alloc.h */
#endif
