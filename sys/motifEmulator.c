/* motifEmulator.c
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
 * pb 2002/03/07 GPL
 * pb 2002/10/12 external definition of availability of Appearance
 * pb 2003/11/02 Mac: if the delete response is UNMAP, call XtUnmanageChild rather than just HideWindow
 *               (makes a difference if a warning message is clicked away through the GoAway button)
 * pb 2003/11/28 Mac: set the above back to just HideWindow plus clearing the modal-dialog and active-text pointers
 * pb 2003/11/28 Win: did the same for Windows!
 *               (this solved an age old bug in Praat by which the picture window could not be closed twice)
 * pb 2004/01/01 extracted text handling to GuiText.c
 * pb 2004/02/28 MacOS X: used SetControlMaximum (32767) to work around MacOS X feature in setting popup control
 * pb 2004/11/24 separated labels from cascade buttons
 * pb 2005/09/01 assume that we have Appearance (i.e. System 8.5 or up)
 * pb 2006/08/07 Windows: remove quotes from around path names when calling the openDocument callback
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/11/06 Carbon control creation functions
 * pb 2007/01/25 XmATTACH_POSITION
 * pb 2007/02/13 Win: removed Ctrl-. as meaning Escape
 * pb 2007/08/07 GuiMacDrawingArea_clipOn_graphicsContext
 * pb 2007/10/06 wchar_t
 */
#ifndef UNIX

/* The Motif emulator for Macintosh and Windows. */

#define USE_LISTBOX_CONTROL  0

#define PRAAT_WINDOW_CLASS_NUMBER  1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "melder.h"
#include "GuiP.h"

#if win
	#define SCROLL32  1
#else
	#define SCROLL32  0
#endif

#if mac
	#include "macport_on.h"
#endif

/*
 * The MEMBER macros only work if klas1 etc are no macros themselves.
 * Therefore, we undefine those who are:
 */
#if win
	#undef MessageBox
#endif

#define _motif_SHIFT_MASK  1
#define _motif_COMMAND_MASK  2
#define _motif_OPTION_MASK  4

#if mac
	void motif_mac_defaultFont (void) {
		TextFont (systemFont);
		TextSize (12);
		TextFace (0);
	}
	static BitMap theScreenBits;
#endif

/* The following variable controls the focus policy of Mac Motif.
 * The Macintosh focus policy is: there is only one active window at a time,
 * and a click on an inactive window causes that window to become active,
 * without responding any further to that click.
 * This is appropriate for small screens, where multiple windows are usually stacked
 * on top of one another, and only a small part of background windows is visible.
 * The Motif focus policy is: all windows are active, and a click in any window causes
 * that window to respond as if it were in front.
 * This is appropriate for large screens, where multiple windows are often tiled about the screen.
 * One of the purposes of Mac Motif is to produce Macintosh look-and-feel,
 * which would require the Macintosh focus policy,
 * but as Macintosh screens are growing larger, you might prefer the faster Motif focus policy,
 * if your screen is large.
 * Therefore, you can choose:
 */
static const int BACKGROUND_WINDOWS_ARE_ACTIVE = 0;   /* 0 = Mac, 1 = Motif. */

#if win
	#define CELL_HEIGHT  15
	#define MESSAGE_BOX_BUTTON_WIDTH  100
#elif mac
	static int USE_QUESTION_MARK_HELP_MENU = 0;
	#define CELL_HEIGHT  16
	#define MESSAGE_BOX_BUTTON_WIDTH  120
#endif

#if win
	static void _motif_update (Widget me, void *event);
#elif mac
	static void _motif_update (Widget me, EventRecord *event);
#endif

/********** Resource names. **********/

#define motif_RESOURCE(xxx)  #xxx,
static const char *motif_resourceNames [] = {
	"XmNnull",
	#include "motifEmulator_resources.h"
	"XmNend"
};
#undef motif_RESOURCE

/* Modes. */

struct Gui theGui;   /* Global variable. */

/********** XWindows routines. **********/

void XMapRaised (int displayDummy, Window window) {
	(void) displayDummy;
	#if win
		ShowWindow ((HWND) window, SW_SHOW);
		SetForegroundWindow ((HWND) window);
	#elif mac
		ShowWindow ((WindowPtr) window);
		if (theGui.modalDialog && theGui.modalDialog != (WindowPtr) window) {
			SendBehind ((WindowPtr) window, theGui.modalDialog);
		} else {
			SelectWindow ((WindowPtr) window);
		}
	#endif
}

int WidthOfScreen (int screen) {
	(void) screen;
	#if win
		return GetSystemMetrics (SM_CXMAXIMIZED);
	#elif mac
		return GetQDGlobalsScreenBits (& theScreenBits) -> bounds.right;
	#endif
}
int HeightOfScreen (int screen) {
	(void) screen;
	#if win
		return GetSystemMetrics (SM_CYMAXIMIZED);
	#elif mac
		return GetQDGlobalsScreenBits (& theScreenBits) -> bounds.bottom - 22;
	#endif
}

/********** X Toolkit **********/

void _Gui_callCallbacks (Widget w, XtCallbackList *callbacks, XtPointer call) {
	int i; for (i = 0; i < MAXNUM_CALLBACKS; i ++)
		if (callbacks -> pairs [i]. proc) callbacks -> pairs [i]. proc (w, callbacks -> pairs [i]. closure, call);
}

/* Scratch. */

#if mac
	static Str255 mac_text;
#endif

/* Convenience. */

#if mac
	#define PfromCstr(p,c) p [0] = strlen (c), strcpy ((char *) p + 1, c);
#endif

/* When dispatching events to widgets, we must translate from the identifier of a Macintosh
 * screen object (Window, Control, menu item) to a Widget.
 * Mac windows and controls have RefCon fields in their WindowRecord or ControlRecord,
 * so we put a reference to the widget in the appropriate RefCon field at window or control creation time.
 * Instead of RefCons, the menus are remembered here:
 */
#if win
	#define MAXIMUM_NUMBER_OF_MENUS  4000
#elif mac
	#define MAXIMUM_NUMBER_OF_MENUS  32767
#endif
static Widget theMenus [1+MAXIMUM_NUMBER_OF_MENUS];   /* We can freely use and reuse these menu ids */
static int (*theOpenDocumentCallback) (MelderFile file);
static int (*theQuitApplicationCallback) (void);
#if win
	static int theCommandShow = False;   /* Last argument of WinMain. */
	static wchar_t theApplicationName [100], theWindowClassName [100], theDrawingAreaClassName [100], theApplicationClassName [100];
	static int (*theUserMessageCallback) (void);
	#define MINIMUM_MENU_ITEM_ID  (MAXIMUM_NUMBER_OF_MENUS + 1)
	#define MAXIMUM_MENU_ITEM_ID  32767
	static short theMenuItems [1+MAXIMUM_MENU_ITEM_ID];   /* We can freely use and reuse the item ids 4001..32767 */
#elif mac
	static Widget theMenuBar;   /* There is only one menu bar on the Macintosh. */
	static int theHelpMenuOffset;   /* The number of items in the global help menu before we add the first item. */
	static int (*theUserMessageCallbackA) (char *message);
	static int (*theUserMessageCallbackW) (wchar_t *message);
#endif

static Widget theApplicationShell;   /* For global menus. */
#define MAXIMUM_NUMBER_OF_SHELLS  1000
static Widget theShells [MAXIMUM_NUMBER_OF_SHELLS];   /* For XmUpdateDisplay and suspend events. */
static int theBackground = False;   /* Set by suspend and resume events; used by Motif-style activation methods. */
static int theDialogHint = False;   /* Should the shell that is currently being created, have dialog or document looks? */
long numberOfWidgets;

static void _motif_addShell (Widget me) {
	int i;
	for (i = 0; i < MAXIMUM_NUMBER_OF_SHELLS; i ++)
		if (theShells [i] == NULL) { theShells [i] = me; break; }
}
static void _motif_removeShell (Widget me) {
	int i;
	for (i = 0; i < MAXIMUM_NUMBER_OF_SHELLS; i ++)
		if (theShells [i] == me) { theShells [i] = NULL; break; }
}

/* For 'automatic' unmanaging. */

static void cb_unmanage (Widget me, XtPointer closure, XtPointer call) {
	(void) closure;
	(void) call;
	XtUnmanageChild (my shell);
}

static void cb_messageBox_ok (Widget okButton, XtPointer closure, XtPointer call) {
	Widget me = (Widget) closure;
	(void) okButton;
	_Gui_callCallbacks (me, & my motif.messageBox.okCallbacks, call);
	if (my autoUnmanage) XtUnmanageChild (me);
}

static void cb_messageBox_cancel (Widget cancelButton, XtPointer closure, XtPointer call) {
	Widget me = (Widget) closure;
	(void) cancelButton;
	_Gui_callCallbacks (me, & my motif.messageBox.cancelCallbacks, call);
	if (my autoUnmanage) XtUnmanageChild (me);
}

static void cb_messageBox_help (Widget helpButton, XtPointer closure, XtPointer call) {
	Widget me = (Widget) closure;
	(void) helpButton;
	_Gui_callCallbacks (me, & my motif.messageBox.helpCallbacks, call);
}

/* AppContext level */

static int theNumberOfWorkProcs;
static XtWorkProc theWorkProcs [10];
static XtPointer theWorkProcClosures [10];

static int theNumberOfTimeOuts;
static XtTimerCallbackProc theTimeOutProcs [10];
static XtPointer theTimeOutClosures [10];
#if defined (macintosh)
	static EventLoopTimerRef theTimers [10];
	static EventTargetRef theUserFocusEventTarget;
#else
	static clock_t theTimeOutStarts [10];
	static unsigned long theTimeOutIntervals [10];
#endif

static void Native_move (Widget w, int dx, int dy);   /* Forward. */

static void cb_scroll (Widget scrollBar, XtPointer closure, XtPointer call) {
	Widget scrolledWindow = (Widget) closure;
	Widget workWindow = scrolledWindow -> motif.scrolledWindow.workWindow;
	int previousShift, newShift, distance;
	int horizontal = scrolledWindow -> motif.scrolledWindow.horizontalBar == scrollBar;
	(void) call;
	if (! workWindow) return;
	Melder_assert (scrolledWindow -> motif.scrolledWindow.clipWindow != NULL);
	#if win
		previousShift = horizontal ?
			scrolledWindow -> motif.scrolledWindow.clipWindow -> x - workWindow -> x :
			scrolledWindow -> motif.scrolledWindow.clipWindow -> y - workWindow -> y;
	#elif mac
		previousShift = horizontal ?
			scrolledWindow -> motif.scrolledWindow.clipWindow -> rect. left - workWindow -> rect. left :
			scrolledWindow -> motif.scrolledWindow.clipWindow -> rect. top - workWindow -> rect. top;   /* different? */
	#endif
	newShift = scrollBar -> value;
	distance = previousShift - newShift;
	if (horizontal)
		{ workWindow -> x += distance; Native_move (workWindow, distance, 0); }
	else
		{ workWindow -> y += distance; Native_move (workWindow, 0, distance); }
	XmUpdateDisplay (NULL);   /* If dragging. */
}

/* These are like the toolkit's query_geometry methods: */

static int Native_titleWidth (Widget me) {
	#if win
		if (my parent -> window) {
			HDC dc = GetDC (my parent -> window);
			SIZE size;
			SelectFont (dc, GetStockFont (ANSI_VAR_FONT));   /* Possible BUG. */
			GetTextExtentPoint32 (dc, my name, wcslen (my name), & size);
			ReleaseDC (my parent -> window, dc);
			return size. cx;
		} else return 7 * wcslen (my name);
	#elif mac
		int width;
		SetPortWindowPort (my macWindow);
		motif_mac_defaultFont ();
		char *nameA = Melder_peekWcsToUtf8 (my name);
		width = TextWidth ((Ptr) nameA, 0, strlen (nameA));
		motif_mac_defaultFont ();
		return width;
	#endif
}

static int NativeLabel_preferredWidth (Widget me) {
	return Native_titleWidth (me) + 10;
}

static int NativeLabel_preferredHeight (Widget me) {
	(void) me;
	return win ? 17 : 18;
}

static int NativeButton_preferredWidth (Widget me) {
	int width = Native_titleWidth (me) + ( win ? 10 : ( my parent -> rowColumnType == XmMENU_BAR ? 10 : 28 ) );
	return width < 41 ? 41 : width;
}

static int NativeToggleButton_preferredWidth (Widget me) {
	return Native_titleWidth (me) + 25;
}

static int NativeToggleButton_preferredHeight (Widget me) {
	(void) me;
	return win ? 17 : 20;
}

static int NativeButton_preferredHeight (Widget me) {
	(void) me;
	return win ? 22 : ( my parent -> rowColumnType == XmMENU_BAR ? 26 : 20 );
}

static int NativeText_preferredHeight (Widget me) {
	(void) me;
	return win ? 17 : 22;
}

/***** WIDGET *****/

Widget _Gui_initializeWidget (int widgetClass, Widget parent, const wchar_t *name) {
	Widget me = Melder_calloc (struct structWidget, 1);
	my magicNumber = 15111959;
	numberOfWidgets ++;
	my widgetClass = widgetClass;
	my parent = parent;
	my inMenu = parent && MEMBER (parent, PulldownMenu);

	/*
	 * Install me into my parent's list of children.
	 */
	if (parent == NULL) {
		;
	} else if (parent -> firstChild == NULL) {
		parent -> firstChild = me;
	} else {
		Widget lastChild = parent -> firstChild;
		while (lastChild -> nextSibling) lastChild = lastChild -> nextSibling;
		lastChild -> nextSibling = me;
		my previousSibling = lastChild;
	}

	/*
	 * Copy the name into my name.
	 */
	my name = Melder_wcsdup (name);

	/*
	 * I am in the same shell as my parent, so I inherit my parent's "shell" attribute.
	 */
	if (MEMBER (me, Shell)) {
		my shell = me;
	} else {
		my shell = my parent -> shell;
		#if mac
			/*
			 * I am in the same shell as my parent, so I'll inherit my parent's Macintosh WindowRef.
			 */
			my macWindow = parent -> macWindow;
		#endif
	}

	/*
	 * The remainder of initialization is about positioning, sizes, attachments, and the contents of a scrolled window.
	 * All of that is irrelevant to menu items.
	 */
	if (my inMenu) return me;

	/* Initial defaults: mainly positioning and sizes. */

	switch (my widgetClass) {
		case xmDrawingAreaWidgetClass: {
			#if win
				my x = 2;
				my y = 2;
				my width = 100;
				my height = 100;
			#endif
		} break; case xmShellWidgetClass: {
			#if win
				my x = 20;
				my y = 3;
				my width = 30;
				my height = 50;
			#elif mac
				my x = 20;
				my y = 30;
				my width = 10;
				my height = 10;
			#endif
			my deleteResponse = XmDESTROY;
		} break; case xmTextWidgetClass: {
			my x = 2;
			my y = 2;
			my width = 102;
			my height = NativeText_preferredHeight (me);
		} break; case xmPushButtonWidgetClass: {
			my x = 2;
			my y = 2;
			my width = NativeButton_preferredWidth (me);
			my height = NativeButton_preferredHeight (me);
		} break; case xmLabelWidgetClass: {
			my x = 2;
			my y = 2;
			my width = NativeLabel_preferredWidth (me);
			my height = NativeLabel_preferredHeight (me);
			my alignment = XmALIGNMENT_BEGINNING;
		} break; case xmCascadeButtonWidgetClass: {
			if (my parent -> rowColumnType == XmMENU_BAR) {
				wchar_t *hyphen = wcsstr (my name, L" -");
				if (hyphen) hyphen [2] = '\0';   /* Chop any trailing spaces. */
				my x = 2;
				my y = 2;
				my width = NativeButton_preferredWidth (me);
				my height = NativeButton_preferredHeight (me) + 4;   /* BUG: menu bar should be large enough. */
			} else {
				my motif.cascadeButton.inBar = TRUE;
			}
			my alignment = XmALIGNMENT_BEGINNING;
		} break; case xmToggleButtonWidgetClass: {
			my x = 2;
			my y = 2;
			my width = NativeToggleButton_preferredWidth (me);
			my height = NativeToggleButton_preferredHeight (me);
		} break; case xmSeparatorWidgetClass: {
			my width = parent -> width;
			my height = 10;
		} break; case xmScrollBarWidgetClass: {
			my width = 16;
			my height = 100;
		} break; case xmMenuBarWidgetClass: {
			my width = 10;
			my height = 10;
		} break; case xmRowColumnWidgetClass: {
			my width = 10;
			my height = 10;
			my orientation = XmVERTICAL;
		} break; case xmScaleWidgetClass: {
			my width = 300;
			my height = win ? 25 : 40;
		} break; case xmFormWidgetClass: {
			if (MEMBER (parent, Shell)) {
				/*
				 * The following trick is necessary for forms that contain scroll bars.
				 */
				my width = parent -> width;
				my height = parent -> height;
			} else {
				/*
				 * EXPERIMENT:
				 * this relies on a certain sequence in building up a window: from top to bottom.
				 * If the form is inside a RowColumn of a certain width established by a widget
				 * closer to the top, this width will be copied. The height, of course, should not be copied.
				 * See Praat's SoundRecorder for an example.
				 */
				my width = parent -> width;
				my height = 10;
			}
		} break; default: {
			my width = parent -> width;
			my height = parent -> height;
		}
	}

	/* Some positions depend on parent. */

	if (parent) switch (parent -> widgetClass) {
		case xmFrameWidgetClass: {
			my x = 1;
			my y = 1;
			my width = parent -> width - 2;
			my height = parent -> height - 2;
		} break; case xmScrolledWindowWidgetClass: {
			my x = 1;
			my y = 1;
			my width = parent -> width - 17;   /* Exact fit: scroll bar (16) plus border (1). */
			my height = parent -> height - 17;
			if (my widgetClass == xmTextWidgetClass) { my width = 3000; my height = 30000; }
		}
	}

	if (my width < 0) my width = 0;
	if (my height < 0) my height = 0;

	#if mac
		/* Determine enclosing rectangle in macwindow co-ordinates. */
		if (! MEMBER (me, Shell) && ! MEMBER (my parent, Shell)) {
			my rect.left = parent -> rect.left + my x;
			my rect.top = parent -> rect.top + my y;
		}
		my rect.right = my rect.left + my width;
		my rect.bottom = my rect.top + my height;
	#endif

	/* Automatic attachment of dialog to parent shell. */

	if (MEMBER3 (me, BulletinBoard, Form, MessageBox) && MEMBER (my parent, Shell))
		my leftAttachment = my rightAttachment = my topAttachment = my bottomAttachment = XmATTACH_FORM;

	if (MEMBER (me, CascadeButton) && wcsequ (name, L"Help"))
		my rightAttachment = XmATTACH_FORM;   /* !!!!!! */

	/* A child of a scrolled window will be installed as the workWindow of that scrolled window,
	 * except if it is a scroll bar or if the clipWindow does not exist yet.
	 * This is because the creations of the scroll bars and the clip window will also arrive here.
	 * Our XmScrolledWindow creation method always creates two scroll bars and a clip window,
	 * before you can create any other children.
	 */

	if (my parent && MEMBER (my parent, ScrolledWindow) &&
		! MEMBER (me, ScrollBar) &&   /* 'me' is one of the two scroll bars, or a new one. */
		my parent -> motif.scrolledWindow.clipWindow != NULL)   /* 'me' is probably the clip window now. */
			my parent -> motif.scrolledWindow.workWindow = me;   /* Install. */
	return me;
}

/********** CLIPPING AND VIEW VALIDATION ROUTINES **********/

#if mac
	/* These routines use Mac position information only (i.e., the 'rect' attribute).
	 * They do not use Motif position information (i.e., the 'x', 'y', 'width', and 'height' attributes.
	 */

	/* The largest possible rect on a Mac, used for "no clip". */
	static Rect _motif_wideRect = { -32768, -32768, 32767, 32767 };
	static Rect _motif_clipRect;

	void _GuiMac_clipOn (Widget me) {
		/* The 'clipRect' will be the intersection of the rects of all its ancestors,
		 * stopping before a shell and making side steps at scrolled windows.
		 */
		Widget parent;
		Rect clipRect = _motif_wideRect;
		for (parent = my parent; ! MEMBER (parent, Shell); parent = parent -> parent) {
			Rect *parentRect = & parent -> rect;
			if (MEMBER (parent, ScrolledWindow)) {
				if (MEMBER (me, ScrollBar)) {
					parentRect = & my rect;
				} else {
					if (parent -> motif.scrolledWindow.clipWindow == NULL) return;   /* During destruction. */
					parentRect = & parent -> motif.scrolledWindow.clipWindow -> rect;
				}
			}
			if (parentRect -> left > clipRect. left) clipRect. left = parentRect -> left;
			if (parentRect -> right < clipRect. right) clipRect. right = parentRect -> right;
			if (parentRect -> top > clipRect. top) clipRect. top = parentRect -> top;
			if (parentRect -> bottom < clipRect. bottom) clipRect. bottom = parentRect -> bottom;
		}
		/*if (MEMBER (me, PushButton) && me == my shell -> defaultButton)
			clipRect. left -= 3, clipRect. right += 3, clipRect. top -= 3, clipRect. bottom += 3;*/
		SetPortWindowPort (my macWindow);
		ClipRect (& clipRect);
		_motif_clipRect = clipRect;
	}

	void GuiMac_clipOff (void) { ClipRect (& _motif_wideRect); }

	void _GuiMac_clipOffValid (Widget me) {
		Rect clipRect = _motif_clipRect;
		if (my rect. left > clipRect. left) clipRect. left = my rect. left;
		if (my rect. right < clipRect. right) clipRect. right = my rect. right;
		if (my rect. top > clipRect. top) clipRect. top = my rect. top;
		if (my rect. bottom < clipRect. bottom) clipRect. bottom = my rect. bottom;
		SetPortWindowPort (my macWindow);
		ValidWindowRect (my macWindow, & clipRect);
		ClipRect (& _motif_wideRect);
	}

	static void _motif_clipOffInvalid (Widget me) {
		Rect clipRect = _motif_clipRect;
		if (my rect. left > clipRect. left) clipRect. left = my rect. left;
		if (my rect. right < clipRect. right) clipRect. right = my rect. right;
		if (my rect. top > clipRect. top) clipRect. top = my rect. top;
		if (my rect. bottom < clipRect. bottom) clipRect. bottom = my rect. bottom;
		SetPortWindowPort (my macWindow);
		InvalWindowRect (my macWindow, & clipRect);
		ClipRect (& _motif_wideRect);
	}

	void GuiMacDrawingArea_clipOn (Widget me) {
		Widget parent;
		Rect clipRect = my rect;
		Melder_assert (my widgetClass == xmDrawingAreaWidgetClass);
		/* InsetRect (& clipRect, my marginWidth, my marginHeight); */
		for (parent = my parent; ! MEMBER (parent, Shell); parent = parent -> parent) {
			Rect *parentRect = & parent -> rect;
			if (MEMBER (parent, ScrolledWindow)) {
				if (MEMBER (me, ScrollBar)) {
					parentRect = & my rect;
				} else {
					Melder_assert (parent -> motif.scrolledWindow.clipWindow != NULL);
					parentRect = & parent -> motif.scrolledWindow.clipWindow -> rect;
				}
			}
			if (parentRect -> left > clipRect. left) clipRect. left = parentRect -> left;
			if (parentRect -> right < clipRect. right) clipRect. right = parentRect -> right;
			if (parentRect -> top > clipRect. top) clipRect. top = parentRect -> top;
			if (parentRect -> bottom < clipRect. bottom) clipRect. bottom = parentRect -> bottom;
		}
		SetPortWindowPort (my macWindow);
		ClipRect (& clipRect);
	}
	int GuiMacDrawingArea_clipOn_graphicsContext (Widget me, void *graphicsContext) {
		Widget parent;
		Rect clipRect = my rect;
		//Melder_assert (my widgetClass == xmDrawingAreaWidgetClass);
		/* InsetRect (& clipRect, my marginWidth, my marginHeight); */
		for (parent = my parent; ! MEMBER (parent, Shell); parent = parent -> parent) {
			Rect *parentRect = & parent -> rect;
			if (MEMBER (parent, ScrolledWindow)) {
				if (MEMBER (me, ScrollBar)) {
					parentRect = & my rect;
				} else {
					Melder_assert (parent -> motif.scrolledWindow.clipWindow != NULL);
					parentRect = & parent -> motif.scrolledWindow.clipWindow -> rect;
				}
			}
			if (parentRect -> left > clipRect. left) clipRect. left = parentRect -> left;
			if (parentRect -> right < clipRect. right) clipRect. right = parentRect -> right;
			if (parentRect -> top > clipRect. top) clipRect. top = parentRect -> top;
			if (parentRect -> bottom < clipRect. bottom) clipRect. bottom = parentRect -> bottom;
		}
		CGContextClipToRect (graphicsContext, CGRectMake (clipRect.left, my shell -> height - clipRect.bottom,
			clipRect.right - clipRect.left, clipRect.bottom - clipRect.top));
		return my shell -> height;
	}
#endif

/***** NATIVE *****/

void _GuiNativeControl_check (Widget me, Boolean value) {
	#if win
		Button_SetCheck (my window, value ? BST_CHECKED : BST_UNCHECKED);
	#elif mac
		SetControlValue (my nat.control.handle, value);
	#endif
}

void _GuiNativeControl_destroy (Widget me) {
	#if win
		DestroyWindow (my window);
	#elif mac
		_GuiMac_clipOn (me);
		DisposeControl (my nat.control.handle);
		GuiMac_clipOff ();
	#endif
}

void _GuiNativeControl_show (Widget me) {
	#if win
		ShowWindow (my window, SW_SHOW);
	#elif mac
		_GuiMac_clipOn (me);
		if (IsControlVisible (my nat.control.handle))
			Draw1Control (my nat.control.handle);
		else
			ShowControl (my nat.control.handle);
		_GuiMac_clipOffValid (me);
	#endif
}

void _GuiNativeControl_hide (Widget me) {
	#if win
		ShowWindow (my window, SW_HIDE);
	#elif mac
		_GuiMac_clipOn (me);
		HideControl (my nat.control.handle);
		_GuiMac_clipOffValid (me);
	#endif
}

void _GuiNativeControl_setSensitive (Widget me) {
	#if win
		EnableWindow (my window, ! my insensitive);
	#elif mac
		_GuiMac_clipOn (me);
		HiliteControl (my nat.control.handle, my insensitive ? 255 : 0);
		GuiMac_clipOff ();
	#endif
}

static wchar_t * motif_win_expandAmpersands (const wchar_t *title) {
	static wchar_t buffer [300];
	const wchar_t *from = title;
	wchar_t *to = & buffer [0];
	while (*from) { if (*from == '&') * to ++ = '&'; * to ++ = * from ++; } * to = '\0';
	return buffer;
}

static void NativeControl_setFont (Widget me, int size) {
	#if win
	#elif mac
		ControlFontStyleRec fontStyle;
		fontStyle. flags = kControlUseFontMask | kControlUseSizeMask | kControlUseJustMask;
		fontStyle. font = systemFont;
		fontStyle. size = size;
		fontStyle. just = my alignment == XmALIGNMENT_END ? teFlushRight :
			my alignment == XmALIGNMENT_CENTER ? teCenter : teFlushLeft;
		SetControlFontStyle (my nat.control.handle, & fontStyle);
	#endif
}

static void NativeControl_setTitle (Widget me) {
	#if win
		HDC dc = GetDC (my window);
		SelectPen (dc, GetStockPen (NULL_PEN));
		SelectBrush (dc, GetStockBrush (LTGRAY_BRUSH));
		Rectangle (dc, 0, 0, my width, my height);
		ReleaseDC (my window, dc);
		SetWindowText (my window, motif_win_expandAmpersands (my name));
	#elif mac
		Melder_assert (my nat.control.handle);
		if (my widgetClass == xmLabelWidgetClass) {
			/*
			 * Static Text controls on the Mac do not understand SetControlTitle.
			 */
			CFStringRef cfString = Melder_peekWcsToCfstring (my name);
			SetControlData (my nat.control.handle, kControlEntireControl, kControlStaticTextCFStringTag, sizeof (CFStringRef), & cfString);
			/* SetControlData does not redraw the control. */
			if (IsControlVisible (my nat.control.handle))
				Draw1Control (my nat.control.handle);
		} else if (my nat.control.isPopup) {
			Widget menu = my subMenuId, item;
			if (menu) for (item = menu -> firstChild; item; item = item -> nextSibling) {
				if (wcsequ (item -> name, my name)) {
					SetControlValue (my nat.control.handle, item -> nat.entry.item);
					return;
				}
			}
			SetControlValue (my nat.control.handle, 0);
		} else {
			SetControlTitleWithCFString (my nat.control.handle, Melder_peekWcsToCfstring (my name));
		}
	#endif
}

static int _XmScrollBar_check (Widget me) {
	if (my maximum < my minimum)
		Melder_warning ("XmScrollBar: maximum (%d) less than minimum (%d).", my maximum, my minimum);
	else if (my sliderSize > my maximum - my minimum)
		Melder_warning ("XmScrollBar: slider size (%d) greater than maximum (%d) minus minimum (%d).",
			my sliderSize, my maximum, my minimum);
	else if (my value < my minimum)
		Melder_warning ("XmScrollBar: value (%d) less than minimum (%d).", my value, my minimum);
	else if (my value > my maximum - my sliderSize)
		Melder_warning ("XmScrollBar: value (%d) greater than maximum (%d) minus slider size (%d).",
			my value, my maximum, my sliderSize);
	else return 1;
	return 0;
}

