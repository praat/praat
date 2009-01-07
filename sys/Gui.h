#ifndef _Gui_h_
#define _Gui_h_
/* Gui.h
 *
 * Copyright (C) 1993-2008 Paul Boersma
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
 * 2008/03/24
 */

#ifdef USE_GTK
	#define gtk 1
	#define motif 0
#else
	#define gtk 0
	#define motif 1
#endif

#ifndef _melder_h_
	#include "melder.h"
#endif

#define GUI_ARGS  Widget w, XtPointer void_me, XtPointer call

#define GUI_IAM(klas)  (void) w; (void) void_me; (void) call; iam (klas);

#define Gui_LEFT_DIALOG_SPACING  20
#define Gui_RIGHT_DIALOG_SPACING  20
#define Gui_TOP_DIALOG_SPACING  14
#define Gui_BOTTOM_DIALOG_SPACING  20
#define Gui_HORIZONTAL_DIALOG_SPACING  12
#define Gui_VERTICAL_DIALOG_SPACING_SAME  12
#define Gui_VERTICAL_DIALOG_SPACING_DIFFERENT  20
#define Gui_TEXTFIELD_HEIGHT  Machine_getTextHeight ()
#define Gui_LABEL_HEIGHT  16
#define Gui_RADIOBUTTON_HEIGHT  18
#define Gui_RADIOBUTTON_SPACING  8
#define Gui_CHECKBUTTON_HEIGHT  20
#define Gui_LABEL_SPACING  8
#define Gui_OPTIONMENU_HEIGHT  20
#define Gui_PUSHBUTTON_HEIGHT  20
#define Gui_OK_BUTTON_WIDTH  69
#define Gui_CANCEL_BUTTON_WIDTH  69
#define Gui_APPLY_BUTTON_WIDTH  69

#define Gui_AUTOMATIC  -32768
#define Gui_HOMOGENEOUS  1

#if gtk
	#include <gtk/gtk.h>
	#include <gdk/gdk.h>
	#include <cairo/cairo.h>
	// GTK include files...
	typedef GMainContext *AppContext;
	typedef GtkWidget *Widget;
	typedef void *XtPointer;
	typedef gint Dimension;
	typedef gboolean Boolean;
	#define True 1
	#define False 0

