/* Eigen_and_Matrix.c
 *
 * Copyright (C) 1993-2004 David Weenink
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
 djmw 20020327
 djmw 20020813 GPL header
 djmw 20040219 Eigen_and_Covariance_project added.
*/

#include "Eigen_and_SSCP.h"

static void Eigen_and_SSCP_project_ (I, thou, Any sscp)
{
	iam (Eigen); 
	thouart (SSCP); 
	SSCP him = (SSCP) sscp;
	long i, j, k, m;
	 
	for (i = 1; i <= my numberOfEigenvalues; i++)
	{
		double tmp;
		for (j = i; j <= my numberOfEigenvalues; j++)
		{
			tmp = 0;
			for (k = 1; k <= my dimension; k++)
			{
				for (m = 1; m <= my dimension; m++)
				{
					tmp += my eigenvectors[i][k] * thy data[k][m] * 
						my eigenvectors[j][m];
				}
			}
			his data[i][j] = his data[j][i] = tmp;
		}
		
		tmp = 0;
		for (m = 1; m <= my dimension; m++)
		{
			tmp += thy centroid[m] * my eigenvectors[i][m];
		}
		his centroid[i] = tmp;
	}
	his numberOfObservations = SSCP_getNumberOfObservations (thee);
}


SSCP Eigen_and_SSCP_project (I, SSCP thee)
{
	iam (Eigen); 
	SSCP him;
	
	if (thy numberOfRows != my dimension) return Melder_errorp
		("SSCP_and_Eigen_project: dimensions don't agree.");
	
	him = SSCP_create (my numberOfEigenvalues);	
	if (him == NULL) return NULL;
	
	Eigen_and_SSCP_project_ (me, thee, him);
	
	return him;
}

Covariance Eigen_and_Covariance_project (I, Covariance thee)
{
	iam (Eigen); 
	Covariance him;
	
	if (thy numberOfRows != my dimension) return Melder_errorp
		("Covariance_and_Eigen_project: dimensions don't agree.");
	
	him = Covariance_create (my numberOfEigenvalues);	
	if (him == NULL) return NULL;
	
	Eigen_and_SSCP_project_ (me, thee, him);
	
	return him;
}

/* End of file Eigen_and_SSCP.c */
