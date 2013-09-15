#ifndef _Gui_h_
#define _Gui_h_
/* Gui.h
 *
 * Copyright (C) 1993-2011,2012,2013 Paul Boersma, 2013 Tom Naughton
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
 * Determine the widget set.
 */
#if defined (UNIX)
	#define gtk 1
	#define motif 0
	#define cocoa 0
#elif defined (_WIN32)
	#define gtk 0
	#define motif 1
	#define cocoa 0
#elif defined (macintosh)
	#if useCarbon
		#define gtk 0
		#define motif 1
		#define cocoa 0
	#else
		#define gtk 0
		#define motif 0
		#define cocoa 1
	#endif
#endif

#include "Collection.h"

#if gtk
	#include <gtk/gtk.h>
	#include <gdk/gdk.h>
	#include <cairo/cairo.h>
#elif defined (macintosh)
	#include "macport_on.h"
    #if useCarbon
        #include <Carbon/Carbon.h>
    #endif
    #include <Cocoa/Cocoa.h>
	#include "macport_off.h"
#elif defined (_WIN32)
	#include "winport_on.h"
	#include <windows.h>
	#include <windowsx.h>
	#include "winport_off.h"
#endif

#define GUI_ARGS  void *void_me, GuiMenuItemEvent event

#define GUI_IAM(klas)  (void) void_me; (void) event; iam (klas);

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
#if gtk
	#define Gui_PUSHBUTTON_HEIGHT  25
#else
	#define Gui_PUSHBUTTON_HEIGHT  20
#endif
#define Gui_OK_BUTTON_WIDTH  69
#define Gui_CANCEL_BUTTON_WIDTH  69
#define Gui_APPLY_BUTTON_WIDTH  69

#define Gui_HOMOGENEOUS  1

#if gtk
	typedef GMainContext *AppContext;
	typedef gint Dimension;
	typedef gboolean Boolean;
	#define True 1
	#define False 0
	typedef void *GuiObject;
#elif cocoa
	Thing_declare (GuiThing);
	@protocol GuiCocoaAny
		- (GuiThing) userData;
		- (void) setUserData: (GuiThing) userData;
	@end
	typedef NSObject <GuiCocoaAny> *GuiObject;
	@interface GuiCocoaButton : NSButton <GuiCocoaAny> @end
	@interface GuiCocoaCheckButton : NSButton <GuiCocoaAny> @end
	@interface GuiCocoaDrawingArea : NSView <GuiCocoaAny> @end
	@interface GuiCocoaLabel : NSTextField <GuiCocoaAny> @end
	@interface GuiCocoaList : NSView <GuiCocoaAny, NSTableViewDataSource, NSTableViewDelegate>
		@property (nonatomic, retain) NSMutableArray *contents;
		@property (nonatomic, retain) NSTableView *tableView;
	@end
	@interface GuiCocoaMenu : NSMenu <GuiCocoaAny> @end
	@interface GuiCocoaMenuButton : NSPopUpButton <GuiCocoaAny> @end
	@interface GuiCocoaMenuItem : NSMenuItem <GuiCocoaAny> @end
	@interface GuiCocoaOptionMenu : NSPopUpButton <GuiCocoaAny> @end
	@interface GuiCocoaProgressBar : NSProgressIndicator <GuiCocoaAny> @end
	@interface GuiCocoaRadioButton : NSButton <GuiCocoaAny> @end
	@interface GuiCocoaScrollBar : NSScroller <GuiCocoaAny> @end
	@interface GuiCocoaScrolledWindow : NSScrollView <GuiCocoaAny> @end
	@interface GuiCocoaTextField : NSTextField <GuiCocoaAny> @end
	@interface GuiCocoaTextView : NSTextView <GuiCocoaAny, NSTextViewDelegate> @end
	@interface GuiCocoaWindow : NSWindow <GuiCocoaAny> @end
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
	typedef bool (*XtWorkProc) (void *client_data);
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
	void GuiInitialize (const char *name, unsigned int *argc, char **argv);
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
	void GuiAppInitialize (const char *name,
		void *dum1, int dum2, unsigned int *argc, char **argv, void *dum3, void *dum4);
	void GuiApp_setApplicationShell (GuiObject shell);
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
		void motif_mac_defaultFont (void);
		void GuiMac_clipOn (GuiObject widget);   /* Clip to the inner area of a drawingArea (for drawing);
			used by graphics drivers for Macintosh (clipping is automatic for Xwindows). */
		int GuiMac_clipOn_graphicsContext (GuiObject me, void *graphicsContext);
		void GuiMac_clipOff (void);
		void motif_mac_setUserMessageCallbackA (int (*userMessageCallback) (char *message));
		void motif_mac_setUserMessageCallbackW (int (*userMessageCallback) (wchar_t *message));
	#elif defined (_WIN32)
		int motif_win_mouseStillDown (void);
		void motif_win_setUserMessageCallback (int (*userMessageCallback) (void));
	#endif