#elif motif
	#if defined (UNIX)
		#include <Xm/Xm.h>
		#include <Xm/BulletinB.h>
		#include <Xm/CascadeB.h>
		#include <Xm/CascadeBG.h>
		#include <Xm/DialogS.h>
		#include <Xm/DrawingA.h>
		#include <Xm/Frame.h>
		#include <Xm/Form.h>
		#include <Xm/FileSB.h>
		#include <Xm/Label.h>
		#include <Xm/LabelG.h>
		#include <Xm/List.h>
		#include <Xm/MainW.h>
		#include <Xm/MessageB.h>
		#include <Xm/PushB.h>
		#include <Xm/PushBG.h>
		#include <Xm/RowColumn.h>
		#include <Xm/Scale.h>
		#include <Xm/ScrollBar.h>
		#include <Xm/ScrolledW.h>
		#include <Xm/SelectioB.h>
		#include <Xm/SeparatoG.h>
		#include <Xm/Separator.h>
		#include <Xm/Text.h>
		#include <Xm/TextF.h>
		#include <Xm/ToggleB.h>
		#include <Xm/ToggleBG.h>
		#ifdef sgi
			#include <X11/Xm/Protocols.h>
		#else
			#include <Xm/AtomMgr.h>
			#include <Xm/Protocols.h>
		#endif
	#else   // Motif on Macintosh or Windows
		#if defined (macintosh)
			#include "macport_on.h"
			#include <Carbon/Carbon.h>
			#include "macport_off.h"
		#endif
		#ifdef _WIN32
			#define Polygon PolygonNotWin
			#include <windows.h>
			#include <windowsx.h>
			#undef Polygon
		#endif

		/*
		 * Definitions of X11 types.
		 */
		#if defined (macintosh)
			typedef struct EventRecord XEvent;
		#else
			typedef MSG XEvent;
		#endif
		typedef unsigned char Boolean;
		typedef long Cardinal;
		typedef unsigned int Dimension;
		typedef int Position;
		typedef long Window;
		typedef char *String;
		typedef struct Display Display;
		/*typedef long Time;*/
		typedef long Atom;
		typedef struct { int /* elsewhere: char* */ name; long value; } Arg, *ArgList;

		/*
		 * Declarations of X11 functions.
		 */
		void XMapRaised (int displayDummy, Window window);
		#define DefaultScreenOfDisplay(d)  0
		int WidthOfScreen (int screen);
		int HeightOfScreen (int screen);

		/*
		 * Definitions of Xt types.
		 */
		typedef void *XtPointer;
		typedef struct structWidget *Widget;   // Opaque
		typedef Widget *WidgetList;
		typedef void *XtAppContext;
		typedef XtAppContext Context;
		typedef long XtWorkProcId, XtIntervalId;
		typedef void (*XtCallbackProc) (Widget w, XtPointer client_data, XtPointer call_data);
		typedef Boolean (*XtWorkProc) (XtPointer client_data);
		typedef void (*XtTimerCallbackProc) (XtPointer, XtIntervalId *);
		typedef unsigned long WidgetClass;
		#define False 0
		#define True 1

		/*
		 * Declarations of Xt functions.
		 */
		void XtAddCallback (Widget w, int kind, XtCallbackProc proc, XtPointer closure);
		XtIntervalId XtAppAddTimeOut (XtAppContext appContext, unsigned long interval,
			XtTimerCallbackProc timerProc, XtPointer closure);
		XtWorkProcId XtAppAddWorkProc (XtAppContext appContext, XtWorkProc workProc, XtPointer closure);
		void XtAppMainLoop (XtAppContext appContext);
		void XtAppNextEvent (XtAppContext appContext, XEvent *event);
		#define XtCalloc  Melder_calloc
		#define XtClass(w)  (w) -> widgetClass
		void XtDestroyWidget (Widget w);
		void XtDispatchEvent (XEvent *event);
		#define XtDisplay(w)  0
		Widget XtInitialize (void *dum1, const char *name,
			void *dum2, int dum3, unsigned int *argc, char **argv);
		Boolean XtIsManaged (Widget w);
		Boolean XtIsShell (Widget w);
		void XtManageChild (Widget w);
		void XtManageChildren (WidgetList children, Cardinal num_children);
		void XtMapWidget (Widget w);
		void XtNextEvent (XEvent *event);
		#define XtRealizeWidget  XtManageChild
		void XtRemoveTimeOut (XtIntervalId id);
		void XtRemoveWorkProc (XtWorkProcId id);
		void XtSetKeyboardFocus (Widget tree, Widget descendant);
		void XtSetSensitive (Widget w, Boolean value);
		void XtUnmanageChild (Widget self);
		void XtUnmanageChildren (WidgetList children, Cardinal num_children);
		Widget XtVaAppInitialize (XtAppContext *appContext, const char *name,
			void *dum1, int dum2, unsigned int *argc, char **argv, void *dum3, void *dum4);
		Widget XtVaCreateWidget (const char *name, int widgetClass, Widget parent, ...);
		Widget XtVaCreateManagedWidget (const char *name, int widgetClass, Widget parent, ...);
		void XtVaGetValues (Widget w, ...);
		void XtVaSetValues (Widget w, ...);
		Window XtWindow (Widget w);
		long Gui_getNumberOfMotifWidgets (void);

		/*
		 * Xm widget classes.
		 */
		#define xmBulletinBoardWidgetClass  0x00000001
		#define xmDrawingAreaWidgetClass  0x00000002
		#define xmFormWidgetClass  0x00000004
		#define xmFrameWidgetClass  0x00000008
		#define xmLabelWidgetClass  0x00000010
		#define xmListWidgetClass  0x00000020
		#define xmMenuBarWidgetClass  0x00000040
		#define xmPulldownMenuWidgetClass  0x00000100
		#define xmPushButtonWidgetClass  0x00000200
		#define xmRowColumnWidgetClass  0x00000400
		#define xmScaleWidgetClass  0x00000800
		#define xmScrollBarWidgetClass  0x00001000
		#define xmScrolledWindowWidgetClass  0x00002000
		#define xmSeparatorWidgetClass  0x00004000
		#define xmShellWidgetClass  0x00008000
		#define xmTextWidgetClass  0x00010000
		#define xmToggleButtonWidgetClass  0x00020000
		#define xmCascadeButtonWidgetClass  0x00040000
		#define topLevelShellWidgetClass  xmShellWidgetClass
		#define xmPushButtonGadgetClass  xmPushButtonWidgetClass
		#define xmCascadeButtonGadgetClass  xmCascadeButtonWidgetClass
		#define xmSeparatorGadgetClass  xmSeparatorWidgetClass
		#define xmToggleButtonGadgetClass  xmToggleButtonWidgetClass

		/*
		 * Xm resource names.
		 */
		enum {
			XmNnull,
			#define motif_RESOURCE(xxx)  xxx,
			#include "motifEmulator_resources.h"
			#undef motif_RESOURCE
			XmNend
		};

		/*
		 * Xm enumerated types.
		 * All start at 1, because we have to reserve zero for the dynamic default value.
		 * This is important for the resources set during creation.
		 */
		enum /* dialog styles */ { XmDIALOG_MODELESS = 1, XmDIALOG_FULL_APPLICATION_MODAL };
		enum /* orientation */ { XmVERTICAL = 1, XmHORIZONTAL };
		enum /* attachment */ { XmATTACH_NONE = 1, XmATTACH_FORM, XmATTACH_POSITION };
		enum /* rowColumn types */ { XmWORK_AREA = 1, XmMENU_BAR };
		enum /* delete responses */ { XmDESTROY = 1, XmUNMAP, XmDO_NOTHING };
		enum /* indicator types */ { XmONE_OF_MANY = 1, XmN_OF_MANY };

		/*
		 * Declarations of Xm functions.
		 */
		void XmAddWMProtocolCallback (Widget shell, Atom protocol, XtCallbackProc callback, char *closure);
		void XmAddWMProtocols (Widget shell, Atom *protocols, Cardinal num_protocols);   /* does nothing */
		Widget XmCreateBulletinBoard (Widget, const char *, ArgList, int);  
		Widget XmCreateBulletinBoardDialog (Widget, const char *, ArgList, int);  
		Widget XmCreateCascadeButton (Widget, const char *, ArgList, int);
		Widget XmCreateCascadeButtonGadget (Widget, const char *, ArgList, int);
		Widget XmCreateDialogShell (Widget, const char *, ArgList, int);
		Widget XmCreateForm (Widget, const char *, ArgList, int);
		Widget XmCreateFormDialog (Widget, const char *, ArgList, int);
		Widget XmCreateMenuBar (Widget, const char *, ArgList, int);
		Widget XmCreatePulldownMenu (Widget, const char *, ArgList, int);   
		Widget XmCreateRadioBox (Widget, const char *, ArgList, int);
		Widget XmCreateRowColumn (Widget, const char *, ArgList, int);
		Widget XmCreateScale (Widget, const char *, ArgList, int);
		Widget XmCreateScrolledWindow (Widget, const char *, ArgList, int);
		Widget XmCreateScrollBar (Widget, const char *, ArgList, int);
		Widget XmCreateSeparator (Widget, const char *, ArgList, int);
		Widget XmCreateSeparatorGadget (Widget, const char *, ArgList, int);
		Widget XmCreateShell (Widget, const char *, ArgList, int);
		Widget XmCreateToggleButton (Widget, const char *, ArgList, int);
		Widget XmCreateToggleButtonGadget (Widget, const char *, ArgList, int);   
		Atom XmInternAtom (Display *display, String name, Boolean only_if_exists);
		void XmScaleGetValue (Widget widget, int *value_return);
		void XmScaleSetValue (Widget widget, int value);
		void XmScrollBarGetValues (Widget me, int *value, int *sliderSize, int *increment, int *pageIncrement);
		void XmScrollBarSetValues (Widget me, int value, int sliderSize, int increment, int pageIncrement, Boolean notify);
		Boolean XmToggleButtonGadgetGetState (Widget widget);
		#define XmToggleButtonGetState XmToggleButtonGadgetGetState
		void XmToggleButtonGadgetSetState (Widget widget, Boolean value, Boolean notify);
		#define XmToggleButtonSetState XmToggleButtonGadgetSetState
		void XmUpdateDisplay (Widget dummy);

	#endif

	#if defined (macintosh)
		#define motif_argXmString(r,t)  r, t
		void motif_mac_defaultFont (void);
		void GuiMac_clipOn (Widget widget);   /* Clip to the inner area of a drawingArea (for drawing);
			used by graphics drivers for Macintosh (clipping is automatic for Xwindows). */
		int GuiMac_clipOn_graphicsContext (Widget me, void *graphicsContext);
		void GuiMac_clipOff (void);
		void motif_mac_setUserMessageCallbackA (int (*userMessageCallback) (char *message));
		void motif_mac_setUserMessageCallbackW (int (*userMessageCallback) (wchar_t *message));
	#elif defined (_WIN32)
		#define motif_argXmString(r,t)  r, t
		int motif_win_mouseStillDown (void);
		void motif_win_setUserMessageCallback (int (*userMessageCallback) (void));
	#else
		#define motif_argXmString(r,t)  XtVaTypedArg, r, XmRString, t, strlen (t) + 1
	#endif

