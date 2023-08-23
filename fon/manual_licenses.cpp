/* manual_licenses.cpp
 *
 * Copyright (C) 1992-2023 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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
//#include "praat_version.h"

void manual_licenses_init (ManPages me);
void manual_licenses_init (ManPages me) {

MAN_BEGIN (U"FLAC BSD 3-clause license", U"ppgb", 20210823)
NORMAL (U"The Praat source code contains a copy of the FLAC software (see @Acknowledgments). "
	"Here is the FLAC license text:")
CODE (U"libFLAC - Free Lossless Audio Codec library")
CODE (U"Copyright (C) 2000-2009  Josh Coalson")
CODE (U"Copyright (C) 2011-2018  Xiph.Org Foundation")
CODE (U"")
CODE (U"Redistribution and use in source and binary forms, with or without "
	"modification, are permitted provided that the following conditions "
	"are met:")
CODE (U"")
CODE (U"- Redistributions of source code must retain the above copyright "
	"notice, this list of conditions and the following disclaimer.")
CODE (U"")
CODE (U"- Redistributions in binary form must reproduce the above copyright "
	"notice, this list of conditions and the following disclaimer in the "
	"documentation and/or other materials provided with the distribution.")
CODE (U"")
CODE (U"- Neither the name of the Xiph.org Foundation nor the names of its "
	"contributors may be used to endorse or promote products derived from "
	"this software without specific prior written permission.")
CODE (U"")
CODE (U"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
	"``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
	"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
	"A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR "
	"CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
	"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
	"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
	"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
	"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
	"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
	"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")
MAN_END

MAN_BEGIN (U"Ogg Vorbis BSD 3-clause license", U"ppgb", 20201227)
NORMAL (U"The Praat source code contains a copy of the Ogg Vorbis software (see @Acknowledgments). "
	"Here is the Ogg Vorbis license text:")
CODE (U"Copyright (c) 2002-2020 Xiph.org Foundation")
CODE (U"")
CODE (U"Redistribution and use in source and binary forms, with or without "
	"modification, are permitted provided that the following conditions "
	"are met:")
CODE (U"")
CODE (U"- Redistributions of source code must retain the above copyright "
	"notice, this list of conditions and the following disclaimer.")
CODE (U"")
CODE (U"- Redistributions in binary form must reproduce the above copyright "
	"notice, this list of conditions and the following disclaimer in the "
	"documentation and/or other materials provided with the distribution.")
CODE (U"")
CODE (U"- Neither the name of the Xiph.org Foundation nor the names of its "
	"contributors may be used to endorse or promote products derived from "
	"this software without specific prior written permission.")
CODE (U"")
CODE (U"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
	"``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
	"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
	"A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR "
	"CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
	"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
	"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
	"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
	"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
	"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
	"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")
MAN_END

MAN_BEGIN (U"Opus BSD 3-clause license", U"ppgb", 20210105)
NORMAL (U"The Praat source code contains a copy of the Opus software (see @Acknowledgments). "
	"Here is the Opus license text:")
CODE (U"Copyright (c) 2001-2011 Xiph.Org, Skype Limited, Octasic,")
CODE (U"                        Jean-Marc Valin, Timothy B. Terriberry,")
CODE (U"                        CSIRO, Gregory Maxwell, Mark Borgerding,")
CODE (U"                        Erik de Castro Lopo")
CODE (U"")
CODE (U"Redistribution and use in source and binary forms, with or without "
	"modification, are permitted provided that the following conditions "
	"are met:")
CODE (U"")
CODE (U"- Redistributions of source code must retain the above copyright "
	"notice, this list of conditions and the following disclaimer.")
CODE (U"")
CODE (U"- Redistributions in binary form must reproduce the above copyright "
	"notice, this list of conditions and the following disclaimer in the "
	"documentation and/or other materials provided with the distribution.")
CODE (U"")
CODE (U"- Neither the name of Internet Society, IETF or IETF Trust, nor the "
	"names of specific contributors may be used to endorse or promote "
	"products derived from this software without specific prior written "
	"permission.")
CODE (U"")
CODE (U"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
	"``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
	"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
	"A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR "
	"CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
	"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
	"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
	"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
	"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
	"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
	"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")
CODE (U"")
CODE (U"Opus is subject to the royalty-free patent licenses which are "
	"specified at:")
CODE (U"")
CODE (U"Xiph.Org Foundation:")
CODE (U"https://datatracker.ietf.org/ipr/1524/")
CODE (U"")
CODE (U"Microsoft Corporation:")
CODE (U"https://datatracker.ietf.org/ipr/1914/")
CODE (U"")
CODE (U"Broadcom Corporation:")
CODE (U"https://datatracker.ietf.org/ipr/1526/")
MAN_END

MAN_BEGIN (U"Skype Limited BSD 3-clause license", U"ppgb", 20220102)
NORMAL (U"The Praat source code contains a copy of the SILK software (see @Acknowledgments). "
	"Here is the Skype Limited license text:")
CODE (U"Copyright (c) 2006-2011 Skype Limited. All rights reserved.")
CODE (U"")
CODE (U"Redistribution and use in source and binary forms, with or without "
	"modification, are permitted provided that the following conditions "
	"are met:")
CODE (U"")
CODE (U"- Redistributions of source code must retain the above copyright "
	"notice, this list of conditions and the following disclaimer.")
CODE (U"")
CODE (U"- Redistributions in binary form must reproduce the above copyright "
	"notice, this list of conditions and the following disclaimer in the "
	"documentation and/or other materials provided with the distribution.")
CODE (U"")
CODE (U"- Neither the name of Internet Society, IETF or IETF Trust, nor the names of specific "
	"contributors, may be used to endorse or promote products derived from "
	"this software without specific prior written permission.")
CODE (U"")
CODE (U"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
	"``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
	"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
	"A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR "
	"CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
	"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
	"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
	"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
	"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
	"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
	"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")
MAN_END

MAN_BEGIN (U"Unicode Inc. license agreement", U"ppgb", 20220515)
NORMAL (U"The Praat source code contains a copy of the Unicode Character Database, "
	"as well as derived software (see @Acknowledgments). "
	"Here is the Unicode Inc. license text:")
CODE (U"UNICODE, INC. LICENSE AGREEMENT - DATA FILES AND SOFTWARE")
CODE (U"")
CODE (U"See Terms of Use <https://www.unicode.org/copyright.html> "
	"for definitions of Unicode Inc.’s Data Files and Software.")
CODE (U"")
CODE (U"NOTICE TO USER: Carefully read the following legal agreement.")
CODE (U"BY DOWNLOADING, INSTALLING, COPYING OR OTHERWISE USING UNICODE INC.'S "
	"DATA FILES (\"DATA FILES\"), AND/OR SOFTWARE (\"SOFTWARE\"), "
	"YOU UNEQUIVOCALLY ACCEPT, AND AGREE TO BE BOUND BY, ALL OF THE "
	"TERMS AND CONDITIONS OF THIS AGREEMENT.")
CODE (U"IF YOU DO NOT AGREE, DO NOT DOWNLOAD, INSTALL, COPY, DISTRIBUTE OR USE "
	"THE DATA FILES OR SOFTWARE.")
CODE (U"")
CODE (U"COPYRIGHT AND PERMISSION NOTICE")
CODE (U"")
CODE (U"Copyright © 1991-2022 Unicode, Inc. All rights reserved.")
CODE (U"Distributed under the Terms of Use in https://www.unicode.org/copyright.html.")
CODE (U"")
CODE (U"Permission is hereby granted, free of charge, to any person obtaining "
	"a copy of the Unicode data files and any associated documentation "
	"(the \"Data Files\") or Unicode software and any associated documentation "
	"(the \"Software\") to deal in the Data Files or Software "
	"without restriction, including without limitation the rights to use, "
	"copy, modify, merge, publish, distribute, and/or sell copies of "
	"the Data Files or Software, and to permit persons to whom the Data Files "
	"or Software are furnished to do so, provided that either")
CODE (U"(a) this copyright and permission notice appear with all copies "
	"of the Data Files or Software, or")
CODE (U"(b) this copyright and permission notice appear in associated Documentation.")
CODE (U"")
CODE (U"THE DATA FILES AND SOFTWARE ARE PROVIDED \"AS IS\", WITHOUT WARRANTY OF "
	"ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE "
	"WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND "
	"NONINFRINGEMENT OF THIRD PARTY RIGHTS.")
CODE (U"IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS "
	"NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL "
	"DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, "
	"DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER "
	"TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR "
	"PERFORMANCE OF THE DATA FILES OR SOFTWARE.")
CODE (U"")
CODE (U"Except as contained in this notice, the name of a copyright holder "
	"shall not be used in advertising or otherwise to promote the sale, "
	"use or other dealings in these Data Files or Software without prior "
	"written authorization of the copyright holder.")
MAN_END

MAN_BEGIN (U"Acknowledgments", U"ppgb", 20220926)
NORMAL (U"The following people contributed source code to Praat:")
LIST_ITEM (U"Paul Boersma: user interface, graphics, @printing, @@Intro|sound@, "
	"@@Intro 3. Spectral analysis|spectral analysis@, @@Intro 4. Pitch analysis|pitch analysis@, "
	"@@Intro 5. Formant analysis|formant analysis@, @@Intro 6. Intensity analysis|intensity analysis@, "
	"@@Intro 7. Annotation|annotation@, @@Intro 8. Manipulation|speech manipulation@, @@voice|voice report@, "
	"@@ExperimentMFC|listening experiments@, "
	"@@articulatory synthesis@, @@OT learning|optimality-theoretic learning@, "
	"tables, @formulas, @scripting, and adaptation of PortAudio, GLPK, regular expressions, and Opus.")
LIST_ITEM (U"David Weenink: "
	"@@feedforward neural networks@, @@principal component analysis@, @@multidimensional scaling@, @@discriminant analysis@, @LPC, "
	"@VowelEditor, "
	"and adaptation of GSL, LAPACK, fftpack, regular expressions, Espeak, Ogg Vorbis, and Opus.")
LIST_ITEM (U"Stefan de Konink and Franz Brauße: major help in port to GTK.")
LIST_ITEM (U"Tom Naughton: major help in port to Cocoa.")
LIST_ITEM (U"Erez Volk: adaptation of FLAC and MAD.")
LIST_ITEM (U"Ola Söder: kNN classifiers, k-means clustering.")
LIST_ITEM (U"Rafael Laboissière: adaptation of XIPA, audio bug fixes for Linux.")
LIST_ITEM (U"Darryl Purnell created the first version of audio for Praat for Linux.")
NORMAL (U"We included the following freely available software libraries in Praat (sometimes with adaptations):")
LIST_ITEM (U"XIPA: IPA font for Unix by Fukui Rei (GPL).")
LIST_ITEM (U"GSL: GNU Scientific Library by Gerard Jungman and Brian Gough (GPL 3 or later).")
LIST_ITEM (U"GLPK: GNU Linear Programming Kit by Andrew Makhorin (GPL 3 or later); "
	"contains AMD software by the same author (LGPL 2.1 or later).")
LIST_ITEM (U"PortAudio: Portable Audio Library by Ross Bencina, Phil Burk, Bjorn Roche, Dominic Mazzoni, Darren Gibbs, "
	"version 19.7.0 of April 2021 (CC-BY-like license).")
LIST_ITEM (U"Espeak: text-to-speech synthesizer by Jonathan Duddington and Reece Dunn (GPL 3 or later).")
LIST_ITEM (U"MAD: MPEG Audio Decoder by Underbit Technologies (GPL 2 or later).")
LIST_ITEM (U"FLAC: Free Lossless Audio Codec by Josh Coalson and Xiph.Org, version 1.3.3 (@@FLAC BSD 3-clause license@).")
LIST_ITEM (U"Ogg Vorbis: audio compression by Christopher Montgomery (@@Ogg Vorbis BSD 3-clause license@).")
LIST_ITEM (U"Opus: audio compression by Jean-Marc Valin, Gregory Maxwell, Christopher Montgomery, Timothy Terriberry, "
	"Koen Vos, Andrew Allen and others (@@Opus BSD 3-clause license@).")
LIST_ITEM (U"SILK: audio compression by Skype Limited (@@Skype Limited BSD 3-clause license@).")
LIST_ITEM (U"fftpack: public-domain Fourier transforms by Paul Swarztrauber, translated to C by Christopher Montgomery.")
LIST_ITEM (U"@LAPACK: public-domain numeric algorithms by Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., "
	"Courant Institute, Argonne National Lab, and Rice University, "
	"C edition by Peng Du, Keith Seymour and Julie Langdou, version 3.2.1 of June 2009.")
LIST_ITEM (U"Regular expressions by Henry Spencer, Mark Edel, Christopher Conrad, Eddy De Greef (GPL 2 or later).")
LIST_ITEM (U"Unicode Character Database by Unicode Inc., version 14.0 of September 2021 (@@Unicode Inc. license agreement@)")
NORMAL (U"Most of the source code of Praat is distributed under the General Public License, version 2 or later. "
	"However, as Praat includes the above software written by others, "
	"the whole of Praat is distributed under the General Public License, version 3 or later.")
NORMAL (U"For their financial support during the development of Praat:")
LIST_ITEM (U"Netherlands Organization for Scientific Research (NWO) (1996–1999).")
LIST_ITEM (U"Nederlandse Taalunie (2006–2008).")
LIST_ITEM (U"Talkbank project, Carnegie Mellon / Linguistic Data Consortium (2002–2003).")
LIST_ITEM (U"Stichting Spraaktechnologie (2014–2016).")
LIST_ITEM (U"Spoken Dutch Corpus (CGN) (1999–2001).")
LIST_ITEM (U"Laboratorium Experimentele OtoRhinoLaryngologie, KU Leuven.")
LIST_ITEM (U"DFG-Projekt Dialektintonation, Universität Freiburg.")
LIST_ITEM (U"Department of Linguistics and Phonetics, Lund University.")
LIST_ITEM (U"Centre for Cognitive Neuroscience, University of Turku.")
LIST_ITEM (U"Linguistics Department, University of Joensuu.")
LIST_ITEM (U"Laboratoire de Sciences Cognitives et Psycholinguistique, Paris.")
LIST_ITEM (U"Department of Linguistics, Northwestern University.")
LIST_ITEM (U"Department of Finnish and General Linguistics, University of Tampere.")
LIST_ITEM (U"Institute for Language and Speech Processing, Paradissos Amaroussiou.")
LIST_ITEM (U"Jörg Jescheniak, Universität Leipzig.")
LIST_ITEM (U"The Linguistics Teaching Laboratory, Ohio State University.")
LIST_ITEM (U"Linguistics & Cognitive Science, Dartmouth College, Hanover NH.")
LIST_ITEM (U"Cornell Phonetics Lab, Ithaca NY.")
NORMAL (U"Finally we thank:")
LIST_ITEM (U"Ton Wempe and Dirk Jan Vet, for technical support and advice.")
LIST_ITEM (U"Daniel Hirst and Daniel McCloy, for managing the Praat Users List.")
LIST_ITEM (U"Rafael Laboissière and Andreas Tille, for maintaining the Debian package.")
LIST_ITEM (U"Jason Bacon and Adriaan de Groot, for maintaining the FreeBSD port.")
LIST_ITEM (U"José Joaquín Atria and Ingmar Steiner, for setting up the source-code repository on GitHub.")
LIST_ITEM (U"Hundreds of Praat users, for sending suggestions and notifying us of problems and thus helping us to improve Praat.")
MAN_END

MAN_BEGIN (U"License", U"ppgb", 20211016)
NORMAL (U"Praat is free software distributed under the @@General Public License, version 3@ or higher. "
	"See @Acknowledgments for details on the licenses of software libraries by others "
	"that are included in Praat.")
MAN_END

MAN_BEGIN (U"General Public License, version 3", U"ppgb", 20211026)
NORMAL (U"This is the license under which Praat as a whole is distributed.")
CODE (U"                    GNU GENERAL PUBLIC LICENSE")
CODE (U"                       Version 3, 29 June 2007")
CODE (U"")
CODE (U" Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>")
CODE (U" Everyone is permitted to copy and distribute verbatim copies")
CODE (U" of this license document, but changing it is not allowed.")
CODE (U"")
CODE (U"                            Preamble")
CODE (U"")
CODE (U"The GNU General Public License is a free, copyleft license for "
	"software and other kinds of works.")
CODE (U"")
CODE (U"The licenses for most software and other practical works are designed "
	"to take away your freedom to share and change the works.  By contrast, "
	"the GNU General Public License is intended to guarantee your freedom to "
	"share and change all versions of a program--to make sure it remains free "
	"software for all its users.  We, the Free Software Foundation, use the "
	"GNU General Public License for most of our software; it applies also to "
	"any other work released this way by its authors.  You can apply it to "
	"your programs, too.")
CODE (U"")
CODE (U"When we speak of free software, we are referring to freedom, not "
	"price.  Our General Public Licenses are designed to make sure that you "
	"have the freedom to distribute copies of free software (and charge for "
	"them if you wish), that you receive source code or can get it if you "
	"want it, that you can change the software or use pieces of it in new "
	"free programs, and that you know you can do these things.")
CODE (U"")
CODE (U"To protect your rights, we need to prevent others from denying you "
	"these rights or asking you to surrender the rights.  Therefore, you have "
	"certain responsibilities if you distribute copies of the software, or if "
	"you modify it: responsibilities to respect the freedom of others.")
CODE (U"")
CODE (U"For example, if you distribute copies of such a program, whether "
	"gratis or for a fee, you must pass on to the recipients the same "
	"freedoms that you received.  You must make sure that they, too, receive "
	"or can get the source code.  And you must show them these terms so they "
	"know their rights.")
CODE (U"")
CODE (U"Developers that use the GNU GPL protect your rights with two steps: "
	"(1) assert copyright on the software, and (2) offer you this License "
	"giving you legal permission to copy, distribute and/or modify it.")
CODE (U"")
CODE (U"For the developers' and authors' protection, the GPL clearly explains "
	"that there is no warranty for this free software.  For both users' and "
	"authors' sake, the GPL requires that modified versions be marked as "
	"changed, so that their problems will not be attributed erroneously to "
	"authors of previous versions.")
CODE (U"")
CODE (U"Some devices are designed to deny users access to install or run "
	"modified versions of the software inside them, although the manufacturer "
	"can do so.  This is fundamentally incompatible with the aim of "
	"protecting users' freedom to change the software.  The systematic "
	"pattern of such abuse occurs in the area of products for individuals to "
	"use, which is precisely where it is most unacceptable.  Therefore, we "
	"have designed this version of the GPL to prohibit the practice for those "
	"products.  If such problems arise substantially in other domains, we "
	"stand ready to extend this provision to those domains in future versions "
	"of the GPL, as needed to protect the freedom of users.")
CODE (U"")
CODE (U"Finally, every program is threatened constantly by software patents. "
	"States should not allow patents to restrict development and use of "
	"software on general-purpose computers, but in those that do, we wish to "
	"avoid the special danger that patents applied to a free program could "
	"make it effectively proprietary.  To prevent this, the GPL assures that "
	"patents cannot be used to render the program non-free.")
CODE (U"")
CODE (U"The precise terms and conditions for copying, distribution and "
	"modification follow.")
CODE (U"")
CODE (U"                       TERMS AND CONDITIONS")
CODE (U"")
CODE (U"0. Definitions.")
CODE (U"")
CODE (U"“This License” refers to version 3 of the GNU General Public License.")
CODE (U"")
CODE (U"“Copyright” also means copyright-like laws that apply to other kinds of "
	"works, such as semiconductor masks.")
CODE (U"")
CODE (U"“The Program” refers to any copyrightable work licensed under this "
	"License.  Each licensee is addressed as “you”.  “Licensees” and "
	"“recipients” may be individuals or organizations.")
CODE (U"")
CODE (U"To “modify” a work means to copy from or adapt all or part of the work "
	"in a fashion requiring copyright permission, other than the making of an "
	"exact copy.  The resulting work is called a “modified version” of the "
	"earlier work or a work “based on” the earlier work.")
CODE (U"")
CODE (U"A “covered work” means either the unmodified Program or a work based "
	"on the Program.")
CODE (U"")
CODE (U"To “propagate” a work means to do anything with it that, without "
	"permission, would make you directly or secondarily liable for "
	"infringement under applicable copyright law, except executing it on a "
	"computer or modifying a private copy.  Propagation includes copying, "
	"distribution (with or without modification), making available to the "
	"public, and in some countries other activities as well.")
CODE (U"")
CODE (U"To “convey” a work means any kind of propagation that enables other "
	"parties to make or receive copies.  Mere interaction with a user through "
	"a computer network, with no transfer of a copy, is not conveying.")
CODE (U"")
CODE (U"An interactive user interface displays “Appropriate Legal Notices” "
	"to the extent that it includes a convenient and prominently visible "
	"feature that (1) displays an appropriate copyright notice, and (2) "
	"tells the user that there is no warranty for the work (except to the "
	"extent that warranties are provided), that licensees may convey the "
	"work under this License, and how to view a copy of this License.  If "
	"the interface presents a list of user commands or options, such as a "
	"menu, a prominent item in the list meets this criterion.")
CODE (U"")
CODE (U"1. Source Code.")
CODE (U"")
CODE (U"The “source code” for a work means the preferred form of the work "
	"for making modifications to it.  “Object code” means any non-source "
	"form of a work.")
CODE (U"")
CODE (U"A “Standard Interface” means an interface that either is an official "
	"standard defined by a recognized standards body, or, in the case of "
	"interfaces specified for a particular programming language, one that "
	"is widely used among developers working in that language.")
CODE (U"")
CODE (U"The “System Libraries” of an executable work include anything, other "
	"than the work as a whole, that (a) is included in the normal form of "
	"packaging a Major Component, but which is not part of that Major "
	"Component, and (b) serves only to enable use of the work with that "
	"Major Component, or to implement a Standard Interface for which an "
	"implementation is available to the public in source code form.  A "
	"“Major Component”, in this context, means a major essential component "
	"(kernel, window system, and so on) of the specific operating system "
	"(if any) on which the executable work runs, or a compiler used to "
	"produce the work, or an object code interpreter used to run it.")
CODE (U"")
CODE (U"The “Corresponding Source” for a work in object code form means all "
	"the source code needed to generate, install, and (for an executable "
	"work) run the object code and to modify the work, including scripts to "
	"control those activities.  However, it does not include the work's "
	"System Libraries, or general-purpose tools or generally available free "
	"programs which are used unmodified in performing those activities but "
	"which are not part of the work.  For example, Corresponding Source "
	"includes interface definition files associated with source files for "
	"the work, and the source code for shared libraries and dynamically "
	"linked subprograms that the work is specifically designed to require, "
	"such as by intimate data communication or control flow between those "
	"subprograms and other parts of the work.")
CODE (U"")
CODE (U"The Corresponding Source need not include anything that users "
	"can regenerate automatically from other parts of the Corresponding "
	"Source.")
CODE (U"")
CODE (U"The Corresponding Source for a work in source code form is that "
	"same work.")
CODE (U"")
CODE (U"2. Basic Permissions.")
CODE (U"")
CODE (U"All rights granted under this License are granted for the term of "
	"copyright on the Program, and are irrevocable provided the stated "
	"conditions are met.  This License explicitly affirms your unlimited "
	"permission to run the unmodified Program.  The output from running a "
	"covered work is covered by this License only if the output, given its "
	"content, constitutes a covered work.  This License acknowledges your "
	"rights of fair use or other equivalent, as provided by copyright law.")
CODE (U"")
CODE (U"You may make, run and propagate covered works that you do not "
	"convey, without conditions so long as your license otherwise remains "
	"in force.  You may convey covered works to others for the sole purpose "
	"of having them make modifications exclusively for you, or provide you "
	"with facilities for running those works, provided that you comply with "
	"the terms of this License in conveying all material for which you do "
	"not control copyright.  Those thus making or running the covered works "
	"for you must do so exclusively on your behalf, under your direction "
	"and control, on terms that prohibit them from making any copies of "
	"your copyrighted material outside their relationship with you.")
CODE (U"")
CODE (U"Conveying under any other circumstances is permitted solely under "
	"the conditions stated below.  Sublicensing is not allowed; section 10 "
	"makes it unnecessary.")
CODE (U"")
CODE (U"3. Protecting Users’ Legal Rights From Anti-Circumvention Law.")
CODE (U"")
CODE (U"No covered work shall be deemed part of an effective technological "
	"measure under any applicable law fulfilling obligations under article "
	"11 of the WIPO copyright treaty adopted on 20 December 1996, or "
	"similar laws prohibiting or restricting circumvention of such "
	"measures.")
CODE (U"")
CODE (U"When you convey a covered work, you waive any legal power to forbid "
	"circumvention of technological measures to the extent such circumvention "
	"is effected by exercising rights under this License with respect to "
	"the covered work, and you disclaim any intention to limit operation or "
	"modification of the work as a means of enforcing, against the work's "
	"users, your or third parties' legal rights to forbid circumvention of "
	"technological measures.")
CODE (U"")
CODE (U"4. Conveying Verbatim Copies.")
CODE (U"")
CODE (U"You may convey verbatim copies of the Program's source code as you "
	"receive it, in any medium, provided that you conspicuously and "
	"appropriately publish on each copy an appropriate copyright notice; "
	"keep intact all notices stating that this License and any "
	"non-permissive terms added in accord with section 7 apply to the code; "
	"keep intact all notices of the absence of any warranty; and give all "
	"recipients a copy of this License along with the Program.")
CODE (U"")
CODE (U"You may charge any price or no price for each copy that you convey, "
	"and you may offer support or warranty protection for a fee.")
CODE (U"")
CODE (U"5. Conveying Modified Source Versions.")
CODE (U"")
CODE (U"You may convey a work based on the Program, or the modifications to "
	"produce it from the Program, in the form of source code under the "
	"terms of section 4, provided that you also meet all of these conditions:")
CODE (U"")
	CODE1 (U"a) The work must carry prominent notices stating that you modified "
		"it, and giving a relevant date.")
	CODE1 (U"")
	CODE1 (U"b) The work must carry prominent notices stating that it is "
		"released under this License and any conditions added under section "
		"7.  This requirement modifies the requirement in section 4 to "
		"“keep intact all notices”.")
	CODE1 (U"")
	CODE1 (U"c) You must license the entire work, as a whole, under this "
		"License to anyone who comes into possession of a copy.  This "
		"License will therefore apply, along with any applicable section 7 "
		"additional terms, to the whole of the work, and all its parts, "
		"regardless of how they are packaged.  This License gives no "
		"permission to license the work in any other way, but it does not "
		"invalidate such permission if you have separately received it.")
	CODE1 (U"")
	CODE1 (U"d) If the work has interactive user interfaces, each must display "
		"Appropriate Legal Notices; however, if the Program has interactive "
		"interfaces that do not display Appropriate Legal Notices, your "
		"work need not make them do so.")
CODE (U"")
CODE (U"A compilation of a covered work with other separate and independent "
	"works, which are not by their nature extensions of the covered work, "
	"and which are not combined with it such as to form a larger program, "
	"in or on a volume of a storage or distribution medium, is called an "
	"“aggregate” if the compilation and its resulting copyright are not "
	"used to limit the access or legal rights of the compilation's users "
	"beyond what the individual works permit.  Inclusion of a covered work "
	"in an aggregate does not cause this License to apply to the other "
	"parts of the aggregate.")
CODE (U"")
CODE (U"6. Conveying Non-Source Forms.")
CODE (U"")
CODE (U"You may convey a covered work in object code form under the terms "
	"of sections 4 and 5, provided that you also convey the "
	"machine-readable Corresponding Source under the terms of this License, "
	"in one of these ways:")
CODE (U"")
	CODE1 (U"a) Convey the object code in, or embodied in, a physical product "
		"(including a physical distribution medium), accompanied by the "
		"Corresponding Source fixed on a durable physical medium "
		"customarily used for software interchange.")
	CODE1 (U"")
	CODE1 (U"b) Convey the object code in, or embodied in, a physical product "
		"(including a physical distribution medium), accompanied by a "
		"written offer, valid for at least three years and valid for as "
		"long as you offer spare parts or customer support for that product "
		"model, to give anyone who possesses the object code either (1) a "
		"copy of the Corresponding Source for all the software in the "
		"product that is covered by this License, on a durable physical "
		"medium customarily used for software interchange, for a price no "
		"more than your reasonable cost of physically performing this "
		"conveying of source, or (2) access to copy the "
		"Corresponding Source from a network server at no charge.")
	CODE1 (U"")
	CODE1 (U"c) Convey individual copies of the object code with a copy of the "
		"written offer to provide the Corresponding Source.  This "
		"alternative is allowed only occasionally and noncommercially, and "
		"only if you received the object code with such an offer, in accord "
		"with subsection 6b.")
	CODE1 (U"")
	CODE1 (U"d) Convey the object code by offering access from a designated "
		"place (gratis or for a charge), and offer equivalent access to the "
		"Corresponding Source in the same way through the same place at no "
		"further charge.  You need not require recipients to copy the "
		"Corresponding Source along with the object code.  If the place to "
		"copy the object code is a network server, the Corresponding Source "
		"may be on a different server (operated by you or a third party) "
		"that supports equivalent copying facilities, provided you maintain "
		"clear directions next to the object code saying where to find the "
		"Corresponding Source.  Regardless of what server hosts the "
		"Corresponding Source, you remain obligated to ensure that it is "
		"available for as long as needed to satisfy these requirements.")
	CODE1 (U"")
	CODE1 (U"e) Convey the object code using peer-to-peer transmission, provided "
		"you inform other peers where the object code and Corresponding "
		"Source of the work are being offered to the general public at no "
		"charge under subsection 6d.")
CODE (U"")
CODE (U"A separable portion of the object code, whose source code is excluded "
	"from the Corresponding Source as a System Library, need not be "
	"included in conveying the object code work.")
CODE (U"")
CODE (U"A “User Product” is either (1) a “consumer product”, which means any "
	"tangible personal property which is normally used for personal, family, "
	"or household purposes, or (2) anything designed or sold for incorporation "
	"into a dwelling.  In determining whether a product is a consumer product, "
	"doubtful cases shall be resolved in favor of coverage.  For a particular "
	"product received by a particular user, “normally used” refers to a "
	"typical or common use of that class of product, regardless of the status "
	"of the particular user or of the way in which the particular user "
	"actually uses, or expects or is expected to use, the product.  A product "
	"is a consumer product regardless of whether the product has substantial "
	"commercial, industrial or non-consumer uses, unless such uses represent "
	"the only significant mode of use of the product.")
CODE (U"")
CODE (U"“Installation Information” for a User Product means any methods, "
	"procedures, authorization keys, or other information required to install "
	"and execute modified versions of a covered work in that User Product from "
	"a modified version of its Corresponding Source.  The information must "
	"suffice to ensure that the continued functioning of the modified object "
	"code is in no case prevented or interfered with solely because "
	"modification has been made.")
CODE (U"")
CODE (U"If you convey an object code work under this section in, or with, or "
	"specifically for use in, a User Product, and the conveying occurs as "
	"part of a transaction in which the right of possession and use of the "
	"User Product is transferred to the recipient in perpetuity or for a "
	"fixed term (regardless of how the transaction is characterized), the "
	"Corresponding Source conveyed under this section must be accompanied "
	"by the Installation Information.  But this requirement does not apply "
	"if neither you nor any third party retains the ability to install "
	"modified object code on the User Product (for example, the work has "
	"been installed in ROM).")
CODE (U"")
CODE (U"The requirement to provide Installation Information does not include a "
	"requirement to continue to provide support service, warranty, or updates "
	"for a work that has been modified or installed by the recipient, or for "
	"the User Product in which it has been modified or installed.  Access to a "
	"network may be denied when the modification itself materially and "
	"adversely affects the operation of the network or violates the rules and "
	"protocols for communication across the network.")
CODE (U"")
CODE (U"Corresponding Source conveyed, and Installation Information provided, "
	"in accord with this section must be in a format that is publicly "
	"documented (and with an implementation available to the public in "
	"source code form), and must require no special password or key for "
	"unpacking, reading or copying.")
CODE (U"")
CODE (U"7. Additional Terms.")
CODE (U"")
CODE (U"“Additional permissions” are terms that supplement the terms of this "
	"License by making exceptions from one or more of its conditions. "
	"Additional permissions that are applicable to the entire Program shall "
	"be treated as though they were included in this License, to the extent "
	"that they are valid under applicable law.  If additional permissions "
	"apply only to part of the Program, that part may be used separately "
	"under those permissions, but the entire Program remains governed by "
	"this License without regard to the additional permissions.")
CODE (U"")
CODE (U"When you convey a copy of a covered work, you may at your option "
	"remove any additional permissions from that copy, or from any part of "
	"it.  (Additional permissions may be written to require their own "
	"removal in certain cases when you modify the work.)  You may place "
	"additional permissions on material, added by you to a covered work, "
	"for which you have or can give appropriate copyright permission.")
CODE (U"")
CODE (U"Notwithstanding any other provision of this License, for material you "
	"add to a covered work, you may (if authorized by the copyright holders of "
	"that material) supplement the terms of this License with terms:")
CODE (U"")
	CODE1 (U"a) Disclaiming warranty or limiting liability differently from the "
		"terms of sections 15 and 16 of this License; or")
	CODE1 (U"")
	CODE1 (U"b) Requiring preservation of specified reasonable legal notices or "
		"author attributions in that material or in the Appropriate Legal "
		"Notices displayed by works containing it; or")
	CODE1 (U"")
	CODE1 (U"c) Prohibiting misrepresentation of the origin of that material, or "
		"requiring that modified versions of such material be marked in "
		"reasonable ways as different from the original version; or")
	CODE1 (U"")
	CODE1 (U"d) Limiting the use for publicity purposes of names of licensors or "
		"authors of the material; or")
	CODE1 (U"")
	CODE1 (U"e) Declining to grant rights under trademark law for use of some "
		"trade names, trademarks, or service marks; or")
	CODE1 (U"")
	CODE1 (U"f) Requiring indemnification of licensors and authors of that "
		"material by anyone who conveys the material (or modified versions of "
		"it) with contractual assumptions of liability to the recipient, for "
		"any liability that these contractual assumptions directly impose on "
		"those licensors and authors.")
CODE (U"")
CODE (U"All other non-permissive additional terms are considered “further "
	"restrictions” within the meaning of section 10.  If the Program as you "
	"received it, or any part of it, contains a notice stating that it is "
	"governed by this License along with a term that is a further "
	"restriction, you may remove that term.  If a license document contains "
	"a further restriction but permits relicensing or conveying under this "
	"License, you may add to a covered work material governed by the terms "
	"of that license document, provided that the further restriction does "
	"not survive such relicensing or conveying.")
CODE (U"")
CODE (U"If you add terms to a covered work in accord with this section, you "
	"must place, in the relevant source files, a statement of the "
	"additional terms that apply to those files, or a notice indicating "
	"where to find the applicable terms.")
CODE (U"")
CODE (U"Additional terms, permissive or non-permissive, may be stated in the "
	"form of a separately written license, or stated as exceptions; "
	"the above requirements apply either way.")
CODE (U"")
CODE (U"8. Termination.")
CODE (U"")
CODE (U"You may not propagate or modify a covered work except as expressly "
	"provided under this License.  Any attempt otherwise to propagate or "
	"modify it is void, and will automatically terminate your rights under "
	"this License (including any patent licenses granted under the third "
	"paragraph of section 11).")
CODE (U"")
CODE (U"However, if you cease all violation of this License, then your "
	"license from a particular copyright holder is reinstated (a) "
	"provisionally, unless and until the copyright holder explicitly and "
	"finally terminates your license, and (b) permanently, if the copyright "
	"holder fails to notify you of the violation by some reasonable means "
	"prior to 60 days after the cessation.")
CODE (U"")
CODE (U"Moreover, your license from a particular copyright holder is "
	"reinstated permanently if the copyright holder notifies you of the "
	"violation by some reasonable means, this is the first time you have "
	"received notice of violation of this License (for any work) from that "
	"copyright holder, and you cure the violation prior to 30 days after "
	"your receipt of the notice.")
CODE (U"")
CODE (U"Termination of your rights under this section does not terminate the "
	"licenses of parties who have received copies or rights from you under "
	"this License.  If your rights have been terminated and not permanently "
	"reinstated, you do not qualify to receive new licenses for the same "
	"material under section 10.")
CODE (U"")
CODE (U"9. Acceptance Not Required for Having Copies.")
CODE (U"")
CODE (U"You are not required to accept this License in order to receive or "
	"run a copy of the Program.  Ancillary propagation of a covered work "
	"occurring solely as a consequence of using peer-to-peer transmission "
	"to receive a copy likewise does not require acceptance.  However, "
	"nothing other than this License grants you permission to propagate or "
	"modify any covered work.  These actions infringe copyright if you do "
	"not accept this License.  Therefore, by modifying or propagating a "
	"covered work, you indicate your acceptance of this License to do so.")
CODE (U"")
CODE (U"10. Automatic Licensing of Downstream Recipients.")
CODE (U"")
CODE (U"Each time you convey a covered work, the recipient automatically "
	"receives a license from the original licensors, to run, modify and "
	"propagate that work, subject to this License.  You are not responsible "
	"for enforcing compliance by third parties with this License.")
CODE (U"")
CODE (U"An “entity transaction” is a transaction transferring control of an "
	"organization, or substantially all assets of one, or subdividing an "
	"organization, or merging organizations.  If propagation of a covered "
	"work results from an entity transaction, each party to that "
	"transaction who receives a copy of the work also receives whatever "
	"licenses to the work the party’s predecessor in interest had or could "
	"give under the previous paragraph, plus a right to possession of the "
	"Corresponding Source of the work from the predecessor in interest, if "
	"the predecessor has it or can get it with reasonable efforts.")
CODE (U"")
CODE (U"You may not impose any further restrictions on the exercise of the "
	"rights granted or affirmed under this License.  For example, you may "
	"not impose a license fee, royalty, or other charge for exercise of "
	"rights granted under this License, and you may not initiate litigation "
	"(including a cross-claim or counterclaim in a lawsuit) alleging that "
	"any patent claim is infringed by making, using, selling, offering for "
	"sale, or importing the Program or any portion of it.")
CODE (U"")
CODE (U"11. Patents.")
CODE (U"")
CODE (U"A “contributor” is a copyright holder who authorizes use under this "
	"License of the Program or a work on which the Program is based.  The "
	"work thus licensed is called the contributor’s “contributor version”.")
CODE (U"")
CODE (U"A contributor’s “essential patent claims” are all patent claims "
	"owned or controlled by the contributor, whether already acquired or "
	"hereafter acquired, that would be infringed by some manner, permitted "
	"by this License, of making, using, or selling its contributor version, "
	"but do not include claims that would be infringed only as a "
	"consequence of further modification of the contributor version.  For "
	"purposes of this definition, “control” includes the right to grant "
	"patent sublicenses in a manner consistent with the requirements of "
	"this License.")
CODE (U"")
CODE (U"Each contributor grants you a non-exclusive, worldwide, royalty-free "
	"patent license under the contributor's essential patent claims, to "
	"make, use, sell, offer for sale, import and otherwise run, modify and "
	"propagate the contents of its contributor version.")
CODE (U"")
CODE (U"In the following three paragraphs, a “patent license” is any express "
	"agreement or commitment, however denominated, not to enforce a patent "
	"(such as an express permission to practice a patent or covenant not to "
	"sue for patent infringement).  To “grant” such a patent license to a "
	"party means to make such an agreement or commitment not to enforce a "
	"patent against the party.")
CODE (U"")
CODE (U"If you convey a covered work, knowingly relying on a patent license, "
	"and the Corresponding Source of the work is not available for anyone "
	"to copy, free of charge and under the terms of this License, through a "
	"publicly available network server or other readily accessible means, "
	"then you must either (1) cause the Corresponding Source to be so "
	"available, or (2) arrange to deprive yourself of the benefit of the "
	"patent license for this particular work, or (3) arrange, in a manner "
	"consistent with the requirements of this License, to extend the patent "
	"license to downstream recipients.  “Knowingly relying” means you have "
	"actual knowledge that, but for the patent license, your conveying the "
	"covered work in a country, or your recipient’s use of the covered work "
	"in a country, would infringe one or more identifiable patents in that "
	"country that you have reason to believe are valid.")
CODE (U"")
CODE (U"If, pursuant to or in connection with a single transaction or "
	"arrangement, you convey, or propagate by procuring conveyance of, a "
	"covered work, and grant a patent license to some of the parties "
	"receiving the covered work authorizing them to use, propagate, modify "
	"or convey a specific copy of the covered work, then the patent license "
	"you grant is automatically extended to all recipients of the covered "
	"work and works based on it.")
CODE (U"")
CODE (U"A patent license is “discriminatory” if it does not include within "
	"the scope of its coverage, prohibits the exercise of, or is "
	"conditioned on the non-exercise of one or more of the rights that are "
	"specifically granted under this License.  You may not convey a covered "
	"work if you are a party to an arrangement with a third party that is "
	"in the business of distributing software, under which you make payment "
	"to the third party based on the extent of your activity of conveying "
	"the work, and under which the third party grants, to any of the "
	"parties who would receive the covered work from you, a discriminatory "
	"patent license (a) in connection with copies of the covered work "
	"conveyed by you (or copies made from those copies), or (b) primarily "
	"for and in connection with specific products or compilations that "
	"contain the covered work, unless you entered into that arrangement, "
	"or that patent license was granted, prior to 28 March 2007.")
CODE (U"")
CODE (U"Nothing in this License shall be construed as excluding or limiting "
	"any implied license or other defenses to infringement that may "
	"otherwise be available to you under applicable patent law.")
CODE (U"")
CODE (U"12. No Surrender of Others' Freedom.")
CODE (U"")
CODE (U"If conditions are imposed on you (whether by court order, agreement or "
	"otherwise) that contradict the conditions of this License, they do not "
	"excuse you from the conditions of this License.  If you cannot convey a "
	"covered work so as to satisfy simultaneously your obligations under this "
	"License and any other pertinent obligations, then as a consequence you may "
	"not convey it at all.  For example, if you agree to terms that obligate you "
	"to collect a royalty for further conveying from those to whom you convey "
	"the Program, the only way you could satisfy both those terms and this "
	"License would be to refrain entirely from conveying the Program.")
CODE (U"")
CODE (U"13. Use with the GNU Affero General Public License.")
CODE (U"")
CODE (U"Notwithstanding any other provision of this License, you have "
	"permission to link or combine any covered work with a work licensed "
	"under version 3 of the GNU Affero General Public License into a single "
	"combined work, and to convey the resulting work.  The terms of this "
	"License will continue to apply to the part which is the covered work, "
	"but the special requirements of the GNU Affero General Public License, "
	"section 13, concerning interaction through a network will apply to the "
	"combination as such.")
CODE (U"")
CODE (U"14. Revised Versions of this License.")
CODE (U"")
CODE (U"The Free Software Foundation may publish revised and/or new versions of "
	"the GNU General Public License from time to time.  Such new versions will "
	"be similar in spirit to the present version, but may differ in detail to "
	"address new problems or concerns.")
CODE (U"")
CODE (U"Each version is given a distinguishing version number.  If the "
	"Program specifies that a certain numbered version of the GNU General "
	"Public License “or any later version” applies to it, you have the "
	"option of following the terms and conditions either of that numbered "
	"version or of any later version published by the Free Software "
	"Foundation.  If the Program does not specify a version number of the "
	"GNU General Public License, you may choose any version ever published "
	"by the Free Software Foundation.")
CODE (U"")
CODE (U"If the Program specifies that a proxy can decide which future "
	"versions of the GNU General Public License can be used, that proxy's "
	"public statement of acceptance of a version permanently authorizes you "
	"to choose that version for the Program.")
CODE (U"")
CODE (U"Later license versions may give you additional or different "
	"permissions.  However, no additional obligations are imposed on any "
	"author or copyright holder as a result of your choosing to follow a "
	"later version.")
CODE (U"")
CODE (U"15. Disclaimer of Warranty.")
CODE (U"")
CODE (U"THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY "
	"APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT "
	"HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY "
	"OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, "
	"THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR "
	"PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM "
	"IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF "
	"ALL NECESSARY SERVICING, REPAIR OR CORRECTION.")
CODE (U"")
CODE (U"16. Limitation of Liability.")
CODE (U"")
CODE (U"IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING "
	"WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS "
	"THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY "
	"GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE "
	"USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF "
	"DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD "
	"PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), "
	"EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF "
	"SUCH DAMAGES.")
CODE (U"")
CODE (U"17. Interpretation of Sections 15 and 16.")
CODE (U"")
CODE (U"If the disclaimer of warranty and limitation of liability provided "
	"above cannot be given local legal effect according to their terms, "
	"reviewing courts shall apply local law that most closely approximates "
	"an absolute waiver of all civil liability in connection with the "
	"Program, unless a warranty or assumption of liability accompanies a "
	"copy of the Program in return for a fee.")
CODE (U"")
CODE (U"                     END OF TERMS AND CONDITIONS")
CODE (U"")
CODE (U"            How to Apply These Terms to Your New Programs")
CODE (U"")
CODE (U"If you develop a new program, and you want it to be of the greatest "
	"possible use to the public, the best way to achieve this is to make it "
	"free software which everyone can redistribute and change under these terms.")
CODE (U"")
CODE (U"To do so, attach the following notices to the program.  It is safest "
	"to attach them to the start of each source file to most effectively "
	"state the exclusion of warranty; and each file should have at least "
	"the “copyright” line and a pointer to where the full notice is found.")
CODE (U"")
	CODE1 (U"##<one line to give the program’s name and a brief idea of what it does.>")
	CODE1 (U"##Copyright (C) <year>  <name of author>")
	CODE1 (U"")
	CODE1 (U"##This program is free software: you can redistribute it and/or modify "
		"it under the terms of the GNU General Public License as published by "
		"the Free Software Foundation, either version 3 of the License, or "
		"(at your option) any later version.")
	CODE1 (U"")
	CODE1 (U"##This program is distributed in the hope that it will be useful, "
		"but WITHOUT ANY WARRANTY; without even the implied warranty of "
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
		"GNU General Public License for more details.")
	CODE1 (U"")
	CODE1 (U"##You should have received a copy of the GNU General Public License "
		"along with this program.  If not, see <https://www.gnu.org/licenses/>.")
	CODE1 (U"")
	CODE1 (U"Also add information on how to contact you by electronic and paper mail.")
CODE (U"")
CODE (U"If the program does terminal interaction, make it output a short "
	"notice like this when it starts in an interactive mode:")
CODE (U"")
	CODE1 (U"##<program>  Copyright (C) <year>  <name of author>")
	CODE1 (U"##This program comes with ABSOLUTELY NO WARRANTY; for details type ‘show w’.")
	CODE1 (U"##This is free software, and you are welcome to redistribute it")
	CODE1 (U"##under certain conditions; type ‘show c’ for details.")
	CODE1 (U"")
	CODE1 (U"The hypothetical commands ‘show w’ and ‘show c’ should show the appropriate "
		"parts of the General Public License.  Of course, your program’s commands "
		"might be different; for a GUI interface, you would use an “about box”.")
CODE (U"")
CODE (U"You should also get your employer (if you work as a programmer) or school, "
	"if any, to sign a “copyright disclaimer” for the program, if necessary. "
	"For more information on this, and how to apply and follow the GNU GPL, see "
	"<https://www.gnu.org/licenses/>.")
CODE (U"")
CODE (U"The GNU General Public License does not permit incorporating your program "
	"into proprietary programs.  If your program is a subroutine library, you "
	"may consider it more useful to permit linking proprietary applications with "
	"the library.  If this is what you want to do, use the GNU Lesser General "
	"Public License instead of this License.  But first, please read "
	"<https://www.gnu.org/licenses/why-not-lgpl.html>.")
MAN_END

MAN_BEGIN (U"Privacy and security", U"ppgb", 20221109)
INTRO (U"Praat is an “isolated” app. You download it from praat.org, "
	"then record sounds into Praat (all in RAM) or open a sound file, "
	"then analyse or manipulate that sound. The only way in which your results "
	"are saved to disk (as e.g. a Pitch file, a TextGrid file, or a sound file), "
	"is when you explicitly choose one of the #Save or #Export commands "
	"from Praat’s menus; Praat will not by itself save any data files to disk "
	"or send any information anywhere. When you create a picture in the Picture window, "
	"the only way to move that picture anywhere else is if you save it explicitly "
	"to a picture file (e.g. PNG) or if you Copy–Paste it to e.g. a text editing "
	"app such as e.g. Microsoft Word; Praat will not by itself save any picture to disk "
	"or to the clipboard or send any information anywhere. "
	"Praat will run just fine on your computer if it does not have Internet access, "
	"and in fact Praat cannot even notice whether you are in a network or not. "
	"Praat works entirely stand-alone.")
ENTRY (U"Praat does not call home")
NORMAL (U"When you are using Praat, you can be assured that Praat does not attempt to send any of your data "
	"or pictures or settings to the Praat team. "
	"In fact, Praat never accesses the Internet, not even to @@checking for updates|check for updates@.")
ENTRY (U"No telemetry")
NORMAL (U"Praat does not send anything to the Praat team while you are using Praat:")
LIST_ITEM (U"\\bu No surveillance")
LIST_ITEM (U"\\bu No tracking")
LIST_ITEM (U"\\bu No Google Analytics")
LIST_ITEM (U"\\bu In general, no spying or data mining by the Praat team")
ENTRY (U"What does Praat save to disk without asking you?")
NORMAL (U"Praat will save your preferences to your own disk on your own computer, "
	"in a folder of your own, when you close Praat. "
	"This includes the settings in your Sound window (e.g. your last chosen Pitch range), "
	"so that your Sound windows will look the same after you start Praat up again. "
	"The goal of this is to provide a continuous user experience, and is what you probably expect, "
	"because most apps that you use on your computer work this way.")
ENTRY (U"What we do measure")
NORMAL (U"As mentioned above, Praat does no telemetry, i.e. it does not send us anything while you are using Praat. "
	"We %do% receive some information, though, when %you contact %us. This happens when you download "
	"a new Praat version for your computer. We log the Praat downloads, so that we can potentially count "
	"how often which edition and which version of Praat is downloaded.")
ENTRY (U"Wouldn’t telemetry be useful for the quality of Praat?")
NORMAL (U"Companies that use telemetry tend to justify that by arguing that gathering information on how their app is used "
	"is useful for improving the quality of their app (by collecting error messages), "
	"or to know which features are rarely used, so that those features can be removed.")
NORMAL (U"We are skeptical. If we, as Praat developers, have made a programming error, "
	"then we hope that an “assertion” will help solve the issue. "
	"An assertion is a place in our code where Praat will crash if a certain assumption "
	"is not met. A message window will pop up in Praat that says that Praat will crash, together "
	"with the request to send some relevant information by email to us, the developers of Praat. "
	"If you do send this crash information on to us (you can read it, as it is normal English without secrets), "
	"we will then virtually always find out (sometimes with some more help from you, "
	"such as the sound file or script that caused the crash) "
	"what was wrong, and correct the mistake, so that our programming error (“bug”) no longer occurs "
	"in the next version of Praat. We will also build an automatable test that checks, for all future "
	"versions of Praat, that the bug does not reappear. In this way, every Praat version tends to be more stable and correct "
	"than the previous version. We believe that this practice minimizes the problems with Praat sufficiently, "
	"and no automated reporting of error messages and crash messages is necessary.")
NORMAL (U"As for the removal of obsolete features, we are just very conservative. "
	"Typically, file types from the 1980s and 1990s can typically still be opened in the 2020s, "
	"and old Praat scripts should continue to run for at least 15 years after we marked a language feature "
	"as “deprecated” or “obsolete” (and removed it from the manual). "
	"This has not prevented us from also being able to open file types invented in the 2020s "
	"or to have a modern scripting language that supports vectors, matrices and string arrays, "
	"and backward compatibility hardly hampers the continual modernization of Praat.")
ENTRY (U"Praat scripts and plug-ins")
NORMAL (U"As with R scripts, Python scripts, and quite generally any kinds of scripts from any source, "
	"you should consider Praat scripts written by others, such as plug-ins that you download, "
	"as separate apps with their own privacy and security issues. Use a script or plug-in only "
	"if you completely trust that script or plug-in and its creators.")
MAN_END

MAN_BEGIN (U"Checking for updates", U"ppgb", 20220217)
INTRO (U"Updates for Praat are available from `www.praat.org`.")
NORMAL (U"Your current version (if you are reading this from the manual inside the Praat program, rather than from the website) is " stringize(PRAAT_VERSION_STR)
	", from " stringize(PRAAT_MONTH) " " stringize(PRAAT_DAY) ", " stringize(PRAAT_YEAR) ". "
	"Given that we tend to release new Praat versions once or twice a month, "
	"you can probably guess whether it would be worth your while to have a look at `www.praat.org` "
	"to see what is new, and perhaps download a new version.")
NORMAL (U"Praat improves continually, and old features will almost always continue to work, "
	"so there should never be a reason to continue to work with older versions.")
ENTRY (U"Why no automatic update checking?")
NORMAL (U"Many apps automatically check for updates when you start them up. "
	"This means that the owners of such an app are capable of recording which users use their app when, "
	"which is information that can potentially harm your privacy, for instance when a government "
	"or legal investigation demands that the app owners provide them with access to such information. "
	"The Praat team wants to stay far away from the possibility of such situations occurring, "
	"even if you may be convinced that usage of the Praat program cannot be regarded by anybody "
	"as being anything other than perfectly innocent. For this resason, the Praat program "
	"will never contact the Praat team and, more generally, "
	"will never attempt to access the Internet by itself. For more information, see @@Privacy and security@.")
MAN_END

MAN_PAGES_BEGIN R"~~~(
"Reporting a problem"
© Paul Boersma 2022-01-16

Anything that you consider incorrect behaviour of Praat (a “bug”) can be reported
to the authors by email (`paul.boersma@uva.nl`). This includes any crashes.

Questions about how to use Praat for your specific cases
can be posed to the Praat User List (`https://groups.io/g/Praat-Users-List`).
This includes any questions on why your self-written Praat script does something unexpected.
)~~~" MAN_PAGES_END

}

/* End of file manual_licenses.cpp */
