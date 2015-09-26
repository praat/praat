#ifndef _Thing_h_
#define _Thing_h_
/* Thing.h
 *
 * Copyright (C) 1992-2011,2012,2013,2014,2015 Paul Boersma
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
	/* Arrays with any bounds and 1 or 2 indices, math, and numerics: */
		#include "NUM.h"   /* Including math.h */
	/* The messaging mechanism: */
		#include "melder.h"   /* Including stdio.h string.h etc. */
	/* The macros for struct and class definitions: */
		#include "oo.h"
	/* The input/output mechanism: */
		#include "abcio.h"

#include <string>

#define _Thing_auto_DEBUG  0

/* Public. */

typedef void *Any;   /* Prevent compile-time type checking. */

/*
	Use the macros 'I' and 'thou' for objects in the formal parameter lists
	(if the explicit type cannot be used).
	Use the macros 'iam' and 'thouart'
	as the first declaration in a function definition.
	After this, the object 'me' or 'thee' has the right class (for the compiler),
	so that you can use the macros 'my' and 'thy' to refer to members.
	Example: int Person_getAge (I) { iam (Person); return my age; }
*/
#define I  Any void_me
#define thou  Any void_thee
#define iam(klas)  klas me = (klas) void_me
#define thouart(klas)  klas thee = (klas) void_thee
#define my  me ->
#define thy  thee ->
#define his  him ->
#define our  this ->

typedef struct structClassInfo *ClassInfo;
struct structClassInfo {
	/*
	 * The following five fields are statically initialized by the Thing_implement() macro.
	 */
	const char32 *className;
	ClassInfo parent;
	long size;
	Thing (* _new) ();   // objects have to be constructed via this function, because it calls C++ "new", which initializes the C++ class pointer
	long version;
	/*
	 * The following field is initialized by Thing_recognizeClassesByName, only for classes that have to be read (usually from disk).
	 */
	long sequentialUniqueIdOfReadableClass;
	/*
	 * The following field is initialized by Thing_dummyObject(), which is used only rarely.
	 */
	Thing dummyObject;
};

#define Thing_declare(klas) \
	typedef struct struct##klas *klas; \
	typedef _Thing_auto <struct##klas> auto##klas; \
	extern ClassInfo class##klas

#define Thing_define(klas,parentKlas) \
	Thing_declare (klas); \
	typedef struct##parentKlas klas##_Parent; \
	extern struct structClassInfo theClassInfo_##klas; \
	struct struct##klas : public struct##parentKlas

#define Thing_implement(klas,parentKlas,version) \
	static Thing _##klas##_new () { return new struct##klas; } \
	struct structClassInfo theClassInfo_##klas = { U"" #klas, & theClassInfo_##parentKlas, sizeof (class struct##klas), _##klas##_new, version, 0, NULL }; \
	ClassInfo class##klas = & theClassInfo_##klas

/*
 * Thing has no parent class, so instead of using the Thing_define macro
 * we write out the stuff that does exist.
 */
typedef struct structThing *Thing;
extern ClassInfo classThing;
extern struct structClassInfo theClassInfo_Thing;
struct structThing {
	ClassInfo classInfo;   // the Praat class pointer (every object also has a C++ class pointer initialized by C++ "new")
	char32 *name;
	void * operator new (size_t size) { return Melder_calloc (char, (int64) size); }
	void operator delete (void *ptr, size_t /* size */) { Melder_free (ptr); }

	/*
	 * If a Thing has members of type autoThing,
	 * then we want the destructors of autoThing to be called automatically whenever Thing is `delete`d.
	 * For this to happen, it is necessary that every Thing itself has a destructor.
	 * We therefore define a destructor here,
	 * and we make it virtual to ensure that every subclass has its own automatic version.
	 */
	virtual ~structThing () { }

	virtual void v_destroy () { Melder_free (name); };
		/*
		 * derived::v_destroy calls base::v_destroy at end
		 */
	virtual void v_info ();
		/*
		 * Implement as follows: call a set of MelderInfo_writeXXX describing your data.
		 *
		 * Thing::v_info writes object id, object name, and date;
		 * derived::v_info often calls base::v_info at start and then writes information on the new data,
		 * but a few ancestors can be skipped if their data have new meanings.
		 */
	virtual void v_checkConstraints () { };
		/*
		 * derived::v_checkConstraints typically calls base::v_checkConstraints at start
		 */
	virtual void v_nameChanged () { };
		/*
		 * derived::v_nameChanged may call base::_nameChanged at start, middle or end
		 */
	virtual void v_copyPreferencesToInstance () { };
		/*
		 * derived::v_copyPreferencesToInstance calls base::v_copyPreferencesToInstance at start
		 */
};

