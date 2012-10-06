/* Polygon_extensions.c
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20020813 GPL header
 djmw 20080122 float -> double
 djmw 20100407 Sound_to_Polygon, Sounds_to_Polygon_encloseds
 djmw 20100418 Polygon_rotate. bug: angle was in degrees now in radians
*/

#include "Matrix.h"
#include "NUM2.h"
#include "Polygon_extensions.h"
#include "Vector.h"
#include "DLL.h"

//

void Polygon_getExtrema (Polygon me, double *xmin, double *xmax, double *ymin, double *ymax) {
    *xmin = my x[1]; *xmax = my x[1];
    *ymin = my y[1]; *ymax = my y[1];
    for (long i = 2; i <= my numberOfPoints; i++) {
        if (my x[i] < *xmin) {
            *xmin = my x[i];
        } else if (my x[i] > *xmax) {
            *xmax = my x[i];
        }
        if (my y[i] < *ymin) {
            *ymin = my y[i];
        } else if (my y[i] > *ymax) {
            *ymax = my y[i];
        }
    }
}
Polygon Polygon_createSimple (wchar_t *xystring) {
	try {
		long numberOfPoints;
		autoNUMvector<double> xys (NUMstring_to_numbers (xystring, &numberOfPoints), 1);
		if (numberOfPoints < 6) {
			Melder_throw ("There must be at least 3 points (= x,y pairs) in the Polygon");
		}
		if (numberOfPoints % 2 != 0) {
			Melder_throw ("One value is missing.");
		}
		numberOfPoints /= 2; // x,y pairs
		autoPolygon me = Polygon_create (numberOfPoints);
		for (long i = 1; i <= numberOfPoints; i++) {
			my x[i] = xys[2 * i - 1];
			my y[i] = xys[2 * i];
			if (i > 1 && my x[i] == my x[i - 1] && my y[i] == my y[i - 1]) {
				Melder_warning ("Two successives vertices are equal.");
			}
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Polygon not created.");
	}
}

Polygon Polygon_createFromRandomVertices (long numberOfVertices, double xmin, double xmax, double ymin, double ymax) {
	try {
		autoPolygon me = Polygon_create (numberOfVertices);
		for (long i = 1; i <= numberOfVertices; i++) {
			my x[i] = NUMrandomUniform (xmin, xmax);
			my y[i] = NUMrandomUniform (ymin, ymax);
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Polygon not created.");
	}
}

void Polygon_translate (Polygon me, double xt, double yt) {
	for (long i = 1; i <= my numberOfPoints; i++) {
		my x[i] += xt;
		my y[i] += yt;
	}
}

/* rotate counterclockwise w.r.t. (xc,yc) */
void Polygon_rotate (Polygon me, double alpha, double xc, double yc) {
	double f = alpha * NUMpi / 180, cosa = cos (f), sina = sin (f);

	Polygon_translate (me, -xc, -yc);
	for (long i = 1; i <= my numberOfPoints; i++) {
		double x = my x[i];
		my x[i] = cosa * my x[i] - sina * my y[i];
		my y[i] = sina * x       + cosa * my y[i];
	}
	Polygon_translate (me, xc, yc);
}

void Polygon_scale (Polygon me, double xs, double ys) {
	for (long i = 1; i <= my numberOfPoints; i++) {
		my x[i] *= xs;
		my y[i] *= ys;
	}
}

void Polygon_reverseX (Polygon me) {
	for (long i = 1; i <= my numberOfPoints; i++) {
		my x[i] = -my x[i];
	}

}

void Polygon_reverseY (Polygon me) {
	for (long i = 1; i <= my numberOfPoints; i++) {
		my y[i] = -my y[i];
	}

}

void Polygon_Categories_draw (Polygon me, thou, Graphics graphics, double xmin, double xmax,
                              double ymin, double ymax, int garnish) {
	thouart (Categories);
	double min, max, tmp;

	if (my numberOfPoints != thy size) {
		return;
	}

	if (xmax == xmin) {
		NUMvector_extrema (my x, 1, my numberOfPoints, & min, & max);
		tmp = max - min == 0 ? 0.5 : 0.0;
		xmin = min - tmp; xmax = max + tmp;
	}

	if (ymax == ymin) {
		NUMvector_extrema (my y, 1, my numberOfPoints, & min, & max);
		tmp = max - min == 0 ? 0.5 : 0.0;
		ymin = min - tmp; ymax = max + tmp;
	}

	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);

	for (long i = 1; i <= my numberOfPoints; i++) {
		OrderedOfString_drawItem (thee, graphics, i, my x[i], my y[i]);
	}
	Graphics_unsetInner (graphics);
	if (garnish) {
		Graphics_drawInnerBox (graphics);
		Graphics_marksLeft (graphics, 2, 1, 1, 0);
		if (ymin * ymax < 0.0) {
			Graphics_markLeft (graphics, 0.0, 1, 1, 1, NULL);
		}
		Graphics_marksBottom (graphics, 2, 1, 1, 0);
		if (xmin * xmax < 0.0) {
			Graphics_markBottom (graphics, 0.0, 1, 1, 1, NULL);
		}
	}
}

static void setWindow (Polygon me, Graphics graphics,
                       double xmin, double xmax, double ymin, double ymax) {
	Melder_assert (me);

	if (xmax <= xmin) { /* Autoscaling along x axis. */
		xmax = xmin = my x [1];
		for (long i = 2; i <= my numberOfPoints; i ++) {
			if (my x [i] < xmin) {
				xmin = my x [i];
			}
			if (my x [i] > xmax) {
				xmax = my x [i];
			}
		}
		if (xmin == xmax) {
			xmin -= 1.0;
			xmax += 1.0;
		}
	}
	if (ymax <= ymin) { /* Autoscaling along y axis. */
		ymax = ymin = my y [1];
		for (long i = 2; i <= my numberOfPoints; i ++) {
			if (my y [i] < ymin) {
				ymin = my y [i];
			}
			if (my y [i] > ymax) {
				ymax = my y [i];
			}
		}
		if (ymin == ymax) {
			ymin -= 1.0;
			ymax += 1.0;
		}
	}
	Graphics_setWindow (graphics, xmin, xmax, ymin, ymax);
}

void Polygon_drawMarks (Polygon me, Graphics g, double xmin, double xmax,
                        double ymin, double ymax, double size_mm, const wchar_t *mark) {
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (long i = 1; i <= my numberOfPoints; i++) {
		Graphics_mark (g, my x[i], my y[i], size_mm, mark);
	}
	Graphics_unsetInner (g);
}

#define CLIP_Y(y,ymin,ymax) (clip ? ((y) > (ymax) ? (ymax) : (y) < (ymin) ? (ymin) : (y)) : y)

Polygon Sound_to_Polygon (Sound me, int channel, double tmin, double tmax, double ymin, double ymax, double level) {
	try {
		bool clip = ymin < ymax;
		if (channel < 1 || channel > my ny) {
			Melder_throw ("Channel does not exist.");
		}
		if (tmin >= tmax) {
			tmin = my xmin;
			tmax = my xmax;
		}
		if (tmin < my xmin) {
			tmin = my xmin;
		}
		if (tmax > my xmax) {
			tmax = my xmax;
		}
		if (tmin >= my xmax || tmax < my xmin) {
			Melder_throw ("Invalid domain.");
		}
		long k = 1, i1 = Sampled_xToHighIndex (me, tmin);
		long i2 = Sampled_xToLowIndex (me, tmax);
		long numberOfPoints = i2 - i1 + 1 + 2 + 2; // begin + endpoint + level
		autoPolygon him = Polygon_create (numberOfPoints);

		/*
			In Vector_getValueAtX the interpolation only returns defined values between the
			left and right edges that are calculated as
				left = x1 - 0.5 * dx; right = left + my nx * dx.
			Given a sound, for example on the domain [0,...], the value of 'left' with the above formula might
			not return exactly xmin but instead a very small deviation (due to the imprecise
			representation of real numbers in a computer).
			Querying for the value at xmin which is outside the interpolation domain then produces an 'undefined'.
			We try to avoid this with the following workaround.
		*/
		double xmin = my x1 - 0.5 * my dx;
		double xmax = xmin + my nx * my dx;
		tmin = tmin < xmin ? xmin : tmin;
		tmax = tmax > xmax ? xmax : tmax;
		// End of workaround
		his x[k] = tmin;
		his y[k++] = CLIP_Y (level, ymin, ymax);
		his x[k] = tmin;
		double y = Vector_getValueAtX (me, tmin, channel, Vector_VALUE_INTERPOLATION_LINEAR);
		his y[k++] = CLIP_Y (y, ymin, ymax);
		for (long i = i1; i <= i2; i++) {
			y = my z[channel][i];
			his x[k] = my x1 + (i - 1) * my dx;
			his y[k++] = CLIP_Y (y, ymin, ymax);
		}
		his x[k] = tmax;
		y = Vector_getValueAtX (me, tmax, channel, Vector_VALUE_INTERPOLATION_LINEAR);
		his y[k++] = CLIP_Y (y, ymin, ymax);
		his x[k] = tmax;
		his y[k++] = CLIP_Y (level, ymin, ymax);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ":no Polygon created.");
	}
}

