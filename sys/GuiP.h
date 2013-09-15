#ifndef _GuiP_h_
#define _GuiP_h_
/* GuiP.h
 *
 * Copyright (C) 1993-2011,2012,2013 Paul Boersma
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

void _GuiObject_position (GuiObject me, int left, int right, int top, int bottom, GuiForm parent);
void * _GuiObject_getUserData (GuiObject me);
void _GuiObject_setUserData (GuiObject me, void *userData);

class GuiControlBlockValueChangedCallbacks {
	private:
		GuiControl d_control;
	public:
		GuiControlBlockValueChangedCallbacks (GuiControl control) : d_control (control) {
			d_control -> d_blockValueChangedCallbacks = true;
		}
		~GuiControlBlockValueChangedCallbacks () {
			d_control -> d_blockValueChangedCallbacks = false;
		}
};

#if gtk
	void GuiGtk_initialize ();
#elif motif
	#if win
		#include <windows.h>
		#include <commctrl.h>
	#endif

	#if win || mac

		#define MEMBER(widget,klas)  ((widget -> widgetClass & xm##klas##WidgetClass) != 0)
		#define MEMBER2(widget,klas1,klas2)  ((widget -> widgetClass & (xm##klas1##WidgetClass | xm##klas2##WidgetClass)) != 0)
		#define MEMBER3(widget,klas1,klas2,klas3)  ((widget -> widgetClass & (xm##klas1##WidgetClass | xm##klas2##WidgetClass | xm##klas3##WidgetClass)) != 0)
		#define MEMBER4(widget,klas1,klas2,klas3,klas4)  ((widget -> widgetClass & (xm##klas1##WidgetClass | xm##klas2##WidgetClass | xm##klas3##WidgetClass | xm##klas4##WidgetClass)) != 0)
		#define MEMBER5(widget,klas1,klas2,klas3,klas4,klas5)  ((widget -> widgetClass & (xm##klas1##WidgetClass | xm##klas2##WidgetClass | xm##klas3##WidgetClass | xm##klas4##WidgetClass | xm##klas5##WidgetClass)) != 0)

		#define MAXNUM_CALLBACKS  3   /* E.g., maximum number of activate callbacks for a PushButton. */

		typedef struct {
			struct { XtCallbackProc proc; XtPointer closure; } pairs [MAXNUM_CALLBACKS];
		} XtCallbackList;

		class structGuiObject {
		public:
			long magicNumber;
			unsigned long widgetClass;
			GuiObject parent, previousSibling, nextSibling, firstChild;
			wchar_t *name;
			bool inMenu, nativized, managed, insensitive;
			GuiObject textFocus;   /* For shells. */
			GuiObject shell;   /* My shell ancestor. */

			/* Native. */

			#if win
				HWND window;
				union {
					/* XtShell: */
					struct { GuiObject menuBar; int uniqueItemID, duringMoveWindow; } shell;

					/* XmPushButton (if in menu), XmToggleButton (if in menu), XmCascadeButton (if in menu): */
					struct { HMENU handle; ULONG_PTR id; } entry;

					/* XmMenuBar, XmPulldownMenu: */
					struct { HMENU handle; ULONG_PTR id; } menu;
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
				struct { GuiObject horizontalBar, verticalBar, clipWindow, workWindow; } scrolledWindow;
				struct { XtCallbackList moveCallbacks; } drawingArea;
				struct { bool active, isDialog, canFullScreen;
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
			GuiObject defaultButton, cancelButton;   /* For forms and shells. */
			int dialogStyle;   /* For forms and shells. */
			GuiObject messageText;   /* For message boxes. */
			bool autoUnmanage;   /* For bulletin boards. */
			GuiObject subMenuId, popUpButton;   /* For cascade buttons and their menus. */
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
			GuiObject textFocus;
			bool duringUpdate;
			#if win
				HINSTANCE instance;   /* First argument of WinMain. */
			#elif mac
				WindowPtr modalDialog;
			#endif
		} theGui;

		void _Gui_callCallbacks (GuiObject w, XtCallbackList *callbacks, XtPointer call);
		#if mac
			void _GuiMac_clipOnParent (GuiObject me);
			void _GuiMac_clipOffValid (GuiObject me);
			void _GuiMac_clipOffInvalid (GuiObject me);
		#elif win
			wchar_t * _GuiWin_getDrawingAreaClassName (void);
		#endif
		GuiObject _Gui_initializeWidget (int widgetClass, GuiObject parent, const wchar_t *name);
		void _Gui_invalidateWidget (GuiObject me);
		void _Gui_validateWidget (GuiObject me);
		void _Gui_manageScrolledWindow (GuiObject me);
		void _GuiNativeControl_check (GuiObject me, Boolean value);
		void _GuiNativeControl_destroy (GuiObject me);
		void _GuiNativeControl_setFont (GuiObject me, int style, int size);
		void _GuiNativeControl_setTitle (GuiObject me);
		void _GuiNativeControl_show (GuiObject me);
		void _GuiNativeControl_hide (GuiObject me);
		void _GuiNativeControl_setSensitive (GuiObject me);
		wchar_t * _GuiWin_expandAmpersands (const wchar_t *title);

		/********** GuiButton.cpp **********/
		#if win
			void _GuiWinButton_destroy (GuiObject widget);
			void _GuiWinButton_handleClick (GuiObject widget);
			bool _GuiWinButton_tryToHandleShortcutKey (GuiObject widget);
		#elif mac
			void _GuiMacButton_destroy (GuiObject widget);
			void _GuiMacButton_handleClick (GuiObject widget, EventRecord *macEvent);
			bool _GuiMacButton_tryToHandleShortcutKey (GuiObject widget, EventRecord *macEvent);
		#endif

		/********** GuiCheckButton.cpp **********/
		#if win
			void _GuiWinCheckButton_destroy (GuiObject widget);
			void _GuiWinCheckButton_handleClick (GuiObject widget);
		#elif mac
			void _GuiMacCheckButton_destroy (GuiObject widget);
			void _GuiMacCheckButton_handleClick (GuiObject widget, EventRecord *macEvent);
		#endif

		/********** GuiDrawingArea.cpp **********/
		#if win
			void _GuiWinDrawingArea_destroy (GuiObject widget);
			void _GuiWinDrawingArea_update (GuiObject widget);
			void _GuiWinDrawingArea_handleClick (GuiObject widget, int x, int y);
			void _GuiWinDrawingArea_handleKey (GuiObject widget, TCHAR kar);
			void _GuiWinDrawingArea_shellResize (GuiObject widget);
		#elif mac
			void _GuiMacDrawingArea_destroy (GuiObject widget);
			void _GuiMacDrawingArea_update (GuiObject widget);
			void _GuiMacDrawingArea_handleClick (GuiObject widget, EventRecord *macEvent);
			bool _GuiMacDrawingArea_tryToHandleKey (GuiObject widget, EventRecord *macEvent);
			void _GuiMacDrawingArea_shellResize (GuiObject widget);
		#endif

		/********** GuiLabel.cpp **********/
		#if win
			void _GuiWinLabel_destroy (GuiObject widget);
		#elif mac
			void _GuiMacLabel_destroy (GuiObject widget);
		#endif

		/********** GuiList.cpp **********/
		#if win
			void _GuiWinList_destroy (GuiObject widget);
			void _GuiWinList_map (GuiObject widget);
			void _GuiWinList_destroy (GuiObject widget);
			void _GuiWinList_handleClick (GuiObject widget);
		#elif mac
			void _GuiMacList_destroy (GuiObject widget);
			void _GuiMacList_map (GuiObject widget);
			void _GuiMacList_activate (GuiObject widget, bool activate);
			void _GuiMacList_handleClick (GuiObject widget, EventRecord *event);
			void _GuiMacList_handleControlClick (GuiObject widget, EventRecord *event);
			void _GuiMacList_move (GuiObject widget);
			void _GuiMacList_resize (GuiObject widget);
			void _GuiMacList_shellResize (GuiObject widget);
			void _GuiMacList_update (GuiObject widget, RgnHandle visRgn);
		#endif

		/********** GuiRadioButton.cpp **********/
		#if win
			void _GuiWinRadioButton_destroy (GuiObject widget);
			void _GuiWinRadioButton_handleClick (GuiObject widget);
		#elif mac
			void _GuiMacRadioButton_destroy (GuiObject widget);
			void _GuiMacRadioButton_handleClick (GuiObject widget, EventRecord *macEvent);
		#endif

		/********** GuiScale.cpp **********/
		#if win
			void _GuiWinScale_destroy (GuiObject widget);
		#elif mac
			void _GuiMacScale_destroy (GuiObject widget);
		#endif

		/********** GuiScrollBar.cpp **********/
		#if win
			void _GuiWinScrollBar_destroy (GuiObject widget);
		#elif mac
			void _GuiMacScrollBar_destroy (GuiObject widget);
		#endif

		/********** GuiScrolledWindow.cpp **********/
		#if win
			void _GuiWinScrolledWindow_destroy (GuiObject widget);
		#elif mac
			void _GuiMacScrolledWindow_destroy (GuiObject widget);
		#endif

		/********** GuiText.cpp **********/
		#if win
			void _GuiWinText_destroy (GuiObject widget);
			void _GuiWinText_map (GuiObject widget);
		#elif mac
			void _GuiMacText_destroy (GuiObject widget);
			void _GuiMacText_map (GuiObject widget);
		#endif
		void _GuiText_handleFocusReception (GuiObject widget);
		void _GuiText_handleFocusLoss (GuiObject widget);
		#if mac
		void _GuiMac_clearTheTextFocus (void);
		#endif
		void _GuiText_setTheTextFocus (GuiObject widget);
		void _GuiText_handleValueChanged (GuiObject widget);
		void _GuiText_unmanage (GuiObject widget);
		#if mac
			void _GuiMacText_move (GuiObject widget);
			void _GuiMacText_shellResize (GuiObject widget);
			void _GuiMacText_resize (GuiObject widget);
			void _GuiMacText_update (GuiObject widget);
			int _GuiMacText_tryToHandleKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, GuiObject widget, unsigned char keyCode, unsigned char charCode, EventRecord *event);
			int _GuiMacText_tryToHandleReturnKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, GuiObject widget, EventRecord *event);
			int _GuiMacText_tryToHandleClipboardShortcut (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, GuiObject widget, unsigned char charCode, EventRecord *event);
			void _GuiMacText_handleClick (GuiObject widget, EventRecord *event);
			void _GuiMac_makeTextCaretBlink (void);
		#endif
		void _GuiText_init (void);
		void _GuiText_exit (void);

	#endif
#endif

/* End of file GuiP.h */
#endif