static void NativeScrollBar_set (Widget me) {
	if (! _XmScrollBar_check (me)) return;
	#if win
	{
		SCROLLINFO scrollInfo;
		scrollInfo. cbSize = sizeof (SCROLLINFO);
		scrollInfo. fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
		#if SCROLL32
			if (my maximum == my minimum + my sliderSize) {
				scrollInfo. nMin = 0;
				scrollInfo. nMax = 1;
				scrollInfo. nPage = 1;
				scrollInfo. nPos = 0;
			} else {
				scrollInfo. nMin = 0;
				scrollInfo. nMax = 32767;
				scrollInfo. nPage = (32767.0 * my sliderSize) / (my maximum - my minimum);
				scrollInfo. nPos = ((32767.0 - scrollInfo. nPage) * (my value - my minimum)) / (my maximum - my minimum - my sliderSize);
			}
		#else
			scrollInfo. nMin = my minimum;
			scrollInfo. nMax = my maximum;
			scrollInfo. nPage = my sliderSize;
			scrollInfo. nPos = my value;
		#endif
		if (my window) SetScrollInfo (my window, SB_CTL, & scrollInfo, TRUE);
	}
	#elif mac
		if (! my nat.control.handle) return;
		SetControl32BitMinimum (my nat.control.handle, my minimum);
		SetControl32BitMaximum (my nat.control.handle, my maximum - my sliderSize);
		SetControlViewSize (my nat.control.handle, my sliderSize);
		SetControl32BitValue (my nat.control.handle, my value);
	#endif
}

static void NativeMenuItem_delete (Widget me) {
	#if win
		RemoveMenu (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND);
	#elif mac
		Widget subview;
		DeleteMenuItem (my nat.entry.handle, my nat.entry.item);
		for (subview = my parent -> firstChild; subview; subview = subview -> nextSibling) {
			if (subview -> managed && ! MEMBER (subview, PulldownMenu) &&
				subview -> nat.entry.item > my nat.entry.item)
					subview -> nat.entry.item -= 1;
		}
		my nat.entry.item = 0;
	#endif
}

static int NativeMenuItem_getPosition (Widget me) {
	int position = 1;
	#if win
		/*
		 * The following routine could also be used for Mac.
		 */
		Widget sibling;
		for (sibling = my parent -> firstChild; sibling; sibling = sibling -> nextSibling) {
			if (sibling == me) break;
			if (sibling -> managed && ! MEMBER (sibling, PulldownMenu))
				position += 1;
		}
		/*
		 * Bill Gates counts like 0, 1, 2...
		 */
		position -= 1;
	#elif mac
		/*
		 * A Macintosh menu item shall know its "item number",
		 * which is its dynamic position in the current shape of the menu.
		 * We can use this item number to make changes and
		 * it will be sent to us by MenuSelect () while we handle a mouse-down message in the menu.
		 * (Regrettably, this item number has to change whenever items are unmanaged
		 *  or destroyed elsewhere in the menu.)
		 * Basically, the item number is one higher than the item number of the previous sibling.
		 *
		 * "incremental" means that all previous sibling items must have valid item numbers.
		 */
		Widget prev = my previousSibling;
		/*
		 * However, there may also be sibling pulldown menus. These have no item number (the associated cascade buttons have).
		 * Also, unmanaged items are not in the native menu structure, so these have no item number either.
		 */
		while (prev && (MEMBER (prev, PulldownMenu) || ! prev -> managed))
			prev = prev -> previousSibling;
		if (prev) {   /* Is there a previous managed non-pulldown sibling? */
			position = prev -> nat.entry.item + 1;   /* Then I must be the next item in the native menu structure. */
		} else {
			/*
			 * I must be the first item in the native menu structure, as far as the application is concerned.
			 * However, if the menu is the system's help menu (the question mark in System 7),
			 * the menu already contains a button called "Show balloon help".
			 */
			if (my parent -> nat.menu.id == kHMHelpMenuID) {   /* Is this the system help menu? */
				if (theHelpMenuOffset == 0)   /* Haven't we asked yet for the length of the system help menu? */
					theHelpMenuOffset = CountMenuItems (my nat.entry.handle);   /* So ask now. */
				position = theHelpMenuOffset + 1;   /* Then I must be the next item in the native menu structure. */
			} else {   /* This must be a menu of our own. */
				position = 1;   /* I must be the first item in the native menu structure. */
			}
		}
	#endif
	return position;
}

static void NativeMenuItem_check (Widget me, Boolean value) {
	#if win
		if (! my managed) return;
		CheckMenuItem (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND | ( value ? MF_CHECKED : MF_UNCHECKED ));
	#elif mac
		if (! my nat.entry.item) return;
		CheckMenuItem (my nat.entry.handle, my nat.entry.item, value);
	#endif
}

static void NativeMenuItem_setSensitive (Widget me) {
	#if win
		if (! my managed) return;
		EnableMenuItem (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND | ( my insensitive ? MF_GRAYED : MF_ENABLED ));
	#elif mac
		if (! my nat.entry.item) return;
		if (my insensitive) DisableMenuItem (my nat.entry.handle, my nat.entry.item);
		else EnableMenuItem (my nat.entry.handle, my nat.entry.item);
	#endif
}

static void NativeMenuItem_setText (Widget me) {
	int acc = my motif.pushButton.acceleratorChar, modifiers = my motif.pushButton.acceleratorModifiers;
	#if win
		static MelderString title = { 0 };
		if (acc == 0) {
			MelderString_copy (& title, motif_win_expandAmpersands (my name));
		} else {
			static const wchar_t *keyStrings [256] = {
				0, L"<-", L"->", L"Up", L"Down", L"PAUSE", L"Del", L"Ins", L"Backspace", L"Tab", L"LineFeed", L"Home", L"End", L"Enter", L"PageUp", L"PageDown",
				L"Esc", L"F1", L"F2", L"F3", L"F4", L"F5", L"F6", L"F7", L"F8", L"F9", L"F10", L"F11", L"F12", 0, 0, 0,
				L"Space", L"!", L"\"", L"#", L"$", L"%", L"&", L"\'", L"(", L")", L"*", L"+", L",", L"-", L".", L"/",
				L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L":", L";", L"<", L"=", L">", L"?",
				L"@", L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O",
				L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z", L"[", L"\\", L"]", L"^", L"_",
				L"`", L"a", L"b", L"c", L"d", L"e", L"f", L"g", L"h", L"i", L"j", L"k", L"l", L"m", L"n", L"o",
				L"p", L"q", L"r", L"s", L"t", L"u", L"v", L"w", L"x", L"y", L"z", L"{", L"|", L"}", L"~", L"Del",
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"[", L"]", L",", L"?", L".", L"\\",
				L";", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"-", L"`", L"=", L"\'", 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			const wchar_t *keyString = keyStrings [acc] ? keyStrings [acc] : L"???";
			MelderString_empty (& title);
			MelderString_append6 (&title, motif_win_expandAmpersands (my name), L"\t",
				modifiers & _motif_COMMAND_MASK ? L"Ctrl-" : NULL,
				modifiers & _motif_OPTION_MASK ? L"Alt-" : NULL,
				modifiers & _motif_SHIFT_MASK ? L"Shift-" : NULL, keyString);
		}
		ModifyMenu (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND | MF_STRING, my nat.entry.id, title.string);
	#elif mac
		static int theGlyphs [1+31] = { 0,
			kMenuLeftArrowDashedGlyph, kMenuRightArrowDashedGlyph, kMenuUpArrowDashedGlyph, kMenuDownwardArrowDashedGlyph, 0,
			kMenuDeleteRightGlyph, 0, kMenuDeleteLeftGlyph, kMenuTabRightGlyph, 0,
			0, 0, kMenuReturnGlyph, kMenuPageUpGlyph, kMenuPageDownGlyph,
			kMenuEscapeGlyph, kMenuF1Glyph, kMenuF2Glyph, kMenuF3Glyph, kMenuF4Glyph,
			kMenuF5Glyph, kMenuF6Glyph, kMenuF7Glyph, kMenuF8Glyph, kMenuF9Glyph,
			kMenuF10Glyph, kMenuF11Glyph, kMenuF12Glyph, 0, 0,
			0 };
		SetMenuItemTextWithCFString (my nat.entry.handle, my nat.entry.item, Melder_peekWcsToCfstring (my name));
		if (acc > 32) {
			SetItemCmd (my nat.entry.handle, my nat.entry.item, acc);
		} else {
			Melder_assert (acc > 0 && acc < 32);
			SetItemCmd (my nat.entry.handle, my nat.entry.item, ' ');   /* Funny that this should be needed. */
			SetMenuItemKeyGlyph (my nat.entry.handle, my nat.entry.item, theGlyphs [acc]);
		}
		SetMenuItemModifiers (my nat.entry.handle, my nat.entry.item,
			( modifiers & _motif_OPTION_MASK ? kMenuOptionModifier : 0 ) +
			( modifiers & _motif_SHIFT_MASK ? kMenuShiftModifier : 0 ) +
			( modifiers & _motif_COMMAND_MASK ? 0 : kMenuNoCommandModifier ));
	#endif
}

/********** **********/

/*
 * We now create the native objects associated with this widget,
 * but do not show them on the screen yet (ideally).
 * A reference must be made from widget to native object and back.
 * On Mac, we normally use the RefCon fields of the windows and controls.
 * On Win, we use SetWindowLong (window, GWL_USERDATA, widget).
 */

#if mac
static void listDefinition (short message, Boolean select, Rect *rect, Cell cell, short dataOffset, short dataLength, ListHandle handle) {
	Widget me = (Widget) GetListRefCon (handle);
	switch (message) {
		case lDrawMsg:
		case lHiliteMsg:
			Melder_assert (me != NULL);
			//Melder_fatal ("rect %d %d %d %d", rect->top, rect->bottom, rect->left, rect->right);
			SetPortWindowPort (my macWindow);
			_GuiMac_clipOn (me);
			static RGBColor whiteColour = { 0xFFFF, 0xFFFF, 0xFFFF }, blackColour = { 0, 0, 0 };
			RGBForeColor (& whiteColour);
			PaintRect (rect);
			RGBForeColor (& blackColour);
			if (select) {
				LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);
				InvertRect (rect);
			}
			CGContextRef macGraphicsContext;
			QDBeginCGContext (GetWindowPort (my macWindow), & macGraphicsContext);
			int shellHeight = GuiMacDrawingArea_clipOn_graphicsContext (me, macGraphicsContext);
			static ATSUFontFallbacks fontFallbacks = NULL;
			if (fontFallbacks == NULL) {
				ATSUCreateFontFallbacks (& fontFallbacks);
				ATSUSetObjFontFallbacks (fontFallbacks, 0, NULL, kATSUDefaultFontFallbacks);
			}
			char *text_utf8 = (char *) *((*handle) -> cells) + dataOffset;
			strncpy (Melder_buffer1, text_utf8, dataLength);
			Melder_buffer1 [dataLength] = '\0';
			wchar_t *text_wcs = Melder_peekUtf8ToWcs (Melder_buffer1);
			const MelderUtf16 *text_utf16 = Melder_peekWcsToUtf16 (text_wcs);
			UniCharCount runLength = wcslen (text_wcs);   // BUG
			ATSUTextLayout textLayout;
			ATSUStyle style;
			ATSUCreateStyle (& style);
			Fixed fontSize = 12 << 16;
			Boolean boldStyle = 0;
			Boolean italicStyle = 0;
			ATSUAttributeTag styleAttributeTags [] = { kATSUSizeTag, kATSUQDBoldfaceTag, kATSUQDItalicTag };
			ByteCount styleValueSizes [] = { sizeof (Fixed), sizeof (Boolean), sizeof (Boolean) };
			ATSUAttributeValuePtr styleValues [] = { & fontSize, & boldStyle, & italicStyle };
			ATSUSetAttributes (style, 3, styleAttributeTags, styleValueSizes, styleValues);
			OSStatus err = ATSUCreateTextLayoutWithTextPtr (text_utf16, kATSUFromTextBeginning, kATSUToTextEnd, runLength,
				1, & runLength, & style, & textLayout);
			Melder_assert (err == 0);
			ATSUAttributeTag attributeTags [] = { kATSUCGContextTag, kATSULineFontFallbacksTag };
			ByteCount valueSizes [] = { sizeof (CGContextRef), sizeof (ATSUFontFallbacks) };
			ATSUAttributeValuePtr values [] = { & macGraphicsContext, & fontFallbacks };
			ATSUSetLayoutControls (textLayout, 2, attributeTags, valueSizes, values);
			ATSUSetTransientFontMatching (textLayout, true);
			CGContextTranslateCTM (macGraphicsContext, rect -> left, shellHeight - rect -> bottom + 4);
			err = ATSUDrawText (textLayout, kATSUFromTextBeginning, kATSUToTextEnd, 0 /*xDC << 16*/, 0 /*(shellHeight - yDC) << 16*/);
			Melder_assert (err == 0);
			CGContextSynchronize (macGraphicsContext);
			ATSUDisposeTextLayout (textLayout);
			ATSUDisposeStyle (style);
			QDEndCGContext (GetWindowPort (my macWindow), & macGraphicsContext);
			GuiMac_clipOff ();
			break;
/*		case lHiliteMsg:
			Melder_assert (me != NULL);
			SetPortWindowPort (my macWindow);
			_GuiMac_clipOn (me);
			LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);
			InvertRect (rect);
			GuiMac_clipOff ();
			break;*/
	}
}
#endif