#endif

/* Button layout and state: */
#define GuiMenu_INSENSITIVE  (1 << 8)
#define GuiMenu_CHECKBUTTON  (1 << 9)
#define GuiMenu_TOGGLE_ON  (1 << 10)
#define GuiMenu_RADIO_FIRST  (1 << 11)
#define GuiMenu_RADIO_NEXT  (1 << 12)

/* Accelerators: */
#define GuiMenu_OPTION  (1 << 21)
#define GuiMenu_SHIFT  (1 << 22)
#define GuiMenu_COMMAND  (1 << 23)
#define GuiMenu_LEFT_ARROW  1
#define GuiMenu_RIGHT_ARROW  2
#define GuiMenu_UP_ARROW  3
#define GuiMenu_DOWN_ARROW  4
#define GuiMenu_PAUSE  5
#define GuiMenu_DELETE  6
#define GuiMenu_INSERT  7
#define GuiMenu_BACKSPACE  8
#define GuiMenu_TAB  9
#define GuiMenu_LINEFEED  10
#define GuiMenu_HOME  11
#define GuiMenu_END  12
#define GuiMenu_ENTER  13
#define GuiMenu_PAGE_UP  14
#define GuiMenu_PAGE_DOWN  15
#define GuiMenu_ESCAPE  16
#define GuiMenu_F1  17
#define GuiMenu_F2  18
#define GuiMenu_F3  19
#define GuiMenu_F4  20
#define GuiMenu_F5  21
#define GuiMenu_F6  22
#define GuiMenu_F7  23
#define GuiMenu_F8  24
#define GuiMenu_F9  25
#define GuiMenu_F10  26
#define GuiMenu_F11  27
#define GuiMenu_F12  28

