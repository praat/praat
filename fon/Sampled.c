/* Sampled.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2004/10/23 made Sampled_getWindowSamples resistant to large times
 * pb 2004/10/24 getValueAtSample
 * pb 2004/10/31 Function ranges
 * pb 2004/11/06 better interpolation in marginal cases in Sampled_getMean
 * pb 2004/11/08 Sampled_getIntegral
 * pb 2004/11/25 corrected crashing bug in Sampled_getSumAndDefinitionRange
 * pb 2005/03/21 implemented Sampled_getStandardDeviation
 * pb 2005/06/16 removed units
 * pb 2007/10/01 can write as encoding
 * pb 2008/01/19 double
 * pb 2008/09/20 shiftXBy
 */

#include <math.h>
#include "Sampled.h"

#include "oo_COPY.h"
#include "Sampled_def.h"
#include "oo_EQUAL.h"
#include "Sampled_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Sampled_def.h"
#include "oo_WRITE_TEXT.h"
#include "Sampled_def.h"
#include "oo_READ_TEXT.h"
#include "Sampled_def.h"
#include "oo_WRITE_BINARY.h"
#include "Sampled_def.h"
#include "oo_READ_BINARY.h"
#include "Sampled_def.h"
#include "oo_DESCRIPTION.h"
#include "Sampled_def.h"

static double getNx (I) { iam (Sampled); return my nx; }
static double getDx (I) { iam (Sampled); return my dx; }
static double getX (I, long ix) { iam (Sampled); return my x1 + (ix - 1) * my dx; }

static double getValueAtSample (I, long isamp, long ilevel, int unit) {
	iam (Sampled);
	(void) me;
	(void) isamp;
	(void) ilevel;
	(void) unit;
	return NUMundefined;
}

static void shiftX (I, double xfrom, double xto) {
	iam (Sampled);
	inherited (Sampled) shiftX (me, xfrom, xto);
	NUMshift (& my x1, xfrom, xto);
}

class_methods (Sampled, Function) {
	class_method_local (Sampled, copy)
	class_method_local (Sampled, equal)
	class_method_local (Sampled, canWriteAsEncoding)
	class_method_local (Sampled, writeBinary)
	class_method_local (Sampled, readBinary)
	class_method_local (Sampled, writeText)
	class_method_local (Sampled, readText)
	class_method_local (Sampled, description)
	class_method (getNx)
	class_method (getDx)
	class_method (getX)
	class_method (getValueAtSample)
	class_method (shiftX)
	class_methods_end
}

double Sampled_indexToX (I, long i) {
	iam (Sampled);
	return my x1 + (i - 1) * my dx;
}

double Sampled_xToIndex (I, double x) {
	iam (Sampled);
	return (x - my x1) / my dx + 1;
}

long Sampled_xToLowIndex (I, double x) {
	iam (Sampled);
	return (long) floor ((x - my x1) / my dx) + 1;
}

long Sampled_xToHighIndex (I, double x) {
	iam (Sampled);
	return (long) ceil ((x - my x1) / my dx) + 1;
}

long Sampled_xToNearestIndex (I, double x) {
	iam (Sampled);
	return (long) floor ((x - my x1) / my dx + 1.5);
}

long Sampled_getWindowSamples (I, double xmin, double xmax, long *ixmin, long *ixmax) {
	iam (Sampled);
	double rixmin = 1.0 + ceil ((xmin - my x1) / my dx);
	double rixmax = 1.0 + floor ((xmax - my x1) / my dx);
	*ixmin = rixmin < 1.0 ? 1 : (long) rixmin;
	*ixmax = rixmax > (double) my nx ? my nx : (long) rixmax;
	if (*ixmin > *ixmax) return 0;
	return *ixmax - *ixmin + 1;
}

int Sampled_init (I, double xmin, double xmax, long nx, double dx, double x1) {
	iam (Sampled);
	my xmin = xmin; my xmax = xmax; my nx = nx; my dx = dx; my x1 = x1;
	return 1;
}

int Sampled_shortTermAnalysis (I, double windowDuration, double timeStep, long *numberOfFrames, double *firstTime) {
	iam (Sampled);
	double myDuration, thyDuration, ourMidTime;
	Melder_assert (windowDuration > 0.0);
	Melder_assert (timeStep > 0.0);
	myDuration = my dx * my nx;
	if (windowDuration > myDuration)
		return Melder_error2 (Thing_className (me), L" shorter than window length."); 
	*numberOfFrames = floor ((myDuration - windowDuration) / timeStep) + 1;
	Melder_assert (*numberOfFrames >= 1);
	ourMidTime = my x1 - 0.5 * my dx + 0.5 * myDuration;
	thyDuration = *numberOfFrames * timeStep;
	*firstTime = ourMidTime - 0.5 * thyDuration + 0.5 * timeStep;
	return 1;
}

