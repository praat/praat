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

#define inherited(klas)  class##klas -> _parent ->

#define Thing_declare1cpp(klas) \
	typedef struct struct##klas *klas; \
	typedef _Thing_auto <struct##klas> auto##klas; \
	typedef struct struct##klas##_Table *klas##_Table; \
	extern klas##_Table class##klas

#define Thing_declare(klas) \
	typedef struct struct##klas *klas; \
	typedef _Thing_auto <struct##klas> auto##klas; \
	typedef struct struct##klas##_Table *klas##_Table; \
	extern klas##_Table class##klas

#define Thing_define(klas,parentKlas) \
	Thing_declare (klas); \
	typedef struct##parentKlas klas##_Parent; \
	struct struct##klas##_Table { \
		void (* _initialize) (void *table); \
		const wchar *_className; \
		parentKlas##_Table _parent; \
		long _size; \
		void * (* _new) (); \
		long version; \
		long sequentialUniqueIdOfReadableClass; \
		void (*destroy) (I); \
		void (*info) (I); \
	}; \
	extern struct struct##klas##_Table theStruct##klas; \
	struct struct##klas : public struct##parentKlas

#define Thing_declare2cpp(klas,parentKlas) \
	typedef struct##parentKlas klas##_Parent; \
	struct struct##klas##_Table { \
		void (* _initialize) (void *table); \
		const wchar *_className; \
		parentKlas##_Table _parent; \
		long _size; \
		void * (* _new) (); \
		long version; \
		long sequentialUniqueIdOfReadableClass; \
		void (*destroy) (I); \
		void (*info) (I); \
		klas##__methods(klas) \
	}; \
	extern struct struct##klas##_Table theStruct##klas

