form Test command line calls
	sentence First_text I love you
	real Beep_duration 0.4
	sentence Second_text Me too
endform

writeInfoLine: "She: """, first_text$, """"
appendInfoLine: "He: """, second_text$, """"

synth1 = Create SpeechSynthesizer: "English (Great Britain)", "Female1"
Play text: first_text$
Create Sound as pure tone: "beep", 1, 0.0, beep_duration,
... 44100, 440, 0.2, 0.01, 0.01
Play
Remove
synth2 = Create SpeechSynthesizer: "English (America)", "Male1"
Play text: second_text$

exit

Praat.exe c:\Users\pboersma\Desktop\testCommandLineCalls.praat "I love you" 0.4 Me too

Praat.exe c:\Users\pboersma\Desktop\testCommandLineCalls.praat "I love you" 0.4 "Me too"

import os
os.system('Praat.exe c:\\Users\\pboersma\\Desktop\\testCommandLineCalls.praat "I love you" 0.4 Me too')

import subprocess
subprocess.call('Praat.exe c:\\Users\\pboersma\\Desktop\\testCommandLineCalls.praat "I love you" 0.4 Me too')

subprocess.call(['Praat.exe', 'c:\\Users\\pboersma\\Desktop\\testCommandLineCalls.praat', 'I love you', '0.4', 'Me too'])

subprocess.call(['Praat.exe', 'c:\\Users\\pboersma\\Desktop\\testCommandLineCalls.praat', 'I love you', '0.4', 'Me', 'too'])exit
