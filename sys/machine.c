/* machine.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2004/06/17 Machine_getMainWindowMenuBarHeight ()
 */

#if defined (sgi) || defined (sun)
	#include <sys/stat.h>
#endif
#include "machine.h"
#include "melder.h"

#define LookAndFeel_MOTIF  0
#define LookAndFeel_SGI  1
#define LookAndFeel_CDE  2
#define LookAndFeel_SOLARIS  3
#define LookAndFeel_HP  4
#define LookAndFeel_SUN4  5
#define LookAndFeel_MAC  6
#define LookAndFeel_WIN32  7
#define LookAndFeel_LINUX  8

static int lookAndFeel;

int Machine_getMenuBarHeight (void) {
	static int heights [] = {
		26,   /* Motif */
		24,   /* SGI */
		28,   /* CDE */
		26,   /* Solaris */
		26,   /* HP */
		26,   /* Sun4 */
		34,   /* Mac */
		0,    /* Win32 */
		30    /* Linux */
	};
	return heights [lookAndFeel];
}

int Machine_getMainWindowMenuBarHeight (void) {
	#ifdef macintoshXXX
		return 0;
	#else
		return Machine_getMenuBarHeight ();
	#endif
}

int Machine_getTitleBarHeight (void) {
	static int heights [] = {   /* Mostly copied from menu-bar height. */
		26,   /* Motif */
		24,   /* SGI */
		28,   /* CDE */
		26,   /* Solaris */
		26,   /* HP */
		26,   /* Sun4 */
		22,   /* Mac */
		20,   /* Win32 */
		30    /* Linux */
	};
	return heights [lookAndFeel];
}

int Machine_getScrollBarWidth (void) {
	static int widths [] = {
		22,   /* Motif */
		22,   /* SGI */
		22,   /* CDE */
		22,   /* Solaris */
		22,   /* HP */
		22,   /* Sun4 */
		16,   /* Mac */
		17,   /* Win32 */
		18    /* Linux */
	};
	return widths [lookAndFeel];
}

int Machine_getTextHeight (void) {
	static int heights [] = {
		29,   /* Motif */
		29,   /* SGI */
		25,   /* CDE */
		29,   /* Solaris */
		29,   /* HP */
		29,   /* Sun4 */
		22,   /* Mac */
		20,   /* Win32 */
		29    /* Linux */
	};
	return heights [lookAndFeel];
}

void Machine_initLookAndFeel (unsigned int argc, char **argv) {
	#if defined (sgi) || defined (sun)
	struct stat statBuf;
	#endif

	/*
	 * Determining the appropriate look-and-feel: the default depends on the client machine.
	 */
	#if defined (sgi)
		lookAndFeel = stat ("/usr/sbin/schemebr", & statBuf) ? LookAndFeel_MOTIF : LookAndFeel_SGI;
	#elif defined (sun4)
		lookAndFeel = LookAndFeel_SUN4;
	#elif defined (sun)
		lookAndFeel = stat ("/usr/dt/bin/dtaction", & statBuf) ? LookAndFeel_SOLARIS : LookAndFeel_CDE;
	#elif defined (HPUX)
		lookAndFeel = LookAndFeel_HP;
	#elif defined (macintosh)
		lookAndFeel = LookAndFeel_MAC;
		return;
	#elif defined (_WIN32)
		lookAndFeel = LookAndFeel_WIN32;
		return;
	#elif defined (linux)
		lookAndFeel = LookAndFeel_LINUX;
	#else
		lookAndFeel = LookAndFeel_MOTIF;
	#endif

	/*
	 * Command line may override the look-and-feel.
	 */
	if (argc > 1) {
		if (strequ (argv [1], "-sgi")) lookAndFeel = LookAndFeel_SGI;
		else if (strequ (argv [1], "-motif")) lookAndFeel = LookAndFeel_MOTIF;
		else if (strequ (argv [1], "-cde")) lookAndFeel = LookAndFeel_CDE;
		else if (strequ (argv [1], "-solaris")) lookAndFeel = LookAndFeel_SOLARIS;
		else if (strequ (argv [1], "-hp")) lookAndFeel = LookAndFeel_HP;
		else if (strequ (argv [1], "-sun4")) lookAndFeel = LookAndFeel_SUN4;
		else if (strequ (argv [1], "-mac")) lookAndFeel = LookAndFeel_MAC;
		else if (strequ (argv [1], "-linux")) lookAndFeel = LookAndFeel_LINUX;
	}
}