Widget Gui_addMenuBar (Widget form);
int Gui_getResolution (Widget widget);

/* GuiButton creation flags: */
#define GuiButton_DEFAULT  1
#define GuiButton_CANCEL  2
#define GuiButton_INSENSITIVE  4
typedef struct structGuiButtonEvent {
	Widget button;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
} *GuiButtonEvent;
Widget GuiButton_create (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (void *boss, GuiButtonEvent event), void *boss, unsigned long flags);
Widget GuiButton_createShown (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (void *boss, GuiButtonEvent event), void *boss, unsigned long flags);
void GuiButton_setString (Widget widget, const wchar_t *text);   // rarely used

/* GuiCheckButton creation flags: */
#define GuiCheckButton_SET  1
#define GuiCheckButton_INSENSITIVE  2
typedef struct structGuiCheckButtonEvent {
	Widget toggle;
} *GuiCheckButtonEvent;
Widget GuiCheckButton_create (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags);
Widget GuiCheckButton_createShown (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags);
bool GuiCheckButton_getValue (Widget widget);
void GuiCheckButton_setValue (Widget widget, bool value);

Widget GuiColumn_createShown (Widget parent, unsigned long flags);
Widget GuiRow_createShown (Widget parent, unsigned long flags);

/* GuiDialog creation flags: */
#define GuiDialog_MODAL  1
Widget GuiDialog_create (Widget parent, int x, int y, int width, int height,
	const wchar_t *title, void (*goAwayCallback) (void *goAwayBoss), void *goAwayBoss, unsigned long flags);
