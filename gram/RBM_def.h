/* RBM_def.h
 *
 * Copyright (C) 2016,2017 Paul Boersma
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


#define ooSTRUCT RBM
oo_DEFINE_CLASS (RBM, Daata)

	oo_QUESTION (inputsAreBinary)
	oo_LONG (numberOfInputNodes)
	oo_LONG (numberOfOutputNodes)
	oo_DOUBLE_MATRIX (weights, numberOfInputNodes, numberOfOutputNodes)
	oo_DOUBLE_VECTOR (inputBiases, numberOfInputNodes)
	oo_DOUBLE_VECTOR (outputBiases, numberOfOutputNodes)

	oo_DOUBLE_VECTOR (inputActivities, numberOfInputNodes)
	oo_DOUBLE_VECTOR (outputActivities, numberOfOutputNodes)
	oo_DOUBLE_VECTOR (inputReconstruction, numberOfInputNodes)
	oo_DOUBLE_VECTOR (outputReconstruction, numberOfOutputNodes)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (RBM)
#undef ooSTRUCT


/* End of file RBM_def.h */
