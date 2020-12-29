/* Sampled.cpp
 *
 * Copyright (C) 1992-2005,2007,2008,2011,2012,2014-2020 Paul Boersma
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

#include "Sampled.h"

#include "oo_DESTROY.h"
#include "Sampled_def.h"
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

Thing_implement (Sampled, Function, 0);

void structSampled :: v_shiftX (double xfrom, double xto) {
	Sampled_Parent :: v_shiftX (xfrom, xto);
	NUMshift (& our x1, xfrom, xto);
}

void structSampled :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	Sampled_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	NUMscale (& our x1, xminfrom, xmaxfrom, xminto, xmaxto);
	our dx *= (xmaxto - xminto) / (xmaxfrom - xminfrom);
}

integer Sampled_getWindowSamples (Sampled me, double xmin, double xmax, integer *ixmin, integer *ixmax) {
	const double ixmin_real = 1.0 + Melder_roundUp   ((xmin - my x1) / my dx);
	const double ixmax_real = 1.0 + Melder_roundDown ((xmax - my x1) / my dx);   // could be above 32-bit LONG_MAX
	*ixmin = ( ixmin_real < 1.0 ? 1 : (integer) ixmin_real );
	*ixmax = ( ixmax_real > (double) my nx ? my nx : (integer) ixmax_real );
	if (*ixmin > *ixmax)
		return 0;
	return *ixmax - *ixmin + 1;
}

void Sampled_init (Sampled me, double xmin, double xmax, integer nx, double dx, double x1) {
	my xmin = xmin;
	my xmax = xmax;
	my nx = nx;
	my dx = dx;
	my x1 = x1;
}

void Sampled_shortTermAnalysis (Sampled me, double windowDuration, double timeStep, integer *numberOfFrames, double *firstTime) {
	Melder_assert (windowDuration > 0.0);
	Melder_assert (timeStep > 0.0);
	volatile double myDuration = my dx * my nx;
	if (windowDuration > myDuration)
		Melder_throw (me, U": shorter than window length.");
	*numberOfFrames = Melder_ifloor ((myDuration - windowDuration) / timeStep) + 1;
	Melder_assert (*numberOfFrames >= 1);
	const double ourMidTime = my x1 - 0.5 * my dx + 0.5 * myDuration;
	const double thyDuration = *numberOfFrames * timeStep;
	*firstTime = ourMidTime - 0.5 * thyDuration + 0.5 * timeStep;
}

double Sampled_getValueAtSample (Sampled me, integer sampleNumber, integer levelNumber, int unit) {
	if (sampleNumber < 1 || sampleNumber > my nx)
		return undefined;
	return my v_getValueAtSample (sampleNumber, levelNumber, unit);
}

autoVEC Sampled_listValuesOfAllSamples (Sampled me, integer levelNumber, int unit) {
	autoVEC result = raw_VEC (my nx);
	for (integer isamp = 1; isamp <= my nx; isamp ++)
		result [isamp] = my v_getValueAtSample (isamp, levelNumber, unit);
	return result;
}

autoVEC Sampled_listValuesAtXes (Sampled me, constVECVU const& xes, integer levelNumber, int unit, bool interpolate) {
	autoVEC result = raw_VEC (xes.size);
	for (integer ix = 1; ix <= xes.size; ix ++)
		result [ix] = Sampled_getValueAtX (me, xes [ix], levelNumber, unit, interpolate);
	return result;
}

double Sampled_getValueAtX (Sampled me, double x, integer levelNumber, int unit, bool interpolate) {
	if (x < my xmin || x > my xmax)
		return undefined;
	if (interpolate) {
		const double index_real = Sampled_xToIndex (me, x);
		const integer leftIndex = Melder_ifloor (index_real);
		integer nearIndex, farIndex;
		double phase = index_real - leftIndex;
		if (phase < 0.5) {
			nearIndex = leftIndex;
			farIndex = leftIndex + 1;
		} else {
			farIndex = leftIndex;
			nearIndex = leftIndex + 1;
			phase = 1.0 - phase;   // so that 0.0 < phase <= 0.5
		}
		if (nearIndex < 1 || nearIndex > my nx)   // x out of range?
			return undefined;
		const double nearValue = my v_getValueAtSample (nearIndex, levelNumber, unit);
		if (isundef (nearValue))   // function value not defined?
			return undefined;
		if (farIndex < 1 || farIndex > my nx)   // at edge?
			return nearValue;   // extrapolate
		const double farValue = my v_getValueAtSample (farIndex, levelNumber, unit);
		if (isundef (farValue))   // neighbour undefined?
			return nearValue;   // extrapolate
		return nearValue + phase * (farValue - nearValue);   // interpolate
	}
	return Sampled_getValueAtSample (me, Sampled_xToNearestIndex (me, x), levelNumber, unit);
}

static integer autoWindowDomainSamples (Sampled me, double *inout_xmin, double *inout_xmax, integer *out_imin, integer *out_imax) {
	Function_unidirectionalAutowindow (me, inout_xmin, inout_xmax);
	if (! Function_intersectRangeWithDomain (me, inout_xmin, inout_xmax))
		return 0;
	return Sampled_getWindowSamples (me, *inout_xmin, *inout_xmax, out_imin, out_imax);
}

integer Sampled_countDefinedSamples (Sampled me, double xmin, double xmax, integer levelNumber, int unit) {
	integer imin, imax;
	integer numberOfFrames = autoWindowDomainSamples (me, & xmin, & xmax, & imin, & imax);
	if (numberOfFrames < 1)
		return 0;
	integer numberOfDefinedSamples = 0;
	for (integer isamp = imin; isamp <= imax; isamp ++) {
		const double value = my v_getValueAtSample (isamp, levelNumber, unit);
		if (isdefined (value))
			numberOfDefinedSamples += 1;
	}
	return numberOfDefinedSamples;
}

autoVEC Sampled_getSortedValues (Sampled me, double xmin, double xmax, integer levelNumber, int unit) {
	integer numberOfDefinedSamples = Sampled_countDefinedSamples (me, xmin, xmax, levelNumber, unit);
	if (numberOfDefinedSamples == 0)
		return autoVEC();
	autoVEC definedValues = raw_VEC (numberOfDefinedSamples);
	integer imin, imax;
	autoWindowDomainSamples (me, & xmin, & xmax, & imin, & imax);
	integer definedSampleNumber = 0;
	for (integer isamp = imin; isamp <= imax; isamp ++) {
		const double value = my v_getValueAtSample (isamp, levelNumber, unit);
		if (isdefined (value))
			definedValues [++ definedSampleNumber] = value;
	}
	Melder_assert (definedSampleNumber == numberOfDefinedSamples);
	sort_VEC_inout (definedValues.get());
	return definedValues;
}

double Sampled_getQuantile (Sampled me, double xmin, double xmax, double quantile, integer levelNumber, int unit) {
	try {
		autoVEC values = Sampled_getSortedValues (me, xmin, xmax, levelNumber, unit);
		if (values.size == 0)
			return undefined;
		return NUMquantile (values.get(), quantile);
	} catch (MelderError) {
		Melder_throw (me, U": quantile not computed.");
	}
}

static void Sampled_getSumAndDefinitionRange
	(Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate, double *return_sum, double *return_definitionRange)
{
	/*
		This function computes the area under the linearly interpolated curve between xmin and xmax.
		Outside [x1-dx/2, xN+dx/2], the curve is undefined and neither times nor values are counted.
		In [x1-dx/2,x1] and [xN,xN+dx/2], the curve is linearly extrapolated.
	*/
	longdouble sum = 0.0, definitionRange = 0.0;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (Function_intersectRangeWithDomain (me, & xmin, & xmax)) {
		if (interpolate) {
			integer imin, imax;
			if (Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax) > 0) {
				double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
				for (integer isamp = imin; isamp <= imax; isamp ++) {
					const double value = my v_getValueAtSample (isamp, levelNumber, unit);   // a fast way to integrate a linearly interpolated curve; works everywhere except at the edges
					if (isdefined (value)) {
						definitionRange += 1.0;
						sum += value;
					}
				}
				/*
					Corrections within the first and last sampling intervals.
				*/
				if (xmin > leftEdge) {   // otherwise, constant extrapolation over 0.5 sample is OK
					double phase = (my x1 + (imin - 1) * my dx - xmin) / my dx;   // this fraction of sampling interval is still to be determined
					const double rightValue = Sampled_getValueAtSample (me, imin, levelNumber, unit);
					const double leftValue = Sampled_getValueAtSample (me, imin - 1, levelNumber, unit);
					if (isdefined (rightValue)) {
						definitionRange -= 0.5;   // delete constant extrapolation over 0.5 sample
						sum -= 0.5 * rightValue;
						if (isdefined (leftValue)) {
							definitionRange += phase;   // add current fraction
							sum += phase * (rightValue + 0.5 * phase * (leftValue - rightValue));   // interpolate to outside sample
						} else {
							if (phase > 0.5)
								phase = 0.5;
							definitionRange += phase;   // add current fraction, but never more than 0.5
							sum += phase * rightValue;
						}
					} else if (isdefined (leftValue) && phase > 0.5) {
						definitionRange += phase - 0.5;
						sum += (phase - 0.5) * leftValue;
					}
				}
				if (xmax < rightEdge) {   // otherwise, constant extrapolation is OK
					double phase = (xmax - (my x1 + (imax - 1) * my dx)) / my dx;   // this fraction of sampling interval is still to be determined
					const double leftValue = Sampled_getValueAtSample (me, imax, levelNumber, unit);
					const double rightValue = Sampled_getValueAtSample (me, imax + 1, levelNumber, unit);
					if (isdefined (leftValue)) {
						definitionRange -= 0.5;   // delete constant extrapolation over 0.5 sample
						sum -= 0.5 * leftValue;
						if (isdefined (rightValue)) {
							definitionRange += phase;   // add current fraction
							sum += phase * (leftValue + 0.5 * phase * (rightValue - leftValue));   // interpolate to outside sample
						} else {
							if (phase > 0.5)
								phase = 0.5;
							definitionRange += phase;   // add current fraction, but never more than 0.5
							sum += phase * leftValue;
						}
					} else if (isdefined (rightValue) && phase > 0.5) {
						definitionRange += phase - 0.5;
						sum += (phase - 0.5) * rightValue;
					}
				}
			} else {   // no sample centres between xmin and xmax
				/*
					Try to return the mean of the interpolated values at these two points.
					Thus, a small (xmin, xmax) range gives the same value as the (xmin+xmax)/2 point.
				*/
				const double leftValue = Sampled_getValueAtSample (me, imax, levelNumber, unit);
				const double rightValue = Sampled_getValueAtSample (me, imin, levelNumber, unit);
				double phase1 = (xmin - (my x1 + (imax - 1) * my dx)) / my dx;
				double phase2 = (xmax - (my x1 + (imax - 1) * my dx)) / my dx;
				if (imin == imax + 1) {   // not too far from sample definition region
					if (isdefined (leftValue)) {
						if (isdefined (rightValue)) {
							definitionRange += phase2 - phase1;
							sum += (phase2 - phase1) * (leftValue + 0.5 * (phase1 + phase2) * (rightValue - leftValue));
						} else if (phase1 < 0.5) {
							if (phase2 > 0.5)
								phase2 = 0.5;
							definitionRange += phase2 - phase1;
							sum += (phase2 - phase1) * leftValue;
						}
					} else if (isdefined (rightValue) && phase2 > 0.5) {
						if (phase1 < 0.5)
							phase1 = 0.5;
						definitionRange += phase2 - phase1;
						sum += (phase2 - phase1) * rightValue;
					}
				}
			}
		} else {   // no interpolation
			const double rimin = Sampled_xToIndex (me, xmin), rimax = Sampled_xToIndex (me, xmax);
			if (rimax >= 0.5 && rimin < my nx + 0.5) {
				const integer imin = ( rimin < 0.5 ? 0 : Melder_iround (rimin) );
				const integer imax = ( rimax >= my nx + 0.5 ? my nx + 1 : Melder_iround (rimax) );
				for (integer isamp = imin + 1; isamp < imax; isamp ++) {
					const double value = my v_getValueAtSample (isamp, levelNumber, unit);
					if (isdefined (value)) {
						definitionRange += 1.0;
						sum += value;
					}
				}
				if (imin == imax) {
					const double value = my v_getValueAtSample (imin, levelNumber, unit);
					if (isdefined (value)) {
						const double phase = rimax - rimin;
						definitionRange += phase;
						sum += phase * value;
					}
				} else {
					if (imin >= 1) {
						const double value = my v_getValueAtSample (imin, levelNumber, unit);
						if (isdefined (value)) {
							const double phase = imin - rimin + 0.5;
							definitionRange += phase;
							sum += phase * value;
						}
					}
					if (imax <= my nx) {
						const double value = my v_getValueAtSample (imax, levelNumber, unit);
						if (isdefined (value)) {
							const double phase = rimax - imax + 0.5;
							definitionRange += phase;
							sum += phase * value;
						}
					}
				}
			}
		}
	}
	if (return_sum) *return_sum = (double) sum;
	if (return_definitionRange) *return_definitionRange = (double) definitionRange;
}