#endif

int Gui_getResolution (GuiObject widget);
void Gui_getWindowPositioningBounds (double *x, double *y, double *width, double *height);

Thing_declare (GuiForm);
Thing_declare (GuiMenu);
Thing_declare (GuiScrolledWindow);
Thing_declare (GuiShell);
Thing_declare (GuiWindow);

Thing_define (GuiThing, Thing) { public:
	GuiShell d_shell;
	GuiThing d_parent;
	GuiObject d_widget;
	/*
	 * Messages:
	 */
	void f_show ();
	void f_hide ();
	void f_setSensitive (bool sensitive);
	/*
	 * Methods:
	 */
	virtual void v_destroy ();
	virtual void v_show ();
	virtual void v_hide ();
	virtual void v_setSensitive (bool sensitive);
};

Thing_define (GuiControl, GuiThing) { public:
	int d_left, d_right, d_top, d_bottom;
	bool d_blockValueChangedCallbacks;
	/*
	 * Messages:
	 */
	int f_getX ();
	int f_getY ();
	int f_getWidth ();
	int f_getHeight ();
	void f_move (int x, int y);
	void f_setSize (int width, int height);
	/*
	 * Methods:
	 */
	virtual void v_positionInForm (GuiObject widget, int left, int right, int top, int bottom, GuiForm parent);
	virtual void v_positionInScrolledWindow (GuiObject widget, int width, int height, GuiScrolledWindow parent);
};

Thing_define (GuiForm, GuiControl) {
};

Thing_define (GuiShell, GuiForm) { public:
	int d_width, d_height;
	#if gtk
		GtkWindow *d_gtkWindow;
	#elif cocoa
		GuiCocoaWindow *d_cocoaWindow;
	#elif motif
		GuiObject d_xmShell;
	#endif
	void (*d_goAwayCallback) (void *boss);
	void *d_goAwayBoss;
	/*
	 * Messages:
	 */
	int f_getShellWidth ();
	int f_getShellHeight ();
	void f_setTitle (const wchar_t *title);
	void f_drain ();   // drain the double graphics buffer
	/*
	 * Overridden methods:
	 */
	virtual void v_destroy ();
};

/********** GuiButton **********/

Thing_declare (GuiButton);

typedef struct structGuiButtonEvent {
	GuiButton button;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
} *GuiButtonEvent;

Thing_define (GuiButton, GuiControl) { public:
	void (*d_activateCallback) (void *boss, GuiButtonEvent event);
	void *d_activateBoss;
	GuiMenu d_menu;   // for cascade buttons
	/*
	 * Messages:
	 */
	void f_setString (const wchar_t * text);
};

/* GuiButton creation flags: */
#define GuiButton_DEFAULT  1
#define GuiButton_CANCEL  2
#define GuiButton_INSENSITIVE  4
#define GuiButton_ATTRACTIVE  8
GuiButton GuiButton_create      (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (void *boss, GuiButtonEvent event), void *boss, unsigned long flags);
GuiButton GuiButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (void *boss, GuiButtonEvent event), void *boss, unsigned long flags);

/********** GuiCheckButton **********/

Thing_declare (GuiCheckButton);

typedef struct structGuiCheckButtonEvent {
	GuiCheckButton toggle;
} *GuiCheckButtonEvent;

