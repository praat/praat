/* motifEmulator_resources.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1998/09/28
 * pb 2002/03/07 GPL
 * pb 2002/04/06
 */

/*
	This file is included in motif.h, where motif_RESOURCE(XmNxxx) expands into:
		XmNxxx,
	This file is also included in motifMacintosh.c, where motif_RESOURCE(XmNxxx) expands into:
		"XmNxxx".
	With this trick, resource names are known as enums (for fast access) and as strings (for messages).
*/

motif_RESOURCE (XmNaccelerator)   /* XmLabel */
motif_RESOURCE (XmNaccelerators)   /* Core */
motif_RESOURCE (XmNacceleratorText)   /* XmLabel */
motif_RESOURCE (XmNactivateCallback)   /* XmPushButton, XmDrawnButton, XmCascadeButton, XmText */
motif_RESOURCE (XmNadjustLast)   /* XmRowColumn */
motif_RESOURCE (XmNadjustMargin)   /* XmRowColumn */
motif_RESOURCE (XmNalignment)   /* XmLabel */
motif_RESOURCE (XmNallowOverlap)   /* XmBulletinBoard */
motif_RESOURCE (XmNallowShellResize)   /* Shell */
motif_RESOURCE (XmNancestorSensitive)   /* Core */
motif_RESOURCE (XmNargc)   /* ApplicationShell */
motif_RESOURCE (XmNargv)   /* ApplicationShell */
motif_RESOURCE (XmNarmCallback)   /* XmPushButton, XmDrawnButton, XmToggleButton */
motif_RESOURCE (XmNarmColor)   /* XmPushButton */
motif_RESOURCE (XmNarmPixmap)   /* XmPushButton */
motif_RESOURCE (XmNautomaticSelection)   /* XmList */
motif_RESOURCE (XmNautoShowCursorPosition)   /* XmText */
motif_RESOURCE (XmNautoUnmanage)   /* XmBulletinBoard */
motif_RESOURCE (XmNbackground)   /* Core */
motif_RESOURCE (XmNbackgroundPixmap)   /* Core */
motif_RESOURCE (XmNbaseHeight)   /* WMShell */
motif_RESOURCE (XmNbaseWidth)   /* WMShell */
motif_RESOURCE (XmNblinkRate)   /* XmText Output */
motif_RESOURCE (XmNborderColor)   /* Core */
motif_RESOURCE (XmNborderPixmap)   /* Core */
motif_RESOURCE (XmNborderWidth)   /* Core */
motif_RESOURCE (XmNbottomAttachment)   /* XmForm Constraint */
motif_RESOURCE (XmNbottomOffset)   /* XmForm Constraint */
motif_RESOURCE (XmNbottomPosition)   /* XmForm Constraint */
motif_RESOURCE (XmNbottomShadowColor)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNbottomShadowPixmap)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNbottomWidget)   /* XmForm Constraint */
motif_RESOURCE (XmNbrowseSelectionCallback)   /* XmList */
motif_RESOURCE (XmNbuttonFontList)   /* XmBulletinBoard */
motif_RESOURCE (XmNcancelButton)   /* XmBulletinBoard */
motif_RESOURCE (XmNcancelCallback)   /* XmMessagebox */
motif_RESOURCE (XmNcancelLabelString)   /* XmMessagebox */
motif_RESOURCE (XmNcascadePixmap)   /* XmCascadeButton */
motif_RESOURCE (XmNcascadingCallback)   /* XmCascadeButton */
motif_RESOURCE (XmNchildren)   /* Composite */
motif_RESOURCE (XmNclipWindow)   /* XmScrolledWindow */
motif_RESOURCE (XmNcolormap)   /* Core */
motif_RESOURCE (XmNcolumns)   /* XmText Output */
motif_RESOURCE (XmNcreatePopupChildProc)   /* Shell */
motif_RESOURCE (XmNcursorPosition)   /* XmText */
motif_RESOURCE (XmNcursorPositionVisible)   /* XmText Output */
motif_RESOURCE (XmNdecimalPoints)   /* XmScale */
motif_RESOURCE (XmNdecrementCallback)   /* XmScrollBar */
motif_RESOURCE (XmNdefaultActionCallback)   /* XmList */
motif_RESOURCE (XmNdefaultButton)   /* XmBulletinBoard */
motif_RESOURCE (XmNdefaultButtonType)   /* XmMessagebox */
motif_RESOURCE (XmNdefaultButtonShadowThickness)   /* XmPushButton */
motif_RESOURCE (XmNdefaultFontList)   /* VendorShell */
motif_RESOURCE (XmNdefaultPosition)   /* XmBulletinBoard */
motif_RESOURCE (XmNdeleteResponse)   /* VendorShell */
motif_RESOURCE (XmNdepth)   /* Core */
motif_RESOURCE (XmNdestroyCallback)   /* Core */
motif_RESOURCE (XmNdialogStyle)   /* XmBulletinBoard */
motif_RESOURCE (XmNdialogTitle)   /* XmBulletinBoard */
motif_RESOURCE (XmNdialogType)   /* XmMessagebox */
motif_RESOURCE (XmNdisarmCallback)   /* XmPushButton, XmDrawnButton, XmToggleButton */
motif_RESOURCE (XmNdragCallback)   /* XmScrollBar, XmScale */
motif_RESOURCE (XmNeditable)   /* XmText */
motif_RESOURCE (XmNeditMode)   /* XmText */
motif_RESOURCE (XmNentryAlignment)   /* XmRowColumn */
motif_RESOURCE (XmNentryBorder)   /* XmRowColumn */
motif_RESOURCE (XmNentryCallback)   /* XmRowColumn */
motif_RESOURCE (XmNentryClass)   /* XmRowColumn */
motif_RESOURCE (XmNexposeCallback)   /* XmDrawnButton, XmDrawingArea */
motif_RESOURCE (XmNextendedSelectionCallback)   /* XmList */
motif_RESOURCE (XmNfillOnArm)   /* XmPushButton */
motif_RESOURCE (XmNfillOnSelect)   /* XmToggleButton */
motif_RESOURCE (XmNfocusCallback)   /* XmBulletinBoard, XmText */
motif_RESOURCE (XmNfontList)   /* XmLabel, XmText, XmScale, XmList */
motif_RESOURCE (XmNforeground)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNfractionBase)   /* XmForm */
motif_RESOURCE (XmNgainPrimaryCallback)   /* XmText */
motif_RESOURCE (XmNgeometry)   /* Shell */
motif_RESOURCE (XmNheight)   /* Core */
motif_RESOURCE (XmNheightInc)   /* WMShell */
motif_RESOURCE (XmNhelpCallback)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNhelpLabelString)   /* XmMessagebox */
motif_RESOURCE (XmNhighlightColor)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNhighlightOnEnter)   /* XmPrimitive, XmScale */
motif_RESOURCE (XmNhighlightPixmap)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNhighlightThickness)   /* XmPrimitive, XmScale */
motif_RESOURCE (XmNhorizontalScrollBar)   /* XmScrolledWindow */
motif_RESOURCE (XmNhorizontalSpacing)   /* XmForm */
motif_RESOURCE (XmNiconic)   /* TopLevelShell */
motif_RESOURCE (XmNiconMask)   /* WMShell */
motif_RESOURCE (XmNiconName)   /* TopLevelShell */
motif_RESOURCE (XmNiconNameEncoding)   /* TopLevelShell */
motif_RESOURCE (XmNiconPixmap)   /* WMShell */
motif_RESOURCE (XmNiconWindow)   /* WMShell */
motif_RESOURCE (XmNiconX)   /* WMShell */
motif_RESOURCE (XmNiconY)   /* WMShell */
motif_RESOURCE (XmNincrement)   /* XmScrollBar */
motif_RESOURCE (XmNincrementCallback)   /* XmScrollBar */
motif_RESOURCE (XmNindicatorOn)   /* XmToggleButton */
motif_RESOURCE (XmNindicatorSize)   /* XmToggleButton */
motif_RESOURCE (XmNindicatorType)   /* XmToggleButton */
motif_RESOURCE (XmNinitialDelay)   /* XmScrollBar */
motif_RESOURCE (XmNinitialResourcesPersistent)   /* Core */
motif_RESOURCE (XmNinitialState)   /* WMShell */
motif_RESOURCE (XmNinput)   /* WMShell */
motif_RESOURCE (XmNinputCallback)   /* XmDrawingArea */
motif_RESOURCE (XmNinsertPosition)   /* Composite */
motif_RESOURCE (XmNisAligned)   /* XmRowColumn */
motif_RESOURCE (XmNisHomogeneous)   /* XmRowColumn */
motif_RESOURCE (XmNitemCount)   /* XmList */
motif_RESOURCE (XmNitems)   /* XmList */
motif_RESOURCE (XmNkeyboardFocusPolicy)   /* VendorShell */
motif_RESOURCE (XmNlabelFontList)   /* XmBulletinBoard */
motif_RESOURCE (XmNlabelInsensitivePixmap)   /* XmLabel */
motif_RESOURCE (XmNlabelPixmap)   /* XmLabel */
motif_RESOURCE (XmNlabelString)   /* XmLabel, XmRowColumn */
motif_RESOURCE (XmNlabelType)   /* XmLabel */
motif_RESOURCE (XmNleftAttachment)   /* XmForm Constraint */
motif_RESOURCE (XmNleftOffset)   /* XmForm Constraint */
motif_RESOURCE (XmNleftPosition)   /* XmForm Constraint */
motif_RESOURCE (XmNleftWidget)   /* XmForm Constraint */
motif_RESOURCE (XmNlistMarginHeight)   /* XmList */
motif_RESOURCE (XmNlistMarginWidth)   /* XmList */
motif_RESOURCE (XmNlistSizePolicy)   /* XmList */
motif_RESOURCE (XmNlistSpacing)   /* XmList */
motif_RESOURCE (XmNlosePrimaryCallback)   /* XmText */
motif_RESOURCE (XmNlosingFocusCallback)   /* XmText */
motif_RESOURCE (XmNmapCallback)   /* XmRowColumn, XmBulletinBoard */
motif_RESOURCE (XmNmappedWhenManaged)   /* Core */
motif_RESOURCE (XmNmappingDelay)   /* XmCascadeButton */
motif_RESOURCE (XmNmarginBottom)   /* XmLabel */
motif_RESOURCE (XmNmarginHeight)   /* XmLabel, XmDrawingArea, XmRowColumn */
motif_RESOURCE (XmNmarginLeft)   /* XmLabel */
motif_RESOURCE (XmNmarginRight)   /* XmLabel */
motif_RESOURCE (XmNmarginTop)   /* XmLabel */
motif_RESOURCE (XmNmarginWidth)   /* XmLabel, XmDrawingArea, XmRowColumn */
motif_RESOURCE (XmNmaxAspectX)   /* WMShell */
motif_RESOURCE (XmNmaxAspectY)   /* WMShell */
motif_RESOURCE (XmNmaxHeight)   /* WMShell */
motif_RESOURCE (XmNmaximum)   /* XmScrollBar, XmScale */
motif_RESOURCE (XmNmaxLength)   /* XmText */
motif_RESOURCE (XmNmaxWidth)   /* WMShell */
motif_RESOURCE (XmNmenuAccelerator)   /* XmRowColumn */
motif_RESOURCE (XmNmenuHelpWidget)   /* XmRowColumn */
motif_RESOURCE (XmNmenuHistory)   /* XmRowColumn */
motif_RESOURCE (XmNmessageAlignment)   /* XmMessagebox */
motif_RESOURCE (XmNmessageString)   /* XmMessagebox */
motif_RESOURCE (XmNminAspectX)   /* WMShell */
motif_RESOURCE (XmNminAspectY)   /* WMShell */
motif_RESOURCE (XmNminHeight)   /* WMShell */
motif_RESOURCE (XmNminimizeButtons)   /* XmMessagebox */
motif_RESOURCE (XmNminimum)   /* XmScrollBar, XmScale */
motif_RESOURCE (XmNminWidth)   /* WMShell */
motif_RESOURCE (XmNmnemonic)   /* XmLabel */
motif_RESOURCE (XmNmnemonicCharSet)   /* XmLabel */
motif_RESOURCE (XmNmodifyVerifyCallback)   /* XmText */
motif_RESOURCE (XmNmotionVerifyCallback)   /* XmText */
motif_RESOURCE (XmNmoveCallback)   /* XmDrawingArea */   /* MACINTOSH ONLY */
motif_RESOURCE (XmNmultiClick)   /* XmPushButton, XmDrawnButton */
motif_RESOURCE (XmNmultipleSelectionCallback)   /* XmList */
motif_RESOURCE (XmNmwmDecorations)   /* VendorShell */
motif_RESOURCE (XmNmwmFunctions)   /* VendorShell */
motif_RESOURCE (XmNmwmInputMode)   /* VendorShell */
motif_RESOURCE (XmNmwmMenu)   /* VendorShell */
motif_RESOURCE (XmNnavigationType)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNnoResize)   /* XmBulletinBoard */
motif_RESOURCE (XmNnumChildren)   /* Composite */
motif_RESOURCE (XmNnumColumns)   /* XmRowColumn */
motif_RESOURCE (XmNokCallback)   /* XmMessagebox */
motif_RESOURCE (XmNokLabelString)   /* XmMessagebox */
motif_RESOURCE (XmNorientation)   /* XmRowColumn, XmScale, XmScrollBar */
motif_RESOURCE (XmNoverrideRedirect)   /* Shell */
motif_RESOURCE (XmNpacking)   /* XmRowColumn */
motif_RESOURCE (XmNpageDecrementCallback)   /* XmScrollBar */
motif_RESOURCE (XmNpageIncrement)   /* XmScrollBar */
motif_RESOURCE (XmNpageIncrementCallback)   /* XmScrollBar */
motif_RESOURCE (XmNpendingDelete)   /* XmText Input */
motif_RESOURCE (XmNpopdownCallback)   /* Shell */
motif_RESOURCE (XmNpopupCallback)   /* Shell */
motif_RESOURCE (XmNpopupEnabled)   /* XmRowColumn */
motif_RESOURCE (XmNprocessingDirection)   /* XmScrollBar, XmScale */
motif_RESOURCE (XmNpushButtonEnabled)   /* XmDrawnButton */
motif_RESOURCE (XmNradioAlwaysOne)   /* XmRowColumn */
motif_RESOURCE (XmNradioBehavior)   /* XmRowColumn */
motif_RESOURCE (XmNrecomputeSize)   /* XmLabel */
motif_RESOURCE (XmNrepeatDelay)   /* XmScrollBar */
motif_RESOURCE (XmNresizable)   /* XmForm Constraint */
motif_RESOURCE (XmNresizeCallback)   /* XmDrawnButton, XmDrawingArea */
motif_RESOURCE (XmNresizeHeight)   /* XmRowColumn, XmText */
motif_RESOURCE (XmNresizePolicy)   /* XmDrawingArea, XmBulletinBoard */
motif_RESOURCE (XmNresizeWidth)   /* XmRowColumn, XmText */
motif_RESOURCE (XmNrightAttachment)   /* XmForm Constraint */
motif_RESOURCE (XmNrightOffset)   /* XmForm Constraint */
motif_RESOURCE (XmNrightPosition)   /* XmForm Constraint */
motif_RESOURCE (XmNrightWidget)   /* XmForm Constraint */
motif_RESOURCE (XmNrowColumnType)   /* XmRowColumn */
motif_RESOURCE (XmNrows)   /* XmText Output */
motif_RESOURCE (XmNrubberPositioning)   /* XmForm */
motif_RESOURCE (XmNsaveUnder)   /* Shell */
motif_RESOURCE (XmNscaleHeight)   /* XmScale */
motif_RESOURCE (XmNscaleMultiple)   /* XmScale */
motif_RESOURCE (XmNscaleWidth)   /* XmScale */
motif_RESOURCE (XmNscreen)   /* Core */
motif_RESOURCE (XmNscrollBarDisplayPolicy)   /* XmScrolledWindow, XmList */
motif_RESOURCE (XmNscrollBarPlacement)   /* XmScrolledWindow */
motif_RESOURCE (XmNscrolledWindowMarginHeight)   /* XmScrolledWindow */
motif_RESOURCE (XmNscrolledWindowMarginWidth)   /* XmScrolledWindow */
motif_RESOURCE (XmNscrollHorizontal)   /* XmText ScrolledText */
motif_RESOURCE (XmNscrollingPolicy)   /* XmScrolledWindow */
motif_RESOURCE (XmNscrollLeftSide)   /* XmText ScrolledText */
motif_RESOURCE (XmNscrollTopSide)   /* XmText ScrolledText */
motif_RESOURCE (XmNscrollVertical)   /* XmText ScrolledText */
motif_RESOURCE (XmNselectColor)   /* XmToggleButton */
motif_RESOURCE (XmNselectedItemCount)   /* XmList */
motif_RESOURCE (XmNselectedItems)   /* XmList */
motif_RESOURCE (XmNselectInsensitivePixmap)   /* XmToggleButton */
motif_RESOURCE (XmNselectionArray)   /* XmText Input */
motif_RESOURCE (XmNselectionArrayCount)   /* XmText Input */
motif_RESOURCE (XmNselectionPolicy)   /* XmList */
motif_RESOURCE (XmNselectPixmap)   /* XmToggleButton */
motif_RESOURCE (XmNselectThreshold)   /* XmText Input */
motif_RESOURCE (XmNsensitive)   /* Core */
motif_RESOURCE (XmNset)   /* XmToggleButton */
motif_RESOURCE (XmNshadowThickness)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNshadowType)   /* XmDrawnButton, XmBulletinBoard */
motif_RESOURCE (XmNshowArrows)   /* XmScrollBar */
motif_RESOURCE (XmNshowAsDefault)   /* XmPushButton */
motif_RESOURCE (XmNshowValue)   /* XmScale */
motif_RESOURCE (XmNsingleSelectionCallback)   /* XmList */
motif_RESOURCE (XmNsliderSize)   /* XmScrollBar */
motif_RESOURCE (XmNsource)   /* XmText */
motif_RESOURCE (XmNspacing)   /* XmToggleButton, XmRowColumn, XmScrolledWindow */
motif_RESOURCE (XmNstringDirection)   /* XmManager, XmLabel, XmList */
motif_RESOURCE (XmNsubMenuId)   /* XmCascadeButton */
motif_RESOURCE (XmNsymbolPixmap)   /* XmMessagebox */
motif_RESOURCE (XmNtextFontList)   /* XmBulletinBoard */
motif_RESOURCE (XmNtextTranslations)   /* XmBulletinBoard */
motif_RESOURCE (XmNtitle)   /* WMShell */
motif_RESOURCE (XmNtitleEncoding)   /* WMShell */
motif_RESOURCE (XmNtitleString)   /* XmScale */
motif_RESOURCE (XmNtoBottomCallback)   /* XmScrollBar */
motif_RESOURCE (XmNtopAttachment)   /* XmForm Constraint */
motif_RESOURCE (XmNtopCharacter)   /* XmText */
motif_RESOURCE (XmNtopItemPosition)   /* XmList */
motif_RESOURCE (XmNtopOffset)   /* XmForm Constraint */
motif_RESOURCE (XmNtopPosition)   /* XmForm Constraint */
motif_RESOURCE (XmNtopShadowColor)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNtopShadowPixmap)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNtopWidget)   /* XmForm Constraint */
motif_RESOURCE (XmNtoTopCallback)   /* XmScrollBar */
motif_RESOURCE (XmNtransient)   /* WMShell */
motif_RESOURCE (XmNtransientFor)   /* TransientShell */
motif_RESOURCE (XmNtranslations)   /* Core */
motif_RESOURCE (XmNtraversalOn)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNtroughColor)   /* XmScrollBar */
motif_RESOURCE (XmNunmapCallback)   /* XmRowColumn, XmBulletinBoard */
motif_RESOURCE (XmNuserData)   /* XmPrimitive, XmManager */
motif_RESOURCE (XmNvalue)   /* XmText, XmScrollBar, XmScale */
motif_RESOURCE (XmNvalueChangedCallback)   /* XmText, XmToggleButton, XmScrollBar, XmScale */
motif_RESOURCE (XmNverifyBell)   /* XmText */
motif_RESOURCE (XmNverticalScrollBar)   /* XmScrolledWindow */
motif_RESOURCE (XmNverticalSpacing)   /* XmForm */
motif_RESOURCE (XmNvisibleItemCount)   /* XmList */
motif_RESOURCE (XmNvisibleWhenOff)   /* XmToggleButton */
motif_RESOURCE (XmNvisual)   /* Shell */
motif_RESOURCE (XmNvisualPolicy)   /* XmScrolledWindow */
motif_RESOURCE (XmNwaitForWm)   /* WMShell */
motif_RESOURCE (XmNwidth)   /* Core */
motif_RESOURCE (XmNwidthInc)   /* WMShell */
motif_RESOURCE (XmNwindowGroup)   /* WMShell */
motif_RESOURCE (XmNwinGravity)   /* WMShell */
motif_RESOURCE (XmNwmTimeout)   /* WMShell */
motif_RESOURCE (XmNwordWrap)   /* XmText Output */
motif_RESOURCE (XmNworkWindow)   /* XmScrolledWindow */
motif_RESOURCE (XmNx)   /* Core */
motif_RESOURCE (XmNy)   /* Core */

/* End of file motifEmulator_resources.h */
