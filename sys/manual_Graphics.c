/* manual_Graphics.c */
/* Paul Boersma */

#include "ManPagesM.h"

#ifdef THIS_IS_WHERE_I_GOT_TIRED_CONVERTING_THE_HEADER_FILE_INTO_A_MANUAL
/********** Device-dependent. **********/
void *Graphics_x_getGC (Graphics g);
	/* Returns a GC. */
void Graphics_nextSheetOfPaper (Graphics me);   /* For printers. */
#endif

void manual_Graphics_init (ManPages me);
void manual_Graphics_init (ManPages me) {

MAN_BEGIN ("A4", "ppgb", 19960709)
INTRO ("the most common paper size for printers in Europe, 210 \\xx 297 mm.")
MAN_END

MAN_BEGIN ("Graphics coordinate systems", "ppgb", 19961127)
INTRO ("The coordinate systems of the @@Graphics library@ are derived from GKS:")
ENTRY ("Workstation viewport")
NORMAL ("the part of the device that can receive graphics output. "
	"The workstation viewport is a rectangle "
	"expressed as [%x1DC, %x2DC] \\xx [%y1DC, %y2DC] in Device Coordinates (DC), "
	"which are screen pixels, PostScript coordinates, or printer dots.")
NORMAL ("You can change the workstation viewport with @Graphics_setWsViewport.")
ENTRY ("Workstation window")
NORMAL ("the workstation viewport's Normalized Device Coordinates (NDC). "
	"For a rectangular viewport, you will normally want it to be [0.0, 1.0] \\xx [0.0, 1.0]. "
	"For a non-rectangular viewport, you will either want [0.0, 1.0] \\xx [0.0, 1.0], "
	"or something with an aspect ratio of 1 (so that a square has equal dimensions in the %x and %y directions). "
	"Instead of these \"unit\" windows, you may choose any other device-independent coordinate system, "
	"e.g. centimetres or inches.")
NORMAL ("You can change the workstation window with @Graphics_setWsWindow.")
ENTRY ("World viewport")
NORMAL ("the rectangular part of the workstation viewport that will "
	"receive graphics output. The world viewport is expressed in NDC. "
	"Upon creation of a Graphics, "
	"its world viewport is initialized to [0.0, 1.0] \\xx [0.0, 1.0].")
NORMAL ("You can change the world viewport with @Graphics_setViewport, @Graphics_insetViewport, and @Graphics_resetViewport.")
ENTRY ("World window")
NORMAL ("the world viewport's world coordinates (WC). "
	"All drawing routines use world coordinates. "
	"These can be anything which is associated with the interpretation of the %x and %y axes "
	"in the world represented by your drawings. Upon creation of a Graphics, "
	"its world window is initialized to [0.0, 1.0] \\xx [0.0, 1.0].")
NORMAL ("You will often change the world window with @Graphics_setWindow.")
ENTRY ("Inquiry functions")
LIST_ITEM ("\\bu @Graphics_inqViewport")
LIST_ITEM ("\\bu @Graphics_inqWindow")
LIST_ITEM ("\\bu @Graphics_inqWsViewport")
LIST_ITEM ("\\bu @Graphics_inqWsWindow")
MAN_END

MAN_BEGIN ("Graphics creation routines", "ppgb", 19960718)
INTRO ("The routines you have to call before you can access the rest of the @@Graphics library@.")
INTRO ("All of the functions $Graphics_createXXXXXX set the output attributes to the following values:")
LIST_ITEM ("font: $Graphics_HELVETICA")
LIST_ITEM ("font size: 9")
LIST_ITEM ("font style: $Graphics_NORMAL (not italic, not bold)")
LIST_ITEM ("line type: $Graphics_DRAWN (solid line)")
ENTRY ("Routines")
LIST_ITEM ("@Graphics_create")
LIST_ITEM ("@Graphics_create_epsfile")
LIST_ITEM ("@Graphics_create_macintosh")
LIST_ITEM ("@Graphics_create_postscriptjob")
LIST_ITEM ("@Graphics_create_xmdrawingarea")
LIST_ITEM ("@Graphics_create_xwindow")
MAN_END

MAN_BEGIN ("Graphics drivers", "ppgb", 19960718)
INTRO ("The implementers of the device-dependent behaviour of the @@Graphics library@. "
	"These objects inherit from the Graphics class.")
ENTRY ("Common behaviour")
NORMAL ("In their initialization routines, all the drivers should set their %resolution attribute "
	"and their maximum Device Coordinates attributes. If they change any coordinate system, "
	"this has to be done by the appropriate #Graphics_setXxx routine, because the transformation matrix "
	"must be recomputed.")
ENTRY ("Drivers")
LIST_ITEM ("GraphicsXwindows")
LIST_ITEM ("GraphicsScreen")
LIST_ITEM ("GraphicsPostscript")
LIST_ITEM ("GraphicsMotif")
MAN_END

MAN_BEGIN ("Graphics input routines", "ppgb", 19970331)
INTRO ("The input routines of the @@Graphics library@.")
ENTRY ("Purpose")
NORMAL ("to track dragging.")
ENTRY ("Routines")
LIST_ITEM ("\\bu @Graphics_mouseStillDown")
LIST_ITEM ("\\bu @Graphics_getMouseLocation")
MAN_END

MAN_BEGIN ("Graphics library", "ppgb", 20000929)
INTRO ("One of the libraries used for programming the Praat shell.")
ENTRY ("Purpose")
NORMAL ("to draw publication-quality graphics in a system-independent way, "
	"on Xwindows and Macintosh screens and on PostScript printers.")
ENTRY ("Class definition")
NORMAL ("Graphics inherits from @Thing. It is defined as an opaque pointer:")
PROTOTYPE ("##typedef struct Graphics *Graphics;")
ENTRY ("Deeper information")
LIST_ITEM ("@@Graphics coordinate systems")
LIST_ITEM ("@@Graphics creation routines")
LIST_ITEM ("@@Graphics input routines")
LIST_ITEM ("@@Graphics output routines")
LIST_ITEM ("@@Graphics output attributes")
LIST_ITEM ("@@Graphics margin routines")
LIST_ITEM ("@@Graphics millimetre routines")
LIST_ITEM ("@@Graphics_textWidth")
LIST_ITEM ("@@Graphics_textWidth_ps")
LIST_ITEM ("@@Graphics millimetre routines")
LIST_ITEM ("@@Graphics recording routines")
LIST_ITEM ("@@Graphics workstation routines")
LIST_ITEM ("@@Graphics drivers")
MAN_END

MAN_BEGIN ("Graphics margin routines", "ppgb", 20000526)
INTRO ("Routines that draw in the margins of the current viewport. "
	"The following routines allow drawing text and numbers in margins "
	"around an inner drawing. "
	"The margin width depends on the current font size. "
	"Dotted lines can be drawn in the inner drawing.")
ENTRY ("Setting the viewport so that there is room for margins")
PROTOTYPE ("##void Graphics_setInner (Graphics #%g##);")
NORMAL ("Purpose: to inset the viewport so as to keep margins around it.")
NORMAL ("Usage: call this before drawing into the inner drawing.")
PROTOTYPE ("##void Graphics_unsetInner (Graphics #%g##);")
NORMAL ("Usage: call this after drawing into the inner drawing.")
ENTRY ("Delimiting the margins")
NORMAL ("Draw a box around an inset viewport.")
PROTOTYPE ("##void Graphics_drawInnerBox (Graphics #%g##);")
ENTRY ("Text along the edges of the picture")
NORMAL ("Write text around an inset viewport.")
PROTOTYPE ("##void Graphics_textLeft (Graphics #%g##, int #%far##, char *#%text##);")
PROTOTYPE ("##void Graphics_textRight (Graphics #%g##, int #%far##, char *#%text##);")
PROTOTYPE ("##void Graphics_textBottom (Graphics #%g##, int #%far##, char *#%text##);")
PROTOTYPE ("##void Graphics_textTop (Graphics #%g##, int #%far##, char *#%text##);")
TAG ("%far")
DEFINITION ("%far means just inside the viewport, %not %far means just around the inner box.")
ENTRY ("Numbers, ticks, dotted lines")
NORMAL ("Write world-coordinate values and draw ticks around an inset viewport, and draw dotted lines through your picture.")
PROTOTYPE ("##void Graphics_marksLeft (Graphics #%g##, int #%numberOfMarks##, int #%haveNumbers, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksRight (Graphics #%g##, int #%numberOfMarks##, int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksBottom (Graphics #%g##, int #%numberOfMarks##, int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksTop (Graphics #%g##, int #%numberOfMarks##, int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksLeftLogarithmic (Graphics #%g##, int #%numberOfMarksPerDecade##, int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksRightLogarithmic (Graphics #%g##, int #%numberOfMarksPerDecade##, int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksBottomLogarithmic (Graphics #%g##, int #%numberOfMarksPerDecade##, int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksTopLogarithmic (Graphics #%g##, int #%numberOfMarksPerDecade##, int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_markLeft (Graphics #%g##, double #%yWC, int #%hasNumber##, int #%hasTick##, int #%hasDottedLine##, char *#%text##);")
PROTOTYPE ("##void Graphics_markRight (Graphics #%g##, double #%yWC, int #%hasNumber##, int #%hasTick##, int #%hasDottedLine##, char *#%text##);")
PROTOTYPE ("##void Graphics_markBottom (Graphics #%g##, double #%xWC, int #%hasNumber##, int #%hasTick##, int #%hasDottedLine##, char *#%text##);")
PROTOTYPE ("##void Graphics_markTop (Graphics #%g##, double #%xWC, int #%hasNumber##, int #%hasTick##, int #%hasDottedLine##, char *#%text##);")
PROTOTYPE ("##void Graphics_markLeftLogarithmic (Graphics #%g##, double #%y /* > 0 */##, int #%hasNumber##, int #%hasTick##, int #%hasDottedLine##, char *#%text##);")
PROTOTYPE ("##void Graphics_markRightLogarithmic (Graphics #%g##, double #%y##, int #%hasNumber##, int #%hasTick##, int #%hasDottedLine##, char *#%text##);")
PROTOTYPE ("##void Graphics_markBottomLogarithmic (Graphics #%g##, double #%x##, int #%hasNumber##, int #%hasTick##, int #%hasDottedLine##, char *#%text##);")
PROTOTYPE ("##void Graphics_markTopLogarithmic (Graphics #%g##, double #%x##, int #%hasNumber##, int #%hasTick##, int #%hasDottedLine##, char *#%text##);")
PROTOTYPE ("##void Graphics_marksLeftEvery (Graphics #%g##, double #%units##, double #%distance##, "
	"int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksRightEvery (Graphics #%g, double #%units, double #%distance##, "
	"int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksBottomEvery (Graphics #%g, double #%units, double #%distance##, "
	"int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
PROTOTYPE ("##void Graphics_marksTopEvery (Graphics #%g, double #%units, double #%distance##, "
	"int #%haveNumbers##, int #%haveTicks##, int #%haveDottedLines##);")
TAG ("%text")
DEFINITION ("may be NULL.")
TAG ("%hasNumber")
DEFINITION ("determines whether the %x or %y world coordinate is drawn. "
	"If %hasNumber is TRUE and %text is not NULL, the number and text overwrite each other.")
TAG ("%distance, %units")
DEFINITION ("%distance is expressed in %units; e.g., to draw marks every 20 milliseconds "
	"while time is expressed in seconds, call $$Graphics_marksBottomEvery (g, 0.001, 20, ...);$")
TAG ("%numberOfMarksPerDecade")
DEFINITION ("the number of marks in going from e.g. 100 to 1000 along a logarithmic axis. "
	"This number is between 1 and 7. E.g. if this number is 3, marks are drawn "
	"at 100, 200, 500, 1000, 2000, and so on.")
ENTRY ("Usage")
CODE ("Graphics_setInner (g);")
CODE ("Graphics_setWindow (g, ...);   // Draw your picture.")
CODE ("Graphics_polyline (g, ...);")
CODE ("...")
CODE ("Graphics_setInner (g);")
CODE ("Graphics_drawInnerBox (g);")
CODE ("Graphics_textTop (g, FALSE, \"This is a picture of a polygon\");")
CODE ("Graphics_textBottom (g, TRUE, \"Time (s)\");")
CODE ("Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);")
CODE ("Graphics_textLeft (g, TRUE, \"Frequency (Hz)\");")
CODE ("Graphics_marksLeft (g, 2, TRUE, TRUE, FALSE);")
MAN_END

MAN_BEGIN ("Graphics millimetre routines", "ppgb", 19970331)
INTRO ("Routines to convert world coordinates to mm and back in the @@Graphics library@.")
ENTRY ("Routines")
PROTOTYPE ("##double @Graphics_dxMMtoWC (Graphics #%me##, double #%dx_mm##);")
PROTOTYPE ("##double @Graphics_dyMMtoWC (Graphics #%me##, double #%dy_mm##);")
PROTOTYPE ("##double @Graphics_distanceWCtoMM (Graphics %me##, double #%x1WC##, double #%y1WC##, double #%x2WC##, double #%y2WC##);")
PROTOTYPE ("##double @Graphics_dxWCtoMM (Graphics #%me##, double #%dxWC##);")
PROTOTYPE ("##double @Graphics_dyWCtoMM (Graphics #%me##, double #%dyWC##);")
MAN_END

MAN_BEGIN ("Graphics recording routines", "ppgb", 19970331)
INTRO ("The recording routines of the @@Graphics library@.")
NORMAL ("All Graphics output can be recorded for later playback on the same device (e.g., a screen) "
	"or on another device (e.g., a printer). These routines also allow file I/O of graphics output, "
	"and a simple undo mechanism.")
NORMAL ("Recordings can be erased with @Graphics_clearWs.")
ENTRY ("Recording routines")
LIST_ITEM ("\\bu @Graphics_startRecording")
LIST_ITEM ("\\bu @Graphics_stopRecording")
ENTRY ("Playback routines")
LIST_ITEM ("\\bu @Graphics_play")
LIST_ITEM ("\\bu @Graphics_readRecordings")
ENTRY ("File I/O routines")
LIST_ITEM ("\\bu @Graphics_writeRecordings")
LIST_ITEM ("\\bu @Graphics_readRecordings")
ENTRY ("Grouping routines (for undo)")
LIST_ITEM ("\\bu @Graphics_markGroup")
LIST_ITEM ("\\bu @Graphics_undoGroup")
MAN_END

MAN_BEGIN ("Graphics workstation routines", "ppgb", 19961127)
INTRO ("The workstation routines of the @@Graphics library@.")
ENTRY ("Routines")
LIST_ITEM ("\\bu @Graphics_setWsViewport")
LIST_ITEM ("\\bu @Graphics_setWsWindow")
LIST_ITEM ("\\bu @Graphics_inqWsViewport")
LIST_ITEM ("\\bu @Graphics_inqWsWindow")
LIST_ITEM ("\\bu @Graphics_clearWs")
LIST_ITEM ("\\bu @Graphics_flushWs")
LIST_ITEM ("\\bu @Graphics_updateWs")
LIST_ITEM ("\\bu @Graphics_DCtoWC")
LIST_ITEM ("\\bu @Graphics_WCtoDC")
MAN_END

MAN_BEGIN ("Graphics_clearWs", "ppgb", 19961127)
INTRO ("One of the @@Graphics workstation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_clearWs (Graphics #%g##);")
ENTRY ("Purpose")
NORMAL ("to clear the workstation viewport and to erase recorded graphics output.")
MAN_END

MAN_BEGIN ("Graphics_create", "ppgb", 19960709)
INTRO ("One of the @@Graphics creation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##Graphics Graphics_create (void);")
ENTRY ("Purpose")
NORMAL ("With #Graphics_create, you create a dummy Graphics that does not display any graphics. "
	"It can, however, record Graphics output messages sent to it. "
	"You can copy these recordings to another Graphics with $Graphics_play. "
	"This is useful in batch programmes that copy recorded output to files.")
ENTRY ("Return value")
NORMAL ("The newly created Graphics, or NULL in case of failure.")
MAN_END

MAN_BEGIN ("Graphics_create_epsfile", "ppgb", 19981022)
INTRO ("One of the @@Graphics creation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##Graphics Graphics_create_epsfile (char *#%fileName##, int #%resolution##, int #%spots##, "
	"double #%xmin##, double #%xmax##, double #%ymin##, double #%ymin##);")
ENTRY ("Purpose")
NORMAL ("to open a file for writing an @@Encapsulated PostScript@ (EPS) picture.")
ENTRY ("Arguments")
TAG ("%fileName")
DEFINITION ("the file in which the graphics output will be written; "
	"this file will be closed (not removed) with @forget.")
TAG ("%resolution")
DEFINITION ("virtual resolution in dots per inch (dpi); should be between 300 and 2000. "
	"This value will be used for the accuracy with which graphics is positioned, "
	"and for the number of lines drawn per inch for a heavily sampled function. "
	"For printers with a physical resolution of 600 dpi, a value of `600' is best; but `600' will look good on printers of every resolution: "
	"on a 300-dpi printer, a value of `300' may give a smaller file; on a 1200-dpi printer, "
	"you will only notice the difference with `1200' for very stiff line drawings.")
TAG ("%spots")
DEFINITION ("one of the following two constants:")
CODE ("        \\# define GraphicsPostscript_FINE  0")
CODE ("        \\# define GraphicsPostscript_PHOTOCOPYABLE  1")
DEFINITION ("The %spots parameter determines whether grey values look best directly from your PostScript printer (FINE) "
	"or best after photocopying (PHOTOCOPYABLE).")
TAG ("%xmin, %xmax, %ymin, %ymax")
DEFINITION ("bounding box in inches. "
	"For a full-page PostScript job on @A4 paper, "
	"these could be 0, 7.5, 1, and 12, respectively. "
	"For @@Encapsulated PostScript@, they should reflect the size of the picture (as small as possible).")
ENTRY ("Return value")
NORMAL ("the newly created Graphics, or NULL in case of failure.")
ENTRY ("Postconditions")
NORMAL ("Workstation viewport (NDC) is [%xmin, %xmax] \\xx [%ymin, %ymax].")
ENTRY ("Example")
NORMAL ("The following example paints the text \"Hallo\" and a line in the centre of an oblong picture.")
CODE ("Graphics ps = Graphics_create_epsfile (\"hallo.ps\", 600, GraphicsPostscript_FINE, 0.0, 6.0, 0.0, 2.0);")
CODE ("@Graphics_text (ps, 3.0, 1.0, \"Hallo\");")
CODE ("@Graphics_line (ps, 1.0, 2.0, 5.0, 2.0);")
CODE ("@forget (ps);")
NORMAL ("The resulting EPS file can be included in your favourite word processor. In Mirosoft Word, for instance, "
	"you choose \"File...\" from the \"Insert\" menu. The size of the resulting picture "
	"is 6.0 \\xx 2.0 inches (in our example). ")
NORMAL ("You will probably only see some text in the picture, "
	"like \"File name: xxx\" and \"Creator: xxx\", but your picture will be printed correctly on a PostScript printer.")
ENTRY ("Screen previews")
NORMAL ("On Macintosh, you can include a screen preview in the resource fork of the file, by using the ##Graphics recording# mechanism. "
	"This preview will be visible in your word processor. The algorithm is:")
CODE ("Graphics ps = @Graphics_create_epsfile (\"hallo.ps\", 600, GraphicsPostscript_FINE, 0.0, 6.0, 0.0, 2.0), preview;")
CODE ("Graphics_startRecording (ps);")
CODE ("@Graphics_text (ps, 3.0, 1.0, \"Hallo\");")
CODE ("@Graphics_line (ps, 1.0, 2.0, 5.0, 2.0);")
CODE ("preview = Graphics_create_macresourcefork (\"hallo.ps\", 0.0, 6.0, 0.0, 2.0);")
CODE ("Graphics_play (ps, preview);")
CODE ("@forget (ps);")
CODE ("@forget (preview);")
MAN_END

MAN_BEGIN ("Graphics_create_macintosh", "ppgb", 19961127)
INTRO ("One of the @@Graphics creation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##Graphics Graphics_create_macintosh (/* WindowPtr */ void *#%macWindow##, int #%resolution##);")
ENTRY ("Purpose")
NORMAL ("to prepare an existing Macintosh window for drawing.")
ENTRY ("Arguments")
TAG ("%macWindow")
DEFINITION ("a WindowPtr or TPPrPort.")
TAG ("%resolution")
DEFINITION ("dots per inch (screen 72, LaserWriter 300, StyleWriter 360).")
ENTRY ("Return value")
NORMAL ("the newly created Graphics, or NULL in case of failure.")
ENTRY ("Postconditions")
NORMAL ("Workstation viewport is (0, 100) \\xx (0, 100) pixels, "
	"with NDC co-ordinates (0, 1) \\xx (0, 1). Foreground is black, background is white.")
MAN_END

MAN_BEGIN ("Graphics_create_postscriptjob", "ppgb", 19981022)
INTRO ("One of the @@Graphics creation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##Graphics Graphics_create_postscriptjob (char *#%fileName##, int #%resolution##, int #%spots##);")
ENTRY ("Purpose")
NORMAL ("to open a file for writing a PostScript program that can be printed directly.")
ENTRY ("Arguments")
TAG ("%fileName")
DEFINITION ("the file in which the graphics output will be written; "
	"this file will be closed (not removed) with @forget.")
TAG ("%resolution")
DEFINITION ("virtual resolution in dots per inch (dpi); should be between 300 and 2000. "
	"This value will be used for the accuracy with which graphics is positioned, "
	"and for the number of lines drawn per inch for a heavily sampled function. "
	"For printers with a physical resolution of 600 dpi, a value of `600' is best; "
	"but `600' will look good on printers of every resolution: "
	"on a 300-dpi printer, a value of `300' may give a smaller file; on a 1200-dpi printer, "
	"you will only notice the difference with `1200' for very stiff line drawings.")
TAG ("%spots")
DEFINITION ("one of the  following two constants:")
CODE ("        \\# define GraphicsPostscript_FINE  0")
CODE ("        \\# define GraphicsPostscript_PHOTOCOPYABLE  1")
DEFINITION ("The %spots parameter determines whether grey values look best directly from your PostScript printer (FINE) "
	"or best after photocopying (PHOTOCOPYABLE).")
ENTRY ("Return value")
NORMAL ("the newly created Graphics, or NULL in case of failure.")
ENTRY ("Postconditions")
NORMAL ("Workstation viewport (NDC) is [0.0, 7.5] \\xx [1.0, 12.0].")
ENTRY ("Example")
NORMAL ("The following example paints the text \"Hallo\" in the centre of a page.")
CODE ("Graphics ps = Graphics_create_postscriptjob (\"hallo.ps\", 600, GraphicsPostscript_FINE);")
CODE ("@Graphics_text (ps, 3.0, 6.0, \"Hallo\");")
CODE ("@forget (ps);")
NORMAL ("The resulting PostScript file can be printed from the Unix command line with something like")
CODE ("lp -onobanner -c hallo.ps")
MAN_END

MAN_BEGIN ("Graphics_create_xmdrawingarea", "ppgb", 19961127)
INTRO ("One of the @@Graphics creation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##Graphics Graphics_create_xmdrawingarea (/* Widget */ void *#%drawingArea##);")
ENTRY ("Purpose")
NORMAL ("to prepare an existing Motif drawingArea widget for drawing.")
ENTRY ("Return value")
NORMAL ("the newly created Graphics, or NULL in case of failure.")
ENTRY ("Postconditions")
NORMAL ("The workstation viewport is computed from the widget resources "
	"as [%marginWidth, %width - %marginWidth] \\xx [%marginHeight, %height - %marginHeight], "
	"with NDC co-ordinates [0.0, 1.0] \\xx [0.0, 1.0]. "
	"Foreground is black, background is white.")
MAN_END

MAN_BEGIN ("Graphics_create_xwindow", "ppgb", 19960716)
INTRO ("One of the @@Graphics creation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##Graphics Graphics_create_xwindow (/* Display * */ void *#%display##, /* Window */ unsigned long #%window##);")
ENTRY ("Purpose")
NORMAL ("to prepare an existing X window for drawing.")
ENTRY ("Return value")
NORMAL ("the newly created Graphics, or NULL in case of failure.")
ENTRY ("Postconditions")
NORMAL ("Workstation viewport is [0, 100] \\xx [0, 100] pixels, "
	"with NDC co-ordinates [0.0, 1.0] \\xx [0.0, 1.0]. Foreground is black, background is white.")
MAN_END

MAN_BEGIN ("Graphics_DCtoWC", "ppgb", 19961127)
INTRO ("One of the @@Graphics workstation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_DCtoWC (Graphics #%g##, short #%xDC##, short #%yDC##, double *#%xWC##, double *#%yWC##);")
ENTRY ("Purpose")
NORMAL ("to compute world coordinates from device coordinates, the reverse from @Graphics_WCtoDC.")
ENTRY ("Usage")
NORMAL ("Mainly used to translate the coordinates from an input event.")
MAN_END

MAN_BEGIN ("Graphics_distanceWCtoMM", "ppgb", 19970331)
INTRO ("One of the @@Graphics millimetre routines@.")
ENTRY ("Purpose")
NORMAL ("to compute the distance in mm between two points specified in world coordinates.")
ENTRY ("Syntax")
PROTOTYPE ("##double Graphics_distanceWCtoMM (Graphics #%me##, double #%x1WC##, double #%y1WC##, double #%x2WC##, double #%y2WC##);")
ENTRY ("Arguments")
TAG ("%x1WC, %y1WC")
DEFINITION ("the world coordinates of the first point.")
TAG ("%x2WC, %y2WC")
DEFINITION ("the world coordinates of the second point.")
ENTRY ("Return value")
NORMAL ("The distance in millimetres; always positive or zero.")
ENTRY ("Usage")
NORMAL ("You can use #Graphics_distanceWCtoMM to see whether the user clicked near a specified point:")
CODE ("short xDC = event -> where. x, yDC = event -> where. y;")
CODE ("double xWC, yWC;")
CODE ("@Graphics_DCtoWC (g, xDC, yDC, & xWC, & yWC);")
CODE ("if (#Graphics_distanceWCtoMM (g, xWC, yWC, time, frequency) < 2.0) {")
CODE ("   /* User clicked within 2 mm from my (time, frequency) point. Take appropriate action. */")
MAN_END

MAN_BEGIN ("Graphics_dxMMtoWC", "ppgb", 19970331)
INTRO ("One of the @@Graphics millimetre routines@.")
ENTRY ("Purpose")
NORMAL ("to convert a distance in mm to a distance in horizontal world coordinates.")
ENTRY ("Syntax")
PROTOTYPE ("##double Graphics_dxMMtoWC (Graphics #%me##, double #%dx_mm##);")
ENTRY ("Argument")
TAG ("%dx_mm")
DEFINITION ("the distance in millimetres; may be negative.")
ENTRY ("Return value")
NORMAL ("The distance in horizontal world coordinates; has the same sign as %dx_mm.")
MAN_END

MAN_BEGIN ("Graphics_dxWCtoMM", "ppgb", 19970331)
INTRO ("One of the @@Graphics millimetre routines@.")
ENTRY ("Purpose")
NORMAL ("to convert a distance in horizontal world coordinates to a distance in mm.")
ENTRY ("Syntax")
PROTOTYPE ("##double Graphics_dxWCtoMM (Graphics #%me##, double #%dxWC##);")
ENTRY ("Argument")
TAG ("%dxWC")
DEFINITION ("the distance in horizontal world coordinates; may be negative.")
ENTRY ("Return value")
NORMAL ("The distance in millimetres; has the same sign as %dxWC.")
MAN_END

MAN_BEGIN ("Graphics_dyMMtoWC", "ppgb", 19970331)
INTRO ("One of the @@Graphics millimetre routines@.")
ENTRY ("Purpose")
NORMAL ("to convert a distance in mm to a distance in vertical world coordinates.")
ENTRY ("Syntax")
PROTOTYPE ("##double Graphics_dyMMtoWC (Graphics #%me##, double #%dy_mm##);")
ENTRY ("Argument")
TAG ("%dy_mm")
DEFINITION ("the distance in millimetres; may be negative.")
ENTRY ("Return value")
NORMAL ("The distance in vertical world coordinates; has the same sign as %dy_mm.")
MAN_END

MAN_BEGIN ("Graphics_dyWCtoMM", "ppgb", 19970331)
INTRO ("One of the @@Graphics millimetre routines@.")
ENTRY ("Purpose")
NORMAL ("to convert a distance in vertical world coordinates to a distance in mm.")
ENTRY ("Syntax")
PROTOTYPE ("##double Graphics_dyWCtoMM (Graphics #%me##, double #%dyWC##);")
ENTRY ("Argument")
TAG ("%dyWC")
DEFINITION ("the distance in vertical world coordinates; may be negative.")
ENTRY ("Return value")
NORMAL ("The distance in millimetres; has the same sign as %dyWC.")
MAN_END

MAN_BEGIN ("Graphics_flushWs"," ppgb", 19961127)
INTRO ("One of the @@Graphics workstation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_flushWs (Graphics #%g##);")
ENTRY ("Purpose")
NORMAL ("to draw deferred output immediately.")
ENTRY ("Usage")
NORMAL ("Necessary for movies, after the drawing of each picture.")
MAN_END

MAN_BEGIN ("Graphics_getMouseLocation", "ppgb", 19970331)
INTRO ("One of the @@Graphics input routines@.")
ENTRY ("Purpose")
NORMAL ("to determine the whereabouts of the mouse during a drag-and-drop operation.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_getMouseLocation (Graphics #%me##, double *#%xWC##, double *#%yWC##);")
ENTRY ("Usage")
NORMAL ("You should use Graphics_getMouseLocation only inside a @Graphics_mouseStillDown loop:")
CODE ("Graphics_xorOn (g, Graphics_BLUE);")
CODE ("while (@Graphics_mouseStillDown (g)) {")
CODE ("    double x_old = x, y_old = y;")
CODE ("    drawMe (x_old, y_old);")
CODE ("    #Graphics_getMouseLocation (g, & x, & y);")
CODE ("    drawMe (x_old, y_old);")
CODE ("}")
CODE ("Graphics_xorOff (g);")
MAN_END

MAN_BEGIN ("Graphics_inqViewport", "ppgb", 19970911)
INTRO ("A routine to inquire the current settings of one of the @@Graphics coordinate systems@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_inqViewport (Graphics #%g##, short *#%x1NDC##, short *#%x2NDC##, short *#%y1NDC##, short *#%y2NDC##);")
MAN_END

MAN_BEGIN ("Graphics_inqWindow", "ppgb", 19970911)
INTRO ("A routine to inquire the current settings of one of the @@Graphics coordinate systems@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_inqWindow (Graphics #%g##, double *#%x1WC##, double *#%x2WC##, double *#%y1WC##, double *#%y2WC##);")
MAN_END

MAN_BEGIN ("Graphics_inqWsViewport", "ppgb", 19961127)
INTRO ("One of the @@Graphics workstation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_inqWsViewport (Graphics #%g##, short *#%x1DC##, short *#%x2DC##, short *#%y1DC##, short *#%y2DC##);")
ENTRY ("Purpose")
NORMAL ("to inquire the current settings of one of the @@Graphics coordinate systems@.")
MAN_END

MAN_BEGIN ("Graphics_inqWsWindow", "ppgb", 19961127)
INTRO ("One of the @@Graphics workstation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_inqWsWindow (Graphics #%g##, double *#%x1NDC##, double *#%x2NDC##, double *#%y1NDC##, double *#%y2NDC##);")
ENTRY ("Purpose")
NORMAL ("to inquire the current settings of one of the @@Graphics coordinate systems@.")
MAN_END

MAN_BEGIN ("Graphics_insetViewport", "ppgb", 19961127)
INTRO ("A routine to change one of the @@Graphics coordinate systems@.")
ENTRY ("Syntax")
PROTOTYPE ("##typedef struct { double #%x1NDC##, #%x2NDC##, #%y1NDC##, #%y2NDC##; } Graphics_Viewport;")
PROTOTYPE ("##Graphics_Viewport Graphics_insetViewport "
	"(Graphics #%g##, double #%x1rel##, double #%x2rel##, double #%y1rel##, double #%y2rel##);")
ENTRY ("Purpose")
NORMAL ("to set the part of the device that will receive output, relative to the current viewport.")
ENTRY ("Return value")
NORMAL ("the previous viewport.")
ENTRY ("Usage")
NORMAL ("You will often bracket this routine with @Graphics_resetViewport.")
MAN_END

MAN_BEGIN ("Graphics_markGroup", "ppgb", 19970331)
INTRO ("One of the @@Graphics recording routines@.")
ENTRY ("Purpose")
NORMAL ("to divide the drawings into groups.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_markGroup (Graphics #%me##);")
ENTRY ("Behaviour")
NORMAL ("Marks the beginning of a group of graphics output. If recording is on, "
	"this mark is recorded so that @Graphics_undoGroup can find it.")
ENTRY ("Usage")
NORMAL ("This routine only makes sense when recording.")
MAN_END

MAN_BEGIN ("Graphics_mouseStillDown", "ppgb", 19970331)
INTRO ("One of the @@Graphics input routines@.")
ENTRY ("Purpose")
NORMAL ("to detect the mouse-up event that signals the end of a drag-and-drop operation.")
ENTRY ("Syntax")
PROTOTYPE ("##int Graphics_mouseStillDown (Graphics #%me##);")
ENTRY ("Usage")
CODE ("Graphics_xorOn (g, Graphics_BLUE);")
CODE ("while (#Graphics_mouseStillDown (g)) {")
CODE ("    double x_old = x, y_old = y;")
CODE ("    drawMe (x_old, y_old);")
CODE ("    @Graphics_getMouseLocation (g, & x, & y);")
CODE ("    drawMe (x_old, y_old);")
CODE ("}")
CODE ("Graphics_xorOff (g);")
MAN_END

MAN_BEGIN ("Graphics_play", "ppgb", 19970331)
INTRO ("One of the @@Graphics recording routines@.")
ENTRY ("Purpose")
NORMAL ("to play back recorded graphics output on the same or another device.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_play (Graphics #%source##, Graphics #%target##);")
ENTRY ("Behaviour")
NORMAL ("Copies to %target all the graphics output that has been recorded in %source, without clearing %target before. "
	"Of course, if %source is equal to %target, recording is temporarily paused.")
ENTRY ("Printing example")
NORMAL ("To copy the underlying screen contents to a PostScript file (with enhancement of quality):")
CODE ("Graphics postScript = @Graphics_create_postscriptjob (\"plaatje.ps\", 600, GraphicsPostscript_FINE);")
CODE ("#Graphics_play (screen, postScript);")
CODE ("@forget (postScript);   // Closes the file, but does not remove it.")
ENTRY ("Redrawing example")
NORMAL ("You can use Graphics_play to redraw a Graphics in answer to an expose (or update) event; "
	"in that case, %source must be equal to %target.")
NORMAL ("To redraw a screen that has recorded its contents:")
CODE ("#Graphics_play (screen, screen);")
MAN_END

MAN_BEGIN ("Graphics_readRecordings", "ppgb", 19970331)
INTRO ("One of the @@Graphics recording routines@.")
ENTRY ("Purpose")
NORMAL ("to read previously saved graphics output from a file.")
ENTRY ("Syntax")
PROTOTYPE ("##int Graphics_readRecordings (Graphics #%me##, FILE *#%f##);")
ENTRY ("Return value")
NORMAL ("1 if OK, 0 in case of failure.")
ENTRY ("Behaviour")
NORMAL ("Adds the recordings in the stream %f to the recordings of the %me, "
	"without clearing the existing recordings in %me.")
ENTRY ("Usage")
NORMAL ("You would normally call @Graphics_updateWs after this, to visualize the changes in the recordings.")
MAN_END

MAN_BEGIN ("Graphics_resetViewport", "ppgb", 19961127)
INTRO ("A routine to change one of the @@Graphics coordinate systems@.")
ENTRY ("Syntax")
PROTOTYPE ("##typedef struct { double #%x1NDC##, #%x2NDC##, #%y1NDC##, #%y2NDC##; } Graphics_Viewport;")
PROTOTYPE ("##void Graphics_resetViewport (Graphics #%g##, Graphics_Viewport #%viewport##);")
ENTRY ("Purpose")
NORMAL ("to set the part of the device that will receive output "
	"back to where it was before calling @Graphics_insetViewport.")
ENTRY ("Usage")
NORMAL ("Graphics_resetViewport will only be called some time after @Graphics_insetViewport.")
MAN_END

MAN_BEGIN ("Graphics_setViewport", "ppgb", 19961127)
INTRO ("A routine to change one of the @@Graphics coordinate systems@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setViewport (Graphics #%g##, double #%x1NDC##, double #%x2NDC##, double #%y1NDC##, double #%y2NDC##);")
ENTRY ("Purpose")
NORMAL ("to set the part of the device that will receive output.")
MAN_END

MAN_BEGIN ("Graphics_setWindow", "ppgb", 19961127)
INTRO ("A routine to change one of the @@Graphics coordinate systems@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setWindow (Graphics #%g##, double #%x1##, double #%x2##, double #%y1##, double #%y2##);")
ENTRY ("Purpose")
NORMAL ("to set the world coordinates for drawing.")
ENTRY ("Behaviour")
NORMAL ("Graphics_setWindow associates the viewport with the rectangle [%x1, %x2] \\xx [%y1, %y2] in world coordinates.")
MAN_END

MAN_BEGIN ("Graphics_setWsViewport", "ppgb", 19961127)
INTRO ("One of the @@Graphics workstation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setWsViewport (Graphics #%g##, short #%x1DC##, short #%x2DC##, short #%y1DC##, short #%y2DC##);")
ENTRY ("Purpose")
NORMAL ("to change one of the @@Graphics coordinate systems@.")
MAN_END

MAN_BEGIN ("Graphics_setWsWindow", "ppgb", 19961127)
INTRO ("One of the @@Graphics workstation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setWsWindow (Graphics #%g##, double #%x1NDC##, double #%x2NDC##, double #%y1NDC##, double #%y2NDC##);")
ENTRY ("Purpose")
NORMAL ("to change one of the @@Graphics coordinate systems@.")
MAN_END

MAN_BEGIN ("Graphics_startRecording", "ppgb", 19970331)
INTRO ("One of the @@Graphics recording routines@.")
ENTRY ("Purpose")
NORMAL ("to start or continue the recording of subsequent graphics output.")
ENTRY ("Syntax")
PROTOTYPE ("##int Graphics_startRecording (Graphics #%me##);")
ENTRY ("Return value")
NORMAL ("1 if recording was on before this call, 0 if it was off.")
ENTRY ("Postcondition")
NORMAL ("Recording is on. All subsequent graphics output will be remembered by %me, "
	"until the next @Graphics_clearWs, or @forget.")
MAN_END

MAN_BEGIN ("Graphics_stopRecording", "ppgb", 19970331)
INTRO ("One of the @@Graphics recording routines@.")
ENTRY ("Purpose")
NORMAL ("to stop or pause the recording of subsequent graphics output.")
ENTRY ("Syntax")
PROTOTYPE ("##int Graphics_stopRecording (Graphics #%me##);")
ENTRY ("Return value")
NORMAL ("1 if recording was on before this call, 0 if it was off.")
ENTRY ("Postcondition")
NORMAL ("Recording is off. The recorded graphics output is still remembered, "
	"but subsequent graphics output will not be remembered.")
MAN_END

MAN_BEGIN ("Graphics_textWidth", "ppgb", 19971012)
INTRO ("A routine in the @@Graphics library@.")
ENTRY ("Purpose")
NORMAL ("to measure the width of the text in the current font, size, and style, i.e., "
	"the width of the text as it would be drawn with @Graphics_text.")
ENTRY ("Syntax")
PROTOTYPE ("##double Graphics_textWidth (Graphics #%me##, const char *#%text##);")
ENTRY ("Return value")
NORMAL ("The width of the text, in world coordinates.")
ENTRY ("Usage")
NORMAL ("You would use this routine if you want the position of other graphics output "
	"to depend on the width of your text.")
NORMAL ("However, note that the returned text width is valid only for "
	"the current Graphics object, and may not necessarily be correct for the Graphics object "
	"on which you play recorded graphics output, because character widths may vary with your device. "
	"For a solution, see @Graphics_textWidth_ps.")
MAN_END

MAN_BEGIN ("Graphics_textWidth_ps", "ppgb", 19971012)
INTRO ("A routine in the @@Graphics library@.")
ENTRY ("Purpose")
NORMAL ("to measure the width of the text in the current font, size, and style, i.e., "
	"the width of the text as it would be drawn with @Graphics_text to a PostScript device.")
ENTRY ("Syntax")
PROTOTYPE ("##double Graphics_textWidth_ps (Graphics #%me##, const char *#%text##);")
ENTRY ("Return value")
NORMAL ("The width of the text, in world coordinates.")
ENTRY ("Usage")
NORMAL ("You would use this routine if you want the position of other graphics output "
	"to depend on the width of your text.")
NORMAL ("#Graphicxs_textWidth_ps may offer a solution to the problem noted at @Graphics_textWidth. "
	"The width of a bitmapped screen character is generally different from the width of the "
	"corresponding PostScript character. For example, the following code tries to copy text underlining "
	"from a screen to a printer:")
CODE ("Graphics screen = @Graphics_create_xmdrawingarea (drawingArea);")
CODE ("@Graphics_startRecording (screen);")
CODE ("@Graphics_setTextAlignment (screen, Graphics_LEFT, Graphics_BOTTOM);")
CODE ("@Graphics_text (screen, 0, 0, \"Hallo\");")
CODE ("double textWidth = @Graphics_textWidth (screen, \"Hallo\");   // Not recorded.")
CODE ("@Graphics_line (screen, 0, 0, textWidth, 0);")
CODE ("Graphics postScript = @Graphics_create_postscriptjob (\"hallo.ps\", 600, GraphicsPostscript_FINE);")
CODE ("@Graphics_play (screen, postScript);")
CODE ("@forget (postScript);")
CODE ("system (\"lp -c hallo.ps; rm hallo.ps;\");")
NORMAL ("In this example, the line on your paper will not necessarily be as long as your text. "
	"To prevent this situation, you could repeat all graphics output on your printer, "
	"including the #Graphics_textWidth call (as is done in the HyperPage system). "
	"If that is impracticable (as in the Praat picture window), you can allow for some extra space around your text:")
CODE ("Graphics screen = Graphics_create_xmdrawingarea (drawingArea);")
CODE ("Graphics_startRecording (screen);")
CODE ("double textWidth = Graphics_textWidth (screen, \"Hallo\");")
CODE ("Graphics_setTextAlignment (screen, #Graphics_CENTRE, Graphics_BOTTOM);")
CODE ("##textWidth += @Graphics_dxMMtoWC (screen, 3.0);")
CODE ("Graphics_text (screen, ##textWidth / 2#, 0, \"Hallo\");")
CODE ("Graphics_line (screen, 0, 0, textWidth, 0);")
CODE ("Graphics postScript = Graphics_create_postscriptjob (\"hallo.ps\", 600, GraphicsPostscript_FINE);")
CODE ("Graphics_play (screen, postScript);")
CODE ("forget (postScript);")
CODE ("system (\"lp -c hallo.ps; rm hallo.ps;\");")
NORMAL ("In this example, the line will extend 1.5 millimetres beyond both edges of the text on your screen. "
	"On your paper, this margin may be different, but still positive (I hope).")
NORMAL ("A better solution is to use #Graphics_textWidth_ps instead of #Graphics_textWidth:")
CODE ("double textWidth = #Graphics_textWidth_ps (screen, \"Hallo\");")
NORMAL ("Now it will always look exactly right on your printer, though not necessarily on your screen.")
MAN_END

MAN_BEGIN ("Graphics_undoGroup", "ppgb", 19970331)
INTRO ("One of the @@Graphics recording routines@.")
ENTRY ("Purpose")
NORMAL ("to undo the workings of the last group of graphics output.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_undoGroup (Graphics #%me##);")
ENTRY ("Behaviour")
NORMAL ("All graphics output after the last mark that was set with @Graphics_markGroup, is forgotten.")
ENTRY ("Usage")
NORMAL ("This routine only makes sense when recording. You should probably call @Graphics_updateWs (%me) "
	"immediately after this routine. Your drawing method can then call @Graphics_play (%me, %me).")
MAN_END

MAN_BEGIN ("Graphics_updateWs", "ppgb", 19961127)
INTRO ("One of the @@Graphics workstation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_updateWs (Graphics #%g##);")
ENTRY ("Purpose")
NORMAL ("to redraw the device.")
ENTRY ("Usage")
NORMAL ("After @Graphics_setWsViewport, or if a change in the displayed data must be reflected on the screen.")
ENTRY ("Behaviour")
NORMAL ("Graphics_flushWs forces an expose (Xwindows) or update (Macintosh) event.")
MAN_END

MAN_BEGIN ("Graphics_WCtoDC", "ppgb", 19961127)
INTRO ("One of the @@Graphics workstation routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_WCtoDC (Graphics #%g##, double #%xWC##, double #%yWC##, short *#%xDC##, short *#%yDC##);")
ENTRY ("Purpose")
NORMAL ("to compute device coordinates from world coordinates, the reverse from @Graphics_DCtoWC.")
ENTRY ("Usage")
NORMAL ("Rare. Hacking with screen pixels.")
MAN_END

MAN_BEGIN ("Graphics_writeRecordings", "ppgb", 19970331)
INTRO ("One of the @@Graphics recording routines@.")
ENTRY ("Purpose")
NORMAL ("to write recorded graphics output to a file.")
ENTRY ("Syntax")
PROTOTYPE ("##int Graphics_writeRecordings (Graphics #%me##, FILE *#%f##);")
ENTRY ("Return value")
NORMAL ("1 if OK, 0 in case of failure.")
ENTRY ("Behaviour")
NORMAL ("Writes the recordings to the stream %f in a machine-independent binary format.")
MAN_END

MAN_BEGIN ("Graphics output attributes", "ppgb", 20000526)
INTRO ("The attributes that determine the behaviour of the @@Graphics output routines@.")
ENTRY ("Routines to change the output attributes")
PROTOTYPE ("##void @Graphics_setColour (Graphics #%g##, int #%colour##);")
PROTOTYPE ("##void @Graphics_setFont (Graphics #%g##, int #%font##);")
PROTOTYPE ("##void @Graphics_setFontSize (Graphics #%g##, int #%height##);")
PROTOTYPE ("##void @Graphics_setFontStyle (Graphics #%g##, int #%style##);")
PROTOTYPE ("##void @Graphics_setGrey (Graphics #%g##, double #%grey##);")
PROTOTYPE ("##void @Graphics_setLineType (Graphics #%g##, int #%lineType##);")
PROTOTYPE ("##void @Graphics_setLineWidth (Graphics #%g##, double #%lineWidth##);")
PROTOTYPE ("##void @Graphics_setTextAlignment (Graphics #%g##, int #%horizontal##, int #%vertical##);")
PROTOTYPE ("##void @Graphics_setTextRotation (Graphics #%g##, double #%angle##);")
PROTOTYPE ("##void @Graphics_setWrapWidth (Graphics #%g##, double #%wrapWidth##);")
PROTOTYPE ("##void @Graphics_xorOn (Graphics #%g##, int #%colour##);")
PROTOTYPE ("##void @Graphics_xorOff (Graphics #%g##);")
ENTRY ("Routines to inquire the output attributes")
NORMAL ("The functions Graphics_inqXXXXXX return the values "
	"that will be used in the next graphical output to the Graphics specified "
	"(if there will be no intervening Graphics_setXXXXXX). "
	"These functions return the values set with the latest Graphics_setXXXXXX "
	"or, if there has been no Graphics_setXXXXXX, "
	"the default values set by one of the @@Graphics creation routines@.")
PROTOTYPE ("##int Graphics_inqFont (Graphics #%g##);")
PROTOTYPE ("##int Graphics_inqFontSize (Graphics# %g##);")
PROTOTYPE ("##int Graphics_inqFontStyle (Graphics #%g##);")
PROTOTYPE ("##int Graphics_inqLineType (Graphics #%g##);")
PROTOTYPE ("##float Graphics_inqLineWidth (Graphics #%g##);")
MAN_END

MAN_BEGIN ("Graphics output routines", "ppgb", 19970911)
INTRO ("The output routines of the @@Graphics library@.")
NORMAL ("They use the current output attributes (line type, colour, grey value, "
	"font, font size, font style, text rotation, text alignment "
	"of the specified Graphics.")
ENTRY ("Lines")
LIST_ITEM ("\\bu @Graphics_line")
LIST_ITEM ("\\bu @Graphics_polyline: draw a number of connected lines")
LIST_ITEM ("\\bu @Graphics_function")
LIST_ITEM ("\\bu @Graphics_rectangle")
LIST_ITEM ("\\bu @Graphics_rectangle_mm")
LIST_ITEM ("\\bu @Graphics_circle")
LIST_ITEM ("\\bu @Graphics_circle_mm")
LIST_ITEM ("\\bu @Graphics_arc")
LIST_ITEM ("\\bu @Graphics_ellipse")
ENTRY ("Text")
LIST_ITEM ("\\bu @Graphics_text")
LIST_ITEM ("\\bu @Graphics_printf: with variable arguments")
ENTRY ("Filling")
LIST_ITEM ("\\bu @Graphics_fillArea: fill a polygon with a colour or grey value")
LIST_ITEM ("\\bu @Graphics_fillRectangle")
LIST_ITEM ("\\bu @Graphics_fillRectangle_mm")
LIST_ITEM ("\\bu @Graphics_fillCircle")
LIST_ITEM ("\\bu @Graphics_fillCircle_mm")
LIST_ITEM ("\\bu @Graphics_fillEllipse")
ENTRY ("Matrices")
LIST_ITEM ("\\bu @Graphics_cellArray: fill blocks with #float grey values")
LIST_ITEM ("\\bu @Graphics_image: fill blocks with 8-bit quantized grey values")
ENTRY ("Arrows")
LIST_ITEM ("\\bu @Graphics_arrow")
LIST_ITEM ("\\bu @Graphics_arcArrow")
ENTRY ("3-D")
LIST_ITEM ("\\bu @Graphics_altitude")
LIST_ITEM ("\\bu @Graphics_grey")
LIST_ITEM ("\\bu @Graphics_surface")
ENTRY ("For screens")
LIST_ITEM ("\\bu @Graphics_highlight and @Graphics_unhighlight")
MAN_END

MAN_BEGIN ("Graphics_altitude", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_altitude (Graphics #%g##, float **#%z##, "
	"long #%ix1##, long #%ix2##, double #%x1##, double #%x2##, "
	"long #%iy1##, long #%iy2##, double #%y1##, double #%y2##, "
	"int #%numberOfBorders##, float #%borders## []);")
ENTRY ("Purpose")
NORMAL ("to draw altitude contours of a two-dimensional piecewise linear interpolation "
	"of the function %z (%y, %x), sampled as %z [%iy1..%iy2] [%ix1..%ix2], "
	"in a rectangle with bottom left at (%x1, %y1) and top right at (%x2, %y2).")
ENTRY ("Arguments")
TAG ("%z [%iy] [%ix]")
DEFINITION ("the sampled function to be drawn.")
TAG ("%ix1")
DEFINITION ("the second index into %z of the points on the left border.")
TAG ("%ix2")
DEFINITION ("the second index into %z of the points on the right border.")
TAG ("%x1")
DEFINITION ("the %x coordinate of the points on the left border.")
TAG ("%x2")
DEFINITION ("the %x coordinate of the points on the right border.")
TAG ("%iy1")
DEFINITION ("the first index into %z of the points on the bottom border.")
TAG ("%iy2")
DEFINITION ("the first index into %z of the points on the top border.")
TAG ("%y1")
DEFINITION ("the %y coordinate of the points on the bottom border.")
TAG ("%y2")
DEFINITION ("the %y coordinate of the points on the top border.")
TAG ("%numberOfBorders")
DEFINITION ("the number of different heights at which to draw contours.")
TAG ("%borders [1..%numberOfBorders]")
DEFINITION ("the heights at which to draw contours.")
ENTRY ("Behaviour")
NORMAL ("If %ix2 \\<_ %ix1 or %iy2 \\<_ %iy1, nothing is drawn; "
	"otherwise, contours of equal height are drawn "
	"between points that are linearly interpolated between the sample points. "
	"The x and y coordinates of the sample points are equally spaced "
	"between %x [%ix1] = %x1 and %x [%ix2] = %x2 and "
	"between %y [%iy1] = %y1 and %y [%iy2] = %y2: "
	"the space between adjacent values of %x and %y is "
	"%dx = (%x2 \\-- %x1) / (%ix2 \\-- %ix1) and %dy = (%y2 \\-- %y1) / (%iy2 \\-- %iy1), "
	"and %x [%i] = %x1 + (%i \\-- %ix1) * %dx and %y [%i] = %y1 + (%i \\-- %iy1) * %dy.")
MAN_END

MAN_BEGIN ("Graphics_arc", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_arc (Graphics #%g##, double #%x##, double #%y##, double #%r##, double #%fromAngle##, double #%toAngle##);")
ENTRY ("Purpose")
NORMAL ("to draw a circular arc counterclockwise from %fromAngle to %toAngle.")
ENTRY ("Arguments")
TAG ("%x, %y")
DEFINITION ("the world coordinates of the arc's centre of curvature.")
TAG ("%r")
DEFINITION ("the radius of the arc, expressed in world coordinates along the x axis!")
TAG ("%fromAngle, %toAngle")
DEFINITION ("measured in degrees, counted counterclockwise from the horizontal half-line "
	"drawn from (x, y) to the right.")
MAN_END

MAN_BEGIN ("Graphics_arcArrow", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_arcArrow (Graphics #%g##, double #%x##, double #%y##, double #%r##, "
	"double #%fromAngle##, double #%toAngle##, int #%arrowAtStart##, int #%arrowAtEnd##);")
ENTRY ("Purpose")
NORMAL ("the same as @Graphics_arc, but with arrow heads at the start and/or end of the arc.")
ENTRY ("Arguments")
TAG ("%x, %y")
DEFINITION ("the world coordinates of the arc's centre of curvature.")
TAG ("%r")
DEFINITION ("the radius of the arc, expressed in world coordinates along the x axis!")
TAG ("%fromAngle, %toAngle")
DEFINITION ("measured in degrees, counted counterclockwise from the horizontal half-line "
	"drawn from (x, y) to the right.")
TAG ("%arrowAtStart, %arrowAtEnd")
DEFINITION ("if not 0, an arrowhead is drawn.")
MAN_END

MAN_BEGIN ("Graphics_arrow", "ppgb", 19971020)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_arrow (Graphics #%g##, double #%x1##, double #%y1##, double #%x2##, double #%y2##);")
ENTRY ("Purpose")
NORMAL ("to draw a line segment from (%x1, %y1) to (%x2, %y2) with an arrowhead at the end.")
MAN_END

MAN_BEGIN ("Graphics_cellArray", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_cellArray (Graphics #%g##, float **#%z##, "
	"long #%ix1##, long #%ix2##, double #%x1##, double #%x2##, "
	"long #%iy1##, long #%iy2##, double #%y1##, double #%y2##, "
	"double #%minimu#m#, double #%maximum##);")
ENTRY ("Purpose")
NORMAL ("to draw the function %z (%x, %y), sampled as %z [%iy1..%iy2] [%ix1..%ix2], "
	"as equal-sized rectangles (blocks) filled with grey values "
	"1.0 - (%z - %minimum) / (%maximum - %minimum), "
	"in a rectangle with bottom left at (%x1, %y1) and top right at (%x2, %y2).")
ENTRY ("Arguments")
TAG ("%z [%iy] [%ix]")
DEFINITION ("the sampled function to be drawn.")
TAG ("%ix1")
DEFINITION ("the second index into %z of the points at the left border.")
TAG ("%ix2")
DEFINITION ("the second index into %z of the points at the right border.")
TAG ("%x1")
DEFINITION ("the %x world coordinate left from the points at the left border.")
TAG ("%x2")
DEFINITION ("the %x world coordinate right from the points at the right border.")
TAG ("%iy1")
DEFINITION ("the first index into %z of the points at the bottom border.")
TAG ("%iy2")
DEFINITION ("the first index into %z of the points at the top border.")
TAG ("%y1")
DEFINITION ("the %y world coordinate below the bottom points.")
TAG ("%y2")
DEFINITION ("the %y world coordinate above the top points.")
TAG ("%minimum")
DEFINITION ("the value of %z below which the blocks are filled with white.")
TAG ("%maximum")
DEFINITION ("the value of %z above which the blocks are filled with black.")
ENTRY ("Behaviour")
NORMAL ("If %ix2 < %ix1 or %iy2 < %iy1 or %minimum = %maximum, nothing is drawn. "
	"The %x and %y coordinates of the sample points, "
	"which are thought to be in the centres of the blocks, are equally spaced "
	"between %x1 and %x2 and between %y1 and %y2: "
	"the space between adjacent values of %x and %y is "
	"%dx = (%x2 - %x1) / (%ix2 - %ix1 + 1) and "
	"%dy = (%y2 - %y1) / (%iy2 - %iy1 + 1), and "
	"%x [%i] = %x1 + (%i - %ix1 + 0.5) * %dx and "
	"%y [%i] = %y1 + (%i - %iy1 + 0.5) * %dy.")
ENTRY ("Usage")
NORMAL ("If you can quantize the values into 8 bits, you can use @Graphics_image instead "
	"and save some memory.")
MAN_END

MAN_BEGIN ("Graphics_circle", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_circle (Graphics #%g##, double #%x##, double #%y##, double #%r##);")
ENTRY ("Purpose")
NORMAL ("to draw a circle in the current line type.")
ENTRY ("Arguments")
TAG ("%x, %y")
DEFINITION ("the world coordinates of the centre of the circle.")
TAG ("%r")
DEFINITION ("the radius of the circle, expressed in world coordinates along the x axis!")
MAN_END

MAN_BEGIN ("Graphics_circle_mm", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_circle_mm (Graphics #%g##, double #%x##, double #%y##, double #%d##);")
ENTRY ("Purpose")
NORMAL ("to draw a circle in the current line type.")
ENTRY ("Arguments")
TAG ("%x, %y")
DEFINITION ("the world coordinates of the centre of the circle.")
TAG ("%d")
DEFINITION ("the diameter of the circle, expressed in millimetres.")
MAN_END

MAN_BEGIN ("Graphics_ellipse", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_ellipse (Graphics #%g##, double #%x1##, double #%x2##, double #%y1##, double #%y2##);")
MAN_END

MAN_BEGIN ("Graphics_fillArea", "ppgb", 19961210)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_fillArea (Graphics #%g##, long #%numberOfPoints##, float *#%x##, float *#%y##);")
ENTRY ("Purpose")
NORMAL ("to fill a polygon with the current colour or the current grey value, "
	"whichever you have set most recently.")
ENTRY ("Arguments")
TAG ("%numberOfPoints")
DEFINITION ("the number of points to be connected.")
TAG ("%x [0 .. %numberOfPoints-1], %y [0 .. %numberOfPoints-1]")
DEFINITION ("the world coordinates of the points to be connected.")
ENTRY ("Behaviour")
NORMAL ("The first point is (%x [0], %y [0]); "
	"from there, an imaginary line will be drawn to (%x [1], %y [1]), "
	"and so forth until the point (%x [%numberOfPoints-1], %y [%numberOfPoints-1]), "
	"from where an imaginary line will be drawn back to (%x [0], %y [0]), "
	"which closes the contour. "
	"The space enclosed by the resulting path will then be filled. "
	"If %numberOfPoints is less than 3, nothing will be drawn.")
MAN_END

MAN_BEGIN ("Graphics_fillCircle", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_fillCircle (Graphics #%g##, double #%x##, double #%y##, double #%r##);")
ENTRY ("Purpose")
NORMAL ("to fill a circle with the current colour or grey.")
ENTRY ("Arguments")
TAG ("%x, %y")
DEFINITION ("the world coordinates of the centre of the circle.")
TAG ("%r")
DEFINITION ("the radius of the circle, expressed in world coordinates along the x axis!")
MAN_END

MAN_BEGIN ("Graphics_fillCircle_mm", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_fillCircle_mm (Graphics #%g##, double #%x##, double #%y##, double #%d##);")
ENTRY ("Purpose")
NORMAL ("to fill a circle with the current colour or grey.")
ENTRY ("Arguments")
TAG ("%x, %y")
DEFINITION ("the world coordinates of the centre of the circle.")
TAG ("%d")
DEFINITION ("the diameter of the circle, expressed in millimetres.")
MAN_END

MAN_BEGIN ("Graphics_fillEllipse", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_fillEllipse (Graphics #%g##, double #%x1##, double #%x2##, double #%y1##, double #%y2##);")
MAN_END

MAN_BEGIN ("Graphics_gray", "ppgb", 19970911)
INTRO ("Another name for @Graphics_grey.")
MAN_END

MAN_BEGIN ("Graphics_grey", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_grey (Graphics #%g##, float **#%z##, "
	"long #%ix1##, long #%ix2##, double #%x1##, double #%x2##, "
	"long #%iy1##, long #%iy2##, double #%y1##, double #%y2##, "
	"int #%numberOfBorders##, float #%borders## []);")
ENTRY ("Purpose")
NORMAL ("to fill the space between altitude contours of a two-dimensional piecewise linear interpolation "
	"of the function %z (%y, %x), sampled as %z [%iy1..%iy2] [%ix1..%ix2], "
	"in a rectangle with bottom left at (%x1, %y1) and top right at (%x2, %y2).")
ENTRY ("Arguments")
TAG ("%z [%iy] [%ix]")
DEFINITION ("the sampled function to be drawn.")
TAG ("%ix1")
DEFINITION ("the second index into %z of the points on the left border.")
TAG ("%ix2")
DEFINITION ("the second index into %z of the points on the right border.")
TAG ("%x1")
DEFINITION ("the %x coordinate of the points on the left border.")
TAG ("%x2")
DEFINITION ("the %x coordinate of the points on the right border.")
TAG ("%iy1")
DEFINITION ("the first index into %z of the points on the bottom border.")
TAG ("%iy2")
DEFINITION ("the first index into %z of the points on the top border.")
TAG ("%y1")
DEFINITION ("the %y coordinate of the points on the bottom border.")
TAG ("%y2")
DEFINITION ("the %y coordinate of the points on the top border.")
TAG ("%numberOfBorders")
DEFINITION ("the number of different heights at which to draw contours.")
TAG ("%borders [1..%numberOfBorders]")
DEFINITION ("the heights at which to draw contours.")
ENTRY ("Behaviour")
NORMAL ("the space between the contours is filled with shades of grey. "
	"Spaces that contain values of %z that are less than %borders [1], are painted white; "
	"spaces with values of %z that are greater than %borders [%numberOfBorders] are painted black.")
ENTRY ("Usage")
NORMAL ("If you want black lines along the filled contours, follow the call to #Graphics_grey "
	"with a call to @Graphics_altitude with the same arguments.")
MAN_END

MAN_BEGIN ("Graphics_highlight", "ppgb", 19970911)
INTRO ("Two screen-oriented @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_highlight (Graphics #%g##, double #%x1##, double #%x2##, double #%y1##, double #%y2##);")
PROTOTYPE ("##void Graphics_unhighlight (Graphics #%g##, double #%x1##, double #%x2##, double #%y1##, double #%y2##);")
ENTRY ("Purpose")
NORMAL ("to highlight or unhighlight a rectangular area.")
ENTRY ("Usage")
NORMAL ("Use this for selecting things while dragging the mouse.")
ENTRY ("Macintosh behaviour")
NORMAL ("first call: all white pixels will change to the current highlight colour, "
	"which is the colour used for highlighting text (e.g., in text editors "
	"and in the names of icons in the finder); "
	"the user can set this colour with the 'Colour' Control Panel.")
NORMAL ("second call: all changed pixels will return to white.")
NORMAL ("There is no problem when redrawing in this mode in response to an update event, "
	"because the Macintosh is guaranteed to clip to the area to be redrawn.")
ENTRY ("Xwindows behaviour")
NORMAL ("a yellowish shade is mixed with every white, grey, or black pixel. "
	"If there are not enough colours available, "
	"a blue dashed rectangle will be drawn around the area in xor mode. "
	"There will be problems when redrawing in this mode in response to an expose event, "
	"because there is no telling what rectangles Xwindows will schedule for redrawing.")
ENTRY ("PostScript behaviour")
NORMAL ("no action will be taken.")
MAN_END

MAN_BEGIN ("Graphics_image", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_image (Graphics #%g##, unsigned char **#%z##, "
	"long #%ix1##, long #%ix2##, double #%x1##, double #%x2##, "
	"long #%iy1##, long #%iy2##, double #%y1##, double #%y2##, "
	"unsigned char #%minimum##, unsigned char #%maximum##);")
ENTRY ("Purpose")
NORMAL ("to draw the function %z (%x, %y), sampled and quantized as "
	"%z [%iy1..%iy2] [%ix1..%ix2] = 0..255, "
	"as equal-sized rectangles (blocks) filled with grey values "
	"1.0 - (%z - %minimum) / (%maximum - %minimum), "
	"in a rectangle with bottom left at (%x1, %y1) and top right at (%x2, %y2).")
ENTRY ("Arguments")
TAG ("%z [%iy] [%ix]")
DEFINITION ("the sampled function to be drawn.")
TAG ("%ix1")
DEFINITION ("the second index into %z of the points at the left border.")
TAG ("%ix2")
DEFINITION ("the second index into %z of the points at the right border.")
TAG ("%x1")
DEFINITION ("the %x world coordinate left from the points at the left border.")
TAG ("%x2")
DEFINITION ("the %x world coordinate right from the points at the right border.")
TAG ("%iy1")
DEFINITION ("the first index into %z of the points at the bottom border.")
TAG ("%iy2")
DEFINITION ("the first index into %z of the points at the top border.")
TAG ("%y1")
DEFINITION ("the %y world coordinate below the bottom points.")
TAG ("%y2")
DEFINITION ("the %y world coordinate above the top points.")
TAG ("%minimum")
DEFINITION ("the value of %z below which the blocks are filled with white.")
TAG ("%maximum")
DEFINITION ("the value of %z above which the blocks are filled with black.")
ENTRY ("Behaviour")
NORMAL ("If %ix2 < %ix1 or %iy2 < %iy1 or %minimum = %maximum, nothing is drawn. "
	"The %x and %y coordinates of the sample points, "
	"which are thought to be in the centres of the blocks, are equally spaced "
	"between %x1 and %x2 and between %y1 and %y2: "
	"the space between adjacent values of %x and %y is "
	"%dx = (%x2 - %x1) / (%ix2 - %ix1 + 1) and "
	"%dy = (%y2 - %y1) / (%iy2 - %iy1 + 1), and "
	"%x [%i] = %x1 + (%i - %ix1 + 0.5) * %dx and "
	"%y [%i] = %y1 + (%i - %iy1 + 0.5) * %dy.")
ENTRY ("Usage")
NORMAL ("If you cannot quantize, use @Graphics_cellArray instead.")
MAN_END

MAN_BEGIN ("Graphics_line", "ppgb", 19970527)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void #Graphics_line (Graphics #%g##, double #%x1##, double #%y1##, double #%x2##, double #%y2##);")
ENTRY ("Purpose")
NORMAL ("to draw a line in the current line type in the current colour.")
ENTRY ("Arguments")
TAG ("%x1, %y1")
DEFINITION ("the world coordinates of the start of the line.")
TAG ("%x2, %y2")
DEFINITION ("the world coordinates of the end of the line.")
MAN_END

MAN_BEGIN ("Graphics_fillRectangle", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_fillRectangle (Graphics #%g##, double #%x1##, double #%x2##, double #%y1##, double #%y2##);")
MAN_END

MAN_BEGIN ("Graphics_fillRectangle_mm", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_fillRectangle_mm (Graphics #%g##, double #%x##, double #%y##, double #%horizontalSide_mm##, double #%verticalSide_mm##);")
MAN_END

MAN_BEGIN ("Graphics_function", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_function (Graphics #%g##, float #%y## [], long #%ix1##, long #%ix2##, double #%x1##, double #%x2##);")
ENTRY ("Purpose")
NORMAL ("to draw a piecewise linear interpolation of the function y (x), which is sampled as y [ix1..ix2], "
	"between the left border (x1, y [ix1]) and the right border (x2, y [ix2]).")
ENTRY ("Arguments")
TAG ("%y [%ix]")
DEFINITION ("the sampled function to be drawn.")
TAG ("%ix1")
DEFINITION ("the index into y of the point on the left border.")
TAG ("%ix2")
DEFINITION ("the index into y of the point on the right border.")
TAG ("%x1")
DEFINITION ("the x coordinate of the point on the left border.")
TAG ("%x2")
DEFINITION ("the x coordinate of the point on the right border.")
ENTRY ("Behaviour")
NORMAL ("If %ix2 \\<_ %ix1, nothing is drawn; "
	"otherwise, %ix2 \\-- %ix1 lines are drawn between adjacent sample points, "
	"from (%x [%ix1], %y [%ix1]) to (%x [%ix1 + 1], %y [%ix1 + 1]), ..., "
	"and so forth, ending at (%x [%ix2], %y [%ix2]). "
	"The x coordinates of the sample points are equally spaced "
	"between %x [%ix1] = %x1 and %x [%ix2] = %x2: "
	"the space between adjacent values of %x is %dx = (%x2 \\-- %x1) / (%ix2 \\-- %ix1), "
	"and %x [%i] = %x1 + (%i \\-- %ix1) * %dx.")
NORMAL ("This function has been optimized so as to take advantage of the finite resolution "
	"of the device. E.g., if you have 100000 points but there are only 600 pixels "
	"between x1 and x2, only 1200 line segments will be drawn. "
	"If recording is on, however, all 100000 points will be remembered.")
MAN_END

MAN_BEGIN ("Graphics_polyline", "ppgb", 19990628)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_polyline (Graphics #%g##, long #%numberOfPoints##, float *#%x##, float *#%y##);")
ENTRY ("Purpose")
NORMAL ("to draw a polygon in the current line type in the current colour.")
ENTRY ("Arguments")
TAG ("%numberOfPoints")
DEFINITION ("the number of points to be connected.")
TAG ("%x [0 ... %numberOfPoints\\--1], %y [0 ... %numberOfPoints\\--1]")
DEFINITION ("the world coordinates of the points to be connected.")
ENTRY ("Behaviour")
NORMAL ("The first point is (%x [0], %y [0]); from there, a line will be drawn "
	"to (%x [1], %y [1]), and so forth, until the %numberOfPoints %minus %first line "
	"has been drawn, ending at (%x [%numberOfPoints-1], %y [%numberOfPoints-1]). "
	"The polygon will be closed only if the last point equals the first. "
	"If %numberOfPoints is less than 2, nothing will be drawn. ")
MAN_END

MAN_BEGIN ("Graphics_printf", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@, "
	"the variable-argument version of @Graphics_text.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_printf (Graphics #%g##, double# %xWC##, double #%yWC##, char *#%format##, #%...##);")
MAN_END

MAN_BEGIN ("Graphics_rectangle", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_rectangle (Graphics #%g##, double# %x1##, double #%x2##, double #%y1##, double #%y2##);")
MAN_END

MAN_BEGIN ("Graphics_rectangle_mm", "ppgb", 19970911)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_rectangle_mm (Graphics #%g##, double #%x##, double #%y##, double #%horizontalSide_mm##, double #%verticalSide_mm##);")
MAN_END

MAN_BEGIN ("Graphics_setColor", "ppgb", 19970911)
INTRO ("Another name for @Graphics_setColour.")
MAN_END

MAN_BEGIN ("Graphics_setColour", "ppgb", 19970911)
INTRO ("One of the routines that change the @@Graphics output attributes@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setColour (Graphics #%g##, int #%colour##);")
ENTRY ("Behaviour")
NORMAL ("Sets the colour of the lines, text, and filling that will be drawn later, to one of the following values:")
CODE ("        \\# define Graphics_BLACK    0")
CODE ("        \\# define Graphics_WHITE    1")
CODE ("        \\# define Graphics_RED      2")
CODE ("        \\# define Graphics_GREEN    3")
CODE ("        \\# define Graphics_BLUE     4")
CODE ("        \\# define Graphics_CYAN     5")
CODE ("        \\# define Graphics_MAGENTA  6")
CODE ("        \\# define Graphics_YELLOW	   7")
MAN_END

MAN_BEGIN ("Graphics_setFont", "ppgb", 19970911)
INTRO ("One of the routines that change the @@Graphics output attributes@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setFont (Graphics #%g##, int #%font##);")
ENTRY ("Behaviour")
NORMAL ("Sets the font of text that will be drawn later, to one of the following values:")
CODE ("        \\# define Graphics_HELVETICA  0")
CODE ("        \\# define Graphics_TIMES  1")
CODE ("        \\# define Graphics_COURIER  2")
CODE ("        \\# define Graphics_NEWCENTURYSCHOOLBOOK  3")
CODE ("        \\# define Graphics_PALATINO  4")
NORMAL ("Note that the Symbol and Phonetic alphabets are not considered fonts. "
	"Instead, these are character sets. For instance, the symbol \\ng, used in "
	"@Graphics_text will always be drawn as an %eng symbol, "
	"but will have serifs unless the current font is $Graphics_HELVETICA; "
	"and it will be taken from a mono-spaced IPA \"font\" if the current font "
	"is $Graphics_COURIER.")
MAN_END

MAN_BEGIN ("Graphics_setFontSize", "ppgb", 19960716)
INTRO ("One of the routines that change the @@Graphics output attributes@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setFontSize (Graphics #%g##, int #%height##);")
ENTRY ("Behaviour")
NORMAL ("Sets the size (height) of the characters that will be drawn, in points.")
MAN_END

MAN_BEGIN ("Graphics_setFontStyle", "ppgb", 19970911)
INTRO ("One of the routines that change the @@Graphics output attributes@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setFontStyle (Graphics #%g##, int #%style##);")
ENTRY ("Behaviour")
NORMAL ("Sets the style of the text that will be drawn later, to one of the following values:")
CODE ("        \\# define Graphics_NORMAL  0")
CODE ("        \\# define Graphics_BOLD  1")
CODE ("        \\# define Graphics_ITALIC  2")
ENTRY ("Usage")
NORMAL ("You can mix normal, italic, and bold styles. See @@Text styles@.")
MAN_END

MAN_BEGIN ("Graphics_setGray", "ppgb", 19970911)
INTRO ("Another name for @Graphics_setGrey.")
MAN_END

MAN_BEGIN ("Graphics_setGrey", "ppgb", 19970911)
INTRO ("One of the routines that change the @@Graphics output attributes@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setGrey (Graphics #%g##, double #%grey##);")
ENTRY ("Behaviour")
NORMAL ("Sets the colour of the lines, text, and filling that will be drawn later, "
	"to a %grey value between black (0.0) and white (1.0).")
MAN_END

MAN_BEGIN ("Graphics_setLineType", "ppgb", 19970911)
INTRO ("One of the routines that change the @@Graphics output attributes@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setLineType (Graphics #%g##, int #%lineType##);")
ENTRY ("Behaviour")
NORMAL ("Sets the type of the lines that will be drawn to one of the following values:")
CODE ("        \\# define Graphics_DRAWN  0")
CODE ("        \\# define Graphics_DOTTED	  1")
MAN_END

MAN_BEGIN ("Graphics_setLineWidth", "ppgb", 19970911)
INTRO ("One of the routines that change the @@Graphics output attributes@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setLineWidth (Graphics #%g##, double #%lineWidth##);")
ENTRY ("Behaviour")
NORMAL ("Sets the relative width of the lines that wil be drawn.")
MAN_END

MAN_BEGIN ("Graphics_setTextAlignment", "ppgb", 19970331)
INTRO ("A routins that changes one of the @@Graphics output attributes@.")
ENTRY ("Purpose")
NORMAL ("to set the horizontal and vertical text alignment of a Graphics object.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setTextAlignment (Graphics #%me##, int #%horizontalAlignment##, int #%verticalAlignment##);")
ENTRY ("Arguments")
TAG ("%me")
DEFINITION ("the Graphics object.")
TAG ("%horizontalAlignment")
DEFINITION ("determines the alignment with respect to the %x argument of @Graphics_text, "
	"along the base line of the text (see @Graphics_setTextRotation); one of the following values:")
LIST_ITEM ("    \\bu Graphics_NOCHANGE: do not change horizontal alignment.")
LIST_ITEM ("    \\bu Graphics_LEFT: left edge of text is at %x.")
LIST_ITEM ("    \\bu Graphics_CENTRE or Graphics_CENTER: centre of text is at %x.")
LIST_ITEM ("    \\bu Graphics_RIGHT: right edge of text is at %x.")
TAG ("%verticalAlignment")
DEFINITION ("determines the alignment with respect to the %y argument of @Graphics_text, "
	"perpendicular to the base line of the text (see @Graphics_setTextRotation); one of the following values:")
LIST_ITEM ("    \\bu Graphics_NOCHANGE: do not change vertical alignment.")
LIST_ITEM ("    \\bu Graphics_BOTTOM: bottom of text is at %y.")
LIST_ITEM ("    \\bu Graphics_HALF: middle of text is at %y.")
LIST_ITEM ("    \\bu Graphics_TOP: top of text is at %y.")
LIST_ITEM ("    \\bu Graphics_BASELINE: baseline of text is at %y.")
NORMAL ("Initially, after creation of a Graphics object, alignment is bottom left.")
MAN_END

MAN_BEGIN ("Graphics_setTextRotation", "ppgb", 19970331)
INTRO ("A routins that changes one of the @@Graphics output attributes@.")
ENTRY ("Purpose")
NORMAL ("to set the orientation of the base line of text.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setTextRotation (Graphics #%me##, double #%angle##);")
ENTRY ("Arguments")
TAG ("%me")
DEFINITION ("the Graphics object.")
TAG ("%angle")
DEFINITION ("the angle of the base line of the text subsequently drawn with @Graphics_text, "
	"measured counterclockwise from 3 o\'clock, and expressed in degrees.")
ENTRY ("Usage")
NORMAL ("The most common values for %angle are:")
LIST_ITEM ("%angle = 0: normal horizontal text from left to right.")
LIST_ITEM ("%angle = 90: text parallel to %y axis, from bottom to top.")
MAN_END

MAN_BEGIN ("Graphics_setWrapWidth", "ppgb", 19970911)
INTRO ("One of the routines that change the @@Graphics output attributes@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_setWrapWidth (Graphics #%g##, double #%wrapWidth##);")
ENTRY ("Behaviour")
NORMAL ("Sets the width of wrappable text, in inches.")
ENTRY ("Usage")
NORMAL ("Text wrapping is off by default. #Graphics_setWrapWidth causes text wrapping to be %on "
	"if %wrapWidth is greater than 0.0, and %off if %wrapWidth is 0.0.")
MAN_END

MAN_BEGIN ("Graphics_surface", "ppgb", 19970527)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_surface (Graphics #%g##, float **#%z##, "
	"long #%ix1##, long #%ix2##, double #%x1##, double #%x2##, "
	"long #%iy1##, long #%iy2##, double #%y1##, double #%y2##, "
	"double #%minimum##, double #%maximum##, "
	"double #%elevation##, double #%azimuth##);")
ENTRY ("Purpose")
NORMAL ("to draw the function z (y, x), sampled as z [iy1..iy2] [ix1..ix2], "
	"as tetragons with corners at the points, "
	"and filled with grey values averaged over the four values of "
	"(z - minimum) / (maximum - minimum) (white at the top), "
	"from the viewpoint [elevation, azimuth].")
ENTRY ("Arguments")
TAG ("%z [%iy] [%ix]")
DEFINITION ("the sampled function to be drawn.")
TAG ("%ix1")
DEFINITION ("the second index into %z of the points at the low %x border.")
TAG ("%ix2")
DEFINITION ("the second index into %z of the points at the high %x border.")
TAG ("%x1")
DEFINITION ("the %x world coordinate at the points on the low %x border.")
TAG ("%x2")
DEFINITION ("the %x world coordinate of the points on the high %x border.")
TAG ("%iy1")
DEFINITION ("the first index into %z of the points at the low %y border.")
TAG ("%iy2")
DEFINITION ("the first index into %z of the points at the high %y border.")
TAG ("%y1")
DEFINITION ("the %y world coordinate at the points on the low %y border.")
TAG ("%y2")
DEFINITION ("the %y world coordinate at the points on the high %y border.")
TAG ("%minimum")
DEFINITION (" the value of %z below which the tetragons are filled with white.")
TAG ("%maximum")
DEFINITION ("the value of %z above which the tetragons are filled with black.")
TAG ("%elevation")
DEFINITION ("the elevation above the %xy plane; 90 means from above, -90 means from below. "
	"A good value is 30 degrees.")
TAG ("%azimuth")
DEFINITION ("counterclockwise turn of the %x axis around the% z axis, from pointing to the right. "
	"A good value is 45 degrees.")
ENTRY ("Behaviour")
NORMAL ("If %ix2 \\<_ %ix1 or %iy2 \\<_ %iy1 or %minimum = %maximum, nothing is drawn. "
	"The %x and %y coordinates of the sample points, "
	"which are thought to be on the corners of the tetragons, are equally spaced "
	"between %x1 and %x2 and between %y1 and %y2: "
	"the space between adjacent values of %x and %y is "
	"%dx = (%x2 \\-- %x1) / (%ix2 \\-- %ix1) and "
	"%dy = (%y2 \\-- %y1) / (%iy2 \\-- %iy1), and "
	"%x [%i] = %x1 + (%i \\-- %ix1) * %dx and "
	"%y [%i] = %y1 + (%i \\-- %iy1) * %dy.")
MAN_END

MAN_BEGIN ("Graphics_text", "ppgb", 19970331)
INTRO ("One of the @@Graphics output routines@.")
ENTRY ("Purpose")
NORMAL ("to draw a text in the current colour, font, size, style, text rotation, and text alignment.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_text (Graphics #%me##, double #%x##, double #%y##, const char *#%text##);")
ENTRY ("Behaviour")
NORMAL ("The current text rotation determines the angle of the base line. "
	"The current text alignment determines the position of the text "
	"relative to the world point (%x, %y), viewed from the base line.")
MAN_END

MAN_BEGIN ("Graphics_unhighlight", "ppgb", 19970911)
INTRO ("See @Graphics_highlight.")
MAN_END

MAN_BEGIN ("Graphics_xorOn", "ppgb", 19970911)
INTRO ("Two of the routines that change the @@Graphics output attributes@.")
ENTRY ("Syntax")
PROTOTYPE ("##void Graphics_xorOn (Graphics #%g##, int #%colour##);")
PROTOTYPE ("##void Graphics_xorOff (Graphics #%g##);")
ENTRY ("Purpose")
NORMAL ("to bracket drawing calls for XOR-mode rendering.")
ENTRY ("Screen behaviour")
CODE ("Graphics_xorOn (g, Graphics_RED);")
CODE ("Graphics_line (g, 0, 0, 100, 100);")
CODE ("Graphics_xorOff (g);")
NORMAL ("For underlying white pixels, the colour of the resulting line will be red. "
	"For other underlying colours, the colour of the resulting pixels is unpredictable.")
ENTRY ("PostScript behaviour")
NORMAL ("No action will be taken.")
MAN_END

MAN_BEGIN ("Graphics_xorOff", "ppgb", 19970911)
INTRO ("See @Graphics_xorOn.")
MAN_END

MAN_BEGIN ("Inside Graphics_text", "ppgb", 20000921)
INTRO ("This page is about the implementation of @Graphics_text, the messages it sends to the @@Graphics drivers@, "
	"and the methods these drivers use to implement the device-specific behaviour. "
	"This information should be read by anyone who wants to write a graphics driver.")
ENTRY ("Input and output")
NORMAL ("The routine @Graphics_text works on an ASCII string that contains "
	"special characters for italic (\\% ), bold (\\# ), subscript (\\_ ), "
	"and superscript (\\^ ) text, as well as backslash sequences for special symbols, "
	"such as \"\\bsa\\^ \" for \"\\a^\", \"\\bs\\%  \" for \"\\% \", \"\\bsep\" for \"\\ep\", and \"\\bsct\" for \"\\ct\".")
NORMAL ("The string that is drawn on your screen or your paper will contain all these "
	"styles, locations, sizes, and symbols.")
ENTRY ("Algorithm")
NORMAL ("In the following discussion, %my will refer to the Graphics object that is the "
	"first argument to @Graphics_text.")
ENTRY ("Step 1: from ASCII to wide characters")
NORMAL ("@Graphics_text converts the ASCII string into a sequence of structures "
	"of type %_Graphics_widechar, defined in %%GraphicsP.h% as:")
CODE ("struct {")
CODE ("   char %first, %second;   // `a' and `\\^ ', or `\\% ' and ` ', or `e' and `p', or `c' and `t'.")
CODE ("   char %link;")
CODE ("   short %style, %size, %code, %baseline;")
CODE ("   float %width; ")
CODE ("   union { long %integer; const char *%string; } %font;")
CODE ("} _Graphics_widechar;")
NORMAL ("Later on, each of these %widechars will contain "
	"all the information needed to draw one symbol onto the screen or the paper. "
	"For now, @Graphics_text performs steps 1a and 1b simultaneously.")
ENTRY ("Step 1a: parsing style information")
NORMAL ("The appropriate style and sub/superscript information is stored "
	"into each %widechar. ")
NORMAL ("After this, the %style field of each %widechar will contain "
	"one of the following values:")
LIST_ITEM ("\\bu $Graphics_ITALIC, if...")
LIST_ITEM ("\\bu $Graphics_BOLD, if... ")
LIST_ITEM ("\\bu 0, otherwise.")
NORMAL ("The %height field will contain -1 (subscript), 1 (superscript), or 0? ")
NORMAL ("The %link field will contain a pointer to a link string or NULL. "
	"The %font and %size fields of all %widechars will contain the current values of %my %font and %my %fontSize, "
	"i.e., those set by @Graphics_setFont and @Graphics_setFontSize.")
ENTRY ("Step 1b: from single ASCII symbols and backslash sequences to two-byte ASCII sequences")
NORMAL ("The remaining ASCII symbols are translated into two-byte glyphs according to "
	"the conventions of the #Longchar database. "
	"This means that backslash sequences are expanded into their constituents, e.g., an \"\\ep\" is stored "
	"as \"ep\", and a \"\\ct\" is stored as \"ct\". "
	"The symbol \\tm (\\bstm) is converted to its sans-serif counterpart \\TM (\\bsTM), "
	"if %my %font happens to be $Graphics_HELVETICA.")
NORMAL ("After this, some of the remaining (single) ASCII symbols are also converted: "
	"double quotes (\\\" ) are converted into their left-hand (\\bs\\\" l = \") and right-hand (\\bs\\\" r = \") versions "
	"according to their odd or even occurrence in the string, "
	"and sequences of f+i and f+l are converted into single fi and fl symbols. ")
NORMAL ("Finally, non-ASCII symbols (which the user may have copied into a text field) "
	"are converted into their two-byte Longchar version, e.g., \"\\a^\", "
	"which could have been typed instead of \"\\bsa\\^ \", will become \"a\\^ \".")
ENTRY ("Step 2: device-dependent text initialization")
NORMAL ("@Graphics_text notifies the graphics driver that text drawing is going to start, "
	"by sending the %%(Graphics)initText% message. "
	"Most drivers will ignore this, but the Macintosh %initText method will set "
	"the global GrafPtr to point to the Mac window associated with the Graphics object.")
ENTRY ("Step 3: measuring each character")
NORMAL ("For each %widechar, @Graphics_text sends the graphics driver the %%(Graphics)charSize% message, "
	"which asks the driver to fill in the %width attribute "
	"of the %widechar. The driver's %charSize method will use the information in the %first, %second, %font, %style, and %size fields "
	"to determine the value of the %width field. Some drivers (%%GraphicsPostscript::charSize%) will have to consult "
	"the Longchar database for this.")
NORMAL ("The driver may change the %font, %style, and %size fields, and fill in the %code field for later use in step 5.")
ENTRY ("Step 4: alignment")
NORMAL ("@Graphics_text now determines the width of the entire string by adding the widths of the separate characters. "
	"It then calculates the position of the first character, from this total width and the current value of the "
	"horizontal and vertical text alignment attributes and the current text rotation.")
ENTRY ("Step 5: drawing the characters")
NORMAL ("@Graphics_text compares the %font, %size, %style, and position attributes of each character "
	"and determines substrings that can probably be drawn without changing the font on the way. "
	"Each of these substrings is sent to the driver in one or more %charDraw messages. "
	"The driver's %charDraw method decides whether it can draw the substring in one throw, "
	"and returns to @Graphics_text the number of characters actually drawn. This number will be at least 1.")
NORMAL ("@Graphics_text checks whether the substring was drawn completely. If not, the remaining part of the substring "
	"is passed to the driver in a new %charDraw message. Once the substring has been completely drawn, "
	"@Graphics_text asks the driver to draw the next substring, and this continues on until the entire text has been drawn.")
ENTRY ("Step 6: device-dependent text clean-up")
NORMAL ("Finally, @Graphics_text tells the driver in an %exitText message that text drawing can be stopped. ")
NORMAL ("The Macintosh %exitText method will set the window font, style, and font size back to the default values "
	"appropriate for drawing controls.")
ENTRY ("Side effects")
NORMAL ("Beside the obvious side effect of drawing text on the screen or on the paper, "
	"@Graphics_text records the strings and the locations of the links contained in the text, during step 5. "
	"These links can be retrieved with #Graphics_getLinks until the next call to @Graphics_text. ")
NORMAL ("The vertical and horizontal position of a hypothetical character that would be drawn immediately after the last character drawn "
	"by @Graphics_text, is recorded into the Graphics object and can be queried with #Graphics_getTextY and #Graphics_getTextX "
	"before the next call to @Graphics_text.")
MAN_END

MAN_BEGIN ("LaserWriter^\\re", "ppgb", 19960709)
INTRO ("a printer manufactured by Apple Computer^\\re.")
MAN_END

}

/* End of file manual_Graphics.c */