double Sampled_getValueAtSample (I, long isamp, long ilevel, int unit) {
	iam (Sampled);
	if (isamp < 1 || isamp > my nx) return NUMundefined;
	return our getValueAtSample (me, isamp, ilevel, unit);
}

double Sampled_getValueAtX (I, double x, long ilevel, int unit, int interpolate) {
	iam (Sampled);
	if (x < my xmin || x > my xmax) return NUMundefined;
	if (interpolate) {
		double ireal = Sampled_xToIndex (me, x), phase, fnear, ffar;
		long ileft = floor (ireal), inear, ifar;
		phase = ireal - ileft;
		if (phase < 0.5) {
			inear = ileft, ifar = ileft + 1;
		} else {
			ifar = ileft, inear = ileft + 1;
			phase = 1.0 - phase;
		}
		if (inear < 1 || inear > my nx) return NUMundefined;   /* X out of range? */
		fnear = our getValueAtSample (me, inear, ilevel, unit);
		if (fnear == NUMundefined) return NUMundefined;   /* Function value not defined? */
		if (ifar < 1 || ifar > my nx) return fnear;   /* At edge? Extrapolate. */
		ffar = our getValueAtSample (me, ifar, ilevel, unit);
		if (ffar == NUMundefined) return fnear;   /* Neighbour undefined? Extrapolate. */
		return fnear + phase * (ffar - fnear);   /* Interpolate. */
	}
	return Sampled_getValueAtSample (me, Sampled_xToNearestIndex (me, x), ilevel, unit);
}

long Sampled_countDefinedSamples (I, long ilevel, int unit) {
	iam (Sampled);
	long isamp, numberOfDefinedSamples = 0;
	for (isamp = 1; isamp <= my nx; isamp ++) {
		double value = our getValueAtSample (me, isamp, ilevel, unit);
		if (value == NUMundefined) continue;
		numberOfDefinedSamples += 1;
	}
	return numberOfDefinedSamples;
}

double * Sampled_getSortedValues (I, long ilevel, int unit, long *return_numberOfValues) {
	iam (Sampled);
	long isamp, numberOfDefinedSamples = 0;
	double *values = NUMdvector (1, my nx);
	if (values == NULL) return NULL;
	for (isamp = 1; isamp <= my nx; isamp ++) {
		double value = our getValueAtSample (me, isamp, ilevel, unit);
		if (value == NUMundefined) continue;
		values [++ numberOfDefinedSamples] = value;
	}
	if (numberOfDefinedSamples) NUMsort_d (numberOfDefinedSamples, values);
	if (return_numberOfValues) *return_numberOfValues = numberOfDefinedSamples;
	return values;
}

double Sampled_getQuantile (I, double xmin, double xmax, double quantile, long ilevel, int unit) {
	iam (Sampled);
	long i, imin, imax, numberOfDefinedSamples = 0;
	double *values = NUMdvector (1, my nx), result = NUMundefined;
	iferror return NUMundefined;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (! Function_intersectRangeWithDomain (me, & xmin, & xmax)) return NUMundefined;
	Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	for (i = imin; i <= imax; i ++) {
		double value = our getValueAtSample (me, i, ilevel, unit);
		if (NUMdefined (value)) {
			values [++ numberOfDefinedSamples] = value;
		}
	}
	if (numberOfDefinedSamples >= 1) {
		NUMsort_d (numberOfDefinedSamples, values);
		result = NUMquantile (numberOfDefinedSamples, values, quantile);
	}
	NUMdvector_free (values, 1);
	return result;
}