#ifdef UNIX
static char *xresources [200] = {
	"*toolTipEnable: false",
	"*autoUnmanage: false",   // pb 20071230
	"*initialResourcesPersistent: False",
	/*"*keyboardFocusPolicy: pointer",*/
	"*keyboardFocusPolicy: explicit",
	"*highlightThickness: 0",
	"*menuBar*XmToggleButtonGadget.indicatorType: one_of_many",
	"*dynamicSubmenuBar.marginHeight: 0",
	"*dynamicSubmenuBar.marginWidth: 0",
	"*XmDrawingArea.resizePolicy: resize_none",
	"*XmDrawingArea.marginWidth: 0",
	"*XmDrawingArea.marginHeight: 0",
	"*XmFileSelectionBox.cancelLabelString: C  a  n  c  e  l",
	"*UiRadio.borderWidth: 1",
	"*raam.XmScrolledWindow.scrollingPolicy: automatic",
	"*raam.XmScrolledWindow.visualPolicy: constant",
	"*UiFileIn_dirList.scrollingPolicy: automatic",
	"*raam*XmList.visibleItemCount: 1",
	"*raam*XmList.selectionPolicy: extended_select",
	"*raam*XmList.automaticSelection: false",
	"*listSizePolicy: constant",
	"*raam.menuWindow*empty.labelString:",
	"*Movie.dialogTitle: Movie",
	"*Movie.movingArea.width: 300",
	"*Movie.movingArea.height: 300",
/*
	The following code makes the middle and right mouse button work the same way as the left,
	for push buttons in the dynamic window in the Objects window of the Praat shell
	(but not for menu items, because that would cause an X server freeze in case a modal dialog
	pops up during the handling of the click).
	The Ui manager needs this, so it can push the OK button for you.
*/
	"*raam.menuWindow.menu.XmPushButton.translations: #override\\n"
	"   <Btn2Down>: Arm()\\n"
	"   <Btn2Down>,<Btn2Up>: Activate() Disarm()\\n"
	"   <Btn2Up>: Activate() Disarm()\\n"
	"   <Btn3Down>: Arm()\\n"
	"   <Btn3Down>,<Btn3Up>: Activate() Disarm()\\n"
	"   <Btn3Up>: Activate() Disarm()",
0 };
static char *sgiResources [] = {
	"Praatwulg.sgiMode: true",
	"Praatwulg.useSchemes: all",
	"Praatwulg.SgNuseEnhancedFSB: false",
	"Praatwulg*XmScrolledWindow*background: SGI_DYNAMIC BasicBackground",
	"Praatwulg*XmScrolledWindow*XmText.background: SGI_DYNAMIC TextFieldBackground",
	"Praatwulg*XmScrolledWindow*XmPushButton.background: SGI_DYNAMIC ButtonBackground",
	"Praatwulg*XmScrolledWindow*dynamicSubmenuBar.background: SGI_DYNAMIC ButtonBackground",
	"Praatwulg*XmScrolledWindow*XmCascadeButton.fontList: SGI_DYNAMIC PlainLabelFont",
	"Praatwulg*UiRadio*XmLabel.background: SGI_DYNAMIC AlternateBackground2",
	"Praatwulg*raam.dynamicSubmenuBar.XmCascadeButton.fontList: SGI_DYNAMIC PlainLabelFont",
	"Praatwulg*raam.dynamicSubmenuBar.background: SGI_DYNAMIC ButtonBackground",
	"Praatwulg*information*text*fontList: SGI_DYNAMIC PlainLabelFont",
	"Praatwulg*XmScale.background: SGI_DYNAMIC YellowColor",
	"Praatwulg*XmScale.foreground: SGI_DYNAMIC RedColor",
	"Praatwulg*UiForm*column.XmLabel.background: SGI_DYNAMIC AlternateBackground1",
	"Praatwulg*raam.Remove*foreground: SGI_DYNAMIC RedColor",

	"*dynamicSubmenuBar.XmCascadeButton.marginWidth: 6",
	"*dynamicSubmenuBar.XmCascadeButton.marginLeft: 1",
	"*raam.width: 440",
	"*raam.height: 700",
	"*raam.menuWindow*spacing: 1",
0 };
static char *explicitColourResources [] = {
	"*background: grey85",
	"*drawingArea*background: white",
	"*foreground: black",
	"*topShadowColor: white",
	"*bottomShadowColor: grey40",
	"*borderColor: grey40",
	"*XmPushButton.background: grey75",
	"*dynamicSubmenuBar*background: grey75",
	"*XmPushButton.borderColor: grey10",
	"*XmPushButtonGadget.borderColor: grey10",
	"*XmPushButton.topShadowColor: white",
	"*XmPushButtonGadget.topShadowColor: white",
	"*XmPushButton.bottomShadowColor: grey40",
	"*XmPushButtonGadget.bottomShadowColor: grey40",
	"*dynamicSubmenuBar.bottomShadowColor: grey40",
	"*menuBar*background: grey85",
	"*dynamicSubmenuBar*XmPushButton.background: grey85",
	"*menuBar*XmPushButton.topShadowColor: white",
	"*menuBar*XmPushButtonGadget.topShadowColor: white",
	"*dynamicSubmenuBar*XmPushButton.topShadowColor: white",
	"*menuBar*XmPushButton.bottomShadowColor: grey30",
	"*menuBar*XmPushButtonGadget.bottomShadowColor: grey40",
	"*dynamicSubmenuBar*XmPushButton.bottomShadowColor: grey40",
	"*XmText.background: grey90",
#ifdef linux
	"*XmTextField.background: grey90",
#else
	"*XmTextField.background: #ba9090",
#endif
	"*XmText.topShadowColor: grey40",
	"*XmTextField.topShadowColor: grey40",
	"*XmText.bottomShadowColor: grey40",
	"*XmTextField.bottomShadowColor: grey40",
	"*XmToggleButton.selectColor: yellow",
	"*XmToggleButtonGadget.selectColor: yellow",

	"*XmScrollBar.troughColor: grey40",
	"*XmScrollBar.width: grey40",
0 };
static char *cdeResources [] = {
	"*XmPushButton.marginTop: 2",
	"*XmPushButton.marginBottom: 2",
	"*XmPushButton.marginLeft: 2",
	"*XmPushButton.marginRight: 2",
	"*dynamicSubmenuBar.XmCascadeButton.marginTop: 0",
	"*dynamicSubmenuBar.XmCascadeButton.marginBottom: 0",
	"*dynamicSubmenuBar.XmCascadeButton.marginLeft: 1",
	"*dynamicSubmenuBar.XmCascadeButton.marginRight: 1",
	"*menuBar*XmPushButton.marginTop: 0",
	"*menuBar*XmPushButtonGadget.marginTop: 0",
	"*dynamicSubmenuBar*XmPushButton.marginTop: 0",
	"*menuBar*XmPushButton.marginBottom: 0",
	"*menuBar*XmPushButtonGadget.marginBottom: 0",
	"*dynamicSubmenuBar*XmPushButton.marginBottom: 0",
	"*dynamicSubmenuBar.XmCascadeButton.marginWidth: 1",
	"*raam.width: 430",
	"*raam.height: 700",
	"*UiForm*column.XmLabel.background: #987",
	"*raam.Remove*foreground: red",
0 };
static char *kdeResources [] = {
	"*XmPushButton.marginTop: 0",
	"*XmPushButton.marginBottom: 0",
	"*XmPushButton.marginLeft: 0",
	"*XmPushButton.marginRight: 0",
	"*dynamicSubmenuBar.XmCascadeButton.marginTop: 0",
	"*dynamicSubmenuBar.XmCascadeButton.marginBottom: 0",
	"*dynamicSubmenuBar.XmCascadeButton.marginLeft: 1",
	"*dynamicSubmenuBar.XmCascadeButton.marginRight: 1",
	"*menuBar*XmPushButton.marginTop: 0",
	"*menuBar*XmPushButtonGadget.marginTop: 0",
	"*dynamicSubmenuBar*XmPushButton.marginTop: 0",
	"*menuBar*XmPushButton.marginBottom: 0",
	"*menuBar*XmPushButtonGadget.marginBottom: 0",
	"*dynamicSubmenuBar*XmPushButton.marginBottom: 0",
	"*dynamicSubmenuBar.XmCascadeButton.marginWidth: 1",
	"*raam.width: 430",
	"*raam.height: 600",
	"*UiForm*column.XmLabel.background: green",
	"*raam.Remove*foreground: red",
0 };
static char *explicitStyleResources [] = {
	"*allowShellResize: false",
	"*highlightOnEnter: false",
	"*highlightThickness: 0",

	"*XmLabel.marginLeft: 0",

	"*XmPushButton.borderWidth: 0",
	"*XmPushButtonGadget.borderWidth: 0",
	"*XmPushButton.shadowThickness: 2",
	"*XmPushButtonGadget.shadowThickness: 2",
	"*dynamicSubmenuBar.shadowThickness: 2",
	"*dynamicSubmenuBar.XmCascadeButton.shadowThickness: 0",
	"*XmPushButton.highlightThickness: 0",
	"*XmPushButtonGadget.highlightThickness: 0",
	"*XmPushButton.marginTop: 2",
	"*XmPushButton.marginBottom: 2",
	"*XmPushButton.marginLeft: 2",
	"*XmPushButton.marginRight: 2",
	"*dynamicSubmenuBar.XmCascadeButton.marginTop: 2",
	"*dynamicSubmenuBar.XmCascadeButton.marginBottom: 2",
	"*dynamicSubmenuBar.XmCascadeButton.marginLeft: 3",
	"*dynamicSubmenuBar.XmCascadeButton.marginRight: 3",

	"*menuBar.marginHeight: 0",
	"*menuBar.XmCascadeButton.marginWidth: 5",
	"*menuBar.spacing: 3",
	"*menuBar*XmPushButton.borderWidth: 0",
	"*menuBar*XmPushButtonGadget.borderWidth: 0",
	"*dynamicSubmenuBar*XmPushButton.borderWidth: 0",
	"*menuBar*XmPushButton.shadowThickness: 2",
	"*menuBar*XmPushButtonGadget.shadowThickness: 2",
	"*dynamicSubmenuBar*XmPushButton.shadowThickness: 2",
	"*menuBar*XmPushButton.marginTop: 0",
	"*menuBar*XmPushButtonGadget.marginTop: 0",
	"*dynamicSubmenuBar*XmPushButton.marginTop: 0",
	"*menuBar*XmPushButton.marginBottom: 0",
	"*menuBar*XmPushButtonGadget.marginBottom: 0",
	"*dynamicSubmenuBar*XmPushButton.marginBottom: 0",
	"*menuBar*XmCascadeButton.marginLeft: 1",
	"*menuBar*XmCascadeButton.marginRight: 1",
	"*menuBar*XmCascadeButton.marginTop: 0",
	"*menuBar*XmCascadeButton.marginBottom: 0",
	"*menuBar*XmCascadeButtonGadget.marginLeft: 1",
	"*menuBar*XmCascadeButtonGadget.marginRight: 1",
	"*menuBar*XmCascadeButtonGadget.marginTop: 0",
	"*menuBar*XmCascadeButtonGadget.marginBottom: 0",

	"*XmTextField.shadowThickness: 1",

	"*XmScrollBar.width: 22",
	"*XmScrollBar.height: 22",

	"*XmScrolledWindow.spacing: 0",
	"*XmScrolledWindow.shadowThickness: 0",

#ifdef linux
	"*UiForm*column.XmLabel.background: green",
#else
	"*UiForm*column.XmLabel.background: #987",
#endif
	"*raam.Remove*foreground: red",
	"*raam.listWindow.borderWidth: 1",
	"*raam.listWindow.list.borderWidth: 0",
	"*raam.menuWindow.borderWidth: 1",
	"*defaultButtonShadowThickness: 0",

	"*dynamicSubmenuBar.XmCascadeButton.marginWidth: 1",
	"*raam.width: 430",
	"*raam.height: 700",
	"*raam.menuWindow*XmRowColumn.spacing: 2",
	"*raam.menuWindow*dynamicSubmenuBar*spacing: 0",
0 };
static char *motifFonts [] = {
	"*fontList: -*-helvetica-medium-r-normal-*-14-*-*-*-*-*-iso10646-1",
	"*menuBar*fontList: -*-helvetica-bold-o-normal-*-14-*-*-*-*-*-iso10646-1",
	"*dynamicSubmenuBar*XmPushButton.fontList: -*-helvetica-bold-o-normal-*-14-*-*-*-*-*-iso10646-1",
	"*XmLabel.fontList: -*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso10646-1",
	"*information*text*fontList: -*-helvetica-medium-r-normal-*-14-*-*-*-*-*-iso10646-1",
0 };
static char *linuxFonts [] = {
	"*fontList: -*-helvetica-medium-r-normal-*-14-*-*-*-*-*-iso10646-1",
0 };
static char *solarisFonts [] = {
	"*fontList: -b&h-lucida sans-medium-r-normal-sans-*-120-*-*-p-*-iso8859-1",
	"*information*fontList: -b&h-lucida sans-medium-r-normal-sans-*-120-*-*-p-*-iso8859-1",
	"*raam*XmLabel.fontList: -b&h-lucida sans-bold-r-normal-sans-*-120-*-*-p-*-iso8859-1",
	"*XmText.fontList: -b&h-lucida sans typewriter-medium-r-normal-sans-*-120-*-*-m-*-iso8859-1",
0 };
static char *sun4Fonts [] = {
	"*XmText.fontList: 9x15",
0 };
static char *hpFonts [] = {
	"*XmText.fontList: user9x17",
	"*UiRadio*XmToggleButton.fontList: user9x17",
0 };
static void appendResources (char **extra) {
	char **p = xresources;
	while (*p) p ++;
	while (*extra) * p ++ = * extra ++;
}

