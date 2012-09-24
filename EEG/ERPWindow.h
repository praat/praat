#ifndef _ERPWindow_h_
#define _ERPWindow_h_
/* ERPWindow.h
 *
 * Copyright (C) 2012 Paul Boersma
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

#include "SoundEditor.h"
#include "ERP.h"

Thing_define (ERPWindow, SoundEditor) { public:
	// overridden methods:
		virtual const wchar_t * v_getChannelName (long channelNumber) {
			ERP erp = (ERP) this -> data;
			return erp -> d_channelNames [channelNumber];
		}
		virtual void v_drawSelectionViewer ();
	// overridden preferences:
		public:
			static void f_preferences ();
			static bool s_showSelectionViewer; virtual bool & pref_showSelectionViewer () { return s_showSelectionViewer; }
			static kTimeSoundEditor_scalingStrategy s_sound_scalingStrategy; virtual kTimeSoundEditor_scalingStrategy & pref_sound_scalingStrategy () { return s_sound_scalingStrategy; }
			static double s_sound_scaling_height;  virtual double & pref_sound_scaling_height  () { return s_sound_scaling_height;  }
			static double s_sound_scaling_minimum; virtual double & pref_sound_scaling_minimum () { return s_sound_scaling_minimum; }
			static double s_sound_scaling_maximum; virtual double & pref_sound_scaling_maximum () { return s_sound_scaling_maximum; }
};

ERPWindow ERPWindow_create (const wchar_t *title, ERP data);

/* End of file ERPWindow.h */
#endif