double Sampled_getMean (Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate) {
	double sum, definitionRange;
	Sampled_getSumAndDefinitionRange (me, xmin, xmax, levelNumber, unit, interpolate, & sum, & definitionRange);
	return definitionRange <= 0.0 ? undefined : sum / definitionRange;
}

double Sampled_getMean_standardUnit (Sampled me, double xmin, double xmax, integer levelNumber, int averagingUnit, bool interpolate) {
	const double mean = Sampled_getMean (me, xmin, xmax, levelNumber, averagingUnit, interpolate);
	return Function_convertSpecialToStandardUnit (me, mean, levelNumber, averagingUnit);
}

double Sampled_getIntegral (Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate) {
	double sum, definitionRange;
	Sampled_getSumAndDefinitionRange (me, xmin, xmax, levelNumber, unit, interpolate, & sum, & definitionRange);
	return sum * my dx;
}

double Sampled_getIntegral_standardUnit (Sampled me, double xmin, double xmax, integer levelNumber, int averagingUnit, bool interpolate) {
	const double integral = Sampled_getIntegral (me, xmin, xmax, levelNumber, averagingUnit, interpolate);
	return Function_convertSpecialToStandardUnit (me, integral, levelNumber, averagingUnit);
}

static void Sampled_getSum2AndDefinitionRange
	(Sampled me, double xmin, double xmax, integer levelNumber, int unit, double mean, bool interpolate, double *return_sum2, double *return_definitionRange)
{
	/*
		This function computes the area under the linearly interpolated squared difference curve between xmin and xmax.
		Outside [x1-dx/2, xN+dx/2], the curve is undefined and neither times nor values are counted.
		In [x1-dx/2,x1] and [xN,xN+dx/2], the curve is linearly extrapolated.
	*/
	longdouble sum2 = 0.0, definitionRange = 0.0;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (Function_intersectRangeWithDomain (me, & xmin, & xmax)) {
		if (interpolate) {
			integer imin, imax;
			if (Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax) > 0) {
				const double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
				for (integer isamp = imin; isamp <= imax; isamp ++) {
					double value = my v_getValueAtSample (isamp, levelNumber, unit);   // a fast way to integrate a linearly interpolated curve; works everywhere except at the edges
					if (isdefined (value)) {
						value -= mean;
						value *= value;
						definitionRange += 1.0;
						sum2 += value;
					}
				}
				/*
					Corrections within the first and last sampling intervals.
				*/
				if (xmin > leftEdge) {   // otherwise, constant extrapolation over 0.5 sample is OK
					double phase = (my x1 + (imin - 1) * my dx - xmin) / my dx;   // this fraction of sampling interval is still to be determined
					double rightValue = Sampled_getValueAtSample (me, imin, levelNumber, unit);
					double leftValue = Sampled_getValueAtSample (me, imin - 1, levelNumber, unit);
					if (isdefined (rightValue)) {
						rightValue -= mean;
						rightValue *= rightValue;
						definitionRange -= 0.5;   // delete constant extrapolation over 0.5 sample
						sum2 -= 0.5 * rightValue;
						if (isdefined (leftValue)) {
							leftValue -= mean;
							leftValue *= leftValue;
							definitionRange += phase;   // add current fraction
							sum2 += phase * (rightValue + 0.5 * phase * (leftValue - rightValue));   // interpolate to outside sample
						} else {
							if (phase > 0.5)
								phase = 0.5;
							definitionRange += phase;   // add current fraction, but never more than 0.5
							sum2 += phase * rightValue;
						}
					} else if (isdefined (leftValue) && phase > 0.5) {
						leftValue -= mean;
						leftValue *= leftValue;
						definitionRange += phase - 0.5;
						sum2 += (phase - 0.5) * leftValue;
					}
				}
				if (xmax < rightEdge) {   // otherwise, constant extrapolation is OK
					double phase = (xmax - (my x1 + (imax - 1) * my dx)) / my dx;   // this fraction of sampling interval is still to be determined
					double leftValue = Sampled_getValueAtSample (me, imax, levelNumber, unit);
					double rightValue = Sampled_getValueAtSample (me, imax + 1, levelNumber, unit);
					if (isdefined (leftValue)) {
						leftValue -= mean;
						leftValue *= leftValue;
						definitionRange -= 0.5;   // delete constant extrapolation over 0.5 sample
						sum2 -= 0.5 * leftValue;
						if (isdefined (rightValue)) {
							rightValue -= mean;
							rightValue *= rightValue;
							definitionRange += phase;   // add current fraction
							sum2 += phase * (leftValue + 0.5 * phase * (rightValue - leftValue));   // interpolate to outside sample
						} else {
							if (phase > 0.5)
								phase = 0.5;
							definitionRange += phase;   // add current fraction, but never more than 0.5
							sum2 += phase * leftValue;
						}
					} else if (isdefined (rightValue) && phase > 0.5) {
						rightValue -= mean;
						rightValue *= rightValue;
						definitionRange += phase - 0.5;
						sum2 += (phase - 0.5) * rightValue;
					}
				}
			} else {   // no sample centres between xmin and xmax
				/*
					Try to return the mean of the interpolated values at these two points.
					Thus, a small (xmin, xmax) range gives the same value as the (xmin+xmax)/2 point.
				*/
				double leftValue = Sampled_getValueAtSample (me, imax, levelNumber, unit);
				double rightValue = Sampled_getValueAtSample (me, imin, levelNumber, unit);
				double phase1 = (xmin - (my x1 + (imax - 1) * my dx)) / my dx;
				double phase2 = (xmax - (my x1 + (imax - 1) * my dx)) / my dx;
				if (imin == imax + 1) {   // not too far from sample definition region
					if (isdefined (leftValue)) {
						leftValue -= mean;
						leftValue *= leftValue;
						if (isdefined (rightValue)) {
							rightValue -= mean;
							rightValue *= rightValue;
							definitionRange += phase2 - phase1;
							sum2 += (phase2 - phase1) * (leftValue + 0.5 * (phase1 + phase2) * (rightValue - leftValue));
						} else if (phase1 < 0.5) {
							if (phase2 > 0.5)
								phase2 = 0.5;
							definitionRange += phase2 - phase1;
							sum2 += (phase2 - phase1) * leftValue;
						}
					} else if (isdefined (rightValue) && phase2 > 0.5) {
						rightValue -= mean;
						rightValue *= rightValue;
						if (phase1 < 0.5)
							phase1 = 0.5;
						definitionRange += phase2 - phase1;
						sum2 += (phase2 - phase1) * rightValue;
					}
				}
			}
		} else {   // no interpolation
			const double rimin = Sampled_xToIndex (me, xmin), rimax = Sampled_xToIndex (me, xmax);
			if (rimax >= 0.5 && rimin < my nx + 0.5) {
				const integer imin = rimin < 0.5 ? 0 : Melder_iround (rimin);
				const integer imax = rimax >= my nx + 0.5 ? my nx + 1 : Melder_iround (rimax);
				for (integer isamp = imin + 1; isamp < imax; isamp ++) {
					double value = my v_getValueAtSample (isamp, levelNumber, unit);
					if (isdefined (value)) {
						value -= mean;
						value *= value;
						definitionRange += 1.0;
						sum2 += value;
					}
				}
				if (imin == imax) {
					double value = my v_getValueAtSample (imin, levelNumber, unit);
					if (isdefined (value)) {
						const double phase = rimax - rimin;
						value -= mean;
						value *= value;
						definitionRange += phase;
						sum2 += phase * value;
					}
				} else {
					if (imin >= 1) {
						double value = my v_getValueAtSample (imin, levelNumber, unit);
						if (isdefined (value)) {
							const double phase = imin - rimin + 0.5;
							value -= mean;
							value *= value;
							definitionRange += phase;
							sum2 += phase * value;
						}
					}
					if (imax <= my nx) {
						double value = my v_getValueAtSample (imax, levelNumber, unit);
						if (isdefined (value)) {
							const double phase = rimax - imax + 0.5;
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
	if (return_sum2) *return_sum2 = (double) sum2;
	if (return_definitionRange) *return_definitionRange = (double) definitionRange;
}

double Sampled_getStandardDeviation (Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate) {
	double sum, sum2, definitionRange;
	Sampled_getSumAndDefinitionRange (me, xmin, xmax, levelNumber, unit, interpolate, & sum, & definitionRange);
	if (definitionRange < 2.0)
		return undefined;
	Sampled_getSum2AndDefinitionRange (me, xmin, xmax, levelNumber, unit, sum / definitionRange, interpolate, & sum2, & definitionRange);
	return sqrt (sum2 / (definitionRange - 1.0));
}

double Sampled_getStandardDeviation_standardUnit (Sampled me, double xmin, double xmax, integer levelNumber, int averagingUnit, bool interpolate) {
	const double stdev = Sampled_getStandardDeviation (me, xmin, xmax, levelNumber, averagingUnit, interpolate);
	return Function_convertSpecialToStandardUnit (me, stdev, levelNumber, averagingUnit);
}

void Sampled_getMinimumAndX (Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate,
	double *return_minimum, double *return_xOfMinimum)
{
	double minimum = 1e301, xOfMinimum = 0.0;
	if (isundef (xmin) || isundef (xmax)) {
		minimum = xOfMinimum = undefined;
		goto end;
	}
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (! Function_intersectRangeWithDomain (me, & xmin, & xmax)) {
		minimum = xOfMinimum = undefined;   // requested range and logical domain do not intersect
		goto end;
	}
	integer imin, imax;
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
			No sample centres between xmin and xmax.
			Try to return the lesser of the values at these two points.
		*/
		const double fleft = Sampled_getValueAtX (me, xmin, levelNumber, unit, interpolate);
		const double fright = Sampled_getValueAtX (me, xmax, levelNumber, unit, interpolate);
		if (isdefined (fleft) && fleft < minimum) {
			minimum = fleft;
			xOfMinimum = xmin;
		}
		if (isdefined (fright) && fright < minimum) {
			minimum = fright;
			xOfMinimum = xmax;
		}
	} else {
		for (integer i = imin; i <= imax; i ++) {
			const double fmid = my v_getValueAtSample (i, levelNumber, unit);
			if (isundef (fmid)) continue;
			if (! interpolate) {
				if (fmid < minimum) {
					minimum = fmid;
					xOfMinimum = i;
				}
			} else {
				/*
					Try an interpolation, possibly even taking into account a sample just outside the selection.
				*/
				const double fleft = ( i <= 1 ? undefined : my v_getValueAtSample (i - 1, levelNumber, unit) );
				const double fright = ( i >= my nx ? undefined : my v_getValueAtSample (i + 1, levelNumber, unit) );
				if (isundef (fleft) || isundef (fright)) {
					if (fmid < minimum) {
						minimum = fmid;
						xOfMinimum = i;
					}
				} else if (fmid < fleft && fmid <= fright) {
					double y [3] = { fleft, fmid, fright };
					double i_real;
					const double localMinimum = NUMimproveMinimum (constVEC (y, 3), 2, NUM_PEAK_INTERPOLATE_PARABOLIC, & i_real);
					if (localMinimum < minimum) {
						minimum = localMinimum;
						xOfMinimum = i_real + i - 2;
					}
				}
			}
		}
		xOfMinimum = my x1 + (xOfMinimum - 1) * my dx;   // from index plus phase to time
		/*
			Check boundary values.
		*/
		if (interpolate) {
			const double fleft = Sampled_getValueAtX (me, xmin, levelNumber, unit, true);
			const double fright = Sampled_getValueAtX (me, xmax, levelNumber, unit, true);
			if (isdefined (fleft) && fleft < minimum) {
				minimum = fleft;
				xOfMinimum = xmin;
			}
			if (isdefined (fright) && fright < minimum) {
				minimum = fright;
				xOfMinimum = xmax;
			}
		}
		Melder_clip (xmin, & xOfMinimum, xmax);
	}
	if (minimum == 1e301)
		minimum = xOfMinimum = undefined;
end:
	if (return_minimum)
		*return_minimum = minimum;
	if (return_xOfMinimum)
		*return_xOfMinimum = xOfMinimum;
}

double Sampled_getMinimum (Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate) {
	double minimum;
	Sampled_getMinimumAndX (me, xmin, xmax, levelNumber, unit, interpolate, & minimum, nullptr);
	return minimum;
}

double Sampled_getXOfMinimum (Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate) {
	double time;
	Sampled_getMinimumAndX (me, xmin, xmax, levelNumber, unit, interpolate, nullptr, & time);
	return time;
}

void Sampled_getMaximumAndX (Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate,
	double *return_maximum, double *return_xOfMaximum)
{
	double maximum = -1e301, xOfMaximum = 0.0;
	if (isundef (xmin) || isundef (xmax)) {
		maximum = xOfMaximum = undefined;
		goto end;
	}
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (! Function_intersectRangeWithDomain (me, & xmin, & xmax)) {
		maximum = xOfMaximum = undefined;   // requested range and logical domain do not intersect
		goto end;
	}
	integer imin, imax;
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
			No sample centres between tmin and tmax.
			Try to return the greater of the values at these two points.
		*/
		const double fleft = Sampled_getValueAtX (me, xmin, levelNumber, unit, interpolate);
		const double fright = Sampled_getValueAtX (me, xmax, levelNumber, unit, interpolate);
		if (isdefined (fleft) && fleft > maximum) {
			maximum = fleft;
			xOfMaximum = xmin;
		}
		if (isdefined (fright) && fright > maximum) {
			maximum = fright;
			xOfMaximum = xmax;
		}
	} else {
		for (integer i = imin; i <= imax; i ++) {
			const double fmid = my v_getValueAtSample (i, levelNumber, unit);
			if (isundef (fmid)) continue;
			if (! interpolate) {
				if (fmid > maximum) {
					maximum = fmid;
					xOfMaximum = i;
				}
			} else {
				/*
					Try an interpolation, possibly even taking into account a sample just outside the selection.
				*/
				const double fleft =
					( i <= 1 ? undefined : my v_getValueAtSample (i - 1, levelNumber, unit) );
				const double fright =
					( i >= my nx ? undefined : my v_getValueAtSample (i + 1, levelNumber, unit) );
				if (isundef (fleft) || isundef (fright)) {
					if (fmid > maximum) {
						maximum = fmid;
						xOfMaximum = i;
					}
				} else if (fmid > fleft && fmid >= fright) {
					double y [3] = { fleft, fmid, fright };
					double i_real;
					const double localMaximum = NUMimproveMaximum (constVEC (y, 3), 2, NUM_PEAK_INTERPOLATE_PARABOLIC, & i_real);
					if (localMaximum > maximum) {
						maximum = localMaximum;
						xOfMaximum = i_real + i - 2;
					}
				}
			}
		}
		xOfMaximum = my x1 + (xOfMaximum - 1) * my dx;   // from index plus phase to time
		/*
			Check boundary values.
		*/
		if (interpolate) {
			const double fleft = Sampled_getValueAtX (me, xmin, levelNumber, unit, true);
			const double fright = Sampled_getValueAtX (me, xmax, levelNumber, unit, true);
			if (isdefined (fleft) && fleft > maximum) {
				maximum = fleft;
				xOfMaximum = xmin;
			}
			if (isdefined (fright) && fright > maximum) {
				maximum = fright;
				xOfMaximum = xmax;
			}
		}
		Melder_clip (xmin, & xOfMaximum, xmax);
	}
	if (maximum == -1e301)
		maximum = xOfMaximum = undefined;
end:
	if (return_maximum)
		*return_maximum = maximum;
	if (return_xOfMaximum)
		*return_xOfMaximum = xOfMaximum;
}

double Sampled_getMaximum (Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate) {
	double maximum;
	Sampled_getMaximumAndX (me, xmin, xmax, levelNumber, unit, interpolate, & maximum, nullptr);
	return maximum;
}

double Sampled_getXOfMaximum (Sampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate) {
	double time;
	Sampled_getMaximumAndX (me, xmin, xmax, levelNumber, unit, interpolate, nullptr, & time);
	return time;
}

static void Sampled_speckleInside (Sampled me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	integer levelNumber, int unit)
{
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	integer ixmin, ixmax;
	integer numberOfSamples = Sampled_getWindowSamples (me, xmin, xmax, & ixmin, & ixmax);
	if (numberOfSamples <= 0)
		return;
	if (Function_isUnitLogarithmic (me, levelNumber, unit)) {
		ymin = Function_convertStandardToSpecialUnit (me, ymin, levelNumber, unit);
		ymax = Function_convertStandardToSpecialUnit (me, ymax, levelNumber, unit);
	}
	if (ymax <= ymin)
		return;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (integer ix = ixmin; ix <= ixmax; ix ++) {
		const double value = Sampled_getValueAtSample (me, ix, levelNumber, unit);
		if (isdefined (value)) {
			const double x = Sampled_indexToX (me, ix);
			if (value >= ymin && value <= ymax)
				Graphics_speckle (g, x, value);
		}
	}
}

void Sampled_drawInside (Sampled me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool speckle, integer levelNumber, int unit)
{
	try {
		if (speckle) {
			Sampled_speckleInside (me, g, xmin, xmax, ymin, ymax, levelNumber, unit);
			return;
		}
		Function_unidirectionalAutowindow (me, & xmin, & xmax);
		integer ixmin, ixmax, startOfDefinedStretch = -1;
		integer numberOfSamples = Sampled_getWindowSamples (me, xmin, xmax, & ixmin, & ixmax);
		if (numberOfSamples <= 0)
			return;
		if (Function_isUnitLogarithmic (me, levelNumber, unit)) {
			ymin = Function_convertStandardToSpecialUnit (me, ymin, levelNumber, unit);
			ymax = Function_convertStandardToSpecialUnit (me, ymax, levelNumber, unit);
		}
		if (ymax <= ymin)
			return;
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		auto const lowIndex = ixmin - 1, highIndex = ixmax + 1;
		auto const nbuffer = highIndex - lowIndex + 1;
		auto xbuffer = zero_VEC (nbuffer), ybuffer = zero_VEC (nbuffer);
		auto const bufferShift = 1 - lowIndex;
		double *xarray = & xbuffer [bufferShift];
		double *yarray = & ybuffer [bufferShift];
		double previousValue = Sampled_getValueAtSample (me, lowIndex, levelNumber, unit);
		if (isdefined (previousValue)) {
			startOfDefinedStretch = lowIndex;
			xarray [lowIndex] = Sampled_indexToX (me, lowIndex);
			yarray [lowIndex] = previousValue;
		}
		for (integer ix = ixmin; ix <= ixmax; ix ++) {
			const double x = Sampled_indexToX (me, ix);
			const double value = Sampled_getValueAtSample (me, ix, levelNumber, unit);
			if (isdefined (value)) {
				if (isdefined (previousValue)) {
					xarray [ix] = x;
					yarray [ix] = value;
				} else {
					startOfDefinedStretch = ix - 1;
					xarray [ix - 1] = x - 0.5 * my dx;
					yarray [ix - 1] = value;
					xarray [ix] = x;
					yarray [ix] = value;
				}
			} else if (isdefined (previousValue)) {
				Melder_assert (startOfDefinedStretch >= lowIndex);
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
			const double x = Sampled_indexToX (me, highIndex);
			const double value = Sampled_getValueAtSample (me, ixmax + 1, levelNumber, unit);
			Melder_assert (isdefined (previousValue));
			if (isdefined (value)) {
				xarray [highIndex] = x;
				yarray [highIndex] = value;
			} else {
				xarray [highIndex] = x - 0.5 * my dx;
				yarray [highIndex] = previousValue;
			}
			if (xarray [startOfDefinedStretch] < xmin) {
				const double phase = (xmin - xarray [startOfDefinedStretch]) / my dx;
				xarray [startOfDefinedStretch] = xmin;
				yarray [startOfDefinedStretch] = phase * yarray [startOfDefinedStretch + 1] + (1.0 - phase) * yarray [startOfDefinedStretch];
			}
			if (xarray [highIndex] > xmax) {
				const double phase = (xarray [highIndex] - xmax) / my dx;
				xarray [highIndex] = xmax;
				yarray [highIndex] = phase * yarray [ixmax] + (1.0 - phase) * yarray [highIndex];
			}
			Graphics_polyline (g, ixmax + 2 - startOfDefinedStretch, & xarray [startOfDefinedStretch], & yarray [startOfDefinedStretch]);
		}
	} catch (MelderError) {
		Melder_clearError ();
	}
}

/* End of file Sampled.cpp */
