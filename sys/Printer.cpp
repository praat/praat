/* Printer.cpp
 *
 * Copyright (C) 1998-2011,2012,2013 Paul Boersma
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
 * pb 2003/02/05 distinct treatment of NT and non-NT
 * pb 2004/05/25 font choice strategy
 * pb 2004/09/25 use /tmp as temporary directory
 * pb 2005/05/19 preference to switch screen preview off
 * pb 2005/07/21 removed bug in previous change for Linux
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/28 theCurrentPraat
 * pb 2007/04/28 Mac: error messages for failing PostScript passthrough
 * pb 2007/08/12 wchar_t
 * pb 2007/10/05 less char
 * pb 2007/12/09 enums
 * pb 2009/01/18 arguments to UiForm callbacks
 * pb 2011/05/15 C++
 * pb 2011/07/11 C++
 */

#include <unistd.h> // close

#include "melder.h"

#if defined (_WIN32)
	#include <windows.h>
#endif

#include "Printer.h"
#include "praat.h"   // topShell
#include "Ui.h"
#include "site.h"

/*
 * Everything must look the same on every printer, including on PDF,
 * so the margins must be constant with respect to the paper, not to the writable page.
 */

/* exported */ struct Printer thePrinter = {
	kGraphicsPostscript_spots_DEFAULT, kGraphicsPostscript_paperSize_DEFAULT, kGraphicsPostscript_orientation_DEFAULT, false,
	true, kGraphicsPostscript_fontChoiceStrategy_DEFAULT,
	600, 5100, 6600,
	1.0,
	NULL
};

void Printer_prefs (void) {
	Preferences_addEnum (L"Printer.spots", & thePrinter. spots, kGraphicsPostscript_spots, kGraphicsPostscript_spots_DEFAULT);
	Preferences_addEnum (L"Printer.paperSize", & thePrinter. paperSize, kGraphicsPostscript_paperSize, kGraphicsPostscript_paperSize_DEFAULT);
	Preferences_addBool (L"Printer.allowDirectPostScript", & thePrinter. allowDirectPostScript, true);
	Preferences_addEnum (L"Printer.fontChoiceStrategy", & thePrinter. fontChoiceStrategy, kGraphicsPostscript_fontChoiceStrategy, kGraphicsPostscript_fontChoiceStrategy_DEFAULT);
}

#if defined (macintosh)
	static PMPrintSession theMacPrintSession;
	static PMPageFormat theMacPageFormat;
	static PMPrintSettings theMacPrintSettings;
	static GrafPtr theMacPort;
	static PMRect paperSize;
#endif
#ifdef _WIN32
	static PRINTDLG theWinPrint;
	static HDC theWinDC;
#endif

#if defined (_WIN32) || defined (macintosh)
	int Printer_postScript_printf (void *stream, const char *format, ... ) {
		#if defined (_WIN32)
			static union { char chars [3002]; short shorts [1501]; } theLine;
		#elif defined (macintosh)
			static Handle theLine;
		#endif
		int length;
		va_list args;
		va_start (args, format);
		(void) stream;
		#if cocoa
		#elif defined (_WIN32)
			vsprintf (theLine.chars + 2, format, args);
			length = strlen (theLine.chars + 2);
			theLine.shorts [0] = length;
			if (length > 0 && theLine.chars [length + 1] == '\n') {
				theLine.chars [length + 1] = '\r';
				theLine.chars [length + 2] = '\n';
				theLine.chars [length + 3] = '\0';
				length ++;
			}
			Escape (theWinDC, POSTSCRIPT_PASSTHROUGH, length + 2, theLine.chars, NULL);
		#elif defined (macintosh)
			if (! theLine) {
				theLine = NewHandle (3000);
				HLock (theLine);
			}
			vsprintf (*theLine, format, args);
			length = strlen (*theLine);
			if (length > 0 && (*theLine) [length - 1] == '\n')
				(*theLine) [length - 1] = '\r';
			SetPort (theMacPort);
			PMSessionPostScriptData (theMacPrintSession, *theLine, strlen (*theLine));
		#endif
		va_end (args);
		return 1;
	}
#endif

