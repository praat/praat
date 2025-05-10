# meson.build of the library "sys"
# David Weenink & Paul Boersma, 28 April 2025

sources = '''
	ButtonEditor.cpp
	Collection.cpp Data.cpp DataGui.cpp DataEditor.cpp DemoEditor.cpp
	Editor.cpp
	Formula.cpp
	Graphics.cpp Graphics_linesAndAreas.cpp Graphics_text.cpp
	Graphics_colour.cpp Graphics_image.cpp Graphics_record.cpp
	Graphics_utils.cpp Graphics_grey.cpp Graphics_altitude.cpp
	GraphicsPostscript.cpp GraphicsScreen.cpp Graphics_surface.cpp
	Gui.cpp GuiButton.cpp GuiCheckButton.cpp GuiControl.cpp GuiDialog.cpp GuiDrawingArea.cpp GuiFileSelect.cpp GuiForm.cpp GuiLabel.cpp GuiList.cpp GuiMenu.cpp GuiMenuItem.cpp Gui_messages.cpp GuiObject.cpp GuiOptionMenu.cpp GuiProgressBar.cpp GuiRadioButton.cpp GuiScale.cpp GuiScrollBar.cpp GuiScrolledWindow.cpp GuiShell.cpp GuiText.cpp GuiThing.cpp GuiWindow.cpp
	GuiTrust.cpp
	HyperPage.cpp InfoEditor.cpp Interpreter.cpp
	machine.cpp
	ManPage.cpp ManPages.cpp ManPages_toHtml.cpp Manual.cpp  motifEmulator.cpp
	Notebook.cpp NotebookEditor.cpp
	Picture.cpp praat.cpp praat_actions.cpp praat_library.cpp praat_logo.cpp praat_menuCommands.cpp praat_objectMenus.cpp praat_picture.cpp praat_script.cpp praat_statistics.cpp
	Preferences.cpp Printer.cpp
	ScriptEditor.cpp StringsEditor.cpp
	Script.cpp Simple.cpp site.cpp Strings.cpp
	TextEditor.cpp Thing.cpp
	Ui.cpp UiFile.cpp UiPause.cpp'''.split()

libsys = static_library (
	'sys',
	sources : sources,
	dependencies : gtk_dep,
	include_directories : [melder_inc, dwsys_inc]
)

libsys_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libsys
)
