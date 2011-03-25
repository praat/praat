#ifndef _Thing_h_
#define _Thing_h_
/* Thing.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/03/07 GPL
 * pb 2002/03/11 replaced _I with void_me etc for compliance with new ctype.h
 * pb 2004/10/16 C++ compatible structs
 * pb 2004/10/25 C++ compatible assignments
 * pb 2006/12/10 update on "info" documentation
 * pb 2007/06/11 wchar_t
 * pb 2007/10/09 removed char
 * pb 2008/04/04 Thing_infoWithId
 * pb 2009/03/21 modern enums
 * pb 2009/08/17 readable-class IDs
 * pb 2011/03/05 C++
 * pb 2011/03/09 C++
 */

/* The root class of all objects. */

/* Anyone who uses Thing can also use: */
	/* Arrays with any bounds and 1 or two indices, math, and numerics: */
		#include "NUM.h"   /* Including math.h */
	/* The messaging mechanism: */
		#include "melder.h"   /* Including stdio.h string.h etc. */
	/* The macros for struct and class definitions: */
		#include "oo.h"
	/* The input/output mechanism: */
		#include "abcio.h"
		#include "lispio.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* Public. */

typedef void *Any;   /* Prevent compile-time type checking. */

/*
	Use the macros 'I' and 'thou' for objects in the formal parameter lists.
	Use the macros 'iam' and 'thouart'
	as the first declaration in a function definition.
	After this, the object 'me' or 'thee' has the right class (for the compiler),
	so that you can use the macros 'my' and 'thy' to refer to members,
	and 'our' and 'your' to refer to methods.
	Example: int Person_getAge (I) { iam (Person); return my age; }
*/
#define I  Any void_me
#define thou  Any void_thee
#define iam(klas)  klas me = (klas) void_me
#define thouart(klas)  klas thee = (klas) void_thee
#define my  me ->
#define thy  thee ->
#define his  him ->
#define our  my methods ->
#define your  thy methods ->

#define forget(thing)  _Thing_forget ((Thing *) & (thing))
/*
	Function:
		free all memory associated with 'thing'.
	Postconditions:
		thing == NULL;
*/

/* All functions with 'I' as the first argument assume that it is not NULL. */

const wchar_t * Thing_className (I);
/* Return your class name. */

int Thing_member (I, void *klas);
/*
	return TRUE if you are a 'klas',
	i.e., if you are an object of the class 'klas' or of one of the classes derived from 'klas'.
	E.g., Thing_member (object, classThing) will always return TRUE.
*/

int Thing_subclass (void *klas, void *ancestor);
/*
	return TRUE if <klas> is a subclass of <ancestor>,
	i.e., if <klas> equals <ancestor>, or if the parent class of <klas> is a subclass of <ancestor>.
	E.g., Thing_subclass (classX, classThing) will always return TRUE.
*/

void Thing_info (I);
void Thing_infoWithId (I, unsigned long id);

#define Thing_new(klas)  (klas) _Thing_new ((void *) class##klas)
/*
	Function:
		return a new object of class 'klas'.
	Postconditions:
		result -> methods == class'klas';
		other members are 0;
		result -> methods -> destroy != NULL;   // Class table initialized.
*/

Any _Thing_new (void *klas);
/*
	Function:
		return a new object of class 'klas'.
	Postconditions:
		result -> methods == 'klas';
		other members are 0;
		result -> methods -> destroy != NULL;   // Class table initialized.
*/

void Thing_recognizeClassesByName (void *readableClass, ...);
/*
	Function:
		make Thing_classFromClassName () and Thing_newFromClassName ()
		recognize a class from its name (a string).
	Arguments:
		as many classes as you want; finish with a NULL.
		It is not an error if a class occurs more than once in the list.
	Behaviour:
		calling this routine more than once, each time for different classes,
		has the same result as calling it once for all these classes together.
		Thing can remember up to 1000 string-readable classes.
	Usage:
		you should call this routine for all classes that you want to read by name,
		e.g., with Data_readFromTextFile () or Data_readFromBinaryFile (),
		or with Data_readText () or Data_readBinary () if the object is a Collection.
		Calls to this routine should preferably be put in the beginning of main ().
*/
void Thing_recognizeClassByOtherName (void *readableClass, const wchar_t *otherName);
long Thing_listReadableClasses (void);

Any Thing_newFromClassNameA (const char *className);
Any Thing_newFromClassName (const wchar_t *className);
/*
	Function:
		return a new object of class 'className', or NULL if the class name is not recognized.
	Postconditions:
		result -> methods == class'className';
		other members are 0;
		class'className' -> destroy != NULL;   // class'className' has been initialized.
	Side effect:
		see Thing_classFromClassName.
*/

void *Thing_classFromClassName (const wchar_t *className);
/*
	Function:
		Return the class table of class 'className', or NULL if it is not recognized.
		E.g. the value returned from Thing_classFromClassName (L"PietjePuk")
		will be equal to classPietjePuk.
	Postcondition:
		class'className' -> destroy != NULL;   // class'className' has been initialized.
	Side effect:
		Sets the global variable Thing_version.
		If 'className' equals L"PietjePuk 300", the value returned will be classPietjePuk,
		and Thing_version will be set to 300.
*/