#define forget(thing)  do { _Thing_forget (thing); thing = NULL; } while (false)
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

const char32 * Thing_className (Thing me);
/* Return your class name. */

bool Thing_isa (Thing me, ClassInfo klas);
/*
	return true if you are a 'klas',
	i.e., if you are an object of the class 'klas' or of one of the classes derived from 'klas'.
	E.g., Thing_isa (object, classThing) will always return true.
*/

bool Thing_isSubclass (ClassInfo klas, ClassInfo ancestor);
/*
	return true if <klas> is a subclass of <ancestor>,
	i.e., if <klas> equals <ancestor>, or if the parent class of <klas> is a subclass of <ancestor>.
	E.g., Thing_isSubclass (classX, classThing) will always return true.
*/

void Thing_info (Thing me);
void Thing_infoWithIdAndFile (Thing me, unsigned long id, MelderFile file);

#define Thing_new(Klas)  static_cast<Klas> (Thing_newFromClass (class##Klas))
/*
	Function:
		return a new object of class 'klas'.
	Postconditions:
		result -> classInfo == class'klas';
		other members are 0.
*/

Thing Thing_newFromClass (ClassInfo klas);
/*
	Function:
		return a new object of class 'klas'.
	Postconditions:
		result -> classInfo == 'klas';
		other members are 0.
*/

void Thing_recognizeClassesByName (ClassInfo readableClass, ...);
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
void Thing_recognizeClassByOtherName (ClassInfo readableClass, const char32 *otherName);
long Thing_listReadableClasses (void);

Thing Thing_newFromClassName (const char32 *className, int *p_formatVersion);
/*
	Function:
		return a new object of class 'className', or NULL if the class name is not recognized.
	Postconditions:
		result -> classInfo == class'className';
		other members are 0.
	Side effect:
		see Thing_classFromClassName.
*/

ClassInfo Thing_classFromClassName (const char32 *className, int *formatVersion);
/*
	Function:
		Return the class info table of class 'className', or NULL if it is not recognized.
		E.g. the value returned from Thing_classFromClassName (L"PietjePuk")
		will be equal to classPietjePuk.
	Side effect:
		If 'className' equals L"PietjePuk 300", the value returned will be classPietjePuk,
		and formatVersion (if not NULL) will be set to 300.
*/

