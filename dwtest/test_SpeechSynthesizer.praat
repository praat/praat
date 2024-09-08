# test_SpeechSynthesizer.praat
# djmw 20120130, 20120522, 20160524, 20171103
# ppgb 20240908

appendInfoLine: "test_SpeechSynthesizer.praat"

voiceList = Tabulate SpeechSynthesizer voice properties
numberOfVoices = Get number of rows

appendInfoLine: tab$, "Languages:"
languageList = Tabulate SpeechSynthesizer language properties
numberOfLanguages = Get number of rows

numberOfSounds = 0
for ilang to numberOfLanguages
	selectObject: languageList
	language$ = Get value: ilang, "language name"
	appendInfo: tab$, tab$, language$, ":"
	for ivoice to 3
		selectObject: voiceList
		voice$ = Get value: randomInteger (1, numberOfVoices), "voice name"
		appendInfo:  " ", voice$
		ss = Create SpeechSynthesizer: language$, voice$
		sound = To Sound: "a e u", "no"
		;Play
		removeObject: ss, sound
		numberOfSounds += 1
	endfor
	appendInfo: newline$
endfor
appendInfoLine: tab$, tab$, numberOfLanguages, " languages, ", numberOfSounds, " sounds created/removed"
appendInfoLine: tab$, "Languages: OK"

appendInfoLine: tab$, "Writing and reading:"
ss = Create SpeechSynthesizer: language$, voice$
Save as text file: "kanweg.SpeechSynthesizer"
ss2 = Read from file: "kanweg.SpeechSynthesizer"
appendInfoLine: tab$, "Writing and reading: OK"

appendInfoLine: tab$, "Compatibility:"

Create SpeechSynthesizer: "Aragonese", "Andrea"
languageName$ = Get language name
voiceName$ = Get voice name
phonemeSetName$ = Get phoneme set name
assert languageName$ = "Aragonese"
assert voiceName$ = "Andrea"
assert phonemeSetName$ = "Aragonese"
Remove

Create SpeechSynthesizer: "armenian (West Armenia)", "Shelby"
languageName$ = Get language name
voiceName$ = Get voice name
phonemeSetName$ = Get phoneme set name
assert languageName$ = "Armenian (West Armenia)"
assert voiceName$ = "Shelby"
assert phonemeSetName$ = "Armenian (West Armenia)"
Remove

Create SpeechSynthesizer: "Assamese", "steph2"
languageName$ = Get language name
voiceName$ = Get voice name
phonemeSetName$ = Get phoneme set name
assert languageName$ = "Assamese"
assert voiceName$ = "Steph2"
assert phonemeSetName$ = "Assamese"
Remove

asserterror Unknown language “ffsd”.'newline$'
...If you think that Praat should know this language, write to the authors
Create SpeechSynthesizer: "ffsd", "steph2"

asserterror Unknown voice “sfdhj”.'newline$'
...If you think that Praat should know this voice, write to the authors
Create SpeechSynthesizer: "Assamese", "sfdhj"

asserterror Unknown voice “sfdhj”.'newline$'
...If you think that Praat should know this voice, write to the authors
Create SpeechSynthesizer... Assamese sfdhj
		# (pre-2014 syntax should also understand "not found")

Create SpeechSynthesizer: "Default", "Andy"
languageName$ = Get language name
voiceName$ = Get voice name
phonemeSetName$ = Get phoneme set name
assert languageName$ = "English (Great Britain)"
assert voiceName$ = "Andy"
assert phonemeSetName$ = "English (Great Britain)"
Remove

Create SpeechSynthesizer: "English", "Alicia"
languageName$ = Get language name
voiceName$ = Get voice name
phonemeSetName$ = Get phoneme set name
assert languageName$ = "English (Great Britain)"
assert voiceName$ = "Alicia"
assert phonemeSetName$ = "English (Great Britain)"
Remove

Create SpeechSynthesizer: "English", "f4"
languageName$ = Get language name
voiceName$ = Get voice name
phonemeSetName$ = Get phoneme set name
assert languageName$ = "English (Great Britain)"
assert voiceName$ = "Female4"
assert phonemeSetName$ = "English (Great Britain)"
Remove

Create SpeechSynthesizer: "English", "default"
languageName$ = Get language name
voiceName$ = Get voice name
phonemeSetName$ = Get phoneme set name
assert languageName$ = "English (Great Britain)"
assert voiceName$ = "Male1"
assert phonemeSetName$ = "English (Great Britain)"
Remove

Create SpeechSynthesizer: "gmw/nl", "Andrea"
languageName$ = Get language name
voiceName$ = Get voice name
phonemeSetName$ = Get phoneme set name
assert languageName$ = "Dutch"
assert voiceName$ = "Andrea"
assert phonemeSetName$ = "Dutch"
Remove

Create SpeechSynthesizer: "de", "Steph2"
languageName$ = Get language name
voiceName$ = Get voice name
phonemeSetName$ = Get phoneme set name
assert languageName$ = "German"
assert voiceName$ = "Steph2"
assert phonemeSetName$ = "German"
Remove

ss3 = Create SpeechSynthesizer: "Default", "default"
ss4 = Create SpeechSynthesizer: "English", "f1"
appendInfoLine: tab$, "Compatibility: OK"

appendInfoLine: tab$, "Get phonemes from text:"
text$ = "This is some text."
phonemes$ = "ðɪs ɪz sʌm tɛkst"
phonemesWithSpaces$ = "ð ɪ s   ɪ z   s ʌ m   t ɛ k s t "
textout$ = Get phonemes from text: text$
assert phonemes$ = textout$
textoutWithSpaces$ = Get phonemes from text (space-separated): text$
assert phonemesWithSpaces$ = textoutWithSpaces$
appendInfoLine: tab$, "Get phonemes from text: OK"

removeObject: voiceList, languageList, ss, ss2,  ss3, ss4

appendInfoLine: "test_SpeechSynthesizer.praat OK"