#if defined (_WIN32)
	static void initPostScriptPage (void) {
		/*
		 * Save the driver's state.
		 */
		Printer_postScript_printf (NULL, "/PraatPictureSaveObject save def\n");
		/*
		 * The LaserWriter driver puts the coordinates upside down.
		 * According to the PostScript Reference Manual,
		 * "There are few situations in which a PostScript language program
		 *  should execute initgraphics explicitly."
		 * This is one of them.
		 BUG: it probably is *not* one of them. Just do something like
		 	currentmatrix [1 0 -1 0 0 0] mul setmatrix
		 or whatever it is.
		 */
		#if 1
		Printer_postScript_printf (NULL, "initmatrix initclip\n");
		#else
Printer_postScript_printf (NULL, "8 8 scale initclip\n");
		#endif
	}
	static void exitPostScriptPage (void) {
		Printer_postScript_printf (NULL, "PraatPictureSaveObject restore\n");
	}
#endif

#if cocoa
#elif defined (_WIN32)
	static void initPrinter (void) {
	}
#elif defined (macintosh)
	static void initPrinter (void) {
		Boolean result;
		PMResolution res300 = { 300, 300 }, res600 = { 600, 600 };
		if (theMacPrintSettings == NULL) {   /* Once. */
			PMCreateSession (& theMacPrintSession);   /* Initialize the Printing Manager. */
			PMCreatePageFormat (& theMacPageFormat);
			PMCreatePrintSettings (& theMacPrintSettings);
			PMSessionDefaultPageFormat (theMacPrintSession, theMacPageFormat);
			PMSessionDefaultPrintSettings (theMacPrintSession, theMacPrintSettings);
		}
		PMSessionValidatePageFormat (theMacPrintSession, theMacPageFormat, & result);
		PMSessionValidatePrintSettings (theMacPrintSession, theMacPrintSettings, & result);
		/*
		 * BUG.
		 * If we now ask for the available printer resolutions,
		 * we may get the answer that there's only 300 dpi (perhaps PostScript drivers say so?).
		 * So we don't rely on that and have a buggy assumption instead.
		 */
		PMSetResolution (theMacPageFormat, & res300);   /* Perhaps all printers have this... */
		PMSetResolution (theMacPageFormat, & res600);   /* ... but this is preferred. */
	}
#endif

