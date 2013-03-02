#ifndef _prefs_h_
#define _prefs_h_
/* prefs.h
 *
 * Copyright (C) 2013 Paul Boersma
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

#include "Preferences.h"

#define prefs_begin(Klas) \
	public: static void f_preferences (); \
	virtual void v_copyPreferencesToInstance ();

#define prefs_add_int(Klas,name,version,default) \
	private: static int s_##name; public: virtual int & pref_##name () { return s_##name; } \
	private: static const wchar_t * sdefault_##name; public: virtual const wchar_t * default_##name () { return sdefault_##name; }
#define prefs_add_int_with_data(Klas,name,version,default)  public: int p_##name; prefs_add_int (Klas, name, version, default)

#define prefs_add_long(Klas,name,version,default) \
	private: static long s_##name; public: virtual long & pref_##name () { return s_##name; } \
	private: static const wchar_t * sdefault_##name; public: virtual const wchar_t * default_##name () { return sdefault_##name; }
#define prefs_add_long_with_data(Klas,name,version,default)  public: long p_##name; prefs_add_long (Klas, name, version, default)

#define prefs_add_bool(Klas,name,version,default) \
	private: static bool s_##name; public: virtual bool & pref_##name () { return s_##name; } \
	private: static bool sdefault_##name; public: virtual bool default_##name () { return sdefault_##name; }
#define prefs_add_bool_with_data(Klas,name,version,default)  public: bool p_##name; prefs_add_bool (Klas, name, version, default)

#define prefs_add_double(Klas,name,version,default) \
	private: static double s_##name; public: virtual double & pref_##name () { return s_##name; } \
	private: static const wchar_t * sdefault_##name; public: virtual const wchar_t * default_##name () { return sdefault_##name; }
#define prefs_add_double_with_data(Klas,name,version,default)  public: double p_##name; prefs_add_double (Klas, name, version, default)

#define prefs_add_enum(Klas,name,version,enumerated,default) \
	private: static enum enumerated s_##name; public: virtual enum enumerated & pref_##name () { return s_##name; } \
	private: static enum enumerated sdefault_##name; public: virtual enum enumerated default_##name () { return sdefault_##name; }
#define prefs_add_enum_with_data(Klas,name,version,enumerated,default)  public: enumerated p_##name; prefs_add_enum (Klas, name, version, enumerated, default)

#define prefs_add_string(Klas,name,version,default) \
	private: static wchar_t s_##name [Preferences_STRING_BUFFER_SIZE]; public: virtual wchar_t * pref_##name () { return & s_##name [0]; } \
	private: static const wchar_t * sdefault_##name; public: virtual const wchar_t * default_##name () { return sdefault_##name; }
#define prefs_add_string_with_data(Klas,name,version,default)  public: wchar_t p_##name [Preferences_STRING_BUFFER_SIZE]; prefs_add_string (Klas, name, version, default)

#define prefs_end(Klas)

/* End of file prefs.h */
#endif