Thing_define (GuiCheckButton, GuiControl) { public:
	void (*d_valueChangedCallback) (void *boss, GuiCheckButtonEvent event);
	void *d_valueChangedBoss;
	/*
	 * Messages:
	 */
	bool f_getValue ();
	void f_setValue (bool value);
};

/* GuiCheckButton creation flags: */
#define GuiCheckButton_SET  1
#define GuiCheckButton_INSENSITIVE  2
GuiCheckButton GuiCheckButton_create      (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags);
GuiCheckButton GuiCheckButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags);

/********** GuiDialog **********/

Thing_declare (GuiDialog);

Thing_define (GuiDialog, GuiShell) { public:
};

/* GuiDialog creation flags: */
#define GuiDialog_MODAL  1
GuiDialog GuiDialog_create (GuiWindow parent, int x, int y, int width, int height,
	const wchar_t *title, void (*goAwayCallback) (void *goAwayBoss), void *goAwayBoss, unsigned long flags);

/********** GuiDrawingArea **********/

Thing_declare (GuiDrawingArea);
Thing_declare (GuiScrollBar);

typedef struct structGuiDrawingAreaExposeEvent {
	GuiDrawingArea widget;
	int x, y, width, height;
} *GuiDrawingAreaExposeEvent;
typedef struct structGuiDrawingAreaClickEvent {
	GuiDrawingArea widget;
	int x, y;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
	int button;
} *GuiDrawingAreaClickEvent;
typedef struct structGuiDrawingAreaKeyEvent {
	GuiDrawingArea widget;
	wchar_t key;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
} *GuiDrawingAreaKeyEvent;
typedef struct structGuiDrawingAreaResizeEvent {
	GuiDrawingArea widget;
	int width, height;
} *GuiDrawingAreaResizeEvent;

