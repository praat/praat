#ifndef _NUMfilter_h_
#define _NUMfilter_h_
/* NUMfilter.h
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

void NUMfbtoa (double formant, double bandwidth, double dt, double *a1, double *a2);
void VECfilterSecondOrderSection_a_inplace (VECVU const& x, double a1, double a2);
void VECfilterSecondOrderSection_fb_inplace (VECVU const& x, double dt, double formant, double bandwidth);
double NUMftopreemphasis (double f, double dt);
void VECpreemphasize_a_inplace (VECVU const& x, double preemphasis);
void VECdeemphasize_a_inplace (VECVU const& x, double preemphasis);
void VECpreemphasize_f_inplace (VECVU const& x, double dt, double frequency);
void VECdeemphasize_f_inplace (VECVU const& x, double dt, double frequency);

/* End of file NUMfilter.h */
#endif