void Printer_nextPage (void) {
	#if cocoa
	#elif defined (_WIN32)
		if (thePrinter. postScript) {
			exitPostScriptPage ();
			EndPage (theWinDC);
			StartPage (theWinDC);
			initPostScriptPage ();
		} else {
			if (EndPage (theWinDC) < 0) ;   /* BUG: should give the opportunity of cancellation. */
			StartPage (theWinDC);
			/*
			 * Since StartPage has reset the DC, restore some of our non-default settings.
			 */
			SetBkMode (theWinDC, TRANSPARENT);
			SetTextAlign (theWinDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
		}
	#elif defined (macintosh)
		PMSessionEndPage (theMacPrintSession);
		PMSessionBeginPage (theMacPrintSession, theMacPageFormat, NULL);
		PMSessionGetGraphicsContext (theMacPrintSession, kPMGraphicsContextQuickdraw, (void **) & theMacPort);
		SetPort (theMacPort);
		SetOrigin (- paperSize. left, - paperSize. top);
	#endif
}

int Printer_pageSetup (void) {
	#if cocoa
	#elif defined (_WIN32)
	#elif defined (macintosh)
		Boolean accepted;
		initPrinter ();
		PMSessionPageSetupDialog (theMacPrintSession, theMacPageFormat, & accepted);
	#endif
	return 1;
}

static void DO_Printer_postScriptSettings (UiForm dia, int narg, Stackel args, const wchar_t *sendingString_dummy, Interpreter interpreter_dummy, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
	(void) sendingString_dummy;
	(void) interpreter_dummy;
	(void) invokingButtonTitle;
	(void) modified;
	(void) dummy;
	#if defined (_WIN32)
		thePrinter. allowDirectPostScript = GET_INTEGER (L"Allow direct PostScript");
	#endif
	thePrinter. spots = GET_ENUM (kGraphicsPostscript_spots, L"Grey resolution");
	#if defined (UNIX)
		thePrinter. paperSize = GET_ENUM (kGraphicsPostscript_paperSize, L"Paper size");
	 	if (thePrinter. paperSize == kGraphicsPostscript_paperSize_A3) {
	 		thePrinter. paperWidth = 842 * thePrinter. resolution / 72;
	 		thePrinter. paperHeight = 1191 * thePrinter. resolution / 72;
		} else if (thePrinter. paperSize == kGraphicsPostscript_paperSize_US_LETTER) {
			thePrinter. paperWidth = 612 * thePrinter. resolution / 72;
			thePrinter. paperHeight = 792 * thePrinter. resolution / 72;
		} else {
			thePrinter. paperWidth = 595 * thePrinter. resolution / 72;
			thePrinter. paperHeight = 842 * thePrinter. resolution / 72;
		}
		thePrinter. orientation = GET_ENUM (kGraphicsPostscript_orientation, L"Orientation");
		thePrinter. magnification = GET_REAL (L"Magnification");
		Site_setPrintCommand (GET_STRING (L"printCommand"));
	#endif
	thePrinter. fontChoiceStrategy = GET_ENUM (kGraphicsPostscript_fontChoiceStrategy, L"Font choice strategy");
}

int Printer_postScriptSettings (void) {
	static Any dia;
	if (dia == NULL) {
		Any radio;
		dia = UiForm_create (theCurrentPraatApplication -> topShell, L"PostScript settings", DO_Printer_postScriptSettings, NULL, L"PostScript settings...", L"PostScript settings...");
		#if defined (_WIN32)
			BOOLEAN (L"Allow direct PostScript", TRUE);
		#endif
		RADIO_ENUM (L"Grey resolution", kGraphicsPostscript_spots, DEFAULT)
		#if defined (UNIX)
			RADIO_ENUM (L"Paper size", kGraphicsPostscript_paperSize, DEFAULT);
			RADIO_ENUM (L"Orientation", kGraphicsPostscript_orientation, DEFAULT);
			POSITIVE (L"Magnification", L"1.0");
			LABEL (L"label", L"Print command:");
			#if defined (linux)
				TEXTFIELD (L"printCommand", L"lpr %s");
			#else
				TEXTFIELD (L"printCommand", L"lp -c %s");
			#endif
		#endif
		RADIO_ENUM (L"Font choice strategy", kGraphicsPostscript_fontChoiceStrategy, DEFAULT);
		UiForm_finish (dia);
	}
	#if defined (_WIN32)
		SET_INTEGER (L"Allow direct PostScript", thePrinter. allowDirectPostScript);
	#endif
	SET_ENUM (L"Grey resolution", kGraphicsPostscript_spots, thePrinter. spots);
	#if defined (UNIX)
		SET_ENUM (L"Paper size", kGraphicsPostscript_paperSize, thePrinter. paperSize);
		SET_ENUM (L"Orientation", kGraphicsPostscript_orientation, thePrinter. orientation);
		SET_REAL (L"Magnification", thePrinter. magnification);
		SET_STRING (L"printCommand", Site_getPrintCommand ());
	#endif
	SET_ENUM (L"Font choice strategy", kGraphicsPostscript_fontChoiceStrategy, thePrinter. fontChoiceStrategy);
	UiForm_do (dia, false);
	return 1;
}

#ifdef _WIN32
	static BOOL CALLBACK AbortFunc (HDC hdc, int nCode) {
		MSG msg;
		(void) hdc;
		(void) nCode;
		while (PeekMessage (& msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage (& msg);
			DispatchMessage (& msg);
		}
		return TRUE;
	}
	HDC Printer_getDC (void) {
		if (! theWinPrint. hDevMode) {
			memset (& theWinPrint, 0, sizeof (PRINTDLG));
			theWinPrint. lStructSize = sizeof (PRINTDLG);
			theWinPrint. Flags = PD_RETURNDEFAULT | PD_RETURNDC;
			PrintDlg (& theWinPrint);
		}
		return theWinPrint. hDC;
	}
#endif

int Printer_print (void (*draw) (void *boss, Graphics g), void *boss) {
	try {
		#if defined (UNIX)
			structMelderFile tempFile = { 0 };
			char tempPath_utf8 [] = "/tmp/picXXXXXX";
			close (mkstemp (tempPath_utf8));
			Melder_pathToFile (Melder_peekUtf8ToWcs (tempPath_utf8), & tempFile);
			thePrinter. graphics = Graphics_create_postscriptjob (& tempFile, thePrinter. resolution,
				thePrinter. spots, thePrinter. paperSize, thePrinter. orientation, thePrinter. magnification);
			draw (boss, thePrinter. graphics);
			forget (thePrinter. graphics);
			char command [500];
			sprintf (command, Melder_peekWcsToUtf8 (Site_getPrintCommand ()), tempPath_utf8);
			system (command);
			MelderFile_delete (& tempFile);
		#elif cocoa
		#elif defined (_WIN32)
			int postScriptCode = POSTSCRIPT_PASSTHROUGH;
			DOCINFO docInfo;
			DEVMODE *devMode;
			initPrinter ();
			if (! theWinPrint. hDevMode) {
				memset (& theWinPrint, 0, sizeof (PRINTDLG));
				theWinPrint. lStructSize = sizeof (PRINTDLG);
				theWinPrint. Flags = PD_RETURNDEFAULT;
				if (! PrintDlg (& theWinPrint)) Melder_throw ("Cannot initialize printer.");
			}
			if (Melder_backgrounding) {
				theWinPrint. Flags = PD_RETURNDEFAULT | PD_RETURNDC;
				if (! PrintDlg (& theWinPrint) || theWinPrint. hDC == NULL) {
					Melder_throw ("Cannot print from a script on this computer.");
				}
			} else {
				theWinPrint. Flags &= ~ PD_RETURNDEFAULT;
				theWinPrint. Flags |= PD_RETURNDC;
				if (! PrintDlg (& theWinPrint)) return 1;
			}
			theWinDC = theWinPrint. hDC;
			thePrinter. postScript = thePrinter. allowDirectPostScript &&
				Escape (theWinDC, QUERYESCSUPPORT, sizeof (int), (LPSTR) & postScriptCode, NULL);
			/*
			 * The HP colour inkjet printer returns in dmFields:
			 * 0, 1, 8, 9, 10, 11, 12, 13, 14, 15, 23, 24, 25, 26 = DM_ORIENTATION |
			 *    DM_PAPERSIZE | DM_COPIES | DM_DEFAULTSOURCE | DM_PRINTQUALITY |
			 *    DM_COLOR | DM_DUPLEX | DM_YRESOLUTION | DM_TTOPTION | DM_COLLATE |
			 *    DM_ICMMETHOD | DM_ICMINTENT | DM_MEDIATYPE | DM_DITHERTYPE
			 */
			devMode = * (DEVMODE **) theWinPrint. hDevMode;
			thePrinter. resolution = devMode -> dmFields & DM_YRESOLUTION ? devMode -> dmYResolution :
				devMode -> dmFields & DM_PRINTQUALITY ?
				( devMode -> dmPrintQuality > 0 ? devMode -> dmPrintQuality : 300 ) : 300;
			if (devMode -> dmFields & DM_PAPERWIDTH) {
				thePrinter. paperWidth = devMode -> dmPaperWidth * thePrinter. resolution / 254;
				thePrinter. paperHeight = devMode -> dmPaperLength * thePrinter. resolution / 254;
			} else if (devMode -> dmFields & DM_PAPERSIZE) {
				static struct { float width, height; } sizes [] = { { 0, 0 }, { 8.5, 11 }, { 8.5, 11 }, { 11, 17 },
					{ 17, 11 }, { 8.5, 14 }, { 5.5, 8.5 }, { 7.25, 10.5 }, { 297/25.4, 420/25.4 },
					{ 210/25.4, 297/25.4 }, { 210/25.4, 297/25.4 }, { 148.5/25.4, 210/25.4 },
					{ 250/25.4, 354/25.4 }, { 182/25.4, 257/25.4 }, { 8.5, 13 },
					{ 215/25.4, 275/25.4 }, { 10, 14 }, { 11, 17 }, { 8.5, 11 }, { 3.875, 8.875 },
					{ 4.125, 9.5 }, { 4.5, 10.375 } };
				int paperSize = devMode -> dmPaperSize;
				if (paperSize <= 0 || paperSize > 21) paperSize = 1;
				thePrinter. paperWidth = sizes [paperSize]. width * thePrinter. resolution;
				thePrinter. paperHeight = sizes [paperSize]. height * thePrinter. resolution;
				if (devMode -> dmOrientation == DMORIENT_LANDSCAPE) {
					long dummy = thePrinter. paperWidth;
					thePrinter. paperWidth = thePrinter. paperHeight;
					thePrinter. paperHeight = dummy;
				}
			} else {
				thePrinter. paperWidth = 1000;
				thePrinter. paperHeight = 1000;
			}
			EnableWindow ((HWND) XtWindow (theCurrentPraatApplication -> topShell -> d_xmShell), FALSE);
			SetAbortProc (theWinDC, AbortFunc);
			memset (& docInfo, 0, sizeof (DOCINFO));
			docInfo. cbSize = sizeof (DOCINFO);
			docInfo. lpszDocName = L"Praatjes";
			docInfo. lpszOutput = NULL;
			if (thePrinter. postScript) {
				StartDoc (theWinDC, & docInfo);
				StartPage (theWinDC);
				initPostScriptPage ();
				thePrinter. graphics = Graphics_create_postscriptprinter ();
				draw (boss, thePrinter. graphics);
				forget (thePrinter. graphics);
				exitPostScriptPage ();
				EndPage (theWinDC);
				EndDoc (theWinDC);
			} else {
				StartDoc (theWinDC, & docInfo);
				StartPage (theWinDC);
				thePrinter. graphics = Graphics_create_screenPrinter (NULL, theWinDC);
				draw (boss, thePrinter. graphics);
				forget (thePrinter. graphics);
				if (EndPage (theWinDC) < 0) {
					Melder_throw ("Cannot print page.");
				} else {
					EndDoc (theWinDC);
				}
			}
			EnableWindow ((HWND) XtWindow (theCurrentPraatApplication -> topShell -> d_xmShell), TRUE);
			DeleteDC (theWinDC), theWinDC = NULL;
		#elif defined (macintosh)
			Boolean result;
			initPrinter ();
			if (Melder_backgrounding) {
				PMSessionValidatePageFormat (theMacPrintSession, theMacPageFormat, & result);
				PMSessionValidatePrintSettings (theMacPrintSession, theMacPrintSettings, & result);
			} else {
				Boolean accepted;
				PMSessionPrintDialog (theMacPrintSession, theMacPrintSettings, theMacPageFormat, & accepted);
				if (! accepted) return 1;   // normal cancelled return
			}
			PMSessionValidatePageFormat (theMacPrintSession, theMacPageFormat, & result);
			PMSessionValidatePrintSettings (theMacPrintSession, theMacPrintSettings, & result);
			PMResolution res;
			PMGetResolution (theMacPageFormat, & res);
			thePrinter. resolution = res. hRes;
			PMGetAdjustedPaperRect (theMacPageFormat, & paperSize);
			thePrinter. paperWidth = paperSize. right - paperSize. left;
			thePrinter. paperHeight = paperSize. bottom - paperSize. top;
			thePrinter. postScript = false;
			PMOrientation orientation;
			PMGetOrientation (theMacPageFormat, & orientation);
			thePrinter. orientation = orientation == kPMLandscape ||
				orientation == kPMReverseLandscape ? kGraphicsPostscript_orientation_LANDSCAPE : kGraphicsPostscript_orientation_PORTRAIT;
			PMSessionBeginDocument (theMacPrintSession, theMacPrintSettings, theMacPageFormat);   // PMSessionBeginCGDocumentNoDialog
			PMSessionBeginPage (theMacPrintSession, theMacPageFormat, NULL);   // PMSessionBeginPageNoDialog
			PMSessionGetGraphicsContext (theMacPrintSession, kPMGraphicsContextQuickdraw, (void **) & theMacPort);
			/*
			 * On PostScript, the point (0, 0) is the bottom left corner of the paper, which is fine.
			 * On the screen, however, the point (0, 0) is the top left corner of the writable page.
			 * Since we want paper-related margins, not writable-page-related margins,
			 * we require that this point gets the coordinates (250, 258) or so,
			 * so that the top left corner of the paper gets coordinates (0, 0).
			 * The "left" and "top" attributes of rPaper are negative values (e.g. -250 and -258),
			 * so multiply them by -1.
			 *
			 * Under Carbon, the port has to be set inside the page.
			 */
			SetPort (theMacPort);
			SetOrigin (- paperSize. left, - paperSize. top);
			thePrinter. graphics = Graphics_create_screenPrinter (NULL, theMacPort);
			draw (boss, thePrinter. graphics);
			forget (thePrinter. graphics);
			if (theMacPort) {
				PMSessionEndPage (theMacPrintSession);   // PMSessionEndPageNoDialog
				PMSessionEndDocument (theMacPrintSession);   // PMSessionEndDocumentNoDialog
				theMacPort = NULL;
			}
		#endif
		return 1;
	} catch (MelderError) {
		#if cocoa
		#elif defined (macintosh)
			if (theMacPort) {
				PMSessionEndPage (theMacPrintSession);
				PMSessionEndDocument (theMacPrintSession);
				theMacPort = NULL;
			}
		#endif
		Melder_throw ("Not printed.");
	}
}

/* End of file Printer.cpp */