static void Sampled_getSumAndDefinitionRange
	(I, double xmin, double xmax, long ilevel, int unit, int interpolate, double *return_sum, double *return_definitionRange)
{
	/*
		This function computes the area under the linearly interpolated curve between xmin and xmax.
		Outside [x1-dx/2, xN+dx/2], the curve is undefined and neither times nor values are counted.
		In [x1-dx/2,x1] and [xN,xN+dx/2], the curve is linearly extrapolated.
	*/
	iam (Sampled);
	long imin, imax, isamp;
	double sum = 0.0, definitionRange = 0.0;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (Function_intersectRangeWithDomain (me, & xmin, & xmax)) {
		if (interpolate) {
			if (Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
				double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
				for (isamp = imin; isamp <= imax; isamp ++) {
					double value = our getValueAtSample (me, isamp, ilevel, unit);   /* A fast way to integrate a linearly interpolated curve; works everywhere except at the edges. */
					if (NUMdefined (value)) {
						definitionRange += 1.0;
						sum += value;
					}
				}
				/*
				 * Corrections within the first and last sampling intervals.
				 */
				if (xmin > leftEdge) {   /* Otherwise, constant extrapolation over 0.5 sample is OK. */
					double phase = (my x1 + (imin - 1) * my dx - xmin) / my dx;   /* This fraction of sampling interval is still to be determined. */
					double rightValue = Sampled_getValueAtSample (me, imin, ilevel, unit);
					double leftValue = Sampled_getValueAtSample (me, imin - 1, ilevel, unit);
					if (NUMdefined (rightValue)) {
						definitionRange -= 0.5;   /* Delete constant extrapolation over 0.5 sample. */
						sum -= 0.5 * rightValue;
						if (NUMdefined (leftValue)) {
							definitionRange += phase;   /* Add current fraction. */
							sum += phase * (rightValue + 0.5 * phase * (leftValue - rightValue));   /* Interpolate to outside sample. */
						} else {
							if (phase > 0.5) phase = 0.5;
							definitionRange += phase;   /* Add current fraction, but never more than 0.5. */
							sum += phase * rightValue;
						}
					} else if (NUMdefined (leftValue) && phase > 0.5) {
						definitionRange += phase - 0.5;
						sum += (phase - 0.5) * leftValue;
					}
				}
				if (xmax < rightEdge) {   /* Otherwise, constant extrapolation is OK. */
					double phase = (xmax - (my x1 + (imax - 1) * my dx)) / my dx;   /* This fraction of sampling interval is still to be determined. */
					double leftValue = Sampled_getValueAtSample (me, imax, ilevel, unit);
					double rightValue = Sampled_getValueAtSample (me, imax + 1, ilevel, unit);
					if (NUMdefined (leftValue)) {
						definitionRange -= 0.5;   /* Delete constant extrapolation over 0.5 sample. */
						sum -= 0.5 * leftValue;
						if (NUMdefined (rightValue)) {
							definitionRange += phase;   /* Add current fraction. */
							sum += phase * (leftValue + 0.5 * phase * (rightValue - leftValue));   /* Interpolate to outside sample. */
						} else {
							if (phase > 0.5) phase = 0.5;
							definitionRange += phase;   /* Add current fraction, but never more than 0.5. */
							sum += phase * leftValue;
						}
					} else if (NUMdefined (rightValue) && phase > 0.5) {
						definitionRange += phase - 0.5;
						sum += (phase - 0.5) * rightValue;
					}
				}
			} else {   /* No sample centres between xmin and xmax. */
				/*
				 * Try to return the mean of the interpolated values at these two points.
				 * Thus, a small (xmin, xmax) range gives the same value as the (xmin+xmax)/2 point.
				 */
				double leftValue = Sampled_getValueAtSample (me, imax, ilevel, unit);
				double rightValue = Sampled_getValueAtSample (me, imin, ilevel, unit);
				double phase1 = (xmin - (my x1 + (imax - 1) * my dx)) / my dx;
				double phase2 = (xmax - (my x1 + (imax - 1) * my dx)) / my dx;
				if (imin == imax + 1) {   /* Not too far from sample definition region. */
					if (NUMdefined (leftValue)) {
						if (NUMdefined (rightValue)) {
							definitionRange += phase2 - phase1;
							sum += (phase2 - phase1) * (leftValue + 0.5 * (phase1 + phase2) * (rightValue - leftValue));
						} else if (phase1 < 0.5) {
							if (phase2 > 0.5) phase2 = 0.5;
							definitionRange += phase2 - phase1;
							sum += (phase2 - phase1) * leftValue;
						}
					} else if (NUMdefined (rightValue) && phase2 > 0.5) {
						if (phase1 < 0.5) phase1 = 0.5;
						definitionRange += phase2 - phase1;
						sum += (phase2 - phase1) * rightValue;
					}
				}
			}
		} else {   /* No interpolation. */
			double rimin = Sampled_xToIndex (me, xmin), rimax = Sampled_xToIndex (me, xmax);
			if (rimax >= 0.5 && rimin < my nx + 0.5) {
				imin = rimin < 0.5 ? 0 : (long) floor (rimin + 0.5);
				imax = rimax >= my nx + 0.5 ? my nx + 1 : (long) floor (rimax + 0.5);
				for (isamp = imin + 1; isamp < imax; isamp ++) {
					double value = our getValueAtSample (me, isamp, ilevel, unit);
					if (NUMdefined (value)) {
						definitionRange += 1.0;
						sum += value;
					}
				}
				if (imin == imax) {
					double value = our getValueAtSample (me, imin, ilevel, unit);
					if (NUMdefined (value)) {
						double phase = rimax - rimin;
						definitionRange += phase;
						sum += phase * value;
					}
				} else {
					if (imin >= 1) {
						double value = our getValueAtSample (me, imin, ilevel, unit);
						if (NUMdefined (value)) {
							double phase = imin - rimin + 0.5;
							definitionRange += phase;
							sum += phase * value;
						}
					}
					if (imax <= my nx) {
						double value = our getValueAtSample (me, imax, ilevel, unit);
						if (NUMdefined (value)) {
							double phase = rimax - imax + 0.5;
							definitionRange += phase;
							sum += phase * value;
						}
					}
				}
			}
		}
	}
	if (return_sum) *return_sum = sum;
	if (return_definitionRange) *return_definitionRange = definitionRange;
}

