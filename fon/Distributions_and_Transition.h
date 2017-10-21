/* Distributions_and_Transition.h
 *
 * Copyright (C) 1997-2011,2015,2017 Paul Boersma
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

#include "Distributions.h"
#include "Transition.h"

autoTransition Distributions_to_Transition (Distributions underlying, Distributions surface,
	integer environment, Transition adjacency, bool greedy);

autoDistributions Distributions_Transition_map (Distributions me, Transition map);

autoDistributions Transition_to_Distributions_conflate (Transition me);

/* End of file Distributions_and_Transition.h */
