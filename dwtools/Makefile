# Makefile
# 
#  Copyright (C) 1993-2005 David Weenink
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or (at
#  your option) any later version.
# 
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# 
# 
# 
#  djmw 20030825 Removed dependency on -I ../GSL
#  djmw 20061222 Latest modification.


include ../makefile.defs

CFLAGS =  -I ../LPC -I ../fon -I ../sys -I ../dwsys -I ../stat

all: libdwtools.a

clean:
	rm *.o

OBJECTS = Activation.o AffineTransform.o \
	Categories.o CategoriesEditor.o \
	Categories_and_Strings.o CCA.o CCA_and_Correlation.o \
	CC.o CCs_to_DTW.o \
	ClassificationTable.o Confusion.o \
	Configuration.o Configuration_AffineTransform.o \
	Configuration_and_Procrustes.o Distance.o \
	DTW.o DTW_and_TextGrid.o \
	Discriminant.o  Discriminant_Pattern_Categories.o \
	Eigen_and_Matrix.o Eigen_and_Procrustes.o \
	Eigen_and_TableOfReal.o\
	Eigen_and_SSCP.o Excitations.o \
	FilterBank.o Formant_extensions.o \
	Intensity_extensions.o \
	LFCC.o LongSound_extensions.o \
	MelFilter_and_MFCC.o MFCC.o \
	manual_dwtools.o manual_MDS.o manual_Permutation.o \
	Minimizers.o \
	Matrix_extensions.o \
	Matrix_Categories.o MDS.o \
	Pattern.o PCA.o \
	Pitch_extensions.o Polynomial.o \
	Polygon_extensions.o Procrustes.o \
	Proximity.o \
	Sampled2.o \
	Sound_and_FilterBank.o Sound_extensions.o \
	Sound_to_MFCC.o \
	Sound_to_Pitch2.o Sound_to_SPINET.o SPINET.o SPINET_to_Pitch.o \
	Spectrum_extensions.o SSCP.o Strings_extensions.o \
	Table_extensions.o TableOfReal_and_SVD.o\
	TableOfReal_extensions.o TableOfReal_and_Matrix.o \
	TableOfReal_and_Permutation.o \
	TextGrid_extensions.o \
	praat_MDS_init.o praat_David_init.o

$(OBJECTS): *.h ../sys/*.h ../fon/*.h ../dwsys/*.h ../stat/*.h ../LPC/*.h


libdwtools.a: $(OBJECTS)
	touch libdwtools.a
	rm libdwtools.a
	ar cq libdwtools.a $(OBJECTS) 
	$(RANLIB) libdwtools.a
	