double Sampled_getMean (I, double xmin, double xmax, long ilevel, int unit, int interpolate) {
	iam (Sampled);
	double sum, definitionRange;
	Sampled_getSumAndDefinitionRange (me, xmin, xmax, ilevel, unit, interpolate, & sum, & definitionRange);
	return definitionRange <= 0.0 ? NUMundefined : sum / definitionRange;
}

double Sampled_getMean_standardUnit (I, double xmin, double xmax, long ilevel, int averagingUnit, int interpolate) {
	iam (Sampled);
	return ClassFunction_convertSpecialToStandardUnit (my methods, Sampled_getMean (me, xmin, xmax, ilevel, averagingUnit, interpolate), ilevel, averagingUnit);
}

double Sampled_getIntegral (I, double xmin, double xmax, long ilevel, int unit, int interpolate) {
	iam (Sampled);
	double sum, definitionRange;
	Sampled_getSumAndDefinitionRange (me, xmin, xmax, ilevel, unit, interpolate, & sum, & definitionRange);
	return sum * my dx;
}

double Sampled_getIntegral_standardUnit (I, double xmin, double xmax, long ilevel, int averagingUnit, int interpolate) {
	iam (Sampled);
	return ClassFunction_convertSpecialToStandardUnit (my methods, Sampled_getIntegral (me, xmin, xmax, ilevel, averagingUnit, interpolate), ilevel, averagingUnit);
}

