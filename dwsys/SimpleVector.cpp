/* SimpleVector.cpp
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
 djmw 20020812 GPL header
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
*/

#include "SimpleVector.h"

#include "oo_DESTROY.h"
#include "SimpleVector_def.h"
#include "oo_COPY.h"
#include "SimpleVector_def.h"
#include "oo_EQUAL.h"
#include "SimpleVector_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SimpleVector_def.h"
#include "oo_WRITE_TEXT.h"
#include "SimpleVector_def.h"
#include "oo_WRITE_BINARY.h"
#include "SimpleVector_def.h"
#include "oo_READ_TEXT.h"
#include "SimpleVector_def.h"
#include "oo_READ_BINARY.h"
#include "SimpleVector_def.h"
#include "oo_DESCRIPTION.h"
#include "SimpleVector_def.h"

class_methods (DoubleVector, Data)
	class_method_local (DoubleVector, destroy)
	class_method_local (DoubleVector, equal)
	class_method_local (DoubleVector, canWriteAsEncoding)
	class_method_local (DoubleVector, copy)
	class_method_local (DoubleVector, readText)
	class_method_local (DoubleVector, readBinary)
	class_method_local (DoubleVector, writeText)
	class_method_local (DoubleVector, writeBinary)
	class_method_local (DoubleVector, description)
class_methods_end

void DoubleVector_init (I, long min, long max)
{
	iam (DoubleVector);
	my min = min; my max = max;
	my v = NUMvector<double> (min, max);
}

DoubleVector DoubleVector_create (long min, long max)
{
	try {
		autoDoubleVector me = Thing_new (DoubleVector);
		DoubleVector_init (me.peek(), min, max);
		return me.transfer();
	} catch (MelderError) { Melder_throw ("DoubleVector not created."); }
}

class_methods (ComplexVector, Data)
	class_method_local (ComplexVector, destroy)
	class_method_local (ComplexVector, equal)
	class_method_local (ComplexVector, canWriteAsEncoding)
	class_method_local (ComplexVector, copy)
	class_method_local (ComplexVector, readText)
	class_method_local (ComplexVector, readBinary)
	class_method_local (ComplexVector, writeText)
	class_method_local (ComplexVector, writeBinary)
	class_method_local (ComplexVector, description)
class_methods_end

void ComplexVector_init (I, long min, long max)
{
		iam (ComplexVector);
		my min = min; my max = max;
		my v = NUMdcvector (min, max);
}

ComplexVector ComplexVector_create (long min, long max)
{
	try {
		autoComplexVector me = Thing_new (ComplexVector);
		ComplexVector_init (me.peek(), min, max);
		return me.transfer();
	} catch (MelderError) { Melder_throw ("ComplexVector not created."); }
}

/* End of file SimpleVector.cpp */
