/* FeatureWeights_def.h
 *
 * Copyright (C) 2007-2008 Ola Söder
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

/* $URL: svn://pegasos.dyndns.biz/praat/trunk/kNN/FeatureWeights_def.h $
 * $Rev: 137 $
 * $Author: stix $
 * $Date: 2008-08-10 19:34:07 +0200 (Sun, 10 Aug 2008) $
 * $Id: FeatureWeights_def.h 137 2008-08-10 17:34:07Z stix $
 */

/*
 * os 20070529 Intial release?
 */

#define ooSTRUCT FeatureWeights
    oo_DEFINE_CLASS (FeatureWeights, Data)
        oo_OBJECT (TableOfReal, 0, fweights)
    oo_END_CLASS (FeatureWeights)
#undef ooSTRUCT
