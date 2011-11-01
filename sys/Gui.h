#ifndef _Gui_h_
#define _Gui_h_
/* Gui.h
 *
 * Copyright (C) 1993-2011 Paul Boersma
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

#if defined (UNIX)
	#define gtk 1
	#define motif 0
#else
	#define gtk 0
	#define motif 1
#endif

#include "Collection.h"

#if gtk
	#include <gtk/gtk.h>
	#include <gdk/gdk.h>
	#include <cairo/cairo.h>
#elif defined (macintosh)
	#include "macport_on.h"
	#include <Carbon/Carbon.h>
	#include "macport_off.h"
#elif defined (_WIN32)
	#include "winport_on.h"
	#include <windows.h>
	#include <windowsx.h>
	#include "winport_off.h"
#endif

#define GUI_ARGS  GuiObject w, XtPointer void_me, XtPointer call

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
	typedef GMainContext *AppContext;
	typedef void *XtPointer;
	typedef gint Dimension;
	typedef gboolean Boolean;
	#define True 1
	#define False 0
	typedef void *GuiObject;
#elif motif
	typedef class structGuiObject *GuiObject;   // Opaque

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
	typedef void *Window;
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
	typedef GuiObject *GuiObjectList;
	typedef long XtWorkProcId, XtIntervalId;
	typedef void (*XtCallbackProc) (GuiObject w, XtPointer client_data, XtPointer call_data);
	typedef Boolean (*XtWorkProc) (XtPointer client_data);
	typedef void (*XtTimerCallbackProc) (XtPointer, XtIntervalId *);
	typedef unsigned long WidgetClass;
	#define False 0
	#define True 1

	/*
	 * Declarations of Xt functions.
	 */
	void XtAddCallback (GuiObject w, int kind, XtCallbackProc proc, XtPointer closure);
	XtIntervalId GuiAddTimeOut (unsigned long interval,
		XtTimerCallbackProc timerProc, XtPointer closure);
	XtWorkProcId GuiAddWorkProc (XtWorkProc workProc, XtPointer closure);
	void GuiMainLoop ();
	void GuiNextEvent (XEvent *event);
	#define XtCalloc  Melder_calloc
	#define XtClass(w)  (w) -> widgetClass
	void XtDestroyWidget (GuiObject w);
	void XtDispatchEvent (XEvent *event);
	#define XtDisplay(w)  0
	GuiObject GuiInitialize (const char *name, unsigned int *argc, char **argv);
	Boolean XtIsManaged (GuiObject w);
	Boolean XtIsShell (GuiObject w);
	void XtManageChild (GuiObject w);
	void XtManageChildren (GuiObjectList children, Cardinal num_children);
	void XtMapWidget (GuiObject w);
	void XtNextEvent (XEvent *event);
	void XtRemoveTimeOut (XtIntervalId id);
	void XtRemoveWorkProc (XtWorkProcId id);
	void XtSetKeyboardFocus (GuiObject tree, GuiObject descendant);
	void XtSetSensitive (GuiObject w, Boolean value);
	void XtUnmanageChild (GuiObject self);
	void XtUnmanageChildren (GuiObjectList children, Cardinal num_children);
	GuiObject GuiAppInitialize (const char *name,
		void *dum1, int dum2, unsigned int *argc, char **argv, void *dum3, void *dum4);
	GuiObject XtVaCreateWidget (const char *name, int widgetClass, GuiObject parent, ...);
	GuiObject XtVaCreateManagedWidget (const char *name, int widgetClass, GuiObject parent, ...);
	void XtVaGetValues (GuiObject w, ...);
	void XtVaSetValues (GuiObject w, ...);
	Window XtWindow (GuiObject w);
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
	void XmAddWMProtocolCallback (GuiObject shell, Atom protocol, XtCallbackProc callback, char *closure);
	GuiObject XmCreateBulletinBoard (GuiObject, const char *, ArgList, int);  
	GuiObject XmCreateBulletinBoardDialog (GuiObject, const char *, ArgList, int);  
	GuiObject XmCreateCascadeButton (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateCascadeButtonGadget (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateDialogShell (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateForm (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateFormDialog (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateMenuBar (GuiObject, const char *, ArgList, int);
	GuiObject XmCreatePulldownMenu (GuiObject, const char *, ArgList, int);   
	GuiObject XmCreateRadioBox (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateRowColumn (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateScale (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateScrolledWindow (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateScrollBar (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateSeparator (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateSeparatorGadget (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateShell (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateToggleButton (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateToggleButtonGadget (GuiObject, const char *, ArgList, int);   
	void XmScaleGetValue (GuiObject widget, int *value_return);
	void XmScaleSetValue (GuiObject widget, int value);
	void XmScrollBarGetValues (GuiObject me, int *value, int *sliderSize, int *increment, int *pageIncrement);
	void XmScrollBarSetValues (GuiObject me, int value, int sliderSize, int increment, int pageIncrement, Boolean notify);
	Boolean XmToggleButtonGadgetGetState (GuiObject widget);
	#define XmToggleButtonGetState XmToggleButtonGadgetGetState
	void XmToggleButtonGadgetSetState (GuiObject widget, Boolean value, Boolean notify);
	#define XmToggleButtonSetState XmToggleButtonGadgetSetState
	void XmUpdateDisplay (GuiObject dummy);

	#if defined (macintosh)
		#define motif_argXmString(r,t)  r, t
		void motif_mac_defaultFont (void);
		void GuiMac_clipOn (GuiObject widget);   /* Clip to the inner area of a drawingArea (for drawing);
			used by graphics drivers for Macintosh (clipping is automatic for Xwindows). */
		int GuiMac_clipOn_graphicsContext (GuiObject me, void *graphicsContext);
		void GuiMac_clipOff (void);
		void motif_mac_setUserMessageCallbackA (int (*userMessageCallback) (char *message));
		void motif_mac_setUserMessageCallbackW (int (*userMessageCallback) (wchar_t *message));
	#elif defined (_WIN32)
		#define motif_argXmString(r,t)  r, t
		int motif_win_mouseStillDown (void);
		void motif_win_setUserMessageCallback (int (*userMessageCallback) (void));
	#endif

#endif

/* Button layout and state: */
#define GuiMenu_INSENSITIVE  (1 << 8)
#define GuiMenu_CHECKBUTTON  (1 << 9)
#define GuiMenu_TOGGLE_ON  (1 << 10)
#define GuiMenu_ATTRACTIVE  (1 << 11)
#define GuiMenu_RADIO_FIRST  (1 << 12)
#define GuiMenu_RADIO_NEXT  (1 << 13)
#define GuiMenu_BUTTON_STATE_MASK  (GuiMenu_INSENSITIVE|GuiMenu_CHECKBUTTON|GuiMenu_TOGGLE_ON|GuiMenu_ATTRACTIVE|GuiMenu_RADIO_FIRST|GuiMenu_RADIO_NEXT)

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

GuiObject Gui_addMenuBar (GuiObject form);
int Gui_getResolution (GuiObject widget);
void Gui_getWindowPositioningBounds (double *x, double *y, double *width, double *height);

/* GuiButton creation flags: */
#define GuiButton_DEFAULT  1
#define GuiButton_CANCEL  2
#define GuiButton_INSENSITIVE  4
#define GuiButton_ATTRACTIVE  8
typedef struct structGuiButtonEvent {
	GuiObject button;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
} *GuiButtonEvent;
GuiObject GuiButton_create (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (void *boss, GuiButtonEvent event), void *boss, unsigned long flags);
GuiObject GuiButton_createShown (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (void *boss, GuiButtonEvent event), void *boss, unsigned long flags);
void GuiButton_setString (GuiObject widget, const wchar_t *text);   // rarely used

/* GuiCheckButton creation flags: */
#define GuiCheckButton_SET  1
#define GuiCheckButton_INSENSITIVE  2
typedef struct structGuiCheckButtonEvent {
	GuiObject toggle;
} *GuiCheckButtonEvent;
GuiObject GuiCheckButton_create (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags);
GuiObject GuiCheckButton_createShown (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags);
bool GuiCheckButton_getValue (GuiObject widget);
void GuiCheckButton_setValue (GuiObject widget, bool value);

GuiObject GuiColumn_createShown (GuiObject parent, unsigned long flags);
GuiObject GuiRow_createShown (GuiObject parent, unsigned long flags);

/* GuiDialog creation flags: */
#define GuiDialog_MODAL  1
GuiObject GuiDialog_create (GuiObject parent, int x, int y, int width, int height,
	const wchar_t *title, void (*goAwayCallback) (void *goAwayBoss), void *goAwayBoss, unsigned long flags);
GuiObject GuiDialog_getButtonArea (GuiObject widget);

SortedSetOfString GuiFileSelect_getInfileNames (GuiObject parent, const wchar_t *title, bool allowMultipleFiles);
wchar_t * GuiFileSelect_getOutfileName (GuiObject parent, const wchar_t *title, const wchar_t *defaultName);
wchar_t * GuiFileSelect_getDirectoryName (GuiObject parent, const wchar_t *title);

/* GuiDrawingArea creation flags: */
#define GuiDrawingArea_BORDER  1
enum mouse_events { MOTION_NOTIFY = 1, BUTTON_PRESS, BUTTON_RELEASE };
typedef struct structGuiDrawingAreaExposeEvent {
	GuiObject widget;
	int x, y, width, height;
} *GuiDrawingAreaExposeEvent;
typedef struct structGuiDrawingAreaClickEvent {
	GuiObject widget;
	int x, y;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
	int button;
	enum mouse_events type;
} *GuiDrawingAreaClickEvent;
typedef struct structGuiDrawingAreaKeyEvent {
	GuiObject widget;
	wchar_t key;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
} *GuiDrawingAreaKeyEvent;
typedef struct structGuiDrawingAreaResizeEvent {
	GuiObject widget;
	int width, height;
} *GuiDrawingAreaResizeEvent;
GuiObject GuiDrawingArea_create (GuiObject parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags);
GuiObject GuiDrawingArea_createShown (GuiObject parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags);
void GuiDrawingArea_setExposeCallback (GuiObject widget, void (*callback) (void *boss, GuiDrawingAreaExposeEvent event), void *boss);
void GuiDrawingArea_setClickCallback (GuiObject widget, void (*callback) (void *boss, GuiDrawingAreaClickEvent event), void *boss);

/* GuiLabel creation flags: */
#define GuiLabel_CENTRE  1
#define GuiLabel_RIGHT  2
GuiObject GuiLabel_create (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *labelText, unsigned long flags);
GuiObject GuiLabel_createShown (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *labelText, unsigned long flags);
void GuiLabel_setString (GuiObject widget, const wchar_t *text);

typedef struct structGuiListEvent {
	GuiObject list;
} *GuiListEvent;
GuiObject GuiList_create (GuiObject parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header);
GuiObject GuiList_createShown (GuiObject parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header);
void GuiList_deleteAllItems (GuiObject me);
void GuiList_deleteItem (GuiObject me, long position);
void GuiList_deselectAllItems (GuiObject me);
void GuiList_deselectItem (GuiObject me, long position);
long GuiList_getBottomPosition (GuiObject me);
long GuiList_getNumberOfItems (GuiObject me);
long * GuiList_getSelectedPositions (GuiObject me, long *numberOfSelected);
long GuiList_getTopPosition (GuiObject me);
void GuiList_insertItem (GuiObject me, const wchar_t *itemText, long position);
void GuiList_replaceItem (GuiObject me, const wchar_t *itemText, long position);
void GuiList_setTopPosition (GuiObject me, long topPosition);
void GuiList_selectItem (GuiObject me, long position);
void GuiList_setSelectionChangedCallback (GuiObject me, void (*callback) (void *boss, GuiListEvent event), void *boss);
void GuiList_setDoubleClickCallback (GuiObject me, void (*callback) (void *boss, GuiListEvent event), void *boss);

GuiObject GuiMenuBar_addMenu (GuiObject bar, const wchar_t *title, long flags);
GuiObject GuiMenuBar_addMenu2 (GuiObject bar, const wchar_t *title, long flags, GuiObject *menuTitle);
#if gtk
GuiObject GuiMenuBar_addMenu3 (GuiObject parent, const wchar_t *title, long flags, GuiObject *button);
#endif

/* Flags is a combination of the above defines. */

GuiObject GuiMenu_addItem (GuiObject menu, const wchar_t *title, long flags,
	void (*commandCallback) (GuiObject, XtPointer, XtPointer), const void *closure);
/* Flags is a combination of the above defines. */
GuiObject GuiMenu_addSeparator (GuiObject menu);
void GuiMenuItem_check (GuiObject menuItem, bool check);

/* GuiRadioButton creation flags: */
#define GuiRadioButton_SET  1
#define GuiRadioButton_INSENSITIVE  2
typedef struct structGuiRadioButtonEvent {
	GuiObject toggle;
} *GuiRadioButtonEvent;
GuiObject GuiRadioButton_create (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags);
GuiObject GuiRadioButton_createShown (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags);
bool GuiRadioButton_getValue (GuiObject widget);
void GuiRadioButton_setValue (GuiObject widget, bool value);

#if gtk
void * GuiRadioButton_getGroup (GuiObject widget);
void GuiRadioButton_setGroup (GuiObject widget, void *group);
#endif

typedef struct structGuiTextEvent {
	GuiObject text;
} *GuiTextEvent;

/* GuiText creation flags: */
#define GuiText_SCROLLED  1
#define GuiText_MULTILINE  2
#define GuiText_WORDWRAP  4
#define GuiText_NONEDITABLE  8
GuiObject GuiText_create (GuiObject parent, int left, int right, int top, int bottom, unsigned long flags);
GuiObject GuiText_createShown (GuiObject parent, int left, int right, int top, int bottom, unsigned long flags);
void GuiText_copy (GuiObject widget);
void GuiText_cut (GuiObject widget);
wchar_t * GuiText_getSelection (GuiObject widget);
wchar_t * GuiText_getString (GuiObject widget);
wchar_t * GuiText_getStringAndSelectionPosition (GuiObject widget, long *first, long *last);
void GuiText_paste (GuiObject widget);
void GuiText_redo (GuiObject widget);
void GuiText_remove (GuiObject widget);
void GuiText_replace (GuiObject widget, long from_pos, long to_pos, const wchar_t *value);
void GuiText_scrollToSelection (GuiObject widget);
void GuiText_setChangeCallback (GuiObject widget, void (*changeCallback) (void *boss, GuiTextEvent event), void *changeBoss);
void GuiText_setFontSize (GuiObject widget, int size);
void GuiText_setRedoItem (GuiObject widget, GuiObject item);
void GuiText_setSelection (GuiObject widget, long first, long last);
void GuiText_setString (GuiObject widget, const wchar_t *text);
void GuiText_setUndoItem (GuiObject widget, GuiObject item);
void GuiText_undo (GuiObject widget);
void GuiText_updateChangeCountAfterSave (GuiObject widget);

/* GuiWindow creation flags: */
#define GuiWindow_FULLSCREEN  1
GuiObject GuiWindow_create (GuiObject parentOfShell, int x, int y, int width, int height,
	const wchar_t *title, void (*goAwayCallback) (void *goAwayBoss), void *goAwayBoss, unsigned long flags);
	// returns a Form widget that has a new Shell parent.
void GuiWindow_setTitle (GuiObject shell, const wchar_t *title);
int GuiWindow_setDirty (GuiObject shell, int dirty);
/*
	Purpose: like on MacOSX you get this little dot in the red close button,
		and the window proxy icon dims.
	Return value:
		TRUE if the system supports this feature, FALSE if not;
		the point of this is that you can use a different user feedback strategy, like appending
		the text "(modified)" to the window title, if this feature is not supported.
*/
void GuiWindow_setFile (GuiObject shell, MelderFile file);
/*
	Purpose: set the window title, and (on MacOS X) the window proxy icon and the window path menu.
*/
void GuiWindow_drain (GuiObject me);
/*
	Purpose: drain the double graphics buffer.
*/

void GuiObject_destroy (GuiObject me);
long GuiObject_getHeight (GuiObject me);
long GuiObject_getWidth (GuiObject me);
long GuiObject_getX (GuiObject me);
long GuiObject_getY (GuiObject me);
void GuiObject_hide (GuiObject me);
void GuiObject_move (GuiObject me, long x, long y);
GuiObject GuiObject_parent (GuiObject w);
void GuiObject_setSensitive (GuiObject me, bool sensitive);
void GuiObject_show (GuiObject me);
void GuiObject_size (GuiObject me, long width, long height);

/********** EVENTS **********/

void Gui_setOpenDocumentCallback (void (*openDocumentCallback) (MelderFile file));
void Gui_setQuitApplicationCallback (int (*quitApplicationCallback) (void));

/* End of file Gui.h */
#endif