#define class_methods(klas,parentKlas) \
	static void _##klas##_initialize (void *table);   /* Forward declaration. */ \
	static void *_##klas##_new () { return (Thing) new struct##klas; } \
	struct struct##klas##_Table theStruct##klas = { \
		_##klas##_initialize, L"" #klas,   /* Partial initialization because init and */ \
		& theStruct##parentKlas, sizeof (struct struct##klas),   /* parent must be known. */ \
		_##klas##_new }; \
	klas##_Table class##klas = & theStruct##klas; \
	static void _##klas##_initialize (void *table) { \
		klas##_Table us = (klas##_Table) table; \
		if (! class##parentKlas -> destroy)   /* Parent class not initialized? */ \
			class##parentKlas -> _initialize (class##parentKlas); \
		if (Melder_debug == 43) Melder_casual ("Initializing class %ls (%ld), part %ls.", us -> _className, table, class##parentKlas -> _className); \
		class##parentKlas -> _initialize (us);   /* Inherit methods from parent class. */
#define class_method(method)  us -> method = method;   /* Override one method. */
#define class_method_local(klas,method)  us -> method = class##klas##_##method;
#define class_methods_end  }

#define Thing__methods(klas)
typedef struct structThing *Thing;
typedef struct structThing_Table *Thing_Table;
struct structThing_Table {
	void (* _initialize) (void *table);
	const wchar *_className;
	Thing_Table	_parent;
	long _size;
	void * (* _new) ();
	long version;
	long sequentialUniqueIdOfReadableClass;
	void (*destroy) (I);
	void (*info) (I);
};
struct structThing {
	Thing_Table methods;
	wchar *name;
	void * operator new (size_t size) { return Melder_calloc (char, size); }
	void operator delete (void *ptr, size_t size) { (void) size; Melder_free (ptr); }
// new methods:
	virtual void v_destroy () { Melder_free (name); };
		/*
		 * derived::v_destroy calls base::v_destroy at end
		 */
	virtual void v_info ();
		/*
		 * Thing::v_info writes object id, object name, and date;
		 * derived::v_info often calls base::v_info at start and then writes information on contents
		 */
	virtual void v_checkConstraints () { };
		/*
		 * derived::v_checkConstraints typically calls base::v_checkConstraints at start
		 */
	virtual void v_nameChanged () { };
		/*
		 * derived::v_nameChanged may call base::v_nameChanged at start, middle or end
		 */
};
extern struct structThing_Table theStructThing;
extern Thing_Table classThing;

#define forget(thing)  _Thing_forget ((Thing *) & (thing))
/*
	Function:
		free all memory associated with 'thing'.
	Postcondition:
		thing == NULL;
*/
#define forget_nozero(thing)  do { _Thing_forget_nozero (thing); delete thing; } while (false)
/*
	Function:
		free all memory associated with 'thing'.
*/

/* All functions with 'Thing me' as the first argument assume that it is not NULL. */

const wchar * Thing_className (Thing me);
/* Return your class name. */

bool Thing_member (Thing me, void *klas);
/*
	return true if you are a 'klas',
	i.e., if you are an object of the class 'klas' or of one of the classes derived from 'klas'.
	E.g., Thing_member (object, classThing) will always return true.
*/

bool Thing_subclass (void *klas, void *ancestor);
/*
	return true if <klas> is a subclass of <ancestor>,
	i.e., if <klas> equals <ancestor>, or if the parent class of <klas> is a subclass of <ancestor>.
	E.g., Thing_subclass (classX, classThing) will always return true.
*/

void Thing_info (Thing me);
void Thing_infoWithId (Thing me, unsigned long id);

#define Thing_new(Klas)  (Klas) _Thing_new (class##Klas)
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
void Thing_recognizeClassByOtherName (void *readableClass, const wchar *otherName);
long Thing_listReadableClasses (void);

Any Thing_newFromClassNameA (const char *className);
Any Thing_newFromClassName (const wchar *className);
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

wchar * Thing_getName (Thing me);
/* Return a pointer to your internal name (which can be NULL). */
wchar * Thing_messageName (Thing me);

void Thing_setName (Thing me, const wchar_t *name);
/*
	Function:
		remember that you are called 'name'.
	Postconditions:
		my name *and* my name are copies of 'name'.
*/

void Thing_swap (Thing me, Thing thee);
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

/* For the macros. */

void _Thing_forget (Thing *me);
void _Thing_forget_nozero (Thing me);
void * _Thing_check (Thing me, void *table, const char *fileName, int line);
	/* Macros 'iam', 'thouart', 'heis'. */

/* For debugging. */

long Thing_getTotalNumberOfThings (void);
/* This number is 0 initially, increments at every successful `new', and decrements at every `forget'. */

extern long Thing_version;
/* Set by Thing_classFromClassName. */

template <class T>
class _Thing_auto {
	T *ptr;
public:
	/*
	 * Things like
	 *    autoPitch pitch (Pitch_create (...));
	 * and
	 *    autoPitch pitch = Pitch_create (...);
	 * should work.
	 */
	_Thing_auto (T *ptr) : ptr (ptr) {
		//if (Melder_debug == 37) Melder_casual ("begin initializing autopointer %ld with pointer %ld", this, ptr);
		therror;   // if this happens, the destructor won't be called, but that is not necessary anyway
		//if (Melder_debug == 37) Melder_casual ("end initializing autopointer %ld with pointer %ld", this, ptr);
	}
	_Thing_auto () : ptr (NULL) { }
	/*
	 * pitch should be destroyed when going out of scope,
	 * both at the end of the try block and when a throw occurs.
	 */
	~_Thing_auto () {
		//if (Melder_debug == 37) Melder_casual ("begin forgetting autopointer %ld with pointer %ld", this, ptr);
		if (ptr) forget (ptr);
		//if (Melder_debug == 37) Melder_casual ("end forgetting autopointer %ld with pointer %ld", this, ptr);
	}
	T* peek () const {
		return ptr;
	}
	/*
	 * The expression
	 *    pitch.ptr -> xmin
	 * should be abbreviatable by
	 *    pitch -> xmin
	 */
	T* operator-> () const {   // as r-value
		return ptr;
	}
	T& operator* () const {   // as l-value
		return *ptr;
	}
	/*
	 * There are two ways to access the pointer; with and without transfer of ownership.
	 *
	 * Without transfer:
	 *    autoPitch pitch = Sound_to_Pitch (...);
	 *    Pitch_draw (pitch.peek());
	 *
	 * With transfer:
	 *    return thee.transfer();
	 * and
	 *    *out_pitch = pitch.transfer();
	 *    *out_pulses = pulses.transfer();
	 * and
	 *    Collection_addItem (me, pitch.transfer());
	 * and
	 *    praat_new (pitch.transfer(), my name);
	 */
	T* transfer () {
		T* temp = ptr;
		ptr = NULL;   // make the pointer non-automatic again
		return temp;
	}
	//operator T* () { return ptr; }   // this way only if peek() and transfer() are the same, e.g. in case of reference counting
	// template <class Y> Y* operator= (_Thing_auto<Y>& a) { }
	/*
	 * An autoThing can be cloned. This can be used for giving ownership without losing ownership.
	 */
	T* clone () const {
		return static_cast<T *> (Data_copy (ptr));
	}
	/*
	 * Replacing a pointer in an existing autoThing should be an exceptional phenomenon,
	 * and therefore has to be done explicitly (rather than via an assignment),
	 * so that you can easily spot ugly places in your source code.
	 * In order not to leak memory, the old object is destroyed.
	 */
	void reset (T* const newPtr) {
		if (ptr) forget (ptr);
		ptr = newPtr;
		therror;
	}
private:
	/*
	 * The compiler should prevent initializations like
	 *    autoPitch pitch2 = pitch;
	 */
	template <class Y> _Thing_auto (_Thing_auto<Y> &);   // copy constructor
	//_Thing_auto (const _Thing_auto &);
	/*
	 * The compiler should prevent assignments like
	 *    pitch2 = pitch;
	 */
	_Thing_auto& operator= (const _Thing_auto&);   // copy assignment
	//template <class Y> _Thing_auto& operator= (const _Thing_auto<Y>&);
};

template <class T>
class autoThingVector {
	T* ptr;
	long from, to;
public:
	autoThingVector<T> (long from, long to) : from (from), to (to) {
		ptr = static_cast <T*> (NUMvector (sizeof (T), from, to)); therror
	}
	autoThingVector (T *ptr, long from, long to) : ptr (ptr), from (from), to (to) {
		therror
	}
	autoThingVector () : ptr (NULL), from (1), to (0) {
	}
	~autoThingVector<T> () {
		if (ptr) {
			for (long i = from; i <= to; i ++)
				forget (ptr [i]);
			NUMvector_free (sizeof (T), ptr, from);
		}
	}
	T& operator[] (long i) {
		return ptr [i];
	}
	T* peek () const {
		return ptr;
	}
	T* transfer () {
		T* temp = ptr;
		ptr = NULL;   // make the pointer non-automatic again
		return temp;
	}
	void reset (long newFrom, long to) {
		if (ptr) NUMvector_free (sizeof (T), ptr, from);
		ptr = static_cast <T*> (NUMvector (sizeof (T), from = newFrom, to)); therror
	}
};

/* End of file Thing.h */
#endif