void GuiDialog_show (Widget widget);

/* GuiDrawingArea creation flags: */
#define GuiDrawingArea_BORDER  1
enum mouse_events { MOTION_NOTIFY = 1, BUTTON_PRESS, BUTTON_RELEASE };
typedef struct structGuiDrawingAreaExposeEvent {
	Widget widget;
	int x, y, width, height;
} *GuiDrawingAreaExposeEvent;
typedef struct structGuiDrawingAreaClickEvent {
	Widget widget;
	int x, y;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
	int button;
	enum mouse_events type;
} *GuiDrawingAreaClickEvent;
typedef struct structGuiDrawingAreaKeyEvent {
	Widget widget;
	wchar_t key;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
} *GuiDrawingAreaKeyEvent;
typedef struct structGuiDrawingAreaResizeEvent {
	Widget widget;
	int width, height;
} *GuiDrawingAreaResizeEvent;
Widget GuiDrawingArea_create (Widget parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags);
Widget GuiDrawingArea_createShown (Widget parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags);
void GuiDrawingArea_setExposeCallback (Widget widget, void (*callback) (void *boss, GuiDrawingAreaExposeEvent event), void *boss);
void GuiDrawingArea_setClickCallback (Widget widget, void (*callback) (void *boss, GuiDrawingAreaClickEvent event), void *boss);

/* GuiLabel creation flags: */
#define GuiLabel_CENTRE  1
#define GuiLabel_RIGHT  2
Widget GuiLabel_create (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *labelText, unsigned long flags);
Widget GuiLabel_createShown (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *labelText, unsigned long flags);
void GuiLabel_setString (Widget widget, const wchar_t *text);

typedef struct structGuiListEvent {
	Widget list;
} *GuiListEvent;
Widget GuiList_create (Widget parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header);
Widget GuiList_createShown (Widget parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header);
void GuiList_deleteAllItems (Widget me);
void GuiList_deleteItem (Widget me, long position);
void GuiList_deselectAllItems (Widget me);
void GuiList_deselectItem (Widget me, long position);
long GuiList_getBottomPosition (Widget me);
long GuiList_getNumberOfItems (Widget me);
long * GuiList_getSelectedPositions (Widget me, long *numberOfSelected);
long GuiList_getTopPosition (Widget me);
void GuiList_insertItem (Widget me, const wchar_t *itemText, long position);
void GuiList_replaceItem (Widget me, const wchar_t *itemText, long position);
void GuiList_setTopPosition (Widget me, long topPosition);
void GuiList_selectItem (Widget me, long position);
void GuiList_setSelectionChangedCallback (Widget me, void (*callback) (void *boss, GuiListEvent event), void *boss);
void GuiList_setDoubleClickCallback (Widget me, void (*callback) (void *boss, GuiListEvent event), void *boss);

Widget GuiMenuBar_addMenu (Widget bar, const wchar_t *title, long flags);
Widget GuiMenuBar_addMenu2 (Widget bar, const wchar_t *title, long flags, Widget *menuTitle);
/* Flags is a combination of the above defines. */

Widget GuiMenu_addItem (Widget menu, const wchar_t *title, long flags,
	void (*commandCallback) (Widget, XtPointer, XtPointer), const void *closure);