static void _GuiNativizeWidget (Widget me) {
	if (my nativized) return;
	if (my inMenu) {
		if (MEMBER (me, PulldownMenu)) {
			#if win
				int id;
				for (id = 1; id <= MAXIMUM_NUMBER_OF_MENUS; id ++) if (! theMenus [id]) break;
				my nat.menu.id = id;
				theMenus [my nat.menu.id] = me;   /* Instead of UserData fields. */
				/*
				 * This will be a hierarchical menu.
				 */
				my nat.menu.handle = CreatePopupMenu ();
			#elif mac
				int id;
				for (id = 1; id <= MAXIMUM_NUMBER_OF_MENUS; id ++) if (! theMenus [id]) break;
				my nat.menu.id = id;
				theMenus [my nat.menu.id] = me;   /* Instead of RefCon fields. */
				/*
				 * This will be a hierarchical menu.
				 */
				CreateNewMenu (my nat.menu.id, 0, & my nat.menu.handle);
				SetMenuTitleWithCFString (my nat.menu.handle, Melder_peekWcsToCfstring (my name));
				InsertMenu (my nat.menu.handle, kInsertHierarchicalMenu);
			#endif
		} else {
			/*
			 * Any menu item (push button, toggle button, or cascade button) shall contain its native parent menu handle.
			 */
			my nat.entry.handle = my parent -> nat.menu.handle;
			#if win
			{
				/*
				 * A Windows menu item shall have a shell-unique ID,
				 * which we can use to make changes and
				 * which will be sent to us by the WM_COMMAND message.
				 * This ID should be higher than 4000, in order to be different from the menu IDs.
				 * In our implementation, item IDs are application-unique.
				 */
				int id;
				for (id = MINIMUM_MENU_ITEM_ID; id <= MAXIMUM_MENU_ITEM_ID; id ++) if (! theMenuItems [id]) break;
				my nat.entry.id = id;   // install unique ID
				theMenuItems [id] = TRUE;
			}
			#endif
		}
	} else switch (my widgetClass) {
		case xmBulletinBoardWidgetClass: {
			#if win
				my window = CreateWindowEx (0, theWindowClassName, L"bulletinBoard", WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
			#endif
		} break;
		case xmDrawingAreaWidgetClass: {
			#if win
				my window = CreateWindowEx (0, theDrawingAreaClassName, L"drawingArea", WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
			#endif
		} break;
		case xmFormWidgetClass: {
			#if win
				my window = CreateWindowEx (0, theWindowClassName, L"form", WS_CHILD | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
			#endif
		} break;
		case xmRowColumnWidgetClass: {
			#if win
				my window = CreateWindowEx (0, theWindowClassName, L"rowColumn", WS_CHILD | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
			#endif
		} break;
		case xmListWidgetClass: {
			#if win
				my window = CreateWindowEx (0, L"listbox", L"list",
					WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | WS_CLIPSIBLINGS |
					( my selectionPolicy == XmSINGLE_SELECT || my selectionPolicy == XmBROWSE_SELECT ? 0 : LBS_EXTENDEDSEL ),
					my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
				SetWindowFont (my window, GetStockFont (ANSI_VAR_FONT), FALSE);
				/*if (MEMBER (my parent, ScrolledWindow)) {
					XtDestroyWidget (my parent -> motif.scrolledWindow.horizontalBar);
					my parent -> motif.scrolledWindow.horizontalBar = NULL;
					XtDestroyWidget (my parent -> motif.scrolledWindow.verticalBar);
					my parent -> motif.scrolledWindow.verticalBar = NULL;
				}*/
			#elif mac
				#if USE_LISTBOX_CONTROL
					CreateListBoxControl (my macWindow, & my rect, false, 1000, 1, true, true, CELL_HEIGHT, 400, false, NULL, & my nat.control.handle);
					GetControlData (my nat.control.handle, kControlEntireControl, kControlListBoxListHandleTag,
						sizeof (my nat.list.handle), & my nat.list.handle, NULL);
					SetControlReference (my nat.control.handle, (long) me);
					my isControl = TRUE;
					NativeControl_setFont (me, 12);
				#else
					Rect dataBounds = { 0, 0, 0, 1 };
					Point cSize;
					SetPt (& cSize, my rect.right - my rect.left + 1, CELL_HEIGHT);
					#if 0
					my nat.list.handle = LNew (& my rect, & dataBounds, cSize, 0,
						my macWindow, false, false, false, false);
					#else
					static ListDefSpec listDefSpec;
					if (listDefSpec. u. userProc == NULL) {
						listDefSpec. defType = kListDefUserProcType;
						listDefSpec. u. userProc = listDefinition;
					}
					CreateCustomList (& my rect, & dataBounds, cSize, & listDefSpec, my macWindow,
						false, false, false, false, & my nat.list.handle);
					SetListRefCon (my nat.list.handle, me);
					#endif
				#endif
				if (my selectionPolicy != XmSINGLE_SELECT && my selectionPolicy != XmBROWSE_SELECT)
					SetListSelectionFlags (my nat.list.handle, lExtendDrag | lNoRect);
			#endif
		} break;
		case xmMenuBarWidgetClass: {
			#if win
				if (! my shell -> motif.shell.isDialog && my shell -> nat.shell.menuBar == NULL && my parent -> widgetClass != xmRowColumnWidgetClass) {
					HMENU bar = CreateMenu ();
					SetMenu (my shell -> window, bar);
					my nat.menu.handle = bar;
					my shell -> nat.shell.menuBar = me;   // does this have to be?
				} else {
					my widgetClass = xmRowColumnWidgetClass;   /* !!!!!!!!!!!!! */
					my orientation = XmHORIZONTAL;
					my rowColumnType = XmMENU_BAR;
					my window = CreateWindowEx (0, theWindowClassName, L"rowColumn", WS_CHILD,
						my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
					SetWindowLong (my window, GWL_USERDATA, (long) me);
				}
			#elif mac
				/*
				 * The first menu bar that occurs in the application shell,
				 * will be positioned in the Macintosh menu bar.
				 * All other menu bars are at the top of their own windows.
				 * This is Motif style, and only works well for Macintoshes with large screens.
				 */
				if ((Widget) GetWRefCon (my macWindow) == theApplicationShell && theMenuBar == NULL) {
					theMenuBar = me;
				} else {
					my widgetClass = xmRowColumnWidgetClass;   /* !!!!!!!!!!!!! */
					my orientation = XmHORIZONTAL;
					my rowColumnType = XmMENU_BAR;
				}
			#endif
		} break;
		case xmMessageBoxWidgetClass: {
			#if win
				my window = CreateWindowEx (0, theWindowClassName, L"messageBox", WS_CHILD | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
			#endif
			my motif.messageBox.okButton = XtVaCreateManagedWidget ("OK", xmPushButtonWidgetClass, me,
				XmNx, 10, XmNy, 105, XmNwidth, MESSAGE_BOX_BUTTON_WIDTH, NULL);
			XtAddCallback (my motif.messageBox.okButton, XmNactivateCallback, cb_messageBox_ok, (XtPointer) me);
			my motif.messageBox.cancelButton = XtVaCreateManagedWidget ("Cancel", xmPushButtonWidgetClass, me,
				XmNx, 20 + MESSAGE_BOX_BUTTON_WIDTH, XmNy, 105, XmNwidth, MESSAGE_BOX_BUTTON_WIDTH, NULL);
			XtAddCallback (my motif.messageBox.cancelButton, XmNactivateCallback, cb_messageBox_cancel, (XtPointer) me);
			my motif.messageBox.helpButton = XtVaCreateManagedWidget ("Help", xmPushButtonWidgetClass, me,
				XmNx, 30 + 2 * MESSAGE_BOX_BUTTON_WIDTH, XmNy, 105, XmNwidth, MESSAGE_BOX_BUTTON_WIDTH, NULL);
			XtAddCallback (my motif.messageBox.helpButton, XmNactivateCallback, cb_messageBox_help, (XtPointer) me);
			my messageText = XtVaCreateManagedWidget ("text", xmTextWidgetClass, me,
				XmNwidth, 42 + 3 * MESSAGE_BOX_BUTTON_WIDTH, XmNheight, 100, NULL);
		} break;
		case xmPulldownMenuWidgetClass: {
			#if win
				int id;
				for (id = 1; id <= MAXIMUM_NUMBER_OF_MENUS; id ++) if (! theMenus [id]) break;
				my nat.menu.id = id;
				theMenus [my nat.menu.id] = me;   /* Instead of UserData fields. */
				if (MEMBER (my parent, MenuBar)) {
					Widget menu;
					UINT beforeID = -1;
					my nat.menu.handle = CreatePopupMenu ();
					/*
					 * Insert the menu before the Help menu, if that exists; otherwise, at the end.
					 */
					for (menu = my parent -> firstChild; menu != NULL; menu = menu -> nextSibling) {
						if (MEMBER (menu, PulldownMenu) && wcsequ (menu -> name, L"Help") && menu != me) {
							beforeID = (UINT) menu -> nat.menu./*handle*/id;
							break;
						}
					}
					{
						MENUITEMINFO info;
						info. cbSize = sizeof (MENUITEMINFO);
						info. fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;
						info. fType = MFT_STRING | ( wcsequ (my name, L"Help") ? MFT_RIGHTJUSTIFY : 0 );
						info. dwTypeData = my name;
						info. hSubMenu = my nat.menu.handle;
						info. wID = (UINT) my nat.menu./*handle*/id;
						InsertMenuItem (my parent -> nat.menu.handle, beforeID, 0, & info);
					} /*else if (beforeID >= 0)
						InsertMenu (my parent -> nat.menu.handle, beforeID, MF_STRING | MF_POPUP | MF_BYCOMMAND,
							(UINT) my nat.menu.handle, my name);
					else
						AppendMenu (my parent -> nat.menu.handle, MF_STRING | MF_POPUP, (UINT) my nat.menu.handle, my name);*/
					DrawMenuBar (my shell -> window);
				} else if (MEMBER (my parent, RowColumn) && my parent -> rowColumnType == XmMENU_BAR) {
					my nat.menu.handle = CreatePopupMenu ();
				}
			#elif mac
				int id;
				for (id = 1; id <= MAXIMUM_NUMBER_OF_MENUS; id ++) if (! theMenus [id]) break;
				my nat.menu.id = id;
				theMenus [my nat.menu.id] = me;   /* Instead of RefCon fields. */
				if (MEMBER (my parent, MenuBar)) {
					/*
					 * This will be a menu in the Macintosh menu bar.
					 */
					if (USE_QUESTION_MARK_HELP_MENU && wcsequ (my name, L"Help")) {
						HMGetHelpMenu (& my nat.menu.handle, NULL);
						theMenus [my nat.menu.id] = NULL;
						my nat.menu.id = kHMHelpMenuID;
						theMenus [MAXIMUM_NUMBER_OF_MENUS] = me;
					} else {
						int beforeID = 0;
						CreateNewMenu (my nat.menu.id, 0, & my nat.menu.handle);
						SetMenuTitleWithCFString (my nat.menu.handle, Melder_peekWcsToCfstring (my name));
						for (id = 1; id <= MAXIMUM_NUMBER_OF_MENUS; id ++) {
							Widget menu = theMenus [id];
							if (menu && wcsequ (menu -> name, L"Help")) {
								beforeID = id;
								break;
							}
						}
						InsertMenu (my nat.menu.handle, beforeID);
					}
				} else if (MEMBER (my parent, RowColumn)) {
					/*
					 * This will be a menu outside the main window,
					 * or at a different place in the main window.
					 * It is implemented as a Mac pop-up menu.
					 */
					CreateNewMenu (my nat.menu.id, 0, & my nat.menu.handle);
					SetMenuTitleWithCFString (my nat.menu.handle, Melder_peekWcsToCfstring (my name));
					InsertMenu (my nat.menu.handle, -1);
				}
			#endif
		} break;
		case xmLabelWidgetClass: {
			#if win
				my window = CreateWindow (L"static", motif_win_expandAmpersands (my name), WS_CHILD |
					( my alignment == XmALIGNMENT_END ? SS_RIGHT :
					  my alignment == XmALIGNMENT_CENTER ? SS_CENTER : SS_LEFT ) | SS_CENTERIMAGE,
					my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
				SetWindowFont (my window, GetStockFont (ANSI_VAR_FONT), FALSE);
			#elif mac
				CreateStaticTextControl (my macWindow, & my rect, NULL, NULL, & my nat.control.handle);
				SetControlReference (my nat.control.handle, (long) me);
				my isControl = TRUE;
				NativeControl_setFont (me, 12);
				NativeControl_setTitle (me);
			#endif
		} break;
		case xmCascadeButtonWidgetClass: {
			if (! my motif.cascadeButton.inBar) {
				#if win
					my window = CreateWindow (L"button", motif_win_expandAmpersands (my name),
						WS_CHILD | BS_PUSHBUTTON | WS_CLIPSIBLINGS,
						my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
					SetWindowLong (my window, GWL_USERDATA, (long) me);
					SetWindowFont (my window, GetStockFont (ANSI_VAR_FONT), FALSE);
				#elif mac
					if (wcsstr (my name, L" -") || (my parent -> rowColumnType == XmMENU_BAR && my parent -> y < 5)) {
						my nat.control.isBevel = true;
						CreateBevelButtonControl (my macWindow, & my rect, NULL, kControlBevelButtonSmallBevel,
							kControlBehaviorPushbutton, NULL, 1, kControlBehaviorCommandMenu, 0, & my nat.control.handle);
						Melder_assert (my nat.control.handle != NULL);
						SetControlReference (my nat.control.handle, (long) me);
						my isControl = TRUE;
						NativeControl_setFont (me, 12);
						NativeControl_setTitle (me);
					} else {
						my nat.control.isPopup = true;
						CreatePopupButtonControl (my macWindow, & my rect, NULL, 1, false,
							0, teFlushLeft, 0, & my nat.control.handle);
						Melder_assert (my nat.control.handle != NULL);
						SetControlReference (my nat.control.handle, (long) me);
						my isControl = TRUE;
						NativeControl_setFont (me, 13);
						NativeControl_setTitle (me);
						SetControlMaximum (my nat.control.handle, 32767);   /* The default seems to be 9 on MacOS X. */
					}
				#endif
			}
		} break;
		case xmPushButtonWidgetClass: {
			#if win
				my window = CreateWindow (L"button", motif_win_expandAmpersands (my name),
					WS_CHILD | BS_PUSHBUTTON | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
				SetWindowFont (my window, GetStockFont (ANSI_VAR_FONT), FALSE);
			#elif mac
				CreatePushButtonControl (my macWindow, & my rect, NULL, & my nat.control.handle);
				Melder_assert (my nat.control.handle);
				SetControlReference (my nat.control.handle, (long) me);
				my isControl = TRUE;
				NativeControl_setFont (me, 12);
				NativeControl_setTitle (me);
			#endif
		} break;
		case xmTextWidgetClass: {
			_GuiText_nativizeWidget (me);
		} break;
		case xmToggleButtonWidgetClass: {
			my isRadioButton = my parent -> radioBehavior || my motif.toggleButton.indicatorType == XmONE_OF_MANY;
			#if win
				my window = CreateWindow (L"button", motif_win_expandAmpersands (my name), WS_CHILD |
					( my isRadioButton ? ( my parent -> radioBehavior ? BS_AUTORADIOBUTTON : BS_RADIOBUTTON )
						: BS_AUTOCHECKBOX ) | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
				SetWindowFont (my window, GetStockFont (ANSI_VAR_FONT), FALSE);
			#elif mac
				if (my isRadioButton) {
					CreateRadioButtonControl (my macWindow, & my rect, NULL, 0, 0, & my nat.control.handle);
				} else {
					CreateCheckBoxControl (my macWindow, & my rect, NULL, 0, 0, & my nat.control.handle);
				}
				Melder_assert (my nat.control.handle);
				SetControlReference (my nat.control.handle, (long) me);
				my isControl = TRUE;
				NativeControl_setFont (me, 12);
				NativeControl_setTitle (me);
			#endif
		} break;
		case xmScaleWidgetClass: {
			#if win
				my window = CreateWindow (PROGRESS_CLASS, motif_win_expandAmpersands (my name), WS_CHILD | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
				SendMessage (my window, PBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG (0, 1000));
			#endif
		} break;
		case xmScrollBarWidgetClass: {
			#if win
				my window = CreateWindow (L"scrollbar", my name, WS_CHILD |
					( wcsequ (my name, L"verticalScrollBar") ? SBS_VERT : SBS_HORZ ) | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
				NativeScrollBar_set (me);
			#elif mac
				my nat.control.handle = NewControl (my macWindow, & my rect,
					"\p", false, 0, 0, 0, scrollBarProc, (long) me);
				Melder_assert (my nat.control.handle);
				my isControl = TRUE;
			#endif
			my minimum = 0;
			my maximum = 100;
			my value = 0;
			my sliderSize = 100;
		} break;
		case xmScrolledWindowWidgetClass: {
			/*
			 * The space of the scrolled window is complete filled by three of its children:
			 * the two scroll bars and the clip window. The first child you create yourself will be the
			 * work window: this work window can grow larger than the clip window.
			 * While normally every widget is only clipped to the rects of its ancestors,
			 * for a scrolled window the clip window will be inserted into this chain.
			 * Example: if the widget hierarchy is
			 *      shell.form.scrolledWindow.column.row.pushButton,
			 * the clipping hierarchy will be
			 *      (shell.)form.(scrolledWindow.)clipWindow.column.row.pushButton
			 */
			#if win
				my window = CreateWindowEx (0, theWindowClassName, L"scrolledWindow", WS_CHILD | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
			#endif
			my motif.scrolledWindow.horizontalBar = XmCreateScrollBar (me, "horizontalScrollBar", NULL, 0);
			my motif.scrolledWindow.verticalBar = XmCreateScrollBar (me, "verticalScrollBar", NULL, 0);
			XtVaSetValues (my motif.scrolledWindow.horizontalBar, XmNorientation, XmHORIZONTAL,
				XmNleftAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 15, XmNheight, 16,
				XmNminimum, 0, XmNmaximum, 100, XmNsliderSize, 100,
				XmNincrement, CELL_HEIGHT, XmNpageIncrement, 101 - CELL_HEIGHT, NULL);
			XtVaSetValues (my motif.scrolledWindow.verticalBar, XmNorientation, XmVERTICAL,
				XmNtopAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 15, XmNwidth, 16,
				XmNminimum, 0, XmNmaximum, 100, XmNsliderSize, 100,
				XmNincrement, CELL_HEIGHT, XmNpageIncrement, 101 - CELL_HEIGHT, NULL);
			my motif.scrolledWindow.clipWindow = XmCreateBulletinBoard (me, "clipWindow", NULL, 0);
			XtVaSetValues (my motif.scrolledWindow.clipWindow,
				XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 1,   /* For border. */
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 16,   /* For scroll bar. */
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 1,   /* For border. */
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 16, NULL);   /* For scroll bar. */
			XtAddCallback (my motif.scrolledWindow.verticalBar, XmNvalueChangedCallback, cb_scroll, (XtPointer) me);
			XtAddCallback (my motif.scrolledWindow.horizontalBar, XmNvalueChangedCallback, cb_scroll, (XtPointer) me);
		} break;
		case xmShellWidgetClass: {
			#if win
				static LPCTSTR className = theApplicationClassName;   /* Only for first window. */
				my window = CreateWindowEx (theDialogHint ? WS_EX_DLGMODALFRAME /* | WS_EX_TOPMOST */ : 0,
					className, className,
					theDialogHint ? WS_CAPTION | WS_SYSMENU : WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,  my parent ? my parent -> window : NULL, NULL, theGui.instance, NULL);
				className = theWindowClassName;   /* All later windows. */
				SetWindowLong (my window, GWL_USERDATA, (long) me);
			#elif mac
				Rect r = my rect;
				OffsetRect (& r, 0, 22);
				CreateNewWindow (kDocumentWindowClass,
					kWindowCloseBoxAttribute +
					( theDialogHint ? 0 : kWindowCollapseBoxAttribute + kWindowResizableAttribute + kWindowFullZoomAttribute),
					& r, & my nat.window.ptr);
				SetWRefCon (my nat.window.ptr, (long) me);   /* So we can find the widget from the event with GetWRefCon (). */
				CreateRootControl (my nat.window.ptr, & my nat.window.rootControl);
				if (theDialogHint) {
					SetThemeWindowBackground (my nat.window.ptr, kThemeBrushDialogBackgroundActive, False);
				} else {
					SetThemeWindowBackground (my nat.window.ptr, kThemeBrushDialogBackgroundActive, False);
				}
				my macWindow = my nat.window.ptr;   /* Associate drawing context; child widgets will inherit. */
				my motif.shell.active = true;   /* Why? */
				SetPortWindowPort (my macWindow);
				motif_mac_defaultFont ();   /* Will be set back after every change. */
			#endif
			my motif.shell.isDialog = theDialogHint;   /* So we can maintain a single Shell class instead of two different. */
		} break;
		default: break;
	}
	my nativized = True;
}

static Widget createWidget (int widgetClass, Widget parent, const char *name) {
	Widget me = _Gui_initializeWidget (widgetClass, parent, Melder_peekUtf8ToWcs (name));
	_GuiNativizeWidget (me);
	return me;
}

static Widget vaCreateWidget (int widgetClass, Widget parent, const char *name, va_list arg) {
	Widget me = _Gui_initializeWidget (widgetClass, parent, Melder_peekUtf8ToWcs (name));
	int resource;
	while (resource = va_arg (arg, int), resource != 0) switch (resource) {
		case XmNalignment: {
			my alignment = va_arg (arg, int);
		} break; case XmNindicatorType: {
			Melder_assert (my widgetClass == xmToggleButtonWidgetClass);
			my motif.toggleButton.indicatorType = va_arg (arg, int);
		} break; case XmNselectionPolicy: {
			Melder_assert (my widgetClass == xmListWidgetClass);
			my selectionPolicy = va_arg (arg, int);
		} break; default: {
			(void) va_arg (arg, int);
		}
	}
	_GuiNativizeWidget (me);
	return me;
}

void _Gui_invalidateWidget (Widget me) {
	if (! my managed) return;   /* Should be: visible. */
	if (MEMBER (me, Shell) /*||
		 my widgetClass == xmRowColumnWidgetClass ||
		 my widgetClass == xmFormWidgetClass*/) return;   /* Composites are not invalidated !!!!! ???? */
	#if mac
		_GuiMac_clipOn (me);
		EraseRect (& my rect);
		_motif_clipOffInvalid (me);
	#endif
}

void _Gui_validateWidget (Widget me) {
	if (! my managed) return;   /* Should be: visible. */
	if (MEMBER (me, Shell)) return;
	#if mac
		_GuiMac_clipOn (me);
		_GuiMac_clipOffValid (me);   
	#endif
}

static void Native_move (Widget me, int dx, int dy) {
/* Native_move () changes the native attributes and visual position of the widget:
 * No motif attributes (like 'x' and 'y') are changed.
 * Usage:
 *     Native_move () is normally called immediately after changing the 'x' or 'y' attribute.
 *     It can be seen as the 'realization' of a Motif move.
 */
#if win
	(void) dx;
	(void) dy;
	if (! my window) return;   // ignore menu items
	if (MEMBER (me, Shell)) {
		my nat.shell.duringMoveWindow = True;
		if (my motif.shell.isDialog)
			MoveWindow (my window, my x, my y, my width + 2 * GetSystemMetrics (SM_CXFIXEDFRAME),
				my height + 2 * GetSystemMetrics (SM_CYFIXEDFRAME) + GetSystemMetrics (SM_CYCAPTION), TRUE);
		else
			MoveWindow (my window, my x, my y,
				my width + 2 * GetSystemMetrics (SM_CXSIZEFRAME),
				my height + 2 * GetSystemMetrics (SM_CYSIZEFRAME) + GetSystemMetrics (SM_CYCAPTION) +
				( my nat.shell.menuBar != NULL ? GetSystemMetrics (SM_CYMENU) : 0 ), TRUE);
	} else
		MoveWindow (my window, my x, my y, my width, my height, True);
#elif mac
	/*
	 * On Mac, the 'rect' attribute is changed, as is the position information in the Mac object.
	 * Native_move () calls itself with the same arguments for all the widget's children.
	 */
	Widget child;
	if ((dx == 0 && dy == 0) || MEMBER (me, MenuBar) || (my parent && MEMBER (my parent, PulldownMenu))) return;
	if (! my isControl)
		_Gui_invalidateWidget (me);   /* At old position. BUG: clipping may be wrong because parent may have moved;
				however, parent will have been erased in that case, so we will not see it. */
	/*
	 * Because all non-shell coordinates are port-relative on Mac, we have to adapt them to the new x and y.
	 */
	my rect.left += dx, my rect.right += dx;
	my rect.top += dy, my rect.bottom += dy;
	if (MEMBER (me, Text)) {
		_GuiMacText_move (me);
	} else if (my isControl) {
		_GuiMac_clipOn (me);
		MoveControl (my nat.control.handle, my rect.left, my rect.top);
		_GuiMac_clipOffValid (me);
	} else if (MEMBER (me, List)) {
		(** my nat.list.handle). rView = my rect;
	} else if (MEMBER (me, Shell)) {
		MoveWindow (my nat.window.ptr, my rect.left, my rect.top + 22, 0);
	} else if (MEMBER (me, DrawingArea)) {
		_Gui_callCallbacks (me, & my motif.drawingArea.moveCallbacks, 0);   /* Only Mac? */
	}
	if (! my isControl) _Gui_invalidateWidget (me);   /* At new position. */
	/*
	 * Because all non-shell coordinates are port-relative on Mac, we have to recursively move the children.
	 */
	if (! MEMBER (me, Shell))
		for (child = my firstChild; child; child = child -> nextSibling)
			Native_move (child, dx, dy);
#endif
}

#if mac
/*
 * hideControls and reshowControls are only called by shellResizeWidget,
 * but they must be defined separately because they are recursive.
 */
static void reshowControls (Widget me) {
	Widget child;
	if (my widgetClass == xmPulldownMenuWidgetClass) return;
	for (child = my firstChild; child; child = child -> nextSibling) {
		if (child -> isControl) {
			/*
			 * This is a Mac control widget.
			 * Let Mac do the redrawing if Motif considers it visible and Mac considers it invisible?
			 */
			if (child -> managed && ! IsControlVisible (child -> nat.control.handle)) {
				_GuiMac_clipOn (child);
				ShowControl (child -> nat.control.handle);
				_GuiMac_clipOffValid (child);
			}
		} else if (! MEMBER (child, Shell)) {
			/*
			 * This may be a widget with children.
			 * Some of those children may be controls, or contain controls, so recurse.
			 */
			reshowControls (child);
		}
	}
}
static void hideControls (Widget me) {
	Widget child;
	if (my widgetClass == xmPulldownMenuWidgetClass) return;
	for (child = my firstChild; child; child = child -> nextSibling) {
		if (child -> isControl) {
			if (child -> managed) {
				HideControl (child -> nat.control.handle);
			}
		} else if (! MEMBER (child, Shell)) {
			hideControls (child);
		}
	}
}
#endif

static void _motif_manageScrolledWindow (Widget w);   /* Forward. */
static void shellResizeWidget (Widget me, int dx, int dy, int dw, int dh) {
	Widget child;
	#if win
	Melder_assert (! my shell || ! my shell -> nat.shell.duringMoveWindow);
		if (my window && ! MEMBER (me, Shell)) {
			/*RECT rect, client;
			GetWindowRect (my window, & rect);
			GetClientRect (my window, & client);
			ScreenToClient (my parent -> window, (LPPOINT) & rect.left);
			ScreenToClient (my parent -> window, (LPPOINT) & rect.right);
			if (rect.left!=my x||rect.right!=my x + my width||rect.top !=my y||rect.bottom!=my y +my height)
			Melder_warning ("class %d x %d left %d y %d top %d width %d right %d height %d bottom %d",
			my widgetClass, my x, rect.left, my y, rect.top, my width, rect.right, my height, rect.bottom);*/
			MoveWindow (my window, my x, my y, my width, my height, TRUE);
			if (MEMBER (me, DrawingArea) && my resizeCallback) my resizeCallback (me, my resizeClosure, NULL);
		}
	#elif mac
		if ((dx == 0 && dy == 0 && dw == 0 && dh == 0) || MEMBER (me, MenuBar) || (my parent && MEMBER (my parent, PulldownMenu))) return;
		OffsetRect (& my rect, dx, dy);
		my rect.right += dw;
		my rect.bottom += dh;
		if (MEMBER (me, Text)) {
			_GuiMacText_shellResize (me);
		} else if (my isControl) {
			ControlHandle c = my nat.control.handle;
			/*if (my managed) HideControl (c);*/
			if (dx || dy) MoveControl (c, my rect.left, my rect.top);
			if (dw || dh) SizeControl (c, my width, my height);
			//if (MEMBER (me, List)) {
			//	(** my nat.list.handle). rView = my rect;
			//	(** my nat.list.handle). cellSize. h = my width;
			//}
		} else if (MEMBER (me, List)) {
			(** my nat.list.handle). rView = my rect;
			(** my nat.list.handle). cellSize. h = my width;
		} else if (MEMBER (me, Shell)) {
			Rect portRect;
			GetWindowPortBounds (my nat.window.ptr, & portRect);
			SetPortWindowPort (my nat.window.ptr);
			/*
				Erase old area.
			*/
			EraseRect (& portRect);
			SizeWindow (my nat.window.ptr, my width, my height, false);
			hideControls (me);
			/*
				Redraw new area.
			*/
			GetWindowPortBounds (my nat.window.ptr, & portRect);
			InvalWindowRect (my nat.window.ptr, & portRect);
		} else if (MEMBER (me, DrawingArea)) {
			if (my resizeCallback) my resizeCallback (me, my resizeClosure, NULL);
			if (dx != 0 || dy != 0)
				_Gui_callCallbacks (me, & my motif.drawingArea.moveCallbacks, 0);
		}
	#endif
	for (child = my firstChild; child; child = child -> nextSibling) {
		int cdx = 0, cdy = 0, cdw = 0, cdh = 0;
		if (MEMBER (child, Shell)) continue;
		if (child -> rightAttachment == XmATTACH_FORM) {
			if (child -> leftAttachment == XmATTACH_FORM) cdw = dw; else cdx = dw;
		}
		if (child -> leftAttachment == XmATTACH_POSITION && child -> rightAttachment == XmATTACH_POSITION) {
			int xLeft = my width * (child -> leftPosition / 100.0);
			int xRight = my width * (child -> rightPosition / 100.0);
			cdx = xLeft - child -> x;
			cdw = (xRight - xLeft) - child -> width;
		}
		if (child -> bottomAttachment == XmATTACH_FORM) {
			if (child -> topAttachment == XmATTACH_FORM) cdh = dh; else cdy = dh;
		}
		if (child -> topAttachment == XmATTACH_POSITION && child -> bottomAttachment == XmATTACH_POSITION) {
			int yTop = my height * (child -> topPosition / 100.0);
			int yBottom = my height * (child -> bottomPosition / 100.0);
			cdy = yTop - child -> y;
			cdh = (yBottom - yTop) - child -> height;
		}
		child -> x += cdx;
		child -> y += cdy;
		child -> width += cdw;
		child -> height += cdh;
		shellResizeWidget (child, dx + cdx, dy + cdy, cdw, cdh);
	}
	if (MEMBER (me, ScrolledWindow))
		_motif_manageScrolledWindow (me);
	#if mac
		if (MEMBER (me, Shell)) reshowControls (me);
	#endif
}

static void resizeWidget (Widget me, int dw, int dh) {
	Widget child;
	#if win
		if (my window && ! MEMBER (me, Shell)) {
			MoveWindow (my window, my x, my y, my width, my height, TRUE);
			if (MEMBER (me, DrawingArea) && my resizeCallback) my resizeCallback (me, my resizeClosure, NULL);
		}
	#elif mac
	{
		if ((dw == 0 && dh == 0) || (my parent && MEMBER (my parent, PulldownMenu))) return;
		if (! my isControl) _Gui_invalidateWidget (me);   /* At old position. */
		my rect.right += dw;
		my rect.bottom += dh;
		if (MEMBER (me, Text)) {
			_GuiMacText_resize (me);
		} else if (my isControl) {
			SizeControl (my nat.control.handle, my width, my height);
			_Gui_validateWidget (me);
		} else if (MEMBER (me, List)) {
			(** my nat.list.handle). rView = my rect;
			SetPortWindowPort (my macWindow);
			(** my nat.list.handle). cellSize. h = my width;
			if (my parent -> widgetClass == xmScrolledWindowWidgetClass)
				_motif_manageScrolledWindow (my parent);
		} else if (MEMBER (me, Shell)) {
			SizeWindow (my macWindow, my width, my height, true);
		} else if (MEMBER (me, DrawingArea)) {
			if (my resizeCallback) my resizeCallback (me, my resizeClosure, NULL);
		}
		if (! my isControl) _Gui_invalidateWidget (me);   /* At new position. */
	}
	#endif
	if (MEMBER2 (me, Form, ScrolledWindow))
		for (child = my firstChild; child; child = child -> nextSibling) {
			int cdx = 0, cdy = 0, cdw = 0, cdh = 0;
			if (child -> widgetClass == xmShellWidgetClass) continue;
			if (child -> rightAttachment == XmATTACH_FORM) {
				if (child -> leftAttachment == XmATTACH_FORM) cdw = dw; else cdx = dw;
			}
			if (child -> leftAttachment == XmATTACH_POSITION && child -> rightAttachment == XmATTACH_POSITION) {
				int xLeft = my width * (child -> leftPosition / 100.0);
				int xRight = my width * (child -> rightPosition / 100.0);
				cdx = xLeft - child -> x;
				cdw = (xRight - xLeft) - child -> width;
			}
			if (child -> bottomAttachment == XmATTACH_FORM) {
				if (child -> topAttachment == XmATTACH_FORM) cdh = dh; else cdy = dh;
			}
			if (child -> topAttachment == XmATTACH_POSITION && child -> bottomAttachment == XmATTACH_POSITION) {
				int yTop = my height * (child -> topPosition / 100.0);
				int yBottom = my height * (child -> bottomPosition / 100.0);
				cdy = yTop - child -> y;
				cdh = (yBottom - yTop) - child -> height;
			}
			if (cdx || cdy) {
				child -> x += cdx;
				child -> y += cdy;
				Native_move (child, cdx, cdy);
			}
			if (cdw || cdh) {
				child -> width += cdw;
				child -> height += cdh;
				resizeWidget (child, cdw, cdh);
			}
		}
	if (MEMBER (me, Shell)) {
		#if mac
			Rect portRect;
			int right, bottom;
			GetWindowPortBounds (my macWindow, & portRect);
			right = portRect. right, bottom = portRect. bottom;
		#elif win
			int right = 1000, bottom = 500;
		#endif
		#if mac
		{
			Rect rect;
			SetRect (& rect, right - 15, bottom - 15, right + 1, bottom + 1);
			ClipRect (& rect);
			DrawGrowIcon (my macWindow);
			GuiMac_clipOff ();
		}
		#endif
		for (child = my firstChild; child; child = child -> nextSibling)
			if (MEMBER2 (child, Form, ScrolledWindow)) {
				child -> width += dw;
				child -> height += dh;
				resizeWidget (child, dw, dh);
			}
	}
}

static void _motif_setValues (Widget me, va_list arg) {
	int resource;
	Boolean move = False, resize = False, attach = False, scrollset = False;
	char *text;
	#if win
		int oldX = my x, oldY = my y, oldWidth = my width, oldHeight = my height;
	#endif
	#if mac
		Str255 ptext;
	#endif
	while (resource = va_arg (arg, int), resource != 0) switch (resource) {
		case XmNaccelerator: {
			char *string = va_arg (arg, char *), *key = strstr (string, "<Key>");
			Melder_assert (MEMBER (me, PushButton));
			if (! key) break;
			key = & key [5];
			if (my inMenu && key [0]) {
				int kar = 0, modifiers = 0;
				if (strstr (string, "Ctrl")) modifiers |= _motif_COMMAND_MASK;
				if (strstr (string, "Shift")) modifiers |= _motif_SHIFT_MASK;
				if (strstr (string, "Mod1")) modifiers |= _motif_OPTION_MASK;
				if (key [1] == '\0') kar = key [0];   /* Single character. */
				else if (strequ (key, "Return")) kar = 13;
				else if (strequ (key, "BackSpace")) kar = 8;
				else if (strequ (key, "question")) kar = '?';
				else if (strequ (key, "bracketleft")) kar = '[';
				else if (strequ (key, "bracketright")) kar = ']';
				else if (strequ (key, "Left")) kar = motif_LEFT_ARROW;
				else if (strequ (key, "Right")) kar = motif_RIGHT_ARROW;
				else if (strequ (key, "Up")) kar = motif_UP_ARROW;
				else if (strequ (key, "Down")) kar = motif_DOWN_ARROW;
				else if (strequ (key, "Pause")) kar = motif_PAUSE;
				else if (strequ (key, "Delete")) kar = motif_DELETE;
				else if (strequ (key, "Insert")) kar = motif_INSERT;
				else if (strequ (key, "Tab")) kar = motif_TAB;
				else if (strequ (key, "Home")) kar = motif_HOME;
				else if (strequ (key, "End")) kar = motif_END;
				else if (strequ (key, "Page_Up")) kar = motif_PAGE_UP;
				else if (strequ (key, "Page_Down")) kar = motif_PAGE_DOWN;
				else if (strequ (key, "Escape")) kar = motif_ESCAPE;
				else if (strequ (key, "F1")) kar = motif_F1;
				else if (strequ (key, "F2")) kar = motif_F2;
				else if (strequ (key, "F3")) kar = motif_F3;
				else if (strequ (key, "F4")) kar = motif_F4;
				else if (strequ (key, "F5")) kar = motif_F5;
				else if (strequ (key, "F6")) kar = motif_F6;
				else if (strequ (key, "F7")) kar = motif_F7;
				else if (strequ (key, "F8")) kar = motif_F8;
				else if (strequ (key, "F9")) kar = motif_F9;
				else if (strequ (key, "F10")) kar = motif_F10;
				else if (strequ (key, "F11")) kar = motif_F11;
				else if (strequ (key, "F12")) kar = motif_F12;
				if (kar > 0 && kar < 32)
					my shell -> motif.shell.lowAccelerators [modifiers] |= 1 << kar;
				else if (kar == '?' || kar == '{' || kar == '}' || kar == '\"' || kar == '<' || kar == '>' || kar == '|' ||
								kar == '_' || kar == '+' || kar == '~')
					modifiers |= _motif_SHIFT_MASK;
				my motif.pushButton.acceleratorChar = kar;
				my motif.pushButton.acceleratorModifiers = modifiers;
				NativeMenuItem_setText (me);
			}
			break;
		}
		case XmNacceleratorText: {
			char *string = va_arg (arg, char *);
			Melder_assert (MEMBER (me, PushButton));
			break;
		}
		case XmNalignment:
			my alignment = va_arg (arg, int);
			break;
		case XmNautoUnmanage:
			my autoUnmanage = va_arg (arg, int);
			break;
		case XmNborderWidth: (void) va_arg (arg, int); break;
		case XmNbottomAttachment:
			my bottomAttachment = va_arg (arg, int);
			attach = True;
			break;
		case XmNbottomOffset:
			my bottomOffset = va_arg (arg, int);
			attach = True;
			break;
		case XmNbottomPosition: my bottomPosition = va_arg (arg, int);
			attach = True;
			break;
		case XmNcancelButton:
			Melder_assert (MEMBER3 (me, Form, BulletinBoard, MessageBox));
			my shell -> cancelButton = my cancelButton = va_arg (arg, Widget);
			break;
		case XmNcancelLabelString: {
			char *text = va_arg (arg, char *);
			Melder_assert (MEMBER (me, MessageBox));
			if (my motif.messageBox.cancelButton) XtVaSetValues (my motif.messageBox.cancelButton, XmNlabelString, text, NULL);
			break;
		}
		case XmNcolumns: {
			int columns = va_arg (arg, int);
			Melder_assert (MEMBER (me, Text));
			my width = columns * (mac ? 7 : 9) + 4;
			resize = True;
		} break;
		case XmNdefaultButton:
			Melder_assert (MEMBER3 (me, Form, BulletinBoard, MessageBox));
			my shell -> defaultButton = my defaultButton = va_arg (arg, Widget);
			Melder_assert (MEMBER (my defaultButton, PushButton));
			#if mac
			{
				Boolean set = true;
				Melder_assert (my defaultButton -> nat.control.handle);
				SetControlData (my defaultButton -> nat.control.handle, kControlEntireControl, kControlPushButtonDefaultTag, sizeof (Boolean), & set);
			}
			#endif
			break;
		case XmNdeleteResponse:
			Melder_assert (MEMBER (me, Shell));
			my deleteResponse = va_arg (arg, int);
			if (my deleteResponse == XmDO_NOTHING && ! my motif.shell.goAwayCallback) {
				#if mac
					ChangeWindowAttributes (my macWindow, 0, kWindowCloseBoxAttribute);
				#endif
			}
			break;
		case XmNdialogStyle:
			Melder_assert (MEMBER3 (me, Form, BulletinBoard, MessageBox));
			my shell -> dialogStyle = my dialogStyle = va_arg (arg, int);
			break;
		case XmNdialogTitle:
			Melder_assert (MEMBER3 (me, Form, BulletinBoard, MessageBox));
			text = va_arg (arg, char *);
			#if win
				SetWindowText (my shell -> window, Melder_peekUtf8ToWcs (text));
			#elif mac
				ptext [0] = strlen (text); strcpy ((char *) ptext + 1, text);
				SetWTitle (my macWindow, ptext);
			#endif
			break;
		case XmNdialogType:
			Melder_assert (MEMBER (me, MessageBox));
			my shell -> dialogType = my dialogType = va_arg (arg, int);
			break;
		case XmNeditMode: (void) va_arg (arg, int); break;
		case XmNeditable:
			Melder_assert (MEMBER (me, Text));
			my motif.text.editable = va_arg (arg, int);
			break;
		case XmNentryAlignment:
			Melder_assert (MEMBER (me, RowColumn));
			(void) va_arg (arg, int);
			break;
		case XmNheight:
			my height = va_arg (arg, int);
			if (MEMBER (me, Shell)) {
				int maximumHeight =
					#if win
						GetSystemMetrics (SM_CYMAXIMIZED) - GetSystemMetrics (SM_CYCAPTION) -
							GetSystemMetrics (SM_CYSIZEFRAME) - GetSystemMetrics (SM_CYMENU) - 15;
					#elif mac
						GetQDGlobalsScreenBits (& theScreenBits) -> bounds.bottom - 41;
					#endif
				if (my height > maximumHeight) my height = maximumHeight;
			}
			resize = True;
			break;
		case XmNhelpLabelString: {
			char *text = va_arg (arg, char *);
			Melder_assert (MEMBER (me, MessageBox));
			if (my motif.messageBox.helpButton) XtVaSetValues (my motif.messageBox.helpButton, XmNlabelString, text, NULL);
			break;
		}
		case XmNhorizontalScrollBar: {
			/* Have to kill my own bar first. */
			XtDestroyWidget (my motif.scrolledWindow.horizontalBar);
			/* Then replace by new bar. */
			my motif.scrolledWindow.horizontalBar = va_arg (arg, Widget);
			/* Make sure it is in the right position. */
			XtVaSetValues (my motif.scrolledWindow.horizontalBar, XmNorientation, XmHORIZONTAL,
				XmNleftAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 15, XmNheight, 16, NULL);
			/*
			 * Note that we may now have a horizontal bar with application-defined scrolling policy,
			 * and a vertical bar with automatic scrolling policy.
			 * This is more flexible than X Motif, but may be simulated in X Motif with the following strategy,
			 * which is compatible with MotifEmulator:
			 *   - set the scrolling policy to XmAUTOMATIC: two scroll bars will be created;
			 *   - ask for the horizontal scroll bar with XtVaGetValues;
			 *   - remove all the callbacks from the horizontal scroll bar with XtRemoveAllCallbacks;
			 *   - add your application-defined callback with XtAddCallback.
			 * We just hope now that X Motif does not look into the XmNscrollingPolicy resource after this...
			 */
		} break;
		case XmNiconName:
			Melder_assert (MEMBER (me, Shell));
			text = va_arg (arg, char *);
			/* Ignore. */
			break;
		case XmNincrement:
			Melder_assert (MEMBER (me, ScrollBar));
			my increment = va_arg (arg, int);
			break;
		case XmNitemCount:
			(void) va_arg (arg, int);
			break;
		case XmNindicatorType:
			(void) va_arg (arg, int);
			break;
		case XmNlabelString:
			Melder_assert (MEMBER3 (me, Label, CascadeButton, PushButton));
			text = va_arg (arg, char *);
			Melder_free (my name);
			my name = Melder_utf8ToWcs (text);
			if (my inMenu) {
				NativeMenuItem_setText (me);
			} else if (MEMBER (me, CascadeButton) && my motif.cascadeButton.inBar) {
				/* BUG: menu title change not implemented */
			} else {
				if (MEMBER (me, Label)) {
					int preferredWidth = NativeLabel_preferredWidth (me);
					if (preferredWidth > my width) {
						my width = preferredWidth;
						resize = True;
					}
					NativeControl_setTitle (me);
				} else {
					NativeControl_setTitle (me);
				}
			}
			break;
		case XmNleftAttachment: my leftAttachment = va_arg (arg, int);
			attach = True;
			break;
		case XmNleftOffset: my leftOffset = va_arg (arg, int);
			attach = True;
			break;
		case XmNleftPosition: my leftPosition = va_arg (arg, int);
			attach = True;
			break;
		case XmNlistSizePolicy: (void) va_arg (arg, int); break;
		case XmNmarginHeight: (void) va_arg (arg, int); break;
		case XmNmarginWidth: (void) va_arg (arg, int); break;
		case XmNmaximum:
			my maximum = va_arg (arg, int);
			if (MEMBER (me, ScrollBar)) scrollset = True;
			else if (MEMBER (me, Scale)) _Gui_invalidateWidget (me);
			break;
		case XmNmenuHelpWidget:
			(void) va_arg (arg, Widget);
			break;
		case XmNmessageString:
			Melder_assert (MEMBER (me, MessageBox));
			text = va_arg (arg, char *);
			XmTextSetString (my messageText, text);
			break;
		case XmNminimum:
			my minimum = va_arg (arg, int);
			if (MEMBER (me, ScrollBar)) scrollset = True;
			else if (MEMBER (me, Scale)) _Gui_invalidateWidget (me);
			break;
		case XmNokLabelString: {
			char *text = va_arg (arg, char *);
			Melder_assert (MEMBER (me, MessageBox));
			if (my motif.messageBox.okButton) XtVaSetValues (my motif.messageBox.okButton, XmNlabelString, text, NULL);
			break; }
		case XmNorientation:
			Melder_assert (MEMBER3 (me, RowColumn, ScrollBar, Scale));
			my orientation = va_arg (arg, int);
			break;
		case XmNpacking:
			Melder_assert (MEMBER (me, RowColumn));
			my packing = va_arg (arg, int);
			break;
		case XmNpageIncrement:
			Melder_assert (MEMBER (me, ScrollBar));
			my pageIncrement = va_arg (arg, int);
			break;
		case XmNradioBehavior:
			Melder_assert (MEMBER (me, RowColumn));
			my radioBehavior = va_arg (arg, int);
			break;
		case XmNrightAttachment:
			my rightAttachment = va_arg (arg, int);
			attach = True;
			break;
		case XmNrightOffset:
			my rightOffset = va_arg (arg, int);
			attach = True;
			break;
		case XmNrightPosition: my rightPosition = va_arg (arg, int);
			attach = True;
			break;
		case XmNrowColumnType:
			Melder_assert (MEMBER (me, RowColumn));
			my rowColumnType = va_arg (arg, int);
			break;
		case XmNrows: (void) va_arg (arg, int); break;
		case XmNscaleHeight:
			Melder_assert (MEMBER (me, Scale));
			my height = va_arg (arg, int);
			resize = True;
			break;
		case XmNscaleWidth:
			Melder_assert (MEMBER (me, Scale));
			my width = va_arg (arg, int);
			resize = True;
			break;
		case XmNscrollBarDisplayPolicy: (void) va_arg (arg, int); break;
		case XmNscrollingPolicy: (void) va_arg (arg, int); break;
		case XmNselectionPolicy:
			Melder_assert (MEMBER (me, List));
			my selectionPolicy = va_arg (arg, int);
			break;
		case XmNset: {
			int set = va_arg (arg, int);
			XmToggleButtonSetState (me, set, 0);
			break; }
		case XmNsliderSize:
			Melder_assert (MEMBER (me, ScrollBar));
			my sliderSize = va_arg (arg, int);
			scrollset = True;
			break;
		case XmNspacing:
			(void) va_arg (arg, int);
			break;
		case XmNsubMenuId:
			Melder_assert (MEMBER (me, CascadeButton));
			my subMenuId = va_arg (arg, Widget);
			my subMenuId -> popUpButton = me;
			if (my inMenu) {
				#if win
				#elif mac
					if (my nat.entry.item) {
						SetMenuItemHierarchicalID (my nat.entry.handle, my nat.entry.item, my subMenuId -> nat.menu.id);
					}
				#endif
			} else {
				#if win
				#elif mac
					if (my isControl && my nat.control.isPopup) {
						SetControlData (my nat.control.handle, kControlMenuPart, kControlPopupButtonMenuHandleTag, 4, & my subMenuId -> nat.menu.handle);
						NativeControl_setTitle (me);
					}
				#endif
			}
			break;
		case XmNtitle:
			Melder_assert (MEMBER (me, Shell));
			text = va_arg (arg, char *);
			#if mac
				ptext [0] = strlen (text); strcpy ((char *) ptext + 1, text);
				SetWTitle (my nat.window.ptr, ptext);
			#elif win
				SetWindowText (my window, Melder_peekUtf8ToWcs (text));
			#endif
			break;
		case XmNtitleString:
			Melder_assert (MEMBER (me, Scale));
			text = va_arg (arg, char *);
			Melder_free (my name);
			my name = Melder_utf8ToWcs (text);
			_Gui_invalidateWidget (me);
			break;
		case XmNtopAttachment:
			my topAttachment = va_arg (arg, int);
			attach = True;
			break;
		case XmNtopItemPosition:
			(void) va_arg (arg, int);
			break;
		case XmNtopOffset:
			my topOffset = va_arg (arg, int);
			attach = True;
			break;
		case XmNtopPosition: my topPosition = va_arg (arg, int);
			attach = True;
			break;
		case XmNtraversalOn:
			(void) va_arg (arg, int);
			break;
		case XmNvalue:
			my value = va_arg (arg, int);
			if (MEMBER (me, ScrollBar)) scrollset = True;
			else if (MEMBER (me, Scale)) {
				#if win
					SendMessage (my window, PBM_SETPOS, (WPARAM) my value, 0);
				#elif mac
					_Gui_invalidateWidget (me);
				#endif
			}
			break;
		case XmNverticalScrollBar: {
			/* Have to kill my own bar first. */
			XtDestroyWidget (my motif.scrolledWindow.verticalBar);
			/* Then replace by new bar. */
			my motif.scrolledWindow.verticalBar = va_arg (arg, Widget);
			/* Make sure it is in the right position. */
			XtVaSetValues (my motif.scrolledWindow.verticalBar, XmNorientation, XmVERTICAL,
				XmNtopAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 15, XmNwidth, 16, NULL);
		} break;
		case XmNvisibleItemCount: {
			int height;
			Melder_assert (MEMBER (me, List));
			my visibleItemCount = va_arg (arg, int);
			height = my visibleItemCount * CELL_HEIGHT;
			if (MEMBER (my parent, ScrolledWindow))
				XtVaSetValues (my parent, XmNheight, height + 16, NULL);
			else
				XtVaSetValues (me, XmNheight, height, NULL);
		} break;
		case XmNwidth:
			my width = va_arg (arg, int);
			resize = True;
			break;
		case XmNwordWrap: {
			int wrap = va_arg (arg, int);
			#if win
				DestroyWindow (my window);
				if (wrap) {
					my window = CreateWindowW (L"edit", NULL, WS_CHILD | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
						my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
				} else {
					my window = CreateWindowW (L"edit", NULL, WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
						my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
				}
				SetWindowLong (my window, GWL_USERDATA, (long) me);
		static HFONT font;
		if (! font) font = CreateFontW (15, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
				SetWindowFont (my window, font /*GetStockFont (ANSI_VAR_FONT)*/, FALSE);
			#endif
		} break;
		case XmNx: my x = va_arg (arg, int); move = True; break;
		case XmNy: my y = va_arg (arg, int); move = True; break;

		default: {
			if (resource < 0 || resource >= sizeof motif_resourceNames / sizeof (char *))
				Melder_flushError ("(XtVaSetValues:) Resource out of range (%d).", resource);
			else
				Melder_flushError ("(XtVaSetValues:) Unknown resource \"%s\".",
					motif_resourceNames [resource]);
			return;   /* Because we do not know how to skip this unknown resource." */
		}
	}

	if (attach) {
		Melder_assert (MEMBER2 (my parent, Form, ScrolledWindow));
		if (my leftAttachment == XmATTACH_FORM) {
			my x = my leftOffset;
			move = True;
			if (my rightAttachment == XmATTACH_FORM) {
				my width = my parent -> width - my leftOffset - my rightOffset;
				resize = True;
			}
		} else if (my rightAttachment == XmATTACH_FORM) {
			my x = my parent -> width - my width - my rightOffset;
			move = True;
		}
		if (my leftAttachment == XmATTACH_POSITION && my rightAttachment == XmATTACH_POSITION) {
			my x = my parent -> width * (my leftPosition / 100.0);
			int xRight = my parent -> width * (my rightPosition / 100.0);
			my width = xRight - my x;
			resize = True;
		}
		if (my topAttachment == XmATTACH_FORM) {
			my y = my topOffset;
			move = True;
			if (my bottomAttachment == XmATTACH_FORM) {
				my height = my parent -> height - my topOffset - my bottomOffset;
				resize = True;
			}
		} else if (my bottomAttachment == XmATTACH_FORM) {
			my y = my parent -> height - my height - my bottomOffset;
			move = True;
		}
		if (my topAttachment == XmATTACH_POSITION && my bottomAttachment == XmATTACH_POSITION) {
			my y = my parent -> height * (my topPosition / 100.0);
			int yBottom = my parent -> height * (my bottomPosition / 100.0);
			my height = yBottom - my y;
			resize = True;
		}
	}
	if (move) {
		#if win
			Native_move (me, 0, 0);
		#elif mac
			int dx, dy;
			dx = my x - my rect.left + (my parent && my widgetClass != xmShellWidgetClass ? my parent -> rect.left : 0);
			dy = my y - my rect.top + (my parent && my widgetClass != xmShellWidgetClass ? my parent -> rect.top : 0);
			Native_move (me, dx, dy);
		#endif
	}
	if (resize) {
		#if mac
			int oldWidth = my rect.right - my rect.left, oldHeight = my rect.bottom - my rect.top;
		#endif
		resizeWidget (me, my width - oldWidth, my height - oldHeight);
	}
	if (scrollset) NativeScrollBar_set (me);
}

#if mac
static Widget _motif_findSubwidget (Widget me, int x, int y) {
	if (! my managed) return NULL;
	if (my firstChild == NULL && my parent != NULL) {
		Rect *rect;
		if (MEMBER (my parent, MenuBar)) return NULL;
		rect = ! MEMBER (me, ScrollBar) && my parent && MEMBER (my parent, ScrolledWindow) ?
			& my parent -> motif.scrolledWindow.clipWindow -> rect : & my rect;
		if (x >= rect -> left && x <= rect -> right && y >= rect -> top && y <= rect -> bottom)
			return me;
		else
			return NULL;
	} else {
		Widget child = my firstChild;
		for (child = my firstChild; child != NULL; child = child -> nextSibling) {
			if (! MEMBER (child, Shell)) {
				Widget result = _motif_findSubwidget (child, x, y);
				if (result) return result;
			}
		}
		return NULL;
	}
}
#endif

static void _motif_manageScrolledWindow (Widget me) {
	int workWidth, workHeight, horzAutomatic, vertAutomatic;
	Widget clipWindow, workWindow, horzBar = my motif.scrolledWindow.horizontalBar, vertBar = my motif.scrolledWindow.verticalBar;
	Melder_assert (my widgetClass == xmScrolledWindowWidgetClass);
	clipWindow = my motif.scrolledWindow.clipWindow;
	workWindow = my motif.scrolledWindow.workWindow;
	if (clipWindow == NULL || horzBar == NULL || vertBar == NULL) return;   /* Apparently during destruction of scrolled window. */
	/*
	 * We must find out if the scrolling policy of each bar is automatic.
	 * Otherwise, we must not change them automatically.
	 */
	horzAutomatic = horzBar -> motif.scrollBar.valueChangedCallbacks.pairs [0].proc == cb_scroll;
	vertAutomatic = vertBar -> motif.scrollBar.valueChangedCallbacks.pairs [0].proc == cb_scroll;
	/*
	 * If the work window has been unmanaged or destroyed, the automatic scroll bars should be empty and insensitive.
	 */
	if (workWindow == NULL || ! workWindow -> managed) {
		if (horzAutomatic)
			XtVaSetValues (horzBar, XmNmaximum, 100, XmNsliderSize, 100, XmNvalue, 0, XmNpageIncrement, 1, NULL);
		if (vertAutomatic)
			XtVaSetValues (vertBar, XmNmaximum, 100, XmNsliderSize, 100, XmNvalue, 0, XmNpageIncrement, 1, NULL);
		return;
	}
	workWidth = workWindow -> width > 10 ? workWindow -> width : 10;
	workHeight = workWindow -> height > 10 ? workWindow -> height : 10;
	/*
	 * If the scroll bar is automatic, the slider width is set to the visible height of the work window,
	 * and the maximum is set to the entire height of the work window.
	 * If the value becomes greater than the maximum minus the slider size,
	 * the value is reduced and the work window is scrolled up (i.e. moved down).
	 */
	if (horzAutomatic) {
		int maximum = workWidth;
		int sliderSize = workWidth < clipWindow -> width ? workWidth : clipWindow -> width;
		int value = horzBar -> value;
		if (value > maximum - sliderSize) {
			value = maximum - sliderSize;
			workWindow -> x += horzBar -> value - value;
			Native_move (workWindow, horzBar -> value - value, 0);
		}
		XtVaSetValues (horzBar, XmNmaximum, maximum, XmNsliderSize, sliderSize, XmNvalue, value,
			XmNpageIncrement, clipWindow -> width - (CELL_HEIGHT - 1), NULL);
	}
	if (vertAutomatic) {   /* Automatic? */
		int maximum = workHeight;
		int sliderSize = workHeight < clipWindow -> height ? workHeight : clipWindow -> height;
		int value = vertBar -> value;
		if (value > maximum - sliderSize) {
			value = maximum - sliderSize;
			workWindow -> y += vertBar -> value - value;
			Native_move (workWindow, 0, vertBar -> value - value);
		}
		XtVaSetValues (vertBar, XmNmaximum, maximum, XmNsliderSize, sliderSize, XmNvalue, value,
			XmNpageIncrement, clipWindow -> height - (CELL_HEIGHT - 1), NULL);
	}
}

static void _motif_manage (Widget me) {
	Widget child;
	int x = 2, y = 2;
	int width = 0, height = 0, dw, dh;
	#if mac
		Widget helpMenu = NULL;
	#endif
	/*if (my widgetClass == xmScrolledWindowWidgetClass) return;   /* Ignore. */

	/*
	 * My geometry follows the layout of my children.
	 * If I am a RowColumn, I am growing while my children move.
	 * Because these moves may involve drawing, which needs clipping to my rect,
	 * I have to grow with every managed child separately,
	 * not just after managing all of them.
	 */

	for (child = my firstChild; child; child = child -> nextSibling) {
		if (child -> managed) {
			int dx = 0, dy = 0;   /* By default, the child does not move. */
			if (MEMBER (me, RowColumn)) {
				#if mac
					if (my rowColumnType == XmMENU_BAR && MEMBER (child, CascadeButton) &&
						child -> rightAttachment == XmATTACH_FORM) helpMenu = child; else
				#endif
				{
					if (x > child -> x) dx = x - child -> x;
					if (y > child -> y) dy = y - child -> y;
					child -> x += dx; x = child -> x;
					child -> y += dy; y = child -> y;
					if (my orientation != XmHORIZONTAL) {
						#if win
							y += child -> height + 1;
						#elif mac
							y += child -> height + 6;
						#endif
					} else {
						#if mac
							if (my rowColumnType == XmMENU_BAR && MEMBER (child, CascadeButton)) x += child -> width - 1; else
						#endif
						x += child -> width + 3;
					}
				}
			}
			if (child -> x + child -> width > width)
				width = child -> x + child -> width;
			if (child -> y + child -> height > height)
				height = child -> y + child -> height;
			if (MEMBER4 (me, Shell, Form, BulletinBoard, MessageBox)) {
				/* These widgets grow with their children. */
				dw = width - my width, dh = height - my height;
				if (dw < 0) dw = 0;
				if (dh < 0) dh = 0;
			} else if (MEMBER (me, RowColumn)) {
				/* A RowColumn shrinks and grows with its children. */
				dw = width - my width + 2;
				dh = height - my height + 2;
			} else {   /* ? */
				dw = width - my width;
				dh = height - my height;
			}
			my width += dw;
			my height += dh;
			#if mac
				my rect. right += dw;
				my rect. bottom += dh;
			#endif

			/*
			 * Now that I have grown, there is room for my child to move.
			 */
			if (MEMBER (me, RowColumn)) Native_move (child, dx, dy);

			/*
			 * Resize my attached other children.
			 */
			#if 0
			if (MEMBER3 (me, RowColumn, Form, ScrolledWindow)) {
				Widget child2;
				for (child2 = my firstChild; child2; child2 = child2 -> nextSibling) if (child2 != child && child2 -> managed) {
					int cdx = 0, cdy = 0, cdw = 0, cdh = 0;
					if (child2 -> widgetClass == xmShellWidgetClass) continue;
					if (child2 -> rightAttachment == XmATTACH_FORM)
						if (child2 -> leftAttachment == XmATTACH_FORM) cdw = dw; else cdx = dw;
					if (child2 -> bottomAttachment == XmATTACH_FORM)
						if (child2 -> topAttachment == XmATTACH_FORM) cdh = dh; else cdy = dh;
					if (cdx || cdy) {
						child2 -> x += cdx;
						child2 -> y += cdy;
						Native_move (child2, cdx, cdy);
					}
					if (cdw || cdh) {
						child2 -> width += cdw;
						child2 -> height += cdh;
						resizeWidget (child2, cdw, cdh);
					}
				}
			}
			#endif
		}
	}
	#if mac
		if (helpMenu) {
			int x_old = helpMenu -> x;
			my width = my parent -> width;
			my rect.right = my parent -> rect.right;
			helpMenu -> x = my width - helpMenu -> width - 2;
			/*helpMenu -> rect.left = helpMenu -> x;
			helpMenu -> rect.right = my rect.right - 6;*/
			Native_move (helpMenu, helpMenu -> x - x_old, 0);
		}
	#endif

	#if win
		if (my window) Native_move (me, 0, 0);
	#elif mac
		if (MEMBER (me, Shell)) SizeWindow (my nat.window.ptr, my width, my height, true);
	#endif

	/* If I have grown, I have to notify my parent. */

	if (! MEMBER (me, Shell)) {
		if (MEMBER5 (my parent, RowColumn, Form, BulletinBoard, Shell, MessageBox)) _motif_manage (my parent);
		else if (MEMBER (my parent, ScrolledWindow)) _motif_manageScrolledWindow (my parent);
	}
}

/***** X TOOLKIT *****/

static void xt_addCallback (XtCallbackList *callbacks, XtCallbackProc proc, XtPointer closure) {
	int i;
	for (i = 0; i < MAXNUM_CALLBACKS; i ++) {
		if (! callbacks -> pairs [i]. proc) {
			callbacks -> pairs [i]. proc = proc;
			callbacks -> pairs [i]. closure = closure;
			return;
		}
	}
	Melder_assert (i < MAXNUM_CALLBACKS);   /* Will always fail if we arrive here. */
}
static void xt_removeCallbacks (XtCallbackList *callbacks) {
	int i;
	for (i = 0; i < MAXNUM_CALLBACKS; i ++) {
		callbacks -> pairs [i]. proc = NULL;
		callbacks -> pairs [i]. closure = NULL;
	}
}

void XtAddCallback (Widget me, int kind, XtCallbackProc proc, XtPointer closure) {
	switch (kind) {
		case XmNactivateCallback:
			my activateCallback = proc; my activateClosure = closure;
		break;
		case XmNcancelCallback:
			Melder_assert (my widgetClass == xmMessageBoxWidgetClass);
			xt_addCallback (& my motif.messageBox.cancelCallbacks, proc, closure);
		break;
		case XmNdecrementCallback:
		break;
		case XmNdefaultActionCallback:
			Melder_assert (my widgetClass == xmListWidgetClass);
			my motif.list.defaultActionCallback = proc; my motif.list.defaultActionClosure = closure;
		break;
		case XmNdestroyCallback:
			my destroyCallback = proc; my destroyClosure = closure;
		break;
		case XmNdragCallback:
			Melder_assert (my widgetClass == xmScrollBarWidgetClass);
			xt_addCallback (& my motif.scrollBar.dragCallbacks, proc, closure);
		break;
		case XmNexposeCallback:
			my exposeCallback = proc; my exposeClosure = closure;
		break;
		case XmNbrowseSelectionCallback:
		case XmNextendedSelectionCallback:
		case XmNsingleSelectionCallback:
			my extendedSelectionCallback = proc; my extendedSelectionClosure = closure;
		break;
		case XmNhelpCallback:
			Melder_assert (my widgetClass == xmMessageBoxWidgetClass);
			xt_addCallback (& my motif.messageBox.helpCallbacks, proc, closure);
		break;
		case XmNincrementCallback:
		break;
		case XmNinputCallback:
			my inputCallback = proc; my inputClosure = closure;
		break;
		case XmNmotionVerifyCallback:
			Melder_assert (my widgetClass == xmTextWidgetClass);
			xt_addCallback (& my motif.text.motionVerifyCallbacks, proc, closure);
		break;
		case XmNmoveCallback:
			Melder_assert (my widgetClass == xmDrawingAreaWidgetClass);
			xt_addCallback (& my motif.drawingArea.moveCallbacks, proc, closure);
		break;
		case XmNokCallback:
			Melder_assert (my widgetClass == xmMessageBoxWidgetClass);
			xt_addCallback (& my motif.messageBox.okCallbacks, proc, closure);
		break;
		case XmNpageIncrementCallback:
		break;
		case XmNpageDecrementCallback:
		break;
		case XmNresizeCallback:
			my resizeCallback = proc; my resizeClosure = closure;
		break;
		case XmNvalueChangedCallback:
			if (my widgetClass == xmScrollBarWidgetClass)
				xt_addCallback (& my motif.scrollBar.valueChangedCallbacks, proc, closure);
			else if (my widgetClass == xmTextWidgetClass)
				xt_addCallback (& my motif.text.valueChangedCallbacks, proc, closure);
			else if (my widgetClass == xmToggleButtonWidgetClass)
				xt_addCallback (& my motif.toggleButton.valueChangedCallbacks, proc, closure);
			else Melder_assert (False);
		break;
		default:
			if (kind < 0 || kind >= sizeof motif_resourceNames / sizeof (char *))
				Melder_flushError ("(XtAddCallback:) Callback name out of range (%d).", kind);
			else
				Melder_flushError ("(XtAddCallback:) Unknown callback \"%s\".", motif_resourceNames [kind]);
	}
}

XtWorkProcId XtAppAddWorkProc (XtAppContext appContext, XtWorkProc workProc, XtPointer closure) {
	int i = 1;
	(void) appContext;
	while (i < 10 && theWorkProcs [i]) i ++;
	Melder_assert (i < 10);
	theWorkProcs [i] = workProc;
	theWorkProcClosures [i] = closure;
	theNumberOfWorkProcs ++;
	return i;
}

void XtRemoveWorkProc (XtWorkProcId id) {
	theWorkProcs [id] = NULL;
	theNumberOfWorkProcs --;
}

#if defined (macintosh)
static pascal void timerAction (EventLoopTimerRef timer, void *closure) {
	long i = (int) closure;
	(void) timer;
	theTimeOutProcs [i] (theTimeOutClosures [i], & i);
	XtRemoveTimeOut (i);
}
#endif

XtIntervalId XtAppAddTimeOut (XtAppContext appContext, unsigned long interval, XtTimerCallbackProc proc, XtPointer closure) {
	long i = 1;
	(void) appContext;
	while (i < 10 && theTimeOutProcs [i]) i ++;
	Melder_assert (i < 10);
	theTimeOutProcs [i] = proc;
#if defined (macintosh)
{
	EventLoopRef mainLoop = GetMainEventLoop ();
	static EventLoopTimerUPP timerUPP;
	if (timerUPP == NULL) timerUPP = NewEventLoopTimerUPP (timerAction);
	InstallEventLoopTimer (mainLoop, interval * kEventDurationMillisecond, 0, timerUPP, (void *) i, & theTimers [i]);
}
#else
	theTimeOutStarts [i] = clock ();
	theTimeOutIntervals [i] = (interval * (double) CLOCKS_PER_SEC) / 1000;
#endif
	theTimeOutClosures [i] = closure;
	theNumberOfTimeOuts ++;
	return i;
}

void XtRemoveTimeOut (XtIntervalId id) {
	theTimeOutProcs [id] = NULL;
	theNumberOfTimeOuts --;
	#if defined (macintosh)
		RemoveEventLoopTimer (theTimers [id]);
	#endif
}

void XtDestroyWidget (Widget me) {
	Widget subview = my firstChild;
	/*
	 * Prevent subsequent messages.
	 */
	#if win
		HWND natWindow;
		if (my window) SetWindowLong (my window, GWL_USERDATA, 0);
	#endif
	if (my widgetClass == xmShellWidgetClass) {
		XtUnmanageChild (me);
		#if win
			natWindow = my window;   /* Save for destroy. */
			my window = NULL;
		#elif mac
			my macWindow = NULL;   /* Notify children. */
		#endif
	}
	if (MEMBER3 (me, Form, BulletinBoard, MessageBox) && MEMBER (my parent, Shell) &&
		#if win
		 	my parent -> window
		#elif mac
			my parent -> macWindow
		#endif
		)
	{
		XtDestroyWidget (my parent);
		return;
	}
	while (subview) {
		Widget nextSibling = subview -> nextSibling;   /* Save... */
		if (subview -> widgetClass != xmShellWidgetClass)   /* FIX instead of Xm's being_destroyed mark. */
			XtDestroyWidget (subview);
		subview = nextSibling;   /* ...because we can't dereference dead subview. */
	}
	if (my destroyCallback) my destroyCallback (me, my destroyClosure, NULL);
	switch (my widgetClass) {
		case xmLabelWidgetClass: {
			/* XtUnmanageChild (me); // Crashes! Therefore: */
			#if win
				DestroyWindow (my window);
			#elif mac
				_GuiNativeControl_destroy (me);
			#endif
		} break;
		case xmCascadeButtonWidgetClass: {
			#if win
				if (! my inMenu && ! MEMBER (my parent, MenuBar)) _GuiNativeControl_destroy (me);
			#elif mac
				if (my isControl) _GuiNativeControl_destroy (me);
			#endif
		} break;
		case xmScaleWidgetClass: {
			#if win
				DestroyWindow (my window);
			#elif mac
				_GuiMac_clipOn (me);
				EraseRect (& my rect);
				GuiMac_clipOff ();
			#endif
		} break;
		case xmShellWidgetClass: {
			#if win
				DestroyWindow (natWindow);
			#elif mac
				DisposeWindow (my nat.window.ptr);
			#endif
			_motif_removeShell (me);
		} break;
		case xmListWidgetClass: {
			#if win
				DestroyWindow (my window);
			#elif mac
				if (my isControl) {
					_GuiNativeControl_destroy (me);
				} else {
					_GuiMac_clipOn (me);
					LDispose (my nat.list.handle);
					GuiMac_clipOff ();
				}
			#endif
		} break;
		case xmDrawingAreaWidgetClass:
		case xmRowColumnWidgetClass:
		case xmFormWidgetClass:
		case xmMessageBoxWidgetClass:
		case xmBulletinBoardWidgetClass: {
			#if win
				DestroyWindow (my window);
			#endif
		} break;
		case xmTextWidgetClass: {
			_GuiText_destroyWidget (me);
		} break;
		case xmPushButtonWidgetClass: {
			if (my inMenu) {
				#if win
					if (my nat.entry.id) theMenuItems [my nat.entry.id] = FALSE;
				#elif mac
					DeleteMenuItem (my nat.entry.handle, my nat.entry.item);
					for (subview = my parent -> firstChild; subview; subview = subview -> nextSibling) {
						if (subview -> nat.entry.item > my nat.entry.item)
							subview -> nat.entry.item -= 1;
					}
				#endif
			} else {
				if (me == my shell -> defaultButton)
					my shell -> defaultButton = NULL;   /* Remove dangling reference. */
				if (me == my shell -> cancelButton)
					my shell -> cancelButton = NULL;   /* Remove dangling reference. */
				_GuiNativeControl_destroy (me);
			}
		} break;
		case xmToggleButtonWidgetClass: {
			if (my inMenu) {
				#if win
					if (my nat.entry.id) theMenuItems [my nat.entry.id] = FALSE;
				#elif mac
					DeleteMenuItem (my nat.entry.handle, my nat.entry.item);
					for (subview = my parent -> firstChild; subview; subview = subview -> nextSibling) {
						if (subview -> nat.entry.item > my nat.entry.item)
							subview -> nat.entry.item -= 1;
					}
				#endif
			} else {
				_GuiNativeControl_destroy (me);
			}
		} break;
		case xmScrollBarWidgetClass: {
			_GuiNativeControl_destroy (me);
		} break;
		case xmScrolledWindowWidgetClass: {
			/* The scroll bars will be destroyed automatically because they are my children. */
		} break;
		case xmSeparatorWidgetClass: {
			if (my inMenu) {
				#if win
					if (my nat.entry.id) theMenuItems [my nat.entry.id] = FALSE;
				#elif mac
					DeleteMenuItem (my nat.entry.handle, my nat.entry.item);
					for (subview = my parent -> firstChild; subview; subview = subview -> nextSibling) {
						if (subview -> nat.entry.item > my nat.entry.item)
							subview -> nat.entry.item -= 1;
					}
				#endif
			}
		} break;
		case xmPulldownMenuWidgetClass: {
			#if win
				if (MEMBER (my parent, MenuBar))
					RemoveMenu (my parent -> nat.menu.handle, (UINT) my nat.menu./*handle*/id, MF_BYCOMMAND);
				DestroyMenu (my nat.menu.handle);
				theMenus [my nat.menu.id] = NULL;
			#elif mac
				DeleteMenu (my nat.menu.id);
				DisposeMenu (my nat.menu.handle);
				theMenus [my nat.menu.id] = NULL;
			#endif
		} break;
	}
	Melder_free (my name);
	if (my parent && me == my parent -> firstChild)   /* Remove dangling reference. */
		my parent -> firstChild = my nextSibling;
	if (my previousSibling)   /* Remove dangling reference. */
		my previousSibling -> nextSibling = my nextSibling;
	if (my nextSibling)   /* Remove dangling reference: two-way linked list. */
		my nextSibling -> previousSibling = my previousSibling;
	if (my parent && MEMBER (my parent, ScrolledWindow)) {
		if (me == my parent -> motif.scrolledWindow.workWindow) {
			my parent -> motif.scrolledWindow.workWindow = NULL;
			_motif_manageScrolledWindow (my parent);
		} else if (me == my parent -> motif.scrolledWindow.horizontalBar) {
			my parent -> motif.scrolledWindow.horizontalBar = NULL;
		} else if (me == my parent -> motif.scrolledWindow.verticalBar) {
			my parent -> motif.scrolledWindow.verticalBar = NULL;
		} else if (me == my parent -> motif.scrolledWindow.clipWindow) {
			my parent -> motif.scrolledWindow.clipWindow = NULL;
		}
	}
	Melder_free (me);
	numberOfWidgets --;
}

void XtFree (char *me) {
	Melder_free (me);
}

Boolean XtIsManaged (Widget me) { return my managed; }

Boolean XtIsShell (Widget me) {
	return my widgetClass == xmShellWidgetClass;
}

void XtMapWidget (Widget me) {
	switch (my widgetClass) {
		case xmShellWidgetClass:
			#if win
				ShowWindow (my window, me == theApplicationShell ? theCommandShow : theCommandShow);
				//UpdateWindow (my window);
			#elif mac
				ShowWindow (my nat.window.ptr);
			#endif
			break;
		default:
			break;
	}
}

static void mapWidget (Widget me) {
	Widget child;
	Melder_assert (my widgetClass != xmPulldownMenuWidgetClass);
	if (my inMenu) {
		#if win
			int position = NativeMenuItem_getPosition (me);
			switch (my widgetClass) {
				case xmPushButtonWidgetClass: {
					InsertMenu (my nat.entry.handle, position, MF_STRING | MF_BYPOSITION | ( my insensitive ? MF_GRAYED : MF_ENABLED ),
						my nat.entry.id, motif_win_expandAmpersands (my name));
				} break;
				case xmToggleButtonWidgetClass: {
					InsertMenu (my nat.entry.handle, position, MF_STRING | MF_UNCHECKED | MF_BYPOSITION | ( my insensitive ? MF_GRAYED : MF_ENABLED ),
						my nat.entry.id, motif_win_expandAmpersands (my name));
				} break;
				case xmCascadeButtonWidgetClass: {
					my nat.entry.id = (UINT) my subMenuId -> nat.menu.handle;
					InsertMenu (my nat.entry.handle, position, MF_POPUP | MF_BYPOSITION | ( my insensitive ? MF_GRAYED : MF_ENABLED ),
						my nat.entry.id, motif_win_expandAmpersands (my name));
				} break;
				case xmSeparatorWidgetClass: {
					InsertMenu (my nat.entry.handle, position, MF_SEPARATOR | MF_BYPOSITION, my nat.entry.id, motif_win_expandAmpersands (my name));
				} break;
			}
		#elif mac
			Widget sibling;
			int item;
			/*
			 * Insert a native menu item.
			 */
			if (my nat.entry.item) return;
			/*
			 * A Macintosh menu item shall know its "item number",
			 * which is its dynamic position in the current shape of the menu.
			 */
			/*
			 * Position of new native item.
			 */
			item = NativeMenuItem_getPosition (me);
			/*
			 * Shift all other item numbers.
			 */
			for (sibling = my parent -> firstChild; sibling; sibling = sibling -> nextSibling) {
				if (sibling -> managed && sibling -> widgetClass != xmPulldownMenuWidgetClass && sibling -> nat.entry.item >= item)
					sibling -> nat.entry.item += 1;
			}
			/*
			 * Remember.
			 */
			my nat.entry.item = item;
			/*
			 * Set text, sensitivity, submenu. BUGS: should also set toggle state and accelerator text.
			 */
			if (my widgetClass == xmSeparatorWidgetClass) {
				InsertMenuItem (my nat.entry.handle, "\p-", my nat.entry.item - 1);
			} else {
				InsertMenuItem (my nat.entry.handle, "\p ", my nat.entry.item - 1);
				SetMenuItemTextWithCFString (my nat.entry.handle, my nat.entry.item, Melder_peekWcsToCfstring (my name));
				if (my insensitive) DisableMenuItem (my nat.entry.handle, my nat.entry.item);
				if (mac_text [mac_text [0]] == ':')
					SetItemStyle (my nat.entry.handle, my nat.entry.item, underline);
				if (my widgetClass == xmCascadeButtonWidgetClass) {
					if (my subMenuId) {
						SetMenuItemHierarchicalID (my nat.entry.handle, my nat.entry.item, my subMenuId -> nat.menu.id);
					}
				}
				/*
				 * All the items in the Apple Menu folder are added to the Apple menu.
				 */
				if (my parent -> name [0] == appleMark)
					AppendResMenu (my nat.entry.handle, 'DRVR');
			}
		#endif
	} else switch (my widgetClass) {
		#if win
		case xmBulletinBoardWidgetClass:
		case xmDrawingAreaWidgetClass:
		case xmScrolledWindowWidgetClass:
		case xmFormWidgetClass:
		case xmRowColumnWidgetClass:
		case xmMessageBoxWidgetClass:
			ShowWindow (my window, SW_SHOW); break;
		#endif
		case xmShellWidgetClass: {
			#if win
				ShowWindow (my window, me == theApplicationShell ? theCommandShow : theCommandShow);
			#elif mac
				SelectWindow (my nat.window.ptr);
				ShowWindow (my nat.window.ptr);
			#endif
			if (my dialogStyle == XmDIALOG_FULL_APPLICATION_MODAL)
				#if mac
					theGui.modalDialog = my nat.window.ptr
				#endif
				;
		} break;
		case xmMenuBarWidgetClass: {
			#if win
				DrawMenuBar (my shell -> window);   // every window has its own menu bar
			#elif mac
				DrawMenuBar ();   /* There is a single Mac menu bar for all windows. */
			#endif
		} break;
		case xmPushButtonWidgetClass: _GuiNativeControl_show (me); break;
		case xmToggleButtonWidgetClass: _GuiNativeControl_show (me); break;
		case xmScrollBarWidgetClass: {
			#if win
			if (! my window) {
				my window = CreateWindow (L"scrollbar", my name, WS_CHILD |
					( my orientation == XmHORIZONTAL ? SBS_HORZ : SBS_VERT) | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
				SetWindowLong (my window, GWL_USERDATA, (long) me);
				NativeScrollBar_set (me);
			}
			#endif
			_GuiNativeControl_show (me);
		} break;
		case xmLabelWidgetClass: {
			_GuiNativeControl_show (me);
		} break;
		case xmCascadeButtonWidgetClass: {
			#if win
				if (! MEMBER (my parent, MenuBar)) _GuiNativeControl_show (me);
			#elif mac
				if (my isControl) _GuiNativeControl_show (me);
			#endif
		} break;
		case xmScaleWidgetClass: {
			#if win
				_GuiNativeControl_show (me);
			#elif mac
				_Gui_invalidateWidget (me);
			#endif
		} break;
		case xmTextWidgetClass: {
			#if win
				ShowWindow (my window, SW_SHOW);
			#elif mac
				_GuiMacText_map (me);
			#endif
		} break;
		case xmListWidgetClass: {
			#if win
				ShowWindow (my window, SW_SHOW);
			#elif mac
				if (my isControl) {
					_GuiNativeControl_show (me);
					Melder_casual ("showing a list");
					//_GuiMac_clipOn (me);
					//LSetDrawingMode (true, my nat.list.handle);
					//_motif_clipOffInvalid (me);
				} else {
					_GuiMac_clipOn (me);
					LSetDrawingMode (true, my nat.list.handle);
					_motif_clipOffInvalid (me);
				}
			#endif
		} break;
		default:
			break;
	}
	for (child = my firstChild; child != NULL; child = child -> nextSibling)
		if (child -> managed) mapWidget (child);
}

void XtManageChild (Widget me) {
	if (my managed) return;

	if (MEMBER (me, ScrolledWindow)) {
		XtManageChild (my motif.scrolledWindow.horizontalBar);
		XtManageChild (my motif.scrolledWindow.verticalBar);
		/*XtManageChild (my motif.scrolledWindow.clipWindow);*/
	}

	my managed = 1;

	/* Geometry management if my parent is a manager. */

	if (! MEMBER (me, Shell)) {
		if (MEMBER5 (my parent, RowColumn, Form, BulletinBoard, Shell, MessageBox)) _motif_manage (my parent);
		if (MEMBER (me, ScrolledWindow)) _motif_manageScrolledWindow (me);
		if (MEMBER (my parent, ScrolledWindow)) _motif_manageScrolledWindow (my parent);
	}

	/* Map to the screen (suppose that mapped_when_managed is true). */
	/* Condition: the entire up chain has been managed. */
	/* Shells or their immediate manager children can be mapped directly. */

	if (my parent && MEMBER (my parent, Shell) && MEMBER3 (me, Form, BulletinBoard, MessageBox)) {
		my parent -> managed = 1;
		mapWidget (my parent);
	} else if (my inMenu) {
		mapWidget (me);
	} else {
		int visible = True;
		Widget widget;
		for (widget = me; widget != NULL; widget = widget -> parent) {
			if (! widget -> managed &&   /* If a parent is invisible, so are its children. */
					! MEMBER (widget, PulldownMenu))   /* The exception: "shown" even if not popped up. */
				{ visible = False; break; }
			if (MEMBER (widget, Shell)) break;   /* Root: end of chain. */
		}
		if (visible) mapWidget (me);
	}

	/* I may have been created by XmCreateScrolledText or XmCreateScrolledList. */
	/* In that case, my parent should be managed. */

	if (MEMBER2 (me, Text, List) && MEMBER (my parent, ScrolledWindow)) XtManageChild (my parent);
}

void XtManageChildren (WidgetList children, Cardinal num_children) {
	Cardinal i;
	for (i = 0; i < num_children; i ++) XtManageChild (children [i]);
}

Widget XtParent (Widget me) {
	return my parent;
}

void XtRemoveCallback (Widget w, int kind, XtCallbackProc proc, XtPointer closure) {
	(void) proc;
	(void) closure;
	XtRemoveAllCallbacks (w, kind);
}

void XtRemoveCallbacks (Widget w, int kind, XtCallbackProc proc) {
	(void) proc;
	XtRemoveAllCallbacks (w, kind);
}

void XtRemoveAllCallbacks (Widget me, int kind) {
	switch (kind) {
		case XmNactivateCallback: my activateCallback = NULL; break;
		case XmNdestroyCallback: my destroyCallback = NULL; break;
		case XmNdragCallback: xt_removeCallbacks (& my motif.scrollBar.dragCallbacks); break;
		case XmNexposeCallback: my exposeCallback = NULL; break;
		case XmNextendedSelectionCallback: my extendedSelectionCallback = NULL; break;
		case XmNinputCallback: my inputCallback = NULL; break;
		case XmNresizeCallback: my resizeCallback = NULL; break;
		case XmNvalueChangedCallback: xt_removeCallbacks (& my motif.scrollBar.valueChangedCallbacks); break;
		default:
			if (kind < 0 || kind >= sizeof motif_resourceNames / sizeof (char *))
				Melder_flushError ("(XtRemoveCallback:) Callback name out of range (%d).", kind);
			else
				Melder_flushError ("(XtRemoveCallback:) Unknown callback \"%s\".", motif_resourceNames [kind]);
	}
}

void XtSetSensitive (Widget me, Boolean value) {
	if (my insensitive != value) return;
	my insensitive = ! value;
	if (! my parent) return;
	switch (my widgetClass) {
		case xmPushButtonWidgetClass:
		case xmToggleButtonWidgetClass: {
			if (my inMenu) {
				NativeMenuItem_setSensitive (me);
			} else {
				_GuiNativeControl_setSensitive (me);
			}
		} break;
		case xmScrollBarWidgetClass: _GuiNativeControl_setSensitive (me); break;
		case xmLabelWidgetClass: {
			_GuiNativeControl_setSensitive (me);
		} break;
		case xmCascadeButtonWidgetClass: {
			if (my inMenu || my motif.cascadeButton.inBar) {
				if (my subMenuId) {
					if (value) {
						#if mac
							EnableMenuItem (my subMenuId -> nat.menu.handle, 0);
						#endif
					} else {
						#if mac
							DisableMenuItem (my subMenuId -> nat.menu.handle, 0);
						#endif
					}
					#if mac
						DrawMenuBar ();
					#endif
				}
			} else {
				_GuiNativeControl_setSensitive (me);
			}
		} break;
		case xmPulldownMenuWidgetClass: {
			if (my popUpButton)
				XtSetSensitive (my popUpButton, value);
		} break;
		default:
			break;
	}
}

void XtUnmanageChild (Widget me) {
	if (! my managed) return;
	if (my inMenu) {
		if (! MEMBER (me, PulldownMenu)) NativeMenuItem_delete (me);
	} else switch (my widgetClass) {
		case xmShellWidgetClass:
			#if win
				_GuiText_handleFocusLoss (my textFocus);
				ShowWindow (my window, SW_HIDE);
			#elif mac
				if (my nat.window.ptr == FrontWindow ())
					_GuiMac_clearTheTextFocus ();
				if (my nat.window.ptr == theGui.modalDialog)
					theGui.modalDialog = NULL;
				HideWindow (my nat.window.ptr);
			#endif
			if (my firstChild && MEMBER3 (my firstChild, Form, BulletinBoard, MessageBox))
				my firstChild -> managed = 0;
			break;
		case xmPushButtonWidgetClass: _GuiNativeControl_hide (me); break;
		case xmToggleButtonWidgetClass: _GuiNativeControl_hide (me); break;
		case xmLabelWidgetClass:
			_GuiNativeControl_hide (me);
			break;
		case xmCascadeButtonWidgetClass:
			#if win
				if (! MEMBER (my parent, MenuBar)) _GuiNativeControl_hide (me);
			#elif mac
				if (my isControl) _GuiNativeControl_hide (me);
			#endif
			break;
		case xmScrollBarWidgetClass: _GuiNativeControl_hide (me); break;
		case xmFormWidgetClass:
		case xmBulletinBoardWidgetClass:
		case xmMessageBoxWidgetClass:
			if (MEMBER (my parent, Shell)) XtUnmanageChild (my parent);
			break;
		case xmTextWidgetClass: {
			_GuiText_unmanage (me);
		} break;
		default:
			_Gui_invalidateWidget (me);
			break;
	}

	my managed = 0;

	if (! MEMBER (me, Shell)) {
		if (MEMBER5 (my parent, RowColumn, Form, BulletinBoard, Shell, MessageBox)) _motif_manage (my parent);
		else if (MEMBER (my parent, ScrolledWindow)) _motif_manageScrolledWindow (my parent);
	}
}

void XtUnmanageChildren (WidgetList children, Cardinal num_children) {
	Cardinal i;
	for (i = 0; i < num_children; i ++) XtUnmanageChild (children [i]);
}

#if mac
	static pascal OSErr _motif_processOpenApplicationMessage (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon) {
		(void) theAppleEvent;
		(void) reply;
		(void) handlerRefCon;
		return noErr;
		Melder_warning ("Open app event.");
	}
	static pascal OSErr _motif_processQuitApplicationMessage (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon) {
		/*
			This hook shall return noErr if it is OK to quit,
			otherwise userCanceledErr. It should not exit the application by itself. Here it does!!
		*/
		(void) theAppleEvent;
		(void) reply;
		(void) handlerRefCon;
		if (theQuitApplicationCallback)
			return theQuitApplicationCallback () ? noErr : userCanceledErr;   /* BUG anti Mac documentation */
		exit (0);
		return noErr;
	}
	static pascal OSErr _motif_processOpenDocumentsMessage (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon) {
		AEDescList documentList;
		OSErr err;
		long document, numberOfDocuments;
		(void) reply;
		(void) handlerRefCon;
		err = AEGetParamDesc (theAppleEvent, keyDirectObject, typeAEList, & documentList);
		if (err) return err;
		err = AECountItems (& documentList, & numberOfDocuments);
		for (document = 1; document <= numberOfDocuments; document ++ ) {
			AEKeyword keyWord;
			DescType typeCode;
			Size actualSize;
			FSRef fsref;
			err = AEGetNthPtr (& documentList, document, typeFSRef, & keyWord, & typeCode, & fsref, sizeof (fsref), & actualSize);
			if (! err) {
				structMelderFile file = { 0 };
				Melder_machToFile (& fsref, & file);
				if (theOpenDocumentCallback)
					theOpenDocumentCallback (& file);
			}
		}
		AEDisposeDesc (& documentList);
		return noErr;
	}
	static pascal OSErr _motif_processSignalA (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon) {
		static int duringAppleEvent = FALSE;
		(void) reply;
		(void) handlerRefCon;
		if (! duringAppleEvent) {
			char *buffer;
			long actualSize;
			duringAppleEvent = TRUE;
			AEInteractWithUser (kNoTimeOut, NULL, NULL);
			AEGetParamPtr (theAppleEvent, 1, typeChar, NULL, NULL, 0, & actualSize);
			buffer = malloc (actualSize);
			AEGetParamPtr (theAppleEvent, 1, typeChar, NULL, & buffer [0], actualSize, NULL);
			if (theUserMessageCallbackA)
				theUserMessageCallbackA (buffer);
			free (buffer);
			duringAppleEvent = FALSE;
		}
		return noErr;
	}
	static pascal OSErr _motif_processSignalW (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon) {
		static int duringAppleEvent = FALSE;
		(void) reply;
		(void) handlerRefCon;
		if (! duringAppleEvent) {
			wchar_t *buffer;
			long actualSize;
			duringAppleEvent = TRUE;
			AEInteractWithUser (kNoTimeOut, NULL, NULL);
			AEGetParamPtr (theAppleEvent, 1, typeUnicodeText, NULL, NULL, 0, & actualSize);
			buffer = malloc (actualSize);
			AEGetParamPtr (theAppleEvent, 1, typeUnicodeText, NULL, & buffer [0], actualSize, NULL);
			if (theUserMessageCallbackW)
				theUserMessageCallbackW (buffer);
			free (buffer);
			duringAppleEvent = FALSE;
		}
		return noErr;
	}
#elif win
	static LRESULT CALLBACK windowProc (HWND window, UINT message, WPARAM wParam, LPARAM lParam);
#endif

Widget XtInitialize (void *dum1, const char *name,
	void *dum2, int dum3, unsigned int *argc, char **argv)
{
	(void) dum1;
	(void) dum2;
	(void) dum3;
	(void) argc;
	#if mac
		(void) argv;
		_GuiText_init ();
		RegisterAppearanceClient ();
		InitCursor ();
		FlushEvents (everyEvent, 0);
		AEInstallEventHandler (kCoreEventClass, kAEOpenApplication, NewAEEventHandlerUPP (_motif_processOpenApplicationMessage), 0, false);
		AEInstallEventHandler (kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP (_motif_processQuitApplicationMessage), 0, false);
		AEInstallEventHandler (kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerUPP (_motif_processOpenDocumentsMessage), 0, false);
		AEInstallEventHandler (758934755, 0, NewAEEventHandlerUPP (_motif_processSignalA), 0, false);
		AEInstallEventHandler (758934756, 0, NewAEEventHandlerUPP (_motif_processSignalW), 0, false);
		if (Melder_systemVersion >= 0x0800) USE_QUESTION_MARK_HELP_MENU = 1;
		theUserFocusEventTarget = GetUserFocusEventTarget ();
	#elif win
	{
		HWND window;
		WNDCLASSEX windowClass;
		swprintf (theApplicationName, 100, argv [0] ? Melder_peekUtf8ToWcs (argv [0]) : L"Unknown");
		swprintf (theApplicationClassName, 100, L"PraatShell%d %ls", PRAAT_WINDOW_CLASS_NUMBER, theApplicationName);
		swprintf (theWindowClassName, 100, L"PraatChildWindow%d %ls", PRAAT_WINDOW_CLASS_NUMBER, theApplicationName);
		swprintf (theDrawingAreaClassName, 100, L"PraatDrawingArea%d %ls", PRAAT_WINDOW_CLASS_NUMBER, theApplicationName);
		window = FindWindow (theWindowClassName, NULL);
		if (window != NULL) {
			/*
			 * We are in the second instance of Praat.
			 * The user double-clicked Praat while it was running,
			 * or she dropped a file on the Praat icon,
			 * or she double-clicked a Praat file.
			 */
			if (IsIconic (window)) ShowWindow (window, SW_RESTORE);
			SetForegroundWindow (window);
			if (theOpenDocumentCallback && argv [3] [0]) {
				structMelderFile file = { 0 };
				/*
				 * The user dropped a file on the Praat icon or double-clicked a Praat file
				 * while Praat was already running.
				 * Windows may have enclosed the path between quotes;
				 * this is especially likely to happen if the path contains spaces,
				 * which on Windows XP is very usual.
				 */
				Melder_relativePathToFile (Melder_peekUtf8ToWcs (argv [3] [0] == '\"' ? argv [3] + 1 : argv [3]), & file);
				if (wcslen (file. path) > 0 && file. path [wcslen (file. path) - 1] == '\"') {
					file. path [wcslen (file. path) - 1] = '\0';
				}
				theOpenDocumentCallback (& file);
			}
			exit (0);   // possible problem
		}

		theGui.instance = (HINSTANCE) atol (argv [1]);

		windowClass. cbSize = sizeof (WNDCLASSEX);
		windowClass. style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS |
			CS_OWNDC   // crucial: see GraphicsScreen_init ()
			;
		windowClass. lpfnWndProc = windowProc;
		windowClass. cbClsExtra = 0;
		windowClass. cbWndExtra = 0;
		windowClass. hInstance = theGui.instance;
		windowClass. hIcon = NULL;
		windowClass. hCursor = LoadCursor (NULL, IDC_ARROW);
		windowClass. hbrBackground = /*(HBRUSH) (COLOR_WINDOW + 1)*/ GetStockBrush (LTGRAY_BRUSH);
		windowClass. lpszMenuName = NULL;
		windowClass. lpszClassName = theWindowClassName;
		windowClass. hIconSm = NULL;
		RegisterClassEx (& windowClass);
		windowClass. hbrBackground = GetStockBrush (WHITE_BRUSH);
		windowClass. lpszClassName = theDrawingAreaClassName;
		RegisterClassEx (& windowClass);
		windowClass. lpszClassName = theApplicationClassName;
		RegisterClassEx (& windowClass);
		InitCommonControls ();
		theCommandShow = atoi (argv [2]);
	}
	#endif
	return theApplicationShell = XmCreateShell (NULL, name, NULL, 0);
}

Widget XtVaAppInitialize (XtAppContext *dum1, const char *name,
	void *dum2, int dum3, unsigned int *argc, char **argv, void *dum4, void *dum5)
{
	(void) dum1;
	(void) dum4;
	(void) dum5;
	return XtInitialize (0, name, dum2, dum3, argc, argv);
}

Widget XtVaCreateManagedWidget (const char *name, int widgetClass, Widget parent, ...) {
	Widget me;
	va_list arg;
	va_start (arg, parent);
	me = vaCreateWidget (widgetClass, parent, name, arg);
	_motif_setValues (me, arg);
	va_end (arg);
	XtManageChild (me);
	return me;
}

Widget XtVaCreateWidget (const char *name, int widgetClass, Widget parent, ...) {
	Widget me;
	va_list arg;
	va_start (arg, parent);
	me = vaCreateWidget (widgetClass, parent, name, arg);
	_motif_setValues (me, arg);
	va_end (arg);
	return me;
}

void XtVaGetValues (Widget me, ...) {
	char *text;
	#if mac
		Str255 ptext;
	#endif
	int resource;
	va_list arg;
	va_start (arg, me);
	while ((resource = va_arg (arg, int)) != 0) switch (resource) {
		case XmNx: *va_arg (arg, int *) = my x; break;
		case XmNy: *va_arg (arg, int *) = my y; break;
		case XmNwidth: *va_arg (arg, int *) = my width; break;
		case XmNheight: *va_arg (arg, int *) = my height; break;
		case XmNtitle:
		case XmNiconName:
			Melder_assert (my widgetClass == xmShellWidgetClass);
			#if mac
				GetWTitle (my nat.window.ptr, ptext);
				text = Melder_malloc (char, ptext [0] + 1);
				strncpy (text, (char *) ptext + 1, ptext [0]);
				text [ptext [0]] = 0;
				*va_arg (arg, char **) = text;
			#elif win
				*va_arg (arg, char **) = NULL;
			#endif
			break;
		case XmNlabelString:
		case XmNtitleString:
			Melder_assert (my widgetClass == xmLabelWidgetClass || my widgetClass == xmCascadeButtonWidgetClass ||
								my widgetClass == xmScaleWidgetClass);
			text = Melder_wcsToUtf8 (my name);
			*va_arg (arg, char **) = text;
			break;
		case XmNdialogTitle:
			Melder_assert (my widgetClass == xmFormWidgetClass ||
								my widgetClass == xmBulletinBoardWidgetClass ||
								my widgetClass == xmMessageBoxWidgetClass);
			#if mac
				GetWTitle (my macWindow, ptext);
				text = Melder_malloc (char, ptext [0] + 1);
				strncpy (text, (char *) ptext + 1, ptext [0]);
				text [ptext [0]] = 0;
				*va_arg (arg, char **) = text;
			#elif win
				*va_arg (arg, char **) = NULL;
			#endif
			break;
		case XmNnumChildren: {
			Cardinal numChildren = 0;
			Widget child;
			for (child = my firstChild; child != NULL; child = child -> nextSibling) numChildren ++;
			*va_arg (arg, Cardinal *) = numChildren;
			break; }
		case XmNtopItemPosition: {
			int d, n;
			Melder_assert (my widgetClass == xmListWidgetClass);
			#if win
			d = my parent -> widgetClass == xmScrolledWindowWidgetClass ?
				(my parent -> motif.scrolledWindow.clipWindow -> y -
				 my parent -> motif.scrolledWindow.workWindow -> y + 5) / CELL_HEIGHT + 1 : 1;
			#elif mac
			d = my parent -> widgetClass == xmScrolledWindowWidgetClass ?
				(my parent -> motif.scrolledWindow.clipWindow -> rect.top -
				 my parent -> motif.scrolledWindow.workWindow -> rect.top + 5) / CELL_HEIGHT + 1 : 1;   /* Different? */
			#endif
			if (d < 1) d = 1;
			n =
				#if mac
					(** my nat.list.handle). dataBounds. bottom
				#elif win
					0
				#endif
				;
			if (d > n) d = 0;
			*va_arg (arg, int *) = d;
			break; }
 		case XmNitemCount:
			Melder_assert (my widgetClass == xmListWidgetClass);
			*va_arg (arg, int *) =
				#if mac
					(** my nat.list.handle). dataBounds. bottom
				#elif win
					0
				#endif
				;
			break;
  		case XmNvisibleItemCount:
			Melder_assert (my widgetClass == xmListWidgetClass);
			*va_arg (arg, int *) = my visibleItemCount;
			break;
		case XmNselectionPolicy:
			Melder_assert (my widgetClass == xmListWidgetClass);
			*va_arg (arg, int *) = my selectionPolicy;
			break;
		case XmNradioBehavior:
			Melder_assert (my widgetClass == xmRowColumnWidgetClass);
			*va_arg (arg, int *) = my radioBehavior;
			break;
		case XmNautoUnmanage:
			*va_arg (arg, int *) = my autoUnmanage;
			break;
		case XmNorientation:
			Melder_assert (my widgetClass == xmRowColumnWidgetClass ||
								my widgetClass == xmScrollBarWidgetClass);
			*va_arg (arg, int *) = my orientation;
			break;
		case XmNpacking:
			Melder_assert (my widgetClass == xmRowColumnWidgetClass);
			*va_arg (arg, int *) = my packing;
			break;
		case XmNrowColumnType:
			Melder_assert (my widgetClass == xmRowColumnWidgetClass);
			*va_arg (arg, int *) = my rowColumnType;
			break;
		case XmNentryAlignment:
			Melder_assert (my widgetClass == xmRowColumnWidgetClass);
			*va_arg (arg, int *) = 0;
			break;
		case XmNdefaultButton:
			Melder_assert (my widgetClass == xmFormWidgetClass ||
								my widgetClass == xmBulletinBoardWidgetClass ||
								my widgetClass == xmMessageBoxWidgetClass);
			*va_arg (arg, Widget *) = my defaultButton;
			break;
		case XmNcancelButton:
			Melder_assert (my widgetClass == xmFormWidgetClass ||
								my widgetClass == xmBulletinBoardWidgetClass ||
								my widgetClass == xmMessageBoxWidgetClass);
			*va_arg (arg, Widget *) = my cancelButton;
			break;
		case XmNsubMenuId:
			Melder_assert (my widgetClass == xmCascadeButtonWidgetClass);
			*va_arg (arg, Widget *) = my subMenuId;
			break;
		case XmNdialogStyle:
			Melder_assert (my widgetClass == xmFormWidgetClass ||
								my widgetClass == xmBulletinBoardWidgetClass);
			*va_arg (arg, int *) = my dialogStyle;
			break;
		case XmNdialogType:
			Melder_assert (my widgetClass == xmMessageBoxWidgetClass);
			*va_arg (arg, int *) = my dialogType;
			break;
		case XmNleftAttachment:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my leftAttachment;
			break;
		case XmNrightAttachment:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my rightAttachment;
			break;
		case XmNtopAttachment:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my topAttachment;
			break;
		case XmNbottomAttachment:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my bottomAttachment;
			break;
		case XmNleftOffset:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my leftOffset;
			break;
		case XmNrightOffset:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my rightOffset;
			break;
		case XmNtopOffset:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my topOffset;
			break;
		case XmNbottomOffset:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my bottomOffset;
			break;
		case XmNminimum: *va_arg (arg, int *) = my minimum; break;
		case XmNmaximum: *va_arg (arg, int *) = my maximum; break;
		case XmNvalue: *va_arg (arg, int *) = my value; break;
		case XmNincrement:
			Melder_assert (my widgetClass == xmScrollBarWidgetClass);
			*va_arg (arg, int *) = my increment;
			break;
		case XmNpageIncrement:
			Melder_assert (my widgetClass == xmScrollBarWidgetClass);
			*va_arg (arg, int *) = my pageIncrement;
			break;
		case XmNsliderSize:
			Melder_assert (my widgetClass == xmScrollBarWidgetClass);
			*va_arg (arg, int *) = my sliderSize;
			break;
		case XmNmarginWidth: *va_arg (arg, int *) = 0; break;
		case XmNmarginHeight:
			*va_arg (arg, int *) = 0;
			break;
		case XmNdeleteResponse:
			Melder_assert (my widgetClass == xmShellWidgetClass);
			*va_arg (arg, int *) = my deleteResponse;
			break;
		case XmNcolumns: *va_arg (arg, int *) = 40; break;
		case XmNhorizontalScrollBar: *va_arg (arg, Widget *) = my motif.scrolledWindow.horizontalBar; break;
		case XmNverticalScrollBar: *va_arg (arg, Widget *) = my motif.scrolledWindow.verticalBar; break;
		default: {
			if (resource < 0 || resource >= sizeof motif_resourceNames / sizeof (char *))
				Melder_flushError ("(XtVaGetValues:) Resource out of range (%d).", resource);
			else
				Melder_flushError ("(XtVaGetValues:) Unknown resource \"%s\".",
					motif_resourceNames [resource]);
			return;
		}
	}
	va_end (arg);
}

void XtVaSetValues (Widget me, ...) {
	va_list arg;
	va_start (arg, me);
	_motif_setValues (me, arg);
	va_end (arg);
}

Window XtWindow (Widget me) {
	#if mac
		return (Window) my macWindow;
	#elif win
		return (Window) my window;
	#endif
}

/***** MOTIF *****/

void XmAddWMProtocolCallback (Widget me, Atom protocol, XtCallbackProc callback, char *closure) {
	if (protocol == 'delw') {
		my motif.shell.goAwayCallback = callback;
		my motif.shell.goAwayClosure = closure;
		#if mac
			ChangeWindowAttributes (my macWindow, kWindowCloseBoxAttribute, 0);
		#endif
	}
}

void XmAddWMProtocols (Widget shell, Atom *protocols, Cardinal num_protocols) {
	(void) shell;
	(void) protocols;
	(void) num_protocols;
}

Widget XmCreateBulletinBoard (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmBulletinBoardWidgetClass, parent, name);
}

Widget XmCreateBulletinBoardDialog (Widget parent, const char *name, ArgList dum1, int dum2) {
	Widget shell = XmCreateDialogShell (parent, name, dum1, dum2);
	return XmCreateBulletinBoard (shell, name, dum1, dum2);
}

Widget XmCreateCascadeButton (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmCascadeButtonWidgetClass, parent, name);
}

Widget XmCreateCascadeButtonGadget (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmCascadeButtonGadgetClass, parent, name);
}

Widget XmCreateDialogShell (Widget parent, const char *name, ArgList dum1, int dum2) {
	Widget shell;
	theDialogHint = True;
	shell = XmCreateShell (parent, name, dum1, dum2);
	theDialogHint = False;
	return shell;
}

Widget XmCreateDrawingArea (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmDrawingAreaWidgetClass, parent, name);
}

Widget XmCreateForm (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmFormWidgetClass, parent, name);
}

Widget XmCreateFormDialog (Widget parent, const char *name, ArgList dum1, int dum2) {
	Widget shell = XmCreateDialogShell (parent, name, dum1, dum2);
	return XmCreateForm (shell, name, dum1, dum2);
}

Widget XmCreateFrame (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmFrameWidgetClass, parent, name);
}

Widget XmCreateLabel (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmLabelWidgetClass, parent, name);
}

Widget XmCreateLabelGadget (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmLabelWidgetClass, parent, name);
}

Widget XmCreateList (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmListWidgetClass, parent, name);
}

Widget XmCreateMenuBar (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmMenuBarWidgetClass, parent, name);
}

Widget XmCreateMessageBox (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmMessageBoxWidgetClass, parent, name);
}

Widget XmCreateMessageDialog (Widget parent, const char *name, ArgList dum1, int dum2) {
	Widget shell = XmCreateDialogShell (parent, name, dum1, dum2), dialog;
	int screenWidth = WidthOfScreen (DefaultScreenOfDisplay (XtDisplay (parent)));
	int screenHeight = HeightOfScreen (DefaultScreenOfDisplay (XtDisplay (parent))) - 22;
	shell -> deleteResponse = XmUNMAP;
	dialog = XmCreateMessageBox (shell, name, dum1, dum2);
	XtVaSetValues (shell, XmNx, (screenWidth - 300) / 2, XmNy, (screenHeight - 150) / 3, NULL);
	return dialog;
}

Widget XmCreatePulldownMenu (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmPulldownMenuWidgetClass, parent, name);
}

Widget XmCreatePushButton (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmPushButtonWidgetClass, parent, name);
}

Widget XmCreatePushButtonGadget (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmPushButtonWidgetClass, parent, name);
}

Widget XmCreateRadioBox (Widget parent, const char *name, ArgList dum1, int dum2) {
	Widget result = createWidget (xmRowColumnWidgetClass, parent, name);
	(void) dum1;
	(void) dum2;
	XtVaSetValues (result, XmNradioBehavior, True, NULL);
	return result;
}

Widget XmCreateRowColumn (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmRowColumnWidgetClass, parent, name);
}

Widget XmCreateScale (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmScaleWidgetClass, parent, name);
}

Widget XmCreateScrollBar (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmScrollBarWidgetClass, parent, name);
}

Widget XmCreateScrolledList (Widget parent, const char *name, ArgList dum1, int dum2) {
	Widget scrolled = createWidget (xmScrolledWindowWidgetClass, parent, name);
	(void) dum1;
	(void) dum2;
	return createWidget (xmListWidgetClass, scrolled, name);
}

Widget XmCreateScrolledWindow (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmScrolledWindowWidgetClass, parent, name);
}