/* Area inbetween */

Polygon Sounds_to_Polygon_enclosed (Sound me, Sound thee, int channel, double tmin, double tmax, double ymin, double ymax) {
	try {
		bool clip = ymin < ymax;
		if (my ny > 1 && thy ny > 1 && my ny != thy ny) {
			Melder_throw ("The numbers of channels of the two sounds have to be equal or 1.");
		}

		long numberOfChannels = my ny > thy ny ? my ny : thy ny;

		if (channel < 1 || channel > numberOfChannels) {
			Melder_throw ("Channel does not exist.");
		}
		// find overlap in the domains  with xmin workaround as in Sound_to_Polygon
		double xmin1 = my x1 - 0.5 * my dx, xmin2 = thy x1 - 0.5 * thy dx ;
		double xmin = my xmin > thy xmin ? xmin1 : xmin2;
		double xmax = my xmax < thy xmax ? xmin1 + my nx * my dx : xmin2 + thy nx * thy dx;
		if (xmax <= xmin) {
			Melder_throw ("Domains must overlap.");
		}
		if (tmin >= tmax) {
			tmin = xmin;
			tmax = xmax;
		}
		if (tmin < xmin) {
			tmin = xmin;
		}
		if (tmax > xmax) {
			tmax = xmax;
		}
		if (tmin >= xmax || tmax < xmin) {
			Melder_throw ("Invalid domain.");
		}

		long k = 1;
		long ib1 = Sampled_xToHighIndex (me, tmin);
		long ie1 = Sampled_xToLowIndex (me, tmax);
		long n1 = ie1 - ib1 + 1;
		long ib2 = Sampled_xToHighIndex (thee, tmin);
		long ie2 = Sampled_xToLowIndex (thee, tmax);
		long n2 = ie2 - ib2 + 1;
		long numberOfPoints = n1 + n2 + 4; // me + thee + begin + endpoint + closing

		autoPolygon him = Polygon_create (numberOfPoints);

		// my starting point at tmin

		double y = Vector_getValueAtX (me, tmin, (my ny == 1 ? 1 : channel), Vector_VALUE_INTERPOLATION_LINEAR);
		his x[k] = tmin;
		his y[k++] = CLIP_Y (y, ymin, ymax);

		// my samples

		for (long i = ib1; i <= ie1; i++) {
			double t = my x1 + (i - 1) * my dx;
			y = my z[my ny == 1 ? 1 : channel][i];
			his x[k] = t;
			his y[k++] = CLIP_Y (y, ymin, ymax);
		}

		// my end point at tmax

		y = Vector_getValueAtX (me, tmax, (my ny == 1 ? 1 : channel), Vector_VALUE_INTERPOLATION_LINEAR);
		his x[k] = tmax;
		his y[k++] = y;

		// thy starting point at tmax

		y = Vector_getValueAtX (thee, tmax, (thy ny == 1 ? 1 : channel), Vector_VALUE_INTERPOLATION_LINEAR);
		his x[k] = tmax;
		his y[k++] = y;

		// thy samples

		for (long i = ie2; i >= ib2; i--) {
			double t = thy x1 + (i - 1) * thy dx;
			y = thy z[thy ny == 1 ? 1 : channel][i];
			his x[k] = t;
			his y[k++] = CLIP_Y (y, ymin, ymax);
		}

		// thy end point at tmin

		y = Vector_getValueAtX (thee, tmin, (thy ny == 1 ? 1 : channel), Vector_VALUE_INTERPOLATION_LINEAR);
		his x[k] = tmin;
		his y[k] = y;

		Melder_assert (k == numberOfPoints);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no enclosed Polygon created.");
	}
}