/* Flags is a combination of the above defines. */

Widget GuiMenu_addSeparator (Widget menu);

/* GuiRadioButton creation flags: */
#define GuiRadioButton_SET  1
#define GuiRadioButton_INSENSITIVE  2
typedef struct structGuiRadioButtonEvent {
	Widget toggle;
} *GuiRadioButtonEvent;
Widget GuiRadioButton_create (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags);
Widget GuiRadioButton_createShown (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags);
bool GuiRadioButton_getValue (Widget widget);
void GuiRadioButton_setValue (Widget widget, bool value);

#if gtk
void * GuiRadioButton_getGroup(Widget widget);
void GuiRadioButton_setGroup(Widget widget, void *group);
#endif

typedef struct structGuiTextEvent {
	Widget text;
} *GuiTextEvent;

/* GuiText creation flags: */
#define GuiText_SCROLLED  1
#define GuiText_MULTILINE  2
#define GuiText_WORDWRAP  4
#define GuiText_NONEDITABLE  8
Widget GuiText_create (Widget parent, int left, int right, int top, int bottom, unsigned long flags);
Widget GuiText_createShown (Widget parent, int left, int right, int top, int bottom, unsigned long flags);
void GuiText_copy (Widget widget);
void GuiText_cut (Widget widget);
wchar_t * GuiText_getSelection (Widget widget);
void GuiText_getSelectionPosition (Widget widget, long *first, long *last);
wchar_t *GuiText_getString (Widget widget);
void GuiText_paste (Widget widget);
void GuiText_redo (Widget widget);
void GuiText_remove (Widget widget);
void GuiText_replace (Widget widget, long from_pos, long to_pos, const wchar_t *value);
void GuiText_scrollToSelection (Widget widget);
void GuiText_setChangeCallback (Widget widget, void (*changeCallback) (void *boss, GuiTextEvent event), void *changeBoss);
void GuiText_setFontSize (Widget widget, int size);
void GuiText_setSelection (Widget widget, long first, long last);
void GuiText_setString (Widget widget, const wchar_t *text);
void GuiText_undo (Widget widget);
void GuiText_updateChangeCountAfterSave (Widget widget);

Widget GuiWindow_create (Widget parentOfShell, int x, int y, int width, int height,
	const wchar_t *title, void (*goAwayCallback) (void *goAwayBoss), void *goAwayBoss, unsigned long flags);
	// returns a Form widget that has a new Shell parent.
void GuiWindow_show (Widget form);
void GuiWindow_setTitle (Widget shell, const wchar_t *title);
int GuiWindow_setDirty (Widget shell, int dirty);
/*
	Purpose: like on MacOSX you get this little dot in the red close button,
		and the window proxy icon dims.
	Return value:
		TRUE if the system supports this feature, FALSE if not;
		the point of this is that you can use a different user feedback strategy, like appending
		the text "(modified)" to the window title, if this feature is not supported.
*/
void GuiWindow_setFile (Widget shell, MelderFile file);
/*
	Purpose: set the window title, and (on MacOS X) the window proxy icon and the window path menu.
*/
void GuiWindow_drain (Widget me);
/*
	Purpose: drain the double graphics buffer.
*/

void GuiObject_destroy (Widget me);
long GuiObject_getHeight (Widget me);
long GuiObject_getWidth (Widget me);
long GuiObject_getX (Widget me);
long GuiObject_getY (Widget me);
void GuiObject_hide (Widget me);
void GuiObject_move (Widget me, long x, long y);
Widget GuiObject_parent (Widget w);
void GuiObject_setSensitive (Widget me, bool sensitive);
void GuiObject_show (Widget me);
void GuiObject_size (Widget me, long width, long height);

/********** EVENTS **********/

void Gui_setOpenDocumentCallback (int (*openDocumentCallback) (MelderFile file));
void Gui_setQuitApplicationCallback (int (*quitApplicationCallback) (void));

/* End of file Gui.h */
#endif
