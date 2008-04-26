#ifndef _GuiP_h_
#define _GuiP_h_
/* GuiP.h
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
 * pb 2007/12/25
 */

#include "Gui.h"

/*
 * In GUI implementations, we order everything by ease of programming: Unix, Windows, Macintosh.
 */
#if defined (UNIX)
	#define uni 1
	#define win 0
	#define mac 0
#endif
#if defined (_WIN32)
	#define uni 0
	#define win 1
	#define mac 0
#endif
#if defined (macintosh)
	#define uni 0
	#define win 0
	#define mac 1
#endif

void _GuiObject_position (Widget me, int left, int right, int top, int bottom);
void * _GuiObject_getUserData (Widget me);
void _GuiObject_setUserData (Widget me, void *userData);

#if gtk
#elif motif
	#if win
		#include <windows.h>
		#include <commctrl.h>
	#endif

	#if win || mac

		/*
		#define MEMBER(widget,klas)  (widget -> widgetClass == xm##klas##WidgetClass)
		#define MEMBER2(widget,klas1,klas2)  (MEMBER (widget, klas1) || MEMBER (widget, klas2))
		#define MEMBER3(widget,klas1,klas2,klas3)  (MEMBER2 (widget, klas1, klas2) || MEMBER (widget, klas3))
		#define MEMBER4(widget,klas1,klas2,klas3,klas4)  (MEMBER3 (widget, klas1, klas2, klas3) || MEMBER (widget, klas4))
		#define MEMBER5(widget,klas1,klas2,klas3,klas4,klas5)  (MEMBER4 (widget, klas1, klas2, klas3, klas4) || MEMBER (widget, klas5))
		*/
		#define MEMBER(widget,klas)  ((widget -> widgetClass & xm##klas##WidgetClass) != 0)
		#define MEMBER2(widget,klas1,klas2)  ((widget -> widgetClass & (xm##klas1##WidgetClass | xm##klas2##WidgetClass)) != 0)
		#define MEMBER3(widget,klas1,klas2,klas3)  ((widget -> widgetClass & (xm##klas1##WidgetClass | xm##klas2##WidgetClass | xm##klas3##WidgetClass)) != 0)
		#define MEMBER4(widget,klas1,klas2,klas3,klas4)  ((widget -> widgetClass & (xm##klas1##WidgetClass | xm##klas2##WidgetClass | xm##klas3##WidgetClass | xm##klas4##WidgetClass)) != 0)
		#define MEMBER5(widget,klas1,klas2,klas3,klas4,klas5)  ((widget -> widgetClass & (xm##klas1##WidgetClass | xm##klas2##WidgetClass | xm##klas3##WidgetClass | xm##klas4##WidgetClass | xm##klas5##WidgetClass)) != 0)

		#define MAXNUM_CALLBACKS  3   /* E.g., maximum number of activate callbacks for a PushButton. */

		typedef struct {
			struct { XtCallbackProc proc; XtPointer closure; } pairs [MAXNUM_CALLBACKS];
		} XtCallbackList;

		struct structWidget {
			long magicNumber;
			unsigned long widgetClass;
			Widget parent, previousSibling, nextSibling, firstChild;
			wchar_t *name;
			bool inMenu, nativized, managed, insensitive;
			Widget textFocus;   /* For shells. */
			Widget shell;   /* My shell ancestor. */

			/* Native. */

			#if win
				HWND window;
				union {
					/* XtShell: */
					struct { Widget menuBar; int uniqueItemID, duringMoveWindow; } shell;

					/* XmPushButton (if in menu), XmToggleButton (if in menu), XmCascadeButton (if in menu): */
					struct { HMENU handle; int id; } entry;

					/* XmMenuBar, XmPulldownMenu: */
					struct { HMENU handle; int id; } menu;
				} nat;
			#elif mac
				Rect rect;   /* Window coordinates. */
				WindowPtr macWindow;
				bool isControl;
				struct {
					/* XtShell: */
					struct { WindowPtr ptr; ControlRef rootControl; } window;

					/* XmPushButton (if not in menu), XmToggleButton (if not in menu), XmScrollBar, XmLabel (sometimes), XmCascadeButton (if not in menu or bar): */
					struct { ControlHandle handle; bool isBevel, isPopup; } control;

					/* XmPushButton (if in menu), XmToggleButton (if in menu), XmCascadeButton (if in menu): */
					struct { MenuHandle handle; int item; } entry;

					/* XmPulldownMenu: */
					struct { MenuHandle handle; int item /* if cascading */; int id; } menu;
				} nat;
			#endif

			/* Motif */

			union {
				struct { Widget horizontalBar, verticalBar, clipWindow, workWindow; } scrolledWindow;
				struct { XtCallbackList moveCallbacks; } drawingArea;
				struct { int active, isDialog;
					unsigned long lowAccelerators [8]; XtCallbackProc goAwayCallback; XtPointer goAwayClosure; } shell;
				struct { unsigned char acceleratorChar; int acceleratorModifiers; } pushButton;
				struct { int inBar; } cascadeButton;
				struct { unsigned char acceleratorChar; int acceleratorModifiers; XtCallbackList valueChangedCallbacks; } toggleButton;
				struct { XtCallbackList valueChangedCallbacks, dragCallbacks; } scrollBar;
			} motiff;

			/* Resources. */

			int x, y, width, height;
			bool isRadioButton;   /* For radio buttons and check buttons. */
			int radioBehavior, rowColumnType;   /* For row-columns. */
			int orientation;   /* For row-columns and scroll bars. */
			Widget defaultButton, cancelButton;   /* For forms and shells. */
			int dialogStyle;   /* For forms and shells. */
			Widget messageText;   /* For message boxes. */
			bool autoUnmanage;   /* For bulletin boards. */
			Widget subMenuId, popUpButton;   /* For cascade buttons and their menus. */
			long increment, pageIncrement, sliderSize;   /* For scroll bars. */
			long minimum, maximum, value;   /* For scales and scroll bars. */

			XtCallbackProc activateCallback, destroyCallback;
			XtPointer activateClosure, destroyClosure;
			int leftAttachment, rightAttachment, topAttachment, bottomAttachment;
			int leftOffset, rightOffset, topOffset, bottomOffset;
			int leftPosition, rightPosition, topPosition, bottomPosition;
			int deleteResponse;   /* For shells. */
			void *userData;
		};

		#define my  me ->

		extern struct Gui {
			Widget textFocus;
			bool duringUpdate;
			#if win
				HINSTANCE instance;   /* First argument of WinMain. */
			#elif mac
				WindowPtr modalDialog;
			#endif
		} theGui;

		void _Gui_callCallbacks (Widget w, XtCallbackList *callbacks, XtPointer call);
		#if mac
			void _GuiMac_clipOnParent (Widget me);
			void _GuiMac_clipOffValid (Widget me);
			void _GuiMac_clipOffInvalid (Widget me);
		#elif win
			wchar_t * _GuiWin_getDrawingAreaClassName (void);
		#endif
		Widget _Gui_initializeWidget (int widgetClass, Widget parent, const wchar_t *name);
		void _Gui_invalidateWidget (Widget me);
		void _Gui_validateWidget (Widget me);
		void _Gui_manageScrolledWindow (Widget me);
		void _GuiNativeControl_check (Widget me, Boolean value);
		void _GuiNativeControl_destroy (Widget me);
		void _GuiNativeControl_setFont (Widget me, int size);
		void _GuiNativeControl_setTitle (Widget me);
		void _GuiNativeControl_show (Widget me);
		void _GuiNativeControl_hide (Widget me);
		void _GuiNativeControl_setSensitive (Widget me);
		wchar_t * _GuiWin_expandAmpersands (const wchar_t *title);

		/********** GuiButton.c **********/
		void _GuiWinMacButton_destroy (Widget widget);
		#if win
			void _GuiWinButton_handleClick (Widget widget);
			bool _GuiWinButton_tryToHandleShortcutKey (Widget widget);
		#elif mac
			void _GuiMacButton_handleClick (Widget widget, EventRecord *macEvent);
			bool _GuiMacButton_tryToHandleShortcutKey (Widget widget, EventRecord *macEvent);
		#endif

		/********** GuiCheckButton.c **********/
		void _GuiWinMacCheckButton_destroy (Widget widget);
		#if win
			void _GuiWinCheckButton_handleClick (Widget widget);
		#elif mac
			void _GuiMacCheckButton_handleClick (Widget widget, EventRecord *macEvent);
		#endif

		/********** GuiDrawingArea.c **********/
		void _GuiWinMacDrawingArea_destroy (Widget widget);
		#if win
			void _GuiWinDrawingArea_update (Widget widget);
			void _GuiWinDrawingArea_handleClick (Widget widget, int x, int y);
			void _GuiWinDrawingArea_handleKey (Widget widget, TCHAR kar);
			void _GuiWinDrawingArea_shellResize (Widget widget);
		#elif mac
			void _GuiMacDrawingArea_update (Widget widget);
			void _GuiMacDrawingArea_handleClick (Widget widget, EventRecord *macEvent);
			bool _GuiMacDrawingArea_tryToHandleKey (Widget widget, EventRecord *macEvent);
			void _GuiMacDrawingArea_shellResize (Widget widget);
		#endif

		/********** GuiLabel.c **********/
		void _GuiWinMacLabel_destroy (Widget widget);

		/********** GuiList.c **********/
		void _GuiWinMacList_destroy (Widget widget);
		void _GuiWinMacList_map (Widget widget);
		#if win
			void _GuiWinList_destroy (Widget widget);
			void _GuiWinList_handleClick (Widget widget);
		#elif mac
			void _GuiMacList_activate (Widget widget, bool activate);
			void _GuiMacList_handleClick (Widget widget, EventRecord *event);
			void _GuiMacList_handleControlClick (Widget widget, EventRecord *event);
			void _GuiMacList_move (Widget widget);
			void _GuiMacList_resize (Widget widget);
			void _GuiMacList_shellResize (Widget widget);
			void _GuiMacList_update (Widget widget, RgnHandle visRgn);
		#endif

		/********** GuiRadioButton.c **********/
		void _GuiWinMacRadioButton_destroy (Widget widget);
		#if win
			void _GuiWinRadioButton_handleClick (Widget widget);
		#elif mac
			void _GuiMacRadioButton_handleClick (Widget widget, EventRecord *macEvent);
		#endif

		/********** GuiText.c **********/
		void _GuiWinMacText_destroy (Widget widget);
		void _GuiWinMacText_map (Widget widget);
		void _GuiText_handleFocusReception (Widget widget);
		void _GuiText_handleFocusLoss (Widget widget);
		#if mac
		void _GuiMac_clearTheTextFocus (void);
		#endif
		void _GuiText_setTheTextFocus (Widget widget);
		void _GuiText_handleValueChanged (Widget widget);
		void _GuiText_unmanage (Widget widget);
		#if mac
			void _GuiMacText_move (Widget widget);
			void _GuiMacText_shellResize (Widget widget);
			void _GuiMacText_resize (Widget widget);
			void _GuiMacText_update (Widget widget);
			int _GuiMacText_tryToHandleKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, Widget widget, unsigned char keyCode, unsigned char charCode, EventRecord *event);
			int _GuiMacText_tryToHandleReturnKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, Widget widget, EventRecord *event);
			int _GuiMacText_tryToHandleClipboardShortcut (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, Widget widget, unsigned char charCode, EventRecord *event);
			void _GuiMacText_handleClick (Widget widget, EventRecord *event);
			void _GuiMac_makeTextCaretBlink (void);
		#endif
		void _GuiText_init (void);
		void _GuiText_exit (void);

	#endif
#endif

/* End of file GuiP.h */
#endif