static void Sampled_getSum2AndDefinitionRange
	(I, double xmin, double xmax, long ilevel, int unit, double mean, int interpolate, double *return_sum2, double *return_definitionRange)
{
	/*
		This function computes the area under the linearly interpolated squared difference curve between xmin and xmax.
		Outside [x1-dx/2, xN+dx/2], the curve is undefined and neither times nor values are counted.
		In [x1-dx/2,x1] and [xN,xN+dx/2], the curve is linearly extrapolated.
	*/
	iam (Sampled);
	long imin, imax, isamp;
	double sum2 = 0.0, definitionRange = 0.0;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (Function_intersectRangeWithDomain (me, & xmin, & xmax)) {
		if (interpolate) {
			if (Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
				double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
				for (isamp = imin; isamp <= imax; isamp ++) {
					double value = our getValueAtSample (me, isamp, ilevel, unit);   /* A fast way to integrate a linearly interpolated curve; works everywhere except at the edges. */
					if (NUMdefined (value)) {
						value -= mean;
						value *= value;
						definitionRange += 1.0;
						sum2 += value;
					}
				}
				/*
				 * Corrections within the first and last sampling intervals.
				 */
				if (xmin > leftEdge) {   /* Otherwise, constant extrapolation over 0.5 sample is OK. */
					double phase = (my x1 + (imin - 1) * my dx - xmin) / my dx;   /* This fraction of sampling interval is still to be determined. */
					double rightValue = Sampled_getValueAtSample (me, imin, ilevel, unit);
					double leftValue = Sampled_getValueAtSample (me, imin - 1, ilevel, unit);
					if (NUMdefined (rightValue)) {
						rightValue -= mean;
						rightValue *= rightValue;
						definitionRange -= 0.5;   /* Delete constant extrapolation over 0.5 sample. */
						sum2 -= 0.5 * rightValue;
						if (NUMdefined (leftValue)) {
							leftValue -= mean;
							leftValue *= leftValue;
							definitionRange += phase;   /* Add current fraction. */
							sum2 += phase * (rightValue + 0.5 * phase * (leftValue - rightValue));   /* Interpolate to outside sample. */
						} else {
							if (phase > 0.5) phase = 0.5;
							definitionRange += phase;   /* Add current fraction, but never more than 0.5. */
							sum2 += phase * rightValue;
						}
					} else if (NUMdefined (leftValue) && phase > 0.5) {
						leftValue -= mean;
						leftValue *= leftValue;
						definitionRange += phase - 0.5;
						sum2 += (phase - 0.5) * leftValue;
					}
				}
				if (xmax < rightEdge) {   /* Otherwise, constant extrapolation is OK. */
					double phase = (xmax - (my x1 + (imax - 1) * my dx)) / my dx;   /* This fraction of sampling interval is still to be determined. */
					double leftValue = Sampled_getValueAtSample (me, imax, ilevel, unit);
					double rightValue = Sampled_getValueAtSample (me, imax + 1, ilevel, unit);
					if (NUMdefined (leftValue)) {
						leftValue -= mean;
						leftValue *= leftValue;
						definitionRange -= 0.5;   /* Delete constant extrapolation over 0.5 sample. */
						sum2 -= 0.5 * leftValue;
						if (NUMdefined (rightValue)) {
							rightValue -= mean;
							rightValue *= rightValue;
							definitionRange += phase;   /* Add current fraction. */
							sum2 += phase * (leftValue + 0.5 * phase * (rightValue - leftValue));   /* Interpolate to outside sample. */
						} else {
							if (phase > 0.5) phase = 0.5;
							definitionRange += phase;   /* Add current fraction, but never more than 0.5. */
							sum2 += phase * leftValue;
						}
					} else if (NUMdefined (rightValue) && phase > 0.5) {
						rightValue -= mean;
						rightValue *= rightValue;
						definitionRange += phase - 0.5;
						sum2 += (phase - 0.5) * rightValue;
					}
				}
			} else {   /* No sample centres between xmin and xmax. */
				/*
				 * Try to return the mean of the interpolated values at these two points.
				 * Thus, a small (xmin, xmax) range gives the same value as the (xmin+xmax)/2 point.
				 */
				double leftValue = Sampled_getValueAtSample (me, imax, ilevel, unit);
				double rightValue = Sampled_getValueAtSample (me, imin, ilevel, unit);
				double phase1 = (xmin - (my x1 + (imax - 1) * my dx)) / my dx;
				double phase2 = (xmax - (my x1 + (imax - 1) * my dx)) / my dx;
				if (imin == imax + 1) {   /* Not too far from sample definition region. */
					if (NUMdefined (leftValue)) {
						leftValue -= mean;
						leftValue *= leftValue;
						if (NUMdefined (rightValue)) {
							rightValue -= mean;
							rightValue *= rightValue;
							definitionRange += phase2 - phase1;
							sum2 += (phase2 - phase1) * (leftValue + 0.5 * (phase1 + phase2) * (rightValue - leftValue));
						} else if (phase1 < 0.5) {
							if (phase2 > 0.5) phase2 = 0.5;
							definitionRange += phase2 - phase1;
							sum2 += (phase2 - phase1) * leftValue;
						}
					} else if (NUMdefined (rightValue) && phase2 > 0.5) {
						rightValue -= mean;
						rightValue *= rightValue;
						if (phase1 < 0.5) phase1 = 0.5;
						definitionRange += phase2 - phase1;
						sum2 += (phase2 - phase1) * rightValue;
					}
				}
			}
		} else {   /* No interpolation. */
			double rimin = Sampled_xToIndex (me, xmin), rimax = Sampled_xToIndex (me, xmax);
			if (rimax >= 0.5 && rimin < my nx + 0.5) {
				imin = rimin < 0.5 ? 0 : (long) floor (rimin + 0.5);
				imax = rimax >= my nx + 0.5 ? my nx + 1 : (long) floor (rimax + 0.5);
				for (isamp = imin + 1; isamp < imax; isamp ++) {
					double value = our getValueAtSample (me, isamp, ilevel, unit);
					if (NUMdefined (value)) {
						value -= mean;
						value *= value;
						definitionRange += 1.0;
						sum2 += value;
					}
				}
				if (imin == imax) {
					double value = our getValueAtSample (me, imin, ilevel, unit);
					if (NUMdefined (value)) {
						double phase = rimax - rimin;
						value -= mean;
						value *= value;
						definitionRange += phase;
						sum2 += phase * value;
					}
				} else {
					if (imin >= 1) {
						double value = our getValueAtSample (me, imin, ilevel, unit);
						if (NUMdefined (value)) {
							double phase = imin - rimin + 0.5;
							value -= mean;
							value *= value;
							definitionRange += phase;
							sum2 += phase * value;
						}
					}
					if (imax <= my nx) {
						double value = our getValueAtSample (me, imax, ilevel, unit);
						if (NUMdefined (value)) {
							double phase = rimax - imax + 0.5;
							value -= mean;
							value *= value;
							definitionRange += phase;
							sum2 += phase * value;
						}
					}
				}
			}
		}
	}
	if (return_sum2) *return_sum2 = sum2;
	if (return_definitionRange) *return_definitionRange = definitionRange;
}

