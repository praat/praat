/* manual_statistics.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2005/04/27 GPL
 */

#include "ManPagesM.h"

void manual_statistics_init (ManPages me);
void manual_statistics_init (ManPages me) {

MAN_BEGIN ("Statistics", "ppgb", 20050427)
INTRO ("This is the tutorial about basic statistical techniques in Praat, which work "
	"with the @Table object. It assumes that you are familiar with the @Intro.")
NORMAL ("(Under construction..................)")
LIST_ITEM ("\\bu @@Logistic regression@")
NORMAL ("For more sophisticated techniques, see:")
LIST_ITEM ("\\bu @@Principal component analysis@")
LIST_ITEM ("\\bu @@Multidimensional scaling@")
LIST_ITEM ("\\bu @@Discriminant analysis@")
MAN_END

MAN_BEGIN ("Logistic regression", "ppgb", 20050427)
INTRO ("This page explains how you do logistic regression with Praat. "
	"You start by writing a table in a text file. "
	"The following example contains natural stimuli (female speaker) with measured F1 and duration values, "
	"and the responses of a certain listener who is presented each stimulus 10 times.")
CODE1 (" F1    Dur   /ae/  /E/")
CODE1 (" 764    87    2     8")
CODE1 (" 674   104    3     7")
CODE1 (" 574   126    0    10")
CODE1 (" 566    93    1     9")
CODE1 (" 618   118    1     9")
CODE1 ("1025   147   10     0")
CODE1 (" 722   117    7     3")
CODE1 (" 696   169    9     1")
CODE1 ("1024   124   10     0")
CODE1 (" 752    92    6     4")
NORMAL ("In this table we see 10 different stimuli, each characterized by a certain combination "
	"of the independent variables %F1 (first formant in Hertz) and %Dur (duration in milliseconds). "
	"The first row of the table means that there was a stimulus with an F1 of 764 Hz and a duration of 87 ms, "
	"and that the listener responded to this stimulus 2 times with the response category /\\ae/, "
	"and the remaining 8 times with the category /\\ep/.")
NORMAL ("A table as above can be typed into a text file. The columns can be separated with spaces and/or tab stops. "
	"The file can be read into Praat with ##Read Table from table file...#. "
	"The command \"To logistic regression\" will become available in the #Statistics menu.")
ENTRY ("What does it do?")
NORMAL ("The logistic regression method will find values %\\al, %%\\be__F1_% and %%\\be__dur_% "
	"that optimize")
FORMULA ("%\\al + %%\\be__F1_% %F1__%k_ + %%\\be__dur_% %Dur__%k_ = ln (%p__%k_(/\\ep/)/%p__%k_(/\\ae/))")
NORMAL ("where %k runs from 1 to 10, and %p__%k_(/\\ae/) + %p__%k_(/\\ep/) = 1.")
NORMAL ("The optimization criterion is %%maximum likelihood%, i.e. those %\\al, %%\\be__F1_% and %%\\be__dur_% "
	"will be chosen that lead to values for %p__%k_(/\\ae/) and %p__%k_(/\\ep/) that make the observations in the table "
	"most likely.")
NORMAL ("Praat will create an object of type #LogisticRegression in the list. "
	"When you then click the #Info button, Praat will write the values of %\\al (the %intercept), "
	"%%\\be__F1_% and %%\\be__dur_% into the Info window.")
NORMAL ("The number of independent variables does not have to be 2; it can be 1 or more. "
	"The number of dependent categories is always 2, and they should always be put in the last two columns of the table.")
MAN_END

}

/* End of file manual_statistics.c */
