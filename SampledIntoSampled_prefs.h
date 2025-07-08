#ifndef _SampledIntoSampled_prefs_h_
#define _SampledIntoSampled_prefs_h_
/* SampledIntoSampled_prefs.h
 *
 * Copyright (C) 2024-2025 David Weenink
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

Prefs_begin (SampledIntoSampled)

	InstancePrefs_addBool  (SampledIntoSampled, useMultiThreading, 1, false)
	ClassPrefs_addInteger  (SampledIntoSampled, maximumNumberOfThreads,  1, U"20")
	ClassPrefs_addInteger  (SampledIntoSampled, maximumNumberOfFramesPerThread,  1, U"40")
	ClassPrefs_addInteger  (SampledIntoSampled, minimumNumberOfFramesPerThread,  1, U"40")
	ClassPrefs_addBool	   (SampledIntoSampled, extraAnalysisInfo, 1, false)
	
Prefs_end (SampledIntoSampled)

#endif /* _SampledIntoSampled_prefs_h_ */
 