wchar_t * Thing_getName (I);
/* Return a pointer to your internal name (which can be NULL). */
wchar_t * Thing_messageName (I);

void Thing_setName (I, const wchar_t *name);
/*
	Function:
		remember that you are called 'name'.
	Postconditions:
		my name *and* my name are copies of 'name'.
*/

void Thing_overrideClass (I, void *klas);
/*
	Function:
		change my class to 'klas'.
	Postconditions:
		my methods == klas;
		klas -> destroy != NULL;   // 'klas' has been initialized.
	Usage:
		- Safe typecast if my methods is a subclass of 'klas',
			in which case you can also safely use "my methods = klas".
		- Safe typecast if 'klas' is a dummy subclass of my methods,
			i.e., if 'klas' does not add members or methods (so this is just a name change);
			in this case, you cannot just use "my methods = klas" if you are not sure whether
			'klas' has been initialized (by a previous 'new' or so).
			An application of this is giving a collection of objects of class "Foo"
			the name "Foos" instead of "Collection".
		- Unsafe in all other situations. Normally, 'I' should contain the members and methods of 'klas',
			perhaps with different names.
*/

void Thing_swap (I, thou);
/*
	Function:
		Swap my and thy contents.
	Precondition:
		my methods == thy methods;
	Postconditions:
		my xxx == thy old xxx;
		thy xxx == my old xxx;
	Usage:
		Swap two objects without changing any references to them.
*/

#define inherited(klas)  class##klas -> _parent ->

/* The inheritor should put one of the macros class_create or class_create_opaque */
/* after the definitions of the members and methods. */
/* There are two cases: */
/* 1. The declaration of the class members should be visible to the user. */
/*    In this case, use class_create in the klas.h header file. */
/* 2. The declaration of the class members should be opaque to the user, */
/*    but visible to the inheritor. */
/*    In this case, put the statement "typedef struct klas *klas;" in klas.h, */
/*    and use class_create_opaque in the klasP.h header file */
/*    (or in klas.c if there will not be any inheritors). */

#ifdef __cplusplus
	#define _THING_DECLARE_AUTO(Type)  typedef _Thing_auto <struct##Type> auto##Type;
#else
	#define _THING_DECLARE_AUTO(Type)
#endif

#define Thing_declare1(klas) \
	typedef struct struct##klas *klas; \
	_THING_DECLARE_AUTO (klas) \
	klas##__parents (klas) \
	typedef struct struct##klas##_Table *klas##_Table; \
	extern klas##_Table class##klas

#define Thing_inherit(klas,parentKlas) \
	static inline parentKlas klas##_as_##parentKlas (klas me) { return (parentKlas) me; }

#define Thing_declare2(klas,parentKlas) \
	static inline parentKlas klas##_as_parent (klas me) { return (parentKlas) me; } \
	struct struct##klas##_Table { \
		void (* _initialize) (void *table); \
		const wchar_t *_className; \
		parentKlas##_Table _parent; \
		long _size; \
		klas##__methods(klas) \
	}; \
	struct struct##klas { \
		klas##_Table methods; \
		klas##__members(klas) \
	}; \
	extern struct struct##klas##_Table theStruct##klas

#define class_create_opaque(klas,parentKlas) \
	typedef struct struct##klas##_Table *klas##_Table; \
	struct struct##klas##_Table { \
		void (* _initialize) (void *table); \
		const wchar_t *_className; \
		parentKlas##_Table	_parent; \
		long _size; \
		klas##_methods \
	}; \
	struct struct##klas { \
		klas##_Table methods; \
		klas##_members \
	}; \
	extern struct struct##klas##_Table theStruct##klas; \
	extern klas##_Table class##klas

#define class_create(klas,parentKlas) \
	typedef struct struct##klas *klas; \
	_THING_DECLARE_AUTO (klas) \
	class_create_opaque (klas, parentKlas)

/* For klas.c, after the definitions of the methods. */