#define INTERSECTION_OUTSIDE 0
#define INTERSECTION_PROPER 1
#define INTERSECTION_AT_EDGE 2
#define INTERSECTION_AT_VERTEX 3
#define INTERSECTION_COPLANAR 4
#define INTERSECTION_COPLANAR_AT_EDGE 6
#define INTERSECTION_COPLANAR_AT_VERTEX 7

// 12 is from subject 34 from clipping
static int get_collinearIntersectionPoint (double x1, double x2, double x3, double x4, double *xs) {
	bool p3inb12 = (x1 < x3 and x3 <= x2) or (x1 > x3 and x3 >= x2);
	bool p4inb12 = (x1 < x4 and x4 <= x2) or (x1 > x4 and x4 >= x2);
	int intersection = INTERSECTION_AT_VERTEX;
	if (x1 < x2) {
		if (x3 < x4) {
			if (x2 < x3 || x4 < x1) {
				intersection = INTERSECTION_OUTSIDE;
			} else if (p3inb12) {
				// 4 can be inside or outside
				*xs = p4inb12 ? x4 : x2;
			} else if (p4inb12) {
				// 3 is outside
				*xs = x4;
			} else {
				// 3 and 4 outside 12 segment
				*xs = x2;
				intersection = INTERSECTION_AT_EDGE;
			}
		} else {
			// x4 < x3
			if (x2 < x4 || x3 < x1) {
				intersection = INTERSECTION_OUTSIDE;
			} else if (p3inb12) {
				// 4 can be inside or outside
				*xs = x4;
			} else if (p4inb12) {
				// 3 outside
				*xs = x4;
			} else {
				*xs = x2;
				intersection = INTERSECTION_AT_EDGE;
			}
		}
	} else {
		// x2 < x1
		if (x3 < x4) {
			if (x1 < x3 || x4 < x2) {
				intersection = INTERSECTION_OUTSIDE;
			} else if (p3inb12) {
				*xs = p4inb12 ? x4 : x3;
			} else if (p4inb12) {
				// 3 outside
				*xs = x4;
			} else {
				*xs = x2;
				intersection = INTERSECTION_AT_EDGE;
			}
		} else {
			// x4 < x3
			if (x1 < x4 || x3 < x2) {
				intersection = INTERSECTION_OUTSIDE;
			} else if (p3inb12) {
				*xs = x3;
			} else if (p4inb12) {
				*xs = x4;
			} else {
				*xs = x3;
			}
		}
	}
	return intersection;
}

// Line from a to b :
//	(1) ab = (1-s)*a+s*b = a + s*(b-a) = a + s * b'
//	(2) cd = c+t*(d-c) = c + t * d'
// where b' = b - a and d' = d - c.
// at intersection:
//	(3) a + s * b' = c + t * d'
//  Cross (3) with d';
// 	cross (a, d') + s * cross (b', d') = cross (c, d')
//		s = cross (c - a, d') / cross (b', d')
// Cross (3) with b'
//	cross (a, b') = cross (c, b') + t * cross (d', b')
//		t = cross (c - a, b') / cross (b', d')
// if cross (b', b') then b' and d' are parallel
// After Paul Rourke
// Area of triangle a,b,c = ((bx-ax)(cy-ay) - (cx-ax)(by-ay)) / 2
#define POS(ax,ay,bx,by,cx,cy) (bx - ax) * (cy - ay) - (cx - ax) * (by-ay);
// if POS(a,b,c) (> 0 ; <0, 0) then c is on (the left of;  the right of; collinear with) the line segment (a,b)

