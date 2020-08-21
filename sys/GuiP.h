#ifndef _GuiP_h_
#define _GuiP_h_
/* GuiP.h
 *
 * Copyright (C) 1993-2013,2015-2018,2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Gui.h"

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
	#include <windows.h>
	#include <commctrl.h>

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
		integer magicNumber;
		uinteger widgetClass;
		GuiObject parent, previousSibling, nextSibling, firstChild;
		autostring32 name;
		bool inMenu, nativized, managed, insensitive;
		GuiObject textFocus;   /* For shells. */
		GuiObject shell;   /* My shell ancestor. */

		/* Native. */

		HWND window;
		union {
			/* XtShell: */
			struct { GuiObject menuBar; int uniqueItemID, duringMoveWindow; } shell;

			/* XmPushButton (if in menu), XmToggleButton (if in menu), XmCascadeButton (if in menu): */
			struct { HMENU handle; ULONG_PTR id; } entry;

			/* XmMenuBar, XmPulldownMenu: */
			struct { HMENU handle; ULONG_PTR id; } menu;
		} nat;

		/* Motif */

		union {
			struct { GuiObject horizontalBar, verticalBar, clipWindow, workWindow; } scrolledWindow;
			struct { XtCallbackList moveCallbacks; } drawingArea;
			struct { bool active, isDialog, canFullScreen;
				uinteger lowAccelerators [8]; XtCallbackProc goAwayCallback; XtPointer goAwayClosure; } shell;
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
		integer increment, pageIncrement, sliderSize;   /* For scroll bars. */
		integer minimum, maximum, value;   /* For scales and scroll bars. */

		XtCallbackProc activateCallback, destroyCallback;
		XtPointer activateClosure, destroyClosure;
		int leftAttachment, rightAttachment, topAttachment, bottomAttachment;
		int leftOffset, rightOffset, topOffset, bottomOffset;
		int leftPosition, rightPosition, topPosition, bottomPosition;
		int deleteResponse;   /* For shells. */
		void *userData;
	};

	extern struct Gui {
		GuiObject textFocus;
		bool duringUpdate;
		HINSTANCE instance;   // first argument of WinMain
		int commandShow;   // another argument of WinMain
	} theGui;

	void _Gui_callCallbacks (GuiObject w, XtCallbackList *callbacks, XtPointer call);
	char32 * _GuiWin_getDrawingAreaClassName (void);

	GuiObject _Gui_initializeWidget (int widgetClass, GuiObject parent, conststring32 name);
	void _Gui_invalidateWidget (GuiObject me);
	void _Gui_validateWidget (GuiObject me);
	void _Gui_manageScrolledWindow (GuiObject me);
	void _GuiNativeControl_check (GuiObject me, Boolean value);
	void _GuiNativeControl_destroy (GuiObject me);
	void _GuiNativeControl_setTitle (GuiObject me);
	void _GuiNativeControl_show (GuiObject me);
	void _GuiNativeControl_hide (GuiObject me);
	void _GuiNativeControl_setSensitive (GuiObject me);
	char32 * _GuiWin_expandAmpersands (conststring32 title);

	/********** GuiButton.cpp **********/
	void _GuiWinButton_destroy (GuiObject widget);
	void _GuiWinButton_handleClick (GuiObject widget);
	bool _GuiWinButton_tryToHandleShortcutKey (GuiObject widget);

	/********** GuiCheckButton.cpp **********/
	void _GuiWinCheckButton_destroy (GuiObject widget);
	void _GuiWinCheckButton_handleClick (GuiObject widget);

	/********** GuiDrawingArea.cpp **********/
	void _GuiWinDrawingArea_destroy (GuiObject widget);
	void _GuiWinDrawingArea_update (GuiObject widget);
	void _GuiWinDrawingArea_handleMouse (GuiObject widget, structGuiDrawingArea_MouseEvent::Phase phase, int x, int y);
	void _GuiWinDrawingArea_handleKey (GuiObject widget, TCHAR kar);
	void _GuiWinDrawingArea_shellResize (GuiObject widget);

	/********** GuiLabel.cpp **********/
	void _GuiWinLabel_destroy (GuiObject widget);

	/********** GuiList.cpp **********/
	void _GuiWinList_destroy (GuiObject widget);
	void _GuiWinList_map (GuiObject widget);
	void _GuiWinList_destroy (GuiObject widget);
	void _GuiWinList_handleClick (GuiObject widget);

	/********** GuiRadioButton.cpp **********/
	void _GuiWinRadioButton_destroy (GuiObject widget);
	void _GuiWinRadioButton_handleClick (GuiObject widget);

	/********** GuiScale.cpp **********/
	void _GuiWinScale_destroy (GuiObject widget);

	/********** GuiScrollBar.cpp **********/
	void _GuiWinScrollBar_destroy (GuiObject widget);

	/********** GuiScrolledWindow.cpp **********/
	void _GuiWinScrolledWindow_destroy (GuiObject widget);

	/********** GuiText.cpp **********/
	void _GuiWinText_destroy (GuiObject widget);
	void _GuiWinText_map (GuiObject widget);
	void _GuiText_handleFocusReception (GuiObject widget);
	void _GuiText_handleFocusLoss (GuiObject widget);
	void _GuiText_setTheTextFocus (GuiObject widget);
	void _GuiText_handleValueChanged (GuiObject widget);
	void _GuiText_unmanage (GuiObject widget);
	void _GuiText_init ();
	void _GuiText_exit ();

#endif

/* End of file GuiP.h */
#endif
