/* Sound_and_PCA.cpp
 *
 * Copyright (C) 2012 David Weenink
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
 djmw 20121001
*/

#include "ICA.h"
#include "Sound_and_PCA.h"
#include "Sound_extensions.h"
#include "NUM2.h"

static void checkChannelsWithinRange (long *channels, long n, long min, long max) {
	for (long i = 1; i <= n; i++) {
		if (channels[i] < min || channels[i] > max) {
			Melder_throw ("Channel ", Melder_integer (channels[i]), " is not within range [", Melder_integer (min), ", ", Melder_integer (max), "].");
		}
	}
}

PCA Sound_to_PCA_channels (Sound me, double startTime, double endTime) {
	try {
		// covariance is cross-correlation with lag time 0
		autoCrossCorrelationTable thee = Sound_to_CrossCorrelationTable (me, startTime, endTime, 0);
		autoPCA him = SSCP_to_PCA (thee.peek());
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no PCA created.");
	}
}

Sound Sound_and_PCA_to_Sound_pc_selectedChannels (Sound me, PCA thee, long numberOfComponents, long *channels, long numberOfChannels) {
	try {
		bool channelSelection = channels != 0 && numberOfChannels > 0;
		if (numberOfComponents <= 0 || numberOfComponents > thy numberOfEigenvalues) {
			numberOfComponents = thy numberOfEigenvalues;
		}
		numberOfComponents = numberOfComponents > my ny ? my ny : numberOfComponents;
		if (channelSelection) {
			checkChannelsWithinRange (channels, numberOfChannels, 1, my ny);
		}
		autoSound him = (Sound) Data_copy (me);
		// R['i',j] = E(i,k]*S['k',j]
		// use kij-variant for faster inner loop
		for (long k = 1; k <= thy dimension; k++) {
			long channel_k = channelSelection ? channels[k] : k;
			for (long i = 1; i <= numberOfComponents; i++) {
				long channel_i = channelSelection ? channels[i] : i;
				double ev_ik = thy eigenvectors[i][k];
				for (long j = 1; j <= my nx; j++) {
					his z[channel_i][j] += ev_ik * my z[channel_k][j];
				}
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no principal components calculated with ", thee);
	}
}

Sound Sound_and_PCA_principalComponents (Sound me, PCA thee, long numberOfComponents) {
	return Sound_and_PCA_to_Sound_pc_selectedChannels (me, thee, numberOfComponents, NULL, 0);
}

Sound Sound_and_PCA_whitenSelectedChannels (Sound me, PCA thee, long numberOfComponents, long *channels, long numberOfChannels) {
	try {
		bool channelSelection = channels != 0 && numberOfChannels > 0;
		if (numberOfComponents <= 0 || numberOfComponents > thy numberOfEigenvalues) {
            numberOfComponents = thy numberOfEigenvalues;
        }
		if (channelSelection) {
			checkChannelsWithinRange (channels, numberOfChannels, 1, my ny);
		}
        autoNUMmatrix<double> whiten (1, thy dimension, 1, thy dimension);
		// W = E D^(-1/2) E' from http://cis.legacy.ics.tkk.fi/aapo/papers/IJCNN99_tutorialweb/node26.html
        for (long i = 1; i <= thy dimension; i++) {
            for (long j = i; j <= thy dimension; j++) {
                double wij = 0;
                for (long k = 1; k <= numberOfComponents; k++) {
                    wij += thy eigenvectors[k][i] * thy eigenvectors[k][j] / sqrt (thy eigenvalues[k]);
                }
                whiten[i][j] = whiten[j][i] = wij;
            }
        }
		autoSound him = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
		for (long k = 1; k <= numberOfChannels; k++) {
			long channel_k = channelSelection ? channels[k] : k;
            for (long i = 1; i <= numberOfChannels; i++) {
                long channel_i = channelSelection ? channels[i] : i;
				double w_ik = whiten[i][k];
                for (long j = 1; j <= my nx; j++) {
                    his z[channel_i][j] += w_ik * my z[channel_k][j];
                }
            }
        }
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not created.");
	}
}

Sound Sound_and_PCA_whitenChannels (Sound me, PCA thee, long numberOfComponents) {
	return Sound_and_PCA_whitenSelectedChannels (me, thee, numberOfComponents, NULL, 0);
}

/* End of file Sound_and_PCA.cpp */