Thing_define (GuiDrawingArea, GuiControl) { public:
	GuiScrollBar d_horizontalScrollBar, d_verticalScrollBar;   // for swiping
	void (*d_exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event);
	void *d_exposeBoss;
	void (*d_clickCallback) (void *boss, GuiDrawingAreaClickEvent event);
	void *d_clickBoss;
	void (*d_keyCallback) (void *boss, GuiDrawingAreaKeyEvent event);
	void *d_keyBoss;
	void (*d_resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event);
	void *d_resizeBoss;
	/*
	 * Messages:
	 */
	void f_setSwipable (GuiScrollBar horizontalScrollBar, GuiScrollBar verticalScrollBar);
	void f_setExposeCallback (void (*callback) (void *boss, GuiDrawingAreaExposeEvent event), void *boss);
	void f_setClickCallback  (void (*callback) (void *boss, GuiDrawingAreaClickEvent  event), void *boss);
	void f_setResizeCallback (void (*callback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss);
};

/* GuiDrawingArea creation flags: */
#define GuiDrawingArea_BORDER  1
GuiDrawingArea GuiDrawingArea_create (GuiForm parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags);
GuiDrawingArea GuiDrawingArea_createShown (GuiForm parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags);
GuiDrawingArea GuiDrawingArea_create (GuiScrolledWindow parent, int width, int height,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags);
GuiDrawingArea GuiDrawingArea_createShown (GuiScrolledWindow parent, int width, int height,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags);

SortedSetOfString GuiFileSelect_getInfileNames (GuiWindow parent, const wchar_t *title, bool allowMultipleFiles);
wchar_t * GuiFileSelect_getOutfileName (GuiWindow parent, const wchar_t *title, const wchar_t *defaultName);
wchar_t * GuiFileSelect_getDirectoryName (GuiWindow parent, const wchar_t *title);

/********** GuiForm **********/

GuiForm GuiForm_createInScrolledWindow (GuiScrolledWindow parent);

/********** GuiLabel **********/

Thing_declare (GuiLabel);

Thing_define (GuiLabel, GuiControl) { public:
	void f_setString (const wchar_t * text);
};

/* GuiLabel creation flags: */
#define GuiLabel_CENTRE  1
#define GuiLabel_RIGHT  2
GuiLabel GuiLabel_create      (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *labelText, unsigned long flags);
GuiLabel GuiLabel_createShown (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *labelText, unsigned long flags);

/********** GuiList **********/

Thing_declare (GuiList);
Thing_declare (GuiScrolledWindow);

typedef struct structGuiListEvent {
	GuiList list;
} *GuiListEvent;

Thing_define (GuiList, GuiControl) { public:
	bool d_allowMultipleSelection;
	void (*d_selectionChangedCallback) (void *boss, GuiListEvent event);
	void *d_selectionChangedBoss;
	void (*d_doubleClickCallback) (void *boss, GuiListEvent event);
	void *d_doubleClickBoss;
	#if gtk
		GtkListStore *d_liststore;
	#elif cocoa
	#elif motif && useCarbon
		GuiObject d_xmScrolled, d_xmList;
		ListHandle d_macListHandle;
	#endif
	/*
	 * Messages:
	 */
	void f_deleteAllItems ();
	void f_deleteItem (long position);
	void f_deselectAllItems ();
	void f_deselectItem (long position);
	long f_getBottomPosition ();
	long f_getNumberOfItems ();
	long * f_getSelectedPositions (long *numberOfSelected);
	long f_getTopPosition ();
	void f_insertItem (const wchar_t *itemText, long position);
	void f_replaceItem (const wchar_t *itemText, long position);
	void f_setTopPosition (long topPosition);
	void f_selectItem (long position);
	void f_setSelectionChangedCallback (void (*callback) (void *boss, GuiListEvent event), void *boss);
	void f_setDoubleClickCallback (void (*callback) (void *boss, GuiListEvent event), void *boss);
};

GuiList GuiList_create      (GuiForm parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header);
GuiList GuiList_createShown (GuiForm parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header);

/********** GuiMenu **********/

Thing_declare (GuiMenuItem);

Thing_define (GuiMenu, GuiThing) { public:
	GuiMenuItem d_menuItem;
	GuiButton d_cascadeButton;
	#if gtk
		GtkMenuItem *d_gtkMenuTitle;
	#elif cocoa
		GuiCocoaMenu *d_cocoaMenu;
		GuiCocoaMenuItem *d_cocoaMenuItem;
		GuiCocoaMenuButton *d_cocoaMenuButton;
	#elif motif
		GuiObject d_xmMenuTitle;   // in case the menu is in a menu bar
		GuiObject d_xmMenuBar;   // in case the menu is in a form
	#endif
	/*
	 * Messages:
	 */
	void f_empty ();
	/*
	 * Overridden methods:
	 */
	virtual void v_destroy ();
	virtual void v_show ();
	virtual void v_hide ();
	virtual void v_setSensitive (bool sensitive);
};

GuiMenu GuiMenu_createInWindow (GuiWindow window, const wchar_t *title, long flags);
GuiMenu GuiMenu_createInMenu (GuiMenu supermenu, const wchar_t *title, long flags);
GuiMenu GuiMenu_createInForm (GuiForm form, int left, int right, int top, int bottom, const wchar_t *title, long flags);

/********** GuiMenuItem **********/

Thing_declare (GuiMenuItem);

typedef struct structGuiMenuItemEvent {
	GuiMenuItem menuItem;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
} *GuiMenuItemEvent;

Thing_define (GuiMenuItem, GuiThing) { public:
	GuiMenu d_menu;
	void (*d_commandCallback) (void *boss, GuiMenuItemEvent event);
	void *d_boss;
	#if gtk
		bool d_callbackBlocked;
	#endif
	/*
	 * Messages:
	 */
	void f_check (bool check);
};

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
// or any ASCII character (preferably a letter or digit) between 32 and 126

GuiMenuItem GuiMenu_addItem (GuiMenu menu, const wchar_t *title, long flags,
	void (*commandCallback) (void *boss, GuiMenuItemEvent event), void *boss);
/* Flags is a combination of the above defines. */
GuiMenuItem GuiMenu_addSeparator (GuiMenu menu);

/********** GuiOptionMenu **********/

Thing_define (GuiOptionMenu, GuiControl) { public:
	/*
	 * Messages:
	 */
	void f_init (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags);
	void f_addOption (const wchar_t *text);
	int f_getValue ();
	void f_setValue (int value);
	/*
	 * Hidden data:
	 */
	//private:
		int d_value;
		Ordered d_options;   // of SimpleString
		#if gtk
			//GtkComboBox *d_gtkCascadeButton;
		#elif cocoa
		#elif motif
			GuiObject d_xmMenuBar, d_xmCascadeButton;
		#endif
	/*
	 * Methods:
	 */
	virtual void v_show ();   // overridden
};

GuiOptionMenu GuiOptionMenu_create      (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags);
GuiOptionMenu GuiOptionMenu_createShown (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags);

/********** GuiProgressBar **********/

Thing_declare (GuiProgressBar);

Thing_define (GuiProgressBar, GuiControl) { public:
	#if cocoa
		GuiCocoaProgressBar *d_cocoaProgressBar;
	#endif
	/*
	 * Messages:
	 */
	void f_setValue (double value);   // between 0.0 and 1.0
};

GuiProgressBar GuiProgressBar_create      (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags);
GuiProgressBar GuiProgressBar_createShown (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags);

/********** GuiRadioButton **********/

Thing_declare (GuiRadioButton);

typedef struct structGuiRadioButtonEvent {
	GuiRadioButton toggle;
	int position;
} *GuiRadioButtonEvent;

Thing_define (GuiRadioButton, GuiControl) { public:
	GuiRadioButton d_previous, d_next;   // there's a linked list of grouped radio buttons
	void (*d_valueChangedCallback) (void *boss, GuiRadioButtonEvent event);
	void *d_valueChangedBoss;
	#if cocoa
		GuiCocoaRadioButton *d_cocoaRadioButton;
	#endif
	/*
	 * Messages:
	 */
	bool f_getValue ();
	void f_set ();
};

/* GuiRadioButton creation flags: */
#define GuiRadioButton_SET  1
#define GuiRadioButton_INSENSITIVE  2
GuiRadioButton GuiRadioButton_create      (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags);
GuiRadioButton GuiRadioButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags);

