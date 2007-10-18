#ifndef _Gui_h_
#define _Gui_h_
/* Gui.h
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
 * pb 2007/08/07
 */

#ifndef _melder_h_
	#include "melder.h"
#endif

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

#else
/* Macintosh or Windows */

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

#if defined (macintosh)
	typedef struct EventRecord XEvent;
#else
	typedef MSG XEvent;
	typedef struct { int message, x, y, key, shiftKeyPressed; } WinDrawingAreaEvent;
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

void XMapRaised (int displayDummy, Window window);

#define DefaultScreenOfDisplay(d)  0
int WidthOfScreen (int screen);
int HeightOfScreen (int screen);

typedef void *XtPointer;
typedef struct structWidget *Widget;   /* Opaque. */
typedef Widget *WidgetList;
typedef void *XtAppContext;
typedef long XtWorkProcId, XtIntervalId;
typedef void (*XtCallbackProc) (Widget w, XtPointer client_data, XtPointer call_data);
typedef Boolean (*XtWorkProc) (XtPointer client_data);
typedef void (*XtTimerCallbackProc) (XtPointer, XtIntervalId *);
#define False 0
#define True 1

void XtAddCallback (Widget w, int kind, XtCallbackProc proc, XtPointer closure);
XtIntervalId XtAppAddTimeOut (XtAppContext appContext, unsigned long interval,
	XtTimerCallbackProc timerProc, XtPointer closure);
XtWorkProcId XtAppAddWorkProc (XtAppContext appContext, XtWorkProc workProc, XtPointer closure);
void XtAppMainLoop (XtAppContext appContext);
void XtAppNextEvent (XtAppContext appContext, XEvent *event);
void XtDestroyWidget (Widget w);
void XtDispatchEvent (XEvent *event);
#define XtDisplay(w)  0
#define XtCalloc  Melder_calloc
void XtFree (char *);
Widget XtInitialize (void *dum1, const char *name,
	void *dum2, int dum3, unsigned int *argc, char **argv);
Boolean XtIsManaged (Widget w);
Boolean XtIsShell (Widget w);
void XtManageChild (Widget w);
void XtManageChildren (WidgetList children, Cardinal num_children);
void XtMapWidget (Widget w);
void XtNextEvent (XEvent *event);
Widget XtParent (Widget w);
#define XtRealizeWidget  XtManageChild
void XtRemoveCallback (Widget w, int kind, XtCallbackProc proc, XtPointer closure);
void XtRemoveCallbacks (Widget w, int kind, XtCallbackProc proc);
void XtRemoveAllCallbacks (Widget w, int kind);
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

/* widget classes */
#define xmBulletinBoardWidgetClass  0x00000001
#define xmDrawingAreaWidgetClass  0x00000002
#define xmFormWidgetClass  0x00000004
#define xmFrameWidgetClass  0x00000008
#define xmLabelWidgetClass  0x00000010
#define xmListWidgetClass  0x00000020
#define xmMenuBarWidgetClass  0x00000040
#define xmMessageBoxWidgetClass  0x00000080
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
#define xmCascadeButtonGadgetClass  xmCascadeButtonWidgetClass
#define xmLabelGadgetClass  xmLabelWidgetClass
#define xmPushButtonGadgetClass  xmPushButtonWidgetClass
#define xmSeparatorGadgetClass  xmSeparatorWidgetClass
#define xmTextFieldWidgetClass xmTextWidgetClass
#define xmToggleButtonGadgetClass  xmToggleButtonWidgetClass

enum { /* resource names */
	XmNnull,
	#define motif_RESOURCE(xxx)  xxx,
	#include "motifEmulator_resources.h"
	#undef motif_RESOURCE
	XmNend
};

/* The enums start at 1, since we have to reserve zero for the dynamic default value. */
/* This is important for the resources set during creation. */