#define Thing_dummyObject(klas) \
	((klas) _Thing_dummyObject (class##klas))
Thing _Thing_dummyObject (ClassInfo classInfo);

char32 * Thing_getName (Thing me);
/* Return a pointer to your internal name (which can be NULL). */
char32 * Thing_messageName (Thing me);

void Thing_setName (Thing me, const char32 *name /* cattable */);
/*
	Function:
		remember that you are called 'name'.
	Postconditions:
		my name *and* my name are copies of 'name'.
*/

#define Thing_cast(Klas,var,expr) \
	Klas var = static_cast <Klas> (expr);   /* The compiler checks this. */ \
	Melder_assert (var == NULL || Thing_isa (var, class##Klas));

void Thing_swap (Thing me, Thing thee);
/*
	Function:
		Swap my and thy contents.
	Precondition:
		my classInfo == thy classInfo;
	Postconditions:
		my xxx == thy old xxx;
		thy xxx == my old xxx;
	Usage:
		Swap two objects without changing any references to them.
*/

/* For the macros. */

void _Thing_forget (Thing me);
void _Thing_forget_nozero (Thing me);
void * _Thing_check (Thing me, ClassInfo table, const char *fileName, int line);
	/* Macros 'iam', 'thouart', 'heis'. */

/* For debugging. */

long Thing_getTotalNumberOfThings (void);
/* This number is 0 initially, increments at every successful `new', and decrements at every `forget'. */

template <class T>
class _Thing_auto {
	T *d_ptr;
public:
	/*
	 * Things like
	 *    autoPitch pitch (Pitch_create (...));
	 * and
	 *    autoPitch pitch = Pitch_create (...);
	 * should work.
	 */
	inline _Thing_auto (T *a_ptr) : d_ptr (a_ptr) {
		#if _Thing_auto_DEBUG
			if (d_ptr) fprintf (stderr, "constructor %p %s\n", d_ptr, Melder_peek32to8 (d_ptr -> classInfo -> className));
		#endif
	}
	/*
	 * Things like
	 *    autoPitch pitch;
	 * should initialize the pointer to NULL.
	 */
	inline _Thing_auto () : d_ptr (NULL) {
		#if _Thing_auto_DEBUG
			fprintf (stderr, "default constructor\n");
		#endif
	}
	/*
	 * pitch should be destroyed when going out of scope,
	 * both at the end of the try block and when a throw occurs.
	 */
	inline ~_Thing_auto () {
		#if _Thing_auto_DEBUG
			fprintf (stderr, "destructor %p %s\n", d_ptr, d_ptr ? Melder_peek32to8 (d_ptr -> classInfo -> className) : "(class unknown)");
		#endif
		if (d_ptr) {
			_Thing_forget (d_ptr);
			d_ptr = NULL;
		}
	}
	inline T* peek () const {
		return d_ptr;
	}
	/*
	 * The expression
	 *    pitch.d_ptr -> xmin
	 * should be abbreviatable by
	 *    pitch -> xmin
	 */
	inline T* operator-> () const {   // as r-value
		return d_ptr;
	}
	inline T& operator* () const {   // as l-value
		return *d_ptr;
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
	inline T* transfer () {
		T* temp = d_ptr;
		d_ptr = NULL;   // make the pointer non-automatic again
		return temp;
	}
	#if 1
		inline operator T* () { return d_ptr; }   // this way only if peek() and transfer() are the same, e.g. in case of reference counting
		// template <class Y> Y* operator= (_Thing_auto<Y>& a) { }
	#endif
	/*
	 * An autoThing can be cloned. This can be used for giving ownership without losing ownership.
	 */
	inline T* clone () const {
		return static_cast<T *> (Data_copy (d_ptr));
	}
	/*
	 * Replacing a pointer in an existing autoThing should be an exceptional phenomenon,
	 * and therefore has to be done explicitly (rather than via an assignment),
	 * so that you can easily spot ugly places in your source code.
	 * In order not to leak memory, the old object is destroyed.
	 */
	inline void reset (T* ptr) {
		_Thing_forget (d_ptr);
		d_ptr = ptr;
	}
	inline void zero () {
		d_ptr = NULL;
	}
private:
	/*
	 * The compiler should prevent initializations from _Thing_auto l-values, as in
	 *    autoPitch pitch2 = pitch;
	 * This is because the syntax of this statement is *copy* syntax,
	 * but the semantics of this statement has to be, confusingly, *move* semantics
	 * (i.e., pitch.d_ptr should be set to NULL),
	 * because if the semantics were copy semantics instead,
	 * a destructor would be called at some point for both pitch and pitch 2,
	 * twice deleting the same object, which is a run-time error.
	 */
	inline _Thing_auto<T> (const _Thing_auto<T>&);   // disable copy constructor from an l-value of class T*
	template <class Y> inline _Thing_auto<T> (const _Thing_auto<Y>&);   // disable copy constructor from an l-value of a descendant class of T*
	/*
	 * The compiler should prevent assignments from _Thing_auto l-values, as in
	 *    pitch2 = pitch;
	 * This is because the syntax of this statement is *copy* syntax,
	 * but the semantics of this statement has to be, confusingly, *move* semantics
	 * (i.e., pitch.d_ptr should be set to NULL),
	 * because if the semantics were copy semantics instead,
	 * a destructor would be called at some point for both pitch and pitch 2,
	 * twice deleting the same object, which is a run-time error.
	 */
	inline _Thing_auto<T>& operator= (const _Thing_auto<T>&);   // disable copy assignment from an l-value of class T*
	template <class Y> inline _Thing_auto<T>& operator= (const _Thing_auto<Y>&);   // disable copy assignment from an l-value of a descendant class of T*
public:
	/*
	 * The compiler should treat initializations from _Thing_auto r-values, as in
	 *    extern autoPitch Pitch_create (...);
	 *    autoPitch pitch = Pitch_create (...);
	 * as move constructors.
	 */
	inline _Thing_auto<T> (_Thing_auto<T>&& other) noexcept : d_ptr (other.d_ptr) {
		#if _Thing_auto_DEBUG
			if (d_ptr) fprintf (stderr, "move constructor %p from same class %s\n", d_ptr, Melder_peek32to8 (d_ptr -> classInfo -> className));
		#endif
		other.d_ptr = NULL;
	}
	template <class Y> inline _Thing_auto<T> (_Thing_auto<Y>&& other) noexcept : d_ptr (other.peek()) {
		#if _Thing_auto_DEBUG
			if (d_ptr) fprintf (stderr, "move constructor %p from other class %s\n", d_ptr, Melder_peek32to8 (d_ptr -> classInfo -> className));
		#endif
		other.zero();
	}
	/*
	 * The compiler should treat assignments from _Thing_auto r-values, as in
	 *    extern autoPitch Pitch_create (...);
	 *    autoPitch pitch;
	 *    pitch = Pitch_create (...);
	 * as move assignments.
	 */
	inline _Thing_auto<T>& operator= (_Thing_auto<T>&& other) noexcept {
		if (other. d_ptr != d_ptr) {
			#if _Thing_auto_DEBUG
				fprintf (stderr, "move assignment before %p from same class %s\n", d_ptr, d_ptr ? Melder_peek32to8 (d_ptr -> classInfo -> className) : "(class unknown)");
			#endif
			_Thing_forget (d_ptr);
			d_ptr = other. d_ptr;
			#if _Thing_auto_DEBUG
				fprintf (stderr, "move assignment after %p from same class %s\n", d_ptr, d_ptr ? Melder_peek32to8 (d_ptr -> classInfo -> className) : "(class unknown)");
			#endif
			other. d_ptr = NULL;
		}
		return *this;
	}
	template <class Y> inline _Thing_auto<T>& operator= (_Thing_auto<Y>&& other) noexcept {
		if (other.peek() != d_ptr) {
			#if _Thing_auto_DEBUG
				fprintf (stderr, "move assignment before %p from other class %s\n", d_ptr, d_ptr ? Melder_peek32to8 (d_ptr -> classInfo -> className) : "(class unknown)");
			#endif
			_Thing_forget (d_ptr);
			d_ptr = other.peek();
			#if _Thing_auto_DEBUG
				fprintf (stderr, "move assignment after %p from other class %s\n", d_ptr, d_ptr ? Melder_peek32to8 (d_ptr -> classInfo -> className) : "(class unknown)");
			#endif
			other.zero();
		}
		return *this;
	}
	/*
	 * Move semantics from l-values can be achieved with move syntax:
	 *    autoPitch pitch2 = pitch.move();   // calls the move constructor and therefore nullifies pitch
	 *
	 *    pitch2 = pitch.move();   // performs move assignment and therefore nullifies pitch
	 */
	inline _Thing_auto<T>&& move () noexcept { return static_cast <_Thing_auto<T>&&> (*this); }
	/*
	 * Returning _Thing_auto from a function works as hoped for:
	 *    autoPitch Sound_to_Pitch (Sound me) {
	 *       autoPitch thee = Pitch_create (...);
	 *       ...
	 *       return thee;
	 *    }
	 *    autoPitch pitch = Sound_to_Pitch (sound);
	 * returns a moved thee in pitch.
	 *
	 * In function arguments, transfer of ownership works only explicitly:
	 *    extern void Collection_addItem (Collection me, autoDaata item);
	 *    autoPitch pitch = Pitch_create (...);
	 *    Collection_addItem (collection, pitch.move());   // compiler error if you don't call move()
	 */

};

template <class T>
class autoThingVector {
	T* d_ptr;
	long d_from, d_to;
public:
	autoThingVector<T> (long from, long to) : d_from (from), d_to (to) {
		d_ptr = static_cast <T*> (NUMvector (sizeof (T), from, to));
	}
	autoThingVector (T *ptr, long from, long to) : d_ptr (ptr), d_from (from), d_to (to) {
	}
	autoThingVector () : d_ptr (NULL), d_from (1), d_to (0) {
	}
	~autoThingVector<T> () {
		if (d_ptr) {
			for (long i = d_from; i <= d_to; i ++)
				forget (d_ptr [i]);
			NUMvector_free (sizeof (T), d_ptr, d_from);
		}
	}
	T& operator[] (long i) {
		return d_ptr [i];
	}
	T* peek () const {
		return d_ptr;
	}
	T* transfer () {
		T* temp = d_ptr;
		d_ptr = NULL;   // make the pointer non-automatic again
		return temp;
	}
	void reset (long from, long to) {
		if (d_ptr) {
			for (long i = d_from; i <= d_to; i ++)
				forget (d_ptr [i]);
			NUMvector_free (sizeof (T), d_ptr, d_from);
			d_ptr = NULL;
		}
		d_from = from;   // this assignment is safe, because d_ptr is NULL
		d_to = to;
		d_ptr = static_cast <T*> (NUMvector (sizeof (T), from, to));
	}
};

/* End of file Thing.h */
#endif