double Sampled_getStandardDeviation (I, double xmin, double xmax, long ilevel, int unit, int interpolate) {
	iam (Sampled);
	double sum, sum2, definitionRange;
	Sampled_getSumAndDefinitionRange (me, xmin, xmax, ilevel, unit, interpolate, & sum, & definitionRange);
	if (definitionRange < 2.0) return NUMundefined;
	Sampled_getSum2AndDefinitionRange (me, xmin, xmax, ilevel, unit, sum / definitionRange, interpolate, & sum2, & definitionRange);
	return sqrt (sum2 / (definitionRange - 1.0));
}

double Sampled_getStandardDeviation_standardUnit (I, double xmin, double xmax, long ilevel, int averagingUnit, int interpolate) {
	iam (Sampled);
	return ClassFunction_convertSpecialToStandardUnit (my methods, Sampled_getStandardDeviation (me, xmin, xmax, ilevel, averagingUnit, interpolate), ilevel, averagingUnit);
}

void Sampled_getMinimumAndX (I, double xmin, double xmax, long ilevel, int unit, int interpolate,
	double *return_minimum, double *return_xOfMinimum)
{
	iam (Sampled);
	long imin, imax, i;
	double minimum = 1e301, xOfMinimum = 0.0;
	if (xmin == NUMundefined || xmax == NUMundefined) {
		minimum = xOfMinimum = NUMundefined;
		goto end;
	}
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (! Function_intersectRangeWithDomain (me, & xmin, & xmax)) {
		minimum = xOfMinimum = NUMundefined;   /* Requested range and logical domain do not intersect. */
		goto end;
	}
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
		 * No sample centres between xmin and xmax.
		 * Try to return the lesser of the values at these two points.
		 */
		double fleft = Sampled_getValueAtX (me, xmin, ilevel, unit, interpolate);
		double fright = Sampled_getValueAtX (me, xmax, ilevel, unit, interpolate);
		if (NUMdefined (fleft) && fleft < minimum) minimum = fleft, xOfMinimum = xmin;
		if (NUMdefined (fright) && fright < minimum) minimum = fright, xOfMinimum = xmax;
	} else {
		for (i = imin; i <= imax; i ++) {
			double fmid = our getValueAtSample (me, i, ilevel, unit);
			if (fmid == NUMundefined) continue;
			if (interpolate == FALSE) {
				if (fmid < minimum) minimum = fmid, xOfMinimum = i;
			} else {
				/*
				 * Try an interpolation, possibly even taking into account a sample just outside the selection.
				 */
				double fleft = i <= 1 ? NUMundefined : our getValueAtSample (me, i - 1, ilevel, unit);
				double fright = i >= my nx ? NUMundefined : our getValueAtSample (me, i + 1, ilevel, unit);
				if (fleft == NUMundefined || fright == NUMundefined) {
					if (fmid < minimum) minimum = fmid, xOfMinimum = i;
				} else if (fmid < fleft && fmid <= fright) {
					double y [4], i_real, localMinimum;
					y [1] = fleft, y [2] = fmid, y [3] = fright;
					localMinimum = NUMimproveMinimum (y, 3, 2, NUM_PEAK_INTERPOLATE_PARABOLIC, & i_real);
					if (localMinimum < minimum)
						minimum = localMinimum, xOfMinimum = i_real + i - 2;
				}
			}
		}
		xOfMinimum = my x1 + (xOfMinimum - 1) * my dx;   /* From index plus phase to time. */
		/* Check boundary values. */
		if (interpolate) {
			double fleft = Sampled_getValueAtX (me, xmin, ilevel, unit, TRUE);
			double fright = Sampled_getValueAtX (me, xmax, ilevel, unit, TRUE);
			if (NUMdefined (fleft) && fleft < minimum) minimum = fleft, xOfMinimum = xmin;
			if (NUMdefined (fright) && fright < minimum) minimum = fright, xOfMinimum = xmax;
		}
		if (xOfMinimum < xmin) xOfMinimum = xmin;
		if (xOfMinimum > xmax) xOfMinimum = xmax;
	}
	if (minimum == 1e301) minimum = xOfMinimum = NUMundefined;
end:
	if (return_minimum) *return_minimum = minimum;
	if (return_xOfMinimum) *return_xOfMinimum = xOfMinimum;
}

double Sampled_getMinimum (I, double xmin, double xmax, long ilevel, int unit, int interpolate) {
	iam (Sampled);
	double minimum;
	Sampled_getMinimumAndX (me, xmin, xmax, ilevel, unit, interpolate, & minimum, NULL);
	return minimum;
}