enum /* dialog styles */ { XmDIALOG_MODELESS = 1, XmDIALOG_FULL_APPLICATION_MODAL };
enum /* orientation */ { XmVERTICAL = 1, XmHORIZONTAL };
enum /* packing */ { XmPACK_TIGHT = 1, XmPACK_COLUMN, XmPACK_NONE };
enum /* attachment */ { XmATTACH_NONE = 1, XmATTACH_FORM, XmATTACH_POSITION };
enum /* rowColumn types */ { XmWORK_AREA = 1, XmMENU_BAR };
enum /* edit modes */ { XmSINGLE_LINE_EDIT = 1, XmMULTI_LINE_EDIT };
enum /* scroll bar display policies */ { XmAS_NEEDED = 1, XmSTATIC };
enum /* delete responses */ { XmDESTROY = 1, XmUNMAP, XmDO_NOTHING };
enum /* indicator types */ { XmONE_OF_MANY = 1, XmN_OF_MANY };
enum /* dialog children */ { XmDIALOG_OK_BUTTON = 1, XmDIALOG_CANCEL_BUTTON, XmDIALOG_HELP_BUTTON };
enum /* highlight modes */ { XmHIGHLIGHT_NORMAL = 1, XmHIGHLIGHT_SELECTED, XmHIGHLIGHT_SECONDARY_SELECTED };
enum /* alignments */ { XmALIGNMENT_BEGINNING = 1, XmALIGNMENT_CENTER, XmALIGNMENT_END };
enum /* selection policies */ {
	XmEXTENDED_SELECT = 1,		/* mac's default */
	XmMULTIPLE_SELECT,
	XmSINGLE_SELECT,
	XmBROWSE_SELECT
};
enum /* dialog types */ {
	XmDIALOG_MESSAGE = 1,
	XmDIALOG_ERROR,
	XmDIALOG_WARNING,
	XmDIALOG_INFORMATION,
	XmDIALOG_QUESTION
};
enum /* policies */ {
	XmCONSTANT = 1,
	XmVARIABLE,   /* list size policies */
	XmRESIZE_IF_POSSIBLE,
	XmAUTOMATIC,   /* scrolling policies */
	XmAPPLICATION_DEFINED
};

typedef char *XmString;
typedef long XmTextPosition;
typedef int XmHighlightMode;