void GuiRadioGroup_begin ();
void GuiRadioGroup_end ();

/********** GuiScale **********/

Thing_declare (GuiScale);

Thing_define (GuiScale, GuiControl) { public:
	/*
	 * Messages:
	 */
	int f_getValue ();
	void f_setValue (int value);
};

GuiScale GuiScale_create      (GuiForm parent, int left, int right, int top, int bottom,
	int minimum, int maximum, int value, unsigned long flags);
GuiScale GuiScale_createShown (GuiForm parent, int left, int right, int top, int bottom,
	int minimum, int maximum, int value, unsigned long flags);

/********** GuiScrollBar **********/

Thing_declare (GuiScrollBar);

typedef struct structGuiScrollBarEvent {
	GuiScrollBar scrollBar;
} *GuiScrollBarEvent;

Thing_define (GuiScrollBar, GuiControl) { public:
	void (*d_valueChangedCallback) (void *boss, GuiScrollBarEvent event);
	void *d_valueChangedBoss;
	/*
	 * Messages:
	 */
	int f_getValue ();
	void f_set (double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement);
};

/* GuiScrollBar creation flags: */
#define GuiScrollBar_HORIZONTAL  1
GuiScrollBar GuiScrollBar_create      (GuiForm parent, int left, int right, int top, int bottom,
	double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement,
	void (*valueChangedCallback) (void *boss, GuiScrollBarEvent event), void *valueChangedBoss, unsigned long flags);
GuiScrollBar GuiScrollBar_createShown (GuiForm parent, int left, int right, int top, int bottom,
	double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement,
	void (*valueChangedCallback) (void *boss, GuiScrollBarEvent event), void *valueChangedBoss, unsigned long flags);

/********** GuiScrolledWindow **********/

Thing_declare (GuiScrolledWindow);

Thing_define (GuiScrolledWindow, GuiControl) { public:
};

GuiScrolledWindow GuiScrolledWindow_create      (GuiForm parent, int left, int right, int top, int bottom,
	int horizontalScrollbarPersistence, int verticalScrollbarPersistence, unsigned long flags);