char **Machine_getXresources (void) {
	/*
	 * Add the look-and-feel-dependent resources to the common resources in "xresources".
	 */
	switch (lookAndFeel) {
		case LookAndFeel_MOTIF:
			appendResources (explicitColourResources);
			appendResources (explicitStyleResources);
			appendResources (motifFonts);
			break;
		case LookAndFeel_SGI:
			appendResources (sgiResources);
			break;
		case LookAndFeel_CDE:
			appendResources (cdeResources);
			break;
		case LookAndFeel_SOLARIS:
			appendResources (explicitColourResources);
			appendResources (explicitStyleResources);
			appendResources (solarisFonts);
			break;
		case LookAndFeel_HP:
			appendResources (explicitColourResources);
			appendResources (explicitStyleResources);
			appendResources (motifFonts);
			appendResources (hpFonts);
			break;
		case LookAndFeel_SUN4:
			appendResources (explicitColourResources);
			appendResources (explicitStyleResources);
			appendResources (motifFonts);
			appendResources (sun4Fonts);
			break;
		case LookAndFeel_MAC:
			break;
		case LookAndFeel_WIN32:
			break;
		case LookAndFeel_LINUX:
			appendResources (kdeResources);
			//appendResources (linuxFonts);
			break;
	}
	return & xresources [0];
}
#endif

/* End of file machine.c */