#define class_methods(klas,parentKlas) \
	static void _##klas##_initialize (void *table);   /* Forward declaration. */ \
	struct struct##klas##_Table theStruct##klas = { \
		_##klas##_initialize, L"" #klas,   /* Partial initialization because init and */ \
		& theStruct##parentKlas, sizeof (struct struct##klas) };   /* parent must be known. */ \
	klas##_Table class##klas = & theStruct##klas; \
	static void _##klas##_initialize (void *table) { \
		klas##_Table us = (klas##_Table) table; \
		if (! class##parentKlas -> destroy)   /* Parent class not initialized? */ \
			class##parentKlas -> _initialize (class##parentKlas); \
		class##parentKlas -> _initialize (us);   /* Inherit methods from parent class. */
#define class_method(method)  us -> method = method;   /* Override one method. */
#define class_method_local(klas,method)  us -> method = class##klas##_##method;
#define class_methods_end  }

/* For the inheritors. */

#define Thing_members \
	wchar_t *name;
#define Thing_methods \
	long version; \
	long sequentialUniqueIdOfReadableClass; \
	void (*destroy) (I); \
	void (*info) (I); \
	void (*nameChanged) (I);
typedef struct structThing *Thing;
class_create_opaque (Thing, Thing);   /* Root class: no parent. */

/*
	Methods:

	void destroy (I)
		Message sent by _Thing_forget:
			destroy all of my members who are arrays or objects,
			except those who are NULL already (always check).
		Inheritor:
			Use NUMxvector_free and NUMxmatrix_free for destroying arrays;
			you do not have to set the array members to NULL.
			Use 'forget' for destroying objects.
			You can call the inherited 'destroy' last, for destroying the inherited arrays and objects.
		Example:
			iam (Miep);
			NUMdvector_free (my array);
			forget (my object);
			inherited (Miep) destroy (me);
		Thing::destroy does nothing.
		After exit:
			the memory associated with me will be freed,
			and one pointer to it will be set to NULL (see 'forget').

	void info (I)
		Message sent by Thing_info:
			use a sequence of MelderInfo_writeXXX to give some information about you;
			these are often preceded by classData -> info (me).
		Thing::info shows my class name.

	void nameChanged (I)
		Message sent by Thing_setName after setting the new name:
			if you are capable of showing your name, show your new name.
		Thing::nameChanged does nothing.
*/

/* For the macros. */

void _Thing_forget (Thing *me);
	/* Macro 'forget'. */
void * _Thing_check (I, void *table, const char *fileName, int line);
	/* Macros 'iam', 'thouart', 'heis'. */

/* For debugging. */

long Thing_getTotalNumberOfThings (void);
/* This number is 0 initially, increments at every successful `new', and decrements at every `forget'. */

extern long Thing_version;
/* Set by Thing_classFromClassName. */

#ifdef __cplusplus
	}
template <class T>
class _Thing_auto {
	T *ptr;
public:
	/*
	 * Things like
	 *    autoPitch pitch (Pitch_create (...));
	 *    autoPitch pitch = Pitch_create (...);
	 * should work.
	 */
	_Thing_auto (T *ptr) throw (int) : ptr (ptr) {
		//if (Melder_debug == 37) Melder_casual ("begin initializing autopointer %ld with pointer %ld", this, ptr);
		iferror throw 1;   // if this happens, the destructor won't be called, but that is not necessary anyway
		//if (Melder_debug == 37) Melder_casual ("end initializing autopointer %ld with pointer %ld", this, ptr);
	}
	/*
	 * pitch should be destroyed when going out of scope,
	 * both at the end of the try block and when a throw occurs.
	 */
	~_Thing_auto () throw () {
		//if (Melder_debug == 37) Melder_casual ("begin forgetting autopointer %ld with pointer %ld", this, ptr);
		if (ptr) forget (ptr);
		//if (Melder_debug == 37) Melder_casual ("end forgetting autopointer %ld with pointer %ld", this, ptr);
	}
	/*
	 * Access the pointer.
	 */
	T* peek () const throw () { return ptr; }
	//T* operator() () const throw () { return ptr; }
	/*
	 * The expression
	 *    pitch.ptr -> xmin
	 * should be abbreviatable by
	 *    pitch -> xmin
	 */
	T* operator-> () const throw () { return ptr; }   // as r-value
	T& operator* () const throw () { return *ptr; }   // as l-value
	/*
	 * The expression
	 *    Pitch pitch2 = pitch.ptr;
	 * should be abbreviatable by
	 *    Pitch pitch2 = pitch;
	 * But how?
	 */
	//operator T* () { return ptr; }   // this way; but perhaps it's better not to allow that, and have an explicit peek() versus transfer()
	// template <class Y> Y* operator= (_Thing_auto<Y>& a) { }
	/*
	 * Assignments like
	 *    return thee.transfer();
	 * and
	 *    out_pitch = pitch.transfer();
	 *    out_pulses = pulses.transfer();
	 * and
	 *    Collection_addItem (me, pitch.transfer());
	 *    praat_new (pitch.transfer(), NAME);
	 * make the pointer non-automatic again.
	 */
	T* transfer (void) throw () { T* temp = ptr; ptr = NULL; return temp; }
	/*
	 * Replacing a pointer in an existing autoThing should be an exceptional phenomenon,
	 * and therefore has to be done explicitly (rather than via an assignment),
	 * so that you can easily spot ugly places in your source code.
	 * In order not to leak memory, the old object is destroyed.
	 */
	void reset (const T* const newPtr) { if (ptr) forget (ptr); ptr = newPtr; iferror return 1; }
private:
	/*
	 * The compiler should prevent initializations like
	 *    autoPitch pitch2 = pitch;
	 */
	template <class Y> _Thing_auto (_Thing_auto<Y> &);
	//_Thing_auto (const _Thing_auto &);
	/*
	 * The compiler should prevent assignments like
	 *    pitch2 = pitch;
	 */
	_Thing_auto& operator= (const _Thing_auto&);
	//template <class Y> _Thing_auto& operator= (const _Thing_auto<Y>&);
};

#endif

/* End of file Thing.h */
#endif
