/* FilterBank.c
 *
 * Copyright (C) 1993-2008 David Weenink
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
 djmw 20010718
 djmw 20020813 GPL header
 djmw 20030901 Added fiter function drawing and frequency scale drawing.
 djmw 20050731 +FilterBank_and_PCA_drawComponent
 djmw 20071017 Melder_error<n>
 djmw 20071201 Melder_warning<n>
 djmw 20080122 float -> double
*/

#include "Eigen_and_Matrix.h"
#include "FilterBank.h"
#include "Matrix_extensions.h"
#include "NUM2.h"

static double scaleFrequency (double f, int scale_from, int scale_to)
{
	double fhz = NUMundefined;
	
	if (scale_from == scale_to) return f;
	if (scale_from == FilterBank_HERTZ)
	{
		fhz = f;
	}
	else if (scale_from == FilterBank_BARK)
	{
		fhz = BARKTOHZ (f);
	}
	else if (scale_from == FilterBank_MEL)
	{
		fhz = MELTOHZ (f);
	}
	
	if (scale_to == FilterBank_HERTZ || fhz == NUMundefined) return fhz;
 
	if (scale_to == FilterBank_BARK)	
	{
		f = HZTOBARK (fhz);
	}
	else if (scale_to == FilterBank_MEL)
	{
		f = HZTOMEL (fhz);
	}
	else
	{
		return NUMundefined;
	}
	return f;
}

static wchar_t *GetFreqScaleText (int scale)
{
	wchar_t *hertz = L"Frequency (Hz)";
	wchar_t *bark = L"Frequency (Bark)";
	wchar_t *mel = L"Frequency (mel)";
	wchar_t *error = L"Frequency (undefined)";
	if (scale == FilterBank_HERTZ)
	{
		return hertz;
	}
	else if (scale == FilterBank_BARK)
	{
		return bark;
	}
	else if (scale == FilterBank_MEL)
	{
		return mel;
	}
	return error;
	
}

static int checkLimits (I, int fromFreqScale, int toFreqScale, int *fromFilter,
	int *toFilter, double *zmin, double *zmax, int dbScale, 
	double *ymin, double *ymax)
{
	iam (Matrix);
	
	if (*fromFilter == 0)  *fromFilter = 1;
	if (*toFilter == 0)  *toFilter = my ny;
	if (*toFilter < *fromFilter)
	{
		*fromFilter = 1;
		*toFilter = my ny;
	}
	if (*fromFilter < 1) *fromFilter = 1;
	if (*toFilter > my ny) *toFilter = my ny;
	if (*fromFilter > *toFilter)
	{
		Melder_warning3 (L"Filter numbers must be in range [1, ", Melder_integer (my ny), L"]");
		return 0;
	}
		
	if (*zmin < 0 || *zmax < 0)
	{
		Melder_warning1 (L"Frequencies must be positive.");
		return 0;
	}
	if (*zmax <= *zmin)
	{
		*zmin = scaleFrequency (my ymin, fromFreqScale, toFreqScale);
		*zmax = scaleFrequency (my ymax, fromFreqScale, toFreqScale); 
	}

	if (*ymax <= *ymin)
	{
		*ymax = 1; *ymin = 0;
		if (dbScale)
		{
			*ymax = 0; *ymin = -60;
		}
	}
	return 1;
}

static double to_dB (double a, double factor, double ref_dB)
{
	if (a <= 0) return ref_dB;
	a = factor * log10 (a);
	if (a < ref_dB) a = ref_dB;
	return a;
}

static void setDrawingLimits (double *a, long n, double amin, double amax,
	long *ibegin, long *iend)
{
	long i, lower = 1;
	
	*ibegin = 0;
	*iend = n + 1;

	for (i = 1; i <= n; i++)
	{
		if (a[i] == NUMundefined)
		{
			if (lower == 0)
			{
				/* high frequency part */
				*iend = i;
				break;
			}
			*ibegin = i;
			continue; 
		}
		lower = 0;
		if (a[i] < amin)
		{
			a[i] = amin; 
		}
		else if (a[i] > amax)
		{
			a[i] = amax;
		}
	}
		
	(*ibegin)++;
	(*iend)--;
}

static int classFilterBank_getFrequencyScale (I)
{
	(void) void_me;
	return FilterBank_HERTZ;
}

class_methods (FilterBank, Matrix)
	class_method_local (FilterBank, getFrequencyScale)
class_methods_end

static int classBarkFilter_getFrequencyScale (I)
{
	(void) void_me;
	return FilterBank_BARK;
}