void XmAddWMProtocolCallback (Widget shell, Atom protocol, XtCallbackProc callback, char *closure);
void XmAddWMProtocols (Widget shell, Atom *protocols, Cardinal num_protocols);   /* does nothing */
Widget XmCreateBulletinBoard (Widget, const char *, ArgList, int);  
Widget XmCreateBulletinBoardDialog (Widget, const char *, ArgList, int);  
Widget XmCreateCascadeButton (Widget, const char *, ArgList, int);
Widget XmCreateCascadeButtonGadget (Widget, const char *, ArgList, int);
Widget XmCreateDialogShell (Widget, const char *, ArgList, int);
Widget XmCreateDrawingArea (Widget, const char *, ArgList, int);
Widget XmCreateForm (Widget, const char *, ArgList, int);
Widget XmCreateFormDialog (Widget, const char *, ArgList, int);
Widget XmCreateFrame (Widget, const char *, ArgList, int);
Widget XmCreateLabel (Widget, const char *, ArgList, int);
Widget XmCreateLabelGadget (Widget, const char *, ArgList, int);
Widget XmCreateList (Widget, const char *, ArgList, int);
Widget XmCreateMenuBar (Widget, const char *, ArgList, int);
Widget XmCreateMessageBox (Widget, const char *, ArgList, int);   
Widget XmCreateMessageDialog (Widget, const char *, ArgList, int);   
Widget XmCreatePulldownMenu (Widget, const char *, ArgList, int);   
Widget XmCreatePushButton (Widget, const char *, ArgList, int);
Widget XmCreatePushButtonGadget (Widget, const char *, ArgList, int);   
Widget XmCreateRadioBox (Widget, const char *, ArgList, int);
Widget XmCreateRowColumn (Widget, const char *, ArgList, int);
Widget XmCreateScale (Widget, const char *, ArgList, int);
Widget XmCreateScrolledText (Widget, const char *, ArgList, int);
Widget XmCreateScrolledWindow (Widget, const char *, ArgList, int);
Widget XmCreateScrolledList (Widget, const char *, ArgList, int);
Widget XmCreateScrollBar (Widget, const char *, ArgList, int);
Widget XmCreateSeparator (Widget, const char *, ArgList, int);
Widget XmCreateSeparatorGadget (Widget, const char *, ArgList, int);
Widget XmCreateShell (Widget, const char *, ArgList, int);
Widget XmCreateText (Widget, const char *, ArgList, int);
#define XmCreateTextField  XmCreateText
Widget XmCreateToggleButton (Widget, const char *, ArgList, int);
Widget XmCreateToggleButtonGadget (Widget, const char *, ArgList, int);   
Atom XmInternAtom (Display *display, String name, Boolean only_if_exists);
void XmListAddItem (Widget self, XmString item, int pos);
void XmListAddItems (Widget widget, XmString *items, int item_count, int position);
void XmListAddItemsUnselected (Widget widget, XmString *items, int item_count, int position);
void XmListAddItemUnselected (Widget widget, XmString item, int position);
void XmListDeleteAllItems (Widget widget);
void XmListDeleteItem (Widget widget, XmString item);
void XmListDeleteItems (Widget widget, XmString *items, int item_count);
void XmListDeleteItemsPos (Widget widget, int item_count, int position);
void XmListDeletePos (Widget widget, int position);
void XmListDeselectAllItems (Widget widget);
void XmListDeselectItem (Widget widget, XmString item);
void XmListDeselectPos (Widget widget, int position);
Boolean XmListGetMatchPos (Widget widget, XmString item, int **position_list, int *position_count);
Boolean XmListGetSelectedPos (Widget self, int **position_list, int *position_count);
Boolean XmListItemExists (Widget widget, XmString item);
int XmListItemPos (Widget widget, XmString item);
void XmListReplaceItemsPos (Widget widget, XmString *new_items, int item_count, int position);
void XmListReplaceItemsPosUnselected (Widget widget, XmString *new_items, int item_count, int position);
void XmListSelectItem (Widget widget, XmString item, Boolean notify);
void XmListSelectPos (Widget widget, int position, Boolean notify);
void XmListSetAddMode (Widget widget, Boolean mode);
void XmListSetBottomItem (Widget widget, XmString item);
void XmListSetBottomPos (Widget widget, int position);
void XmListSetHorizPos (Widget widget, int position);
void XmListSetItem (Widget widget, XmString item);
void XmListSetPos (Widget widget, int position);
Widget XmMessageBoxGetChild (Widget me, int child);
void XmScaleGetValue (Widget widget, int *value_return);
void XmScaleSetValue (Widget widget, int value);
void XmScrollBarGetValues (Widget me, int *value, int *sliderSize, int *increment, int *pageIncrement);
void XmScrollBarSetValues (Widget me, int value, int sliderSize, int increment, int pageIncrement, Boolean notify);
XmString XmStringCreateSimple (const char *cstring);
void XmStringFree (XmString self);
void XmTextClearSelection (Widget widget, long time);
Boolean XmTextCopy (Widget widget, long time);
Boolean XmTextCut (Widget widget, long time);
int XmTextGetBaseline (Widget widget);
Boolean XmTextGetEditable (Widget widget);
#define XmTextFieldGetEditable  XmTextGetEditable
XmTextPosition XmTextGetInsertionPosition (Widget widget);
XmTextPosition XmTextGetLastPosition (Widget widget);
int XmTextGetMaxLength (Widget widget);
#define XmTextFieldGetMaxLength  XmTextGetMaxLength
char * XmTextGetSelection (Widget widget);
Boolean XmTextGetSelectionPosition (Widget widget, XmTextPosition *left, XmTextPosition *right);
/* XmTextSource XmTextGetSource (Widget widget); */
char * XmTextGetString (Widget widget);
#define XmTextFieldGetString  XmTextGetString
XmTextPosition XmTextGetTopCharacter (Widget widget);
void XmTextInsert (Widget widget, XmTextPosition position, char *value);
Boolean XmTextPaste (Widget widget);
Boolean XmTextPosToXY (Widget widget, XmTextPosition position, Position *x, Position *y);
Boolean XmTextRemove (Widget widget);
void XmTextReplace (Widget widget, XmTextPosition from_pos, XmTextPosition to_pos, char *value);
void XmTextScroll (Widget widget, int lines);
void XmTextSetAddMode (Widget widget, Boolean state);   /* Ignored: does not conform to Mac guidelines. */
void XmTextSetEditable (Widget widget, Boolean editable);
#define XmTextFieldSetEditable  XmTextSetEditable
void XmTextSetHighlight (Widget widget, XmTextPosition left, XmTextPosition right, XmHighlightMode mode);
void XmTextSetInsertionPosition (Widget widget, XmTextPosition position);
void XmTextSetMaxLength (Widget widget, int max_length);
#define XmTextFieldSetMaxLength  XmTextSetMaxLength
void XmTextSetSelection (Widget widget, XmTextPosition first, XmTextPosition last, long time);
/* void XmTextSetSource (Widget widget, XmTextSource source, XmTextPosition top_character, XmTextPosition cursor_position); */
void XmTextSetString (Widget widget, const char *value);
#define XmTextFieldSetString  XmTextSetString
void XmTextSetTopCharacter (Widget widget, XmTextPosition top_character);
void XmTextShowPosition (Widget widget, XmTextPosition position);
XmTextPosition XmTextXYToPos (Widget widget, Position x, Position y);
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

/********** MENUS **********/

/* Button layout and state: */
#define motif_INSENSITIVE  (1 << 8)
#define motif_CHECKABLE  (1 << 9)
#define motif_CHECKED  (1 << 10)

