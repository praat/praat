/* File espeakdata_voices.cpp (version 1.48.04) was automatically generated on Thu Dec  3 15:21:15 2015 from files in espeak-data/voices  */

#include "espeakdata_FileInMemory.h"
#include "Collection.h"
#include "FileInMemory.h"
#include "melder.h"

autoFileInMemorySet create_espeakdata_voices () {
	try {
		autoFileInMemorySet me = FileInMemorySet_create ();
		static unsigned char espeakdata_voices1_data[28] = {
			110, 97, 109, 101, 32, 112, 101, 114, 115, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 97, 10, 32, 10, 0};
		autoFileInMemory espeakdata_voices1 = FileInMemory_createWithData (27, reinterpret_cast<const char *> (&espeakdata_voices1_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/fa", 
			U"fa");
		my addItem_move (espeakdata_voices1.move());

		static unsigned char espeakdata_voices2_data[202] = {
			110, 97, 109, 101, 32, 112, 101, 114, 115, 105, 97, 110, 45, 112, 105, 110, 103, 108, 105, 115, 104, 10, 47, 47, 32, 83, 111, 109, 101, 116,
			105, 109, 101, 115, 44, 32, 70, 97, 114, 115, 105, 32, 115, 112, 101, 97, 107, 101, 114, 115, 32, 119, 114, 105, 116, 101, 32, 70, 97, 114,
			115, 105, 32, 119, 111, 114, 100, 115, 32, 117, 115, 105, 110, 103, 32, 69, 110, 103, 108, 105, 115, 104, 32, 99, 104, 97, 114, 97, 99, 116,
			101, 114, 115, 44, 32, 112, 97, 114, 116, 105, 99, 117, 108, 97, 114, 108, 121, 32, 105, 110, 32, 67, 104, 97, 116, 32, 97, 110, 100, 32,
			83, 77, 83, 32, 40, 116, 101, 120, 116, 101, 32, 109, 101, 115, 115, 97, 103, 101, 115, 41, 46, 41, 44, 32, 99, 97, 108, 108, 101, 100,
			32, 80, 105, 110, 103, 108, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 97, 45, 112, 105, 110, 10, 100, 105, 99, 116,
			114, 117, 108, 101, 115, 32, 49, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 102, 97, 10, 10, 0};
		autoFileInMemory espeakdata_voices2 = FileInMemory_createWithData (201, reinterpret_cast<const char *> (&espeakdata_voices2_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/fa-pin", 
			U"fa-pin");
		my addItem_move (espeakdata_voices2.move());

		static unsigned char espeakdata_voices3_data[36] = {
			110, 97, 109, 101, 32, 104, 105, 110, 100, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 105, 10, 103, 101, 110, 100, 101, 114, 32,
			109, 97, 108, 101, 10, 0};
		autoFileInMemory espeakdata_voices3 = FileInMemory_createWithData (35, reinterpret_cast<const char *> (&espeakdata_voices3_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/hi", 
			U"hi");
		my addItem_move (espeakdata_voices3.move());

		static unsigned char espeakdata_voices4_data[39] = {
			110, 97, 109, 101, 32, 97, 114, 109, 101, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 121, 10, 103, 101, 110, 100,
			101, 114, 32, 109, 97, 108, 101, 10, 0};
		autoFileInMemory espeakdata_voices4 = FileInMemory_createWithData (38, reinterpret_cast<const char *> (&espeakdata_voices4_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/hy", 
			U"hy");
		my addItem_move (espeakdata_voices4.move());

		static unsigned char espeakdata_voices5_data[326] = {
			110, 97, 109, 101, 32, 97, 114, 109, 101, 110, 105, 97, 110, 45, 119, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 121,
			45, 119, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 121, 32, 32, 56, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97,
			108, 101, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 10, 47, 47, 32, 99, 104, 97, 110, 103, 101, 32, 99, 111, 110,
			115, 111, 110, 97, 110, 116, 115, 32, 102, 111, 114, 32, 87, 101, 115, 116, 32, 65, 114, 109, 101, 110, 105, 97, 110, 32, 112, 114, 111, 110,
			117, 110, 99, 105, 97, 116, 105, 111, 110, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 98, 32, 32, 112, 35, 10, 114, 101, 112,
			108, 97, 99, 101, 32, 48, 48, 32, 100, 32, 32, 116, 35, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 100, 122, 32, 116, 115,
			35, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 100, 90, 32, 116, 83, 35, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48,
			32, 103, 32, 32, 107, 35, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 112, 32, 32, 98, 10, 114, 101, 112, 108, 97, 99,
			101, 32, 48, 48, 32, 116, 32, 32, 100, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 116, 115, 32, 100, 122, 10, 114, 101, 112,
			108, 97, 99, 101, 32, 48, 48, 32, 116, 83, 32, 100, 90, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 107, 32, 32, 103, 10,
			10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 82, 50, 32, 82, 32, 32, 47, 47, 32, 63, 63, 10, 10, 0};
		autoFileInMemory espeakdata_voices5 = FileInMemory_createWithData (325, reinterpret_cast<const char *> (&espeakdata_voices5_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/hy-west", 
			U"hy-west");
		my addItem_move (espeakdata_voices5.move());

		static unsigned char espeakdata_voices6_data[147] = {
			110, 97, 109, 101, 32, 105, 110, 100, 111, 110, 101, 115, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 105, 100, 10, 103, 101,
			110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 54, 48, 32, 50, 48,
			48, 32, 32, 49, 56, 48, 32, 49, 56, 48, 32, 32, 48, 32, 48, 32, 32, 50, 50, 48, 32, 50, 52, 48, 10, 115, 116, 114, 101, 115,
			115, 65, 109, 112, 32, 32, 32, 32, 49, 54, 32, 32, 49, 56, 32, 32, 32, 49, 56, 32, 32, 49, 56, 32, 32, 32, 48, 32, 48, 32,
			32, 50, 50, 32, 32, 50, 49, 10, 10, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115, 32, 56, 48, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_voices6 = FileInMemory_createWithData (146, reinterpret_cast<const char *> (&espeakdata_voices6_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/id", 
			U"id");
		my addItem_move (espeakdata_voices6.move());

		static unsigned char espeakdata_voices7_data[27] = {
			110, 97, 109, 101, 32, 103, 101, 111, 114, 103, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 97, 10, 0};
		autoFileInMemory espeakdata_voices7 = FileInMemory_createWithData (26, reinterpret_cast<const char *> (&espeakdata_voices7_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/ka", 
			U"ka");
		my addItem_move (espeakdata_voices7.move());

		static unsigned char espeakdata_voices8_data[56] = {
			110, 97, 109, 101, 32, 107, 97, 110, 110, 97, 100, 97, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 110, 10, 10, 105, 110, 116, 111,
			110, 97, 116, 105, 111, 110, 32, 50, 10, 47, 47, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_voices8 = FileInMemory_createWithData (55, reinterpret_cast<const char *> (&espeakdata_voices8_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/kn", 
			U"kn");
		my addItem_move (espeakdata_voices8.move());

		static unsigned char espeakdata_voices9_data[53] = {
			110, 97, 109, 101, 32, 107, 117, 114, 100, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 117, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 47, 47, 119, 111, 114, 100, 115, 32, 49, 32, 52, 56, 10, 10, 0};
		autoFileInMemory espeakdata_voices9 = FileInMemory_createWithData (52, reinterpret_cast<const char *> (&espeakdata_voices9_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/ku", 
			U"ku");
		my addItem_move (espeakdata_voices9.move());

		static unsigned char espeakdata_voices10_data[70] = {
			110, 97, 109, 101, 32, 109, 97, 108, 97, 121, 97, 108, 97, 109, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 109, 108, 10, 103, 101, 110,
			100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 47, 47, 99, 111, 110, 115, 111,
			110, 97, 110, 116, 115, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_voices10 = FileInMemory_createWithData (69, reinterpret_cast<const char *> (&espeakdata_voices10_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/ml", 
			U"ml");
		my addItem_move (espeakdata_voices10.move());

		static unsigned char espeakdata_voices11_data[458] = {
			47, 47, 32, 76, 97, 115, 116, 32, 117, 112, 100, 97, 116, 101, 100, 58, 32, 49, 52, 32, 79, 99, 116, 111, 98, 101, 114, 32, 50, 48,
			49, 48, 44, 32, 74, 97, 115, 111, 110, 32, 79, 110, 103, 32, 40, 106, 97, 115, 111, 110, 64, 112, 111, 114, 116, 97, 108, 103, 114, 111,
			111, 118, 101, 46, 99, 111, 109, 41, 10, 110, 97, 109, 101, 32, 109, 97, 108, 97, 121, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 109,
			115, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 105, 100, 10, 10, 116, 114, 97,
			110, 115, 108, 97, 116, 111, 114, 32, 105, 100, 10, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 54, 48, 32, 50,
			48, 48, 32, 32, 49, 56, 48, 32, 49, 56, 48, 32, 32, 48, 32, 48, 32, 32, 50, 50, 48, 32, 50, 52, 48, 10, 115, 116, 114, 101,
			115, 115, 65, 109, 112, 32, 32, 32, 32, 49, 54, 32, 32, 49, 56, 32, 32, 32, 49, 56, 32, 32, 49, 56, 32, 32, 32, 48, 32, 48,
			32, 32, 50, 50, 32, 32, 50, 49, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 9, 51, 9, 47, 47, 32, 76, 101, 115, 115, 32,
			105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 44, 32, 97, 110, 100, 32, 99, 111, 109, 109, 97, 32, 100, 111, 101, 115, 32, 110, 111, 116,
			32, 114, 97, 105, 115, 101, 32, 116, 104, 101, 32, 112, 105, 116, 99, 104, 46, 10, 10, 47, 47, 32, 78, 117, 97, 110, 99, 101, 32, 45,
			32, 80, 101, 110, 105, 110, 115, 117, 108, 97, 32, 77, 97, 108, 97, 121, 115, 105, 97, 10, 47, 47, 32, 114, 101, 112, 108, 97, 99, 101,
			9, 51, 32, 97, 9, 64, 9, 47, 47, 32, 99, 104, 97, 110, 103, 101, 32, 39, 115, 97, 121, 97, 39, 32, 116, 111, 32, 39, 115, 97,
			121, 101, 39, 10, 9, 9, 9, 9, 47, 47, 32, 40, 111, 110, 108, 121, 32, 116, 104, 101, 32, 108, 97, 115, 116, 32, 112, 104, 111, 110,
			101, 109, 101, 32, 111, 102, 32, 97, 32, 119, 111, 114, 100, 44, 32, 111, 110, 108, 121, 32, 105, 110, 32, 117, 110, 115, 116, 114, 101, 115,
			115, 101, 100, 32, 115, 121, 108, 108, 97, 98, 108, 101, 115, 41, 10, 9, 9, 9, 9, 10, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115,
			32, 56, 48, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_voices11 = FileInMemory_createWithData (457, reinterpret_cast<const char *> (&espeakdata_voices11_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/ms", 
			U"ms");
		my addItem_move (espeakdata_voices11.move());

		static unsigned char espeakdata_voices12_data[50] = {
			110, 97, 109, 101, 32, 110, 101, 112, 97, 108, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 110, 101, 10, 103, 101, 110, 100, 101, 114,
			32, 109, 97, 108, 101, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 0};
		autoFileInMemory espeakdata_voices12 = FileInMemory_createWithData (49, reinterpret_cast<const char *> (&espeakdata_voices12_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/ne", 
			U"ne");
		my addItem_move (espeakdata_voices12.move());

		static unsigned char espeakdata_voices13_data[26] = {
			110, 97, 109, 101, 32, 112, 117, 110, 106, 97, 98, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 112, 97, 10, 0};
		autoFileInMemory espeakdata_voices13 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeakdata_voices13_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/pa", 
			U"pa");
		my addItem_move (espeakdata_voices13.move());

		static unsigned char espeakdata_voices14_data[64] = {
			110, 97, 109, 101, 32, 116, 97, 109, 105, 108, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 116, 97, 10, 103, 101, 110, 100, 101, 114, 32,
			109, 97, 108, 101, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115, 32,
			56, 48, 10, 0};
		autoFileInMemory espeakdata_voices14 = FileInMemory_createWithData (63, reinterpret_cast<const char *> (&espeakdata_voices14_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/ta", 
			U"ta");
		my addItem_move (espeakdata_voices14.move());

		static unsigned char espeakdata_voices15_data[39] = {
			110, 97, 109, 101, 32, 116, 117, 114, 107, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 116, 114, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices15 = FileInMemory_createWithData (38, reinterpret_cast<const char *> (&espeakdata_voices15_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/tr", 
			U"tr");
		my addItem_move (espeakdata_voices15.move());

		static unsigned char espeakdata_voices16_data[60] = {
			110, 97, 109, 101, 32, 118, 105, 101, 116, 110, 97, 109, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 118, 105, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 119, 111, 114, 100, 115, 32, 49, 10, 112, 105, 116, 99, 104, 32, 56, 48, 32, 49, 49, 56, 10, 0};
		autoFileInMemory espeakdata_voices16 = FileInMemory_createWithData (59, reinterpret_cast<const char *> (&espeakdata_voices16_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/vi", 
			U"vi");
		my addItem_move (espeakdata_voices16.move());

		static unsigned char espeakdata_voices17_data[203] = {
			110, 97, 109, 101, 32, 118, 105, 101, 116, 110, 97, 109, 95, 104, 117, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 118, 105, 45, 104,
			117, 101, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 118, 105, 45, 104, 117, 101, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49,
			10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 119, 111, 114, 100, 115, 32, 49, 10, 112, 105, 116, 99, 104, 32, 56, 50,
			32, 49, 49, 56, 32, 32, 32, 47, 47, 56, 48, 32, 49, 49, 56, 10, 47, 47, 98, 114, 101, 97, 116, 104, 32, 32, 32, 55, 53, 32,
			55, 53, 32, 54, 48, 32, 52, 48, 32, 49, 53, 32, 49, 48, 10, 32, 47, 47, 98, 114, 101, 97, 116, 104, 119, 32, 32, 49, 53, 48,
			32, 49, 53, 48, 32, 50, 48, 48, 32, 50, 48, 48, 32, 52, 48, 48, 32, 52, 48, 48, 10, 32, 118, 111, 105, 99, 105, 110, 103, 32,
			57, 48, 32, 32, 47, 47, 49, 56, 10, 32, 102, 108, 117, 116, 116, 101, 114, 32, 32, 50, 48, 10, 0};
		autoFileInMemory espeakdata_voices17 = FileInMemory_createWithData (202, reinterpret_cast<const char *> (&espeakdata_voices17_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/vi-hue", 
			U"vi-hue");
		my addItem_move (espeakdata_voices17.move());

		static unsigned char espeakdata_voices18_data[203] = {
			110, 97, 109, 101, 32, 118, 105, 101, 116, 110, 97, 109, 95, 115, 103, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 118, 105, 45, 115,
			103, 110, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 118, 105, 45, 115, 103, 110, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 50,
			10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 119, 111, 114, 100, 115, 32, 49, 10, 112, 105, 116, 99, 104, 32, 56, 50,
			32, 49, 49, 56, 32, 32, 32, 47, 47, 56, 48, 32, 49, 49, 56, 10, 47, 47, 98, 114, 101, 97, 116, 104, 32, 32, 32, 55, 53, 32,
			55, 53, 32, 54, 48, 32, 52, 48, 32, 49, 53, 32, 49, 48, 10, 32, 47, 47, 98, 114, 101, 97, 116, 104, 119, 32, 32, 49, 53, 48,
			32, 49, 53, 48, 32, 50, 48, 48, 32, 50, 48, 48, 32, 52, 48, 48, 32, 52, 48, 48, 10, 32, 118, 111, 105, 99, 105, 110, 103, 32,
			57, 48, 32, 32, 47, 47, 49, 56, 10, 32, 102, 108, 117, 116, 116, 101, 114, 32, 32, 50, 48, 10, 0};
		autoFileInMemory espeakdata_voices18 = FileInMemory_createWithData (202, reinterpret_cast<const char *> (&espeakdata_voices18_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/vi-sgn", 
			U"vi-sgn");
		my addItem_move (espeakdata_voices18.move());

		static unsigned char espeakdata_voices19_data[612] = {
			110, 97, 109, 101, 32, 77, 97, 110, 100, 97, 114, 105, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 122, 104, 10, 103, 101, 110, 100,
			101, 114, 32, 109, 97, 108, 101, 10, 119, 111, 114, 100, 115, 32, 49, 10, 112, 105, 116, 99, 104, 32, 56, 48, 32, 49, 49, 56, 10, 10,
			100, 105, 99, 116, 95, 109, 105, 110, 32, 49, 48, 48, 48, 48, 48, 10, 10, 47, 47, 102, 111, 114, 32, 115, 111, 109, 101, 32, 100, 105,
			97, 108, 101, 99, 116, 115, 10, 10, 47, 47, 91, 101, 110, 93, 58, 32, 114, 101, 112, 108, 97, 99, 101, 32, 110, 103, 32, 119, 105, 116,
			104, 32, 110, 10, 47, 47, 91, 122, 104, 93, 58, 32, 239, 191, 189, 222, 186, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239,
			191, 189, 239, 191, 189, 239, 191, 189, 110, 103, 239, 191, 189, 239, 191, 189, 239, 191, 189, 110, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101,
			32, 48, 32, 78, 32, 110, 10, 10, 47, 47, 91, 101, 110, 93, 58, 32, 114, 101, 112, 108, 97, 99, 101, 32, 114, 102, 120, 32, 99, 111,
			110, 115, 111, 110, 97, 110, 116, 115, 10, 47, 47, 91, 122, 104, 93, 58, 32, 239, 191, 189, 222, 190, 239, 191, 189, 239, 191, 189, 239, 191,
			189, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191, 189, 114, 239, 191, 189, 239, 191, 189, 239, 191, 189, 108, 239, 191, 189, 239, 191, 189,
			122, 239, 191, 189, 239, 191, 189, 101, 114, 239, 191, 189, 239, 191, 189, 239, 191, 189, 101, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32,
			48, 32, 116, 115, 46, 104, 32, 116, 115, 104, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 116, 115, 46, 32, 116, 115, 10,
			47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 115, 46, 32, 115, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 105,
			46, 32, 105, 91, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 122, 46, 32, 108, 10, 47, 47, 114, 101, 112, 108, 97, 99,
			101, 32, 48, 32, 122, 46, 32, 122, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 64, 114, 32, 64, 10, 10, 47, 47, 91,
			101, 110, 93, 58, 32, 114, 101, 112, 108, 97, 99, 101, 32, 98, 101, 103, 105, 110, 110, 105, 110, 103, 32, 110, 32, 111, 114, 32, 108, 10,
			47, 47, 91, 122, 104, 93, 58, 32, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191, 189, 110, 108, 239, 191, 189, 239, 191, 189, 110, 239,
			191, 189, 239, 191, 189, 239, 191, 189, 108, 239, 191, 189, 239, 191, 189, 108, 239, 191, 189, 239, 191, 189, 239, 191, 189, 110, 10, 47, 47, 114,
			101, 112, 108, 97, 99, 101, 32, 50, 32, 110, 32, 108, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 50, 32, 108, 32, 110, 10, 10,
			47, 47, 91, 101, 110, 93, 58, 32, 114, 101, 112, 108, 97, 99, 101, 32, 98, 101, 103, 105, 110, 110, 105, 110, 103, 32, 119, 32, 119, 105,
			116, 104, 32, 118, 10, 47, 47, 91, 122, 104, 93, 58, 32, 119, 239, 191, 189, 239, 191, 189, 239, 191, 189, 118, 10, 47, 47, 114, 101, 112,
			108, 97, 99, 101, 32, 48, 32, 119, 32, 32, 118, 0};
		autoFileInMemory espeakdata_voices19 = FileInMemory_createWithData (611, reinterpret_cast<const char *> (&espeakdata_voices19_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/zh", 
			U"zh");
		my addItem_move (espeakdata_voices19.move());

		static unsigned char espeakdata_voices20_data[211] = {
			110, 97, 109, 101, 32, 99, 97, 110, 116, 111, 110, 101, 115, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 122, 104, 45, 121, 117, 101,
			10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 121, 117, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 122, 104, 121, 10, 10, 116, 114,
			97, 110, 115, 108, 97, 116, 111, 114, 32, 122, 104, 121, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 122, 104, 121, 10, 100, 105, 99, 116,
			105, 111, 110, 97, 114, 121, 32, 122, 104, 121, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 47, 47, 32, 105, 110, 116,
			101, 114, 112, 114, 101, 116, 32, 69, 110, 103, 108, 105, 115, 104, 32, 108, 101, 116, 116, 101, 114, 115, 32, 97, 115, 32, 49, 61, 69, 110,
			103, 108, 105, 115, 104, 32, 119, 111, 114, 100, 115, 44, 32, 50, 61, 106, 121, 117, 116, 112, 105, 110, 103, 10, 100, 105, 99, 116, 114, 117,
			108, 101, 115, 32, 49, 10, 10, 119, 111, 114, 100, 115, 32, 49, 10, 100, 105, 99, 116, 95, 109, 105, 110, 32, 49, 48, 48, 48, 48, 10,
			0};
		autoFileInMemory espeakdata_voices20 = FileInMemory_createWithData (210, reinterpret_cast<const char *> (&espeakdata_voices20_data), 
			U"espeak-1.48.04-source/espeak-data/voices/asia/zh-yue", 
			U"zh-yue");
		my addItem_move (espeakdata_voices20.move());

		static unsigned char espeakdata_voices21_data[39] = {
			110, 97, 109, 101, 32, 103, 101, 114, 109, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 100, 101, 10, 103, 101, 110, 100, 101, 114,
			32, 109, 97, 108, 101, 10, 10, 10, 0};
		autoFileInMemory espeakdata_voices21 = FileInMemory_createWithData (38, reinterpret_cast<const char *> (&espeakdata_voices21_data), 
			U"espeak-1.48.04-source/espeak-data/voices/de", 
			U"de");
		my addItem_move (espeakdata_voices21.move());

		static unsigned char espeakdata_voices22_data[39] = {
			110, 97, 109, 101, 32, 100, 101, 102, 97, 117, 108, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices22 = FileInMemory_createWithData (38, reinterpret_cast<const char *> (&espeakdata_voices22_data), 
			U"espeak-1.48.04-source/espeak-data/voices/default", 
			U"default");
		my addItem_move (espeakdata_voices22.move());

		static unsigned char espeakdata_voices23_data[111] = {
			110, 97, 109, 101, 32, 101, 110, 103, 108, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 103, 98, 32, 32, 50,
			10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 117, 107, 32, 32, 50, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110,
			32, 50, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 47, 47, 112, 105, 116, 99, 104, 32, 56, 48, 32, 49, 49, 56,
			10, 10, 116, 117, 110, 101, 115, 32, 115, 49, 32, 99, 49, 32, 113, 49, 32, 101, 49, 10, 0};
		autoFileInMemory espeakdata_voices23 = FileInMemory_createWithData (110, reinterpret_cast<const char *> (&espeakdata_voices23_data), 
			U"espeak-1.48.04-source/espeak-data/voices/en", 
			U"en");
		my addItem_move (espeakdata_voices23.move());

		static unsigned char espeakdata_voices24_data[265] = {
			47, 47, 32, 109, 111, 118, 105, 110, 103, 32, 116, 111, 119, 97, 114, 100, 115, 32, 85, 83, 32, 69, 110, 103, 108, 105, 115, 104, 10, 110,
			97, 109, 101, 32, 101, 110, 103, 108, 105, 115, 104, 45, 117, 115, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 117, 115, 32,
			50, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 114, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 32, 51, 10,
			103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 101, 110, 45, 117, 115, 10, 100, 105,
			99, 116, 114, 117, 108, 101, 115, 32, 51, 32, 54, 10, 111, 112, 116, 105, 111, 110, 32, 114, 101, 100, 117, 99, 101, 95, 116, 32, 49, 10,
			10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 52, 48, 32, 49, 50, 48, 32, 49, 57, 48, 32, 49, 55, 48, 32,
			48, 32, 48, 32, 50, 53, 53, 32, 51, 48, 48, 10, 115, 116, 114, 101, 115, 115, 65, 109, 112, 32, 32, 49, 55, 32, 49, 54, 32, 32,
			49, 57, 32, 49, 57, 32, 32, 49, 57, 32, 49, 57, 32, 32, 50, 49, 32, 49, 57, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48,
			51, 32, 73, 32, 32, 105, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 73, 50, 32, 105, 10, 10, 0};
		autoFileInMemory espeakdata_voices24 = FileInMemory_createWithData (264, reinterpret_cast<const char *> (&espeakdata_voices24_data), 
			U"espeak-1.48.04-source/espeak-data/voices/en-us", 
			U"en-us");
		my addItem_move (espeakdata_voices24.move());

		static unsigned char espeakdata_voices25_data[182] = {
			110, 97, 109, 101, 32, 115, 112, 97, 110, 105, 115, 104, 45, 108, 97, 116, 105, 110, 45, 97, 109, 10, 108, 97, 110, 103, 117, 97, 103, 101,
			32, 101, 115, 45, 108, 97, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 115, 45, 109, 120, 32, 54, 10, 108, 97, 110, 103, 117, 97,
			103, 101, 32, 101, 115, 32, 54, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32,
			101, 115, 45, 108, 97, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 50, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50,
			10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 55, 48, 32, 50, 48, 48, 32, 32, 50, 51, 48, 32, 49, 56, 48,
			32, 32, 48, 32, 48, 32, 32, 50, 53, 48, 32, 50, 56, 48, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 84, 32, 115,
			10, 0};
		autoFileInMemory espeakdata_voices25 = FileInMemory_createWithData (181, reinterpret_cast<const char *> (&espeakdata_voices25_data), 
			U"espeak-1.48.04-source/espeak-data/voices/es-la", 
			U"es-la");
		my addItem_move (espeakdata_voices25.move());

		static unsigned char espeakdata_voices26_data[40] = {
			110, 97, 109, 101, 32, 97, 114, 97, 103, 111, 110, 101, 115, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 110, 10, 103, 101, 110,
			100, 101, 114, 32, 109, 97, 108, 101, 10, 0};
		autoFileInMemory espeakdata_voices26 = FileInMemory_createWithData (39, reinterpret_cast<const char *> (&espeakdata_voices26_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/an", 
			U"an");
		my addItem_move (espeakdata_voices26.move());

		static unsigned char espeakdata_voices27_data[111] = {
			110, 97, 109, 101, 32, 98, 117, 108, 103, 97, 114, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 98, 103, 10, 10, 115, 116,
			114, 101, 115, 115, 65, 109, 112, 32, 49, 51, 32, 49, 50, 32, 49, 55, 32, 49, 55, 32, 50, 48, 32, 50, 50, 32, 50, 50, 32, 50,
			49, 32, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 56, 48, 32, 49, 55, 48, 32, 32, 50, 48, 48, 32, 50,
			48, 48, 32, 32, 50, 48, 48, 32, 50, 48, 48, 32, 32, 50, 49, 48, 32, 50, 50, 48, 0};
		autoFileInMemory espeakdata_voices27 = FileInMemory_createWithData (110, reinterpret_cast<const char *> (&espeakdata_voices27_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/bg", 
			U"bg");
		my addItem_move (espeakdata_voices27.move());

		static unsigned char espeakdata_voices28_data[258] = {
			110, 97, 109, 101, 32, 98, 111, 115, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 98, 115, 10, 112, 104, 111, 110, 101,
			109, 101, 115, 32, 104, 114, 10, 100, 105, 99, 116, 105, 111, 110, 97, 114, 121, 32, 104, 98, 115, 10, 103, 101, 110, 100, 101, 114, 32, 109,
			97, 108, 101, 10, 10, 112, 105, 116, 99, 104, 32, 56, 49, 32, 49, 50, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 48, 32, 49, 48,
			48, 32, 49, 48, 48, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 32, 57, 55, 32, 32, 57, 55, 32, 49, 48,
			48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 50, 32, 32, 57, 55, 32, 32, 57, 55, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110,
			116, 32, 51, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 52, 32, 32, 57, 55, 32,
			49, 48, 50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 53, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10,
			10, 115, 116, 114, 101, 115, 115, 65, 100, 100, 32, 49, 48, 32, 49, 48, 32, 48, 32, 48, 32, 48, 32, 48, 32, 45, 51, 48, 32, 45,
			51, 48, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 51, 32, 52, 10, 0};
		autoFileInMemory espeakdata_voices28 = FileInMemory_createWithData (257, reinterpret_cast<const char *> (&espeakdata_voices28_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/bs", 
			U"bs");
		my addItem_move (espeakdata_voices28.move());

		static unsigned char espeakdata_voices29_data[39] = {
			110, 97, 109, 101, 32, 99, 97, 116, 97, 108, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 99, 97, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices29 = FileInMemory_createWithData (38, reinterpret_cast<const char *> (&espeakdata_voices29_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/ca", 
			U"ca");
		my addItem_move (espeakdata_voices29.move());

		static unsigned char espeakdata_voices30_data[37] = {
			110, 97, 109, 101, 32, 99, 122, 101, 99, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 99, 115, 10, 103, 101, 110, 100, 101, 114, 32,
			109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices30 = FileInMemory_createWithData (36, reinterpret_cast<const char *> (&espeakdata_voices30_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/cs", 
			U"cs");
		my addItem_move (espeakdata_voices30.move());

		static unsigned char espeakdata_voices31_data[50] = {
			108, 97, 110, 103, 117, 97, 103, 101, 32, 99, 121, 10, 110, 97, 109, 101, 32, 119, 101, 108, 115, 104, 10, 103, 101, 110, 100, 101, 114, 32,
			109, 97, 108, 101, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 52, 10, 0};
		autoFileInMemory espeakdata_voices31 = FileInMemory_createWithData (49, reinterpret_cast<const char *> (&espeakdata_voices31_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/cy", 
			U"cy");
		my addItem_move (espeakdata_voices31.move());

		static unsigned char espeakdata_voices32_data[58] = {
			110, 97, 109, 101, 32, 100, 97, 110, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 100, 97, 10, 103, 101, 110, 100, 101, 114,
			32, 109, 97, 108, 101, 10, 10, 116, 117, 110, 101, 115, 32, 115, 50, 32, 99, 50, 32, 113, 50, 32, 101, 50, 10, 10, 10, 0};
		autoFileInMemory espeakdata_voices32 = FileInMemory_createWithData (57, reinterpret_cast<const char *> (&espeakdata_voices32_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/da", 
			U"da");
		my addItem_move (espeakdata_voices32.move());

		static unsigned char espeakdata_voices33_data[38] = {
			110, 97, 109, 101, 32, 103, 114, 101, 101, 107, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 108, 10, 103, 101, 110, 100, 101, 114, 32,
			109, 97, 108, 101, 10, 10, 10, 0};
		autoFileInMemory espeakdata_voices33 = FileInMemory_createWithData (37, reinterpret_cast<const char *> (&espeakdata_voices33_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/el", 
			U"el");
		my addItem_move (espeakdata_voices33.move());

		static unsigned char espeakdata_voices34_data[67] = {
			110, 97, 109, 101, 32, 115, 112, 97, 110, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 115, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 47, 47, 105, 110, 116, 111, 110, 97, 116, 105,
			111, 110, 32, 51, 10, 10, 0};
		autoFileInMemory espeakdata_voices34 = FileInMemory_createWithData (66, reinterpret_cast<const char *> (&espeakdata_voices34_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/es", 
			U"es");
		my addItem_move (espeakdata_voices34.move());

		static unsigned char espeakdata_voices35_data[28] = {
			110, 97, 109, 101, 32, 101, 115, 116, 111, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 116, 10, 10, 0};
		autoFileInMemory espeakdata_voices35 = FileInMemory_createWithData (27, reinterpret_cast<const char *> (&espeakdata_voices35_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/et", 
			U"et");
		my addItem_move (espeakdata_voices35.move());

		static unsigned char espeakdata_voices36_data[39] = {
			110, 97, 109, 101, 32, 102, 105, 110, 110, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 105, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices36 = FileInMemory_createWithData (38, reinterpret_cast<const char *> (&espeakdata_voices36_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/fi", 
			U"fi");
		my addItem_move (espeakdata_voices36.move());

		static unsigned char espeakdata_voices37_data[95] = {
			110, 97, 109, 101, 32, 102, 114, 101, 110, 99, 104, 45, 66, 101, 108, 103, 105, 117, 109, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102,
			114, 45, 98, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 114, 32, 56, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101,
			10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 50, 10, 116, 117, 110, 101, 115, 32, 115, 51, 32, 99, 51, 32, 113, 51, 32, 101,
			51, 10, 10, 10, 0};
		autoFileInMemory espeakdata_voices37 = FileInMemory_createWithData (94, reinterpret_cast<const char *> (&espeakdata_voices37_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/fr-be", 
			U"fr-be");
		my addItem_move (espeakdata_voices37.move());

		static unsigned char espeakdata_voices38_data[66] = {
			110, 97, 109, 101, 32, 105, 114, 105, 115, 104, 45, 103, 97, 101, 105, 108, 103, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 103, 97,
			10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 32, 32, 47, 47, 32, 102, 105, 120, 32, 102, 111, 114, 32, 101, 99, 108, 105,
			112, 115, 105, 115, 10, 0};
		autoFileInMemory espeakdata_voices38 = FileInMemory_createWithData (65, reinterpret_cast<const char *> (&espeakdata_voices38_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/ga", 
			U"ga");
		my addItem_move (espeakdata_voices38.move());

		static unsigned char espeakdata_voices39_data[291] = {
			110, 97, 109, 101, 32, 99, 114, 111, 97, 116, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 114, 10, 108, 97, 110, 103,
			117, 97, 103, 101, 32, 104, 98, 115, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 100, 105, 99, 116, 105, 111, 110, 97,
			114, 121, 32, 104, 98, 115, 10, 10, 47, 47, 32, 97, 116, 116, 114, 105, 98, 117, 116, 101, 115, 32, 116, 111, 119, 97, 114, 100, 115, 32,
			33, 118, 97, 114, 105, 97, 110, 116, 51, 10, 112, 105, 116, 99, 104, 32, 56, 49, 32, 49, 50, 48, 10, 102, 111, 114, 109, 97, 110, 116,
			32, 48, 32, 49, 48, 48, 32, 49, 48, 48, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 32, 57, 55, 32, 32,
			57, 55, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 50, 32, 32, 57, 55, 32, 32, 57, 55, 32, 49, 48, 48, 10, 102,
			111, 114, 109, 97, 110, 116, 32, 51, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 52,
			32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 53, 32, 32, 57, 55, 32, 49, 48, 50,
			32, 49, 48, 48, 10, 10, 115, 116, 114, 101, 115, 115, 65, 100, 100, 32, 49, 48, 32, 49, 48, 32, 48, 32, 48, 32, 48, 32, 48, 32,
			45, 51, 48, 32, 45, 51, 48, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 0};
		autoFileInMemory espeakdata_voices39 = FileInMemory_createWithData (290, reinterpret_cast<const char *> (&espeakdata_voices39_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/hr", 
			U"hr");
		my addItem_move (espeakdata_voices39.move());

		static unsigned char espeakdata_voices40_data[74] = {
			110, 97, 109, 101, 32, 104, 117, 110, 103, 97, 114, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 117, 10, 103, 101, 110,
			100, 101, 114, 32, 109, 97, 108, 101, 10, 111, 112, 116, 105, 111, 110, 32, 98, 114, 97, 99, 107, 101, 116, 32, 48, 32, 48, 10, 112, 105,
			116, 99, 104, 32, 56, 49, 32, 49, 49, 55, 10, 10, 10, 0};
		autoFileInMemory espeakdata_voices40 = FileInMemory_createWithData (73, reinterpret_cast<const char *> (&espeakdata_voices40_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/hu", 
			U"hu");
		my addItem_move (espeakdata_voices40.move());

		static unsigned char espeakdata_voices41_data[41] = {
			110, 97, 109, 101, 32, 105, 99, 101, 108, 97, 110, 100, 105, 99, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 105, 115, 10, 103, 101, 110,
			100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices41 = FileInMemory_createWithData (40, reinterpret_cast<const char *> (&espeakdata_voices41_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/is", 
			U"is");
		my addItem_move (espeakdata_voices41.move());

		static unsigned char espeakdata_voices42_data[75] = {
			32, 10, 110, 97, 109, 101, 32, 105, 116, 97, 108, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 105, 116, 10, 103, 101, 110,
			100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 116, 117, 110, 101, 115, 32, 115, 52, 32, 99, 52, 32, 113, 52, 32, 101, 52, 10, 10, 114,
			101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 105, 32, 73, 10, 0};
		autoFileInMemory espeakdata_voices42 = FileInMemory_createWithData (74, reinterpret_cast<const char *> (&espeakdata_voices42_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/it", 
			U"it");
		my addItem_move (espeakdata_voices42.move());

		static unsigned char espeakdata_voices43_data[43] = {
			10, 110, 97, 109, 101, 32, 108, 105, 116, 104, 117, 97, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 108, 116, 10, 103,
			101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices43 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeakdata_voices43_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/lt", 
			U"lt");
		my addItem_move (espeakdata_voices43.move());

		static unsigned char espeakdata_voices44_data[58] = {
			110, 97, 109, 101, 32, 108, 97, 116, 118, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 108, 118, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 32, 111, 32, 32, 111, 58, 10, 10, 0};
		autoFileInMemory espeakdata_voices44 = FileInMemory_createWithData (57, reinterpret_cast<const char *> (&espeakdata_voices44_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/lv", 
			U"lv");
		my addItem_move (espeakdata_voices44.move());

		static unsigned char espeakdata_voices45_data[42] = {
			110, 97, 109, 101, 32, 109, 97, 99, 101, 100, 111, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 109, 107, 10, 103, 101,
			110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices45 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeakdata_voices45_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/mk", 
			U"mk");
		my addItem_move (espeakdata_voices45.move());

		static unsigned char espeakdata_voices46_data[36] = {
			108, 97, 110, 103, 117, 97, 103, 101, 32, 110, 108, 10, 110, 97, 109, 101, 32, 100, 117, 116, 99, 104, 10, 103, 101, 110, 100, 101, 114, 32,
			109, 97, 108, 101, 10, 0};
		autoFileInMemory espeakdata_voices46 = FileInMemory_createWithData (35, reinterpret_cast<const char *> (&espeakdata_voices46_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/nl", 
			U"nl");
		my addItem_move (espeakdata_voices46.move());

		static unsigned char espeakdata_voices47_data[66] = {
			110, 97, 109, 101, 32, 110, 111, 114, 119, 101, 103, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 110, 111, 10, 108, 97, 110,
			103, 117, 97, 103, 101, 32, 110, 98, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105,
			111, 110, 32, 52, 10, 0};
		autoFileInMemory espeakdata_voices47 = FileInMemory_createWithData (65, reinterpret_cast<const char *> (&espeakdata_voices47_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/no", 
			U"no");
		my addItem_move (espeakdata_voices47.move());

		static unsigned char espeakdata_voices48_data[51] = {
			110, 97, 109, 101, 32, 112, 111, 108, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 112, 108, 10, 103, 101, 110, 100, 101, 114,
			32, 109, 97, 108, 101, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 0};
		autoFileInMemory espeakdata_voices48 = FileInMemory_createWithData (50, reinterpret_cast<const char *> (&espeakdata_voices48_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/pl", 
			U"pl");
		my addItem_move (espeakdata_voices48.move());

		static unsigned char espeakdata_voices49_data[97] = {
			110, 97, 109, 101, 32, 112, 111, 114, 116, 117, 103, 97, 108, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 112, 116, 45, 112, 116, 10, 108,
			97, 110, 103, 117, 97, 103, 101, 32, 112, 116, 32, 54, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 112, 104, 111, 110, 101,
			109, 101, 115, 32, 112, 116, 45, 112, 116, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 105, 110, 116, 111, 110, 97, 116,
			105, 111, 110, 32, 50, 10, 0};
		autoFileInMemory espeakdata_voices49 = FileInMemory_createWithData (96, reinterpret_cast<const char *> (&espeakdata_voices49_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/pt-pt", 
			U"pt-pt");
		my addItem_move (espeakdata_voices49.move());

		static unsigned char espeakdata_voices50_data[41] = {
			110, 97, 109, 101, 32, 114, 111, 109, 97, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 114, 111, 10, 103, 101, 110, 100,
			101, 114, 32, 109, 97, 108, 101, 10, 10, 10, 0};
		autoFileInMemory espeakdata_voices50 = FileInMemory_createWithData (40, reinterpret_cast<const char *> (&espeakdata_voices50_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/ro", 
			U"ro");
		my addItem_move (espeakdata_voices50.move());

		static unsigned char espeakdata_voices51_data[72] = {
			110, 97, 109, 101, 32, 114, 117, 115, 115, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 114, 117, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 97, 32, 97, 35, 10, 10, 100, 105, 99, 116, 95,
			109, 105, 110, 32, 32, 50, 48, 48, 48, 48, 10, 0};
		autoFileInMemory espeakdata_voices51 = FileInMemory_createWithData (71, reinterpret_cast<const char *> (&espeakdata_voices51_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/ru", 
			U"ru");
		my addItem_move (espeakdata_voices51.move());

		static unsigned char espeakdata_voices52_data[38] = {
			110, 97, 109, 101, 32, 115, 108, 111, 118, 97, 107, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 107, 10, 103, 101, 110, 100, 101, 114,
			32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices52 = FileInMemory_createWithData (37, reinterpret_cast<const char *> (&espeakdata_voices52_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/sk", 
			U"sk");
		my addItem_move (espeakdata_voices52.move());

		static unsigned char espeakdata_voices53_data[116] = {
			110, 97, 109, 101, 32, 97, 108, 98, 97, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 113, 10, 103, 101, 110, 100,
			101, 114, 32, 109, 97, 108, 101, 10, 10, 47, 47, 32, 97, 100, 100, 32, 116, 104, 105, 115, 32, 108, 105, 110, 101, 32, 116, 111, 32, 114,
			101, 109, 111, 118, 101, 32, 39, 195, 171, 39, 32, 97, 116, 32, 116, 104, 101, 32, 101, 110, 100, 32, 111, 102, 32, 119, 111, 114, 100, 115,
			10, 47, 47, 32, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 32, 64, 47, 32, 32, 78, 85, 76, 76, 10, 0};
		autoFileInMemory espeakdata_voices53 = FileInMemory_createWithData (115, reinterpret_cast<const char *> (&espeakdata_voices53_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/sq", 
			U"sq");
		my addItem_move (espeakdata_voices53.move());

		static unsigned char espeakdata_voices54_data[278] = {
			110, 97, 109, 101, 32, 115, 101, 114, 98, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 114, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 100, 105, 99, 116, 105, 111, 110, 97, 114, 121, 32, 104, 98, 115, 10, 10, 47, 47, 32, 97, 116, 116, 114,
			105, 98, 117, 116, 101, 115, 32, 116, 111, 119, 97, 114, 100, 115, 32, 33, 118, 97, 114, 105, 97, 110, 116, 51, 32, 112, 105, 116, 99, 104,
			32, 56, 48, 32, 49, 50, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 48, 32, 49, 48, 48, 32, 49, 48, 48, 32, 49, 48, 48, 10,
			102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 32, 57, 55, 32, 32, 57, 55, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32,
			50, 32, 32, 57, 55, 32, 32, 57, 55, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 51, 32, 32, 57, 55, 32, 49, 48,
			50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 52, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 102, 111,
			114, 109, 97, 110, 116, 32, 53, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 10, 115, 116, 114, 101, 115, 115, 65, 100, 100,
			32, 49, 48, 32, 49, 48, 32, 48, 32, 48, 32, 48, 32, 48, 32, 45, 51, 48, 32, 45, 51, 48, 10, 100, 105, 99, 116, 114, 117, 108,
			101, 115, 32, 50, 32, 52, 10, 0};
		autoFileInMemory espeakdata_voices54 = FileInMemory_createWithData (277, reinterpret_cast<const char *> (&espeakdata_voices54_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/sr", 
			U"sr");
		my addItem_move (espeakdata_voices54.move());

		static unsigned char espeakdata_voices55_data[39] = {
			110, 97, 109, 101, 32, 115, 119, 101, 100, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 118, 10, 103, 101, 110, 100, 101,
			114, 32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices55 = FileInMemory_createWithData (38, reinterpret_cast<const char *> (&espeakdata_voices55_data), 
			U"espeak-1.48.04-source/espeak-data/voices/europe/sv", 
			U"sv");
		my addItem_move (espeakdata_voices55.move());

		static unsigned char espeakdata_voices56_data[83] = {
			110, 97, 109, 101, 32, 102, 114, 101, 110, 99, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 114, 45, 102, 114, 10, 108, 97, 110,
			103, 117, 97, 103, 101, 32, 102, 114, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101,
			115, 32, 49, 10, 116, 117, 110, 101, 115, 32, 115, 51, 32, 99, 51, 32, 113, 51, 32, 101, 51, 10, 0};
		autoFileInMemory espeakdata_voices56 = FileInMemory_createWithData (82, reinterpret_cast<const char *> (&espeakdata_voices56_data), 
			U"espeak-1.48.04-source/espeak-data/voices/fr", 
			U"fr");
		my addItem_move (espeakdata_voices56.move());

		static unsigned char espeakdata_voices57_data[68] = {
			110, 97, 109, 101, 32, 97, 102, 114, 105, 107, 97, 97, 110, 115, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 102, 10, 103, 101, 110,
			100, 101, 114, 32, 109, 97, 108, 101, 10, 114, 111, 117, 103, 104, 110, 101, 115, 115, 32, 48, 10, 112, 105, 116, 99, 104, 32, 54, 51, 32,
			49, 50, 48, 10, 10, 10, 10, 0};
		autoFileInMemory espeakdata_voices57 = FileInMemory_createWithData (67, reinterpret_cast<const char *> (&espeakdata_voices57_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/af", 
			U"af");
		my addItem_move (espeakdata_voices57.move());

		static unsigned char espeakdata_voices58_data[202] = {
			110, 97, 109, 101, 32, 101, 110, 103, 108, 105, 115, 104, 45, 110, 111, 114, 116, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110,
			45, 117, 107, 45, 110, 111, 114, 116, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 117, 107, 32, 32, 51, 10, 108, 97,
			110, 103, 117, 97, 103, 101, 32, 101, 110, 32, 53, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 112, 104, 111, 110, 101,
			109, 101, 115, 32, 101, 110, 45, 110, 10, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 54, 48, 32, 49, 53, 48,
			32, 32, 49, 56, 48, 32, 49, 56, 48, 32, 32, 50, 50, 48, 32, 50, 50, 48, 32, 32, 50, 57, 48, 32, 50, 57, 48, 10, 10, 114,
			101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 105, 64, 51, 32, 105, 64, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 78, 32,
			110, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 105, 32, 32, 73, 50, 10, 10, 10, 0};
		autoFileInMemory espeakdata_voices58 = FileInMemory_createWithData (201, reinterpret_cast<const char *> (&espeakdata_voices58_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/en-n", 
			U"en-n");
		my addItem_move (espeakdata_voices58.move());

		static unsigned char espeakdata_voices59_data[195] = {
			110, 97, 109, 101, 32, 101, 110, 103, 108, 105, 115, 104, 95, 114, 112, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 117, 107,
			45, 114, 112, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 117, 107, 32, 32, 52, 10, 108, 97, 110, 103, 117, 97, 103, 101,
			32, 101, 110, 32, 53, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 101, 110,
			45, 114, 112, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 32, 111, 64, 32, 32, 79, 64, 10, 114, 101, 112, 108, 97, 99, 101,
			32, 48, 48, 32, 105, 64, 51, 32, 105, 64, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 73, 32, 105, 10, 114, 101, 112, 108,
			97, 99, 101, 32, 48, 51, 32, 73, 50, 32, 105, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 64, 32, 97, 35, 10, 114, 101,
			112, 108, 97, 99, 101, 32, 48, 51, 32, 51, 32, 97, 35, 10, 0};
		autoFileInMemory espeakdata_voices59 = FileInMemory_createWithData (194, reinterpret_cast<const char *> (&espeakdata_voices59_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/en-rp", 
			U"en-rp");
		my addItem_move (espeakdata_voices59.move());

		static unsigned char espeakdata_voices60_data[247] = {
			110, 97, 109, 101, 32, 101, 110, 45, 115, 99, 111, 116, 116, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 115,
			99, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 32, 52, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 112,
			104, 111, 110, 101, 109, 101, 115, 32, 101, 110, 45, 115, 99, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 53, 32, 54, 32, 55, 10,
			115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 32, 49, 56, 48, 32, 49, 51, 48, 32, 50, 48, 48, 32, 50, 48, 48, 32,
			48, 32, 48, 32, 50, 53, 48, 32, 50, 55, 48, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 64, 32, 86, 10, 114, 101,
			112, 108, 97, 99, 101, 32, 48, 51, 32, 73, 32, 105, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 73, 50, 32, 105, 10, 114,
			101, 112, 108, 97, 99, 101, 32, 48, 49, 32, 97, 73, 32, 97, 73, 50, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 50, 32, 97, 32,
			97, 47, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 50, 32, 117, 58, 32, 85, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 50, 32,
			51, 58, 32, 86, 82, 10, 0};
		autoFileInMemory espeakdata_voices60 = FileInMemory_createWithData (246, reinterpret_cast<const char *> (&espeakdata_voices60_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/en-sc", 
			U"en-sc");
		my addItem_move (espeakdata_voices60.move());

		static unsigned char espeakdata_voices61_data[317] = {
			110, 97, 109, 101, 32, 101, 110, 45, 119, 101, 115, 116, 105, 110, 100, 105, 101, 115, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110,
			45, 119, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 117, 107, 32, 32, 52, 10, 108, 97, 110, 103, 117, 97, 103, 101,
			32, 101, 110, 32, 49, 48, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 101,
			110, 45, 119, 105, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 56, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32,
			49, 55, 53, 32, 49, 55, 53, 32, 32, 49, 55, 53, 32, 49, 55, 53, 32, 32, 50, 50, 48, 32, 50, 50, 48, 32, 32, 50, 53, 48,
			32, 50, 57, 48, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 68, 32, 100, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48,
			48, 32, 84, 32, 116, 91, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 85, 64, 32, 111, 64, 10, 114, 101, 112, 108, 97, 99,
			101, 32, 48, 48, 32, 105, 64, 51, 32, 105, 64, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 64, 32, 97, 35, 10, 114, 101,
			112, 108, 97, 99, 101, 32, 48, 51, 32, 51, 32, 97, 35, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 78, 32, 110, 10, 10,
			102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 32, 57, 56, 32, 32, 49, 48, 48, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116,
			32, 50, 32, 32, 57, 56, 32, 32, 49, 48, 48, 32, 49, 48, 48, 10, 0};
		autoFileInMemory espeakdata_voices61 = FileInMemory_createWithData (316, reinterpret_cast<const char *> (&espeakdata_voices61_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/en-wi", 
			U"en-wi");
		my addItem_move (espeakdata_voices61.move());

		static unsigned char espeakdata_voices62_data[206] = {
			110, 97, 109, 101, 32, 101, 110, 103, 108, 105, 115, 104, 95, 119, 109, 105, 100, 115, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110,
			45, 117, 107, 45, 119, 109, 105, 100, 115, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 117, 107, 32, 57, 10, 108, 97, 110,
			103, 117, 97, 103, 101, 32, 101, 110, 32, 57, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 112, 104, 111, 110, 101, 109,
			101, 115, 32, 101, 110, 45, 119, 109, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 104, 32, 78, 85, 76, 76, 10, 114, 101,
			112, 108, 97, 99, 101, 32, 48, 48, 32, 111, 64, 32, 79, 64, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 105, 64, 51, 32,
			105, 64, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 54, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 52, 10, 115, 116,
			114, 101, 115, 115, 65, 100, 100, 32, 48, 32, 48, 32, 48, 32, 48, 32, 48, 32, 48, 32, 48, 32, 50, 48, 10, 0};
		autoFileInMemory espeakdata_voices62 = FileInMemory_createWithData (205, reinterpret_cast<const char *> (&espeakdata_voices62_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/en-wm", 
			U"en-wm");
		my addItem_move (espeakdata_voices62.move());

		static unsigned char espeakdata_voices63_data[54] = {
			110, 97, 109, 101, 32, 101, 115, 112, 101, 114, 97, 110, 116, 111, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 111, 10, 103, 101, 110,
			100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 97, 112, 111, 115, 116, 114, 111, 112, 104, 101, 32, 50, 10, 0};
		autoFileInMemory espeakdata_voices63 = FileInMemory_createWithData (53, reinterpret_cast<const char *> (&espeakdata_voices63_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/eo", 
			U"eo");
		my addItem_move (espeakdata_voices63.move());

		static unsigned char espeakdata_voices64_data[111] = {
			110, 97, 109, 101, 32, 103, 114, 101, 101, 107, 45, 97, 110, 99, 105, 101, 110, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 103, 114,
			99, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 55,
			48, 32, 49, 55, 48, 32, 32, 49, 57, 48, 32, 49, 57, 48, 32, 32, 48, 32, 48, 32, 32, 50, 51, 48, 32, 50, 52, 48, 10, 100,
			105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 119, 111, 114, 100, 115, 32, 51, 10, 10, 0};
		autoFileInMemory espeakdata_voices64 = FileInMemory_createWithData (110, reinterpret_cast<const char *> (&espeakdata_voices64_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/grc", 
			U"grc");
		my addItem_move (espeakdata_voices64.move());

		static unsigned char espeakdata_voices65_data[70] = {
			110, 97, 109, 101, 32, 108, 111, 106, 98, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 106, 98, 111, 10, 10, 115, 112, 101, 101,
			100, 32, 56, 48, 32, 32, 32, 47, 47, 32, 115, 112, 101, 101, 100, 32, 97, 100, 106, 117, 115, 116, 109, 101, 110, 116, 44, 32, 112, 101,
			114, 99, 101, 110, 116, 97, 103, 101, 10, 0};
		autoFileInMemory espeakdata_voices65 = FileInMemory_createWithData (69, reinterpret_cast<const char *> (&espeakdata_voices65_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/jbo", 
			U"jbo");
		my addItem_move (espeakdata_voices65.move());

		static unsigned char espeakdata_voices66_data[299] = {
			110, 97, 109, 101, 32, 108, 97, 116, 105, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 108, 97, 10, 103, 101, 110, 100, 101, 114, 32,
			109, 97, 108, 101, 10, 115, 116, 114, 101, 115, 115, 114, 117, 108, 101, 32, 50, 32, 51, 51, 32, 48, 32, 50, 10, 47, 47, 32, 114, 117,
			108, 101, 61, 112, 101, 110, 117, 108, 116, 105, 109, 97, 116, 101, 10, 47, 47, 32, 102, 108, 97, 103, 115, 61, 48, 49, 48, 48, 48, 48,
			49, 32, 40, 110, 111, 32, 97, 117, 116, 111, 109, 97, 116, 105, 99, 32, 115, 101, 99, 111, 110, 100, 97, 114, 121, 32, 115, 116, 114, 101,
			115, 115, 32, 43, 32, 100, 111, 110, 39, 116, 32, 115, 116, 114, 101, 115, 32, 109, 111, 110, 111, 115, 121, 108, 108, 97, 98, 108, 101, 115,
			41, 10, 47, 47, 32, 117, 110, 115, 116, 114, 101, 115, 115, 101, 100, 95, 119, 100, 49, 61, 48, 10, 47, 47, 32, 117, 110, 115, 116, 114,
			101, 115, 115, 101, 100, 95, 119, 100, 50, 61, 50, 10, 10, 47, 47, 32, 115, 104, 111, 114, 116, 32, 103, 97, 112, 32, 98, 101, 116, 119,
			101, 101, 110, 32, 119, 111, 114, 100, 115, 10, 119, 111, 114, 100, 115, 32, 50, 10, 10, 47, 47, 32, 78, 111, 116, 101, 58, 32, 84, 104,
			101, 32, 76, 97, 116, 105, 110, 32, 118, 111, 105, 99, 101, 32, 110, 101, 101, 100, 115, 32, 108, 111, 110, 103, 32, 118, 111, 119, 101, 108,
			115, 32, 116, 111, 32, 98, 101, 32, 109, 97, 114, 107, 101, 100, 32, 119, 105, 116, 104, 32, 109, 97, 99, 114, 111, 110, 115, 10, 0};
		autoFileInMemory espeakdata_voices66 = FileInMemory_createWithData (298, reinterpret_cast<const char *> (&espeakdata_voices66_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/la", 
			U"la");
		my addItem_move (espeakdata_voices66.move());

		static unsigned char espeakdata_voices67_data[91] = {
			110, 97, 109, 101, 32, 108, 105, 110, 103, 117, 97, 95, 102, 114, 97, 110, 99, 97, 95, 110, 111, 118, 97, 13, 10, 108, 97, 110, 103, 117,
			97, 103, 101, 32, 108, 102, 110, 13, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 13, 10, 13, 10, 112, 104, 111, 110, 101, 109,
			101, 115, 32, 98, 97, 115, 101, 50, 13, 10, 108, 95, 117, 110, 112, 114, 111, 110, 111, 117, 110, 99, 97, 98, 108, 101, 32, 48, 13, 10,
			0};
		autoFileInMemory espeakdata_voices67 = FileInMemory_createWithData (90, reinterpret_cast<const char *> (&espeakdata_voices67_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/lfn", 
			U"lfn");
		my addItem_move (espeakdata_voices67.move());

		static unsigned char espeakdata_voices68_data[44] = {
			110, 97, 109, 101, 32, 115, 119, 97, 104, 105, 108, 105, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 119, 10,
			103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 0};
		autoFileInMemory espeakdata_voices68 = FileInMemory_createWithData (43, reinterpret_cast<const char *> (&espeakdata_voices68_data), 
			U"espeak-1.48.04-source/espeak-data/voices/other/sw", 
			U"sw");
		my addItem_move (espeakdata_voices68.move());

		static unsigned char espeakdata_voices69_data[107] = {
			110, 97, 109, 101, 32, 98, 114, 97, 122, 105, 108, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 112, 116, 45, 98, 114, 10, 108, 97, 110,
			103, 117, 97, 103, 101, 32, 112, 116, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101,
			115, 32, 50, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 50, 48, 48, 32, 49, 49, 53, 32, 50, 51, 48, 32, 50,
			51, 48, 32, 48, 32, 48, 32, 50, 53, 48, 32, 50, 55, 48, 10, 10, 0};
		autoFileInMemory espeakdata_voices69 = FileInMemory_createWithData (106, reinterpret_cast<const char *> (&espeakdata_voices69_data), 
			U"espeak-1.48.04-source/espeak-data/voices/pt", 
			U"pt");
		my addItem_move (espeakdata_voices69.move());

		static unsigned char espeakdata_voices70_data[32] = {
			110, 97, 109, 101, 32, 97, 109, 104, 97, 114, 105, 99, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 109, 10,
			10, 0};
		autoFileInMemory espeakdata_voices70 = FileInMemory_createWithData (31, reinterpret_cast<const char *> (&espeakdata_voices70_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/am", 
			U"am");
		my addItem_move (espeakdata_voices70.move());

		static unsigned char espeakdata_voices71_data[34] = {
			110, 97, 109, 101, 32, 97, 115, 115, 97, 109, 101, 115, 101, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 115,
			10, 10, 32, 0};
		autoFileInMemory espeakdata_voices71 = FileInMemory_createWithData (33, reinterpret_cast<const char *> (&espeakdata_voices71_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/as", 
			U"as");
		my addItem_move (espeakdata_voices71.move());

		static unsigned char espeakdata_voices72_data[36] = {
			110, 97, 109, 101, 32, 97, 122, 101, 114, 98, 97, 105, 106, 97, 110, 105, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101,
			32, 97, 122, 10, 10, 0};
		autoFileInMemory espeakdata_voices72 = FileInMemory_createWithData (35, reinterpret_cast<const char *> (&espeakdata_voices72_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/az", 
			U"az");
		my addItem_move (espeakdata_voices72.move());

		static unsigned char espeakdata_voices73_data[43] = {
			110, 97, 109, 101, 32, 98, 101, 110, 103, 97, 108, 105, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 98, 110, 10,
			103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 0};
		autoFileInMemory espeakdata_voices73 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeakdata_voices73_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/bn", 
			U"bn");
		my addItem_move (espeakdata_voices73.move());

		static unsigned char espeakdata_voices74_data[31] = {
			110, 97, 109, 101, 32, 98, 97, 115, 113, 117, 101, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 117, 10, 10,
			0};
		autoFileInMemory espeakdata_voices74 = FileInMemory_createWithData (30, reinterpret_cast<const char *> (&espeakdata_voices74_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/eu", 
			U"eu");
		my addItem_move (espeakdata_voices74.move());

		static unsigned char espeakdata_voices75_data[41] = {
			110, 97, 109, 101, 32, 115, 99, 111, 116, 116, 105, 115, 104, 45, 103, 97, 101, 108, 105, 99, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103,
			117, 97, 103, 101, 32, 103, 100, 10, 32, 10, 0};
		autoFileInMemory espeakdata_voices75 = FileInMemory_createWithData (40, reinterpret_cast<const char *> (&espeakdata_voices75_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/gd", 
			U"gd");
		my addItem_move (espeakdata_voices75.move());

		static unsigned char espeakdata_voices76_data[33] = {
			110, 97, 109, 101, 32, 103, 117, 106, 97, 114, 97, 116, 105, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 103, 117,
			10, 10, 0};
		autoFileInMemory espeakdata_voices76 = FileInMemory_createWithData (32, reinterpret_cast<const char *> (&espeakdata_voices76_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/gu", 
			U"gu");
		my addItem_move (espeakdata_voices76.move());

		static unsigned char espeakdata_voices77_data[31] = {
			110, 97, 109, 101, 32, 103, 114, 101, 101, 110, 108, 97, 110, 100, 105, 99, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 108, 10, 10,
			0};
		autoFileInMemory espeakdata_voices77 = FileInMemory_createWithData (30, reinterpret_cast<const char *> (&espeakdata_voices77_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/kl", 
			U"kl");
		my addItem_move (espeakdata_voices77.move());

		static unsigned char espeakdata_voices78_data[69] = {
			110, 97, 109, 101, 32, 107, 111, 114, 101, 97, 110, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 111, 10, 103,
			101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 112, 105, 116, 99, 104, 32, 56, 48, 32, 49, 49, 56, 10, 105, 110, 116, 111, 110, 97,
			116, 105, 111, 110, 32, 50, 10, 10, 0};
		autoFileInMemory espeakdata_voices78 = FileInMemory_createWithData (68, reinterpret_cast<const char *> (&espeakdata_voices78_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/ko", 
			U"ko");
		my addItem_move (espeakdata_voices78.move());

		static unsigned char espeakdata_voices79_data[125] = {
			110, 97, 109, 101, 32, 110, 97, 104, 117, 97, 116, 108, 45, 99, 108, 97, 115, 115, 105, 99, 97, 108, 10, 108, 97, 110, 103, 117, 97, 103,
			101, 32, 110, 99, 105, 10, 103, 101, 110, 100, 101, 114, 32, 109, 97, 108, 101, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32,
			51, 10, 115, 116, 114, 101, 115, 115, 114, 117, 108, 101, 32, 50, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 32, 49,
			57, 48, 32, 32, 49, 57, 48, 32, 32, 50, 48, 48, 32, 32, 50, 48, 48, 32, 32, 48, 32, 32, 48, 32, 32, 50, 50, 48, 32, 32,
			50, 52, 48, 10, 0};
		autoFileInMemory espeakdata_voices79 = FileInMemory_createWithData (124, reinterpret_cast<const char *> (&espeakdata_voices79_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/nci", 
			U"nci");
		my addItem_move (espeakdata_voices79.move());

		static unsigned char espeakdata_voices80_data[29] = {
			110, 97, 109, 101, 32, 111, 114, 105, 121, 97, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 111, 114, 10, 0};
		autoFileInMemory espeakdata_voices80 = FileInMemory_createWithData (28, reinterpret_cast<const char *> (&espeakdata_voices80_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/or", 
			U"or");
		my addItem_move (espeakdata_voices80.move());

		static unsigned char espeakdata_voices81_data[52] = {
			110, 97, 109, 101, 32, 112, 97, 112, 105, 97, 109, 101, 110, 116, 111, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32,
			112, 97, 112, 10, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 98, 97, 115, 101, 50, 10, 10, 0};
		autoFileInMemory espeakdata_voices81 = FileInMemory_createWithData (51, reinterpret_cast<const char *> (&espeakdata_voices81_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/pap", 
			U"pap");
		my addItem_move (espeakdata_voices81.move());

		static unsigned char espeakdata_voices82_data[45] = {
			110, 97, 109, 101, 32, 115, 105, 110, 104, 97, 108, 97, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 105, 10,
			10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 0};
		autoFileInMemory espeakdata_voices82 = FileInMemory_createWithData (44, reinterpret_cast<const char *> (&espeakdata_voices82_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/si", 
			U"si");
		my addItem_move (espeakdata_voices82.move());

		static unsigned char espeakdata_voices83_data[34] = {
			110, 97, 109, 101, 32, 115, 108, 111, 118, 101, 110, 105, 97, 110, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115,
			108, 10, 32, 0};
		autoFileInMemory espeakdata_voices83 = FileInMemory_createWithData (33, reinterpret_cast<const char *> (&espeakdata_voices83_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/sl", 
			U"sl");
		my addItem_move (espeakdata_voices83.move());

		static unsigned char espeakdata_voices84_data[60] = {
			110, 97, 109, 101, 32, 116, 101, 108, 117, 103, 117, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 116, 101, 10, 10,
			105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 47, 47, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_voices84 = FileInMemory_createWithData (59, reinterpret_cast<const char *> (&espeakdata_voices84_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/te", 
			U"te");
		my addItem_move (espeakdata_voices84.move());

		static unsigned char espeakdata_voices85_data[43] = {
			110, 97, 109, 101, 32, 117, 114, 100, 117, 45, 116, 101, 115, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 117, 114, 10, 10, 115, 116,
			114, 101, 115, 115, 114, 117, 108, 101, 32, 54, 10, 10, 0};
		autoFileInMemory espeakdata_voices85 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeakdata_voices85_data), 
			U"espeak-1.48.04-source/espeak-data/voices/test/ur", 
			U"ur");
		my addItem_move (espeakdata_voices85.move());

		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySet not created.");
	}
}


