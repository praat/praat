/* GuiWindow.c
 *
 * Copyright (C) 1993-2007 Paul Boersma
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

/*
 * pb 2004/01/07 this file separated from Gui.c
 * pb 2004/02/12 don't trust window modification feedback on MacOS 9
 * pb 2004/04/06 GuiWindow_drain separated from XmUpdateDisplay
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2007/06/19 wchar_t
 * pb 2007/12/30 extraction
 */

#include "GuiP.h"
#include "UnicodeData.h"
#define my  me ->
#define iam(x)  x me = (x) void_me

typedef struct structGuiWindow {
	Widget widget;
	void (*goAwayCallback) (void *boss);
	void *goAwayBoss;
} *GuiWindow;

#if gtk
	static gboolean _GuiWindow_destroyCallback (Widget widget, GdkEvent *event, gpointer void_me) {
		(void) widget;
		iam (GuiWindow);
		Melder_free (me);
		return TRUE;
	}

	static gboolean _GuiWindow_goAwayCallback (Widget widget, GdkEvent *event, gpointer void_me) {
		(void) widget;
		iam (GuiWindow);
		if (my goAwayCallback != NULL) {
			my goAwayCallback (my goAwayBoss);
		}
		return TRUE;
	}
#elif motif
	static void _GuiMotifWindow_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiWindow);
		//Melder_casual ("destroying window widget");
		Melder_free (me);
	}
	static void _GuiMotifWindow_goAwayCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiWindow);
		if (my goAwayCallback != NULL) {
			my goAwayCallback (my goAwayBoss);
		}
	}
#endif

Widget GuiWindow_create (Widget parent, int x, int y, int width, int height,
	const wchar_t *title, void (*goAwayCallback) (void *goAwayBoss), void *goAwayBoss, unsigned long flags)
{
	GuiWindow me = Melder_calloc (struct structGuiWindow, 1);
	my goAwayCallback = goAwayCallback;
	my goAwayBoss = goAwayBoss;
	#if gtk
		(void) parent;
		Widget shell = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		g_signal_connect (G_OBJECT (shell), "delete-event", goAwayCallback ? G_CALLBACK (_GuiWindow_goAwayCallback) : G_CALLBACK (gtk_widget_hide), me);
		g_signal_connect (G_OBJECT (shell), "destroy-event", G_CALLBACK (_GuiWindow_destroyCallback), me);

		// TODO: Paul ik denk dat Gui_AUTOMATIC voor GTK gewoon -1 moet zijn veel minder (onnodig) gezeur
		if (width == Gui_AUTOMATIC) width = -1;
		if (height == Gui_AUTOMATIC) height = -1;

		gtk_window_set_default_size (GTK_WINDOW (shell), width, height);
		GuiWindow_setTitle (shell, title);
		
		my widget = gtk_vbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (shell), my widget);
		_GuiObject_setUserData (my widget, me);
	#elif motif
		#if win || mac
			(void) parent;
			Widget shell = XmCreateShell (NULL, flags & GuiWindow_FULLSCREEN ? "Praatwulgfullscreen" : "Praatwulg", NULL, 0);
		#else
			Widget shell = XtAppCreateShell (NULL, "Praatwulg", applicationShellWidgetClass, XtDisplay (parent), NULL, 0);
			/*Widget shell = XtVaCreateWidget ("picture", topLevelShellWidgetClass, widget, NULL);*/
		#endif
		XtVaSetValues (shell, XmNdeleteResponse, goAwayCallback ? XmDO_NOTHING : XmUNMAP, NULL);
		if (x != Gui_AUTOMATIC) XtVaSetValues (shell, XmNx, x, NULL);
		if (y != Gui_AUTOMATIC) XtVaSetValues (shell, XmNy, y, NULL);
		if (width != Gui_AUTOMATIC) XtVaSetValues (shell, XmNwidth, (Dimension) width, NULL);
		if (height != Gui_AUTOMATIC) XtVaSetValues (shell, XmNheight, (Dimension) height, NULL);
		if (goAwayCallback) {
			Atom atom = XmInternAtom (XtDisplay (shell), "WM_DELETE_WINDOW", True);
			XmAddWMProtocols (shell, & atom, 1);
			XmAddWMProtocolCallback (shell, atom, _GuiMotifWindow_goAwayCallback, (void *) me);
		}
		GuiWindow_setTitle (shell, title);
		my widget = XmCreateForm (shell, "dialog", NULL, 0);
		_GuiObject_setUserData (my widget, me);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifWindow_destroyCallback, me);
		XtVaSetValues (my widget, XmNdialogStyle, XmDIALOG_MODELESS, XmNautoUnmanage, False, NULL);
	#endif
	return my widget;
}

void GuiWindow_show (Widget widget) {
	#if gtk
		gtk_window_present (GTK_WINDOW (GuiObject_parent (widget)));
	#elif motif
		XtManageChild (widget);
		XMapRaised (XtDisplay (GuiObject_parent (widget)), XtWindow (GuiObject_parent (widget)));
	#endif
}

void GuiWindow_setTitle (Widget shell, const wchar_t *title) {
	#if gtk
		gtk_window_set_title (GTK_WINDOW (shell), Melder_peekWcsToUtf8 (title));
	#elif mac
		SetWindowTitleWithCFString (shell -> nat.window.ptr, Melder_peekWcsToCfstring (title));
	#elif win
		SetWindowText (shell -> window, title);
	#elif motif
		XtVaSetValues (shell, XmNtitle, Melder_peekWcsToUtf8 (title), XmNiconName, Melder_peekWcsToUtf8 (title), NULL);
	#endif
}

int GuiWindow_setDirty (Widget shell, int dirty) {
	#if mac
		SetWindowModified (shell -> nat.window.ptr, dirty);
		return 1;
	#else
		(void) shell;
		(void) dirty;
		return 0;
	#endif
}

void GuiWindow_drain (Widget me) {
	#if mac
		QDFlushPortBuffer (GetWindowPort (my macWindow), NULL);
		/*
		 * The following TRICK cost me half a day to work out.
		 * It turns out that after a call to QDFlushPortBuffer (),
		 * it takes MacOS ages to compute a new dirty region while
		 * the next graphics commands are executed. Such a dirty region
		 * could well be the region that includes all the pixels drawn by
		 * the graphics commands, and nothing else. One can imagine
		 * that such a thing takes five seconds when the graphics is
		 * a simple Graphics_function () of e.g. noise.
		 */
		static Rect bounds = { -32768, -32768, 32767, 32767 };
		QDAddRectToDirtyRegion (GetWindowPort (my macWindow), & bounds);
	#else
		(void) me;
	#endif
}

/* End of file GuiWindow.c */