Widget XmCreateSeparator (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmSeparatorWidgetClass, parent, name);
}

Widget XmCreateSeparatorGadget (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmSeparatorWidgetClass, parent, name);
}

Widget XmCreateShell (Widget parent, const char *name, ArgList dum1, int dum2) {
	Widget me = createWidget (xmShellWidgetClass, parent, name);
	(void) dum1;
	(void) dum2;
	_motif_addShell (me);
	return me;
}

Widget XmCreateToggleButton (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmToggleButtonWidgetClass, parent, name);
}

Widget XmCreateToggleButtonGadget (Widget parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmToggleButtonWidgetClass, parent, name);
}

Atom XmInternAtom (Display *display, String name, Boolean only_if_exists) {
	(void) display;
	(void) only_if_exists;
	if (strequ (name, "WM_DELETE_WINDOW")) return 'delw';
	return 0;
}

void XmListAddItem (Widget me, XmString item, int position) {
	#if win
		if (position)
			ListBox_InsertString (my window, position - 1, Melder_peekUtf8ToWcs (item));
		else
			ListBox_AddString (my window, Melder_peekUtf8ToWcs (item));
	#elif mac
		Cell cell;
		int n = (** my nat.list.handle). dataBounds. bottom;
		if (position == 0)
			position = n + 1;   /* At end. */
		cell.h = 0; cell. v = position - 1;
		_GuiMac_clipOn (me);
		LAddRow (1, position - 1, my nat.list.handle);
		LSetCell (item, (short) strlen (item), cell, my nat.list.handle);
		(** my nat.list.handle). visible. bottom = n + 1;
		_motif_clipOffInvalid (me);
		XtVaSetValues (me, XmNheight, (n + 1) * CELL_HEIGHT, NULL);
	#endif
}

