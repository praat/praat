/*
 * Copyright (C) 2005 to 2015 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2015-2018 Reece H. Dunn
 * Copyright (C) 2018 Juho Hiltunen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#ifndef ESPEAK_NG_DICTIONARY_H
#define ESPEAK_NG_DICTIONARY_H

#include "espeak_ng.h"       // for ESPEAK_NG_API
#include "phoneme.h"                   // for PHONEME_TAB
#include "synthesize.h"                // for PHONEME_LIST
#include "translate.h"                 // for Translator, WORD_TAB

#ifdef __cplusplus
extern "C"
{
#endif

extern ESPEAK_NG_API void strncpy0(char *to, const char *from, int size);
int LoadDictionary(Translator *tr, const char *name, int no_error);
int HashDictionary(const char *string);
const char *EncodePhonemes(const char *p, char *outptr, int *bad_phoneme);
void DecodePhonemes(const char *inptr, char *outptr);
char *WritePhMnemonic(char *phon_out, PHONEME_TAB *ph, PHONEME_LIST *plist, int use_ipa, int *flags);
const char *GetTranslatedPhonemeString(int phoneme_mode);
int IsVowel(Translator *tr, int letter);
int Unpronouncable(Translator *tr, char *word, int posn);
void ChangeWordStress(Translator *tr, char *word, int new_stress);
void SetWordStress(Translator *tr, char *output, unsigned int *dictionary_flags, int tonic, int control);
void AppendPhonemes(Translator *tr, char *string, int size, const char *ph);
int TranslateRules(Translator *tr, char *p_start, char *phonemes, int ph_size, char *end_phonemes, int word_flags, unsigned int *dict_flags);
int TransposeAlphabet(Translator *tr, char *text);
int Lookup(Translator *tr, const char *word, char *ph_out);
int LookupDictList(Translator *tr, char **wordptr, char *ph_out, unsigned int *flags, int end_flags, WORD_TAB *wtab);
int LookupFlags(Translator *tr, const char *word, unsigned int **flags_out);
int RemoveEnding(Translator *tr, char *word, int end_type, char *word_copy);

#ifdef __cplusplus
}
#endif

#endif

