/* PowerCepstrumWorkspace_def.h
 *
 * Copyright (C) 2025 David Weenink
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

#define ooSTRUCT PowerCepstrumWorkspace
oo_DEFINE_CLASS (PowerCepstrumWorkspace, Daata)

	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (PowerCepstrum, powercepstrum)
	oo_INTEGER (numberOfPoints)
	oo_INTEGER (imin) // [imin,imax]  the slope calculation interval
	oo_INTEGER (imax)
	oo_VEC (x, numberOfPoints)
	oo_VEC (y, numberOfPoints) // in dB
	oo_OBJECT (Matrix, 2, asdBs) // for peak detection
	oo_OBJECT (SlopeSelector, 0, slopeSelector)
	oo_BOOLEAN (slopeKnown)
	oo_BOOLEAN (peakKnown)
	oo_BOOLEAN (trendSubtracted)
	oo_ENUM (kSlopeSelector_method, method)
	oo_ENUM (kCepstrum_trendType, trendLineType)
	oo_ENUM (kVector_peakInterpolation, peakInterpolationType)
	oo_DOUBLE (qminSearchInterval) // peak in [pitchFloor, pitchCeiling]
	oo_DOUBLE (qmaxSearchInterval)
	oo_DOUBLE (slope)
	oo_DOUBLE (intercept)
	oo_DOUBLE (cpp) // peakdB - trenddB
	oo_DOUBLE (trenddB)
	oo_DOUBLE (peakdB)
	oo_DOUBLE (peakQuefrency)
	oo_INTEGER (maximumNumberOfRhamonics)
	oo_INTEGER (numberOfRhamonics)
	oo_MAT (rhamonics, maximumNumberOfRhamonics, 5_integer) // power, q1, q, q2, amplitude

	#if oo_DECLARING

		void newData (constPowerCepstrum);

		void getSlopeAndIntercept ();

		void getPeakAndPosition ();

		void subtractTrend ();

		double getTrend (double quefrency);

		void getCPP ();

		void todBs ();
		
		void fromdBs (PowerCepstrum me);
		
		void setMaximumNumberOfRhamonics (integer maximumNumberOfRhamonics);
		
		void getNumberOfRhamonics (double qmin, double qmax);
		
		void getRhamonicPeaks (double qmin, double qmax);
		
		void getRhamonicsPower (double qmin, double qmax, double f0fractionalWidth);
				
		double getRNR (double qmin, double qmax, double f0fractionalWidth);
		
	#endif

oo_END_CLASS (PowerCepstrumWorkspace)
#undef ooSTRUCT

/* End of file PowerCepstrumToMatrixWorkspace_def.h */
