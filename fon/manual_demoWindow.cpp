/* manual_demoWindow.cpp
 *
 * Copyright (C) 1992-2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

#include "ManPagesM.h"

void manual_demoWindow_init (ManPages me);
void manual_demoWindow_init (ManPages me) {

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Demo window"
© Paul Boersma 2009–2023

The Demo window is a window in which you can draw and ask for user input.
You can use it for demonstrations, presentations, simulations, adaptive listening experiments,
and stand-alone programs (for the latter, see @@Scripting 9. Turning a script into a stand-alone program@).

, @@Demo window 1. My first Demo window script@ (@`demo`)
, @@Demo window 2. Getting user input@ (@`demoWaitForInput`, @`demoClicked`, @`demoKeyPressed`, @`demoKey$`)
, @@Demo window 3. Getting click locations@ (@`demoX`, @`demoY`, @`demoClickedIn`)
, @@Demo window 4. Full-screen viewing@
, @@Demo window 5. Asynchronous play@ (@`asynchronous`)
, @@Demo window 6. Animation@ (@`demoShow`, @`sleep`, @`demoPeekInput`)
, @@Demo window 7. Miscellaneous@ (@`demoShiftKeyPressed`, @`demoWindowTitle`)
, @@Demo window 8. Tips and Tricks@

################################################################################
"Demo window 1. My first Demo window script"
© Paul Boersma 2009–2023

The Demo window is Praat’s least visible window: you can create it only through a script.
Try the following script after selecting a Sound object:
{;
	\#`{demo} \@{Sound: ||Draw:} 0, 3, -1, 1, “yes”, “curve”
}
You see the Demo window turn up on the screen, with the Sound painted into it.
It works because the @@Sound: ||Draw...@ command is available in the Objects window when you select a Sound. Then try:
{;
	\#`{demo} \@{Draw line:} 0, -1, 3, 1
}
You see a line drawn from (0 seconds, -1 Pa) to (3 seconds, +1 Pascal) in the waveform.
It works because the @@Draw line...@ command is available in the Picture window. Then try:
{;
	\#`{demo} \@{Erase all}
	\#`{demo} \@{Red}
	\#`{demo} \@{Axes:} 0, 100, 0, 100
	\#`{demo} \@{Text:} 50, “centre”, 50, “half”, “Hello”
}
You see a text appear in red, in the centre of the window.
This works because you are using commands from the Picture window, including the @@Axes...@ command,
which sets the world coordinates to something else than before (before, the world coordinates were determined by the Sound).

Now suppose you want the Sound to appear in the top half of the window,
and some texts in the bottom left and bottom right corners of the window.
You can use @@Select outer viewport...@ and @@Select inner viewport...@,
if you know that the size of the Demo window is “100” horizontally and “100” vertically (rather than 12\xx12, as the @@Picture window@),
and that the point (0, 0) lies in the bottom left (rather than the top left, as in the Picture window):
{;
	\#`{demo} \@{Erase all}
	\#`{demo} \@{Black}
	\#`{demo} \@{Times}
	\#`{demo} \@{24}
	\#`{demo} \#@{Select outer viewport:} 0, 100, 50, 100
	\#`{demo} \@{Sound: ||Draw:} 0, 0, 0, 0, “yes”, “curve”
	\#`{demo} \#@{Select inner viewport:} 0, 100, 0, 100
	\#`{demo} \@{Axes:} 0, 10, 0, 10
	\#`{demo} \@{Text:} 0, “left”, 0, “bottom”, “Left bottom corner”
	\#`{demo} \@{Text:} 10, “right”, 0, “bottom”, “Right bottom corner”
}
As the title page of a presentation, you could do:
{;
	\#`{demo} \@{Erase all}
	\#`{demo} \@{Select inner viewport:} 0, 100, 0, 100
	\#`{demo} \@{Axes:} 0, 100, 0, 100
	\#`{demo} \@{Paint rectangle:} “purple”, 0, 100, 0, 100
	\#`{demo} \@{Pink}
	\#`{demo} \@{Text:} 50, “centre”, 50, “half”, “This is my title”
}
################################################################################
"Demo window 2. Getting user input"
© Paul Boersma 2009–2023

For almost all applications, you will want the user (or the participant in an experiment) to be able to click on things in the Demo window,
or to control the Demo window by pressing keys. Here is a presentation with two screens:
{;
	\`{demo} \@{Erase all}
	\`{demo} \@{Select inner viewport:} 0, 100, 0, 100
	\`{demo} \@{Axes:} 0, 100, 0, 100
	\`{demo} \@{Paint rectangle:} “purple”, 0, 100, 0, 100
	\`{demo} \@{Pink}
	\`{demo} \@{Text:} 50, “centre”, 50, “half”, “This is the first page”
	\`{demoWaitForInput} ()
	\`{demo} \@{Erase all}
	\`{demo} \@{Paint rectangle:} “purple”, 0, 100, 0, 100
	\`{demo} \@{Text:} 50, “centre”, 50, “half”, “This is the second page”
}
In this example, you go from the first to the second screen either by clicking with the mouse or by pressing any key.
You will usually want to be more selective in your choice of user actions to respond to.
The function @`demoWaitForInput` always returns 1, so that you can use it nicely in a loop, in which you can react selectively:
{;
	\`{label} FIRST_SCREEN
	\`{demo} \@{Erase all}
	\`{demo} \@{Black}
	\`{demo} \@{Times}
	\`{demo} \@{24}
	\`{demo} \@{Select inner viewport:} 0, 100, 0, 100
	\`{demo} \@{Axes:} 0, 100, 0, 100
	\`{demo} \@{Paint rectangle:} “purple”, 0, 100, 0, 100
	\`{demo} \@{Pink}
	\`{demo} \@{Text:} 50, “centre”, 50, “half”, “This is the first page”
	while \#`{demoWaitForInput} ()
		if \`{demoClicked} ()
			goto SECOND_SCREEN
		elsif \`{demoKeyPressed} ()
			if \`{demoKey$} () = “→” or \`{demoKey$} () = “ ”
				goto SECOND_SCREEN
			endif
		endif
	endwhile
	\`{label} SECOND_SCREEN
	\`{demo} \@{Erase all}
	\`{demo} \@{Paint rectangle:} “purple”, 0, 100, 0, 100
	\`{demo} \@{Text:} 50, “centre”, 50, “half”, “This is the second page”
	while \#`{demoWaitForInput} ()
		if \`{demoClicked} ()
			goto END
		elsif \`{demoKeyPressed} ()
			if \`{demoKey$} () = “←”
				goto FIRST_SCREEN
			elsif \`{demoKey$} () = “→” or \`{demoKey$} () = “ ”
				goto END
			endif
		endif
	endwhile
	\`{label} END
}
This script allows you to use the arrow keys and the space bar to navigate between the two screens. A shorter version is:
{;
	\`{label} FIRST_SCREEN
	\#`{demo} \@{Erase all}
	\#`{demo} \@{Black}
	\#`{demo} \@{Times}
	\#`{demo} \@{24}
	\#`{demo} \@{Select inner viewport:} 0, 100, 0, 100
	\#`{demo} \@{Axes:} 0, 100, 0, 100
	\#`{demo} \@{Paint rectangle:} \"purple\", 0, 100, 0, 100
	\#`{demo} \@{Pink}
	\#`{demo} \@{Text:} 50, \"centre\", 50, \"half\", \"This is the first page\"
	while \#`{demoWaitForInput} ()
		goto SECOND_SCREEN \#`{demoInput} (“•→ ”)
	endwhile
	\`{label} SECOND_SCREEN
	\#`{demo} \@{Erase all}
	\#`{demo} \@{Paint rectangle:} \"purple\", 0, 100, 0, 100
	\#`{demo} \@{Text:} 50, \"centre\", 50, \"half\", \"This is the second page\"
	while \#`{demoWaitForInput} ()
		goto END \#`{demoInput} (“•→ ”)
		goto FIRST_SCREEN \#`{demoInput} (“←”)
	endwhile
	\`{label} END
}
This uses two tricks, namely the possibility of following the #`goto` statement by a condition
and using @`demoInput` to quickly test for multiple possible inputs (the bullet represents a mouse click).

################################################################################
"Demo window 3. Getting click locations"
© Paul Boersma 2009-2023

You can use the functions @`demoX` and @`demoY` to see where the user has clicked.
These functions respond in world coordinates. To see whether the user has clicked in the sound that occupies the
upper half of the screen in the above example, you do for instance
{;
	while \`{demoWaitForInput} ()
		if \`{demoClicked} ()
			\`{demo} Select outer viewport: 0, 100, 50, 100
			\`{demo} Axes: 0, 3, -1, 1
			if \#`{demoX} () >= 0 and \#`{demoX} () < 3 and \#`{demoY} () >= -1 and \#`{demoY} () < 1
}
The last line can be shortened to:
{;
			if \#`{demoClickedIn} (0, 3, -1, 1)
}
Another example of when you want to know the click location is when you test for a click on a button
that you drew on the screen:
{;
	\#`{demo} Paint rounded rectangle: “pink”, 30, 70, 16, 24
	\#`{demo} Text: 50, “centre”, 20, “half”, “Analyse”
	while \#`{demoWaitForInput} ()
		goto ANALYSE \#`{demoClickedIn} (30, 70, 16, 24)
}
################################################################################
"Demo window 4. Full-screen viewing"
© Paul Boersma 2009–2023

When you click in the “zoom box” (the green button in the title bar of the Demo window on the Mac),
the Demo window will zoom out very strongly: it will fill up the whole screen. The menu bar becomes invisible,
although you can still make it temporarily visible and accessible by moving the mouse to the upper edge of the screen.
The Dock also becomes invisible, although you can make it temporarily visible and accessible by moving the mouse to the edge
of the screen (the left, bottom, or right edge, depending on where your Dock normally is).
When you click the zoom box again, the Demo window is restored to its original size.
See also @@Demo window 8. ||Tips and Tricks@.

################################################################################
"Demo window 5. Asynchronous play"
© Paul Boersma 2009–2023

If you select a Sound and execute the command
{;
	\@{Sound: ||Play}
}
Praat will play the whole sound before proceeding to the next line of your script.
You will often instead want Praat to continue running your script while the sound is playing.
To accomplish that, use the @`asynchronous` directive:
{;
	\@{Create Sound as pure tone:} “tone”, 1, 0, 0.2, 44100, 440, 0.2, 0.01, 0.01
	\#`{asynchronous} \@{Sound: ||Play}
	\@{Remove}
}
The sound will continue to play, even after the Sound object has been removed.

Please note that a following Play command will interrupt the playing of the first:
{;
	while \`{demoWaitForInput} ()
		if \`{demoClicked} ()
			\@{Create Sound as pure tone:} “tone”, 1, 0, 3.0, 44100,
			... \`{randomGauss} (440, 100), 0.2, 0.01, 0.01
			\#`{asynchronous} \@{Sound: ||Play}
			\@{Remove}
		endif
	endwhile
}
The first sound will stop playing soon after the user clicks for the second time.

################################################################################
"Demo window 6. Animation"
© Paul Boersma 2009–2023

In the above examples, things will often get drawn to the screen with some delay,
i.e., you may not see the erasures and paintings happen. This is because several operating systems
use %buffering of graphics. These systems will draw the graphics only just before getting user input.
This means that @`demoWaitForInput` is the place where your drawings will typically be painted on the screen.
If you want painting to happen earlier (e.g. in animations), you can use
{;
	\#`{demoShow} ()
}
Also in animations, you will often want to regulate the time span between two consecutive drawings.
If you want 0.05 seconds between drawings, you can put Praat to sleep temporarily with
{;
	\#`{sleep} (0.05)
}
If you need user input during the animation, you can replace @`demoWaitForInput` or #`demoShow` with
{;
	\#`{demoPeekInput}()
}
which returns immediately without waiting and will tell you (via e.g. @`demoClicked` or @`demoKey$`)
whether a mouse or key event happened during drawing or sleeping.

################################################################################
"Demo window 7. Miscellaneous"
© Paul Boersma 2009–2023

To see whether any function keys are pressed (during a mouse click or key press),
you can use @`demoShiftKeyPressed` (), @`demoCommandKeyPressed` (), and @`demoOptionKeyPressed` ().

To put some text in the title bar of the Demo window, try
{;
	\`{demoWindowTitle}: “This is the title of my presentation”
}
################################################################################
"Demo window 8. Tips and Tricks"
© Paul Boersma 2009–2023

The initial size of the Demo window when you start it up is 1344\xx756 pixels,
which is 70 percent of a standard wide screen (1920\xx1080 pixels).
This means that if a font looks good at a size of 35 in the initial Demo window,
the font will look equally good at a size of 50 when you use a 1920\xx1080 video projector full-screen.

If you resize the Demo window with the handle in the bottom left, or if you zoom the window out to the full screen,
you may see that the relative positions of the contents of the window will change. Also, clicking on buttons and in parts
of the window may yield unexpected %x and %y values. It is therefore advisable to resize the window only if you are on a page
that you can get out of by pressing a key, or by clicking anywhere in the window without using @`demoX`, @`demoY` or @`demoClickedIn`.

If you click away the Demo window while it is waiting for input, you get a message saying “You interrupted the script...”.
If you do not want to see this message, you should make sure that the user can reach the end of the script, for instance by
pressing the \-> key on the last page. To make sure the user sees that the script has ended, you could end it with #`demo Erase all`.

Your demo can save its preferences in a folder of your choice,
e.g. in #`preferencesDirectory$ + “/../GuineaPigAnalyzer”` if your demo is called GuineaPigAnalyzer.
If you want to be less conspicuous and like to use the Praat preferences folder instead,
please use the #`apps` subfolder, in this way:
{;
	\`{createFolder}: preferencesDirectory$ + “/apps”
	\`{createFolder}: preferencesDirectory$ + “/apps/GuineaPigAnalyzer”
}
################################################################################
)~~~"
MAN_PAGES_END

}

/* End of file manual_demoWindow.cpp */
