#ifndef _vorbis_os_h_
#define _vorbis_os_h_
/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2015             *
 * by the Xiph.Org Foundation https://xiph.org/                     *
 *                                                                  *
 ********************************************************************

 function: #ifdef jail to whip a few platforms into the UNIX ideal.

 ********************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include "config_types.h"

#include "vorbis_misc.h"

#ifndef _V_IFDEFJAIL_H_
#  define _V_IFDEFJAIL_H_

#ifndef M_PI
#  define M_PI (3.1415926536f)
#endif

#endif /* _V_IFDEFJAIL_H_ */

#ifdef HAVE_ALLOCA_H
#  include <alloca.h>
#endif

#ifdef USE_MEMORY_H
#  include <memory.h>
#endif

#if defined(__WINS__)
#  define alloca _alloca
#endif


typedef int vorbis_fpu_control;

static inline int vorbis_ftoi(double f){
        /* Note: MSVC and GCC (at least on some systems) round towards zero, thus,
           the floor() call is required to ensure correct roudning of
           negative numbers */
        return (int)floor(f+.5);
}

/* We don't have special code for this compiler/arch, so do it the slow way */
#  define vorbis_fpu_setround(vorbis_fpu_control) {}
#  define vorbis_fpu_restore(vorbis_fpu_control) {}


#endif /* _vorbis_os_h_ */
