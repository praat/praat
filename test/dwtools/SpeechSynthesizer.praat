my.SpeechSynthesizer = Create SpeechSynthesizer: "French (France)", "Female1"
text$ = "voici les clés de ton bonheur"
phonemes$ = Get phonemes from text: text$
writeInfo: length (text$), " ", length (phonemes$), " ", phonemes$
assert length (text$) = 29
assert length (phonemes$) = 27
assert phonemes$ = "vwasi le- kle də- tɔ̃ bɔnœʁ"
Remove

my.SpeechSynthesizer = Create SpeechSynthesizer: "Hindi", "Female1"
phonemes$ = Get phonemes from text: "अवग्रहः"
writeInfo: length (text$), " ", length (phonemes$), " ", phonemes$
assert phonemes$ = "ʌʋʌɡɾəhəh"
Remove

