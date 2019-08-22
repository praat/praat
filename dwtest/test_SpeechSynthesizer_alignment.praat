# test_SpeechSynthesizer_alignment.praat
# djmw 20180821

appendInfoLine: "test_SpeechSynthesizer_alignment.praat"

synthesizer = Create SpeechSynthesizer: "English (Great Britain)", "Female1"
Speech output settings: 44100, 0.01, 1, 1, 130, "IPA" 
To Sound: "This is some text", "yes"
sound1 = selected ("Sound")
textgrid1 = selected ("TextGrid")

selectObject: synthesizer
Speech output settings: 44100, 0.01, 1, 1, 180, "IPA"
To Sound: "This is some text", "yes"
sound2 = selected ("Sound")
textgrid2 = selected ("TextGrid")

asserterror The domains of the Sound and the TextGrid must be equal
selectObject: synthesizer, sound1, textgrid2
To TextGrid (align): 1, 1, 1, -35, 0.1, 0.1

asserterror The interval range end number should not be smaller than the interval range start number
selectObject: synthesizer, sound1, textgrid1
To TextGrid (align): 1, 2, 1, -35, 0.1, 0.1

asserterror The specified interval range end number (3) exceeds the number of intervals (1) in this tier
selectObject: synthesizer, sound1, textgrid1
To TextGrid (align): 1, 1, 3, -35, 0.1, 0.1

selectObject: synthesizer, sound1, textgrid1
tg = To TextGrid (align): 1, 1, 1, -40, 0.1, 0.1


removeObject: textgrid2, sound2, textgrid1, sound1, tg, synthesizer
appendInfoLine: "test_SpeechSynthesizer_alignment.praat OK"