class_methods (BarkFilter, FilterBank)
	class_method_local (BarkFilter, getFrequencyScale)
class_methods_end

BarkFilter BarkFilter_create (double tmin, double tmax, long nt, double dt,
	double t1, double fmin, double fmax, long nf, double df, long f1)
{
	BarkFilter me = new (BarkFilter);
	if (me == NULL || ! Matrix_init (me, tmin, tmax, nt, dt, t1,
		fmin, fmax, nf, df, f1)) forget (me);
	return me;
}

double FilterBank_getFrequencyInHertz (I, double f, int scale_from)
{
	(void) void_me; 
	return scaleFrequency (f, scale_from, FilterBank_HERTZ);
}

double FilterBank_getFrequencyInBark (I, double f, int scale_from)
{
	(void) void_me; 
	return scaleFrequency (f, scale_from, FilterBank_BARK);
}

double FilterBank_getFrequencyInMel (I, double f, int scale_from)
{
	(void) void_me; 
	return scaleFrequency (f, scale_from, FilterBank_MEL);
}

int FilterBank_getFrequencyScale (I)
{
	iam (FilterBank);
	return our getFrequencyScale (me);
}

void FilterBank_drawFrequencyScales (I, Graphics g, int horizontalScale, double xmin, 
	double xmax, int verticalScale, double ymin, double ymax, int garnish)
{
	iam (FilterBank);
	long i, ibegin, iend, n = 2000;
	double *a, df;
	int myFreqScale = FilterBank_getFrequencyScale (me);

	if (xmin < 0 || xmax < 0 ||ymin < 0 || ymax < 0)
	{
		Melder_warning1 (L"Frequencies must be >= 0.");
		return;
	}
	
	if (xmin >= xmax)
	{
		double xmint = my ymin;
		double xmaxt = my ymax;
		if (ymin < ymax)
		{
			xmint = scaleFrequency (ymin, verticalScale, myFreqScale);
			xmaxt = scaleFrequency (ymax, verticalScale, myFreqScale); 
		}
		xmin = scaleFrequency (xmint, myFreqScale, horizontalScale);
		xmax = scaleFrequency (xmaxt, myFreqScale, horizontalScale);
	}
	
	if (ymin >= ymax)
	{
		ymin = scaleFrequency (xmin, horizontalScale, verticalScale); 
		ymax = scaleFrequency (xmax, horizontalScale, verticalScale);
	}
	
	a = NUMdvector (1, n);
	if (a == NULL) return;
	
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	df = (xmax - xmin) / (n - 1);
	
	for (i = 1; i <= n; i++)
	{
		double f = xmin + (i - 1) * df;
		a[i] = scaleFrequency (f, horizontalScale, verticalScale);
	}
	
	setDrawingLimits (a, n, ymin, ymax,	& ibegin, & iend);
	if (ibegin <= iend)
	{
		double fmin = xmin + (ibegin - 1) * df;
		double fmax = xmax - (n - iend) * df;
		Graphics_function (g, a, ibegin, iend, fmin, fmax);
	}
	Graphics_unsetInner (g);
	
	if (garnish)
	{
		Graphics_drawInnerBox (g);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
    	Graphics_textLeft (g, 1, GetFreqScaleText (verticalScale));
    	Graphics_marksBottom (g, 2, 1, 1, 0);
    	Graphics_textBottom (g, 1, GetFreqScaleText (horizontalScale));
	}
	
	NUMdvector_free (a, 1);
}