double Sampled_getXOfMinimum (I, double xmin, double xmax, long ilevel, int unit, int interpolate) {
	iam (Sampled);
	double time;
	Sampled_getMinimumAndX (me, xmin, xmax, ilevel, unit, interpolate, NULL, & time);
	return time;
}

void Sampled_getMaximumAndX (I, double xmin, double xmax, long ilevel, int unit, int interpolate,
	double *return_maximum, double *return_xOfMaximum)
{
	iam (Sampled);
	long imin, imax, i;
	double maximum = -1e301, xOfMaximum = 0.0;
	if (xmin == NUMundefined || xmax == NUMundefined) {
		maximum = xOfMaximum = NUMundefined;
		goto end;
	}
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (! Function_intersectRangeWithDomain (me, & xmin, & xmax)) {
		maximum = xOfMaximum = NUMundefined;   /* Requested range and logical domain do not intersect. */
		goto end;
	}
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
		 * No sample centres between tmin and tmax.
		 * Try to return the greater of the values at these two points.
		 */
		double fleft = Sampled_getValueAtX (me, xmin, ilevel, unit, interpolate);
		double fright = Sampled_getValueAtX (me, xmax, ilevel, unit, interpolate);
		if (NUMdefined (fleft) && fleft > maximum) maximum = fleft, xOfMaximum = xmin;
		if (NUMdefined (fright) && fright > maximum) maximum = fright, xOfMaximum = xmax;
	} else {
		for (i = imin; i <= imax; i ++) {
			double fmid = our getValueAtSample (me, i, ilevel, unit);
			if (fmid == NUMundefined) continue;
			if (interpolate == FALSE) {
				if (fmid > maximum) maximum = fmid, xOfMaximum = i;
			} else {
				/*
				 * Try an interpolation, possibly even taking into account a sample just outside the selection.
				 */
				double fleft = i <= 1 ? NUMundefined : our getValueAtSample (me, i - 1, ilevel, unit);
				double fright = i >= my nx ? NUMundefined : our getValueAtSample (me, i + 1, ilevel, unit);
				if (fleft == NUMundefined || fright == NUMundefined) {
					if (fmid > maximum) maximum = fmid, xOfMaximum = i;
				} else if (fmid > fleft && fmid >= fright) {
					double y [4], i_real, localMaximum;
					y [1] = fleft, y [2] = fmid, y [3] = fright;
					localMaximum = NUMimproveMaximum (y, 3, 2, NUM_PEAK_INTERPOLATE_PARABOLIC, & i_real);
					if (localMaximum > maximum)
						maximum = localMaximum, xOfMaximum = i_real + i - 2;
				}
			}
		}
		xOfMaximum = my x1 + (xOfMaximum - 1) * my dx;   /* From index plus phase to time. */
		/* Check boundary values. */
		if (interpolate) {
			double fleft = Sampled_getValueAtX (me, xmin, ilevel, unit, TRUE);
			double fright = Sampled_getValueAtX (me, xmax, ilevel, unit, TRUE);
			if (NUMdefined (fleft) && fleft > maximum) maximum = fleft, xOfMaximum = xmin;
			if (NUMdefined (fright) && fright > maximum) maximum = fright, xOfMaximum = xmax;
		}
		if (xOfMaximum < xmin) xOfMaximum = xmin;
		if (xOfMaximum > xmax) xOfMaximum = xmax;
	}
	if (maximum == -1e301) maximum = xOfMaximum = NUMundefined;
end:
	if (return_maximum) *return_maximum = maximum;
	if (return_xOfMaximum) *return_xOfMaximum = xOfMaximum;
}

double Sampled_getMaximum (I, double xmin, double xmax, long ilevel, int unit, int interpolate) {
	iam (Sampled);
	double maximum;
	Sampled_getMaximumAndX (me, xmin, xmax, ilevel, unit, interpolate, & maximum, NULL);
	return maximum;
}

double Sampled_getXOfMaximum (I, double xmin, double xmax, long ilevel, int unit, int interpolate) {
	iam (Sampled);
	double time;
	Sampled_getMaximumAndX (me, xmin, xmax, ilevel, unit, interpolate, NULL, & time);
	return time;
}

static void Sampled_speckleInside (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double speckle_mm, long ilevel, int unit)
{
	iam (Sampled);
	long ixmin, ixmax, ix;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	Sampled_getWindowSamples (me, xmin, xmax, & ixmin, & ixmax);
	if (ClassFunction_isUnitLogarithmic (my methods, ilevel, unit)) {
		ymin = ClassFunction_convertStandardToSpecialUnit (my methods, ymin, ilevel, unit);
		ymax = ClassFunction_convertStandardToSpecialUnit (my methods, ymax, ilevel, unit);
	}
	if (ymax <= ymin) return;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (ix = ixmin; ix <= ixmax; ix ++) {
		double value = Sampled_getValueAtSample (me, ix, ilevel, unit);
		if (NUMdefined (value)) {
			double x = Sampled_indexToX (me, ix);
			if (value >= ymin && value <= ymax) {
				Graphics_fillCircle_mm (g, x, value, speckle_mm);
			}
		}
	}
}