/* Accelerators: */
#define motif_OPTION  (1 << 21)
#define motif_SHIFT  (1 << 22)
#define motif_COMMAND  (1 << 23)
#define motif_LEFT_ARROW  1
#define motif_RIGHT_ARROW  2
#define motif_UP_ARROW  3
#define motif_DOWN_ARROW  4
#define motif_PAUSE  5
#define motif_DELETE  6
#define motif_INSERT  7
#define motif_BACKSPACE  8
#define motif_TAB  9
#define motif_LINEFEED  10
#define motif_HOME  11
#define motif_END  12
#define motif_ENTER  13
#define motif_PAGE_UP  14
#define motif_PAGE_DOWN  15
#define motif_ESCAPE  16
#define motif_F1  17
#define motif_F2  18
#define motif_F3  19
#define motif_F4  20
#define motif_F5  21
#define motif_F6  22
#define motif_F7  23
#define motif_F8  24
#define motif_F9  25
#define motif_F10  26
#define motif_F11  27
#define motif_F12  28

Widget motif_addShell (Widget widget, long flags);
Widget motif_addMenuBar (Widget form);
Widget motif_addMenu (Widget bar, const wchar_t *title, long flags);
Widget motif_addMenu2 (Widget bar, const wchar_t *title, long flags, Widget *menuTitle);
/* Flags is a combination of the above defines. */

Widget motif_addItem (Widget menu, const wchar_t *title, long flags,
	void (*commandCallback) (Widget, XtPointer, XtPointer), const void *closure);
/* Flags is a combination of the above defines. */

Widget motif_addSeparator (Widget menu);

int motif_getResolution (Widget widget);

Widget GuiText_createScrolled (Widget parent, const char *name, int editable, int topOffset);
void GuiText_setFontSize (Widget me, int size);
void GuiText_undo (Widget me);
void GuiText_redo (Widget me);
void GuiText_updateChangeCountAfterSave (Widget me);
wchar_t *GuiText_getStringW (Widget me);
void GuiText_setStringW (Widget me, const wchar_t *text);
wchar_t * GuiText_getSelectionW (Widget widget);
void GuiText_replaceW (Widget widget, XmTextPosition from_pos, XmTextPosition to_pos, const wchar_t *value);

void GuiWindow_setTitleW (Widget me, const wchar_t *titleW);
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

/********** EVENTS **********/

#if defined (macintosh)
	typedef XEvent *MotifEvent;
	#define MotifEvent_fromCallData(call)  ((MotifEvent) call)
	#define MotifEvent_shiftKeyPressed(event)  ((event -> modifiers & 512) != 0)
	#define MotifEvent_x(event)  (event -> where. h)
	#define MotifEvent_y(event)  (event -> where. v)
	#define MotifEvent_isButtonPressedEvent(event)  (event -> what == mouseDown)
	#define MotifEvent_isKeyPressedEvent(event)  (event -> what == keyDown)
#elif defined (_WIN32)
	typedef WinDrawingAreaEvent *MotifEvent;
	#define MotifEvent_fromCallData(call)  ((MotifEvent) call)
	#define MotifEvent_shiftKeyPressed(event)  (event -> shiftKeyPressed != 0)
	#define MotifEvent_x(event)  (event -> x)
	#define MotifEvent_y(event)  (event -> y)
	#define MotifEvent_isButtonPressedEvent(event)   (event -> message == WM_LBUTTONDOWN)
	#define MotifEvent_isKeyPressedEvent(event)   (event -> message == WM_CHAR)
#else
	typedef XEvent *MotifEvent;
	#define MotifEvent_fromCallData(call)  (((XmDrawingAreaCallbackStruct *) call) -> event)
	#define MotifEvent_shiftKeyPressed(event)  ((event -> xkey.state & ShiftMask) != 0)
	#define MotifEvent_x(event)  (event -> xbutton.x)
	#define MotifEvent_y(event)  (event -> xbutton.y)
	#define MotifEvent_isButtonPressedEvent(event)  (event -> type == ButtonPress)
	#define MotifEvent_isKeyPressedEvent(event)  (event -> type == KeyPress)
#endif

void motif_setOpenDocumentCallback (int (*openDocumentCallback) (MelderFile file));
void motif_setQuitApplicationCallback (int (*quitApplicationCallback) (void));

/********** COMPATIBILITY **********/

#ifdef UNIX
	#define MOTIF_CONST_CHAR_ARG(a)  (char *) (a)
#else
	#define MOTIF_CONST_CHAR_ARG(a)  (a)
#endif
/*
	Use as follows:
		int myFunc (Widget w, const char *text) {
			...
			XmTextSetString (w, MOTIF_CONST_CHAR_ARG (text));
			...
		}
*/

#define MOTIF_CALLBACK(f)  static void f (Widget w, XtPointer void_me, XtPointer call) { \
	(void) w; (void) void_me; (void) call; {
#define MOTIF_CALLBACK_END  } }

#define GUI_CALLBACK  static void
#define GUI_ARGS  Widget w, XtPointer void_me, XtPointer call
#define GUI_IAM(klas)  (void) w; (void) void_me; (void) call; iam (klas);

/* End of file Gui.h */
#endif