GuiScrolledWindow GuiScrolledWindow_createShown (GuiForm parent, int left, int right, int top, int bottom,
	int horizontalScrollbarPersistence, int verticalScrollbarPersistence, unsigned long flags);

/********** GuiText **********/

Thing_declare (GuiText);

typedef struct structGuiTextEvent {
	GuiText text;
} *GuiTextEvent;

#if gtk
	typedef gchar * history_data;
#else
	typedef char * history_data;
#endif

typedef struct _history_entry_s history_entry;
struct _history_entry_s {
	history_entry *prev, *next;
	long first, last;
	history_data text;
	bool type_del : 1;
};

Thing_define (GuiText, GuiControl) { public:
	void (*d_changeCallback) (void *boss, GuiTextEvent event);
	void *d_changeBoss;
	#if cocoa
		GuiCocoaScrolledWindow *d_cocoaScrollView;
		GuiCocoaTextView *d_cocoaTextView;
	#elif defined (macintosh)
		TXNObject d_macMlteObject;
		TXNFrameID d_macMlteFrameId;
	#else
		history_entry *d_prev, *d_next;
		GuiMenuItem d_undo_item, d_redo_item;
		bool d_history_change : 1;
	#endif
	#if motif
		bool d_editable;
	#endif
	/*
	 * Messages:
	 */
	void f_copy ();
	void f_cut ();
	wchar_t * f_getSelection ();
	wchar_t * f_getString ();
	wchar_t * f_getStringAndSelectionPosition (long *first, long *last);
	void f_paste ();
	void f_redo ();
	void f_remove ();
	void f_replace (long from_pos, long to_pos, const wchar_t *value);
	void f_scrollToSelection ();
	void f_setChangeCallback (void (*changeCallback) (void *boss, GuiTextEvent event), void *changeBoss);
	void f_setFontSize (int size);
	void f_setRedoItem (GuiMenuItem item);
	void f_setSelection (long first, long last);
	void f_setString (const wchar_t *text);
	void f_setUndoItem (GuiMenuItem item);
	void f_undo ();
	void f_updateChangeCountAfterSave ();
};

/* GuiText creation flags: */
#define GuiText_SCROLLED  1
#define GuiText_MULTILINE  2
#define GuiText_WORDWRAP  4
#define GuiText_NONEDITABLE  8
GuiText GuiText_create      (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags);
GuiText GuiText_createShown (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags);

/********** GuiWindow **********/

Thing_define (GuiWindow, GuiShell) { public:
	#if gtk
		GtkMenuBar *d_gtkMenuBar;
	#elif cocoa
		int d_menuBarWidth;
	#elif motif
		GuiObject d_xmMenuBar;
	#endif
	/*
	 * Messages:
	 */
	void f_addMenuBar ();
	bool f_setDirty (bool dirty);
	/*
		Purpose: on OSX you get a little dot in the red close button,
			and the window proxy icon dims.
		Return value:
			TRUE if the system supports this feature, FALSE if not;
			the point of this is that you can use a different user feedback strategy, like appending
			the text "(modified)" to the window title, if this feature is not supported.
	*/
	void f_setFile (MelderFile file);
	/*
		Purpose: set the window title, and (on MacOS X) the window proxy icon and the window path menu.
	*/
};

/* GuiWindow creation flags: */
#define GuiWindow_FULLSCREEN  1
GuiWindow GuiWindow_create (int x, int y, int width, int height,
	const wchar_t *title, void (*goAwayCallback) (void *goAwayBoss), void *goAwayBoss, unsigned long flags);
	// returns a Form widget that has a new Shell parent.

void GuiObject_destroy (GuiObject me);

/********** EVENTS **********/

void Gui_setOpenDocumentCallback (void (*openDocumentCallback) (MelderFile file));
void Gui_setQuitApplicationCallback (int (*quitApplicationCallback) (void));

extern GuiObject theGuiTopMenuBar;
extern unsigned long theGuiTopLowAccelerators [8];

/* End of file Gui.h */
#endif