void Sampled_drawInside (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double speckle_mm, long ilevel, int unit)
{
	iam (Sampled);
	long ixmin, ixmax, ix, startOfDefinedStretch = -1;
	double *xarray = NULL, *yarray = NULL;
	double previousValue = NUMundefined;
	if (speckle_mm != 0.0) {
		Sampled_speckleInside (me, g, xmin, xmax, ymin, ymax, speckle_mm, ilevel, unit);
		return;
	}
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	Sampled_getWindowSamples (me, xmin, xmax, & ixmin, & ixmax);
	if (ClassFunction_isUnitLogarithmic (my methods, ilevel, unit)) {
		ymin = ClassFunction_convertStandardToSpecialUnit (my methods, ymin, ilevel, unit);
		ymax = ClassFunction_convertStandardToSpecialUnit (my methods, ymax, ilevel, unit);
	}
	if (ymax <= ymin) return;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	xarray = NUMdvector (ixmin - 1, ixmax + 1); cherror
	yarray = NUMdvector (ixmin - 1, ixmax + 1); cherror
	previousValue = Sampled_getValueAtSample (me, ixmin - 1, ilevel, unit);
	if (NUMdefined (previousValue)) {
		startOfDefinedStretch = ixmin - 1;
		xarray [ixmin - 1] = Sampled_indexToX (me, ixmin - 1);
		yarray [ixmin - 1] = previousValue;
	}
	for (ix = ixmin; ix <= ixmax; ix ++) {
		double x = Sampled_indexToX (me, ix), value = Sampled_getValueAtSample (me, ix, ilevel, unit);
		if (NUMdefined (value)) {
			if (NUMdefined (previousValue)) {
				xarray [ix] = x;
				yarray [ix] = value;
			} else {
				startOfDefinedStretch = ix - 1;
				xarray [ix - 1] = x - 0.5 * my dx;
				yarray [ix - 1] = value;
				xarray [ix] = x;
				yarray [ix] = value;
			}
		} else if (NUMdefined (previousValue)) {
			Melder_assert (startOfDefinedStretch >= ixmin - 1);
			if (ix > ixmin) {
				xarray [ix] = x - 0.5 * my dx;
				yarray [ix] = previousValue;
				if (xarray [startOfDefinedStretch] < xmin) {
					double phase = (xmin - xarray [startOfDefinedStretch]) / my dx;
					xarray [startOfDefinedStretch] = xmin;
					yarray [startOfDefinedStretch] = phase * yarray [startOfDefinedStretch + 1] + (1.0 - phase) * yarray [startOfDefinedStretch];
				}
				Graphics_polyline (g, ix + 1 - startOfDefinedStretch, & xarray [startOfDefinedStretch], & yarray [startOfDefinedStretch]);
			}
			startOfDefinedStretch = -1;
		}
		previousValue = value;
	}
	if (startOfDefinedStretch > -1) {
		double x = Sampled_indexToX (me, ixmax + 1), value = Sampled_getValueAtSample (me, ixmax + 1, ilevel, unit);
		Melder_assert (NUMdefined (previousValue));
		if (NUMdefined (value)) {
			xarray [ixmax + 1] = x;
			yarray [ixmax + 1] = value;
		} else {
			xarray [ixmax + 1] = x - 0.5 * my dx;
			yarray [ixmax + 1] = previousValue;
		}
		if (xarray [startOfDefinedStretch] < xmin) {
			double phase = (xmin - xarray [startOfDefinedStretch]) / my dx;
			xarray [startOfDefinedStretch] = xmin;
			yarray [startOfDefinedStretch] = phase * yarray [startOfDefinedStretch + 1] + (1.0 - phase) * yarray [startOfDefinedStretch];
		}
		if (xarray [ixmax + 1] > xmax) {
			double phase = (xarray [ixmax + 1] - xmax) / my dx;
			xarray [ixmax + 1] = xmax;
			yarray [ixmax + 1] = phase * yarray [ixmax] + (1.0 - phase) * yarray [ixmax + 1];
		}
		Graphics_polyline (g, ixmax + 2 - startOfDefinedStretch, & xarray [startOfDefinedStretch], & yarray [startOfDefinedStretch]);
	}
end:
	NUMdvector_free (xarray, ixmin - 1);
	NUMdvector_free (yarray, ixmin - 1);
	Melder_clearError ();
}

/* End of file Sampled.c */
