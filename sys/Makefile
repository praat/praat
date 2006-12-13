# Makefile of the library "sys"
# Paul Boersma, December 10, 2006

include ../makefile.defs
CFLAGS = -I ../dwsys -I ../GSL

OBJECTS = NUM.o NUMarrays.o NUMrandom.o NUMsort.o NUMear.o \
   enum.o abcio.o lispio.o longchar.o complex.o \
   melder_ftoa.o melder_atof.o melder_error.o melder_alloc.o melder.o melder_strings.o \
   melder_token.o melder_files.o melder_audio.o melder_audiofiles.o \
   melder_debug.o melder_sysenv.o melder_info.o \
   Thing.o Data.o Simple.o Collection.o Strings.o \
   Graphics.o Graphics_linesAndAreas.o Graphics_text.o Graphics_colour.o \
   Graphics_image.o Graphics_mouse.o Graphics_record.o \
   Graphics_utils.o Graphics_grey.o Graphics_altitude.o \
   GraphicsPostscript.o Graphics_surface.o \
   ManPage.o ManPages.o Script.o machine.o \
   GraphicsScreen.o Printer.o motif.o \
   Preferences.o site.o \
   Picture.o Ui.o UiFile.o Editor.o DataEditor.o HyperPage.o Manual.o TextEditor.o \
   praat.o praat_actions.o praat_menuCommands.o praat_picture.o sendpraat.o sendsocket.o \
   praat_script.o praat_statistics.o praat_logo.o \
   praat_objectMenus.o InfoEditor.o ScriptEditor.o ButtonEditor.o Interpreter.o Formula.o \
   motifEmulator.o GuiText.o GuiWindow.o

all: libsys.a
libsys.a: $(OBJECTS)
	touch libsys.a
	rm libsys.a
	ar cq libsys.a $(OBJECTS)
	$(RANLIB) libsys.a

$(OBJECTS): *.h ../dwsys/*.h
