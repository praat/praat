/* WorkvectorPool_def.h
 *
 * Copyright (C) 2024 David Weenink
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

#define ooSTRUCT WorkvectorPool
oo_DEFINE_CLASS (WorkvectorPool, Daata)

	oo_INTEGER (poolMemorySize)
	oo_VEC (memoryPool, poolMemorySize)
	oo_INTEGER (numberOfVectors)
	oo_INTVEC (vectorSizes, numberOfVectors)
	oo_INTVEC (vectorStart, numberOfVectors)
	oo_BOOLVEC (inuse, numberOfVectors)
	oo_BOOLEAN (reusable)
	
	#if oo_DECLARING
	
		VEC getRawVEC (integer vectorIndex, integer size) {
			Melder_assert (vectorIndex > 0 && vectorIndex <= numberOfVectors);
			Melder_assert (size <= vectorSizes [vectorIndex]);
			Melder_assert (reusable || (! reusable && ! inuse [vectorIndex]));
			inuse [vectorIndex] = true;
			const integer end = vectorStart [vectorIndex] + size - 1;
			return memoryPool.part (vectorStart [vectorIndex], end);
		}
		
		VEC getZeroVEC (integer vectorIndex, integer size) {
			VEC result = getRawVEC (vectorIndex, size);
			result  <<=  0.0;
			return result;
		}
		
		MAT getRawMAT (integer vectorIndex, integer nrow, integer ncol) {
			Melder_assert (vectorIndex > 0 && vectorIndex <= numberOfVectors);
			const integer wantedSize = nrow * ncol;
			Melder_assert (wantedSize <= vectorSizes [vectorIndex]);
			inuse [vectorIndex] = true;
			const integer end = vectorStart [vectorIndex] + wantedSize - 1;
			VEC pool = memoryPool.part (vectorStart [vectorIndex], end);
			return pool.asmatrix (nrow, ncol);
		}
		
		MAT getZeroMAT (integer vectorIndex, integer nrow, integer ncol) {
			MAT result = getRawMAT (vectorIndex, nrow, ncol);
			result  <<=  0.0;
			return result;
		}
		
		void freeVEC (integer vectorIndex) {
			Melder_assert (vectorIndex > 0 && vectorIndex <= numberOfVectors);
			Melder_assert (inuse [vectorIndex]);
			inuse [vectorIndex] = false;
		}
		
	#endif

oo_END_CLASS (WorkvectorPool)
#undef ooSTRUCT

/* End of file WorkvectorPool_def.h */
