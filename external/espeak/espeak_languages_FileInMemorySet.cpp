/* espeak_languages_FileInMemorySet.cpp
 *
 * This file was automatically created from files in the folder `generate/espeak/data`
 * by the script `generate/espeak/GENERATE.praat` in the Praat source distribution.
 *
 * eSpeak NG version: 1.52-dev, downloaded 2024-08-24T19:38Z from https://github.com/espeak-ng/espeak-ng
 * File creation date: Wed Aug 28 17:43:56 2024
 *
 * Copyright (C) 2005-2014 Jonathan Duddington (for eSpeak)
 * Copyright (C) 2015-2023 Reese Dunn (for eSpeak-NG)
 * Copyright (C) 2012-2024 David Weenink, 2024 Paul Boersma (for Praat)
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

#include "espeakdata_FileInMemory.h"
#include "FileInMemorySet.h"

autoFileInMemorySet create_espeak_languages_FileInMemorySet () {
	try {
		autoFileInMemorySet me = FileInMemorySet_create ();
		static unsigned char espeak_languages_FileInMemorySet1_data[112] = {
			110,97,109,101,32,86,105,101,116,110,97,109,101,115,101,32,40,78,111,114,116,104,101,114,110,41,10,108,97,110,
			103,117,97,103,101,32,118,105,10,10,119,111,114,100,115,32,49,32,50,10,112,105,116,99,104,32,57,53,32,49,
			55,53,10,10,10,116,111,110,101,32,49,48,48,32,50,50,53,32,56,48,48,32,49,48,48,32,50,48,48,48,
			32,53,48,32,53,52,48,48,32,55,53,32,56,48,48,48,32,50,48,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet1 = FileInMemory_createWithData (111, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet1_data), true, 
			U"./data/lang/aav/vi", 
			U"vi");
		my addItem_move (espeak_languages_FileInMemorySet1.move());

		static unsigned char espeak_languages_FileInMemorySet2_data[144] = {
			110,97,109,101,32,86,105,101,116,110,97,109,101,115,101,32,40,67,101,110,116,114,97,108,41,10,108,97,110,103,
			117,97,103,101,32,118,105,45,118,110,45,120,45,99,101,110,116,114,97,108,10,112,104,111,110,101,109,101,115,32,
			118,105,45,104,117,101,10,100,105,99,116,114,117,108,101,115,32,49,10,10,119,111,114,100,115,32,49,10,112,105,
			116,99,104,32,56,50,32,49,49,56,32,32,32,47,47,56,48,32,49,49,56,10,32,118,111,105,99,105,110,103,
			32,57,48,32,32,47,47,49,56,10,32,102,108,117,116,116,101,114,32,32,50,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet2 = FileInMemory_createWithData (143, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet2_data), true, 
			U"./data/lang/aav/vi-VN-x-central", 
			U"vi-VN-x-central");
		my addItem_move (espeak_languages_FileInMemorySet2.move());

		static unsigned char espeak_languages_FileInMemorySet3_data[143] = {
			110,97,109,101,32,86,105,101,116,110,97,109,101,115,101,32,40,83,111,117,116,104,101,114,110,41,10,108,97,110,
			103,117,97,103,101,32,118,105,45,118,110,45,120,45,115,111,117,116,104,10,112,104,111,110,101,109,101,115,32,118,
			105,45,115,103,110,10,100,105,99,116,114,117,108,101,115,32,50,10,10,119,111,114,100,115,32,49,10,112,105,116,
			99,104,32,56,50,32,49,49,56,32,32,32,47,47,56,48,32,49,49,56,10,32,118,111,105,99,105,110,103,32,
			57,48,32,32,47,47,49,56,10,32,102,108,117,116,116,101,114,32,32,50,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet3 = FileInMemory_createWithData (142, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet3_data), true, 
			U"./data/lang/aav/vi-VN-x-south", 
			U"vi-VN-x-south");
		my addItem_move (espeak_languages_FileInMemorySet3.move());

		static unsigned char espeak_languages_FileInMemorySet4_data[42] = {
			110,97,109,101,32,69,115,112,101,114,97,110,116,111,10,108,97,110,103,117,97,103,101,32,101,111,10,10,97,112,
			111,115,116,114,111,112,104,101,32,50,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet4 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet4_data), true, 
			U"./data/lang/art/eo", 
			U"eo");
		my addItem_move (espeak_languages_FileInMemorySet4.move());

		static unsigned char espeak_languages_FileInMemorySet5_data[30] = {
			110,97,109,101,32,73,110,116,101,114,108,105,110,103,117,97,10,108,97,110,103,117,97,103,101,32,105,97,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet5 = FileInMemory_createWithData (29, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet5_data), true, 
			U"./data/lang/art/ia", 
			U"ia");
		my addItem_move (espeak_languages_FileInMemorySet5.move());

		static unsigned char espeak_languages_FileInMemorySet6_data[51] = {
			110,97,109,101,32,73,100,111,10,108,97,110,103,117,97,103,101,32,105,111,10,112,104,111,110,101,109,101,115,32,
			101,111,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,32,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet6 = FileInMemory_createWithData (50, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet6_data), true, 
			U"./data/lang/art/io", 
			U"io");
		my addItem_move (espeak_languages_FileInMemorySet6.move());

		static unsigned char espeak_languages_FileInMemorySet7_data[70] = {
			110,97,109,101,32,76,111,106,98,97,110,10,108,97,110,103,117,97,103,101,32,106,98,111,10,10,115,112,101,101,
			100,32,56,48,32,32,32,47,47,32,115,112,101,101,100,32,97,100,106,117,115,116,109,101,110,116,44,32,112,101,
			114,99,101,110,116,97,103,101,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet7 = FileInMemory_createWithData (69, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet7_data), true, 
			U"./data/lang/art/jbo", 
			U"jbo");
		my addItem_move (espeak_languages_FileInMemorySet7.move());

		static unsigned char espeak_languages_FileInMemorySet8_data[136] = {
			110,97,109,101,32,76,105,110,103,117,97,32,70,114,97,110,99,97,32,78,111,118,97,13,10,108,97,110,103,117,
			97,103,101,32,108,102,110,13,10,13,10,112,104,111,110,101,109,101,115,32,98,97,115,101,50,13,10,108,95,117,
			110,112,114,111,110,111,117,110,99,97,98,108,101,32,48,13,10,110,117,109,98,101,114,115,32,50,32,51,13,10,
			13,10,115,116,114,101,115,115,76,101,110,103,116,104,32,32,49,53,48,32,49,52,48,32,49,56,48,32,49,56,
			48,32,48,32,48,32,50,48,48,32,50,48,48,13,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet8 = FileInMemory_createWithData (135, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet8_data), true, 
			U"./data/lang/art/lfn", 
			U"lfn");
		my addItem_move (espeak_languages_FileInMemorySet8.move());

		static unsigned char espeak_languages_FileInMemorySet9_data[57] = {
			110,97,109,101,32,75,108,105,110,103,111,110,10,108,97,110,103,117,97,103,101,32,112,105,113,100,10,115,116,97,
			116,117,115,32,116,101,115,116,105,110,103,10,115,116,114,101,115,115,82,117,108,101,32,51,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet9 = FileInMemory_createWithData (56, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet9_data), true, 
			U"./data/lang/art/piqd", 
			U"piqd");
		my addItem_move (espeak_languages_FileInMemorySet9.move());

		static unsigned char espeak_languages_FileInMemorySet10_data[141] = {
			110,97,109,101,32,80,121,97,115,104,10,108,97,110,103,117,97,103,101,32,112,121,10,109,97,105,110,116,97,105,
			110,101,114,32,76,111,103,97,110,32,83,116,114,101,111,110,100,106,32,60,108,111,103,97,110,64,108,105,98,101,
			114,105,116,46,99,97,62,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,10,115,112,101,101,100,32,
			56,48,32,32,32,47,47,32,115,112,101,101,100,32,97,100,106,117,115,116,109,101,110,116,44,32,112,101,114,99,
			101,110,116,97,103,101,10,115,116,114,101,115,115,82,117,108,101,32,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet10 = FileInMemory_createWithData (140, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet10_data), true, 
			U"./data/lang/art/py", 
			U"py");
		my addItem_move (espeak_languages_FileInMemorySet10.move());

		static unsigned char espeak_languages_FileInMemorySet11_data[58] = {
			110,97,109,101,32,76,97,110,103,32,66,101,108,116,97,10,108,97,110,103,117,97,103,101,32,113,100,98,10,10,
			110,117,109,98,101,114,115,32,52,32,51,10,10,114,101,112,108,97,99,101,32,49,32,116,32,63,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet11 = FileInMemory_createWithData (57, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet11_data), true, 
			U"./data/lang/art/qdb", 
			U"qdb");
		my addItem_move (espeak_languages_FileInMemorySet11.move());

		static unsigned char espeak_languages_FileInMemorySet12_data[174] = {
			110,97,109,101,32,81,117,101,110,121,97,10,108,97,110,103,117,97,103,101,32,113,121,97,10,115,116,114,101,115,
			115,82,117,108,101,32,50,10,47,47,32,114,117,108,101,61,112,101,110,117,108,116,105,109,97,116,101,44,32,119,
			105,116,104,32,113,121,97,95,114,117,108,101,115,32,102,111,114,32,108,105,103,104,116,32,112,101,110,117,108,116,
			105,109,97,116,101,32,115,121,108,108,97,98,108,101,115,32,116,111,32,109,111,118,101,32,112,114,105,109,97,114,
			121,32,115,116,114,101,115,115,32,116,111,32,116,104,101,32,112,114,101,99,101,100,105,110,103,32,40,97,110,116,
			101,112,101,110,117,108,116,105,109,97,116,101,41,32,115,121,108,108,97,98,108,101,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet12 = FileInMemory_createWithData (173, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet12_data), true, 
			U"./data/lang/art/qya", 
			U"qya");
		my addItem_move (espeak_languages_FileInMemorySet12.move());

		static unsigned char espeak_languages_FileInMemorySet13_data[176] = {
			110,97,109,101,32,83,105,110,100,97,114,105,110,10,108,97,110,103,117,97,103,101,32,115,106,110,10,115,116,114,
			101,115,115,82,117,108,101,32,50,10,47,47,32,114,117,108,101,61,112,101,110,117,108,116,105,109,97,116,101,44,
			32,119,105,116,104,32,115,106,110,95,114,117,108,101,115,32,102,111,114,32,108,105,103,104,116,32,112,101,110,117,
			108,116,105,109,97,116,101,32,115,121,108,108,97,98,108,101,115,32,116,111,32,109,111,118,101,32,112,114,105,109,
			97,114,121,32,115,116,114,101,115,115,32,116,111,32,116,104,101,32,112,114,101,99,101,100,105,110,103,32,40,97,
			110,116,101,112,101,110,117,108,116,105,109,97,116,101,41,32,115,121,108,108,97,98,108,101,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet13 = FileInMemory_createWithData (175, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet13_data), true, 
			U"./data/lang/art/sjn", 
			U"sjn");
		my addItem_move (espeak_languages_FileInMemorySet13.move());

		static unsigned char espeak_languages_FileInMemorySet14_data[104] = {
			110,97,109,101,32,120,101,120,116,97,110,45,116,101,115,116,10,108,97,110,103,117,97,103,101,32,120,101,120,10,
			10,112,104,111,110,101,109,101,115,32,112,116,45,98,114,10,112,104,111,110,101,109,101,115,32,112,116,10,10,112,
			105,116,99,104,32,56,48,32,49,51,48,10,10,100,105,99,116,114,117,108,101,115,32,49,10,116,117,110,101,115,
			32,115,55,32,99,55,32,113,55,32,101,55,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet14 = FileInMemory_createWithData (103, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet14_data), true, 
			U"./data/lang/art/xex", 
			U"xex");
		my addItem_move (espeak_languages_FileInMemorySet14.move());

		static unsigned char espeak_languages_FileInMemorySet15_data[115] = {
			110,97,109,101,32,78,97,104,117,97,116,108,32,40,67,108,97,115,115,105,99,97,108,41,10,108,97,110,103,117,
			97,103,101,32,110,99,105,10,10,105,110,116,111,110,97,116,105,111,110,32,51,10,115,116,114,101,115,115,82,117,
			108,101,32,50,10,115,116,114,101,115,115,76,101,110,103,116,104,32,32,49,57,48,32,32,49,57,48,32,32,50,
			48,48,32,32,50,48,48,32,32,48,32,32,48,32,32,50,50,48,32,32,50,52,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet15 = FileInMemory_createWithData (114, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet15_data), true, 
			U"./data/lang/azc/nci", 
			U"nci");
		my addItem_move (espeak_languages_FileInMemorySet15.move());

		static unsigned char espeak_languages_FileInMemorySet16_data[29] = {
			110,97,109,101,32,76,105,116,104,117,97,110,105,97,110,10,108,97,110,103,117,97,103,101,32,108,116,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet16 = FileInMemory_createWithData (28, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet16_data), true, 
			U"./data/lang/bat/lt", 
			U"lt");
		my addItem_move (espeak_languages_FileInMemorySet16.move());

		static unsigned char espeak_languages_FileInMemorySet17_data[313] = {
			110,97,109,101,32,76,97,116,103,97,108,105,97,110,10,108,97,110,103,117,97,103,101,32,108,116,103,10,109,97,
			105,110,116,97,105,110,101,114,32,86,97,108,100,105,115,32,86,105,116,111,108,105,110,115,32,60,118,97,108,100,
			105,115,46,118,105,116,111,108,105,110,115,64,111,100,111,46,108,118,62,10,115,116,97,116,117,115,32,116,101,115,
			116,105,110,103,10,112,104,111,110,101,109,101,115,32,108,118,10,100,105,99,116,105,111,110,97,114,121,32,108,118,
			10,100,105,99,116,114,117,108,101,115,32,50,32,32,32,47,47,32,83,101,116,116,105,110,103,32,102,111,114,32,
			76,97,116,103,97,108,105,97,110,32,112,114,111,110,117,110,99,105,97,116,105,111,110,10,119,111,114,100,115,32,
			48,32,50,10,112,105,116,99,104,32,54,52,32,49,49,56,10,116,111,110,101,32,54,48,32,49,53,48,32,50,
			48,52,32,49,48,48,32,52,48,48,32,50,53,53,32,55,48,48,32,49,48,32,51,48,48,48,32,50,53,53,
			10,115,116,114,101,115,115,65,109,112,32,49,50,32,49,48,32,56,32,56,32,48,32,48,32,49,53,32,49,54,
			10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,54,48,32,49,52,48,32,50,48,48,32,49,52,48,32,
			48,32,48,32,50,52,48,32,49,54,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet17 = FileInMemory_createWithData (312, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet17_data), true, 
			U"./data/lang/bat/ltg", 
			U"ltg");
		my addItem_move (espeak_languages_FileInMemorySet17.move());

		static unsigned char espeak_languages_FileInMemorySet18_data[230] = {
			110,97,109,101,32,76,97,116,118,105,97,110,10,108,97,110,103,117,97,103,101,32,108,118,10,109,97,105,110,116,
			97,105,110,101,114,32,86,97,108,100,105,115,32,86,105,116,111,108,105,110,115,32,60,118,97,108,100,105,115,46,
			118,105,116,111,108,105,110,115,64,111,100,111,46,108,118,62,10,115,116,97,116,117,115,32,109,97,116,117,114,101,
			10,119,111,114,100,115,32,48,32,50,10,112,105,116,99,104,32,54,55,32,49,50,51,10,116,111,110,101,32,54,
			48,32,49,53,48,32,50,48,52,32,49,48,48,32,52,48,48,32,50,53,53,32,55,48,48,32,49,48,32,51,
			48,48,48,32,50,53,53,10,115,116,114,101,115,115,65,109,112,32,49,49,32,56,32,49,49,32,57,32,48,32,
			48,32,49,52,32,49,50,10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,54,48,32,49,50,48,32,50,
			48,48,32,49,51,48,32,48,32,48,32,50,51,48,32,49,56,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet18 = FileInMemory_createWithData (229, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet18_data), true, 
			U"./data/lang/bat/lv", 
			U"lv");
		my addItem_move (espeak_languages_FileInMemorySet18.move());

		static unsigned char espeak_languages_FileInMemorySet19_data[42] = {
			110,97,109,101,32,83,119,97,104,105,108,105,10,108,97,110,103,117,97,103,101,32,115,119,10,10,115,116,97,116,
			117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet19 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet19_data), true, 
			U"./data/lang/bnt/sw", 
			U"sw");
		my addItem_move (espeak_languages_FileInMemorySet19.move());

		static unsigned char espeak_languages_FileInMemorySet20_data[43] = {
			110,97,109,101,32,83,101,116,115,119,97,110,97,10,108,97,110,103,117,97,103,101,32,116,110,10,10,115,116,97,
			116,117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet20 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet20_data), true, 
			U"./data/lang/bnt/tn", 
			U"tn");
		my addItem_move (espeak_languages_FileInMemorySet20.move());

		static unsigned char espeak_languages_FileInMemorySet21_data[125] = {
			110,97,109,101,32,71,101,111,114,103,105,97,110,10,108,97,110,103,117,97,103,101,32,107,97,10,108,111,119,101,
			114,99,97,115,101,83,101,110,116,101,110,99,101,9,47,47,32,65,32,112,101,114,105,111,100,32,102,111,108,108,
			111,119,101,100,32,98,121,32,97,32,108,111,119,101,114,99,97,115,101,32,108,101,116,116,101,114,32,105,115,32,
			99,111,110,115,105,100,101,114,101,100,32,97,32,115,101,110,116,101,110,99,101,32,40,109,107,104,101,100,114,117,
			108,105,41,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet21 = FileInMemory_createWithData (124, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet21_data), true, 
			U"./data/lang/ccs/ka", 
			U"ka");
		my addItem_move (espeak_languages_FileInMemorySet21.move());

		static unsigned char espeak_languages_FileInMemorySet22_data[38] = {
			110,97,109,101,32,87,101,108,115,104,10,108,97,110,103,117,97,103,101,32,99,121,10,10,105,110,116,111,110,97,
			116,105,111,110,32,52,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet22 = FileInMemory_createWithData (37, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet22_data), true, 
			U"./data/lang/cel/cy", 
			U"cy");
		my addItem_move (espeak_languages_FileInMemorySet22.move());

		static unsigned char espeak_languages_FileInMemorySet23_data[67] = {
			110,97,109,101,32,71,97,101,108,105,99,32,40,73,114,105,115,104,41,10,108,97,110,103,117,97,103,101,32,103,
			97,10,10,100,105,99,116,114,117,108,101,115,32,49,32,32,47,47,32,102,105,120,32,102,111,114,32,101,99,108,
			105,112,115,105,115,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet23 = FileInMemory_createWithData (66, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet23_data), true, 
			U"./data/lang/cel/ga", 
			U"ga");
		my addItem_move (espeak_languages_FileInMemorySet23.move());

		static unsigned char espeak_languages_FileInMemorySet24_data[52] = {
			110,97,109,101,32,71,97,101,108,105,99,32,40,83,99,111,116,116,105,115,104,41,10,108,97,110,103,117,97,103,
			101,32,103,100,10,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet24 = FileInMemory_createWithData (51, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet24_data), true, 
			U"./data/lang/cel/gd", 
			U"gd");
		my addItem_move (espeak_languages_FileInMemorySet24.move());

		static unsigned char espeak_languages_FileInMemorySet25_data[40] = {
			110,97,109,101,32,79,114,111,109,111,10,108,97,110,103,117,97,103,101,32,111,109,10,10,115,116,97,116,117,115,
			32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet25 = FileInMemory_createWithData (39, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet25_data), true, 
			U"./data/lang/cus/om", 
			U"om");
		my addItem_move (espeak_languages_FileInMemorySet25.move());

		static unsigned char espeak_languages_FileInMemorySet26_data[56] = {
			110,97,109,101,32,75,97,110,110,97,100,97,10,108,97,110,103,117,97,103,101,32,107,110,10,10,105,110,116,111,
			110,97,116,105,111,110,32,50,10,47,47,99,111,110,115,111,110,97,110,116,115,32,56,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet26 = FileInMemory_createWithData (55, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet26_data), true, 
			U"./data/lang/dra/kn", 
			U"kn");
		my addItem_move (espeak_languages_FileInMemorySet26.move());

		static unsigned char espeak_languages_FileInMemorySet27_data[58] = {
			110,97,109,101,32,77,97,108,97,121,97,108,97,109,10,108,97,110,103,117,97,103,101,32,109,108,10,10,105,110,
			116,111,110,97,116,105,111,110,32,50,10,47,47,99,111,110,115,111,110,97,110,116,115,32,56,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet27 = FileInMemory_createWithData (57, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet27_data), true, 
			U"./data/lang/dra/ml", 
			U"ml");
		my addItem_move (espeak_languages_FileInMemorySet27.move());

		static unsigned char espeak_languages_FileInMemorySet28_data[52] = {
			110,97,109,101,32,84,97,109,105,108,10,108,97,110,103,117,97,103,101,32,116,97,10,10,105,110,116,111,110,97,
			116,105,111,110,32,50,10,99,111,110,115,111,110,97,110,116,115,32,56,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet28 = FileInMemory_createWithData (51, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet28_data), true, 
			U"./data/lang/dra/ta", 
			U"ta");
		my addItem_move (espeak_languages_FileInMemorySet28.move());

		static unsigned char espeak_languages_FileInMemorySet29_data[71] = {
			110,97,109,101,32,84,101,108,117,103,117,10,108,97,110,103,117,97,103,101,32,116,101,10,10,115,116,97,116,117,
			115,32,116,101,115,116,105,110,103,10,10,105,110,116,111,110,97,116,105,111,110,32,50,10,47,47,99,111,110,115,
			111,110,97,110,116,115,32,56,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet29 = FileInMemory_createWithData (70, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet29_data), true, 
			U"./data/lang/dra/te", 
			U"te");
		my addItem_move (espeak_languages_FileInMemorySet29.move());

		static unsigned char espeak_languages_FileInMemorySet30_data[31] = {
			110,97,109,101,32,71,114,101,101,110,108,97,110,100,105,99,10,108,97,110,103,117,97,103,101,32,107,108,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet30 = FileInMemory_createWithData (30, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet30_data), true, 
			U"./data/lang/esx/kl", 
			U"kl");
		my addItem_move (espeak_languages_FileInMemorySet30.move());

		static unsigned char espeak_languages_FileInMemorySet31_data[55] = {
			110,97,109,101,32,66,97,115,113,117,101,10,108,97,110,103,117,97,103,101,32,101,117,10,10,115,116,97,116,117,
			115,32,116,101,115,116,105,110,103,10,115,116,114,101,115,115,82,117,108,101,32,49,53,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet31 = FileInMemory_createWithData (54, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet31_data), true, 
			U"./data/lang/eu", 
			U"eu");
		my addItem_move (espeak_languages_FileInMemorySet31.move());

		static unsigned char espeak_languages_FileInMemorySet32_data[44] = {
			110,97,109,101,32,68,97,110,105,115,104,10,108,97,110,103,117,97,103,101,32,100,97,10,10,116,117,110,101,115,
			32,115,50,32,99,50,32,113,50,32,101,50,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet32 = FileInMemory_createWithData (43, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet32_data), true, 
			U"./data/lang/gmq/da", 
			U"da");
		my addItem_move (espeak_languages_FileInMemorySet32.move());

		static unsigned char espeak_languages_FileInMemorySet33_data[105] = {
			110,97,109,101,32,70,97,114,111,101,115,101,10,108,97,110,103,117,97,103,101,32,102,111,10,109,97,105,110,116,
			97,105,110,101,114,32,105,83,111,108,118,101,73,84,32,65,112,83,32,40,65,110,100,114,97,115,32,69,108,105,
			97,115,115,101,110,41,32,60,97,110,100,114,97,115,64,105,115,111,108,118,101,105,116,46,110,101,116,62,10,115,
			116,97,116,117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet33 = FileInMemory_createWithData (104, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet33_data), true, 
			U"./data/lang/gmq/fo", 
			U"fo");
		my addItem_move (espeak_languages_FileInMemorySet33.move());

		static unsigned char espeak_languages_FileInMemorySet34_data[28] = {
			110,97,109,101,32,73,99,101,108,97,110,100,105,99,10,108,97,110,103,117,97,103,101,32,105,115,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet34 = FileInMemory_createWithData (27, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet34_data), true, 
			U"./data/lang/gmq/is", 
			U"is");
		my addItem_move (espeak_languages_FileInMemorySet34.move());

		static unsigned char espeak_languages_FileInMemorySet35_data[88] = {
			110,97,109,101,32,78,111,114,119,101,103,105,97,110,32,66,111,107,109,195,165,108,10,108,97,110,103,117,97,103,
			101,32,110,98,10,108,97,110,103,117,97,103,101,32,110,111,10,112,104,111,110,101,109,101,115,32,110,111,10,100,
			105,99,116,105,111,110,97,114,121,32,110,111,10,10,105,110,116,111,110,97,116,105,111,110,32,52,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet35 = FileInMemory_createWithData (87, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet35_data), true, 
			U"./data/lang/gmq/nb", 
			U"nb");
		my addItem_move (espeak_languages_FileInMemorySet35.move());

		static unsigned char espeak_languages_FileInMemorySet36_data[26] = {
			110,97,109,101,32,83,119,101,100,105,115,104,10,108,97,110,103,117,97,103,101,32,115,118,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet36 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet36_data), true, 
			U"./data/lang/gmq/sv", 
			U"sv");
		my addItem_move (espeak_languages_FileInMemorySet36.move());

		static unsigned char espeak_languages_FileInMemorySet37_data[124] = {
			110,97,109,101,32,65,102,114,105,107,97,97,110,115,10,108,97,110,103,117,97,103,101,32,97,102,10,10,109,97,
			105,110,116,97,105,110,101,114,32,67,104,114,105,115,116,111,32,100,101,32,75,108,101,114,107,32,60,99,104,114,
			105,115,116,111,100,101,107,108,101,114,107,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,
			109,97,116,117,114,101,10,10,114,111,117,103,104,110,101,115,115,32,48,10,112,105,116,99,104,32,54,51,32,49,
			50,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet37 = FileInMemory_createWithData (123, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet37_data), true, 
			U"./data/lang/gmw/af", 
			U"af");
		my addItem_move (espeak_languages_FileInMemorySet37.move());

		static unsigned char espeak_languages_FileInMemorySet38_data[43] = {
			110,97,109,101,32,71,101,114,109,97,110,10,108,97,110,103,117,97,103,101,32,100,101,10,116,117,110,101,115,32,
			115,52,32,99,49,32,113,52,32,101,49,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet38 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet38_data), true, 
			U"./data/lang/gmw/de", 
			U"de");
		my addItem_move (espeak_languages_FileInMemorySet38.move());

		static unsigned char espeak_languages_FileInMemorySet39_data[141] = {
			110,97,109,101,32,69,110,103,108,105,115,104,32,40,71,114,101,97,116,32,66,114,105,116,97,105,110,41,10,108,
			97,110,103,117,97,103,101,32,101,110,45,103,98,32,32,50,10,108,97,110,103,117,97,103,101,32,101,110,32,50,
			10,10,109,97,105,110,116,97,105,110,101,114,32,82,101,101,99,101,32,72,46,32,68,117,110,110,32,60,109,115,
			99,108,114,104,100,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,109,97,116,117,114,101,
			10,10,116,117,110,101,115,32,115,49,32,99,49,32,113,49,32,101,49,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet39 = FileInMemory_createWithData (140, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet39_data), true, 
			U"./data/lang/gmw/en", 
			U"en");
		my addItem_move (espeak_languages_FileInMemorySet39.move());

		static unsigned char espeak_languages_FileInMemorySet40_data[336] = {
			110,97,109,101,32,69,110,103,108,105,115,104,32,40,67,97,114,105,98,98,101,97,110,41,10,108,97,110,103,117,
			97,103,101,32,101,110,45,48,50,57,10,108,97,110,103,117,97,103,101,32,101,110,32,49,48,10,10,109,97,105,
			110,116,97,105,110,101,114,32,82,101,101,99,101,32,72,46,32,68,117,110,110,32,60,109,115,99,108,114,104,100,
			64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,109,97,116,117,114,101,10,10,112,104,111,
			110,101,109,101,115,32,101,110,45,119,105,10,100,105,99,116,114,117,108,101,115,32,56,10,115,116,114,101,115,115,
			76,101,110,103,116,104,32,49,55,53,32,49,55,53,32,32,49,55,53,32,49,55,53,32,32,50,50,48,32,50,
			50,48,32,32,50,53,48,32,50,57,48,10,10,114,101,112,108,97,99,101,32,48,48,32,68,32,100,10,114,101,
			112,108,97,99,101,32,48,48,32,84,32,116,91,10,114,101,112,108,97,99,101,32,48,48,32,85,64,32,111,64,
			10,114,101,112,108,97,99,101,32,48,51,32,64,32,97,35,10,114,101,112,108,97,99,101,32,48,51,32,51,32,
			97,35,10,114,101,112,108,97,99,101,32,48,51,32,78,32,110,10,10,102,111,114,109,97,110,116,32,49,32,32,
			57,56,32,32,49,48,48,32,49,48,48,10,102,111,114,109,97,110,116,32,50,32,32,57,56,32,32,49,48,48,
			32,49,48,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet40 = FileInMemory_createWithData (335, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet40_data), true, 
			U"./data/lang/gmw/en-029", 
			U"en-029");
		my addItem_move (espeak_languages_FileInMemorySet40.move());

		static unsigned char espeak_languages_FileInMemorySet41_data[296] = {
			110,97,109,101,32,69,110,103,108,105,115,104,32,40,83,99,111,116,108,97,110,100,41,10,108,97,110,103,117,97,
			103,101,32,101,110,45,103,98,45,115,99,111,116,108,97,110,100,10,108,97,110,103,117,97,103,101,32,101,110,32,
			52,10,10,109,97,105,110,116,97,105,110,101,114,32,82,101,101,99,101,32,72,46,32,68,117,110,110,32,60,109,
			115,99,108,114,104,100,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,109,97,116,117,114,
			101,10,10,112,104,111,110,101,109,101,115,32,101,110,45,115,99,10,100,105,99,116,114,117,108,101,115,32,50,32,
			53,32,54,32,55,10,115,116,114,101,115,115,76,101,110,103,116,104,32,32,49,56,48,32,49,51,48,32,50,48,
			48,32,50,48,48,32,48,32,48,32,50,53,48,32,50,55,48,10,10,114,101,112,108,97,99,101,32,48,51,32,
			64,32,86,10,114,101,112,108,97,99,101,32,48,51,32,73,32,105,10,114,101,112,108,97,99,101,32,48,51,32,
			73,50,32,105,10,114,101,112,108,97,99,101,32,48,49,32,97,73,32,97,73,50,10,114,101,112,108,97,99,101,
			32,48,50,32,97,32,97,47,10,114,101,112,108,97,99,101,32,48,50,32,117,58,32,85,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet41 = FileInMemory_createWithData (295, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet41_data), true, 
			U"./data/lang/gmw/en-GB-scotland", 
			U"en-GB-scotland");
		my addItem_move (espeak_languages_FileInMemorySet41.move());

		static unsigned char espeak_languages_FileInMemorySet42_data[239] = {
			110,97,109,101,32,69,110,103,108,105,115,104,32,40,76,97,110,99,97,115,116,101,114,41,10,108,97,110,103,117,
			97,103,101,32,101,110,45,103,98,45,120,45,103,98,99,108,97,110,10,108,97,110,103,117,97,103,101,32,101,110,
			45,103,98,32,32,51,10,108,97,110,103,117,97,103,101,32,101,110,32,53,10,10,109,97,105,110,116,97,105,110,
			101,114,32,82,101,101,99,101,32,72,46,32,68,117,110,110,32,60,109,115,99,108,114,104,100,64,103,109,97,105,
			108,46,99,111,109,62,10,115,116,97,116,117,115,32,109,97,116,117,114,101,10,10,112,104,111,110,101,109,101,115,
			32,101,110,45,110,10,10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,54,48,32,49,53,48,32,32,49,
			56,48,32,49,56,48,32,32,50,50,48,32,50,50,48,32,32,50,57,48,32,50,57,48,10,10,114,101,112,108,
			97,99,101,32,48,51,32,78,32,110,10,114,101,112,108,97,99,101,32,48,51,32,105,32,32,73,50,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet42 = FileInMemory_createWithData (238, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet42_data), true, 
			U"./data/lang/gmw/en-GB-x-gbclan", 
			U"en-GB-x-gbclan");
		my addItem_move (espeak_languages_FileInMemorySet42.move());

		static unsigned char espeak_languages_FileInMemorySet43_data[189] = {
			110,97,109,101,32,69,110,103,108,105,115,104,32,40,87,101,115,116,32,77,105,100,108,97,110,100,115,41,10,108,
			97,110,103,117,97,103,101,32,101,110,45,103,98,45,120,45,103,98,99,119,109,100,10,108,97,110,103,117,97,103,
			101,32,101,110,45,103,98,32,57,10,108,97,110,103,117,97,103,101,32,101,110,32,57,10,10,112,104,111,110,101,
			109,101,115,32,101,110,45,119,109,10,10,114,101,112,108,97,99,101,32,48,48,32,104,32,78,85,76,76,10,114,
			101,112,108,97,99,101,32,48,48,32,111,64,32,79,64,10,100,105,99,116,114,117,108,101,115,32,54,10,105,110,
			116,111,110,97,116,105,111,110,32,52,10,115,116,114,101,115,115,65,100,100,32,48,32,48,32,48,32,48,32,48,
			32,48,32,48,32,50,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet43 = FileInMemory_createWithData (188, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet43_data), true, 
			U"./data/lang/gmw/en-GB-x-gbcwmd", 
			U"en-GB-x-gbcwmd");
		my addItem_move (espeak_languages_FileInMemorySet43.move());

		static unsigned char espeak_languages_FileInMemorySet44_data[250] = {
			110,97,109,101,32,69,110,103,108,105,115,104,32,40,82,101,99,101,105,118,101,100,32,80,114,111,110,117,110,99,
			105,97,116,105,111,110,41,10,108,97,110,103,117,97,103,101,32,101,110,45,103,98,45,120,45,114,112,10,108,97,
			110,103,117,97,103,101,32,101,110,45,103,98,32,32,52,10,108,97,110,103,117,97,103,101,32,101,110,32,53,10,
			10,109,97,105,110,116,97,105,110,101,114,32,82,101,101,99,101,32,72,46,32,68,117,110,110,32,60,109,115,99,
			108,114,104,100,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,109,97,116,117,114,101,10,
			10,112,104,111,110,101,109,101,115,32,101,110,45,114,112,10,10,114,101,112,108,97,99,101,32,48,48,32,32,111,
			64,32,32,79,64,10,114,101,112,108,97,99,101,32,48,51,32,73,32,105,10,114,101,112,108,97,99,101,32,48,
			51,32,73,50,32,105,10,114,101,112,108,97,99,101,32,48,51,32,64,32,97,35,10,114,101,112,108,97,99,101,
			32,48,51,32,51,32,97,35,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet44 = FileInMemory_createWithData (249, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet44_data), true, 
			U"./data/lang/gmw/en-GB-x-rp", 
			U"en-GB-x-rp");
		my addItem_move (espeak_languages_FileInMemorySet44.move());

		static unsigned char espeak_languages_FileInMemorySet45_data[258] = {
			110,97,109,101,32,69,110,103,108,105,115,104,32,40,65,109,101,114,105,99,97,41,10,108,97,110,103,117,97,103,
			101,32,101,110,45,117,115,32,50,10,108,97,110,103,117,97,103,101,32,101,110,32,51,10,10,109,97,105,110,116,
			97,105,110,101,114,32,82,101,101,99,101,32,72,46,32,68,117,110,110,32,60,109,115,99,108,114,104,100,64,103,
			109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,109,97,116,117,114,101,10,10,112,104,111,110,101,
			109,101,115,32,101,110,45,117,115,10,100,105,99,116,114,117,108,101,115,32,51,32,54,10,10,115,116,114,101,115,
			115,76,101,110,103,116,104,32,49,52,48,32,49,50,48,32,49,57,48,32,49,55,48,32,48,32,48,32,50,53,
			53,32,51,48,48,10,115,116,114,101,115,115,65,109,112,32,32,49,55,32,49,54,32,32,49,57,32,49,57,32,
			32,49,57,32,49,57,32,32,50,49,32,49,57,10,10,114,101,112,108,97,99,101,32,48,51,32,73,32,32,105,
			10,114,101,112,108,97,99,101,32,48,51,32,73,50,32,105,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet45 = FileInMemory_createWithData (257, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet45_data), true, 
			U"./data/lang/gmw/en-US", 
			U"en-US");
		my addItem_move (espeak_languages_FileInMemorySet45.move());

		static unsigned char espeak_languages_FileInMemorySet46_data[272] = {
			110,97,109,101,32,69,110,103,108,105,115,104,32,40,65,109,101,114,105,99,97,44,32,78,101,119,32,89,111,114,
			107,32,67,105,116,121,41,10,108,97,110,103,117,97,103,101,32,101,110,45,117,115,45,110,121,99,10,10,109,97,
			105,110,116,97,105,110,101,114,32,82,105,99,104,97,114,100,32,67,97,108,118,105,32,60,114,105,99,104,97,114,
			100,46,99,97,108,118,105,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,116,101,115,116,
			105,110,103,10,10,112,104,111,110,101,109,101,115,32,101,110,45,117,115,45,110,121,99,10,100,105,99,116,114,117,
			108,101,115,32,51,32,54,10,10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,52,48,32,49,50,48,32,
			49,57,48,32,49,55,48,32,48,32,48,32,50,53,53,32,51,48,48,10,115,116,114,101,115,115,65,109,112,32,
			32,49,55,32,49,54,32,32,49,57,32,49,57,32,32,49,57,32,49,57,32,32,50,49,32,49,57,10,10,114,
			101,112,108,97,99,101,32,48,51,32,73,32,32,105,10,114,101,112,108,97,99,101,32,48,51,32,73,50,32,105,
			10,0};
		autoFileInMemory espeak_languages_FileInMemorySet46 = FileInMemory_createWithData (271, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet46_data), true, 
			U"./data/lang/gmw/en-US-nyc", 
			U"en-US-nyc");
		my addItem_move (espeak_languages_FileInMemorySet46.move());

		static unsigned char espeak_languages_FileInMemorySet47_data[32] = {
			110,97,109,101,32,76,117,120,101,109,98,111,117,114,103,105,115,104,10,108,97,110,103,117,97,103,101,32,108,98,
			10,0};
		autoFileInMemory espeak_languages_FileInMemorySet47 = FileInMemory_createWithData (31, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet47_data), true, 
			U"./data/lang/gmw/lb", 
			U"lb");
		my addItem_move (espeak_languages_FileInMemorySet47.move());

		static unsigned char espeak_languages_FileInMemorySet48_data[24] = {
			110,97,109,101,32,68,117,116,99,104,10,108,97,110,103,117,97,103,101,32,110,108,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet48 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet48_data), true, 
			U"./data/lang/gmw/nl", 
			U"nl");
		my addItem_move (espeak_languages_FileInMemorySet48.move());

		static unsigned char espeak_languages_FileInMemorySet49_data[24] = {
			110,97,109,101,32,71,114,101,101,107,10,108,97,110,103,117,97,103,101,32,101,108,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet49 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet49_data), true, 
			U"./data/lang/grk/el", 
			U"el");
		my addItem_move (espeak_languages_FileInMemorySet49.move());

		static unsigned char espeak_languages_FileInMemorySet50_data[100] = {
			110,97,109,101,32,71,114,101,101,107,32,40,65,110,99,105,101,110,116,41,10,108,97,110,103,117,97,103,101,32,
			103,114,99,10,10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,55,48,32,49,55,48,32,32,49,57,48,
			32,49,57,48,32,32,48,32,48,32,32,50,51,48,32,50,52,48,10,100,105,99,116,114,117,108,101,115,32,49,
			10,119,111,114,100,115,32,51,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet50 = FileInMemory_createWithData (99, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet50_data), true, 
			U"./data/lang/grk/grc", 
			U"grc");
		my addItem_move (espeak_languages_FileInMemorySet50.move());

		static unsigned char espeak_languages_FileInMemorySet51_data[43] = {
			110,97,109,101,32,65,115,115,97,109,101,115,101,10,108,97,110,103,117,97,103,101,32,97,115,10,10,115,116,97,
			116,117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet51 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet51_data), true, 
			U"./data/lang/inc/as", 
			U"as");
		my addItem_move (espeak_languages_FileInMemorySet51.move());

		static unsigned char espeak_languages_FileInMemorySet52_data[26] = {
			110,97,109,101,32,66,101,110,103,97,108,105,10,108,97,110,103,117,97,103,101,32,98,110,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet52 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet52_data), true, 
			U"./data/lang/inc/bn", 
			U"bn");
		my addItem_move (espeak_languages_FileInMemorySet52.move());

		static unsigned char espeak_languages_FileInMemorySet53_data[40] = {
			110,97,109,101,32,66,105,115,104,110,117,112,114,105,121,97,32,77,97,110,105,112,117,114,105,10,108,97,110,103,
			117,97,103,101,32,98,112,121,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet53 = FileInMemory_createWithData (39, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet53_data), true, 
			U"./data/lang/inc/bpy", 
			U"bpy");
		my addItem_move (espeak_languages_FileInMemorySet53.move());

		static unsigned char espeak_languages_FileInMemorySet54_data[43] = {
			110,97,109,101,32,71,117,106,97,114,97,116,105,10,108,97,110,103,117,97,103,101,32,103,117,10,10,115,116,97,
			116,117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet54 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet54_data), true, 
			U"./data/lang/inc/gu", 
			U"gu");
		my addItem_move (espeak_languages_FileInMemorySet54.move());

		static unsigned char espeak_languages_FileInMemorySet55_data[24] = {
			110,97,109,101,32,72,105,110,100,105,10,108,97,110,103,117,97,103,101,32,104,105,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet55 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet55_data), true, 
			U"./data/lang/inc/hi", 
			U"hi");
		my addItem_move (espeak_languages_FileInMemorySet55.move());

		static unsigned char espeak_languages_FileInMemorySet56_data[27] = {
			110,97,109,101,32,75,111,110,107,97,110,105,10,108,97,110,103,117,97,103,101,32,107,111,107,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet56 = FileInMemory_createWithData (26, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet56_data), true, 
			U"./data/lang/inc/kok", 
			U"kok");
		my addItem_move (espeak_languages_FileInMemorySet56.move());

		static unsigned char espeak_languages_FileInMemorySet57_data[42] = {
			110,97,109,101,32,77,97,114,97,116,104,105,10,108,97,110,103,117,97,103,101,32,109,114,10,10,115,116,97,116,
			117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet57 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet57_data), true, 
			U"./data/lang/inc/mr", 
			U"mr");
		my addItem_move (espeak_languages_FileInMemorySet57.move());

		static unsigned char espeak_languages_FileInMemorySet58_data[38] = {
			110,97,109,101,32,78,101,112,97,108,105,10,108,97,110,103,117,97,103,101,32,110,101,10,10,100,105,99,116,114,
			117,108,101,115,32,49,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet58 = FileInMemory_createWithData (37, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet58_data), true, 
			U"./data/lang/inc/ne", 
			U"ne");
		my addItem_move (espeak_languages_FileInMemorySet58.move());

		static unsigned char espeak_languages_FileInMemorySet59_data[40] = {
			110,97,109,101,32,79,114,105,121,97,10,108,97,110,103,117,97,103,101,32,111,114,10,10,115,116,97,116,117,115,
			32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet59 = FileInMemory_createWithData (39, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet59_data), true, 
			U"./data/lang/inc/or", 
			U"or");
		my addItem_move (espeak_languages_FileInMemorySet59.move());

		static unsigned char espeak_languages_FileInMemorySet60_data[26] = {
			110,97,109,101,32,80,117,110,106,97,98,105,10,108,97,110,103,117,97,103,101,32,112,97,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet60 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet60_data), true, 
			U"./data/lang/inc/pa", 
			U"pa");
		my addItem_move (espeak_languages_FileInMemorySet60.move());

		static unsigned char espeak_languages_FileInMemorySet61_data[67] = {
			110,97,109,101,32,83,105,110,100,104,105,10,108,97,110,103,117,97,103,101,32,115,100,10,109,97,105,110,116,97,
			105,110,101,114,32,69,106,97,122,32,83,104,97,104,32,60,101,97,115,104,97,104,54,55,64,103,109,97,105,108,
			46,99,111,109,62,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet61 = FileInMemory_createWithData (66, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet61_data), true, 
			U"./data/lang/inc/sd", 
			U"sd");
		my addItem_move (espeak_languages_FileInMemorySet61.move());

		static unsigned char espeak_languages_FileInMemorySet62_data[56] = {
			110,97,109,101,32,83,105,110,104,97,108,97,10,108,97,110,103,117,97,103,101,32,115,105,10,10,115,116,97,116,
			117,115,32,116,101,115,116,105,110,103,10,10,105,110,116,111,110,97,116,105,111,110,32,50,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet62 = FileInMemory_createWithData (55, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet62_data), true, 
			U"./data/lang/inc/si", 
			U"si");
		my addItem_move (espeak_languages_FileInMemorySet62.move());

		static unsigned char espeak_languages_FileInMemorySet63_data[95] = {
			110,97,109,101,32,85,114,100,117,10,108,97,110,103,117,97,103,101,32,117,114,10,109,97,105,110,116,97,105,110,
			101,114,32,69,106,97,122,32,83,104,97,104,32,60,101,97,115,104,97,104,54,55,64,103,109,97,105,108,46,99,
			111,109,62,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,10,115,116,114,101,115,115,82,117,108,101,
			32,54,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet63 = FileInMemory_createWithData (94, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet63_data), true, 
			U"./data/lang/inc/ur", 
			U"ur");
		my addItem_move (espeak_languages_FileInMemorySet63.move());

		static unsigned char espeak_languages_FileInMemorySet64_data[62] = {
			110,97,109,101,32,65,114,109,101,110,105,97,110,32,40,69,97,115,116,32,65,114,109,101,110,105,97,41,10,108,
			97,110,103,117,97,103,101,32,104,121,10,108,97,110,103,117,97,103,101,32,104,121,45,97,114,101,118,101,108,97,
			10,0};
		autoFileInMemory espeak_languages_FileInMemorySet64 = FileInMemory_createWithData (61, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet64_data), true, 
			U"./data/lang/ine/hy", 
			U"hy");
		my addItem_move (espeak_languages_FileInMemorySet64.move());

		static unsigned char espeak_languages_FileInMemorySet65_data[366] = {
			110,97,109,101,32,65,114,109,101,110,105,97,110,32,40,87,101,115,116,32,65,114,109,101,110,105,97,41,10,108,
			97,110,103,117,97,103,101,32,104,121,119,10,108,97,110,103,117,97,103,101,32,104,121,45,97,114,101,118,109,100,
			97,10,108,97,110,103,117,97,103,101,32,104,121,32,32,56,10,10,100,105,99,116,105,111,110,97,114,121,32,104,
			121,10,100,105,99,116,114,117,108,101,115,32,49,10,10,112,104,111,110,101,109,101,115,32,104,121,10,10,47,47,
			32,99,104,97,110,103,101,32,99,111,110,115,111,110,97,110,116,115,32,102,111,114,32,87,101,115,116,32,65,114,
			109,101,110,105,97,110,32,112,114,111,110,117,110,99,105,97,116,105,111,110,10,114,101,112,108,97,99,101,32,48,
			48,32,98,32,32,112,35,10,114,101,112,108,97,99,101,32,48,48,32,100,32,32,116,35,10,114,101,112,108,97,
			99,101,32,48,48,32,100,122,32,116,115,35,10,114,101,112,108,97,99,101,32,48,48,32,100,90,32,116,83,35,
			10,114,101,112,108,97,99,101,32,48,48,32,103,32,32,107,35,10,10,114,101,112,108,97,99,101,32,48,48,32,
			112,32,32,98,10,114,101,112,108,97,99,101,32,48,48,32,116,32,32,100,10,114,101,112,108,97,99,101,32,48,
			48,32,116,115,32,100,122,10,114,101,112,108,97,99,101,32,48,48,32,116,83,32,100,90,10,114,101,112,108,97,
			99,101,32,48,48,32,107,32,32,103,10,10,114,101,112,108,97,99,101,32,48,48,32,82,50,32,82,32,32,47,
			47,32,63,63,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet65 = FileInMemory_createWithData (365, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet65_data), true, 
			U"./data/lang/ine/hyw", 
			U"hyw");
		my addItem_move (espeak_languages_FileInMemorySet65.move());

		static unsigned char espeak_languages_FileInMemorySet66_data[104] = {
			110,97,109,101,32,65,108,98,97,110,105,97,110,10,108,97,110,103,117,97,103,101,32,115,113,10,10,47,47,32,
			97,100,100,32,116,104,105,115,32,108,105,110,101,32,116,111,32,114,101,109,111,118,101,32,39,195,171,39,32,97,
			116,32,116,104,101,32,101,110,100,32,111,102,32,119,111,114,100,115,10,47,47,32,114,101,112,108,97,99,101,32,
			48,48,32,32,64,47,32,32,78,85,76,76,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet66 = FileInMemory_createWithData (103, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet66_data), true, 
			U"./data/lang/ine/sq", 
			U"sq");
		my addItem_move (espeak_languages_FileInMemorySet66.move());

		static unsigned char espeak_languages_FileInMemorySet67_data[91] = {
			110,97,109,101,32,80,101,114,115,105,97,110,10,108,97,110,103,117,97,103,101,32,102,97,10,109,97,105,110,116,
			97,105,110,101,114,32,83,104,97,100,121,97,114,32,75,104,111,100,97,121,97,114,105,32,60,115,104,97,100,121,
			97,114,56,49,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,109,97,116,117,114,101,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet67 = FileInMemory_createWithData (90, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet67_data), true, 
			U"./data/lang/ira/fa", 
			U"fa");
		my addItem_move (espeak_languages_FileInMemorySet67.move());

		static unsigned char espeak_languages_FileInMemorySet68_data[270] = {
			110,97,109,101,32,80,101,114,115,105,97,110,32,40,80,105,110,103,108,105,115,104,41,10,47,47,32,83,111,109,
			101,116,105,109,101,115,44,32,70,97,114,115,105,32,115,112,101,97,107,101,114,115,32,119,114,105,116,101,32,70,
			97,114,115,105,32,119,111,114,100,115,32,117,115,105,110,103,32,69,110,103,108,105,115,104,32,99,104,97,114,97,
			99,116,101,114,115,44,32,112,97,114,116,105,99,117,108,97,114,108,121,32,105,110,32,67,104,97,116,32,97,110,
			100,32,83,77,83,32,40,116,101,120,116,101,32,109,101,115,115,97,103,101,115,41,46,41,44,32,99,97,108,108,
			101,100,32,80,105,110,103,108,105,115,104,10,108,97,110,103,117,97,103,101,32,102,97,45,108,97,116,110,10,109,
			97,105,110,116,97,105,110,101,114,32,83,104,97,100,121,97,114,32,75,104,111,100,97,121,97,114,105,32,60,115,
			104,97,100,121,97,114,56,49,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,109,97,116,
			117,114,101,10,100,105,99,116,114,117,108,101,115,32,49,10,112,104,111,110,101,109,101,115,32,102,97,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet68 = FileInMemory_createWithData (269, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet68_data), true, 
			U"./data/lang/ira/fa-Latn", 
			U"fa-Latn");
		my addItem_move (espeak_languages_FileInMemorySet68.move());

		static unsigned char espeak_languages_FileInMemorySet69_data[41] = {
			110,97,109,101,32,75,117,114,100,105,115,104,10,108,97,110,103,117,97,103,101,32,107,117,10,10,47,47,119,111,
			114,100,115,32,49,32,52,56,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet69 = FileInMemory_createWithData (40, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet69_data), true, 
			U"./data/lang/ira/ku", 
			U"ku");
		my addItem_move (espeak_languages_FileInMemorySet69.move());

		static unsigned char espeak_languages_FileInMemorySet70_data[570] = {
			110,97,109,101,32,67,104,101,114,111,107,101,101,32,47,47,104,116,116,112,115,58,47,47,103,105,116,104,117,98,
			46,99,111,109,47,101,115,112,101,97,107,45,110,103,47,101,115,112,101,97,107,45,110,103,47,98,108,111,98,47,
			109,97,115,116,101,114,47,100,111,99,115,47,118,111,105,99,101,115,46,109,100,10,108,97,110,103,117,97,103,101,
			32,99,104,114,45,85,83,45,81,97,97,97,45,120,45,119,101,115,116,32,53,10,10,109,97,105,110,116,97,105,
			110,101,114,32,77,105,99,104,97,101,108,32,67,111,110,114,97,100,32,60,109,46,99,111,110,114,97,100,46,50,
			48,50,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,10,
			112,105,116,99,104,32,57,48,32,49,54,48,10,10,118,111,105,99,105,110,103,32,49,48,48,10,10,99,111,110,
			115,111,110,97,110,116,115,32,49,48,48,32,49,48,48,10,10,115,112,101,101,100,32,49,48,48,10,10,119,111,
			114,100,115,32,50,32,49,10,10,112,104,111,110,101,109,101,115,32,99,104,114,10,10,47,47,115,116,114,101,115,
			115,32,111,110,32,97,108,108,32,115,121,108,108,97,98,108,101,115,32,116,111,32,115,105,109,117,108,97,116,101,
			32,115,116,114,101,115,115,32,111,110,32,110,111,32,115,121,108,108,97,98,108,101,115,10,115,116,114,101,115,115,
			82,117,108,101,32,57,32,10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,55,53,32,49,55,53,32,49,
			55,53,32,49,55,53,32,49,55,53,32,49,55,53,32,49,55,53,32,49,55,53,32,47,47,97,108,108,32,118,
			111,119,101,108,115,32,116,104,101,32,115,97,109,101,32,108,101,110,103,116,104,32,114,101,103,97,114,100,108,101,
			115,115,32,111,102,32,115,116,114,101,115,115,10,115,116,114,101,115,115,65,109,112,32,49,48,32,49,48,32,49,
			48,32,49,48,32,49,48,32,49,48,32,49,48,32,49,48,32,32,47,47,97,108,108,32,118,111,119,101,108,115,
			32,116,104,101,32,115,97,109,101,32,115,116,114,101,110,103,116,104,32,114,101,103,97,114,100,108,101,115,115,32,
			111,102,32,109,97,114,107,101,100,32,115,116,114,101,115,115,10,10,105,110,116,111,110,97,116,105,111,110,32,49,
			10,10,116,117,110,101,115,9,99,104,114,115,32,99,104,114,99,32,99,104,114,113,32,99,104,114,101,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet70 = FileInMemory_createWithData (569, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet70_data), true, 
			U"./data/lang/iro/chr", 
			U"chr");
		my addItem_move (espeak_languages_FileInMemorySet70.move());

		static unsigned char espeak_languages_FileInMemorySet71_data[298] = {
			110,97,109,101,32,76,97,116,105,110,10,108,97,110,103,117,97,103,101,32,108,97,10,115,116,114,101,115,115,82,
			117,108,101,32,50,32,48,32,50,10,47,47,32,114,117,108,101,61,112,101,110,117,108,116,105,109,97,116,101,10,
			47,47,32,117,110,115,116,114,101,115,115,101,100,95,119,100,49,61,48,10,47,47,32,117,110,115,116,114,101,115,
			115,101,100,95,119,100,50,61,50,10,115,116,114,101,115,115,79,112,116,32,48,32,53,32,47,47,32,102,108,97,
			103,115,61,48,49,48,48,48,48,49,32,40,110,111,32,97,117,116,111,109,97,116,105,99,32,115,101,99,111,110,
			100,97,114,121,32,115,116,114,101,115,115,32,43,32,100,111,110,39,116,32,115,116,114,101,115,32,109,111,110,111,
			115,121,108,108,97,98,108,101,115,41,10,10,47,47,32,115,104,111,114,116,32,103,97,112,32,98,101,116,119,101,
			101,110,32,119,111,114,100,115,10,119,111,114,100,115,32,50,10,10,47,47,32,78,111,116,101,58,32,84,104,101,
			32,76,97,116,105,110,32,118,111,105,99,101,32,110,101,101,100,115,32,108,111,110,103,32,118,111,119,101,108,115,
			32,116,111,32,98,101,32,109,97,114,107,101,100,32,119,105,116,104,32,109,97,99,114,111,110,115,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet71 = FileInMemory_createWithData (297, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet71_data), true, 
			U"./data/lang/itc/la", 
			U"la");
		my addItem_move (espeak_languages_FileInMemorySet71.move());

		static unsigned char espeak_languages_FileInMemorySet72_data[53] = {
			110,97,109,101,32,74,97,112,97,110,101,115,101,10,108,97,110,103,117,97,103,101,32,106,97,10,112,104,111,110,
			101,109,101,115,32,106,97,10,10,105,110,116,111,110,97,116,105,111,110,32,52,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet72 = FileInMemory_createWithData (52, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet72_data), true, 
			U"./data/lang/jpx/ja", 
			U"ja");
		my addItem_move (espeak_languages_FileInMemorySet72.move());

		static unsigned char espeak_languages_FileInMemorySet73_data[52] = {
			110,97,109,101,32,75,111,114,101,97,110,10,108,97,110,103,117,97,103,101,32,107,111,10,112,105,116,99,104,32,
			56,48,32,49,49,56,10,105,110,116,111,110,97,116,105,111,110,32,50,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet73 = FileInMemory_createWithData (51, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet73_data), true, 
			U"./data/lang/ko", 
			U"ko");
		my addItem_move (espeak_languages_FileInMemorySet73.move());

		static unsigned char espeak_languages_FileInMemorySet74_data[43] = {
			110,97,109,101,32,72,97,119,97,105,105,97,110,10,108,97,110,103,117,97,103,101,32,104,97,119,10,115,116,97,
			116,117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet74 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet74_data), true, 
			U"./data/lang/map/haw", 
			U"haw");
		my addItem_move (espeak_languages_FileInMemorySet74.move());

		static unsigned char espeak_languages_FileInMemorySet75_data[184] = {
			110,97,109,101,32,84,111,116,111,110,116,101,112,101,99,32,77,105,120,101,10,108,97,110,103,117,97,103,101,32,
			109,116,111,10,10,109,97,105,110,116,97,105,110,101,114,32,66,105,108,108,32,68,101,110,103,108,101,114,32,60,
			99,111,100,101,111,102,100,117,115,107,64,103,109,97,105,108,46,99,111,109,62,32,97,110,100,32,69,108,105,122,
			97,98,101,116,104,32,82,101,115,101,110,100,105,122,32,60,101,46,114,46,114,101,115,101,110,100,105,122,55,64,
			103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,10,108,111,119,
			101,114,99,97,115,101,83,101,110,116,101,110,99,101,10,116,117,110,101,115,32,115,54,32,99,54,32,113,54,32,
			101,54,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet75 = FileInMemory_createWithData (183, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet75_data), true, 
			U"./data/lang/miz/mto", 
			U"mto");
		my addItem_move (espeak_languages_FileInMemorySet75.move());

		static unsigned char espeak_languages_FileInMemorySet76_data[211] = {
			110,97,109,101,32,75,39,105,99,104,101,39,10,108,97,110,103,117,97,103,101,32,113,117,99,10,115,116,97,116,
			117,115,32,116,101,115,116,105,110,103,10,115,116,114,101,115,115,82,117,108,101,32,51,32,47,47,32,115,116,114,
			101,115,115,32,111,110,32,102,105,110,97,108,32,115,121,108,108,97,98,108,101,10,115,116,114,101,115,115,65,109,
			112,32,56,32,56,32,50,48,32,49,53,32,48,32,48,32,50,53,32,50,53,32,32,47,47,32,114,101,100,117,
			99,101,32,117,110,115,116,114,101,115,115,101,100,32,118,111,119,101,108,115,10,115,116,114,101,115,115,76,101,110,
			103,116,104,32,49,50,48,32,49,50,48,32,50,48,48,32,49,53,48,32,48,32,48,32,50,53,48,32,50,53,
			48,32,47,47,32,114,101,100,117,99,101,32,117,110,115,116,114,101,115,115,101,100,32,118,111,119,101,108,115,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet76 = FileInMemory_createWithData (210, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet76_data), true, 
			U"./data/lang/myn/quc", 
			U"quc");
		my addItem_move (espeak_languages_FileInMemorySet76.move());

		static unsigned char espeak_languages_FileInMemorySet77_data[135] = {
			110,97,109,101,32,73,110,100,111,110,101,115,105,97,110,10,108,97,110,103,117,97,103,101,32,105,100,10,10,115,
			116,114,101,115,115,76,101,110,103,116,104,32,49,54,48,32,50,48,48,32,32,49,56,48,32,49,56,48,32,32,
			48,32,48,32,32,50,50,48,32,50,52,48,10,115,116,114,101,115,115,65,109,112,32,32,32,32,49,54,32,32,
			49,56,32,32,32,49,56,32,32,49,56,32,32,32,48,32,48,32,32,50,50,32,32,50,49,10,10,99,111,110,
			115,111,110,97,110,116,115,32,56,48,32,56,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet77 = FileInMemory_createWithData (134, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet77_data), true, 
			U"./data/lang/poz/id", 
			U"id");
		my addItem_move (espeak_languages_FileInMemorySet77.move());

		static unsigned char espeak_languages_FileInMemorySet78_data[368] = {
			110,97,109,101,32,77,196,129,111,114,105,10,108,97,110,103,117,97,103,101,32,109,105,10,115,116,97,116,117,115,
			32,116,101,115,116,105,110,103,10,10,47,47,32,104,116,116,112,115,58,47,47,103,105,116,104,117,98,46,99,111,
			109,47,101,115,112,101,97,107,45,110,103,47,101,115,112,101,97,107,45,110,103,47,98,108,111,98,47,109,97,115,
			116,101,114,47,100,111,99,115,47,118,111,105,99,101,115,46,109,100,35,119,111,114,100,115,10,119,111,114,100,115,
			32,49,32,50,10,10,47,47,32,116,97,107,101,110,32,102,114,111,109,32,74,97,99,107,121,10,112,105,116,99,
			104,32,32,49,49,53,32,49,51,48,10,10,102,111,114,109,97,110,116,32,48,32,49,53,48,32,49,53,53,32,
			49,48,48,10,102,111,114,109,97,110,116,32,49,32,57,48,32,49,53,53,32,55,48,10,102,111,114,109,97,110,
			116,32,50,32,57,53,32,55,48,32,54,52,10,102,111,114,109,97,110,116,32,51,32,49,53,32,50,48,32,51,
			48,10,102,111,114,109,97,110,116,32,52,32,50,48,32,51,48,32,52,48,10,102,111,114,109,97,110,116,32,53,
			32,54,53,32,50,48,32,54,53,10,102,111,114,109,97,110,116,32,54,32,55,48,32,56,48,32,49,48,48,10,
			102,111,114,109,97,110,116,32,55,32,50,48,32,56,48,32,49,48,48,10,102,111,114,109,97,110,116,32,56,32,
			49,48,48,32,57,53,32,56,48,10,118,111,105,99,105,110,103,32,49,51,53,10,99,111,110,115,111,110,97,110,
			116,115,32,49,49,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet78 = FileInMemory_createWithData (367, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet78_data), true, 
			U"./data/lang/poz/mi", 
			U"mi");
		my addItem_move (espeak_languages_FileInMemorySet78.move());

		static unsigned char espeak_languages_FileInMemorySet79_data[431] = {
			47,47,32,76,97,115,116,32,117,112,100,97,116,101,100,58,32,49,52,32,79,99,116,111,98,101,114,32,50,48,
			49,48,44,32,74,97,115,111,110,32,79,110,103,32,40,106,97,115,111,110,64,112,111,114,116,97,108,103,114,111,
			111,118,101,46,99,111,109,41,10,110,97,109,101,32,77,97,108,97,121,10,108,97,110,103,117,97,103,101,32,109,
			115,10,112,104,111,110,101,109,101,115,32,105,100,10,10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,54,
			48,32,50,48,48,32,32,49,56,48,32,49,56,48,32,32,48,32,48,32,32,50,50,48,32,50,52,48,10,115,
			116,114,101,115,115,65,109,112,32,32,32,32,49,54,32,32,49,56,32,32,32,49,56,32,32,49,56,32,32,32,
			48,32,48,32,32,50,50,32,32,50,49,10,105,110,116,111,110,97,116,105,111,110,9,51,9,47,47,32,76,101,
			115,115,32,105,110,116,111,110,97,116,105,111,110,44,32,97,110,100,32,99,111,109,109,97,32,100,111,101,115,32,
			110,111,116,32,114,97,105,115,101,32,116,104,101,32,112,105,116,99,104,46,10,10,47,47,32,78,117,97,110,99,
			101,32,45,32,80,101,110,105,110,115,117,108,97,32,77,97,108,97,121,115,105,97,10,47,47,32,114,101,112,108,
			97,99,101,9,51,32,97,9,64,9,47,47,32,99,104,97,110,103,101,32,39,115,97,121,97,39,32,116,111,32,
			39,115,97,121,101,39,10,9,9,9,9,47,47,32,40,111,110,108,121,32,116,104,101,32,108,97,115,116,32,112,
			104,111,110,101,109,101,32,111,102,32,97,32,119,111,114,100,44,32,111,110,108,121,32,105,110,32,117,110,115,116,
			114,101,115,115,101,100,32,115,121,108,108,97,98,108,101,115,41,10,9,9,9,9,10,99,111,110,115,111,110,97,
			110,116,115,32,56,48,32,56,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet79 = FileInMemory_createWithData (430, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet79_data), true, 
			U"./data/lang/poz/ms", 
			U"ms");
		my addItem_move (espeak_languages_FileInMemorySet79.move());

		static unsigned char espeak_languages_FileInMemorySet80_data[89] = {
			110,97,109,101,32,81,117,101,99,104,117,97,10,108,97,110,103,117,97,103,101,32,113,117,10,115,116,114,101,115,
			115,82,117,108,101,32,50,32,47,47,32,115,116,114,101,115,115,32,111,110,32,112,101,110,117,108,116,105,109,97,
			116,101,32,115,121,108,108,97,98,108,101,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet80 = FileInMemory_createWithData (88, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet80_data), true, 
			U"./data/lang/qu", 
			U"qu");
		my addItem_move (espeak_languages_FileInMemorySet80.move());

		static unsigned char espeak_languages_FileInMemorySet81_data[28] = {
			110,97,109,101,32,65,114,97,103,111,110,101,115,101,10,108,97,110,103,117,97,103,101,32,97,110,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet81 = FileInMemory_createWithData (27, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet81_data), true, 
			U"./data/lang/roa/an", 
			U"an");
		my addItem_move (espeak_languages_FileInMemorySet81.move());

		static unsigned char espeak_languages_FileInMemorySet82_data[26] = {
			110,97,109,101,32,67,97,116,97,108,97,110,10,108,97,110,103,117,97,103,101,32,99,97,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet82 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet82_data), true, 
			U"./data/lang/roa/ca", 
			U"ca");
		my addItem_move (espeak_languages_FileInMemorySet82.move());

		static unsigned char espeak_languages_FileInMemorySet83_data[64] = {
			110,97,109,101,32,83,112,97,110,105,115,104,32,40,83,112,97,105,110,41,10,108,97,110,103,117,97,103,101,32,
			101,115,10,100,105,99,116,114,117,108,101,115,32,49,10,116,117,110,101,115,32,115,54,32,99,54,32,113,54,32,
			101,54,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet83 = FileInMemory_createWithData (63, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet83_data), true, 
			U"./data/lang/roa/es", 
			U"es");
		my addItem_move (espeak_languages_FileInMemorySet83.move());

		static unsigned char espeak_languages_FileInMemorySet84_data[168] = {
			110,97,109,101,32,83,112,97,110,105,115,104,32,40,76,97,116,105,110,32,65,109,101,114,105,99,97,41,10,108,
			97,110,103,117,97,103,101,32,101,115,45,52,49,57,10,108,97,110,103,117,97,103,101,32,101,115,45,109,120,32,
			54,10,10,112,104,111,110,101,109,101,115,32,101,115,45,108,97,10,100,105,99,116,114,117,108,101,115,32,50,10,
			105,110,116,111,110,97,116,105,111,110,32,50,10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,55,48,32,
			50,48,48,32,32,50,51,48,32,49,56,48,32,32,48,32,48,32,32,50,53,48,32,50,56,48,10,10,116,117,
			110,101,115,32,115,54,32,99,54,32,113,54,32,101,54,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet84 = FileInMemory_createWithData (167, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet84_data), true, 
			U"./data/lang/roa/es-419", 
			U"es-419");
		my addItem_move (espeak_languages_FileInMemorySet84.move());

		static unsigned char espeak_languages_FileInMemorySet85_data[80] = {
			110,97,109,101,32,70,114,101,110,99,104,32,40,70,114,97,110,99,101,41,10,108,97,110,103,117,97,103,101,32,
			102,114,45,102,114,10,108,97,110,103,117,97,103,101,32,102,114,10,10,100,105,99,116,114,117,108,101,115,32,49,
			10,116,117,110,101,115,32,115,51,32,99,51,32,113,51,32,101,51,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet85 = FileInMemory_createWithData (79, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet85_data), true, 
			U"./data/lang/roa/fr", 
			U"fr");
		my addItem_move (espeak_languages_FileInMemorySet85.move());

		static unsigned char espeak_languages_FileInMemorySet86_data[85] = {
			110,97,109,101,32,70,114,101,110,99,104,32,40,66,101,108,103,105,117,109,41,10,108,97,110,103,117,97,103,101,
			32,102,114,45,98,101,10,108,97,110,103,117,97,103,101,32,102,114,32,56,10,10,100,105,99,116,114,117,108,101,
			115,32,50,10,116,117,110,101,115,32,115,51,32,99,51,32,113,51,32,101,51,10,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet86 = FileInMemory_createWithData (84, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet86_data), true, 
			U"./data/lang/roa/fr-BE", 
			U"fr-BE");
		my addItem_move (espeak_languages_FileInMemorySet86.move());

		static unsigned char espeak_languages_FileInMemorySet87_data[87] = {
			110,97,109,101,32,70,114,101,110,99,104,32,40,83,119,105,116,122,101,114,108,97,110,100,41,10,108,97,110,103,
			117,97,103,101,32,102,114,45,99,104,10,108,97,110,103,117,97,103,101,32,102,114,32,56,10,10,100,105,99,116,
			114,117,108,101,115,32,51,10,116,117,110,101,115,32,115,51,32,99,51,32,113,51,32,101,51,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet87 = FileInMemory_createWithData (86, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet87_data), true, 
			U"./data/lang/roa/fr-CH", 
			U"fr-CH");
		my addItem_move (espeak_languages_FileInMemorySet87.move());

		static unsigned char espeak_languages_FileInMemorySet88_data[141] = {
			110,97,109,101,32,72,97,105,116,105,97,110,32,67,114,101,111,108,101,10,108,97,110,103,117,97,103,101,32,104,
			116,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,109,97,105,110,116,97,105,110,101,114,32,32,47,
			47,32,84,79,68,79,32,115,111,109,101,98,111,100,121,32,115,104,111,117,108,100,32,116,97,107,101,32,114,101,
			115,112,111,110,115,105,98,105,108,105,116,121,32,102,111,114,32,116,104,105,115,10,10,112,104,111,110,101,109,101,
			115,32,104,116,10,100,105,99,116,105,111,110,97,114,121,32,104,116,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet88 = FileInMemory_createWithData (140, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet88_data), true, 
			U"./data/lang/roa/ht", 
			U"ht");
		my addItem_move (espeak_languages_FileInMemorySet88.move());

		static unsigned char espeak_languages_FileInMemorySet89_data[110] = {
			110,97,109,101,32,73,116,97,108,105,97,110,10,108,97,110,103,117,97,103,101,32,105,116,10,10,109,97,105,110,
			116,97,105,110,101,114,32,67,104,114,105,115,116,105,97,110,32,76,101,111,32,77,32,60,108,108,97,106,116,97,
			50,48,49,50,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,109,97,116,117,114,101,10,
			10,116,117,110,101,115,32,115,52,32,99,52,32,113,52,32,101,52,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet89 = FileInMemory_createWithData (109, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet89_data), true, 
			U"./data/lang/roa/it", 
			U"it");
		my addItem_move (espeak_languages_FileInMemorySet89.move());

		static unsigned char espeak_languages_FileInMemorySet90_data[63] = {
			110,97,109,101,32,80,97,112,105,97,109,101,110,116,111,10,108,97,110,103,117,97,103,101,32,112,97,112,10,10,
			115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,10,112,104,111,110,101,109,101,115,32,98,97,115,101,50,
			10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet90 = FileInMemory_createWithData (62, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet90_data), true, 
			U"./data/lang/roa/pap", 
			U"pap");
		my addItem_move (espeak_languages_FileInMemorySet90.move());

		static unsigned char espeak_languages_FileInMemorySet91_data[96] = {
			110,97,109,101,32,80,111,114,116,117,103,117,101,115,101,32,40,80,111,114,116,117,103,97,108,41,10,108,97,110,
			103,117,97,103,101,32,112,116,10,108,97,110,103,117,97,103,101,32,112,116,45,112,116,10,112,104,111,110,101,109,
			101,115,32,112,116,45,112,116,10,10,100,105,99,116,114,117,108,101,115,32,49,10,105,110,116,111,110,97,116,105,
			111,110,32,50,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet91 = FileInMemory_createWithData (95, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet91_data), true, 
			U"./data/lang/roa/pt", 
			U"pt");
		my addItem_move (espeak_languages_FileInMemorySet91.move());

		static unsigned char espeak_languages_FileInMemorySet92_data[110] = {
			110,97,109,101,32,80,111,114,116,117,103,117,101,115,101,32,40,66,114,97,122,105,108,41,10,108,97,110,103,117,
			97,103,101,32,112,116,45,98,114,10,108,97,110,103,117,97,103,101,32,112,116,32,54,10,10,100,105,99,116,114,
			117,108,101,115,32,50,10,115,116,114,101,115,115,76,101,110,103,116,104,32,50,48,48,32,49,49,53,32,50,51,
			48,32,50,51,48,32,48,32,48,32,50,53,48,32,50,55,48,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet92 = FileInMemory_createWithData (109, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet92_data), true, 
			U"./data/lang/roa/pt-BR", 
			U"pt-BR");
		my addItem_move (espeak_languages_FileInMemorySet92.move());

		static unsigned char espeak_languages_FileInMemorySet93_data[27] = {
			110,97,109,101,32,82,111,109,97,110,105,97,110,10,108,97,110,103,117,97,103,101,32,114,111,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet93 = FileInMemory_createWithData (26, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet93_data), true, 
			U"./data/lang/roa/ro", 
			U"ro");
		my addItem_move (espeak_languages_FileInMemorySet93.move());

		static unsigned char espeak_languages_FileInMemorySet94_data[48] = {
			110,97,109,101,32,71,117,97,114,97,110,105,10,108,97,110,103,117,97,103,101,32,103,110,10,100,105,99,116,114,
			117,108,101,115,32,49,10,119,111,114,100,115,32,48,32,49,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet94 = FileInMemory_createWithData (47, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet94_data), true, 
			U"./data/lang/sai/gn", 
			U"gn");
		my addItem_move (espeak_languages_FileInMemorySet94.move());

		static unsigned char espeak_languages_FileInMemorySet95_data[42] = {
			110,97,109,101,32,65,109,104,97,114,105,99,10,108,97,110,103,117,97,103,101,32,97,109,10,10,115,116,97,116,
			117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet95 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet95_data), true, 
			U"./data/lang/sem/am", 
			U"am");
		my addItem_move (espeak_languages_FileInMemorySet95.move());

		static unsigned char espeak_languages_FileInMemorySet96_data[51] = {
			110,97,109,101,32,65,114,97,98,105,99,10,108,97,110,103,117,97,103,101,32,97,114,10,112,104,111,110,101,109,
			101,115,32,97,114,10,10,115,116,114,101,115,115,82,117,108,101,32,52,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet96 = FileInMemory_createWithData (50, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet96_data), true, 
			U"./data/lang/sem/ar", 
			U"ar");
		my addItem_move (espeak_languages_FileInMemorySet96.move());

		static unsigned char espeak_languages_FileInMemorySet97_data[41] = {
			110,97,109,101,32,72,101,98,114,101,119,10,108,97,110,103,117,97,103,101,32,104,101,10,10,115,116,97,116,117,
			115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet97 = FileInMemory_createWithData (40, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet97_data), true, 
			U"./data/lang/sem/he", 
			U"he");
		my addItem_move (espeak_languages_FileInMemorySet97.move());

		static unsigned char espeak_languages_FileInMemorySet98_data[42] = {
			110,97,109,101,32,77,97,108,116,101,115,101,10,108,97,110,103,117,97,103,101,32,109,116,10,10,115,116,97,116,
			117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet98 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet98_data), true, 
			U"./data/lang/sem/mt", 
			U"mt");
		my addItem_move (espeak_languages_FileInMemorySet98.move());

		static unsigned char espeak_languages_FileInMemorySet99_data[94] = {
			110,97,109,101,32,84,105,103,114,105,110,121,97,10,108,97,110,103,117,97,103,101,32,116,105,10,10,109,97,105,
			110,116,97,105,110,101,114,32,66,105,110,105,97,109,32,71,101,98,114,101,109,105,99,104,97,101,108,32,60,98,
			105,110,105,97,109,103,64,103,109,97,105,108,46,99,111,109,62,10,115,116,97,116,117,115,32,116,101,115,116,105,
			110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet99 = FileInMemory_createWithData (93, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet99_data), true, 
			U"./data/lang/sem/ti", 
			U"ti");
		my addItem_move (espeak_languages_FileInMemorySet99.move());

		static unsigned char espeak_languages_FileInMemorySet100_data[687] = {
			110,97,109,101,32,67,104,105,110,101,115,101,32,40,77,97,110,100,97,114,105,110,44,32,108,97,116,105,110,32,
			97,115,32,69,110,103,108,105,115,104,41,10,108,97,110,103,117,97,103,101,32,99,109,110,10,108,97,110,103,117,
			97,103,101,32,122,104,45,99,109,110,10,108,97,110,103,117,97,103,101,32,122,104,10,10,112,104,111,110,101,109,
			101,115,32,99,109,110,10,100,105,99,116,105,111,110,97,114,121,32,99,109,110,10,119,111,114,100,115,32,49,10,
			112,105,116,99,104,32,56,48,32,49,49,56,10,10,100,105,99,116,95,109,105,110,32,49,48,48,48,48,48,10,
			10,47,47,102,111,114,32,115,111,109,101,32,100,105,97,108,101,99,116,115,10,10,47,47,91,101,110,93,58,32,
			114,101,112,108,97,99,101,32,110,103,32,119,105,116,104,32,110,10,47,47,91,122,104,93,58,32,239,191,189,222,
			186,239,191,189,239,191,189,239,191,189,239,191,189,239,191,189,239,191,189,239,191,189,110,103,239,191,189,239,191,189,
			239,191,189,110,10,47,47,114,101,112,108,97,99,101,32,48,32,78,32,110,10,10,47,47,91,101,110,93,58,32,
			114,101,112,108,97,99,101,32,114,102,120,32,99,111,110,115,111,110,97,110,116,115,10,47,47,91,122,104,93,58,
			32,239,191,189,222,190,239,191,189,239,191,189,239,191,189,239,191,189,239,191,189,239,191,189,239,191,189,114,239,191,
			189,239,191,189,239,191,189,108,239,191,189,239,191,189,122,239,191,189,239,191,189,101,114,239,191,189,239,191,189,239,
			191,189,101,10,47,47,114,101,112,108,97,99,101,32,48,32,116,115,46,104,32,116,115,104,10,47,47,114,101,112,
			108,97,99,101,32,48,32,116,115,46,32,116,115,10,47,47,114,101,112,108,97,99,101,32,48,32,115,46,32,115,
			10,47,47,114,101,112,108,97,99,101,32,48,32,105,46,32,105,91,10,47,47,114,101,112,108,97,99,101,32,48,
			32,122,46,32,108,10,47,47,114,101,112,108,97,99,101,32,48,32,122,46,32,122,10,47,47,114,101,112,108,97,
			99,101,32,48,32,64,114,32,64,10,10,47,47,91,101,110,93,58,32,114,101,112,108,97,99,101,32,98,101,103,
			105,110,110,105,110,103,32,110,32,111,114,32,108,10,47,47,91,122,104,93,58,32,239,191,189,239,191,189,239,191,
			189,239,191,189,110,108,239,191,189,239,191,189,110,239,191,189,239,191,189,239,191,189,108,239,191,189,239,191,189,108,
			239,191,189,239,191,189,239,191,189,110,10,47,47,114,101,112,108,97,99,101,32,50,32,110,32,108,10,47,47,114,
			101,112,108,97,99,101,32,50,32,108,32,110,10,10,47,47,91,101,110,93,58,32,114,101,112,108,97,99,101,32,
			98,101,103,105,110,110,105,110,103,32,119,32,119,105,116,104,32,118,10,47,47,91,122,104,93,58,32,119,239,191,
			189,239,191,189,239,191,189,118,10,47,47,114,101,112,108,97,99,101,32,48,32,119,32,32,118,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet100 = FileInMemory_createWithData (686, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet100_data), true, 
			U"./data/lang/sit/cmn", 
			U"cmn");
		my addItem_move (espeak_languages_FileInMemorySet100.move());

		static unsigned char espeak_languages_FileInMemorySet101_data[162] = {
			110,97,109,101,32,67,104,105,110,101,115,101,32,40,77,97,110,100,97,114,105,110,44,32,108,97,116,105,110,32,
			97,115,32,80,105,110,121,105,110,41,10,108,97,110,103,117,97,103,101,32,99,109,110,45,108,97,116,110,45,112,
			105,110,121,105,110,10,108,97,110,103,117,97,103,101,32,122,104,45,99,109,110,10,108,97,110,103,117,97,103,101,
			32,122,104,10,10,112,104,111,110,101,109,101,115,32,99,109,110,10,100,105,99,116,105,111,110,97,114,121,32,99,
			109,110,10,119,111,114,100,115,32,49,10,112,105,116,99,104,32,56,48,32,49,49,56,10,10,100,105,99,116,95,
			109,105,110,32,49,48,48,48,48,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet101 = FileInMemory_createWithData (161, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet101_data), true, 
			U"./data/lang/sit/cmn-Latn-pinyin", 
			U"cmn-Latn-pinyin");
		my addItem_move (espeak_languages_FileInMemorySet101.move());

		static unsigned char espeak_languages_FileInMemorySet102_data[129] = {
			110,97,109,101,32,72,97,107,107,97,32,67,104,105,110,101,115,101,10,108,97,110,103,117,97,103,101,32,104,97,
			107,10,109,97,105,110,116,97,105,110,101,114,32,67,104,101,110,32,67,104,105,101,110,45,116,105,110,103,32,60,
			121,111,120,101,109,46,116,101,109,57,56,64,110,99,116,117,46,101,100,117,46,116,119,62,10,115,116,97,116,117,
			115,32,116,101,115,116,105,110,103,10,112,104,111,110,101,109,101,115,32,104,97,107,10,100,105,99,116,105,111,110,
			97,114,121,32,104,97,107,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet102 = FileInMemory_createWithData (128, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet102_data), true, 
			U"./data/lang/sit/hak", 
			U"hak");
		my addItem_move (espeak_languages_FileInMemorySet102.move());

		static unsigned char espeak_languages_FileInMemorySet103_data[57] = {
			110,97,109,101,32,77,121,97,110,109,97,114,32,40,66,117,114,109,101,115,101,41,10,109,97,105,110,116,97,105,
			110,101,114,32,77,105,110,32,77,97,117,110,103,10,108,97,110,103,117,97,103,101,32,109,121,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet103 = FileInMemory_createWithData (56, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet103_data), true, 
			U"./data/lang/sit/my", 
			U"my");
		my addItem_move (espeak_languages_FileInMemorySet103.move());

		static unsigned char espeak_languages_FileInMemorySet104_data[195] = {
			110,97,109,101,32,67,104,105,110,101,115,101,32,40,67,97,110,116,111,110,101,115,101,41,10,108,97,110,103,117,
			97,103,101,32,121,117,101,10,108,97,110,103,117,97,103,101,32,122,104,45,121,117,101,10,108,97,110,103,117,97,
			103,101,32,122,104,32,56,10,10,112,104,111,110,101,109,101,115,32,121,117,101,10,100,105,99,116,105,111,110,97,
			114,121,32,121,117,101,10,10,47,47,32,105,110,116,101,114,112,114,101,116,32,69,110,103,108,105,115,104,32,108,
			101,116,116,101,114,115,32,97,115,32,49,61,69,110,103,108,105,115,104,32,119,111,114,100,115,44,32,50,61,106,
			121,117,116,112,105,110,103,10,100,105,99,116,114,117,108,101,115,32,49,10,10,119,111,114,100,115,32,49,10,100,
			105,99,116,95,109,105,110,32,49,48,48,48,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet104 = FileInMemory_createWithData (194, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet104_data), true, 
			U"./data/lang/sit/yue", 
			U"yue");
		my addItem_move (espeak_languages_FileInMemorySet104.move());

		static unsigned char espeak_languages_FileInMemorySet105_data[214] = {
			110,97,109,101,32,67,104,105,110,101,115,101,32,40,67,97,110,116,111,110,101,115,101,44,32,108,97,116,105,110,
			32,97,115,32,74,121,117,116,112,105,110,103,41,10,108,97,110,103,117,97,103,101,32,121,117,101,10,108,97,110,
			103,117,97,103,101,32,122,104,45,121,117,101,10,108,97,110,103,117,97,103,101,32,122,104,32,56,10,10,112,104,
			111,110,101,109,101,115,32,121,117,101,10,100,105,99,116,105,111,110,97,114,121,32,121,117,101,10,10,47,47,32,
			105,110,116,101,114,112,114,101,116,32,69,110,103,108,105,115,104,32,108,101,116,116,101,114,115,32,97,115,32,49,
			61,69,110,103,108,105,115,104,32,119,111,114,100,115,44,32,50,61,106,121,117,116,112,105,110,103,10,100,105,99,
			116,114,117,108,101,115,32,50,10,10,119,111,114,100,115,32,49,10,100,105,99,116,95,109,105,110,32,49,48,48,
			48,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet105 = FileInMemory_createWithData (213, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet105_data), true, 
			U"./data/lang/sit/yue-Latn-jyutping", 
			U"yue-Latn-jyutping");
		my addItem_move (espeak_languages_FileInMemorySet105.move());

		static unsigned char espeak_languages_FileInMemorySet106_data[93] = {
			110,97,109,101,32,83,104,97,110,32,40,84,97,105,32,89,97,105,41,10,108,97,110,103,117,97,103,101,32,115,
			104,110,10,109,97,105,110,116,97,105,110,101,114,32,114,111,110,97,108,100,97,117,103,32,60,99,111,110,116,97,
			99,116,64,114,111,110,97,108,100,97,117,103,46,109,108,62,10,115,116,97,116,117,115,32,116,101,115,116,105,110,
			103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet106 = FileInMemory_createWithData (92, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet106_data), true, 
			U"./data/lang/tai/shn", 
			U"shn");
		my addItem_move (espeak_languages_FileInMemorySet106.move());

		static unsigned char espeak_languages_FileInMemorySet107_data[38] = {
			110,97,109,101,32,84,104,97,105,10,108,97,110,103,117,97,103,101,32,116,104,10,115,116,97,116,117,115,32,116,
			101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet107 = FileInMemory_createWithData (37, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet107_data), true, 
			U"./data/lang/tai/th", 
			U"th");
		my addItem_move (espeak_languages_FileInMemorySet107.move());

		static unsigned char espeak_languages_FileInMemorySet108_data[46] = {
			110,97,109,101,32,65,122,101,114,98,97,105,106,97,110,105,10,108,97,110,103,117,97,103,101,32,97,122,10,10,
			115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet108 = FileInMemory_createWithData (45, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet108_data), true, 
			U"./data/lang/trk/az", 
			U"az");
		my addItem_move (espeak_languages_FileInMemorySet108.move());

		static unsigned char espeak_languages_FileInMemorySet109_data[26] = {
			110,97,109,101,32,66,97,115,104,107,105,114,10,108,97,110,103,117,97,103,101,32,98,97,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet109 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet109_data), true, 
			U"./data/lang/trk/ba", 
			U"ba");
		my addItem_move (espeak_languages_FileInMemorySet109.move());

		static unsigned char espeak_languages_FileInMemorySet110_data[41] = {
			110,97,109,101,32,67,104,117,118,97,115,104,10,108,97,110,103,117,97,103,101,32,99,118,10,115,116,97,116,117,
			115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet110 = FileInMemory_createWithData (40, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet110_data), true, 
			U"./data/lang/trk/cv", 
			U"cv");
		my addItem_move (espeak_languages_FileInMemorySet110.move());

		static unsigned char espeak_languages_FileInMemorySet111_data[29] = {
			110,97,109,101,32,75,97,114,97,107,97,108,112,97,107,10,108,97,110,103,117,97,103,101,32,107,97,97,0};
		autoFileInMemory espeak_languages_FileInMemorySet111 = FileInMemory_createWithData (28, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet111_data), true, 
			U"./data/lang/trk/kaa", 
			U"kaa");
		my addItem_move (espeak_languages_FileInMemorySet111.move());

		static unsigned char espeak_languages_FileInMemorySet112_data[41] = {
			110,97,109,101,32,75,97,122,97,107,104,10,108,97,110,103,117,97,103,101,32,107,107,10,115,116,97,116,117,115,
			32,116,101,115,116,105,110,103,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet112 = FileInMemory_createWithData (40, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet112_data), true, 
			U"./data/lang/trk/kk", 
			U"kk");
		my addItem_move (espeak_languages_FileInMemorySet112.move());

		static unsigned char espeak_languages_FileInMemorySet113_data[44] = {
			110,97,109,101,32,75,121,114,103,121,122,10,108,97,110,103,117,97,103,101,32,107,121,10,10,116,117,110,101,115,
			32,115,51,32,99,51,32,113,51,32,101,51,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet113 = FileInMemory_createWithData (43, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet113_data), true, 
			U"./data/lang/trk/ky", 
			U"ky");
		my addItem_move (espeak_languages_FileInMemorySet113.move());

		static unsigned char espeak_languages_FileInMemorySet114_data[40] = {
			110,97,109,101,32,78,111,103,97,105,10,108,97,110,103,117,97,103,101,32,110,111,103,10,115,116,97,116,117,115,
			32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet114 = FileInMemory_createWithData (39, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet114_data), true, 
			U"./data/lang/trk/nog", 
			U"nog");
		my addItem_move (espeak_languages_FileInMemorySet114.move());

		static unsigned char espeak_languages_FileInMemorySet115_data[26] = {
			110,97,109,101,32,84,117,114,107,109,101,110,10,108,97,110,103,117,97,103,101,32,116,107,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet115 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet115_data), true, 
			U"./data/lang/trk/tk", 
			U"tk");
		my addItem_move (espeak_languages_FileInMemorySet115.move());

		static unsigned char espeak_languages_FileInMemorySet116_data[26] = {
			110,97,109,101,32,84,117,114,107,105,115,104,10,108,97,110,103,117,97,103,101,32,116,114,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet116 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet116_data), true, 
			U"./data/lang/trk/tr", 
			U"tr");
		my addItem_move (espeak_languages_FileInMemorySet116.move());

		static unsigned char espeak_languages_FileInMemorySet117_data[24] = {
			110,97,109,101,32,84,97,116,97,114,10,108,97,110,103,117,97,103,101,32,116,116,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet117 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet117_data), true, 
			U"./data/lang/trk/tt", 
			U"tt");
		my addItem_move (espeak_languages_FileInMemorySet117.move());

		static unsigned char espeak_languages_FileInMemorySet118_data[25] = {
			110,97,109,101,32,85,121,103,104,117,114,10,108,97,110,103,117,97,103,101,32,117,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet118 = FileInMemory_createWithData (24, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet118_data), true, 
			U"./data/lang/trk/ug", 
			U"ug");
		my addItem_move (espeak_languages_FileInMemorySet118.move());

		static unsigned char espeak_languages_FileInMemorySet119_data[40] = {
			110,97,109,101,32,85,122,98,101,107,10,108,97,110,103,117,97,103,101,32,117,122,10,10,115,116,97,116,117,115,
			32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet119 = FileInMemory_createWithData (39, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet119_data), true, 
			U"./data/lang/trk/uz", 
			U"uz");
		my addItem_move (espeak_languages_FileInMemorySet119.move());

		static unsigned char espeak_languages_FileInMemorySet120_data[238] = {
			110,97,109,101,32,69,115,116,111,110,105,97,110,10,108,97,110,103,117,97,103,101,32,101,116,10,10,115,116,114,
			101,115,115,65,109,112,32,49,56,32,49,54,32,50,50,32,50,50,32,50,48,32,50,50,32,50,50,32,50,50,
			10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,53,48,32,49,56,48,32,50,48,48,32,50,48,48,32,
			48,32,48,32,50,49,48,32,50,53,48,10,115,116,114,101,115,115,79,112,116,32,49,32,50,32,52,32,54,32,
			47,47,32,40,83,95,78,79,95,68,73,77,32,43,32,83,95,70,73,78,65,76,95,68,73,77,32,61,32,83,
			95,70,73,78,65,76,95,68,73,77,95,79,78,76,89,41,44,32,83,95,70,73,78,65,76,95,78,79,95,50,
			44,32,83,95,50,95,84,79,95,72,69,65,86,89,10,115,116,114,101,115,115,82,117,108,101,32,48,10,10,105,
			110,116,111,110,97,116,105,111,110,32,51,10,115,112,101,108,108,105,110,103,83,116,114,101,115,115,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet120 = FileInMemory_createWithData (237, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet120_data), true, 
			U"./data/lang/urj/et", 
			U"et");
		my addItem_move (espeak_languages_FileInMemorySet120.move());

		static unsigned char espeak_languages_FileInMemorySet121_data[238] = {
			110,97,109,101,32,70,105,110,110,105,115,104,10,108,97,110,103,117,97,103,101,32,102,105,10,10,10,115,116,114,
			101,115,115,65,109,112,32,49,56,32,49,54,32,50,50,32,50,50,32,50,48,32,50,50,32,50,50,32,50,50,
			10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,53,48,32,49,56,48,32,50,48,48,32,50,48,48,32,
			48,32,48,32,50,49,48,32,50,53,48,10,115,116,114,101,115,115,79,112,116,32,49,32,50,32,52,32,54,32,
			47,47,32,40,83,95,78,79,95,68,73,77,32,43,32,83,95,70,73,78,65,76,95,68,73,77,32,61,32,83,
			95,70,73,78,65,76,95,68,73,77,95,79,78,76,89,41,44,32,83,95,70,73,78,65,76,95,78,79,95,50,
			44,32,83,95,50,95,84,79,95,72,69,65,86,89,10,115,116,114,101,115,115,82,117,108,101,32,48,10,10,105,
			110,116,111,110,97,116,105,111,110,32,51,10,115,112,101,108,108,105,110,103,83,116,114,101,115,115,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet121 = FileInMemory_createWithData (237, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet121_data), true, 
			U"./data/lang/urj/fi", 
			U"fi");
		my addItem_move (espeak_languages_FileInMemorySet121.move());

		static unsigned char espeak_languages_FileInMemorySet122_data[74] = {
			110,97,109,101,32,72,117,110,103,97,114,105,97,110,10,108,97,110,103,117,97,103,101,32,104,117,10,98,114,97,
			99,107,101,116,115,32,48,10,98,114,97,99,107,101,116,115,65,110,110,111,117,110,99,101,100,32,48,10,112,105,
			116,99,104,32,56,49,32,49,49,55,10,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet122 = FileInMemory_createWithData (73, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet122_data), true, 
			U"./data/lang/urj/hu", 
			U"hu");
		my addItem_move (espeak_languages_FileInMemorySet122.move());

		static unsigned char espeak_languages_FileInMemorySet123_data[46] = {
			110,97,109,101,32,76,117,108,101,32,83,97,97,109,105,10,108,97,110,103,117,97,103,101,32,115,109,106,10,10,
			115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet123 = FileInMemory_createWithData (45, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet123_data), true, 
			U"./data/lang/urj/smj", 
			U"smj");
		my addItem_move (espeak_languages_FileInMemorySet123.move());

		static unsigned char espeak_languages_FileInMemorySet124_data[53] = {
			110,97,109,101,32,66,101,108,97,114,117,115,105,97,110,10,108,97,110,103,117,97,103,101,32,98,101,10,100,105,
			99,116,95,109,105,110,32,32,50,48,48,48,10,115,112,101,101,100,32,57,53,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet124 = FileInMemory_createWithData (52, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet124_data), true, 
			U"./data/lang/zle/be", 
			U"be");
		my addItem_move (espeak_languages_FileInMemorySet124.move());

		static unsigned char espeak_languages_FileInMemorySet125_data[58] = {
			110,97,109,101,32,82,117,115,115,105,97,110,10,108,97,110,103,117,97,103,101,32,114,117,10,114,101,112,108,97,
			99,101,32,48,51,32,97,32,97,35,10,100,105,99,116,95,109,105,110,32,32,50,48,48,48,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet125 = FileInMemory_createWithData (57, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet125_data), true, 
			U"./data/lang/zle/ru", 
			U"ru");
		my addItem_move (espeak_languages_FileInMemorySet125.move());

		static unsigned char espeak_languages_FileInMemorySet126_data[281] = {
			110,97,109,101,32,82,117,115,115,105,97,110,32,40,76,97,116,118,105,97,41,10,108,97,110,103,117,97,103,101,
			32,114,117,45,108,118,32,50,10,10,109,97,105,110,116,97,105,110,101,114,32,86,97,108,100,105,115,32,86,105,
			116,111,108,105,110,115,32,60,118,97,108,100,105,115,46,118,105,116,111,108,105,110,115,64,111,100,111,46,108,118,
			62,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,10,112,104,111,110,101,109,101,115,32,114,117,45,
			108,118,10,100,105,99,116,114,117,108,101,115,32,50,10,100,105,99,116,95,109,105,110,32,32,50,48,48,48,48,
			10,115,112,101,101,100,32,57,53,10,10,119,111,114,100,115,32,48,32,50,10,116,111,110,101,32,49,53,48,32,
			50,50,48,32,52,53,48,32,50,53,53,32,55,53,48,32,50,48,32,51,53,48,48,32,50,53,53,10,115,116,
			114,101,115,115,65,109,112,32,49,50,32,49,48,32,56,32,56,32,48,32,48,32,49,54,32,49,55,10,115,116,
			114,101,115,115,76,101,110,103,116,104,32,49,54,48,32,49,52,48,32,50,48,48,32,49,52,48,32,48,32,48,
			32,50,52,48,32,49,54,48,10,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet126 = FileInMemory_createWithData (280, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet126_data), true, 
			U"./data/lang/zle/ru-LV", 
			U"ru-LV");
		my addItem_move (espeak_languages_FileInMemorySet126.move());

		static unsigned char espeak_languages_FileInMemorySet127_data[92] = {
			110,97,109,101,32,82,117,115,115,105,97,110,32,40,67,108,97,115,115,105,99,41,10,108,97,110,103,117,97,103,
			101,32,114,117,45,99,108,10,114,101,112,108,97,99,101,32,48,51,32,97,32,97,35,10,100,105,99,116,95,109,
			105,110,32,32,50,48,48,48,48,10,115,112,101,101,100,32,57,53,10,100,105,99,116,114,117,108,101,115,32,51,
			10,0};
		autoFileInMemory espeak_languages_FileInMemorySet127 = FileInMemory_createWithData (91, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet127_data), true, 
			U"./data/lang/zle/ru-cl", 
			U"ru-cl");
		my addItem_move (espeak_languages_FileInMemorySet127.move());

		static unsigned char espeak_languages_FileInMemorySet128_data[98] = {
			110,97,109,101,32,85,107,114,97,105,110,105,97,110,10,108,97,110,103,117,97,103,101,32,117,107,10,10,109,97,
			105,110,116,97,105,110,101,114,32,65,110,100,114,105,106,32,77,105,122,121,107,32,60,97,110,100,109,49,122,121,
			107,64,112,114,111,116,111,110,46,109,101,62,10,115,116,97,116,117,115,32,116,101,115,116,105,110,103,10,10,115,
			112,101,101,100,32,56,48,0};
		autoFileInMemory espeak_languages_FileInMemorySet128 = FileInMemory_createWithData (97, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet128_data), true, 
			U"./data/lang/zle/uk", 
			U"uk");
		my addItem_move (espeak_languages_FileInMemorySet128.move());

		static unsigned char espeak_languages_FileInMemorySet129_data[112] = {
			110,97,109,101,32,66,117,108,103,97,114,105,97,110,10,108,97,110,103,117,97,103,101,32,98,103,10,10,115,116,
			114,101,115,115,65,109,112,32,49,51,32,49,50,32,49,55,32,49,55,32,50,48,32,50,50,32,50,50,32,50,
			49,32,10,115,116,114,101,115,115,76,101,110,103,116,104,32,49,56,48,32,49,55,48,32,32,50,48,48,32,50,
			48,48,32,32,50,48,48,32,50,48,48,32,32,50,49,48,32,50,50,48,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet129 = FileInMemory_createWithData (111, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet129_data), true, 
			U"./data/lang/zls/bg", 
			U"bg");
		my addItem_move (espeak_languages_FileInMemorySet129.move());

		static unsigned char espeak_languages_FileInMemorySet130_data[231] = {
			110,97,109,101,32,66,111,115,110,105,97,110,10,108,97,110,103,117,97,103,101,32,98,115,10,112,104,111,110,101,
			109,101,115,32,104,114,10,10,112,105,116,99,104,32,56,49,32,49,50,48,10,102,111,114,109,97,110,116,32,48,
			32,49,48,48,32,49,48,48,32,49,48,48,10,102,111,114,109,97,110,116,32,49,32,32,57,55,32,32,57,55,
			32,49,48,48,10,102,111,114,109,97,110,116,32,50,32,32,57,55,32,32,57,55,32,49,48,48,10,102,111,114,
			109,97,110,116,32,51,32,32,57,55,32,49,48,50,32,49,48,48,10,102,111,114,109,97,110,116,32,52,32,32,
			57,55,32,49,48,50,32,49,48,48,10,102,111,114,109,97,110,116,32,53,32,32,57,55,32,49,48,50,32,49,
			48,48,10,10,115,116,114,101,115,115,65,100,100,32,49,48,32,49,48,32,48,32,48,32,48,32,48,32,45,51,
			48,32,45,51,48,10,100,105,99,116,114,117,108,101,115,32,51,32,52,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet130 = FileInMemory_createWithData (230, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet130_data), true, 
			U"./data/lang/zls/bs", 
			U"bs");
		my addItem_move (espeak_languages_FileInMemorySet130.move());

		static unsigned char espeak_languages_FileInMemorySet131_data[263] = {
			110,97,109,101,32,67,114,111,97,116,105,97,110,10,108,97,110,103,117,97,103,101,32,104,114,10,108,97,110,103,
			117,97,103,101,32,104,98,115,10,10,47,47,32,97,116,116,114,105,98,117,116,101,115,32,116,111,119,97,114,100,
			115,32,33,118,97,114,105,97,110,116,51,10,112,105,116,99,104,32,56,49,32,49,50,48,10,102,111,114,109,97,
			110,116,32,48,32,49,48,48,32,49,48,48,32,49,48,48,10,102,111,114,109,97,110,116,32,49,32,32,57,55,
			32,32,57,55,32,49,48,48,10,102,111,114,109,97,110,116,32,50,32,32,57,55,32,32,57,55,32,49,48,48,
			10,102,111,114,109,97,110,116,32,51,32,32,57,55,32,49,48,50,32,49,48,48,10,102,111,114,109,97,110,116,
			32,52,32,32,57,55,32,49,48,50,32,49,48,48,10,102,111,114,109,97,110,116,32,53,32,32,57,55,32,49,
			48,50,32,49,48,48,10,10,115,116,114,101,115,115,65,100,100,32,49,48,32,49,48,32,48,32,48,32,48,32,
			48,32,45,51,48,32,45,51,48,10,100,105,99,116,114,117,108,101,115,32,49,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet131 = FileInMemory_createWithData (262, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet131_data), true, 
			U"./data/lang/zls/hr", 
			U"hr");
		my addItem_move (espeak_languages_FileInMemorySet131.move());

		static unsigned char espeak_languages_FileInMemorySet132_data[29] = {
			110,97,109,101,32,77,97,99,101,100,111,110,105,97,110,10,108,97,110,103,117,97,103,101,32,109,107,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet132 = FileInMemory_createWithData (28, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet132_data), true, 
			U"./data/lang/zls/mk", 
			U"mk");
		my addItem_move (espeak_languages_FileInMemorySet132.move());

		static unsigned char espeak_languages_FileInMemorySet133_data[44] = {
			110,97,109,101,32,83,108,111,118,101,110,105,97,110,10,108,97,110,103,117,97,103,101,32,115,108,10,10,115,116,
			97,116,117,115,32,116,101,115,116,105,110,103,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet133 = FileInMemory_createWithData (43, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet133_data), true, 
			U"./data/lang/zls/sl", 
			U"sl");
		my addItem_move (espeak_languages_FileInMemorySet133.move());

		static unsigned char espeak_languages_FileInMemorySet134_data[251] = {
			110,97,109,101,32,83,101,114,98,105,97,110,10,108,97,110,103,117,97,103,101,32,115,114,10,10,47,47,32,97,
			116,116,114,105,98,117,116,101,115,32,116,111,119,97,114,100,115,32,33,118,97,114,105,97,110,116,51,32,112,105,
			116,99,104,32,56,48,32,49,50,48,10,102,111,114,109,97,110,116,32,48,32,49,48,48,32,49,48,48,32,49,
			48,48,10,102,111,114,109,97,110,116,32,49,32,32,57,55,32,32,57,55,32,49,48,48,10,102,111,114,109,97,
			110,116,32,50,32,32,57,55,32,32,57,55,32,49,48,48,10,102,111,114,109,97,110,116,32,51,32,32,57,55,
			32,49,48,50,32,49,48,48,10,102,111,114,109,97,110,116,32,52,32,32,57,55,32,49,48,50,32,49,48,48,
			10,102,111,114,109,97,110,116,32,53,32,32,57,55,32,49,48,50,32,49,48,48,10,10,115,116,114,101,115,115,
			65,100,100,32,49,48,32,49,48,32,48,32,48,32,48,32,48,32,45,51,48,32,45,51,48,10,100,105,99,116,
			114,117,108,101,115,32,50,32,52,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet134 = FileInMemory_createWithData (250, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet134_data), true, 
			U"./data/lang/zls/sr", 
			U"sr");
		my addItem_move (espeak_languages_FileInMemorySet134.move());

		static unsigned char espeak_languages_FileInMemorySet135_data[24] = {
			110,97,109,101,32,67,122,101,99,104,10,108,97,110,103,117,97,103,101,32,99,115,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet135 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet135_data), true, 
			U"./data/lang/zlw/cs", 
			U"cs");
		my addItem_move (espeak_languages_FileInMemorySet135.move());

		static unsigned char espeak_languages_FileInMemorySet136_data[39] = {
			110,97,109,101,32,80,111,108,105,115,104,10,108,97,110,103,117,97,103,101,32,112,108,10,10,105,110,116,111,110,
			97,116,105,111,110,32,50,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet136 = FileInMemory_createWithData (38, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet136_data), true, 
			U"./data/lang/zlw/pl", 
			U"pl");
		my addItem_move (espeak_languages_FileInMemorySet136.move());

		static unsigned char espeak_languages_FileInMemorySet137_data[25] = {
			110,97,109,101,32,83,108,111,118,97,107,10,108,97,110,103,117,97,103,101,32,115,107,10,0};
		autoFileInMemory espeak_languages_FileInMemorySet137 = FileInMemory_createWithData (24, reinterpret_cast<const char *> (&espeak_languages_FileInMemorySet137_data), true, 
			U"./data/lang/zlw/sk", 
			U"sk");
		my addItem_move (espeak_languages_FileInMemorySet137.move());

		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySet not created.");
	}
}

/* End of file espeak_languages_FileInMemorySet.cpp */