void BarkFilter_drawSekeyHansonFilterFunctions (BarkFilter me, Graphics g,
	int toFreqScale, int fromFilter, int toFilter, double zmin, double zmax, 
	int dbScale, double ymin, double ymax, int garnish)
{
	long i, j, n = 1000; 
	double *a = NULL;
		
	if (! checkLimits (me, FilterBank_BARK, toFreqScale, & fromFilter, & toFilter, 
		& zmin, & zmax, dbScale, & ymin, & ymax)) return;
	
	a = NUMdvector (1, n);
	if (a == NULL) return;
	
	Graphics_setInner (g);
	Graphics_setWindow (g, zmin, zmax, ymin, ymax);
	
	for (j = fromFilter; j <= toFilter; j++)
	{
		double df = (zmax - zmin) / (n - 1);
		double zMid = Matrix_rowToY (me, j);
		long ibegin, iend;
		
		for (i = 1; i <= n; i++)
		{
			double z, f = zmin + (i - 1) * df;
			
			z = scaleFrequency (f, toFreqScale, FilterBank_BARK);
			if (z == NUMundefined)
			{
				a[i] = NUMundefined;
			}
			else
			{
				z -= zMid + 0.215;
				a[i] = 7 - 7.5 * z - 17.5 * sqrt (0.196 + z * z);
				if (! dbScale) a[i] = pow (10, a[i]);
			}			
		}
		
		setDrawingLimits (a, n, ymin, ymax,	&ibegin, &iend);
		
		if (ibegin <= iend)
		{
			double fmin = zmin + (ibegin - 1) * df;
			double fmax = zmax - (n - iend) * df;
			Graphics_function (g, a, ibegin, iend, fmin, fmax);
		}
	}
		
			
	Graphics_unsetInner (g);
	
	if (garnish)
	{
		double distance = dbScale ? 10 : 1;
		wchar_t *ytext = dbScale ? L"Amplitude (dB)" : L"Amplitude";
		Graphics_drawInnerBox (g);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
    	Graphics_marksLeftEvery (g, 1, distance, 1, 1, 0);
    	Graphics_textLeft (g, 1, ytext);
    	Graphics_textBottom (g, 1, GetFreqScaleText (toFreqScale));
	}
	
	NUMdvector_free (a, 1);
}

static int classMelFilter_getFrequencyScale (I)
{
	(void) void_me;
	return FilterBank_MEL;
}

class_methods (MelFilter, FilterBank)
	class_method_local (MelFilter, getFrequencyScale)
class_methods_end

MelFilter MelFilter_create (double tmin, double tmax, long nt, double dt,
	double t1, double fmin, double fmax, long nf, double df, double f1)
{
	MelFilter me = new (MelFilter);
	
	if (me == NULL || ! Matrix_init (me, tmin, tmax, nt, dt, t1,
		fmin, fmax, nf, df, f1)) forget (me);
	
	return me;
}

/*
void FilterBank_drawFilters (I, Graphics g, long fromf, long tof,
	double xmin, double xmax, int xlinear, double ymin, double ymax, int ydb,
	double (*tolinf)(double f), double (*tononlin) (double f), 
	double (*filteramp)(double f0, double b, double f))
{
	iam (Matrix);
	

}*/

void FilterBank_drawTimeSlice (I, Graphics g, double t, double fmin, 
	double fmax, double min, double max, wchar_t *xlabel, int garnish)
{
	iam (Matrix);
	Matrix_drawSliceY (me, g, t, fmin, fmax, min, max);
	if (garnish)
	{
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		if (xlabel) Graphics_textBottom (g, 0, xlabel);
	}
}

void MelFilter_drawFilterFunctions (MelFilter me, Graphics g,
	int toFreqScale, int fromFilter, int toFilter, double zmin, double zmax, 
	int dbScale, double ymin, double ymax, int garnish)
{
	long i, j, n = 1000; 
	double *a = NULL;

	if (! checkLimits (me, FilterBank_MEL, toFreqScale, & fromFilter, & toFilter, 
		& zmin, & zmax, dbScale, & ymin, & ymax)) return;
		
	a = NUMdvector (1, n);
	if (a == NULL) return;
	
	Graphics_setInner (g);
	Graphics_setWindow (g, zmin, zmax, ymin, ymax);

	for (j = fromFilter; j <= toFilter; j++)
	{
		double df = (zmax - zmin) / (n - 1); 
		double fc_mel = my y1 + (j - 1) * my dy;
		double fc_hz = MELTOHZ (fc_mel);
		double fl_hz = MELTOHZ (fc_mel - my dy);
		double fh_hz = MELTOHZ (fc_mel + my dy);
		long ibegin, iend;
		
		for (i = 1; i <= n; i++)
		{
			double z, f = zmin + (i - 1) * df;
			
			/* Filterfunction: triangular on a linear frequency scale
				AND a linear amplitude scale.
			*/
			z = scaleFrequency (f, toFreqScale, FilterBank_HERTZ);
			if (z == NUMundefined)
			{
				a[i] = NUMundefined;
			}
			else
			{
				a[i] = NUMtriangularfilter_amplitude (fl_hz, fc_hz, fh_hz, z);
				if (dbScale) a[i] = to_dB (a[i], 10, ymin);
			}
			
		}
				
		setDrawingLimits (a, n, ymin, ymax,	&ibegin, &iend);
		
		if (ibegin <= iend)
		{
			double fmin = zmin + (ibegin - 1) * df;
			double fmax = zmax - (n - iend) * df;
			Graphics_function (g, a, ibegin, iend, fmin, fmax);
		}
	}

	Graphics_unsetInner (g);
	
	if (garnish)
	{
		double distance = dbScale ? 10 : 1;
		wchar_t *ytext = dbScale ? L"Amplitude (dB)" : L"Amplitude";
		Graphics_drawInnerBox (g);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
    	Graphics_marksLeftEvery (g, 1, distance, 1, 1, 0);
    	Graphics_textLeft (g, 1, ytext);
    	Graphics_textBottom (g, 1, GetFreqScaleText (toFreqScale));
	}
	
	NUMdvector_free (a, 1);
}

