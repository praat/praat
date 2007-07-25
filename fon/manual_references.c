/* manual_references.c
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

#include "ManPagesM.h"

void manual_references_init (ManPages me);
void manual_references_init (ManPages me) {

MAN_BEGIN ("Archangeli & Pulleyblank (1994)", "ppgb", 19971021)
NORMAL ("Diana Archangeli & Douglas Pulleyblank (1994): %%Grounded Phonology%. "
	"Cambridge, Mass.: MIT Press.")
MAN_END

MAN_BEGIN ("Boersma (1993)", "ppgb", 20030312)
NORMAL ("Paul Boersma (1993): \"Accurate short-term analysis of the fundamental frequency "
	"and the harmonics-to-noise ratio of a sampled sound.\" "
	"%%Proceedings of the Institute of Phonetic Sciences% #17: 97\\--110. University of Amsterdam.")
NORMAL ("Can be downloaded as a PDF file from http://fon.hum.uva.nl/paul/")
MAN_END

MAN_BEGIN ("Boersma (1997)", "ppgb", 19981219)
NORMAL ("Paul Boersma (1997): \"How we learn variation, optionality, and probability.\" "
	"%%Proceedings of the Institute of Phonetic Sciences% #21: 43\\--58. University of Amsterdam.")
NORMAL ("Available from http://www.fon.hum.uva.nl/paul/. Equals chapter 15 of @@Boersma (1998)@.")
NORMAL ("A less correct version (demoting and promoting a single pair of constraints, instead of them all) "
	"is available as Rutgers Optimality Archive #221, http://ruccs.rutgers.edu/roa.html")
MAN_END

MAN_BEGIN ("Boersma (1998)", "ppgb", 20041020)
NORMAL ("Paul Boersma (1998): %%Functional Phonology% [%%LOT International Series% ##11#]. "
	"The Hague: Holland Academic Graphics. Pages i-ix, 1-493. [Doctoral thesis, University of Amsterdam]")
NORMAL ("This book can be downloaded as a PDF file from ##http://www.fon.hum.uva.nl/paul/#, "
	"where you can also find many Praat scripts for the simulations and pictures in this book. "
	"A paperback version is also available from the author (paul.boersma\\@ uva.nl).")
MAN_END

MAN_BEGIN ("Boersma (2000)", "ppgb", 20001027)
NORMAL ("Paul Boersma (2000): \"Learning a grammar in Functional Phonology.\" "
	"In Joost Dekkers, Frank van der Leeuw, & Jeroen van de Weijer (eds.): "
	"%%Phonology, Syntax, and Acquisition in Optimality Theory%. Oxford University Press.")
NORMAL ("An extended version is chapter 14 of @@Boersma (1998)@.")
MAN_END

MAN_BEGIN ("Boersma & Hayes (2001)", "ppgb", 20020511)
NORMAL ("Paul Boersma & Bruce Hayes (2001): \"Empirical tests of the Gradual Learning Algorithm.\" "
	"%%Linguistic Inquiry% #32: 45\\--86.")
MAN_END

MAN_BEGIN ("Boersma & Kovacic (2006)", "ppgb", 20061203)
NORMAL ("Paul Boersma & Gordana Kovacic (2006): "
	"\"Spectral characteristics of three styles of Croatian folk singing.\" "
	"%%Journal of the Acoustical Society of America% #119: 1805\\--1816.")
MAN_END

MAN_BEGIN ("Childers (1978)", "ppgb", 20030515)
NORMAL ("%%Modern spectrum analysis%, IEEE Press.")
NORMAL ("The Burg algorithm for linear prediction coefficients is described on pages 252-255.")
MAN_END

MAN_BEGIN ("Deliyski (1993)", "ppgb", 20030312)
NORMAL ("Dimitar D. Deliyski: \"Acoustic model and evaluation of pathological voice production.\" "
	"%%Proceedings Eurospeech '93%, Vol. 3, 1969\\--1972.")
MAN_END

MAN_BEGIN ("Escudero & Boersma (2004)", "ppgb", 20050427)
NORMAL ("Paola Escudero & Paul Boersma (2004): \"Bridging the gap between L2 speech perception "
	"and phonological theory.\" %%Studies in Second Language Acquisition% #26: 551\\--585.")
MAN_END

MAN_BEGIN ("Fant (1960)", "ppgb", 19980201)
NORMAL ("Gunnar Fant (1960): %%Acoustic Theory of Speech Production.% Mouton, The Hague.")
MAN_END

MAN_BEGIN ("Flanagan & Landgraf (1968)", "ppgb", 19980201)
NORMAL ("James L. Flanagan & L.L. Landgraf (1968): \"Self-oscillating source for vocal-tract synthesizers\", "
	"%%IEEE Transactions on Audio and Electroacoustics% ##AU-16#: 57-64. "
	"Reprinted in: James L. Flanagan & Lawrence R. Rabiner (eds.) (1973): "
	"%%Speech Synthesis%, Dowden, Hutchinson & Ross, Stroudsburg.")
NORMAL ("The authors show that if we model each vocal cord as a single mass-spring system, "
	"the vocal cords will vibrate passively as a result of the interaction with a glottal airflow.")
MAN_END

MAN_BEGIN ("Hayes & MacEachern (1998)", "ppgb", 19981219)
NORMAL ("Bruce P. Hayes & Margaret MacEachern (1998): \"Quatrain form in English folk verse\", "
	"%Language #74: 473\\--507.")
MAN_END

MAN_BEGIN ("Ishizaka & Flanagan (1972)", "ppgb", 19980201)
NORMAL ("Kenzo Ishizaka & James L. Flanagan (1972): \"Synthesis of voiced sounds from a two-mass model "
	"of the vocal cords\", %%Bell System Technical Journal% #51: 1233-1268. "
	"Reprinted in: James L. Flanagan & Lawrence R. Rabiner (eds.) (1973): "
	"%%Speech Synthesis%, Dowden, Hutchinson & Ross, Stroudsburg.")
NORMAL ("The authors show that if we model each vocal cord as two coupled mass-spring systems, "
	"the passive vibration that results from the interaction with a glottal airflow "
	"will show more realistic behaviour than with the one-mass model of @@Flanagan & Landgraf (1968)@, "
	"at least for a male speaker.")
MAN_END

MAN_BEGIN ("J\\a\"ger (2003)", "ppgb", 20070423)
NORMAL ("Gerhard J\\a\"ger (2003): \"Maximum Entropy Models and Stochastic Optimality Theory.\" "
	"To appear in Jane Grimshaw, Joan Maling, Chris Manning, Jane Simpson, and Annie Zaenen (eds.): "
	"%%Architectures, rules, and preferences: A Festschrift for Joan Bresnan%, "
	"CSLI Publications, Stanford.")
MAN_END

MAN_BEGIN ("Jesteadt, Wier & Green (1977)", "ppgb", 20021215)
NORMAL ("W. Jesteadt, G.C. Wier, & D.M. Green (1977): \"Intensity discrimination as a function "
	"of frequency and sensation level.\" %%Journal of the Acoustical Society of America% #61: 169\\--177.")
MAN_END

MAN_BEGIN ("Klatt & Klatt (1990)", "ppgb", 20050712)
NORMAL ("D.H. Klatt & L.C. Klatt (1990): \"Analysis, synthesis and perception of voice quality "
	"variations among male and female talkers.\" "
	"%%Journal of the Acoustical Society of America% #87: 820\\--856.")
MAN_END

MAN_BEGIN ("Ladefoged (2001)", "ppgb", 20030316)
NORMAL ("Peter Ladefoged (2001). %%Vowels and consonants%: %%an introduction to the sounds of languages%. "
	"Oxford: Blackwell.")
NORMAL ("A very readable introduction to phonetics, mainly acoustic and articulatory. "
	"Has lots of spectrograms of the sounds of the world's languages. Comes with a CD that has all those "
	"sounds and includes training material for transcription (from another book).")
NORMAL ("For a more encyclopaedic treatment of the sounds of the world's languages, "
	"see @@Ladefoged & Maddieson (1996)@ instead.")
MAN_END

MAN_BEGIN ("Ladefoged & Maddieson (1996)", "ppgb", 20030316)
NORMAL ("Peter Ladefoged & Ian Maddieson (1996). %%The sounds of the world's languages%. "
	"Oxford: Blackwell.")
NORMAL ("An extensive reference source for the articulation and acoustics of `all' vowels and consonants "
	"that occur in the world's languages. If you don't find the answer in this book, you will find the "
	"answer in the articles referred to in this book. "
	"The book uses lots of spectrograms, palatograms, and other techniques.")
NORMAL ("The book is not an introductory text. For that, see @@Ladefoged (2001)@ instead.")
MAN_END

MAN_BEGIN ("McCarthy & Prince (1995)", "ppgb", 20021105)
NORMAL ("John J. McCarthy & Alan Prince (1995): Faithfulness and reduplicative identity. "
	"In Jill Beckman, Laura Walsh Dickey & Suzanne Urbanczyk (eds.), %%Papers in Optimality Theory%. "
	"%%University of Massachusetts Occasional Papers% #18. Amherst, Mass.: Graduate Linguistic Student Association. "
	"pp. 249\\--384. [Rutgers Optimality Archive #60, http://roa.rutgers.edu]")
MAN_END

MAN_BEGIN ("Moulines & Charpentier (1990)", "ppgb", 20070722)
NORMAL ("Eric Moulines & Francis Charpentier (1990): Pitch-synchronous waveform processing techniques for text-to-speech synthesis using diphones. "
	"%%Speech Communication% #9: 453\\--467.")
MAN_END

MAN_BEGIN ("Patterson & Wightman (1976)", "ppgb", 20021215)
NORMAL ("R. Patterson & F. Wightman (1976): \"Residue pitch as a function of component spacing.\" "
	"%%Journal of the Acoustical Society of America% #59: 1450\\--1459.")
MAN_END

MAN_BEGIN ("Plomp (1967)", "ppgb", 20021215)
NORMAL ("Reinier Plomp (1967): \"Frequencies dominant in the perception of pitch of complex sounds.\" "
	"%%Journal of the Acoustical Society of America% #42: 191\\--198.")
MAN_END

MAN_BEGIN ("Prince & Smolensky (1993)", "ppgb", 19971021)
NORMAL ("Alan Prince & Paul Smolensky (1993): %%Optimality Theory: Constraint Interaction in Generative Grammar.% "
	"Rutgers University Center for Cognitive Science Technical Report #2.")
MAN_END

MAN_BEGIN ("Rosenberg (1971)", "ppgb", 20050712)
NORMAL ("A. Rosenberg (1971): \"Effect of glottal pulse shape on the quality of natural vowels.\" "
	"%%Journal of the Acoustical Society of America% #49: 583\\--590.")
MAN_END

MAN_BEGIN ("Rosenblatt (1962)", "ppgb", 20070423)
NORMAL ("Frank Rosenblatt (1962): "
	"%%Principles of neurodynamics; perceptrons and the theory of brain mechanisms%. Washington: Spartan Books.")
MAN_END

MAN_BEGIN ("Smolensky & Legendre (2006)", "ppgb", 20070423)
NORMAL ("Paul Smolensky & G\\e'raldine Legendre (1993): %%The harmonic mind.% "
	"MIT Press.")
NORMAL ("These two books contain all material on connectionism by Smolensky and colleagues from the late eighties on, "
	"with extensive editing to make it into a conherent work, plus much new material.")
NORMAL ("This book can be seen in many respects as the predecessor of @@Prince & Smolensky (1993)@.")
MAN_END

MAN_BEGIN ("Soderstrom, Mathis & Smolensky (2006)", "ppgb", 20070423)
NORMAL ("Melanie Soderstrom, Donald Mathis & Paul Smolensky (2006): "
	"\"Abstract genomic encoding of Universl Grammar in Optimality Theory\". "
	"In @@Smolensky & Legendre (2006)@, pp. 403\\--471.")
MAN_END

MAN_BEGIN ("Tesar & Smolensky (1998)", "ppgb", 19991019)
NORMAL ("Bruce Tesar & Paul Smolensky (1998): \"Learnability in Optimality Theory.\" "
	"%%Linguistic Inquiry% #29: 229\\--268.")
NORMAL ("The first version of the constraint-demotion algorithm appeared in:")
NORMAL ("Bruce Tesar & Paul Smolensky (1993): \"The learnability of Optimality Theory: "
	"an algorithm and some basic complexity results\", "
	"ms. Department of Computer Science & Institute of Cognitive Science, University of Colorado at Boulder. "
	"Available as Rutgers Optimality Archive #2, http://ruccs.rutgers.edu/roa.html")
NORMAL ("The Error-Driven Constraint Demotion algorithm can be found in:")
NORMAL ("Bruce Tesar & Paul Smolensky (1996): \"Learnability in Optimality Theory (long version)\". "
 	"Technical Report ##96-3#, Department of Cognitive Science, Johns Hopkins University, Baltimore. "
	"Available as Rutgers Optimality Archive #156, http://ruccs.rutgers.edu/roa.html")
MAN_END

MAN_BEGIN ("Willems (1986)", "ppgb", 20021215)
NORMAL ("Lei Willems (1986): \"Robust formant analysis.\" %%IPO report% #529: 1\\--25. "
	"Eindhoven: Institute for Perception Research.")
MAN_END

}

/* End of file manual_references.c */