// Register the crossing of line a(1)b(2) with c(3)d(4), if the crossing is degenerate only register the tip.
static int LineSegments_getIntersection (double x1, double y1, double x2, double y2, double x3, double y3,
        double x4, double y4, double *mua, double *mub, double eps) {
	// bounding box pre-selection
	double min12 = x1 < x2 ? x1 : x2;
	double max12 = x1 > x2 ? x1 : x2;
	double min34 = x3 < x4 ? x3 : x4;
	double max34 = x3 > x4 ? x3 : x4;
	if (max12 - min34 < -eps or max34 - min12 < -eps) {
		return INTERSECTION_OUTSIDE;    // eps?
	}
	min12 = y1 < y2 ? y1 : y2;
	max12 = y1 > y2 ? y1 : y2;
	min34 = y3 < y4 ? y3 : y4;
	max34 = y3 > y4 ? y3 : y4;
	if (max12 - min34 < -eps or max34 - min12 < -eps) {
		return INTERSECTION_OUTSIDE;
	}

	double cad = (x3 - x1) * (y4 - y3) - (y3 - y1) * (x4 - x3); // cross (c - a, d')
	double cab = (x3 - x1) * (y2 - y1) - (y3 - y1) * (x2 - x1); // cross (c - a, b')
	double bd  = (x2 - x1) * (y4 - y3) - (y2 - y1) * (x4 - x3); // cross (b', d')

	if (fabs (bd) > eps) {
		*mua = cad / bd;
		*mub = cab / bd;
		if (*mua <= eps or * mua > 1 + eps or * mub < eps or * mub > 1 + eps) {
			return INTERSECTION_OUTSIDE;
		}
		if (*mua > eps and *mua <= 1 - eps and *mub >= eps and * mub < 1 - eps) {
			// This occurs most of the cases (hopefully)
			return INTERSECTION_PROPER;
		}
		// Now eps < mua,mub <= 1+eps
		// and at least one of the mu's is near 1,
		// the other is in [eps,1]
		if (fabs (*mua - 1) < eps) {
			*mua = 1;
		}
		if (fabs (*mub - 1) < eps) {
			*mub = 1;
		}
		// is the intersection at an edge or  at vertex
		if (*mua == 1) { // end of ab touches cd
			return *mub == 1 ? INTERSECTION_AT_VERTEX : INTERSECTION_AT_EDGE;
		} else { // ab crosses a vertex
			return INTERSECTION_AT_VERTEX;
		}
	} else { // ab and cd are parallel or coplanar
		if (fabs (cad) > eps and fabs (cab) > eps) {
			return INTERSECTION_OUTSIDE;
		}
		if (x1 == x2) {
			x1 = y1; x2 = y2; x3 = y3; x4 = y4;
		}
		double xs;
		int intersection = get_collinearIntersectionPoint (x1, x2, x3, x4, &xs);
		if (intersection != INTERSECTION_OUTSIDE) {
			*mua = fabs ( (xs - x1) / (x2 - x1));
			*mub = fabs ( (xs - x3) / (x4 - x3));
			intersection += INTERSECTION_COPLANAR;
		}
		return intersection;
	}
}

// Vertices:
// first node has prev = 0, last node has next = 0;
// entry marks the entrance of the OTHER polygon

Thing_define (Vertex, Data) {
	// new data:
public:
	double x, y, alpha;
	DLLNode neighbour;
	long poly_npoints, id;
	int intersect, entry;
	bool processed;
	// overridden methods:
	virtual void v_copy (Any data_to);
};

Thing_implement (Vertex, Data, 0);

void structVertex :: v_copy (thou) {
	thouart (Vertex);
	thy x = x;
	thy y = y;
	thy alpha = alpha;
	thy neighbour = neighbour;
	thy poly_npoints = poly_npoints;
	thy id = id;
	thy intersect = intersect;
	thy entry = entry;
	thy processed = processed;
}

Vertex Vertex_create ();
Vertex Vertex_create () {
	try {
		autoVertex me = Thing_new (Vertex);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Vertex not created.");
	}
}

Thing_define (Vertices, DLL) {
	// overridden methods:
protected:
	static int s_compare (Any data1, Any data2);
	virtual Data_CompareFunction v_getCompareFunction () {
		return s_compare;
	}
};
Thing_implement (Vertices, DLL, 0);

#define VERTEX(n) ((Vertex) (n -> data))

int structVertices :: s_compare (I, thou) {
	iam (DLLNode); thouart (DLLNode);
	return VERTEX (me) -> alpha < VERTEX (thee) -> alpha ? -1 : VERTEX (me) -> alpha > VERTEX (thee) -> alpha ? 1 : 0;
}

Vertices Vertices_create ();
Vertices Vertices_create () {
	try {
		Vertices me = Thing_new (Vertices);
		return me;
	} catch (MelderError) {
		Melder_throw ("Vertices not created.");
	}
}

void Vertices_addCopyBack (Vertices me, DLLNode n);
void Vertices_addCopyBack (Vertices me, DLLNode n) {
	try {
		autoDLLNode nc = (DLLNode) Data_copy (n);
		DLL_addBack ( (DLL) me, nc.transfer());
	} catch (MelderError) {
		Melder_throw (me, ": no copy added.");
	}
}

static bool pointsInsideInterval (double *x, long n, long istart, long iend, long *jstart, long *jend) {
	double xmax = x[istart], xmin = x[istart];
	long imax = istart, imin = istart;
	long iendmod = iend > istart ? iend : iend + n; // circular
	for (long i = istart + 1; i <= iendmod; i++) {
		long index = (i - 1) % n + 1; // make it circular
		if (x[index] > xmax) {
			xmax = x[index];
			imax = index;
		} else if (x[index] < xmin) {
			xmin = x[index];
			imin = index;
		}
	}
	*jstart = imin; *jend = imax;
	if (x[istart] > x[iend]) {
		*jstart = imax;
		*jend = imin;
	}
	if (x[istart] == x[*jstart] and x[iend] == x[*jend]) { // if there are duplicates of the extrema
		*jstart = istart; *jend = iend;
	}
	return *jstart == istart and * jend == iend;
}