/*
void MelFilter_drawFilters (MelFilter me, Graphics g, long from, long to,
	double fmin, double fmax, double ymin, double ymax, int dbscale, 
	int garnish)
{
	long i;
	double df = my dy;

	if (fmin >= fmax)
	{
		fmin = my ymin;
		fmax = my ymax;
	}
	if (from >= to)
	{
		from = 1;
		to = my ny;
	}
	Graphics_setWindow (g, my ymin, my ymax, 0, 1);
	Graphics_setInner (g);	
	for (i = from; i <= to; i++)
	{
		double fc = my y1 + (i - 1) * df;
		double fc_hz = MELTOHZ (fc);
		double fl_hz = MELTOHZ (fc - df);
		double fh_hz = MELTOHZ (fc + df);
		*//*
			Draw triangle
		*//*
		Graphics_line (g, fl_hz, 0, fc_hz, 1);
		Graphics_line (g, fc_hz, 1, fh_hz, 0); 
	}
	Graphics_unsetInner (g);
}
*/
Matrix FilterBank_to_Matrix (I)
{
	iam (Matrix); Matrix thee;
	
	if ((thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		my ymin, my ymax, my ny, my dy, my y1)) == NULL) return NULL;
		
	NUMdmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
	
	return thee;
}

BarkFilter Matrix_to_BarkFilter (I)
{
	iam (Matrix); BarkFilter thee;
	
	if ((thee = BarkFilter_create (my xmin, my xmax, my nx, my dx, my x1,
		my ymin, my ymax, my ny, my dy, my y1)) == NULL) return NULL;
		
	NUMdmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
	
	return thee;
}

MelFilter Matrix_to_MelFilter (I)
{
	iam (Matrix); MelFilter thee;
	
	if ((thee = MelFilter_create (my xmin, my xmax, my nx, my dx, my x1,
		my ymin, my ymax, my ny, my dy, my y1)) == NULL) return NULL;
		
	NUMdmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
	
	return thee;
}

class_methods (FormantFilter, FilterBank)
class_methods_end

FormantFilter FormantFilter_create (double tmin, double tmax, long nt,
	double dt, double t1, double fmin, double fmax, long nf, double df,
	double f1)
{
	FormantFilter me = new (FormantFilter);
	
	if (me == NULL || ! Matrix_init (me, tmin, tmax, nt, dt, t1,
		fmin, fmax, nf, df, f1)) forget (me);
	
	return me;
}

void FormantFilter_drawFilterFunctions (FormantFilter me, Graphics g, double bandwidth,
	int toFreqScale, int fromFilter, int toFilter, double zmin, double zmax, 
	int dbScale, double ymin, double ymax, int garnish)
{
	long i, j, n = 1000; 
	double *a = NULL;
		
	if (! checkLimits (me, FilterBank_HERTZ, toFreqScale, & fromFilter, & toFilter, 
		& zmin, & zmax, dbScale, & ymin, & ymax)) return;
	
	if (bandwidth <= 0)
	{
		Melder_warning1 (L"Bandwidth must be greater than zero.");
	}
		
	a = NUMdvector (1, n);
	if (a == NULL) return;
		
	Graphics_setInner (g);
	Graphics_setWindow (g, zmin, zmax, ymin, ymax);
	
	for (j = fromFilter; j <= toFilter; j++)
	{
		double df = (zmax - zmin) / (n - 1);
		double fc = my y1 + (j - 1) * my dy;
		long ibegin, iend;
		
		for (i = 1; i <= n; i++)
		{
			double z, f = zmin + (i - 1) * df;
			
			z = scaleFrequency (f, toFreqScale, FilterBank_HERTZ);
			if (z == NUMundefined)
			{
				a[i] = NUMundefined;
			}
			else
			{
				a[i] = NUMformantfilter_amplitude (fc, bandwidth, z);			
				if (dbScale) a[i] = to_dB (a[i], 10, ymin);
			}
		}
		
		setDrawingLimits (a, n, ymin, ymax,	&ibegin, &iend);
		
		if (ibegin <= iend)
		{
			double fmin = zmin + (ibegin - 1) * df;
			double fmax = zmax - (n - iend) * df;
			Graphics_function (g, a, ibegin, iend, fmin, fmax);
		}
	}
		
			
	Graphics_unsetInner (g);
	
	if (garnish)
	{
		double distance = dbScale ? 10 : 1;
		wchar_t *ytext = dbScale ? L"Amplitude (dB)" : L"Amplitude";
		Graphics_drawInnerBox (g);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
    	Graphics_marksLeftEvery (g, 1, distance, 1, 1, 0);
    	Graphics_textLeft (g, 1, ytext);
    	Graphics_textBottom (g, 1, GetFreqScaleText (toFreqScale));
	}
	
	NUMdvector_free (a, 1);
}