void XmListAddItems (Widget me, XmString *items, int n, int position) {
	#if win
		int i;
		if (position)
			for (i = 0; i < n; i ++)
				ListBox_InsertString (my window, (position ++) - 1, Melder_peekUtf8ToWcs (items [i]));
		else
			for (i = 0; i < n; i ++)
				ListBox_AddString (my window, Melder_peekUtf8ToWcs (items [i]));
	#elif mac
		int i, there = (** my nat.list.handle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		_GuiMac_clipOn (me);
		if (position == 0)
			position = there + 1;   /* At end. */
		LAddRow (n, position - 1, my nat.list.handle);
		for (i = 0; i < n; i ++) {
			cell.v = position - 1 + i;
			LSetCell (items [i], strlen (items [i]), cell, my nat.list.handle);
			LDraw (cell, my nat.list.handle);
		}
		GuiMac_clipOff ();
	#endif
}

void XmListAddItemUnselected (Widget me, XmString item, int pos) {
	XmListAddItem (me, item, pos);
}

void XmListAddItemsUnselected (Widget me, XmString *items, int n, int pos) {
	XmListAddItems (me, items, n, pos);
}

void XmListDeleteAllItems (Widget me) {
	#if win
		ListBox_ResetContent (my window);
	#elif mac
		_GuiMac_clipOn (me);
		LDelRow (0, 0, my nat.list.handle);
		GuiMac_clipOff ();
	#endif
}

void XmListDeleteItem (Widget me, XmString item) {
	#if win
		int i, n = ListBox_GetCount (my window);
		for (i = n - 1; i >= 0; i --) {
			wchar_t buffer [301];
			ListBox_GetText (my window, i, buffer);
			if (wcsequ (buffer, Melder_peekUtf8ToWcs (item))) ListBox_DeleteString (my window, i);
		}
	#elif mac
		int i, n = (** my nat.list.handle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		_GuiMac_clipOn (me);
		for (i = n - 1; i >= 0; i --) {
			char buffer [301];
			short int length = 300;
			cell.v = i;
			LGetCell (buffer, & length, cell, my nat.list.handle); buffer [length] = '\0';
			if (strequ (buffer, item)) LDelRow (1, i, my nat.list.handle);
		}
		GuiMac_clipOff ();
	#endif
}

void XmListDeleteItems (Widget me, XmString *items, int n) {
	int i;
	for (i = 0; i < n; i ++) XmListDeleteItem (me, items [i]);
}

void XmListDeleteItemsPos (Widget me, int item_count, int position) {
	#if win
		int i;
		for (i = position + item_count - 2; i >= position - 1; i --)
			ListBox_DeleteString (my window, i);
	#elif mac
		_GuiMac_clipOn (me);
		LDelRow (item_count, position - 1, my nat.list.handle);
		GuiMac_clipOff ();
	#endif
}

void XmListDeletePos (Widget me, int position) {
	#if win
		ListBox_DeleteString (my window, position - 1);
	#elif mac
		_GuiMac_clipOn (me);
		LDelRow (1, position - 1, my nat.list.handle);
		GuiMac_clipOff ();
	#endif
}

void XmListDeselectAllItems (Widget me) {
	#if win
		ListBox_SetSel (my window, False, -1);
	#elif mac
		int i, n = (** my nat.list.handle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		_GuiMac_clipOn (me);
		for (i = 0; i < n; i ++) { cell.v = i; LSetSelect (false, cell, my nat.list.handle); }
		GuiMac_clipOff ();
	#endif
}

void XmListDeselectItem (Widget me, XmString item) {
	#if win
		int i, n = ListBox_GetCount (my window);
		for (i = n - 1; i >= 0; i --) {
			wchar_t buffer [301];
			ListBox_GetText (my window, i, buffer);
			if (wcsequ (buffer, Melder_peekUtf8ToWcs (item))) ListBox_SetSel (my window, False, i);
		}
	#elif mac
		int i, n = (** my nat.list.handle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		_GuiMac_clipOn (me);
		for (i = n - 1; i >= 0; i --) {
			char buffer [301];
			short int length = 300;
			cell.v = i;
			LGetCell (buffer, & length, cell, my nat.list.handle); buffer [length] = '\0';
			if (strequ (buffer, item)) LSetSelect (false, cell, my nat.list.handle);
		}
		GuiMac_clipOff ();
	#endif
}

void XmListDeselectPos (Widget me, int position) {
	#if win
		ListBox_SetSel (my window, False, position - 1);
	#elif mac
		Cell cell;
		cell. h = 0;
		cell. v = position - 1; 
		_GuiMac_clipOn (me);
		LSetSelect (false, cell, my nat.list.handle);
		GuiMac_clipOff ();
	#endif
}

Boolean XmListGetMatchPos (Widget me, XmString item, int **position_list, int *position_count) {
	#if win
		int i, n = ListBox_GetCount (my window), count = 0;
		if (n < 1) return False;
		*position_list = Melder_calloc (int, n);
		*position_count = 0;
		for (i = 1; i <= n; i ++) {
			wchar_t buffer [301];
			ListBox_GetText (my window, i - 1, buffer);
			if (wcsequ (buffer, Melder_peekUtf8ToWcs (item))) {
				(*position_count) ++;
				(*position_list) [*position_count - 1] = i;
			}
		}   
		if (*position_count == 0) { Melder_free (*position_list); return False; }
		return True;
	#elif mac
		int i, n = (** my nat.list.handle). dataBounds. bottom;
		Cell cell;
		cell.h = 0;
		if (n < 1) return False;
		*position_list = Melder_calloc (int, n);
		*position_count = 0;
		for (i = 1; i <= n; i ++) {
			char buffer [301];
			short int length = 300;
			cell. v = i - 1;
			LGetCell (buffer, & length, cell, my nat.list.handle); buffer [length] = '\0';
			if (strequ (buffer, item)) {
				(*position_count) ++;
				(*position_list) [*position_count - 1] = i;
			}
		}   
		if (*position_count == 0) { Melder_free (*position_list); return False; }
		return True;
	#endif
}

Boolean XmListGetSelectedPos (Widget me, int **position_list, int *position_count) {
	#if win
		int i, n = ListBox_GetSelCount (my window), *indices;
		if (n == 0) return False;
		if (n == -1) {   // single selection
			int selection = ListBox_GetCurSel (my window);
			if (selection == -1) return False;
			n = 1;
			indices = Melder_calloc (int, n);
			indices [0] = selection;
		} else {
			indices = Melder_calloc (int, n);
			ListBox_GetSelItems (my window, n, indices);
		}
		for (i = 0; i < n; i ++) indices [i] += 1;
		*position_count = n;
		*position_list = indices;
		return True;
	#elif mac
		int i, n = (** my nat.list.handle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		if (n < 1) return False;
		*position_list = Melder_calloc (int, n);
		*position_count = 0;
		for (i = 1; i <= n; i ++) {
			cell. v = i - 1;
			if (LGetSelect (false, & cell, my nat.list.handle)) {
				(*position_count) ++;
				(*position_list) [*position_count - 1] = i;
			}
		}   
		if (*position_count == 0) { Melder_free (*position_list); return False; }
		return True;
	#endif
}

Boolean XmListItemExists (Widget me, XmString item) {
	return XmListItemPos (me, item) != 0;
}

int XmListItemPos (Widget me, XmString item) {
	#if win
		int winPos = ListBox_FindStringExact (my window, 0, Melder_peekUtf8ToWcs (item));
		return winPos < 0 ? 0 : winPos + 1;
	#elif mac
		int i, n = (** my nat.list.handle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		for (i = 1; i <= n; i ++) {
			char buffer [301];
			short int length = 300;
			cell. v = i - 1;
			LGetCell (buffer, & length, cell, my nat.list.handle); buffer [length] = '\0';
			if (strequ (buffer, item)) return i;   /* First instance found. */
		}
	#endif
	return 0;   /* Not found. */
}

void XmListReplaceItemsPos (Widget me, XmString *new_items, int item_count, int position) {
	#if win
		int i;
		for (i = 0; i < item_count; i ++) {
			int winPos = position - 1 + i;
			ListBox_DeleteString (my window, winPos);
			ListBox_InsertString (my window, winPos, Melder_peekUtf8ToWcs (new_items [i]));
		}
	#elif mac
		int i;
		Cell cell; cell.h = 0;
		_GuiMac_clipOn (me);
		for (i = 0; i < item_count; i ++) {
			cell.v = position - 1 + i;
			LSetCell (new_items [i], strlen (new_items [i]), cell, my nat.list.handle);
			LDraw (cell, my nat.list.handle);      
		}
		GuiMac_clipOff ();
	#endif
}

void XmListReplaceItemsPosUnselected (Widget me, XmString *new_items, int item_count, int position) {
	XmListReplaceItemsPos (me, new_items, item_count, position);
}

void XmListSelectItem (Widget me, XmString item, Boolean notify) {
	#if win
		int i, n = ListBox_GetCount (my window);
		for (i = n - 1; i >= 0; i --) {
			wchar_t buffer [301];
			ListBox_GetText (my window, i, buffer);
			if (wcsequ (buffer, Melder_peekUtf8ToWcs (item))) {
				if (my selectionPolicy == XmSINGLE_SELECT || my selectionPolicy == XmBROWSE_SELECT) {
					ListBox_SetCurSel (my window, i);
				} else {
					ListBox_SetSel (my window, True, i);
				}
			}
		}
	#elif mac
		int i, n = (** my nat.list.handle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		_GuiMac_clipOn (me);
		for (i = 0; i < n; i ++) {
			char buffer [301];
			short int length = 300;
			cell.v = i;
			LGetCell (buffer, & length, cell, my nat.list.handle); buffer [length] = '\0';
			if (strequ (buffer, item))
				LSetSelect (true, cell, my nat.list.handle);
			else if (my selectionPolicy == XmSINGLE_SELECT || my selectionPolicy == XmBROWSE_SELECT)
				LSetSelect (false, cell, my nat.list.handle);
		}
		GuiMac_clipOff ();
	#endif
	if (notify && my extendedSelectionCallback)
		my extendedSelectionCallback (me, my extendedSelectionClosure, NULL);
}

void XmListSelectPos (Widget me, int position, Boolean notify) {
	#if win
		if (my selectionPolicy == XmSINGLE_SELECT || my selectionPolicy == XmBROWSE_SELECT) {
			ListBox_SetCurSel (my window, position - 1);
		} else {
			ListBox_SetSel (my window, True, position - 1);
		}
	#elif mac
		Cell cell; cell.h = 0;
		_GuiMac_clipOn (me);
		if (my selectionPolicy == XmSINGLE_SELECT || my selectionPolicy == XmBROWSE_SELECT) {
			int i, n = (** my nat.list.handle). dataBounds. bottom;
			for (i = 0; i < n; i ++) if (i != position - 1) {
				cell.v = i;
				LSetSelect (false, cell, my nat.list.handle);
			}
		}
		cell.v = position - 1; 
		LSetSelect (true, cell, my nat.list.handle);
		GuiMac_clipOff ();
	#endif
	if (notify && my extendedSelectionCallback)
		my extendedSelectionCallback (me, my extendedSelectionClosure, NULL);
}

void XmListSetAddMode (Widget me, Boolean mode) {
	(void) me;
	(void) mode;
}

void XmListSetBottomItem (Widget me, XmString item) {
	#if mac
		int position = XmListItemPos (me, item);
		_GuiMac_clipOn (me);
		if (position)
			LScroll (0, position - (** my nat.list.handle). visible. bottom - 1, my nat.list.handle);
		GuiMac_clipOff ();
	#else
		(void) me;
		(void) item;
	#endif
}

void XmListSetBottomPos (Widget me, int position) {
	#if mac
		_GuiMac_clipOn (me);
		LScroll (0, position - (** my nat.list.handle). visible. bottom - 1, my nat.list.handle);
		GuiMac_clipOff ();
	#else
		(void) me;
		(void) position;
	#endif
}

void XmListSetHorizPos (Widget me, int position) {
	(void) me;
	(void) position;
}

void XmListSetItem (Widget me, XmString item) {
	int position = XmListItemPos (me, item);
	#if win
		ListBox_SetTopIndex (my window, position - 1);
	#elif mac
		_GuiMac_clipOn (me);
		if (position)
			LScroll (0, position - (** my nat.list.handle). visible. top - 1, my nat.list.handle);
		GuiMac_clipOff ();
	#endif
}

void XmListSetPos (Widget me, int position) {
	#if win
		ListBox_SetTopIndex (my window, position - 1);
	#elif mac
		_GuiMac_clipOn (me);
		LScroll (0, position - (** my nat.list.handle). visible. top - 1, my nat.list.handle);
		GuiMac_clipOff ();
	#endif
}

Widget XmMessageBoxGetChild (Widget me, int child) {
	return
		child == XmDIALOG_OK_BUTTON ? my motif.messageBox.okButton :
		child == XmDIALOG_CANCEL_BUTTON ? my motif.messageBox.cancelButton :
		child == XmDIALOG_HELP_BUTTON ? my motif.messageBox.helpButton :
		NULL;
}

void XmScaleGetValue (Widget me, int *value_return) {
	Melder_assert (my widgetClass == xmScaleWidgetClass);
	*value_return = my value;
}

void XmScaleSetValue (Widget me, int value) {
	Melder_assert (my widgetClass == xmScaleWidgetClass);
	my value = value;
	#if win
		SendMessage (my window, PBM_SETPOS, (WPARAM) value, 0);
	#elif mac
		SetPortWindowPort (my macWindow);
	#endif
	_motif_update (me, 0);
}

void XmScrollBarGetValues (Widget me, int *value, int *sliderSize,
	int *increment, int *pageIncrement)
{
	Melder_assert (my widgetClass == xmScrollBarWidgetClass);
	*value = my value;
	*sliderSize = my sliderSize;
	*increment = my increment;
	*pageIncrement = my pageIncrement;
}

void XmScrollBarSetValues (Widget me, int value, int sliderSize,
	int increment, int pageIncrement, Boolean notify)
{
	Melder_assert (my widgetClass == xmScrollBarWidgetClass);
	my value = value;
	my sliderSize = sliderSize;
	my increment = increment;
	my pageIncrement = pageIncrement;
	NativeScrollBar_set (me);
	#if mac
		if (theGui.duringUpdate)
			_Gui_invalidateWidget (me);   /* HACK: necessary because VisRgn has temporarily been changed. */
	#endif
	if (notify)	_Gui_callCallbacks (me, & my motif.scrollBar.valueChangedCallbacks, NULL);
}

XmString XmStringCreateSimple (const char *cstring) {
	return (XmString) Melder_strdup (cstring);
}

void XmStringFree (XmString me) {
	Melder_free (me);
}

Boolean XmToggleButtonGadgetGetState (Widget me) {
	Melder_assert (MEMBER (me, ToggleButton));
	if (my inMenu) {
		#if mac
			short mark;
			GetItemMark (my nat.entry.handle, my nat.entry.item, & mark);
			return mark != noMark;
		#elif win
			return GetMenuState (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND) & MF_CHECKED ? True : False;
		#endif
	}
	#if mac
		return GetControlValue (my nat.control.handle);
	#elif win
		return (Button_GetState (my window) & 0x0003) == BST_CHECKED;
	#endif
}

void XmToggleButtonGadgetSetState (Widget me, Boolean value, Boolean notify) {
	Melder_assert (MEMBER (me, ToggleButton));
	if (my inMenu)
		NativeMenuItem_check (me, value);
	else
		_GuiNativeControl_check (me, value);
	if (notify) _Gui_callCallbacks (me, & my motif.toggleButton.valueChangedCallbacks, NULL);
}

#if mac
	static void _motif_update (Widget me, EventRecord *event) {
		Widget subview;
		RgnHandle visRgn;
		if (! my managed || my inMenu) return;
		for (subview = my firstChild; subview != NULL; subview = subview -> nextSibling) {
			if (subview -> widgetClass != xmShellWidgetClass)   /* Only in same mac window. */
				_motif_update (subview, event);
		}
		visRgn = NewRgn ();
		GetPortVisibleRegion (GetWindowPort (my macWindow), visRgn);
		if (! RectInRgn (& my rect, visRgn)) { DisposeRgn (visRgn); return; }
		SetPortWindowPort (my macWindow);
		switch (my widgetClass) {
			case xmPushButtonWidgetClass:
			case xmToggleButtonWidgetClass:
			case xmScrollBarWidgetClass: {
				_GuiMac_clipOn (me);
				Draw1Control (my nat.control.handle);
				GuiMac_clipOff ();
			} break;
			case xmListWidgetClass: {
				_GuiMac_clipOn (me);
				if (my isControl) {
					Draw1Control (my nat.control.handle);
				} else {
					LUpdate (visRgn, my nat.list.handle);
				}
				GuiMac_clipOff ();
			} break;
			case xmDrawingAreaWidgetClass: {
				if (my exposeCallback) {
					_GuiMac_clipOn (me);
					my exposeCallback (me, my exposeClosure, (XtPointer) event);
					GuiMac_clipOff ();
				}
			} break;
			case xmTextWidgetClass: {
				_GuiMacText_update (me);
			} break;
			case xmLabelWidgetClass: {
				_GuiMac_clipOn (me);
				Draw1Control (my nat.control.handle);
				GuiMac_clipOff ();
			} break;
			case xmCascadeButtonWidgetClass: {
				if (my isControl) {   /* In window menu bar or in dynamic menu. */
					_GuiMac_clipOn (me);
					Draw1Control (my nat.control.handle);
					/* BUG: should make insensitive if not my shell.active or my macWindow != FrontWindow () */
					GuiMac_clipOff ();
				}
			} break;
			case xmRowColumnWidgetClass: {
				/*if (my rowColumnType == XmMENU_BAR)
					DrawThemeMenuBarBackground (& my rect, 0, 0);*/
			} break;
			case xmScrolledWindowWidgetClass: {
				_GuiMac_clipOn (me);
				FrameRect (& my rect);
				GuiMac_clipOff ();
			} break;
			case xmFrameWidgetClass: {
				_GuiMac_clipOn (me);
				FrameRect (& my rect);
				GuiMac_clipOff ();
			} break;
			case xmScaleWidgetClass: {
				Rect r = my rect;
				_GuiMac_clipOn (me);
				InsetRect (& r, 20, 15);
				FrameRect (& r);
				InsetRect (& r, 1, 1);
				if (my value <= my minimum) {
					EraseRect (& r);
				} else {
					Rect active = r, inactive = r;
					active. right = r. left + (float) (r. right - r. left) * my value / (my maximum - my minimum + 1);
					PaintRect (& active);
					inactive. left = active. right;
					EraseRect ( & inactive);
				}
				GuiMac_clipOff ();
			} break;
			default: break;
		}
		DisposeRgn (visRgn);
	}
#elif win
	static void _motif_update (Widget me, void *event) { (void) me; (void) event; }
#endif

#if mac
static void _motif_processUpdateEvent (EventRecord *event) {
	WindowPtr macwindow = (WindowPtr) event -> message;
	Widget shell = (Widget) GetWRefCon (macwindow);
	Rect rect;
	int right, bottom;
	if (! shell) return;
	BeginUpdate (macwindow);
	theGui.duringUpdate = True;
	SetPortWindowPort (macwindow);
	if (! shell -> motif.shell.isDialog) {
		Rect portRect;
		GetWindowPortBounds (macwindow, & portRect);
		right = portRect. right, bottom = portRect. bottom;
		SetRect (& rect, right - 15, bottom - 15, right + 1, bottom + 1);
		ClipRect (& rect);
		DrawGrowIcon (macwindow);
		GuiMac_clipOff ();
	}
	_motif_update (shell, event);
	EndUpdate (macwindow);
	theGui.duringUpdate = False;
}
#endif

void XmUpdateDisplay (Widget displayDummy) {
	/*
	 * Send update messages to all shells.
	 */
	int i;
	(void) displayDummy;
	for (i = 0; i < MAXIMUM_NUMBER_OF_SHELLS; i ++) {
		Widget shell = theShells [i];
		#if mac
			if (shell && shell -> managed) {
				RgnHandle updateRegion = NewRgn ();
				GetWindowRegion (shell -> nat.window.ptr, kWindowUpdateRgn, updateRegion);
				if (Melder_debug != 12 && ! EmptyRgn (updateRegion)) {
					/*
					 * Hack an update event and send it to me.
					 */
					EventRecord event;
					event. message = (long) shell -> nat.window.ptr;
					_motif_processUpdateEvent (& event);
				}
				#if 0
					QDGetDirtyRegion (GetWindowPort (shell -> nat.window.ptr), updateRegion);
					QDFlushPortBuffer (GetWindowPort (shell -> nat.window.ptr), updateRegion);
				#endif
				DisposeRgn (updateRegion);
				GuiWindow_drain (shell);
			}
		#endif
	}
	#if win
		GdiFlush ();
	#endif
}

/***** EVENT *****/

#if win
static void on_scroll (Widget me, UINT part, int pos) {
	if (my maximum == my minimum) return;
	switch (part) {
		case SB_LINEUP: my value -= my increment; break;
		case SB_LINEDOWN: my value += my increment; break;
		case SB_PAGEUP: my value -= my pageIncrement; break;
		case SB_PAGEDOWN: my value += my pageIncrement; break;
		#if SCROLL32
		case SB_THUMBTRACK: case SB_THUMBPOSITION: my value = my minimum +
			pos * ((my maximum - my minimum - my sliderSize) / (32767.0 - (32767.0 * my sliderSize) / (my maximum - my minimum))); break;
		#else
		case SB_THUMBTRACK: case SB_THUMBPOSITION: my value = pos; break;
		#endif
		default: break;
	}
	if (my value < my minimum) my value = my minimum;
	if (my value > my maximum - my sliderSize) my value = my maximum - my sliderSize;
	NativeScrollBar_set (me);
	if (part == SB_THUMBTRACK || part == SB_THUMBPOSITION)
		_Gui_callCallbacks (me, & my motif.scrollBar.dragCallbacks, (XtPointer) (long) part);
	else
		_Gui_callCallbacks (me, & my motif.scrollBar.valueChangedCallbacks, (XtPointer) (long) part);
}
#elif mac
static pascal void _motif_scrollBarAction (ControlHandle maccontrol, short part) {
	Widget me = (Widget) GetControlReference (maccontrol);
	if (me == NULL || part == 0) return;
	switch (part) {
		case kControlUpButtonPart: my value -= my increment; break;
		case kControlDownButtonPart: my value += my increment; break;
		case kControlPageUpPart: my value -= my pageIncrement; break;
		case kControlPageDownPart: my value += my pageIncrement; break;
		case kControlIndicatorPart: my value = GetControl32BitValue (maccontrol); break;
		default: break;
	}
	if (my value < my minimum) my value = my minimum;
	if (my value > my maximum - my sliderSize) my value = my maximum - my sliderSize;
	SetControl32BitValue (maccontrol, my value);
	if (part == kControlIndicatorPart)
		_Gui_callCallbacks (me, & my motif.scrollBar.dragCallbacks, (XtPointer) (long) part);
	else
		_Gui_callCallbacks (me, & my motif.scrollBar.valueChangedCallbacks, (XtPointer) (long) part);
}
#endif

#if mac
static void _motif_activateControls (Widget me, Boolean act) {
	ControlHandle macControl;
	Widget control;
	UInt16 numberOfControls, icontrol;
	Melder_assert (MEMBER (me, Shell));
	Melder_assert (my nat.window.rootControl);
	CountSubControls (my nat.window.rootControl, & numberOfControls);
	SetPortWindowPort (my macWindow);
	for (icontrol = 1; icontrol <= numberOfControls; icontrol ++) {
		GetIndexedSubControl (my nat.window.rootControl, icontrol, & macControl);
		control = (Widget) GetControlReference (macControl);
		if (control && control -> magicNumber == 15111959 && control -> managed) {
			_GuiMac_clipOn (control);
			HiliteControl (macControl, act && ! control -> insensitive ? 0 : 255 );
			GuiMac_clipOff ();
		}
	}
}

static void _motif_activateGrowIcon (Widget me, Boolean act) {
	if (! my motif.shell.isDialog) {
		WindowPtr macwindow = my nat.window.ptr;
		Rect portRect, rect;
		int right, bottom;
		Melder_assert (macwindow != NULL);
		Melder_assert (GetWindowKind (macwindow) == userKind);
		GetWindowPortBounds (macwindow, & portRect);
		right = portRect. right, bottom = portRect. bottom;
		SetRect (& rect, right - 15, bottom - 15, right + 1, bottom + 1);
		SetPortWindowPort (macwindow);
		ClipRect (& rect);
		DrawGrowIcon (macwindow);
		GuiMac_clipOff ();
		if (act) ValidWindowRect (macwindow, & rect);
	}
}

static void _motif_activateText (Widget me, Boolean act) {
	if (act) {
		_GuiText_setTheTextFocus (my textFocus);
	} else {
		_GuiMac_clearTheTextFocus ();
	}
}

static void _motif_activateRest (Widget me, Boolean act) {
	Widget sub;
	if (! my managed) return;
	SetPortWindowPort (my macWindow);
	for (sub = my firstChild; sub != NULL; sub = sub -> nextSibling)
		if (sub -> widgetClass != xmShellWidgetClass)   /* Only in same mac window. */
			_motif_activateRest (sub, act);
	if (my isControl) {
		return;   /* Already done by _motif_activateControls. */
	} else if (my widgetClass == xmListWidgetClass) {
		_GuiMac_clipOn (me);
		LActivate (act, my nat.list.handle);
		GuiMac_clipOff ();
	} else if (my widgetClass == xmTextWidgetClass) {
		/* This is handled on the shell level. */
		return;
	} else if (my widgetClass == xmCascadeButtonWidgetClass &&
				my parent -> widgetClass == xmRowColumnWidgetClass &&
				my parent -> rowColumnType == XmMENU_BAR) {
		if (! act) {
			static RGBColor lightGrey = { 32768, 32768, 32768 }, blak;
			GetForeColor (& blak);
			RGBForeColor (& lightGrey);
			_motif_update (me, NULL);
			RGBForeColor (& blak);
		} else {
			_motif_update (me, NULL);
		}
	}
}

static void _motif_processActivateEvent (EventRecord *event) {
	WindowPtr macwindow = (WindowPtr) event -> message;
	int act = event -> modifiers & 1;
	if (GetWindowKind (macwindow) >= userKind) {
		Widget shell = (Widget) GetWRefCon (macwindow);
		if (! shell) return;

		Melder_assert (GetWindowKind (macwindow) == userKind);
		if (! BACKGROUND_WINDOWS_ARE_ACTIVE) shell -> motif.shell.active = act;
		if (! BACKGROUND_WINDOWS_ARE_ACTIVE) _motif_activateControls (shell, act);
		_motif_activateGrowIcon (shell, act);
		if (BACKGROUND_WINDOWS_ARE_ACTIVE && shell -> dialogStyle == XmDIALOG_FULL_APPLICATION_MODAL) {
			/*
			 * Bypass the Mac activation scheme if Motif style is on.
			 * Deactivate all other shells.
			 */
			int i;
			for (i = 0; i < MAXIMUM_NUMBER_OF_SHELLS; i ++) {
				Widget otherShell = theShells [i];
				if (otherShell && otherShell -> managed && otherShell != shell) {
					otherShell -> motif.shell.active = ! act;
					_motif_activateControls (otherShell, ! act);
					_motif_activateRest (otherShell, ! act);
				}
			}
		}
		_motif_activateText (shell, act);
		if (! BACKGROUND_WINDOWS_ARE_ACTIVE) _motif_activateRest (shell, act);
	}
}

static void _motif_processOsEvent (EventRecord *event) {
	unsigned char messageKind = ((unsigned long) event -> message & 0xFF000000) >> 24;
	if (messageKind == mouseMovedMessage) {
		Melder_fatal ("_motif_processOsEvent -- mouseMovedMessage");
		Point location = event -> where;
	} else if (messageKind == suspendResumeMessage && 0) {
		//Melder_fatal ("_motif_processOsEvent -- suspendResumeMessage");
		WindowPtr frontWindow = FrontWindow ();
		int act = event -> message & resumeFlag ? true : false, i;
		theBackground = ! act;
		for (i = 0; i < MAXIMUM_NUMBER_OF_SHELLS; i ++) {
			Widget shell = theShells [i];
			if (! shell || ! shell -> managed) continue;
			if (BACKGROUND_WINDOWS_ARE_ACTIVE) {
				if (theGui.modalDialog && theGui.modalDialog != shell -> nat.window.ptr) continue;
			} else {
				if (shell -> nat.window.ptr != frontWindow) continue;
			}
			shell -> motif.shell.active = act;
			_motif_activateControls (shell, act);
			_motif_activateGrowIcon (shell, act);
			_motif_activateText (shell, act);
			_motif_activateRest (shell, act);
		}
	}
}

static void _motif_processHighLevelEvent (EventRecord *event) {
	AEProcessAppleEvent (event);
}

static Widget _motif_findDrawingArea (Widget me) {
	Widget sub;
	if (my widgetClass == xmDrawingAreaWidgetClass) return me;
	for (sub = my firstChild; sub != NULL; sub = sub -> nextSibling)
		if (sub -> widgetClass != xmShellWidgetClass) {   /* Only in same mac window. */
			Widget drawingArea = _motif_findDrawingArea (sub);
			if (drawingArea) return drawingArea;
		}
	return NULL;   /* No DrawingArea found. */
}

static int _motif_shell_processKeyboardEquivalent (Widget shell, unsigned char kar, int modifiers, EventRecord *event) {
	WindowPtr macWindow;
	int imenu;
	if (! shell) return 0;
	macWindow = shell -> nat.window.ptr;
	/*
	 * If the user presses Command-?, i.e. the Command key plus the Shift key plus the "/?" key,
	 * Macintosh sends us the "/" character instead of the "?" character. Fix this.
	 */
	if (modifiers == (_motif_COMMAND_MASK | _motif_SHIFT_MASK)) {
		switch (kar) {
			case '/': kar = '\?'; break; case '[': kar = '{'; break; case ']': kar = '}'; break; case '\'': kar = '\"'; break;
			case ',': kar = '<'; break; case '.': kar = '>'; break; case '\\': kar = '|'; break; case '-': kar = '_'; break;
			case '=': kar = '+'; break; case '`': kar = '~'; break;
			default: break;
		}
	}
	/*
	 * If the user presses Option-Command-A, Macintosh sends us byte 140 instead of the "A" character.
	 * These bytes are above 128, except Option-Command-I and Option-Command-N, which give 94 and 126 instead,
	 * but since these are shifted characters ("^" and "~"), there will be no confusion. So we fix it all.
	 */
	if (modifiers == (_motif_COMMAND_MASK | _motif_OPTION_MASK)) {
		if (modifiers & _motif_SHIFT_MASK) {
			/* Ignore the triple modifiers! */
		} else {
			switch ((unsigned char) kar) {
				case 94: kar = 'I'; break; case 126: kar = 'N'; break; case 140: kar = 'A'; break; case 141: kar = 'C'; break;
				case 160: kar = 'T'; break; case 167: kar = 'S'; break; case 168: kar = 'R'; break; case 169: kar = 'G'; break;
				case 171: kar = 'E'; break; case 172: kar = 'U'; break; case 180: kar = 'Y'; break; case 181: kar = 'M'; break;
				case 182: kar = 'D'; break; case 183: kar = 'W'; break; case 185: kar = 'P'; break; case 186: kar = 'B'; break;
				case 189: kar = 'Z'; break; case 191: kar = 'O'; break; case 194: kar = 'L'; break; case 195: kar = 'V'; break;
				case 196: kar = 'F'; break; case 197: kar = 'X'; break; case 198: kar = 'J'; break; case 207: kar = 'Q'; break;
				case 250: kar = 'H'; break; case 251: kar = 'K'; break;
				default: break;
			}
		}
	}
	for (imenu = 1; imenu <= MAXIMUM_NUMBER_OF_MENUS; imenu ++) if (theMenus [imenu] && theMenus [imenu] -> macWindow == macWindow) {
		Widget child;
		for (child = theMenus [imenu] -> firstChild; child != NULL; child = child -> nextSibling)
			if (child -> widgetClass == xmPushButtonWidgetClass &&
					child -> motif.pushButton.acceleratorChar == kar &&
					child -> motif.pushButton.acceleratorModifiers == modifiers)
				if (child -> activateCallback && ! child -> insensitive) {
					child -> activateCallback (child, child -> activateClosure, (XtPointer) event);
					return 1;
				}
	}
	return 0;
}

static void _motif_processKeyboardEquivalent (unsigned char kar, int modifiers, EventRecord *event) {
	/*
	 * First try to send the key command to the active shell.
	 * If that fails, try to send the key command to the application shell.
	 */
	if (! _motif_shell_processKeyboardEquivalent ((Widget) GetWRefCon (FrontWindow ()), kar, modifiers, event))
		_motif_shell_processKeyboardEquivalent (theApplicationShell, kar, modifiers, event);
}

static bool _motif_processKeyDownEvent (EventHandlerCallRef nextHandler, EventRef eventRef, EventRecord *event) {
	/*
	 * This routine determines whether a key-down message is a menu shortcut or is meant to go to a text widget.
	 * It has to find this out fast, so it cannot go through all the menu structures for each key-down.
	 * Therefore, every shell maintains a bit list of which of the lower accelerators are used.
	 */
	Widget shell = (Widget) GetWRefCon (FrontWindow ());
	if (shell == NULL) {
		// Probably a system window such as an error dialog or file selection dialog.
		//CallNextEventHandler (nextHandler, eventRef);
		return false;
	}
	Widget text = theGui.textFocus;
	unsigned char charCode = event -> message & charCodeMask, keyCode = (event -> message & keyCodeMask) >> 8;
	int modifiers = 0;
	if (event -> modifiers & cmdKey) modifiers |= _motif_COMMAND_MASK;
	if (event -> modifiers & optionKey) modifiers |= _motif_OPTION_MASK;
	if (event -> modifiers & shiftKey) modifiers |= _motif_SHIFT_MASK;
	if (charCode < 32) {
		if (charCode == 13) {   /* User pressed Return. */
			/*
			 * First test for keyboard shortcut.
			 */
			if (shell && (shell -> motif.shell.lowAccelerators [modifiers] & 1 << motif_ENTER)) {
				_motif_processKeyboardEquivalent (motif_ENTER, modifiers, event);
				return true;
			}
			/*
			 * Then look for default button in active window.
			 */
			if (shell && shell -> defaultButton) {
				Widget defaultButton = shell -> defaultButton;
				if (defaultButton -> activateCallback) {
					defaultButton -> activateCallback (defaultButton, defaultButton -> activateClosure, (XtPointer) event);
					return true;
				}
				/*
				 * Otherwise, hand it to a text widget.
				 */
			}
			/*
			 * Then look for a text widget with an activate callback.
			 */
			if (_GuiMacText_tryToHandleReturnKey (nextHandler, eventRef, text, event)) return true;
			/*
			 * Otherwise, hand it to a text widget.
			 */
		} else if (charCode == 27) {   /* User pressed Escape. */
			/*
			 * First test for keyboard shortcut.
			 */
			if (shell && (shell -> motif.shell.lowAccelerators [modifiers] & 1 << motif_ESCAPE)) {
				_motif_processKeyboardEquivalent (motif_ESCAPE, modifiers, event);
				return true;
			}
			/*
			 * Then look for cancel button in active window.
			 */
			if (shell && shell -> cancelButton) {
				Widget cancelButton = shell -> cancelButton;
				if (cancelButton -> activateCallback) {
					cancelButton -> activateCallback (cancelButton, cancelButton -> activateClosure, (XtPointer) event);
					return true;
				}
				/*
				 * Do not hand it to a text widget.
				 */
				return true;
			}
			/*
			 * Do not hand it to a text widget.
			 */
			return true;
		} else if (charCode == 9) {   /* User pressed Tab. */
			/*
			 * First test for keyboard shortcut.
			 */
			if (shell && (shell -> motif.shell.lowAccelerators [modifiers] & 1 << motif_TAB)) {
				_motif_processKeyboardEquivalent (motif_TAB, modifiers, event);
				return true;
			}
			/*
			 * Otherwise, hand it to a text widget.
			 */
		} else if (charCode == 8) {   /* User pressed Backspace. */
			/*
			 * First test for keyboard shortcut.
			 */
			if (shell && (shell -> motif.shell.lowAccelerators [modifiers] & 1 << motif_BACKSPACE)) {
				_motif_processKeyboardEquivalent (motif_BACKSPACE, modifiers, event);
				return true;
			}
			/*
			 * Otherwise, hand it to a text widget.
			 */
		} else if (charCode == 5) {   /* Help button. Simulate Command-?. */
			_motif_processKeyboardEquivalent ('?', _motif_COMMAND_MASK | _motif_SHIFT_MASK, event);
			return true;
		} else if (charCode == 16) {   /* F1... F12 */
			int fkey =
				keyCode == 0x7A ? 1 : keyCode == 0x78 ? 2 : keyCode == 0x63 ? 3 : keyCode == 0x76 ? 4 :
				keyCode == 0x60 ? 5 : keyCode == 0x61 ? 6 : keyCode == 0x62 ? 7 : keyCode == 0x64 ? 8 :
				keyCode == 0x65 ? 9 : keyCode == 0x6D ? 10 : keyCode == 0x67 ? 11 : keyCode == 0x6F ? 12 : 0;
			if (fkey)
				_motif_processKeyboardEquivalent (motif_F1 - 1 + fkey, modifiers, event);
			return true;
		} else if (charCode == 11) {
			_motif_processKeyboardEquivalent (motif_PAGE_UP, modifiers, event);
			return true;   /* BUG: we should implement a scroll up in the text widget (IM V-193). */
		} else if (charCode == 12) {
			_motif_processKeyboardEquivalent (motif_PAGE_DOWN, modifiers, event);
			return true;   /* BUG: we should implement a scroll down in the text widget (IM V-193). */
		} else if (charCode == 1) {
			_motif_processKeyboardEquivalent (motif_HOME, modifiers, event);
			return true;   /* BUG: we should implement a top left scroll in the text widget (IM V-192). */
		} else if (charCode == 4) {
			_motif_processKeyboardEquivalent (motif_END, modifiers, event);
			return true;   /* BUG: we should implement a bottom right scroll in the text widget (IM V-193). */
		} else if (charCode == 28) {
			/*
			 * First test for keyboard shortcut.
			 */
			if (shell && (shell -> motif.shell.lowAccelerators [modifiers] & 1 << motif_LEFT_ARROW)) {
				_motif_processKeyboardEquivalent (motif_LEFT_ARROW, modifiers, event);
				return true;
			}
			/*
			 * Otherwise, hand it to a text widget.
			 */
		} else if (charCode == 29) {
			if (shell && (shell -> motif.shell.lowAccelerators [modifiers] & 1 << motif_RIGHT_ARROW)) {
				_motif_processKeyboardEquivalent (motif_RIGHT_ARROW, modifiers, event);
				return true;
			}
		} else if (charCode == 30) {
			if (shell && (shell -> motif.shell.lowAccelerators [modifiers] & 1 << motif_UP_ARROW)) {
				_motif_processKeyboardEquivalent (motif_UP_ARROW, modifiers, event);
				return true;
			}
		} else if (charCode == 31) {
			if (shell && (shell -> motif.shell.lowAccelerators [modifiers] & 1 << motif_DOWN_ARROW)) {
				_motif_processKeyboardEquivalent (motif_DOWN_ARROW, modifiers, event);
				return true;
			}
		}
	} else if (charCode == 127) {
		if (shell && (shell -> motif.shell.lowAccelerators [modifiers] & 1 << motif_DELETE)) {
			_motif_processKeyboardEquivalent (motif_DELETE, modifiers, event);
			return true;
		}
		return true;   /* BUG: we should implement a forward delete (or selection removal) in the text widget (IM V-192). */
	}
	/*
	 * If the Command key is pressed with a printable character, this is always a menu shortcut.
	 */
	else if (modifiers & _motif_COMMAND_MASK) {
		/*
		 * For shortcuts, the status of the Caps Lock key is immaterial.
		 */
		unsigned char kar = toupper (charCode);
		/*
		 * Command-X/C/V/period are defined even if there is no menu.
		 * The existence of a text widget forces Command-C to mean 'Copy'.
		 * The existence of an editable text widget forces Command-X to mean 'Cut' and Command-V to mean 'Paste'.
		 */
		if (_GuiMacText_tryToHandleClipboardShortcut (nextHandler, eventRef, text, kar, event)) return true;
		#define USE_COMMAND_PERIOD_AS_CANCEL  0
		if (USE_COMMAND_PERIOD_AS_CANCEL && kar == '.') {
			/*
			 * Command-period: first test for "Escape" keyboard shortcut.
			 */
			if (shell && (shell -> motif.shell.lowAccelerators [0] & 1 << motif_ESCAPE)) {
				_motif_processKeyboardEquivalent (motif_ESCAPE, 0, event);
				return true;
			}
			/*
			 * Then look for cancel button in active window.
			 */
			if (shell && shell -> cancelButton) {
				Widget cancelButton = shell -> cancelButton;
				if (cancelButton -> activateCallback) {
					cancelButton -> activateCallback (cancelButton, cancelButton -> activateClosure, (XtPointer) event);
					return true;
				}
			}
		}
		/*
		 * The text widget did not catch any Command-X/C/V/period.
		 */
		_motif_processKeyboardEquivalent (kar, modifiers, event);
		/*
		 * After executing a menu shortcut, do not send the key to a text widget as well.
		 */
		return true;
	}
	if (_GuiMacText_tryToHandleKey (nextHandler, eventRef, text, keyCode, charCode, event)) return true;

	/* Last chance: try drawingArea. */
	if (shell) {
		Widget drawingArea = _motif_findDrawingArea (shell);
		if (drawingArea && drawingArea -> inputCallback) {
			drawingArea -> inputCallback (drawingArea, drawingArea -> inputClosure, (XtPointer) event);
			return true;
		}
	}
	return false;
}

static void mac_processMenuChoice (long choice, EventRecord *event) {
	int macMenuID = HiWord (choice), macMenuItem = LoWord (choice);
	Widget menu, item;
	if (macMenuID == 0) return;
	menu = theMenus [macMenuID == kHMHelpMenuID ? MAXIMUM_NUMBER_OF_MENUS : macMenuID];
	if (menu == NULL) return;
	item = menu -> firstChild;
	if (macMenuID == kHMHelpMenuID) macMenuItem -= theHelpMenuOffset;
	while (item && macMenuItem > 1) {
		if (item -> widgetClass != xmPulldownMenuWidgetClass && item -> managed) macMenuItem --;   /* Count managed buttons and separators only. */
		item = item -> nextSibling;
	}
	Melder_assert (item != NULL);
	while (item && ! item -> managed) item = item -> nextSibling;
	Melder_assert (item != NULL);
	if (item -> activateCallback) item -> activateCallback (item, item -> activateClosure, (XtPointer) event);
	else if (item -> widgetClass == xmToggleButtonWidgetClass) {
		XmToggleButtonGadgetSetState (item, 1 - XmToggleButtonGadgetGetState (item), False);
		_Gui_callCallbacks (item, & item -> motif.toggleButton.valueChangedCallbacks, (XtPointer) event);
	}
}

static void _motif_processMouseDownEvent (EventRecord *event) {
	WindowPtr macvenster;
	int part = FindWindow (event -> where, & macvenster);
	switch (part) {
		case inMenuBar: {
			long choice = MenuSelect (event -> where);
			mac_processMenuChoice (choice, event);
			HiliteMenu (0);
		} break;
		case inSysWindow: {
		} break;
		case inDrag: {
			RgnHandle greyRegion;
			Rect greyBounds;
			if (macvenster != FrontWindow ()) {
				if (theGui.modalDialog && theGui.modalDialog != macvenster) {
					SysBeep (1);
					return;
				} else {
					SelectWindow (macvenster);
				}
			}
			greyRegion = GetGrayRgn ();
			GetRegionBounds (greyRegion, & greyBounds);
			DragWindow (macvenster, event -> where, & greyBounds);
		} break;
		case inGoAway: {
			if (TrackGoAway (macvenster, event -> where)) {
				Widget shell = (Widget) GetWRefCon (macvenster);
				int deleteResponse;
				if (! shell) return;
				deleteResponse = shell -> deleteResponse;   /* Save this, in case the callback should kill the widget (XmDO_NOTHING). */
				if (shell -> motif.shell.goAwayCallback)
					shell -> motif.shell.goAwayCallback (shell, shell -> motif.shell.goAwayClosure, event);
				if (deleteResponse == XmDESTROY) {
					XtDestroyWidget (shell);
				} else if (deleteResponse == XmUNMAP) {
					/*
						Unmapping is not the same as unmanaging.
						The following duplicates some of the code of XtUnmanageChild,
						but does not clear the 'managed' attribute.
					*/
					_GuiMac_clearTheTextFocus ();
					if (macvenster == theGui.modalDialog)
						theGui.modalDialog = NULL;
					HideWindow (macvenster);
				}
			}
		} break;
		case inGrow: {
			RgnHandle greyRegion = GetGrayRgn ();
			Rect greyBounds;
			long newSize;
			GetRegionBounds (greyRegion, & greyBounds);
			newSize = GrowWindow (macvenster, event -> where, & greyBounds);
			if (newSize != 0) {
				Widget shell = (Widget) GetWRefCon (macvenster);
				int oldWidth, oldHeight, newWidth, newHeight;
				if (! shell) return;
				oldWidth = shell -> width, oldHeight = shell -> height;
				newWidth = LoWord (newSize), newHeight = HiWord (newSize);
				shell -> width = newWidth;
				shell -> height = newHeight;
				/*Melder_casual ("%d %d %d %d",oldWidth,oldHeight,newWidth,newHeight);*/
				shellResizeWidget (shell, 0, 0, newWidth - oldWidth, newHeight - oldHeight);
			}
		} break;
		case inZoomIn: case inZoomOut: {
			Widget shell = (Widget) GetWRefCon (macvenster);
			if (shell) {
				int oldWidth = shell -> width, oldHeight = shell -> height, newWidth, newHeight;
				Rect bounds;
				ZoomWindow (macvenster, part, 1);
				GetWindowPortBounds (macvenster, & bounds);
				newWidth = bounds.right - bounds.left;
				newHeight = bounds.bottom - bounds.top;
				shell -> width = newWidth;
				shell -> height = newHeight;
				shellResizeWidget (shell, 0, 0, newWidth - oldWidth, newHeight - oldHeight);
			}
		} break;
		case inContent: {
			if (macvenster != FrontWindow ()) {
				if (theGui.modalDialog && theGui.modalDialog != macvenster) { SysBeep (1); return; }
				SelectWindow (macvenster);
				if (! BACKGROUND_WINDOWS_ARE_ACTIVE) return;
				XmUpdateDisplay (NULL);
			}
			if (GetWindowKind (macvenster) == userKind) {
				ControlHandle maccontrol;
				ControlPartCode controlPart;
				SetPortWindowPort (macvenster);
				GlobalToLocal (& event -> where);
				maccontrol = FindControlUnderMouse (event -> where, macvenster, & controlPart);
				if (maccontrol) {
					Widget control = (Widget) GetControlReference (maccontrol);
					if (! control) return;
					if (control -> magicNumber != 15111959 || ! control -> managed) goto LABEL_clickedOutsideControl;
					event -> message = controlPart;
					switch (controlPart) {
						case kControlListBoxPart: {
							if (control -> widgetClass == xmListWidgetClass) {
								_GuiMac_clipOn (control);
								bool pushed = TrackControl (maccontrol, event -> where, NULL);
								GuiMac_clipOff ();
								if (pushed && control -> extendedSelectionCallback)
									control -> extendedSelectionCallback (control, control -> extendedSelectionClosure, (XtPointer) event);
							}
						} break;
						case kControlButtonPart:
						case kControlLabelPart: {
							if (control -> widgetClass == xmPushButtonWidgetClass) {   /* Push button. */
								int pushed;
								_GuiMac_clipOn (control);
								pushed = TrackControl (maccontrol, event -> where, NULL);
								GuiMac_clipOff ();
								if (pushed && control -> activateCallback)
									control -> activateCallback (control, control -> activateClosure, (XtPointer) event);
							} else if (control -> widgetClass == xmCascadeButtonWidgetClass) {   /* Cascade button. */
								Widget menu = control -> subMenuId;
								if (menu && ! control -> insensitive) {
									Point pos;
									long choice = 0;
									_GuiMac_clipOn (control);
									HiliteControl (maccontrol, 10);
									GuiMac_clipOff ();
									SetPt (& pos, control -> rect.left + 2, control -> rect.bottom);
									LocalToGlobal (& pos);
									choice = PopUpMenuSelect (menu -> nat.menu.handle, pos.v, pos.h, 1);
									/*
										Beware: we must unhighlight the cascade button NOW,
										because callbacks may destroy it.
									*/
									_GuiMac_clipOn (control);
									HiliteControl (maccontrol, 0);
									GuiMac_clipOff ();
									mac_processMenuChoice (choice, event);
								}
							}
						} break;
						case kControlCheckBoxPart: {   /* Check box or radio button. */
							if (TrackControl (maccontrol, event -> where, NULL)) {
								int value = GetControlValue (maccontrol);
								if (control -> isRadioButton) {

									/* User clicked radio button. */
									/* Select this button. */

									if (value) return;   /* Radio button already set. */
									SetControlValue (maccontrol, 1);
									if (control -> parent -> radioBehavior) {
	
										/* Deselect the other buttons. */

										Widget child = control -> parent -> firstChild;
										while (child) {
											if (child -> widgetClass == xmToggleButtonWidgetClass && child != control)
												SetControlValue (child -> nat.control.handle, 0);
											child = child -> nextSibling;
										}
									}
								} else {

									/* User clicked check box. */
									/* Toggle its state. */

									SetControlValue (maccontrol, 1 - value);
								}
								_Gui_callCallbacks (control, & control -> motif.toggleButton.valueChangedCallbacks, (XtPointer) event);
							}
						} break;
						case kControlUpButtonPart:
						case kControlDownButtonPart:
						case kControlPageUpPart:
						case kControlPageDownPart: {
							static ControlActionUPP theControlActionUPP;
							if (! theControlActionUPP) theControlActionUPP = NewControlActionUPP (_motif_scrollBarAction);
							TrackControl (maccontrol, event -> where, theControlActionUPP);
						} break;
						case kControlIndicatorPart: {
							if (TrackControl (maccontrol, event -> where, NULL)) {
								control -> value = GetControl32BitValue (maccontrol);
								_Gui_callCallbacks (control, & control -> motif.scrollBar.valueChangedCallbacks, (XtPointer) (long) controlPart);
							}
						} break;
						case kControlEditTextPart: {
							_GuiMacText_handleClick (control, event);
						} break;
						default: break;
					}
				} else {   /* Clicked in content region, but not in a control. */
					Widget shell, clicked;
					LABEL_clickedOutsideControl:
					shell = (Widget) GetWRefCon (macvenster);
					if (! shell) return;
					clicked = _motif_findSubwidget (shell, event -> where. h, event -> where. v);
					if (clicked) {
						if (clicked -> widgetClass == xmListWidgetClass) {
							int doubleClick;
							_GuiMac_clipOn (clicked);
							doubleClick = LClick (event -> where, event -> modifiers,
								clicked -> nat.list.handle);
							GuiMac_clipOff ();
							if (clicked -> extendedSelectionCallback)
								clicked -> extendedSelectionCallback (clicked, clicked -> extendedSelectionClosure, (XtPointer) event);
							if (doubleClick) {
								if (clicked -> motif.list.defaultActionCallback)
									clicked -> motif.list.defaultActionCallback (clicked, clicked -> motif.list.defaultActionClosure, (XtPointer) event);
							}
						} else if (clicked -> widgetClass == xmTextWidgetClass) {
							if (clicked -> isControl) {   /* A click in the margin of an EditText control !! */
								;   /* Do nothing. To react would feel like clicking after all text. */
							} else {
								_GuiMacText_handleClick (clicked, event);
							}
						} else if (clicked -> widgetClass == xmDrawingAreaWidgetClass) {
							if (clicked -> inputCallback)
								clicked -> inputCallback (clicked, clicked -> inputClosure, (XtPointer) event);
						} else if (clicked -> widgetClass == xmCascadeButtonWidgetClass && 0) {
							Widget menu = clicked -> subMenuId;
							if (menu && ! clicked -> insensitive) {
								Point pos;
								long choice = 0;
								Rect r = clicked -> rect; r.left -= 4; r.right += 4; r.top -= 1;
								_GuiMac_clipOn (clicked);
								InvertRect (& r);
								GuiMac_clipOff ();
								SetPt (& pos, r.left + 1, r.bottom);
								LocalToGlobal (& pos);
								choice = PopUpMenuSelect (menu -> nat.menu.handle, pos.v, pos.h, 1);
								/*
									Beware: we must unhighlight the cascade button NOW,
									because callbacks may destroy it.
								*/
								SetPortWindowPort (clicked -> macWindow);
								_motif_update (clicked, NULL);
								mac_processMenuChoice (choice, event);
							}
						}
					}
				}
			}
		} break;
		default: break;
	}
}

static pascal OSStatus keyDownEventHandler (EventHandlerCallRef nextHandler, EventRef eventRef, void *userData) {
	(void) nextHandler;
	(void) userData;
	EventRecord eventRecord;
	ConvertEventRefToEventRecord (eventRef, & eventRecord);
	#if 0
		switch (eventRecord. what) {
			case nullEvent: break;
			case mouseDown: _motif_processMouseDownEvent (& eventRecord); break;
			case mouseUp: break;
			case keyDown: _motif_processKeyDownEvent (nextHandler, eventRef, & eventRecord); break;
			case keyUp: break;
			case autoKey: _motif_processKeyDownEvent (nextHandler, eventRef, & eventRecord); break;
			case updateEvt: _motif_processUpdateEvent (& eventRecord); break;
			case activateEvt: _motif_processActivateEvent (& eventRecord); break;
			case osEvt: _motif_processOsEvent (& eventRecord); break;
			case kHighLevelEvent: _motif_processHighLevelEvent (& eventRecord); break;
			default: break;
		}
		return noErr;
	#else
		if (_motif_processKeyDownEvent (nextHandler, eventRef, & eventRecord)) return noErr;
	#endif
	return eventNotHandledErr;
}

#endif

void XtNextEvent (XEvent *xevent) {
	#if win
		GetMessage (xevent, NULL, 0, 0);
	#elif mac
		static EventHandlerUPP keyDownEventHandlerUPP;
		if (keyDownEventHandlerUPP == NULL) {
			keyDownEventHandlerUPP = NewEventHandlerUPP (keyDownEventHandler);
			EventTypeSpec keyDownEventTypeSpecs [2] = {
				{ kEventClassKeyboard, kEventRawKeyDown },
				{ kEventClassKeyboard, kEventRawKeyRepeat }
			};
			//InstallEventHandler (theUserFocusEventTarget, keyDownEventHandlerUPP, 2, keyDownEventTypeSpecs, NULL, NULL);
			InstallApplicationEventHandler (keyDownEventHandlerUPP, 2, keyDownEventTypeSpecs, NULL, NULL);
		}
		/*
		 * The waiting time should be short enough to allow Melder_progress to wait for an event.
		 * Therefore we take 1 clock tick, so that if Melder_progress updates every 15 clock ticks,
		 * the performance loss will not be more than 7 percent.
		 */
		WaitNextEvent (everyEvent, (EventRecord *) xevent, 1, NULL);
	#endif
}

static void processWorkProcsAndTimeOuts (void) {
	long i;
	if (theNumberOfWorkProcs) for (i = 9; i >= 1; i --)
		if (theWorkProcs [i])
			if (theWorkProcs [i] (theWorkProcClosures [i])) XtRemoveWorkProc (i);
	#if ! defined (macintosh)
		if (theNumberOfTimeOuts) {
			clock_t now = clock ();
			for (i = 1; i < 10; i ++) if (theTimeOutProcs [i]) {
				static volatile clock_t timeElapsed;   /* Careful: use 32-bit integers circularly; prevent optimization. */
				timeElapsed = now - theTimeOutStarts [i];
				if (timeElapsed > theTimeOutIntervals [i]) {
					theTimeOutProcs [i] (theTimeOutClosures [i], & i);
					XtRemoveTimeOut (i);
				}
			}
		}
	#endif
}

void XtAppNextEvent (XtAppContext appContext, XEvent *xevent) {
	(void) appContext;
	#if win
		if (theNumberOfWorkProcs != 0 || theNumberOfTimeOuts != 0) {
			if (PeekMessage (xevent, 0, 0, 0, PM_REMOVE)) {   // Message available?
				;   // Hand message to XtDispatchEvent.
			} else {
				processWorkProcsAndTimeOuts ();   // Handle chores during idle time.
				xevent -> message = 0;   // Hand null message to XtDispatchEvent.
			}
		} else GetMessage (xevent, NULL, 0, 0);   // Be neighbour-friendly: do not hand null events.
	#elif mac
		processWorkProcsAndTimeOuts ();
		XtNextEvent (xevent);
	#endif
}

#if win

static int win_shell_processKeyboardEquivalent (Widget me, int kar, int modifiers) {
	int imenu;
	for (imenu = 1; imenu <= MAXIMUM_NUMBER_OF_MENUS; imenu ++) if (theMenus [imenu] && theMenus [imenu] -> shell == me) {
		Widget child;
		for (child = theMenus [imenu] -> firstChild; child != NULL; child = child -> nextSibling)
			if (child -> widgetClass == xmPushButtonWidgetClass &&
					child -> motif.pushButton.acceleratorChar == kar &&
					child -> motif.pushButton.acceleratorModifiers == modifiers)
				if (child -> activateCallback && ! child -> insensitive) {
					child -> activateCallback (child, child -> activateClosure, 0);
					return 1;
				}
	}
	return 0;
}

static int win_processKeyboardEquivalent (Widget me, int kar, int modifiers) {
	/*
	 * First try to send the key command to the active shell.
	 * If that fails, try to send the key command to the application shell.
	 */
	if (win_shell_processKeyboardEquivalent (me, kar, modifiers)) return 1;
	if (win_shell_processKeyboardEquivalent (theApplicationShell, kar, modifiers)) return 1;
	return 0;
}

	static Widget _motif_findDrawingArea (Widget me) {
		Widget sub;
		if (my widgetClass == xmDrawingAreaWidgetClass) return me;
		for (sub = my firstChild; sub != NULL; sub = sub -> nextSibling)
			if (! MEMBER (sub, Shell)) {   /* Only in same top window. */
				Widget drawingArea = _motif_findDrawingArea (sub);
				if (drawingArea) return drawingArea;
			}
		return NULL;   /* No DrawingArea found. */
	}
#endif

void XtDispatchEvent (XEvent *xevent) {
	#if mac
		EventRecord *event = (EventRecord *) xevent;
		switch (event -> what) {
			case nullEvent: break;
			case mouseDown: _motif_processMouseDownEvent (event); break;
			case mouseUp: break;
			//case keyDown: _motif_processKeyDownEvent (event); break;
			case keyUp: break;
			//case autoKey: _motif_processKeyDownEvent (event); break;
			case updateEvt: _motif_processUpdateEvent (event); break;
			case activateEvt: _motif_processActivateEvent (event); break;
			case osEvt: _motif_processOsEvent (event); break;
			case kHighLevelEvent: _motif_processHighLevelEvent (event); break;
			default: break;
		}
	#else
		MSG *message = (MSG *) xevent;
		if (message -> message == 0) return;   // null message from PeekMessage during work proc or time out.
/*if (message -> message == WM_KEYDOWN || message -> message == WM_SYSKEYDOWN)
{
	int kar = LOWORD (message -> wParam);
	int modifiers = 0;
	Widget me = (Widget) GetWindowLong (message -> hwnd, GWL_USERDATA);
	if (GetKeyState (VK_CONTROL) < 0) modifiers |= _motif_COMMAND_MASK;
	if (GetKeyState (VK_MENU) < 0) modifiers |= _motif_OPTION_MASK;
	if (GetKeyState (VK_SHIFT) < 0) modifiers |= _motif_SHIFT_MASK;
if(kar>=48)Melder_casual ("modifiers:%s%s%s\nmessage: %s\nkar: %d",
modifiers & _motif_COMMAND_MASK ? " control" : "",
modifiers & _motif_OPTION_MASK ? " alt" : "",
modifiers & _motif_SHIFT_MASK ? " shift" : "", message -> message == WM_KEYDOWN ? "keydown" : "syskeydown", kar);
}*/
		/*
		 * Intercept accelerators, which may be function keys or Command combinations.
		 * Some Alt-GR combinations denote special characters (e.g. backslash) on some (e.g. German) keyboards;
		 * in such a case, the message is WM_KEYDOWN, and the reported modifier keys are Control (!) and Alt
		 * (on a German keyboard, the backslash can be generated by Ctrl-Alt-ringel-s as well);
		 * otherwise Alt keys give WM_SYSKEYDOWN messages.
		 */
		if (message -> message == WM_KEYDOWN && (GetKeyState (VK_CONTROL) < 0 || ! (message -> lParam & (1<<29))) ||
			message -> message == WM_SYSKEYDOWN && GetKeyState (VK_MENU) < 0
				&& (message -> lParam & (1<<29)) ||   // R&N 413: Alt key is pressed
			message -> message == WM_SYSKEYDOWN && GetKeyState (VK_CONTROL) < 0)
		{
			int kar = LOWORD (message -> wParam);
			int modifiers = 0;
			Widget me = (Widget) GetWindowLong (message -> hwnd, GWL_USERDATA);
			if (GetKeyState (VK_CONTROL) < 0) modifiers |= _motif_COMMAND_MASK;
			if (GetKeyState (VK_MENU) < 0) modifiers |= _motif_OPTION_MASK;
			if (GetKeyState (VK_SHIFT) < 0) modifiers |= _motif_SHIFT_MASK;
/*if(kar>=48)Melder_casual ("modifiers:%s%s%s\nmessage: %s\nkar: %d",
modifiers & _motif_COMMAND_MASK ? " control" : "",
modifiers & _motif_OPTION_MASK ? " alt" : "",
modifiers & _motif_SHIFT_MASK ? " shift" : "", message -> message == WM_KEYDOWN ? "keydown" : "syskeydown", kar);*/
			if (me && my shell) {
				unsigned long acc = my shell -> motif.shell.lowAccelerators [modifiers];
				//if (kar != VK_CONTROL) Melder_casual ("%d %d", acc, kar);
				if (kar < 48) {
					if (kar == VK_BACK) {   /* Shortcut or text. */
						if (acc & 1 << motif_BACKSPACE) { win_processKeyboardEquivalent (my shell, motif_BACKSPACE, modifiers); return; }
					} else if (kar == VK_TAB) {   /* Shortcut or text. */
						if (acc & 1 << motif_TAB) { win_processKeyboardEquivalent (my shell, motif_TAB, modifiers); return; }
					} else if (kar == VK_RETURN) {   /* Shortcut, default button, or text. */
						if (acc & 1 << motif_ENTER) { win_processKeyboardEquivalent (my shell, motif_ENTER, modifiers); return; }
						else {
							Widget defaultButton = my shell -> defaultButton;
							if (defaultButton && defaultButton -> activateCallback)
								{ defaultButton -> activateCallback (defaultButton, defaultButton -> activateClosure, 0); return; }
						}
					} else if (kar == VK_ESCAPE) {   /* Shortcut or cancel button. */
						if (acc & 1 << motif_ESCAPE) { win_processKeyboardEquivalent (my shell, motif_ESCAPE, modifiers); return; }
						else {
							Widget cancelButton = my shell -> cancelButton;
							if (cancelButton && cancelButton -> activateCallback)
								{ cancelButton -> activateCallback (cancelButton, cancelButton -> activateClosure, 0); return; }
						}
						return;
					} else if (kar == VK_PRIOR) {   /* Shortcut or text. */
						if (acc & 1 << motif_PAGE_UP) { win_processKeyboardEquivalent (my shell, motif_PAGE_UP, modifiers); return; }
					} else if (kar == VK_NEXT) {   /* Shortcut or text. */
						if (acc & 1 << motif_PAGE_DOWN) { win_processKeyboardEquivalent (my shell, motif_PAGE_DOWN, modifiers); return; }
					} else if (kar == VK_HOME) {   /* Shortcut or text. */
						if (acc & 1 << motif_HOME) { win_processKeyboardEquivalent (my shell, motif_HOME, modifiers); return; }
					} else if (kar == VK_END) {   /* Shortcut or text. */
						if (acc & 1 << motif_END) { win_processKeyboardEquivalent (my shell, motif_END, modifiers); return; }
					} else if (kar == VK_LEFT) {   /* Shortcut or text. */
						if (acc & 1 << motif_LEFT_ARROW) { win_processKeyboardEquivalent (my shell, motif_LEFT_ARROW, modifiers); return; }
					} else if (kar == VK_RIGHT) {   /* Shortcut or text. */
						if (acc & 1 << motif_RIGHT_ARROW) { win_processKeyboardEquivalent (my shell, motif_RIGHT_ARROW, modifiers); return; }
					} else if (kar == VK_UP) {   /* Shortcut or text. */
						if (acc & 1 << motif_UP_ARROW) { win_processKeyboardEquivalent (my shell, motif_UP_ARROW, modifiers); return; }
					} else if (kar == VK_DOWN) {   /* Shortcut or text. */
						if (acc & 1 << motif_DOWN_ARROW) { win_processKeyboardEquivalent (my shell, motif_DOWN_ARROW, modifiers); return; }
					} else if (kar == VK_INSERT) {   /* Shortcut. */
						win_processKeyboardEquivalent (my shell, motif_INSERT, modifiers);
						return;
					} else if (kar == VK_DELETE) {   /* Shortcut or text. */
						if (acc & 1 << motif_DELETE) { win_processKeyboardEquivalent (my shell, motif_DELETE, modifiers); return; }
					} else if (kar == VK_HELP) {   /* Simulate Command-?. */
						win_processKeyboardEquivalent (my shell, '?', modifiers | _motif_SHIFT_MASK);
						return;
					}
				} else if (kar >= VK_F1 && kar <= VK_F12) {   /* 112...123 */
					/*
					 * She has pressed one of the F keys.
					 */
					if (win_processKeyboardEquivalent (my shell, kar - VK_F1 + motif_F1, modifiers)) return;
					/* Let windowProc handle Alt-F4 etc. */
				/*
				 * If the Command key is pressed with a printable character, this is often a menu shortcut.
				 */
				} else if (modifiers & _motif_COMMAND_MASK) {
					if (MEMBER (me, Text) && (kar == 'X' || kar == 'C' || kar == 'V' || kar == 'Z')) {
						;   // Let window proc handle text editing.
					} else if (kar >= 186) {
						int shift = modifiers & _motif_SHIFT_MASK;
						/*
						 * BUG: The following is not internationally correct.
						 */
						if (kar == 186 && win_processKeyboardEquivalent (my shell, shift ? ':' : ';', modifiers) ||
						    kar == 187 && win_processKeyboardEquivalent (my shell, shift ? '+' : '=', modifiers) ||
						    kar == 188 && win_processKeyboardEquivalent (my shell, shift ? '<' : ',', modifiers) ||
						    kar == 189 && win_processKeyboardEquivalent (my shell, shift ? '_' : '-', modifiers) ||
						    kar == 190 && win_processKeyboardEquivalent (my shell, shift ? '>' : '.', modifiers) ||
						    kar == 191 && win_processKeyboardEquivalent (my shell, shift ? '?' : '/', modifiers) ||
						    kar == 192 && win_processKeyboardEquivalent (my shell, shift ? '~' : '`', modifiers) ||
						    kar == 219 && win_processKeyboardEquivalent (my shell, shift ? '{' : '[', modifiers) ||   /* Alt-GR-ringel-s is here. */
						    kar == 220 && win_processKeyboardEquivalent (my shell, shift ? '|' : '\\', modifiers) ||
						    kar == 221 && win_processKeyboardEquivalent (my shell, shift ? '}' : ']', modifiers) ||
						    kar == 222 && win_processKeyboardEquivalent (my shell, shift ? '\"' : '\'', modifiers))
						{
							return;
						}
					} else {
						if (win_processKeyboardEquivalent (my shell, kar, modifiers)) return;   /* Handle shortcuts like Ctrl-T and Ctrl-Alt-T. */
						/* Let window proc handle international Alt-GR (= Ctrl-Alt) sequences, which are plain characters. */
					}
				}
				/* Other characters: to text. */
			}
			/* Not me or not my shell: let windowProc handle. */
		} else if (message -> message == WM_CHAR) {
			int kar = LOWORD (message -> wParam);
			/*
			 * Catch character messages to push buttons and toggle buttons:
			 * divert them to a drawing area, if possible.
			 */
			Widget me = (Widget) GetWindowLong (message -> hwnd, GWL_USERDATA);
			if (me && MEMBER2 (me, PushButton, ToggleButton)) {
				Widget drawingArea = _motif_findDrawingArea (my shell);
				if (drawingArea) {
					if (drawingArea -> inputCallback) {
						WinDrawingAreaEvent event;
						event. message = WM_CHAR;
						event. key = kar;
						event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;
						drawingArea -> inputCallback (drawingArea, drawingArea -> inputClosure, (XtPointer) & event);
					}
					return;
				}
			}
		} else if (message -> message == WM_LBUTTONDOWN) {
			/*
			 * Catch mouse-down messages to cascade buttons:
			 * post the associated menu, if any.
			 */
			Widget me = (Widget) GetWindowLong (message -> hwnd, GWL_USERDATA);
			if (me && MEMBER (me, CascadeButton) && my subMenuId) {
				RECT rect;
				GetWindowRect (my window, & rect);
				TrackPopupMenu (my subMenuId -> nat.menu.handle, TPM_LEFTBUTTON |
					TPM_LEFTALIGN | TPM_TOPALIGN, rect.left, rect.bottom - 3, 0, my parent -> window, NULL);
				return;
			}
		}
		TranslateMessage (xevent);   // Generate WM_CHAR messages.
		DispatchMessage (xevent);
	#endif
}

void XtAppMainLoop (XtAppContext appctxt) {
	for (;;) {
		XEvent event;
		XtAppNextEvent (appctxt, & event);
		XtDispatchEvent (& event);
	}
}

#if win
	extern int main (int argc, char *argv []);
	int APIENTRY WinMain (HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int commandShow) {
		int argc = 4;
		char instanceString [20], commandShowString [20], *argv [4];
		(void) previousInstance;
		argv [0] = "dummy";
		sprintf (instanceString, "%ld", (long) instance);
		sprintf (commandShowString, "%d", commandShow);
		argv [1] = & instanceString [0];
		argv [2] = & commandShowString [0];
		argv [3] = commandLine;
		return main (argc, & argv [0]);
	}

	static void on_close (HWND window) {
		Widget me = (Widget) GetWindowLong (window, GWL_USERDATA);
		if (me) {
			if (my widgetClass == xmShellWidgetClass) {
				int deleteResponse = my deleteResponse;   /* Save this, in case the callback should kill the widget (XmDO_NOTHING). */
				Widget parent = my parent;
				if (my motif.shell.goAwayCallback)
					my motif.shell.goAwayCallback (me, my motif.shell.goAwayClosure, NULL);
				if (deleteResponse == XmDESTROY) {
					XtDestroyWidget (me);
				} else if (deleteResponse == XmUNMAP) {
					/*
						Unmapping is not the same as unmanaging.
						One of the oldest bugs in Praat was that in the Windows edition
						the Picture window could not be closed a second time,
						and that after it had been closed and opened the scroll bars did not move when
						the user resized the window.
						The solution was to use only some of the code from XtUnmanageChild here,
						without clearing the 'managed' attribute.
					*/
					_GuiText_handleFocusLoss (my textFocus);
					ShowWindow (my window, SW_HIDE);
				}
				return;
			} else FORWARD_WM_CLOSE (window, DefWindowProc);
		} else FORWARD_WM_CLOSE (window, DefWindowProc);
	}
	static Widget findItem (Widget menu, int id) {
		Widget child = menu -> firstChild;
		for (child = menu -> firstChild; child != NULL; child = child -> nextSibling) {
			if (child -> widgetClass == xmPulldownMenuWidgetClass) {
				Widget result = findItem (child, id);
				if (result) return result;
			} else {
				Melder_assert (MEMBER4 (child, PushButton, ToggleButton, CascadeButton, Separator));
				if (child -> nat.entry.id == id) return child;
			}
		}
		return NULL;
	}
	static void on_command (HWND window, int id, HWND controlWindow, UINT codeNotify) {
		Widget parent = (Widget) GetWindowLong (window, GWL_USERDATA);
		if (parent) {
			if (controlWindow) {
				Widget control = (Widget) GetWindowLong (controlWindow, GWL_USERDATA);
				if (control) {
					switch (control -> widgetClass) {
						/*
						 * Clicking on a button takes away the focus from a text widget.
						 * We have already been notified of that by EN_KILLFOCUS.
						 * We have to put the focus back (see GuiText.c, parenthesized discussion 1.4);
						 * this has to be done before calling the callbacks, because these may destroy or hide the text widget.
						 */
						case xmPushButtonWidgetClass:
							/*
							 * If EN_KILLFOCUS had not cleared the global text focus,
							 * the following message would not actually do SetFocus!
							 */
							_GuiText_setTheTextFocus (control -> shell -> textFocus);
							if (control -> activateCallback)
								control -> activateCallback (control, control -> activateClosure, 0);
							break;
						case xmToggleButtonWidgetClass:
							_GuiText_setTheTextFocus (control -> shell -> textFocus);
							_Gui_callCallbacks (control, & control -> motif.toggleButton.valueChangedCallbacks, 0);
							break;
						case xmListWidgetClass:
							if (codeNotify == LBN_SELCHANGE) {
								if (control -> extendedSelectionCallback)
									control -> extendedSelectionCallback (control, control -> extendedSelectionClosure, 0);
							} else FORWARD_WM_COMMAND (window, id, controlWindow, codeNotify, DefWindowProc);
							break;
						case xmTextWidgetClass:
							if (codeNotify == EN_CHANGE) {
								_GuiText_handleValueChanged (control);
							} else if (codeNotify == EN_SETFOCUS) {
								_GuiText_handleFocusReception (control);
							} else if (codeNotify == EN_KILLFOCUS) {
								_GuiText_handleFocusLoss (control);   /* For button clicks (see above). */
							}
							break;
						default: break;
					}
				} else FORWARD_WM_COMMAND (window, id, controlWindow, codeNotify, DefWindowProc);
			} else {   // menu choice
				Widget menuBar = NULL;
				if (MEMBER (parent, Shell))
					menuBar = parent -> nat.shell.menuBar;
				else if (MEMBER (parent, RowColumn))
					menuBar = parent;
				if (menuBar) {
					Widget item = findItem (menuBar, id);
					if (item) {
						if (item -> widgetClass == xmPushButtonWidgetClass) {
							if (item -> activateCallback)
								item -> activateCallback (item, item -> activateClosure, 0);
						} else if (item -> widgetClass == xmToggleButtonWidgetClass) {
							XmToggleButtonGadgetSetState (item, 1 - XmToggleButtonGadgetGetState (item), False);
							_Gui_callCallbacks (item, & item -> motif.toggleButton.valueChangedCallbacks, 0);
						}
					}
				}
			}
		} else FORWARD_WM_COMMAND (window, id, controlWindow, codeNotify, DefWindowProc);
	}
	static void on_destroy (HWND window) {
		(void) window;
	}
	static void on_lbuttonDown (HWND window, BOOL doubleClick, int x, int y, UINT flags) {
		Widget me = (Widget) GetWindowLong (window, GWL_USERDATA);
		if (me) {
			if (MEMBER (me, DrawingArea)) {
				if (my inputCallback) {
					WinDrawingAreaEvent event;
					event. message = WM_LBUTTONDOWN;
					event. x = x;
					event. y = y;
					event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;
					my inputCallback (me, my inputClosure, (XtPointer) & event);
				}
			} else FORWARD_WM_LBUTTONDOWN (window, doubleClick, x, y, flags, DefWindowProc);
		} else FORWARD_WM_LBUTTONDOWN (window, doubleClick, x, y, flags, DefWindowProc);
	}
	static void on_paint (HWND window) {
		Widget me = (Widget) GetWindowLong (window, GWL_USERDATA);
		if (me) {
			if (my widgetClass == xmDrawingAreaWidgetClass) {
				PAINTSTRUCT paintStruct;
				BeginPaint (window, & paintStruct);
				if (my exposeCallback)
					my exposeCallback (me, my exposeClosure, (XtPointer) & paintStruct);
				EndPaint (window, & paintStruct);
				return;
			} else FORWARD_WM_PAINT (window, DefWindowProc);
		} else FORWARD_WM_PAINT (window, DefWindowProc);
	}
	static void on_hscroll (HWND window, HWND controlWindow, UINT code, int pos) {
		Widget parent = (Widget) GetWindowLong (window, GWL_USERDATA);
		if (parent) {
			Widget control = (Widget) GetWindowLong (controlWindow, GWL_USERDATA);
			if (control) {
				on_scroll (control, code, pos);
			} else FORWARD_WM_HSCROLL (window, controlWindow, code, pos, DefWindowProc);
		} else FORWARD_WM_HSCROLL (window, controlWindow, code, pos, DefWindowProc);
	}
	static void on_vscroll (HWND window, HWND controlWindow, UINT code, int pos) {
		Widget parent = (Widget) GetWindowLong (window, GWL_USERDATA);
		if (parent) {
			Widget control = (Widget) GetWindowLong (controlWindow, GWL_USERDATA);
			if (control) {
				on_scroll (control, code, pos);
			} else FORWARD_WM_VSCROLL (window, controlWindow, code, pos, DefWindowProc);
		} else FORWARD_WM_VSCROLL (window, controlWindow, code, pos, DefWindowProc);
	}
	static void on_size (HWND window, UINT state, int cx, int cy) {
		Widget me = (Widget) GetWindowLong (window, GWL_USERDATA);
		if (me && MEMBER (me, Shell) && (state == SIZE_RESTORED || state == SIZE_MAXIMIZED)) {
			int oldWidth = my width, oldHeight = my height;
			int newWidth = cx;
			int newHeight = cy;
			my width = newWidth;
			my height = newHeight;
			FORWARD_WM_SIZE (window, state, cx, cy, DefWindowProc);
			if (! my managed) {
			} else if (my nat.shell.duringMoveWindow)   // Yeah, a BUG fix. Only react to user actions.
				my nat.shell.duringMoveWindow = False;
			else if (newWidth != oldWidth || newHeight != oldHeight) {
				shellResizeWidget (me, 0, 0, newWidth - oldWidth, newHeight - oldHeight);
			}
		} else FORWARD_WM_SIZE (window, state, cx, cy, DefWindowProc);
	}
	static void on_char (HWND window, TCHAR kar, int repeat) {
		Widget me = (Widget) GetWindowLong (window, GWL_USERDATA);
		if (me) {
			if (MEMBER (me, Shell)) {
				Widget drawingArea = _motif_findDrawingArea (me);
				if (drawingArea) {
					Widget textFocus = drawingArea -> shell -> textFocus;
					if (drawingArea -> inputCallback) {
						WinDrawingAreaEvent event;
						event. message = WM_CHAR;
						event. key = kar;
						event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;
						drawingArea -> inputCallback (drawingArea, drawingArea -> inputClosure, (XtPointer) & event);
					}
				} else {
					FORWARD_WM_CHAR (window, kar, repeat, DefWindowProc);
				}
			} else FORWARD_WM_CHAR (window, kar, repeat, DefWindowProc);
		} else FORWARD_WM_CHAR (window, kar, repeat, DefWindowProc);
	}
	static void on_move (HWND window, int x, int y) {
		Widget me = (Widget) GetWindowLong (window, GWL_USERDATA);
		/*if (me && MEMBER (me, Shell)) {
			my x = x - ( my motif.shell.isDialog ? GetSystemMetrics (SM_CXFIXEDFRAME) : GetSystemMetrics (SM_CXSIZEFRAME) );
			my y = y - GetSystemMetrics (SM_CYCAPTION) - ( my motif.shell.isDialog ? GetSystemMetrics (SM_CYFIXEDFRAME) :
				GetSystemMetrics (SM_CYSIZEFRAME) + GetSystemMetrics (SM_CYMENU) );
		}*/
		FORWARD_WM_MOVE (window, x, y, DefWindowProc);
	}
	static HBRUSH on_ctlColorStatic (HWND window, HDC hdc, HWND controlWindow, int type) {
		Widget parent = (Widget) GetWindowLong (window, GWL_USERDATA);
		(void) type;
		if (parent) {
			Widget control = (Widget) GetWindowLong (controlWindow, GWL_USERDATA);
			if (control) {
				SetBkMode (hdc, TRANSPARENT);
				return GetStockBrush (LTGRAY_BRUSH);
			}
		}
		return FORWARD_WM_CTLCOLORSTATIC (window, hdc, controlWindow, DefWindowProc);
	}
	static HBRUSH on_ctlColorBtn (HWND window, HDC hdc, HWND controlWindow, int type) {
		Widget parent = (Widget) GetWindowLong (window, GWL_USERDATA);
		(void) type;
		if (parent) {
			Widget control = (Widget) GetWindowLong (controlWindow, GWL_USERDATA);
			if (control) {
				SetBkMode (hdc, TRANSPARENT);
				return GetStockBrush (LTGRAY_BRUSH);
			}
		}
		return FORWARD_WM_CTLCOLORBTN (window, hdc, controlWindow, DefWindowProc);
	}
	static void on_activate (HWND window, UINT state, HWND hActive, BOOL minimized) {
		Widget me = (Widget) GetWindowLong (window, GWL_USERDATA);
		if (me && my widgetClass == xmShellWidgetClass) {
			if (state == WA_INACTIVE || minimized) {
				_GuiText_handleFocusLoss (my textFocus);
			} else {
				_GuiText_setTheTextFocus (my textFocus);
			}
			return;
		} else FORWARD_WM_ACTIVATE (window, state, hActive, minimized, DefWindowProc);
	}
	static LRESULT CALLBACK windowProc (HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
			HANDLE_MSG (window, WM_CLOSE, on_close);
			HANDLE_MSG (window, WM_COMMAND, on_command);
			HANDLE_MSG (window, WM_DESTROY, on_destroy);
			HANDLE_MSG (window, WM_LBUTTONDOWN, on_lbuttonDown);
			HANDLE_MSG (window, WM_PAINT, on_paint);
			HANDLE_MSG (window, WM_HSCROLL, on_hscroll);
			HANDLE_MSG (window, WM_VSCROLL, on_vscroll);
			HANDLE_MSG (window, WM_SIZE, on_size);
			HANDLE_MSG (window, WM_CHAR, on_char);
			HANDLE_MSG (window, WM_MOVE, on_move);
			HANDLE_MSG (window, WM_CTLCOLORBTN, on_ctlColorBtn);
			HANDLE_MSG (window, WM_CTLCOLORSTATIC, on_ctlColorStatic);
			HANDLE_MSG (window, WM_ACTIVATE, on_activate);
			case WM_USER: {
				/*if (IsIconic (window)) ShowWindow (window, SW_RESTORE);
				SetForegroundWindow (window);*/
				return theUserMessageCallback ? theUserMessageCallback () : 1;
			}
			default: return DefWindowProc (window, message, wParam, lParam);
		}
	}
	int motif_win_mouseStillDown (void) {
		XEvent event;
		if (! GetCapture ()) SetCapture (theApplicationShell -> window);
		if (PeekMessage (& event, 0, 0, 0, PM_REMOVE)) {
			if (event. message == WM_LBUTTONUP) {
				DispatchMessage (& event);
				ReleaseCapture ();
				return False;
			}
		}
		return True;
	}
	void motif_win_setUserMessageCallback (int (*userMessageCallback) (void)) {
		theUserMessageCallback = userMessageCallback;
	}
#endif
#if mac
	void motif_mac_setUserMessageCallbackA (int (*userMessageCallback) (char *message)) {
		theUserMessageCallbackA = userMessageCallback;
	}
	void motif_mac_setUserMessageCallbackW (int (*userMessageCallback) (wchar_t *message)) {
		theUserMessageCallbackW = userMessageCallback;
	}
#endif
void motif_setOpenDocumentCallback (int (*openDocumentCallback) (MelderFile file)) {
	theOpenDocumentCallback = openDocumentCallback;
}
void motif_setQuitApplicationCallback (int (*quitApplicationCallback) (void)) {
	theQuitApplicationCallback = quitApplicationCallback;
}
#endif
/* End of file motifEmulator.c */