Polygon Polygon_circularPermutation (Polygon me, long nshift);
Polygon Polygon_circularPermutation (Polygon me, long nshift) {
	try {
		autoPolygon thee = Data_copy (me);
		if (nshift != 0) {
			for (long i = 1; i <= my numberOfPoints; i++) {
				long inew = (i + nshift - 1) % my numberOfPoints + 1;
				thy x[inew] = my x[i];
				thy y[inew] = my y[i];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not circularly permuted.");
	}
}

void _Polygons_copyNonCollinearities (Polygon me, Polygon thee, long collstart, long collend);
void _Polygons_copyNonCollinearities (Polygon me, Polygon thee, long collstart, long collend) {
	// Determine if all collinear point are within the interval [colstart,colend]
	long jstart, jend;
	bool allPointsInside = my x[collstart] != my x[collend] ?
	                       pointsInsideInterval (my x, my numberOfPoints, collstart, collend, &jstart, &jend) :
	                       pointsInsideInterval (my y, my numberOfPoints, collstart, collend, &jstart, &jend);
	if (not allPointsInside) {
		if (collstart != jstart) { // also include the extreme point at start
			thy numberOfPoints++;
			thy x[thy numberOfPoints] = my x[jstart]; thy y[thy numberOfPoints] = my y[jstart];
		}
		if (collend != jend) { // also include the extreme point at end
			thy numberOfPoints++;
			thy x[thy numberOfPoints] = my x[jend]; thy y[thy numberOfPoints] = my y[jend];
		}
	}
	thy numberOfPoints++;
	thy x[thy numberOfPoints] = my x[collend]; thy y[thy numberOfPoints] = my y[collend];
}

#define AREA(x1,y1,x2,y2,x3,y3) (x1*(y2 - y3)+x2*(y3-y1)+x3*(y1-y2))
Polygon Polygon_simplify (Polygon me);
Polygon Polygon_simplify (Polygon me) {
	try {
		autoPolygon p1 = Data_copy (me);

		// pass 1: remove doublets
		long np = 1;
		for (long i = 2; i <= my numberOfPoints; i++) {
			if (my x[i] != p1 -> x[np] || my y[i] != p1 -> y[np]) {
				p1 -> x[++np] = my x[i]; p1 -> y[np] = my y[i];
			}
		}
		// last and first points!
		if (p1 -> x[np] == p1 -> x[1] && p1 -> y[np] == p1 -> y[1]) {
			np--;
		}
		if (np < 3) {
			Melder_throw ("Not enough points left after doublet removal.");
		}
		p1 -> numberOfPoints = np;

		// pass 2: remove collinearities

		autoPolygon p = Data_copy (p1.peek());
		p -> numberOfPoints = 0;
		// is there collinearity between the first and the last points of p1?
		double  eps = 1e-15;
		double area  = AREA (p1 -> x[np], p1 -> y[np], p1 -> x[1], p1 -> y[1], p1 -> x[2], p1 -> y[2]);
		long collend = 2;
		while (fabs (area) < eps && collend < np) {
			collend++;
			area = AREA (p1 -> x[collend - 2], p1 -> y[collend - 2], p1 -> x[collend - 1], p1 -> y[collend - 1], p1 -> x[collend], p1 -> y[collend]);
		}
		collend--;
		area = AREA (p1 -> x[np - 1], p1 -> y[np - 1], p1 -> x[np], p1 -> y[np], p1 -> x[1], p1 -> y[1]);
		long collstart = np - 1;
		while (fabs (area) < eps && collstart > collend) {
			collstart--;
			area = AREA (p1 -> x[collstart], p1 -> y[collstart], p1 -> x[collstart + 1], p1 -> y[collstart + 1], p1 -> x[collstart + 2], p1 -> y[collstart + 2]);
		}
		collstart++;
		if (collend - collstart + p1 -> numberOfPoints > 1) {
			_Polygons_copyNonCollinearities (p1.peek(), p.peek(), collstart, collend);
		} else {
			p -> numberOfPoints = 1; p -> x[1] = p1 -> x[1]; p -> y[1] = p1 -> y[1];
		}
		bool collinearity = false;
		long endpos = collstart - 1;
		for (long i = collend + 1; i <= endpos; i++) { // start was < numberOfPoints
			// if i-1, i, i+1 are coplanar, remove i
			// area = x1*(y2 - y3)+x2*(y3-y1)+x3*(y1-y2)
			area = AREA (p1 -> x[i - 1], p1 -> y[i - 1], p1 -> x[i], p1 -> y[i], p1 -> x[i + 1], p1 -> y[i + 1]);
			if (fabs (area) < eps) {
				if (collinearity) {
					collend = i + 1;
				} else { // start of possibly new series
					collinearity = true;
					collstart = i - 1; collend = i + 1;
				}
				if (i != endpos) {
					continue;
				}
			}
			if (not collinearity) {
				p -> numberOfPoints++;
				p -> x[p -> numberOfPoints] = p1 -> x[i];
				p -> y[p -> numberOfPoints] = p1 -> y[i];
			} else { // end of series of collinearities detected
				_Polygons_copyNonCollinearities (p1.peek(), p.peek(), collstart, collend);
				collinearity = false;
			}
		}
		if (not collinearity and collend != endpos + 1) { // only if previous series was not collinear
			p -> numberOfPoints++;
			p -> x[p -> numberOfPoints] = p1 -> x[endpos + 1];
			p -> y[p -> numberOfPoints] = p1 -> y[endpos + 1];
		}
		if (p -> numberOfPoints < 3) {
			Melder_throw ("Not enough points left after collinear points removal.");
		}

		autoPolygon thee = Data_copy (p.peek()); //
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not simplified.");
	}
}

#undef AREA

Vertices Polygon_to_Vertices (Polygon me, bool close);
Vertices Polygon_to_Vertices (Polygon me, bool close) {
	try {
		autoVertices thee = Vertices_create ();
		for (long i = 1 ; i <= my numberOfPoints; i++) {
			autoVertex v = Vertex_create ();
			v -> x = my x[i]; v -> y = my y[i];
			autoDLLNode n = DLLNode_create ( (Data) v.transfer());
			DLL_addBack ( (DLL) thee.peek(), n.transfer());
		}
		Melder_assert (thy numberOfNodes == my numberOfPoints);
		if (close) {
			Vertices_addCopyBack (thee.peek(), thy front);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Vertices created.");
	}
}

#define SKIP_INTERSECTION_NODES(n) while(VERTEX(n) -> intersect != 0) n = n -> next;


void Vertices_print (Vertices me, Vertices thee);
void Vertices_print (Vertices me, Vertices thee) {
	long ns = 0, nc = 0, nt, nt2;
	//	MelderInfo_open();
	DLLNode n = my front;
	MelderInfo_writeLine (L"");
	while (n != 0) {
		double x = VERTEX (n) -> x, y = VERTEX (n) -> y, alpha = VERTEX (n) -> alpha;
		const wchar_t *type = 0, *itype;
		if (VERTEX (n) -> intersect == 0) {
			type = L"S"; ns++; nt = ns; itype = L"-"; nt2 = 0;
		} else {
			type = L"I"; nt = VERTEX (n) -> id; nt2 = VERTEX (VERTEX (n) -> neighbour) -> id;
			itype = Melder_integer (VERTEX (n) -> intersect);
		}
		MelderInfo_write (type, Melder_integer (nt), L" I", itype, L", (", Melder_double (x), L", ", Melder_double (y), L"), ");
		MelderInfo_write (Melder_double (alpha), L", E", Melder_integer (VERTEX (n) -> entry), L"(", Melder_integer (nt2), L")\n");
		n = n -> next;
	}
	MelderInfo_writeLine (L"");
	n = thy front;
	while (n != 0) {
		double x = VERTEX (n) -> x, y = VERTEX (n) -> y, alpha = VERTEX (n) -> alpha;
		const wchar_t *type = 0, *itype;
		if (VERTEX (n) -> intersect == 0) {
			type = L"C"; nc++; nt = nc; itype = L"-"; nt2 = 0;
		} else {
			type = L"I"; nt = VERTEX (n) -> id; nt2 = VERTEX (VERTEX (n) -> neighbour) -> id;
			itype = Melder_integer (VERTEX (n) -> intersect);
		}
		MelderInfo_write (type, Melder_integer (nt), L" I", itype, L", (", Melder_double (x), L", ", Melder_double (y), L"), ");
		MelderInfo_write (Melder_double (alpha), L", E", Melder_integer (VERTEX (n) -> entry), L"(", Melder_integer (nt2), L")\n");
		n = n -> next;
	}
	//	MelderInfo_close();
}

void Vertices_sortIntersections (Vertices me);
void Vertices_sortIntersections (Vertices me) {
	DLLNode ni = my front, first;
	bool intersections = false;
	while (ni != my back) {
		if (VERTEX (ni) -> intersect == 0) {
			if (intersections) { // the previous was the last of a series of intersections
				DLL_sortPart ( (DLL) me, first, ni -> prev);
				// restore myNode pointers
				intersections = false;
			}
		} else {
			if (not intersections) {
				first = ni;
				intersections = true;
			}
		}
		ni = ni -> next;
	}
	// we might have missed the last series of intersections
	if (intersections) {
		DLL_sortPart ( (DLL) me, first, ni -> prev);
	}
}

void Vertices_addIntersections (Vertices me, Vertices thee);
void Vertices_addIntersections (Vertices me, Vertices thee) {
	try {
		double eps = 1e-15;
		long id = 0;
		if (my numberOfNodes < 4 || thy numberOfNodes < 4) {
			Melder_throw ("We need at least three vertices.");
		}
		DLLNode ni = my front; // the node index  in me (s)
		while (ni != my back) { // until penultimate
			double x1 = VERTEX (ni) -> x, y1 =  VERTEX (ni) -> y;
			double x2 = VERTEX (ni -> next) -> x, y2 = VERTEX (ni -> next) -> y;
			DLLNode nj = thy front; // the current node index in thee (c)
			while (nj != thy back && VERTEX (nj) -> intersect == 0) {
				DLLNode njn = nj -> next;
				SKIP_INTERSECTION_NODES (njn)
				double x3 = VERTEX (nj) -> x, y3 = VERTEX (nj) -> y;
				double x4 = VERTEX (njn) -> x, y4 = VERTEX (njn) -> y, mua, mub;
				int intersection = LineSegments_getIntersection (x1, y1, x2, y2, x3, y3, x4, y4, &mua, &mub, eps);
				if (intersection != INTERSECTION_OUTSIDE) {
					id++;
					// 1. create the vertices
					autoVertex ins = Vertex_create ();
					ins -> x = x1 + mua * (x2 - x1);
					ins -> y = y1 + mua * (y2 - y1);
					ins -> alpha = mua;
					ins -> intersect = intersection;
					ins -> id = id;
					autoVertex inc = Data_copy (ins.peek());
					inc -> alpha = mub;
					// 2. create the nodes
					autoDLLNode ns = DLLNode_create (0);
					autoDLLNode nc = DLLNode_create (0);
					// 3. link the neighbours + copy the links
					DLLNode njc = ins -> neighbour = nc.peek();
					DLLNode nic = inc -> neighbour = ns.peek();
					// 4. transfer the vertices to the nodes
					ns -> data = (Data) ins.transfer();
					nc -> data = (Data) inc.transfer();
					// 5. add the nodes to the list
					DLL_addAfter ( (DLL) me, ni, ns.transfer());
					DLL_addAfter ( (DLL) thee, nj, nc.transfer());
					// 6. set node pointer to inserted nodes
					ni = nic; nj = njc;
				}
				nj = nj -> next;
				SKIP_INTERSECTION_NODES (nj)
			}
			ni = ni -> next;
		}
		if (Melder_debug == -1) {
			MelderInfo_open();
			Vertices_print (me, thee);
		}
		Vertices_sortIntersections (me);
		Vertices_sortIntersections (thee);
		if (Melder_debug == -1) {
			Vertices_print (me, thee);
		}
	} catch (MelderError) {
		Melder_throw ("Intersections not calculated.");
	}
}

#define Polygon_EN 1
#define Polygon_EX 2
#define Polygon_ENEX 3
#define Polygon_EXEN 4

void Vertices_markEntryPoints (Vertices me, int firstLocation);
void Vertices_markEntryPoints (Vertices me, int firstLocation) {
	int entry = (firstLocation == Polygon_INSIDE) ? Polygon_EX : (firstLocation == Polygon_OUTSIDE) ? Polygon_EN : Polygon_ENEX; // problematic when on boundary
	// my back/front can never be an intersection node
	for (DLLNode ni = my front -> next; ni != my back; ni = ni -> next) {
		if (VERTEX (ni) -> intersect == 0) {
			continue;
		}
		VERTEX (ni) -> entry = entry;
		entry = (entry == Polygon_EN) ? Polygon_EX : (entry == Polygon_EX) ? Polygon_EN : Polygon_ENEX;
	}
}

Vertices Verticeses_connectClippingPathsUnion (Vertices me, Vertices thee);
Vertices Verticeses_connectClippingPathsUnion (Vertices me, Vertices thee) {
	try {
		// find my first vertex outside thee, by searching the first intersection entry

		DLLNode firstOutside = my front;
		bool firstOutsideSet = true;
		for (DLLNode ni = my front; ni != 0; ni = ni -> next) {
			if (VERTEX (ni) -> intersect == 0) { //
				if (firstOutsideSet) {
					continue;
				}
				firstOutside = ni; break;
			} else if (VERTEX (ni) -> entry == Polygon_EN) {
				break;
			}
			// We were inside
			else {
				firstOutsideSet = false;
			}
		}

		autoVertices him = Vertices_create ();
		long poly_npoints = 0;
		DLLNode current = firstOutside;
		bool inside = false, forward = true;
		do {
			if (VERTEX (current) -> intersect == 0) {
				Vertices_addCopyBack (him.peek(), current); poly_npoints++;
			} else { // intersection point
				// store and jump to other polygon
				Vertices_addCopyBack (him.peek(), current); poly_npoints++;
				current = VERTEX (current) -> neighbour; inside = not inside;
				forward = VERTEX (current) ->  entry == Polygon_EX;
			}
			if (forward) {
				current = current -> next;
				if (current == 0) {
					current = inside ? thy front : my front;
				}
			} else {
				current = current -> prev;
				if (current == 0) {
					current = inside ? thy back : my back;
				}
			}
		} while (current != firstOutside and current != 0 and poly_npoints < my numberOfNodes);
		VERTEX (his front) -> poly_npoints = poly_npoints;
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no clipping path.");
	}
}

Vertices Verticeses_connectClippingPaths (Vertices me, bool use_myinterior, Vertices thee, bool use_thyinterior);
Vertices Verticeses_connectClippingPaths (Vertices me, bool use_myinterior, Vertices thee, bool use_thyinterior) {
	try {
		autoVertices him = Vertices_create ();
		DLLNode prevPoly;
		long poly_npoints = 0;
		for (DLLNode ni = my front; ni != 0; ni = ni -> next) {
			if ( (VERTEX (ni) -> intersect == 0) || VERTEX (ni) -> processed) {
				continue;
			}
			// Intersection found: start new polygon
			DLLNode currentPoly = his back;
			if (currentPoly == his front) {
				Vertices_addCopyBack (him.peek(), ni); poly_npoints++;
				prevPoly = his front;
			} else {
				Vertices_addCopyBack (him.peek(), ni); poly_npoints++;
				VERTEX (prevPoly) -> poly_npoints = poly_npoints;
				poly_npoints = 0;
				prevPoly = currentPoly;
			}
			DLLNode current = ni;
			VERTEX (current) -> processed = true;
			long jumps = 0;
			do {
				if (VERTEX (current) -> entry == Polygon_EN) {
					while ( (current = current -> next) != 0 and VERTEX (current) -> intersect == 0) {
						Vertices_addCopyBack (him.peek(), current); poly_npoints++;
					}
					if (current == 0) { // back of list? Goto front
						current = (jumps % 2 == 0) ? my front : thy front;
						while ( (current = current -> next) != 0 and VERTEX (current) -> intersect == 0) {
							Vertices_addCopyBack (him.peek(), current); poly_npoints++;
						}
						Vertices_addCopyBack (him.peek(), current); poly_npoints++; // intersection point
					} else if (current == ni) {
						break;    // done
					} else {
						current = current -> prev;
					}
				} else if (VERTEX (current) -> entry == Polygon_EX) {
					while ( (current = current -> prev) != 0 and VERTEX (current) -> intersect == 0) {
						Vertices_addCopyBack (him.peek(), current); poly_npoints++;
					}
					if (current == 0) { // start of list? Goto end
						current = (jumps % 2 == 0) ? my back : thy back;
						while ( (current = current -> prev) != 0 and VERTEX (current) -> intersect == 0) {
							Vertices_addCopyBack (him.peek(), current); poly_npoints++;
						}
						Vertices_addCopyBack (him.peek(), current); poly_npoints++; // intersection point
					} else if (current == ni) {
						break;    // done
					} else {
						current = current -> next;
					}
				} else {
				}
				VERTEX (current) -> processed = true;
				current = VERTEX (current) -> neighbour; jumps++;
				VERTEX (current) -> processed = true;
			} while (current != ni); // polygon closed
		}
		// last polygon:
		VERTEX (prevPoly) -> poly_npoints = poly_npoints;
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no clipping vertices created.");
	}
}

//
Polygon Vertices_to_Polygon (Vertices me, DLLNode *ni);
Polygon Vertices_to_Polygon (Vertices me, DLLNode *ni) {
	DLLNode n = *ni;
	try {
		long i = 1, nPoints = VERTEX (n) -> poly_npoints;
		if (nPoints == 0) {
			Melder_throw ("No number info.");
		}
		autoPolygon thee = Polygon_create (nPoints);
		thy x[i] = VERTEX (n) -> x; thy y[i] = VERTEX (n) -> y;
		while ( (n = n -> next) != 0 and VERTEX (n) -> poly_npoints == 0 and i <= nPoints) {
			i++; thy x[i] = VERTEX (n) -> x; thy y[i] = VERTEX (n) -> y;
		}
		*ni =  n;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Polygon not created.");
	}
}

Collection Vertices_to_Polygons (Vertices me);
Collection Vertices_to_Polygons (Vertices me) {
	try {
		autoCollection thee = Collection_create (classPolygon, 10);
		DLLNode ni = my front;
		do {
			autoPolygon p = Vertices_to_Polygon (me, & ni);
			Collection_addItem (thee.peek(), p.transfer());
		} while (ni != 0);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no polygon collection created.");
	}
}

Collection Polygons_findClippings (Polygon me, bool use_myinterior, Polygon thee, bool use_thyinterior);
Collection Polygons_findClippings (Polygon me, bool use_myinterior, Polygon thee, bool use_thyinterior) {
	try {
		autoVertices s = Polygon_to_Vertices (me, true); // subject
		long ns = s -> numberOfNodes;
		autoVertices c = Polygon_to_Vertices (thee, true); // clip
		long nc = c -> numberOfNodes;
		double eps = 1e-15;

		// phase 1: Get all intersections and add them to both lists

		Vertices_addIntersections (s.peek(), c.peek());
		long nnewnodes = s -> numberOfNodes - ns;
		int firstLocation = Polygon_getLocationOfPoint (thee, my x[1], my y[1], eps);
		if (nnewnodes == 0) { // no crossings, either one completely inside the other or separate
			autoCollection apc;
			if (not use_myinterior and not use_thyinterior and firstLocation == Polygon_INSIDE) {
				autoPolygon ap = Data_copy (thee);
				Collection_addItem (apc.peek(), ap.transfer());
			} else {
				autoPolygon ap = Data_copy (me);
				Collection_addItem (apc.peek(), ap.transfer());
			}
			return apc.transfer();
		}

		// phase 2: Determine intersections as entry / exit points

		Vertices_markEntryPoints (s.peek(), firstLocation);

		firstLocation = Polygon_getLocationOfPoint (me, thy x[1], thy y[1], eps);
		Vertices_markEntryPoints (c.peek(), firstLocation);
		if (Melder_debug == -1) {
			Vertices_print (s.peek(), c.peek()); MelderInfo_close();
			Melder_throw ("Bail out of Polygons_findClippings.");
		}

		// phase 3: Determine the clipping paths
		// use_myinterior use_thyinterior  result
		//  true         false      diff me - thee
		//  false        false      union
		//  true         true       clip
		//  false        true       diff thee - me
		autoVertices pgs = 0;
		if (not use_myinterior and not use_thyinterior) {
			pgs.reset (Verticeses_connectClippingPathsUnion (s.peek(), c.peek()));
		} else {
			pgs.reset (Verticeses_connectClippingPaths (s.peek(), use_myinterior, c.peek(), use_thyinterior));
		}
		// phase 4: to Polygons

		autoCollection pols = Vertices_to_Polygons (pgs.peek());
		return pols.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no union Polygon created.");
	}
}

Collection Polygons_clip (Polygon subject, Polygon clipper) {
	try {
		autoCollection him = Polygons_findClippings (subject, true, clipper, true);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (subject, ": no union created.");
	}
}

Polygon Polygons_union (Polygon me, Polygon thee);
Polygon Polygons_union (Polygon me, Polygon thee) {
	try {
		autoCollection him = Polygons_findClippings (me, false, thee, false);
		//Melder_assert (his size == 1);
		autoPolygon p = (Polygon) Collection_subtractItem (him.peek(), 1);
		Collection_dontOwnItems (him.peek());
		return p.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no union created.");
	}
}

#define CROSSING (my y[i] < y0) != (my y[ip1] < y0)
#define AREA { a = (my x[i]-x0)*(my y[ip1]-y0) - (my x[ip1]-x0)*(my y[i]-y0); if (fabs (a) <= eps) return Polygon_EDGE; }
#define RIGHT_CROSSING (a > 0) == (my y[ip1] > my y[i])
#define MODIFY_CROSSING_NUMBER { if (my y[ip1] > my y[i]) nup++; else nup--; }

int Polygon_getLocationOfPoint (Polygon me, double x0, double y0, double eps);
int Polygon_getLocationOfPoint (Polygon me, double x0, double y0, double eps) {
	if (my y[1] == y0 and my x[1] == x0) {
		return Polygon_VERTEX;
	}

	long nup = 0;
	for (long i = 1; i <= my numberOfPoints; i++) {
		double a;
		long ip1 = i < my numberOfPoints ? i + 1 : 1;
		if (my y[ip1] == y0) {
			if (my x[ip1] == x0) {
				return Polygon_VERTEX;
			} else if (my y[i] == y0 and (my x[ip1] > x0) == (my x[i] < x0)) {
				return Polygon_EDGE;
			}
		}
		if (CROSSING) {
			if (my x[i] >= x0) {
				if (my x[ip1] > x0) MODIFY_CROSSING_NUMBER
					else {
						AREA
						if (RIGHT_CROSSING) MODIFY_CROSSING_NUMBER
						}
			} else {
				if (my x[ip1] > x0) {
					AREA
					if (RIGHT_CROSSING) MODIFY_CROSSING_NUMBER
					}
			}
		}
	}
	return nup % 2 == 0 ? Polygon_OUTSIDE : Polygon_INSIDE;
}

/* End of file Polygon_extensions.cpp */