FormantFilter Matrix_to_FormantFilter (I)
{
	iam (Matrix); FormantFilter thee;
	
	if ((thee = FormantFilter_create (my xmin, my xmax, my nx, my dx, my x1,
		my ymin, my ymax, my ny, my dy, my y1)) == NULL) return NULL;
		
	NUMdmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
	
	return thee;
}

Spectrum FormantFilter_to_Spectrum_slice (FormantFilter me, double t)
{
	Spectrum thee;
	long i, frame;
	double sqrtref = sqrt (FilterBank_DBREF);
	double factor2 = 2 * 10 * FilterBank_DBFAC;

	if ((thee = Spectrum_create (my ymax, my ny)) == NULL) return NULL;
	
	thy xmin = my ymin;
	thy xmax = my ymax;
	thy x1 = my y1;
	thy dx = my dy;   /* Frequency step. */
	
	frame = Sampled_xToIndex (me, t);
	if (frame < 1) frame = 1;
	if (frame > my nx) frame = my nx;

	for (i = 1; i <= my ny; i++)
	{	/*
			power = ref * 10 ^ (value / 10)
			sqrt(power) = sqrt(ref) * 10 ^ (value / (2*10))
		*/
		thy z[1][i] = sqrtref * pow (10, my z[i][frame] / factor2);
		thy z[2][i] = 0.0;
	}

	return thee;	
}

Intensity FilterBank_to_Intensity (I)
{
	iam (Matrix);
	Intensity thee;
	double db_ref = 10 * log10 (FilterBank_DBREF);
	long i, j;
	
	thee = Intensity_create (my xmin, my xmax, my nx, my dx, my x1);
	if (thee == NULL) return NULL;

	for (j = 1; j <= my nx; j++)
	{
		double p = 0;
		for (i = 1; i <= my ny; i++)
		{
			p += FilterBank_DBREF * exp (NUMln10 * my z[i][j] / 10);
		}
		thy z[1][j] = 10 * log10 (p) - db_ref;
	}
	return thee;
}

void FilterBank_equalizeIntensities (I, double intensity_db)
{
	iam (Matrix);
	long i, j;
	
	for (j=1; j <= my nx; j++)
	{
		double p = 0, delta_db;
		
		for (i=1; i <= my ny; i++)
		{
			p += FilterBank_DBREF * exp (NUMln10 * my z[i][j] / 10);
		}
		
		delta_db = intensity_db - 10 * log10 (p / FilterBank_DBREF);
		
		for (i=1; i <= my ny; i++)
		{
			my z[i][j] += delta_db;
		}
	}
}

void FilterBank_and_PCA_drawComponent (I, PCA thee, Graphics g, long component, double dblevel,
	double frequencyOffset, double scale, double tmin, double tmax, double fmin, double fmax)
{
	iam (FilterBank);
	FilterBank fcopy = NULL;
	Matrix him = NULL;
	long j;
	
	if (component < 1 || component > thy numberOfEigenvalues)
	{
		(void) Melder_error1 (L"Component too large.");
	}
	/*
	 	Scale Intensity
	 */
	fcopy = Data_copy (me);
	if (fcopy == NULL)  return;
	FilterBank_equalizeIntensities (fcopy, dblevel);
	him = Eigen_and_Matrix_project (thee, fcopy, component);
	if (him == NULL) goto end;
	for (j = 1; j<= my nx; j++)
	{
		fcopy -> z[component][j] = frequencyOffset + scale * fcopy -> z[component][j];	
	}
	Matrix_drawRows (fcopy, g, tmin, tmax, component-0.5, component+0.5, fmin, fmax);
end:
	forget (fcopy);
	forget (him);
}
 
/* End of file Filterbank.c */
