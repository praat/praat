/* File espeakdata_languages.cpp (version 1.49.3-dev) was automatically generated on Sat Oct  7 14:28:07 2017 from files in espeak-ng-data/lang  */

#include "espeakdata_FileInMemory.h"
#include "Collection.h"
#include "FileInMemory.h"
#include "melder.h"

autoFileInMemorySet create_espeakdata_languages () {
	try {
		autoFileInMemorySet me = FileInMemorySet_create ();
		static unsigned char espeakdata_languages1_data[62] = {
			110, 97, 109, 101, 32, 86, 105, 101, 116, 110, 97, 109, 101, 115, 101, 32, 40, 78, 111, 114, 116, 104, 101, 114, 110, 41, 10, 108, 97, 110,
			103, 117, 97, 103, 101, 32, 118, 105, 10, 10, 119, 111, 114, 100, 115, 32, 49, 10, 112, 105, 116, 99, 104, 32, 56, 48, 32, 49, 49, 56,
			10, 0};
		autoFileInMemory espeakdata_languages1 = FileInMemory_createWithData (61, reinterpret_cast<const char *> (&espeakdata_languages1_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/aav/vi", 
			U"vi");
		me -> addItem_move (espeakdata_languages1.move());

		static unsigned char espeakdata_languages2_data[209] = {
			110, 97, 109, 101, 32, 86, 105, 101, 116, 110, 97, 109, 101, 115, 101, 32, 40, 67, 101, 110, 116, 114, 97, 108, 41, 10, 108, 97, 110, 103,
			117, 97, 103, 101, 32, 118, 105, 45, 118, 110, 45, 120, 45, 99, 101, 110, 116, 114, 97, 108, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32,
			118, 105, 45, 104, 117, 101, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 10, 119, 111, 114, 100, 115, 32, 49, 10, 112, 105,
			116, 99, 104, 32, 56, 50, 32, 49, 49, 56, 32, 32, 32, 47, 47, 56, 48, 32, 49, 49, 56, 10, 47, 47, 98, 114, 101, 97, 116, 104,
			32, 32, 32, 55, 53, 32, 55, 53, 32, 54, 48, 32, 52, 48, 32, 49, 53, 32, 49, 48, 10, 32, 47, 47, 98, 114, 101, 97, 116, 104,
			119, 32, 32, 49, 53, 48, 32, 49, 53, 48, 32, 50, 48, 48, 32, 50, 48, 48, 32, 52, 48, 48, 32, 52, 48, 48, 10, 32, 118, 111,
			105, 99, 105, 110, 103, 32, 57, 48, 32, 32, 47, 47, 49, 56, 10, 32, 102, 108, 117, 116, 116, 101, 114, 32, 32, 50, 48, 10, 0};
		autoFileInMemory espeakdata_languages2 = FileInMemory_createWithData (208, reinterpret_cast<const char *> (&espeakdata_languages2_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/aav/vi-VN-x-central", 
			U"vi-VN-x-central");
		me -> addItem_move (espeakdata_languages2.move());

		static unsigned char espeakdata_languages3_data[208] = {
			110, 97, 109, 101, 32, 86, 105, 101, 116, 110, 97, 109, 101, 115, 101, 32, 40, 83, 111, 117, 116, 104, 101, 114, 110, 41, 10, 108, 97, 110,
			103, 117, 97, 103, 101, 32, 118, 105, 45, 118, 110, 45, 120, 45, 115, 111, 117, 116, 104, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 118,
			105, 45, 115, 103, 110, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 50, 10, 10, 119, 111, 114, 100, 115, 32, 49, 10, 112, 105, 116,
			99, 104, 32, 56, 50, 32, 49, 49, 56, 32, 32, 32, 47, 47, 56, 48, 32, 49, 49, 56, 10, 47, 47, 98, 114, 101, 97, 116, 104, 32,
			32, 32, 55, 53, 32, 55, 53, 32, 54, 48, 32, 52, 48, 32, 49, 53, 32, 49, 48, 10, 32, 47, 47, 98, 114, 101, 97, 116, 104, 119,
			32, 32, 49, 53, 48, 32, 49, 53, 48, 32, 50, 48, 48, 32, 50, 48, 48, 32, 52, 48, 48, 32, 52, 48, 48, 10, 32, 118, 111, 105,
			99, 105, 110, 103, 32, 57, 48, 32, 32, 47, 47, 49, 56, 10, 32, 102, 108, 117, 116, 116, 101, 114, 32, 32, 50, 48, 10, 0};
		autoFileInMemory espeakdata_languages3 = FileInMemory_createWithData (207, reinterpret_cast<const char *> (&espeakdata_languages3_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/aav/vi-VN-x-south", 
			U"vi-VN-x-south");
		me -> addItem_move (espeakdata_languages3.move());

		static unsigned char espeakdata_languages4_data[42] = {
			110, 97, 109, 101, 32, 69, 115, 112, 101, 114, 97, 110, 116, 111, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 111, 10, 10, 97, 112,
			111, 115, 116, 114, 111, 112, 104, 101, 32, 50, 10, 0};
		autoFileInMemory espeakdata_languages4 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeakdata_languages4_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/art/eo", 
			U"eo");
		me -> addItem_move (espeakdata_languages4.move());

		static unsigned char espeakdata_languages5_data[30] = {
			110, 97, 109, 101, 32, 73, 110, 116, 101, 114, 108, 105, 110, 103, 117, 97, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 105, 97, 10, 0};
		autoFileInMemory espeakdata_languages5 = FileInMemory_createWithData (29, reinterpret_cast<const char *> (&espeakdata_languages5_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/art/ia", 
			U"ia");
		me -> addItem_move (espeakdata_languages5.move());

		static unsigned char espeakdata_languages6_data[70] = {
			110, 97, 109, 101, 32, 76, 111, 106, 98, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 106, 98, 111, 10, 10, 115, 112, 101, 101,
			100, 32, 56, 48, 32, 32, 32, 47, 47, 32, 115, 112, 101, 101, 100, 32, 97, 100, 106, 117, 115, 116, 109, 101, 110, 116, 44, 32, 112, 101,
			114, 99, 101, 110, 116, 97, 103, 101, 10, 0};
		autoFileInMemory espeakdata_languages6 = FileInMemory_createWithData (69, reinterpret_cast<const char *> (&espeakdata_languages6_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/art/jbo", 
			U"jbo");
		me -> addItem_move (espeakdata_languages6.move());

		static unsigned char espeakdata_languages7_data[136] = {
			110, 97, 109, 101, 32, 76, 105, 110, 103, 117, 97, 32, 70, 114, 97, 110, 99, 97, 32, 78, 111, 118, 97, 13, 10, 108, 97, 110, 103, 117,
			97, 103, 101, 32, 108, 102, 110, 13, 10, 13, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 98, 97, 115, 101, 50, 13, 10, 108, 95, 117,
			110, 112, 114, 111, 110, 111, 117, 110, 99, 97, 98, 108, 101, 32, 48, 13, 10, 110, 117, 109, 98, 101, 114, 115, 32, 50, 32, 51, 13, 10,
			13, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 32, 49, 53, 48, 32, 49, 52, 48, 32, 49, 56, 48, 32, 49, 56,
			48, 32, 48, 32, 48, 32, 50, 48, 48, 32, 50, 48, 48, 13, 10, 0};
		autoFileInMemory espeakdata_languages7 = FileInMemory_createWithData (135, reinterpret_cast<const char *> (&espeakdata_languages7_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/art/lfn", 
			U"lfn");
		me -> addItem_move (espeakdata_languages7.move());

		static unsigned char espeakdata_languages8_data[115] = {
			110, 97, 109, 101, 32, 78, 97, 104, 117, 97, 116, 108, 32, 40, 67, 108, 97, 115, 115, 105, 99, 97, 108, 41, 10, 108, 97, 110, 103, 117,
			97, 103, 101, 32, 110, 99, 105, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 51, 10, 115, 116, 114, 101, 115, 115, 114, 117,
			108, 101, 32, 50, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 32, 49, 57, 48, 32, 32, 49, 57, 48, 32, 32, 50,
			48, 48, 32, 32, 50, 48, 48, 32, 32, 48, 32, 32, 48, 32, 32, 50, 50, 48, 32, 32, 50, 52, 48, 10, 0};
		autoFileInMemory espeakdata_languages8 = FileInMemory_createWithData (114, reinterpret_cast<const char *> (&espeakdata_languages8_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/azc/nci", 
			U"nci");
		me -> addItem_move (espeakdata_languages8.move());

		static unsigned char espeakdata_languages9_data[29] = {
			110, 97, 109, 101, 32, 76, 105, 116, 104, 117, 97, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 108, 116, 10, 0};
		autoFileInMemory espeakdata_languages9 = FileInMemory_createWithData (28, reinterpret_cast<const char *> (&espeakdata_languages9_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/bat/lt", 
			U"lt");
		me -> addItem_move (espeakdata_languages9.move());

		static unsigned char espeakdata_languages10_data[313] = {
			110, 97, 109, 101, 32, 76, 97, 116, 118, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 108, 118, 10, 109, 97, 105, 110, 116,
			97, 105, 110, 101, 114, 32, 86, 97, 108, 100, 105, 115, 32, 86, 105, 116, 111, 108, 105, 110, 115, 32, 60, 118, 97, 108, 100, 105, 115, 46,
			118, 105, 116, 111, 108, 105, 110, 115, 64, 111, 100, 111, 46, 108, 118, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116, 117, 114, 101,
			10, 119, 111, 114, 100, 115, 32, 48, 32, 50, 10, 112, 105, 116, 99, 104, 32, 54, 52, 32, 49, 49, 56, 10, 98, 114, 101, 97, 116, 104,
			32, 32, 32, 53, 32, 50, 32, 48, 32, 48, 32, 48, 32, 49, 10, 98, 114, 101, 97, 116, 104, 119, 32, 32, 50, 48, 32, 52, 48, 32,
			53, 48, 48, 32, 49, 50, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 57, 53, 32, 49, 48, 48, 32, 49, 48, 48, 32,
			45, 53, 10, 47, 47, 116, 117, 110, 101, 115, 32, 115, 53, 32, 99, 53, 32, 113, 53, 32, 101, 53, 10, 116, 111, 110, 101, 32, 49, 53,
			48, 32, 50, 50, 48, 32, 52, 53, 48, 32, 50, 53, 53, 32, 56, 53, 48, 32, 49, 48, 32, 51, 53, 48, 48, 32, 50, 53, 53, 10,
			115, 116, 114, 101, 115, 115, 65, 109, 112, 32, 49, 50, 32, 49, 48, 32, 49, 48, 32, 56, 32, 48, 32, 48, 32, 49, 56, 32, 49, 53,
			10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 56, 48, 32, 49, 56, 48, 32, 49, 56, 48, 32, 49, 54, 48, 32,
			48, 32, 48, 32, 50, 52, 48, 32, 49, 56, 48, 10, 0};
		autoFileInMemory espeakdata_languages10 = FileInMemory_createWithData (312, reinterpret_cast<const char *> (&espeakdata_languages10_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/bat/lv", 
			U"lv");
		me -> addItem_move (espeakdata_languages10.move());

		static unsigned char espeakdata_languages11_data[42] = {
			110, 97, 109, 101, 32, 83, 119, 97, 104, 105, 108, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 119, 10, 10, 115, 116, 97, 116,
			117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages11 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeakdata_languages11_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/bnt/sw", 
			U"sw");
		me -> addItem_move (espeakdata_languages11.move());

		static unsigned char espeakdata_languages12_data[43] = {
			110, 97, 109, 101, 32, 83, 101, 116, 115, 119, 97, 110, 97, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 116, 110, 10, 10, 115, 116, 97,
			116, 117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages12 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeakdata_languages12_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/bnt/tn", 
			U"tn");
		me -> addItem_move (espeakdata_languages12.move());

		static unsigned char espeakdata_languages13_data[27] = {
			110, 97, 109, 101, 32, 71, 101, 111, 114, 103, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 97, 10, 0};
		autoFileInMemory espeakdata_languages13 = FileInMemory_createWithData (26, reinterpret_cast<const char *> (&espeakdata_languages13_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/ccs/ka", 
			U"ka");
		me -> addItem_move (espeakdata_languages13.move());

		static unsigned char espeakdata_languages14_data[38] = {
			110, 97, 109, 101, 32, 87, 101, 108, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 99, 121, 10, 10, 105, 110, 116, 111, 110, 97,
			116, 105, 111, 110, 32, 52, 10, 0};
		autoFileInMemory espeakdata_languages14 = FileInMemory_createWithData (37, reinterpret_cast<const char *> (&espeakdata_languages14_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/cel/cy", 
			U"cy");
		me -> addItem_move (espeakdata_languages14.move());

		static unsigned char espeakdata_languages15_data[67] = {
			110, 97, 109, 101, 32, 71, 97, 101, 108, 105, 99, 32, 40, 73, 114, 105, 115, 104, 41, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 103,
			97, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 32, 32, 47, 47, 32, 102, 105, 120, 32, 102, 111, 114, 32, 101, 99, 108,
			105, 112, 115, 105, 115, 10, 0};
		autoFileInMemory espeakdata_languages15 = FileInMemory_createWithData (66, reinterpret_cast<const char *> (&espeakdata_languages15_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/cel/ga", 
			U"ga");
		me -> addItem_move (espeakdata_languages15.move());

		static unsigned char espeakdata_languages16_data[52] = {
			110, 97, 109, 101, 32, 71, 97, 101, 108, 105, 99, 32, 40, 83, 99, 111, 116, 116, 105, 115, 104, 41, 10, 108, 97, 110, 103, 117, 97, 103,
			101, 32, 103, 100, 10, 10, 115, 116, 97, 116, 117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages16 = FileInMemory_createWithData (51, reinterpret_cast<const char *> (&espeakdata_languages16_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/cel/gd", 
			U"gd");
		me -> addItem_move (espeakdata_languages16.move());

		static unsigned char espeakdata_languages17_data[40] = {
			110, 97, 109, 101, 32, 79, 114, 111, 109, 111, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 111, 109, 10, 10, 115, 116, 97, 116, 117, 115,
			32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages17 = FileInMemory_createWithData (39, reinterpret_cast<const char *> (&espeakdata_languages17_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/cus/om", 
			U"om");
		me -> addItem_move (espeakdata_languages17.move());

		static unsigned char espeakdata_languages18_data[56] = {
			110, 97, 109, 101, 32, 75, 97, 110, 110, 97, 100, 97, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 110, 10, 10, 105, 110, 116, 111,
			110, 97, 116, 105, 111, 110, 32, 50, 10, 47, 47, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_languages18 = FileInMemory_createWithData (55, reinterpret_cast<const char *> (&espeakdata_languages18_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/dra/kn", 
			U"kn");
		me -> addItem_move (espeakdata_languages18.move());

		static unsigned char espeakdata_languages19_data[58] = {
			110, 97, 109, 101, 32, 77, 97, 108, 97, 121, 97, 108, 97, 109, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 109, 108, 10, 10, 105, 110,
			116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 47, 47, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_languages19 = FileInMemory_createWithData (57, reinterpret_cast<const char *> (&espeakdata_languages19_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/dra/ml", 
			U"ml");
		me -> addItem_move (espeakdata_languages19.move());

		static unsigned char espeakdata_languages20_data[52] = {
			110, 97, 109, 101, 32, 84, 97, 109, 105, 108, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 116, 97, 10, 10, 105, 110, 116, 111, 110, 97,
			116, 105, 111, 110, 32, 50, 10, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_languages20 = FileInMemory_createWithData (51, reinterpret_cast<const char *> (&espeakdata_languages20_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/dra/ta", 
			U"ta");
		me -> addItem_move (espeakdata_languages20.move());

		static unsigned char espeakdata_languages21_data[71] = {
			110, 97, 109, 101, 32, 84, 101, 108, 117, 103, 117, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 116, 101, 10, 10, 115, 116, 97, 116, 117,
			115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 47, 47, 99, 111, 110, 115,
			111, 110, 97, 110, 116, 115, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_languages21 = FileInMemory_createWithData (70, reinterpret_cast<const char *> (&espeakdata_languages21_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/dra/te", 
			U"te");
		me -> addItem_move (espeakdata_languages21.move());

		static unsigned char espeakdata_languages22_data[31] = {
			110, 97, 109, 101, 32, 71, 114, 101, 101, 110, 108, 97, 110, 100, 105, 99, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 108, 10, 10,
			0};
		autoFileInMemory espeakdata_languages22 = FileInMemory_createWithData (30, reinterpret_cast<const char *> (&espeakdata_languages22_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/esx/kl", 
			U"kl");
		me -> addItem_move (espeakdata_languages22.move());

		static unsigned char espeakdata_languages23_data[41] = {
			110, 97, 109, 101, 32, 66, 97, 115, 113, 117, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 117, 10, 10, 115, 116, 97, 116, 117,
			115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages23 = FileInMemory_createWithData (40, reinterpret_cast<const char *> (&espeakdata_languages23_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/eu", 
			U"eu");
		me -> addItem_move (espeakdata_languages23.move());

		static unsigned char espeakdata_languages24_data[44] = {
			110, 97, 109, 101, 32, 68, 97, 110, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 100, 97, 10, 10, 116, 117, 110, 101, 115,
			32, 115, 50, 32, 99, 50, 32, 113, 50, 32, 101, 50, 10, 0};
		autoFileInMemory espeakdata_languages24 = FileInMemory_createWithData (43, reinterpret_cast<const char *> (&espeakdata_languages24_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmq/da", 
			U"da");
		me -> addItem_move (espeakdata_languages24.move());

		static unsigned char espeakdata_languages25_data[28] = {
			110, 97, 109, 101, 32, 73, 99, 101, 108, 97, 110, 100, 105, 99, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 105, 115, 10, 0};
		autoFileInMemory espeakdata_languages25 = FileInMemory_createWithData (27, reinterpret_cast<const char *> (&espeakdata_languages25_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmq/is", 
			U"is");
		me -> addItem_move (espeakdata_languages25.move());

		static unsigned char espeakdata_languages26_data[88] = {
			110, 97, 109, 101, 32, 78, 111, 114, 119, 101, 103, 105, 97, 110, 32, 66, 111, 107, 109, 195, 165, 108, 10, 108, 97, 110, 103, 117, 97, 103,
			101, 32, 110, 98, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 110, 111, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 110, 111, 10, 100,
			105, 99, 116, 105, 111, 110, 97, 114, 121, 32, 110, 111, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 52, 10, 0};
		autoFileInMemory espeakdata_languages26 = FileInMemory_createWithData (87, reinterpret_cast<const char *> (&espeakdata_languages26_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmq/nb", 
			U"nb");
		me -> addItem_move (espeakdata_languages26.move());

		static unsigned char espeakdata_languages27_data[26] = {
			110, 97, 109, 101, 32, 83, 119, 101, 100, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 118, 10, 0};
		autoFileInMemory espeakdata_languages27 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeakdata_languages27_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmq/sv", 
			U"sv");
		me -> addItem_move (espeakdata_languages27.move());

		static unsigned char espeakdata_languages28_data[124] = {
			110, 97, 109, 101, 32, 65, 102, 114, 105, 107, 97, 97, 110, 115, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 102, 10, 10, 109, 97,
			105, 110, 116, 97, 105, 110, 101, 114, 32, 67, 104, 114, 105, 115, 116, 111, 32, 100, 101, 32, 75, 108, 101, 114, 107, 32, 60, 99, 104, 114,
			105, 115, 116, 111, 100, 101, 107, 108, 101, 114, 107, 64, 103, 109, 97, 105, 108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32,
			109, 97, 116, 117, 114, 101, 10, 10, 114, 111, 117, 103, 104, 110, 101, 115, 115, 32, 48, 10, 112, 105, 116, 99, 104, 32, 54, 51, 32, 49,
			50, 48, 10, 0};
		autoFileInMemory espeakdata_languages28 = FileInMemory_createWithData (123, reinterpret_cast<const char *> (&espeakdata_languages28_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/af", 
			U"af");
		me -> addItem_move (espeakdata_languages28.move());

		static unsigned char espeakdata_languages29_data[25] = {
			110, 97, 109, 101, 32, 71, 101, 114, 109, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 100, 101, 10, 0};
		autoFileInMemory espeakdata_languages29 = FileInMemory_createWithData (24, reinterpret_cast<const char *> (&espeakdata_languages29_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/de", 
			U"de");
		me -> addItem_move (espeakdata_languages29.move());

		static unsigned char espeakdata_languages30_data[141] = {
			110, 97, 109, 101, 32, 69, 110, 103, 108, 105, 115, 104, 32, 40, 71, 114, 101, 97, 116, 32, 66, 114, 105, 116, 97, 105, 110, 41, 10, 108,
			97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 103, 98, 32, 32, 50, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 32, 50,
			10, 10, 109, 97, 105, 110, 116, 97, 105, 110, 101, 114, 32, 82, 101, 101, 99, 101, 32, 72, 46, 32, 68, 117, 110, 110, 32, 60, 109, 115,
			99, 108, 114, 104, 100, 64, 103, 109, 97, 105, 108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116, 117, 114, 101,
			10, 10, 116, 117, 110, 101, 115, 32, 115, 49, 32, 99, 49, 32, 113, 49, 32, 101, 49, 10, 0};
		autoFileInMemory espeakdata_languages30 = FileInMemory_createWithData (140, reinterpret_cast<const char *> (&espeakdata_languages30_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/en", 
			U"en");
		me -> addItem_move (espeakdata_languages30.move());

		static unsigned char espeakdata_languages31_data[336] = {
			110, 97, 109, 101, 32, 69, 110, 103, 108, 105, 115, 104, 32, 40, 67, 97, 114, 105, 98, 98, 101, 97, 110, 41, 10, 108, 97, 110, 103, 117,
			97, 103, 101, 32, 101, 110, 45, 48, 50, 57, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 32, 49, 48, 10, 10, 109, 97, 105,
			110, 116, 97, 105, 110, 101, 114, 32, 82, 101, 101, 99, 101, 32, 72, 46, 32, 68, 117, 110, 110, 32, 60, 109, 115, 99, 108, 114, 104, 100,
			64, 103, 109, 97, 105, 108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116, 117, 114, 101, 10, 10, 112, 104, 111,
			110, 101, 109, 101, 115, 32, 101, 110, 45, 119, 105, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 56, 10, 115, 116, 114, 101, 115, 115,
			76, 101, 110, 103, 116, 104, 32, 49, 55, 53, 32, 49, 55, 53, 32, 32, 49, 55, 53, 32, 49, 55, 53, 32, 32, 50, 50, 48, 32, 50,
			50, 48, 32, 32, 50, 53, 48, 32, 50, 57, 48, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 68, 32, 100, 10, 114, 101,
			112, 108, 97, 99, 101, 32, 48, 48, 32, 84, 32, 116, 91, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 85, 64, 32, 111, 64,
			10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 64, 32, 97, 35, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 51, 32,
			97, 35, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 78, 32, 110, 10, 10, 102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 32,
			57, 56, 32, 32, 49, 48, 48, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 50, 32, 32, 57, 56, 32, 32, 49, 48, 48,
			32, 49, 48, 48, 10, 0};
		autoFileInMemory espeakdata_languages31 = FileInMemory_createWithData (335, reinterpret_cast<const char *> (&espeakdata_languages31_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/en-029", 
			U"en-029");
		me -> addItem_move (espeakdata_languages31.move());

		static unsigned char espeakdata_languages32_data[296] = {
			110, 97, 109, 101, 32, 69, 110, 103, 108, 105, 115, 104, 32, 40, 83, 99, 111, 116, 108, 97, 110, 100, 41, 10, 108, 97, 110, 103, 117, 97,
			103, 101, 32, 101, 110, 45, 103, 98, 45, 115, 99, 111, 116, 108, 97, 110, 100, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 32,
			52, 10, 10, 109, 97, 105, 110, 116, 97, 105, 110, 101, 114, 32, 82, 101, 101, 99, 101, 32, 72, 46, 32, 68, 117, 110, 110, 32, 60, 109,
			115, 99, 108, 114, 104, 100, 64, 103, 109, 97, 105, 108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116, 117, 114,
			101, 10, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 101, 110, 45, 115, 99, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 50, 32,
			53, 32, 54, 32, 55, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 32, 49, 56, 48, 32, 49, 51, 48, 32, 50, 48,
			48, 32, 50, 48, 48, 32, 48, 32, 48, 32, 50, 53, 48, 32, 50, 55, 48, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32,
			64, 32, 86, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 73, 32, 105, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32,
			73, 50, 32, 105, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 49, 32, 97, 73, 32, 97, 73, 50, 10, 114, 101, 112, 108, 97, 99, 101,
			32, 48, 50, 32, 97, 32, 97, 47, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 50, 32, 117, 58, 32, 85, 10, 0};
		autoFileInMemory espeakdata_languages32 = FileInMemory_createWithData (295, reinterpret_cast<const char *> (&espeakdata_languages32_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/en-GB-scotland", 
			U"en-GB-scotland");
		me -> addItem_move (espeakdata_languages32.move());

		static unsigned char espeakdata_languages33_data[239] = {
			110, 97, 109, 101, 32, 69, 110, 103, 108, 105, 115, 104, 32, 40, 76, 97, 110, 99, 97, 115, 116, 101, 114, 41, 10, 108, 97, 110, 103, 117,
			97, 103, 101, 32, 101, 110, 45, 103, 98, 45, 120, 45, 103, 98, 99, 108, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110,
			45, 103, 98, 32, 32, 51, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 32, 53, 10, 10, 109, 97, 105, 110, 116, 97, 105, 110,
			101, 114, 32, 82, 101, 101, 99, 101, 32, 72, 46, 32, 68, 117, 110, 110, 32, 60, 109, 115, 99, 108, 114, 104, 100, 64, 103, 109, 97, 105,
			108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116, 117, 114, 101, 10, 10, 112, 104, 111, 110, 101, 109, 101, 115,
			32, 101, 110, 45, 110, 10, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 54, 48, 32, 49, 53, 48, 32, 32, 49,
			56, 48, 32, 49, 56, 48, 32, 32, 50, 50, 48, 32, 50, 50, 48, 32, 32, 50, 57, 48, 32, 50, 57, 48, 10, 10, 114, 101, 112, 108,
			97, 99, 101, 32, 48, 51, 32, 78, 32, 110, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 105, 32, 32, 73, 50, 10, 0};
		autoFileInMemory espeakdata_languages33 = FileInMemory_createWithData (238, reinterpret_cast<const char *> (&espeakdata_languages33_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/en-GB-x-gbclan", 
			U"en-GB-x-gbclan");
		me -> addItem_move (espeakdata_languages33.move());

		static unsigned char espeakdata_languages34_data[189] = {
			110, 97, 109, 101, 32, 69, 110, 103, 108, 105, 115, 104, 32, 40, 87, 101, 115, 116, 32, 77, 105, 100, 108, 97, 110, 100, 115, 41, 10, 108,
			97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 103, 98, 45, 120, 45, 103, 98, 99, 119, 109, 100, 10, 108, 97, 110, 103, 117, 97, 103,
			101, 32, 101, 110, 45, 103, 98, 32, 57, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 32, 57, 10, 10, 112, 104, 111, 110, 101,
			109, 101, 115, 32, 101, 110, 45, 119, 109, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 104, 32, 78, 85, 76, 76, 10, 114,
			101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 111, 64, 32, 79, 64, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 54, 10, 105, 110,
			116, 111, 110, 97, 116, 105, 111, 110, 32, 52, 10, 115, 116, 114, 101, 115, 115, 65, 100, 100, 32, 48, 32, 48, 32, 48, 32, 48, 32, 48,
			32, 48, 32, 48, 32, 50, 48, 10, 0};
		autoFileInMemory espeakdata_languages34 = FileInMemory_createWithData (188, reinterpret_cast<const char *> (&espeakdata_languages34_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/en-GB-x-gbcwmd", 
			U"en-GB-x-gbcwmd");
		me -> addItem_move (espeakdata_languages34.move());

		static unsigned char espeakdata_languages35_data[250] = {
			110, 97, 109, 101, 32, 69, 110, 103, 108, 105, 115, 104, 32, 40, 82, 101, 99, 101, 105, 118, 101, 100, 32, 80, 114, 111, 110, 117, 110, 99,
			105, 97, 116, 105, 111, 110, 41, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 103, 98, 45, 120, 45, 114, 112, 10, 108, 97,
			110, 103, 117, 97, 103, 101, 32, 101, 110, 45, 103, 98, 32, 32, 52, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 32, 53, 10,
			10, 109, 97, 105, 110, 116, 97, 105, 110, 101, 114, 32, 82, 101, 101, 99, 101, 32, 72, 46, 32, 68, 117, 110, 110, 32, 60, 109, 115, 99,
			108, 114, 104, 100, 64, 103, 109, 97, 105, 108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116, 117, 114, 101, 10,
			10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 101, 110, 45, 114, 112, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 32, 111,
			64, 32, 32, 79, 64, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 73, 32, 105, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48,
			51, 32, 73, 50, 32, 105, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 64, 32, 97, 35, 10, 114, 101, 112, 108, 97, 99, 101,
			32, 48, 51, 32, 51, 32, 97, 35, 10, 0};
		autoFileInMemory espeakdata_languages35 = FileInMemory_createWithData (249, reinterpret_cast<const char *> (&espeakdata_languages35_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/en-GB-x-rp", 
			U"en-GB-x-rp");
		me -> addItem_move (espeakdata_languages35.move());

		static unsigned char espeakdata_languages36_data[276] = {
			110, 97, 109, 101, 32, 69, 110, 103, 108, 105, 115, 104, 32, 40, 65, 109, 101, 114, 105, 99, 97, 41, 10, 108, 97, 110, 103, 117, 97, 103,
			101, 32, 101, 110, 45, 117, 115, 32, 50, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 110, 32, 51, 10, 10, 109, 97, 105, 110, 116,
			97, 105, 110, 101, 114, 32, 82, 101, 101, 99, 101, 32, 72, 46, 32, 68, 117, 110, 110, 32, 60, 109, 115, 99, 108, 114, 104, 100, 64, 103,
			109, 97, 105, 108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116, 117, 114, 101, 10, 10, 112, 104, 111, 110, 101,
			109, 101, 115, 32, 101, 110, 45, 117, 115, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 51, 32, 54, 10, 111, 112, 116, 105, 111, 110,
			32, 114, 101, 100, 117, 99, 101, 95, 116, 32, 49, 10, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 52, 48, 32,
			49, 50, 48, 32, 49, 57, 48, 32, 49, 55, 48, 32, 48, 32, 48, 32, 50, 53, 53, 32, 51, 48, 48, 10, 115, 116, 114, 101, 115, 115,
			65, 109, 112, 32, 32, 49, 55, 32, 49, 54, 32, 32, 49, 57, 32, 49, 57, 32, 32, 49, 57, 32, 49, 57, 32, 32, 50, 49, 32, 49,
			57, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32, 73, 32, 32, 105, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 51, 32,
			73, 50, 32, 105, 10, 0};
		autoFileInMemory espeakdata_languages36 = FileInMemory_createWithData (275, reinterpret_cast<const char *> (&espeakdata_languages36_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/en-US", 
			U"en-US");
		me -> addItem_move (espeakdata_languages36.move());

		static unsigned char espeakdata_languages37_data[24] = {
			110, 97, 109, 101, 32, 68, 117, 116, 99, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 110, 108, 10, 0};
		autoFileInMemory espeakdata_languages37 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeakdata_languages37_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/gmw/nl", 
			U"nl");
		me -> addItem_move (espeakdata_languages37.move());

		static unsigned char espeakdata_languages38_data[24] = {
			110, 97, 109, 101, 32, 71, 114, 101, 101, 107, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 108, 10, 0};
		autoFileInMemory espeakdata_languages38 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeakdata_languages38_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/grk/el", 
			U"el");
		me -> addItem_move (espeakdata_languages38.move());

		static unsigned char espeakdata_languages39_data[100] = {
			110, 97, 109, 101, 32, 71, 114, 101, 101, 107, 32, 40, 65, 110, 99, 105, 101, 110, 116, 41, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32,
			103, 114, 99, 10, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 55, 48, 32, 49, 55, 48, 32, 32, 49, 57, 48,
			32, 49, 57, 48, 32, 32, 48, 32, 48, 32, 32, 50, 51, 48, 32, 50, 52, 48, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49,
			10, 119, 111, 114, 100, 115, 32, 51, 10, 0};
		autoFileInMemory espeakdata_languages39 = FileInMemory_createWithData (99, reinterpret_cast<const char *> (&espeakdata_languages39_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/grk/grc", 
			U"grc");
		me -> addItem_move (espeakdata_languages39.move());

		static unsigned char espeakdata_languages40_data[43] = {
			110, 97, 109, 101, 32, 65, 115, 115, 97, 109, 101, 115, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 115, 10, 10, 115, 116, 97,
			116, 117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages40 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeakdata_languages40_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/as", 
			U"as");
		me -> addItem_move (espeakdata_languages40.move());

		static unsigned char espeakdata_languages41_data[26] = {
			110, 97, 109, 101, 32, 66, 101, 110, 103, 97, 108, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 98, 110, 10, 0};
		autoFileInMemory espeakdata_languages41 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeakdata_languages41_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/bn", 
			U"bn");
		me -> addItem_move (espeakdata_languages41.move());

		static unsigned char espeakdata_languages42_data[40] = {
			110, 97, 109, 101, 32, 66, 105, 115, 104, 110, 117, 112, 114, 105, 121, 97, 32, 77, 97, 110, 105, 112, 117, 114, 105, 10, 108, 97, 110, 103,
			117, 97, 103, 101, 32, 98, 112, 121, 10, 0};
		autoFileInMemory espeakdata_languages42 = FileInMemory_createWithData (39, reinterpret_cast<const char *> (&espeakdata_languages42_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/bpy", 
			U"bpy");
		me -> addItem_move (espeakdata_languages42.move());

		static unsigned char espeakdata_languages43_data[43] = {
			110, 97, 109, 101, 32, 71, 117, 106, 97, 114, 97, 116, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 103, 117, 10, 10, 115, 116, 97,
			116, 117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages43 = FileInMemory_createWithData (42, reinterpret_cast<const char *> (&espeakdata_languages43_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/gu", 
			U"gu");
		me -> addItem_move (espeakdata_languages43.move());

		static unsigned char espeakdata_languages44_data[24] = {
			110, 97, 109, 101, 32, 72, 105, 110, 100, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 105, 10, 0};
		autoFileInMemory espeakdata_languages44 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeakdata_languages44_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/hi", 
			U"hi");
		me -> addItem_move (espeakdata_languages44.move());

		static unsigned char espeakdata_languages45_data[27] = {
			110, 97, 109, 101, 32, 75, 111, 110, 107, 97, 110, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 111, 107, 10, 0};
		autoFileInMemory espeakdata_languages45 = FileInMemory_createWithData (26, reinterpret_cast<const char *> (&espeakdata_languages45_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/kok", 
			U"kok");
		me -> addItem_move (espeakdata_languages45.move());

		static unsigned char espeakdata_languages46_data[42] = {
			110, 97, 109, 101, 32, 77, 97, 114, 97, 116, 104, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 109, 114, 10, 10, 115, 116, 97, 116,
			117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages46 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeakdata_languages46_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/mr", 
			U"mr");
		me -> addItem_move (espeakdata_languages46.move());

		static unsigned char espeakdata_languages47_data[38] = {
			110, 97, 109, 101, 32, 78, 101, 112, 97, 108, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 110, 101, 10, 10, 100, 105, 99, 116, 114,
			117, 108, 101, 115, 32, 49, 10, 0};
		autoFileInMemory espeakdata_languages47 = FileInMemory_createWithData (37, reinterpret_cast<const char *> (&espeakdata_languages47_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/ne", 
			U"ne");
		me -> addItem_move (espeakdata_languages47.move());

		static unsigned char espeakdata_languages48_data[40] = {
			110, 97, 109, 101, 32, 79, 114, 105, 121, 97, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 111, 114, 10, 10, 115, 116, 97, 116, 117, 115,
			32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages48 = FileInMemory_createWithData (39, reinterpret_cast<const char *> (&espeakdata_languages48_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/or", 
			U"or");
		me -> addItem_move (espeakdata_languages48.move());

		static unsigned char espeakdata_languages49_data[26] = {
			110, 97, 109, 101, 32, 80, 117, 110, 106, 97, 98, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 112, 97, 10, 0};
		autoFileInMemory espeakdata_languages49 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeakdata_languages49_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/pa", 
			U"pa");
		me -> addItem_move (espeakdata_languages49.move());

		static unsigned char espeakdata_languages50_data[67] = {
			110, 97, 109, 101, 32, 83, 105, 110, 100, 104, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 100, 10, 109, 97, 105, 110, 116, 97,
			105, 110, 101, 114, 32, 69, 106, 97, 122, 32, 83, 104, 97, 104, 32, 60, 101, 97, 115, 104, 97, 104, 54, 55, 64, 103, 109, 97, 105, 108,
			46, 99, 111, 109, 62, 10, 0};
		autoFileInMemory espeakdata_languages50 = FileInMemory_createWithData (66, reinterpret_cast<const char *> (&espeakdata_languages50_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/sd", 
			U"sd");
		me -> addItem_move (espeakdata_languages50.move());

		static unsigned char espeakdata_languages51_data[56] = {
			110, 97, 109, 101, 32, 83, 105, 110, 104, 97, 108, 97, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 105, 10, 10, 115, 116, 97, 116,
			117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 0};
		autoFileInMemory espeakdata_languages51 = FileInMemory_createWithData (55, reinterpret_cast<const char *> (&espeakdata_languages51_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/si", 
			U"si");
		me -> addItem_move (espeakdata_languages51.move());

		static unsigned char espeakdata_languages52_data[95] = {
			110, 97, 109, 101, 32, 85, 114, 100, 117, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 117, 114, 10, 109, 97, 105, 110, 116, 97, 105, 110,
			101, 114, 32, 69, 106, 97, 122, 32, 83, 104, 97, 104, 32, 60, 101, 97, 115, 104, 97, 104, 54, 55, 64, 103, 109, 97, 105, 108, 46, 99,
			111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 10, 115, 116, 114, 101, 115, 115, 114, 117, 108, 101,
			32, 54, 10, 10, 0};
		autoFileInMemory espeakdata_languages52 = FileInMemory_createWithData (94, reinterpret_cast<const char *> (&espeakdata_languages52_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/inc/ur", 
			U"ur");
		me -> addItem_move (espeakdata_languages52.move());

		static unsigned char espeakdata_languages53_data[62] = {
			110, 97, 109, 101, 32, 65, 114, 109, 101, 110, 105, 97, 110, 32, 40, 69, 97, 115, 116, 32, 65, 114, 109, 101, 110, 105, 97, 41, 10, 108,
			97, 110, 103, 117, 97, 103, 101, 32, 104, 121, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 121, 45, 97, 114, 101, 118, 101, 108, 97,
			10, 0};
		autoFileInMemory espeakdata_languages53 = FileInMemory_createWithData (61, reinterpret_cast<const char *> (&espeakdata_languages53_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/ine/hy", 
			U"hy");
		me -> addItem_move (espeakdata_languages53.move());

		static unsigned char espeakdata_languages54_data[326] = {
			110, 97, 109, 101, 32, 65, 114, 109, 101, 110, 105, 97, 110, 32, 40, 87, 101, 115, 116, 32, 65, 114, 109, 101, 110, 105, 97, 41, 10, 108,
			97, 110, 103, 117, 97, 103, 101, 32, 104, 121, 45, 97, 114, 101, 118, 109, 100, 97, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 121,
			32, 32, 56, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 10, 47, 47, 32, 99, 104, 97, 110, 103, 101, 32, 99, 111,
			110, 115, 111, 110, 97, 110, 116, 115, 32, 102, 111, 114, 32, 87, 101, 115, 116, 32, 65, 114, 109, 101, 110, 105, 97, 110, 32, 112, 114, 111,
			110, 117, 110, 99, 105, 97, 116, 105, 111, 110, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 98, 32, 32, 112, 35, 10, 114, 101,
			112, 108, 97, 99, 101, 32, 48, 48, 32, 100, 32, 32, 116, 35, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 100, 122, 32, 116,
			115, 35, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 100, 90, 32, 116, 83, 35, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48,
			48, 32, 103, 32, 32, 107, 35, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 112, 32, 32, 98, 10, 114, 101, 112, 108, 97,
			99, 101, 32, 48, 48, 32, 116, 32, 32, 100, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 116, 115, 32, 100, 122, 10, 114, 101,
			112, 108, 97, 99, 101, 32, 48, 48, 32, 116, 83, 32, 100, 90, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 107, 32, 32, 103,
			10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 82, 50, 32, 82, 32, 32, 47, 47, 32, 63, 63, 10, 0};
		autoFileInMemory espeakdata_languages54 = FileInMemory_createWithData (325, reinterpret_cast<const char *> (&espeakdata_languages54_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/ine/hy-arevmda", 
			U"hy-arevmda");
		me -> addItem_move (espeakdata_languages54.move());

		static unsigned char espeakdata_languages55_data[104] = {
			110, 97, 109, 101, 32, 65, 108, 98, 97, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 113, 10, 10, 47, 47, 32,
			97, 100, 100, 32, 116, 104, 105, 115, 32, 108, 105, 110, 101, 32, 116, 111, 32, 114, 101, 109, 111, 118, 101, 32, 39, 195, 171, 39, 32, 97,
			116, 32, 116, 104, 101, 32, 101, 110, 100, 32, 111, 102, 32, 119, 111, 114, 100, 115, 10, 47, 47, 32, 114, 101, 112, 108, 97, 99, 101, 32,
			48, 48, 32, 32, 64, 47, 32, 32, 78, 85, 76, 76, 10, 0};
		autoFileInMemory espeakdata_languages55 = FileInMemory_createWithData (103, reinterpret_cast<const char *> (&espeakdata_languages55_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/ine/sq", 
			U"sq");
		me -> addItem_move (espeakdata_languages55.move());

		static unsigned char espeakdata_languages56_data[91] = {
			110, 97, 109, 101, 32, 80, 101, 114, 115, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 97, 10, 109, 97, 105, 110, 116,
			97, 105, 110, 101, 114, 32, 83, 104, 97, 100, 121, 97, 114, 32, 75, 104, 111, 100, 97, 121, 97, 114, 105, 32, 60, 115, 104, 97, 100, 121,
			97, 114, 56, 49, 64, 103, 109, 97, 105, 108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116, 117, 114, 101, 10,
			0};
		autoFileInMemory espeakdata_languages56 = FileInMemory_createWithData (90, reinterpret_cast<const char *> (&espeakdata_languages56_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/ira/fa", 
			U"fa");
		me -> addItem_move (espeakdata_languages56.move());

		static unsigned char espeakdata_languages57_data[270] = {
			110, 97, 109, 101, 32, 80, 101, 114, 115, 105, 97, 110, 32, 40, 80, 105, 110, 103, 108, 105, 115, 104, 41, 10, 47, 47, 32, 83, 111, 109,
			101, 116, 105, 109, 101, 115, 44, 32, 70, 97, 114, 115, 105, 32, 115, 112, 101, 97, 107, 101, 114, 115, 32, 119, 114, 105, 116, 101, 32, 70,
			97, 114, 115, 105, 32, 119, 111, 114, 100, 115, 32, 117, 115, 105, 110, 103, 32, 69, 110, 103, 108, 105, 115, 104, 32, 99, 104, 97, 114, 97,
			99, 116, 101, 114, 115, 44, 32, 112, 97, 114, 116, 105, 99, 117, 108, 97, 114, 108, 121, 32, 105, 110, 32, 67, 104, 97, 116, 32, 97, 110,
			100, 32, 83, 77, 83, 32, 40, 116, 101, 120, 116, 101, 32, 109, 101, 115, 115, 97, 103, 101, 115, 41, 46, 41, 44, 32, 99, 97, 108, 108,
			101, 100, 32, 80, 105, 110, 103, 108, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 97, 45, 76, 97, 116, 110, 10, 109,
			97, 105, 110, 116, 97, 105, 110, 101, 114, 32, 83, 104, 97, 100, 121, 97, 114, 32, 75, 104, 111, 100, 97, 121, 97, 114, 105, 32, 60, 115,
			104, 97, 100, 121, 97, 114, 56, 49, 64, 103, 109, 97, 105, 108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116,
			117, 114, 101, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 102, 97, 10, 10, 0};
		autoFileInMemory espeakdata_languages57 = FileInMemory_createWithData (269, reinterpret_cast<const char *> (&espeakdata_languages57_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/ira/fa-Latn", 
			U"fa-Latn");
		me -> addItem_move (espeakdata_languages57.move());

		static unsigned char espeakdata_languages58_data[41] = {
			110, 97, 109, 101, 32, 75, 117, 114, 100, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 117, 10, 10, 47, 47, 119, 111,
			114, 100, 115, 32, 49, 32, 52, 56, 10, 10, 0};
		autoFileInMemory espeakdata_languages58 = FileInMemory_createWithData (40, reinterpret_cast<const char *> (&espeakdata_languages58_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/ira/ku", 
			U"ku");
		me -> addItem_move (espeakdata_languages58.move());

		static unsigned char espeakdata_languages59_data[287] = {
			110, 97, 109, 101, 32, 76, 97, 116, 105, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 108, 97, 10, 115, 116, 114, 101, 115, 115, 114,
			117, 108, 101, 32, 50, 32, 51, 51, 32, 48, 32, 50, 10, 47, 47, 32, 114, 117, 108, 101, 61, 112, 101, 110, 117, 108, 116, 105, 109, 97,
			116, 101, 10, 47, 47, 32, 102, 108, 97, 103, 115, 61, 48, 49, 48, 48, 48, 48, 49, 32, 40, 110, 111, 32, 97, 117, 116, 111, 109, 97,
			116, 105, 99, 32, 115, 101, 99, 111, 110, 100, 97, 114, 121, 32, 115, 116, 114, 101, 115, 115, 32, 43, 32, 100, 111, 110, 39, 116, 32, 115,
			116, 114, 101, 115, 32, 109, 111, 110, 111, 115, 121, 108, 108, 97, 98, 108, 101, 115, 41, 10, 47, 47, 32, 117, 110, 115, 116, 114, 101, 115,
			115, 101, 100, 95, 119, 100, 49, 61, 48, 10, 47, 47, 32, 117, 110, 115, 116, 114, 101, 115, 115, 101, 100, 95, 119, 100, 50, 61, 50, 10,
			10, 47, 47, 32, 115, 104, 111, 114, 116, 32, 103, 97, 112, 32, 98, 101, 116, 119, 101, 101, 110, 32, 119, 111, 114, 100, 115, 10, 119, 111,
			114, 100, 115, 32, 50, 10, 10, 47, 47, 32, 78, 111, 116, 101, 58, 32, 84, 104, 101, 32, 76, 97, 116, 105, 110, 32, 118, 111, 105, 99,
			101, 32, 110, 101, 101, 100, 115, 32, 108, 111, 110, 103, 32, 118, 111, 119, 101, 108, 115, 32, 116, 111, 32, 98, 101, 32, 109, 97, 114, 107,
			101, 100, 32, 119, 105, 116, 104, 32, 109, 97, 99, 114, 111, 110, 115, 10, 0};
		autoFileInMemory espeakdata_languages59 = FileInMemory_createWithData (286, reinterpret_cast<const char *> (&espeakdata_languages59_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/itc/la", 
			U"la");
		me -> addItem_move (espeakdata_languages59.move());

		static unsigned char espeakdata_languages60_data[53] = {
			110, 97, 109, 101, 32, 74, 97, 112, 97, 110, 101, 115, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 106, 97, 10, 112, 104, 111, 110,
			101, 109, 101, 115, 32, 106, 97, 10, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 52, 10, 0};
		autoFileInMemory espeakdata_languages60 = FileInMemory_createWithData (52, reinterpret_cast<const char *> (&espeakdata_languages60_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/jpx/ja", 
			U"ja");
		me -> addItem_move (espeakdata_languages60.move());

		static unsigned char espeakdata_languages61_data[52] = {
			110, 97, 109, 101, 32, 75, 111, 114, 101, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 111, 10, 112, 105, 116, 99, 104, 32,
			56, 48, 32, 49, 49, 56, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 10, 0};
		autoFileInMemory espeakdata_languages61 = FileInMemory_createWithData (51, reinterpret_cast<const char *> (&espeakdata_languages61_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/ko", 
			U"ko");
		me -> addItem_move (espeakdata_languages61.move());

		static unsigned char espeakdata_languages62_data[135] = {
			110, 97, 109, 101, 32, 73, 110, 100, 111, 110, 101, 115, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 105, 100, 10, 10, 115,
			116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 54, 48, 32, 50, 48, 48, 32, 32, 49, 56, 48, 32, 49, 56, 48, 32, 32,
			48, 32, 48, 32, 32, 50, 50, 48, 32, 50, 52, 48, 10, 115, 116, 114, 101, 115, 115, 65, 109, 112, 32, 32, 32, 32, 49, 54, 32, 32,
			49, 56, 32, 32, 32, 49, 56, 32, 32, 49, 56, 32, 32, 32, 48, 32, 48, 32, 32, 50, 50, 32, 32, 50, 49, 10, 10, 99, 111, 110,
			115, 111, 110, 97, 110, 116, 115, 32, 56, 48, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_languages62 = FileInMemory_createWithData (134, reinterpret_cast<const char *> (&espeakdata_languages62_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/poz/id", 
			U"id");
		me -> addItem_move (espeakdata_languages62.move());

		static unsigned char espeakdata_languages63_data[356] = {
			108, 97, 110, 103, 117, 97, 103, 101, 32, 109, 105, 10, 115, 116, 97, 116, 117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 10, 47, 47,
			32, 104, 116, 116, 112, 115, 58, 47, 47, 103, 105, 116, 104, 117, 98, 46, 99, 111, 109, 47, 101, 115, 112, 101, 97, 107, 45, 110, 103, 47,
			101, 115, 112, 101, 97, 107, 45, 110, 103, 47, 98, 108, 111, 98, 47, 109, 97, 115, 116, 101, 114, 47, 100, 111, 99, 115, 47, 118, 111, 105,
			99, 101, 115, 46, 109, 100, 35, 119, 111, 114, 100, 115, 10, 119, 111, 114, 100, 115, 32, 49, 32, 50, 10, 10, 47, 47, 32, 116, 97, 107,
			101, 110, 32, 102, 114, 111, 109, 32, 74, 97, 99, 107, 121, 10, 112, 105, 116, 99, 104, 32, 32, 49, 49, 53, 32, 49, 51, 48, 10, 10,
			102, 111, 114, 109, 97, 110, 116, 32, 48, 32, 49, 53, 48, 32, 49, 53, 53, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32,
			49, 32, 57, 48, 32, 49, 53, 53, 32, 55, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 50, 32, 57, 53, 32, 55, 48, 32, 54, 52,
			10, 102, 111, 114, 109, 97, 110, 116, 32, 51, 32, 49, 53, 32, 50, 48, 32, 51, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 52, 32,
			50, 48, 32, 51, 48, 32, 52, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 53, 32, 54, 53, 32, 50, 48, 32, 54, 53, 10, 102, 111,
			114, 109, 97, 110, 116, 32, 54, 32, 55, 48, 32, 56, 48, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 55, 32, 50, 48,
			32, 56, 48, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 56, 32, 49, 48, 48, 32, 57, 53, 32, 56, 48, 10, 118, 111,
			105, 99, 105, 110, 103, 32, 49, 51, 53, 10, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115, 32, 49, 49, 48, 10, 0};
		autoFileInMemory espeakdata_languages63 = FileInMemory_createWithData (355, reinterpret_cast<const char *> (&espeakdata_languages63_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/poz/mi", 
			U"mi");
		me -> addItem_move (espeakdata_languages63.move());

		static unsigned char espeakdata_languages64_data[446] = {
			47, 47, 32, 76, 97, 115, 116, 32, 117, 112, 100, 97, 116, 101, 100, 58, 32, 49, 52, 32, 79, 99, 116, 111, 98, 101, 114, 32, 50, 48,
			49, 48, 44, 32, 74, 97, 115, 111, 110, 32, 79, 110, 103, 32, 40, 106, 97, 115, 111, 110, 64, 112, 111, 114, 116, 97, 108, 103, 114, 111,
			111, 118, 101, 46, 99, 111, 109, 41, 10, 110, 97, 109, 101, 32, 77, 97, 108, 97, 121, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 109,
			115, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 105, 100, 10, 10, 116, 114, 97, 110, 115, 108, 97, 116, 111, 114, 32, 105, 100, 10, 10,
			115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 54, 48, 32, 50, 48, 48, 32, 32, 49, 56, 48, 32, 49, 56, 48, 32,
			32, 48, 32, 48, 32, 32, 50, 50, 48, 32, 50, 52, 48, 10, 115, 116, 114, 101, 115, 115, 65, 109, 112, 32, 32, 32, 32, 49, 54, 32,
			32, 49, 56, 32, 32, 32, 49, 56, 32, 32, 49, 56, 32, 32, 32, 48, 32, 48, 32, 32, 50, 50, 32, 32, 50, 49, 10, 105, 110, 116,
			111, 110, 97, 116, 105, 111, 110, 9, 51, 9, 47, 47, 32, 76, 101, 115, 115, 32, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 44, 32,
			97, 110, 100, 32, 99, 111, 109, 109, 97, 32, 100, 111, 101, 115, 32, 110, 111, 116, 32, 114, 97, 105, 115, 101, 32, 116, 104, 101, 32, 112,
			105, 116, 99, 104, 46, 10, 10, 47, 47, 32, 78, 117, 97, 110, 99, 101, 32, 45, 32, 80, 101, 110, 105, 110, 115, 117, 108, 97, 32, 77,
			97, 108, 97, 121, 115, 105, 97, 10, 47, 47, 32, 114, 101, 112, 108, 97, 99, 101, 9, 51, 32, 97, 9, 64, 9, 47, 47, 32, 99, 104,
			97, 110, 103, 101, 32, 39, 115, 97, 121, 97, 39, 32, 116, 111, 32, 39, 115, 97, 121, 101, 39, 10, 9, 9, 9, 9, 47, 47, 32, 40,
			111, 110, 108, 121, 32, 116, 104, 101, 32, 108, 97, 115, 116, 32, 112, 104, 111, 110, 101, 109, 101, 32, 111, 102, 32, 97, 32, 119, 111, 114,
			100, 44, 32, 111, 110, 108, 121, 32, 105, 110, 32, 117, 110, 115, 116, 114, 101, 115, 115, 101, 100, 32, 115, 121, 108, 108, 97, 98, 108, 101,
			115, 41, 10, 9, 9, 9, 9, 10, 99, 111, 110, 115, 111, 110, 97, 110, 116, 115, 32, 56, 48, 32, 56, 48, 10, 0};
		autoFileInMemory espeakdata_languages64 = FileInMemory_createWithData (445, reinterpret_cast<const char *> (&espeakdata_languages64_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/poz/ms", 
			U"ms");
		me -> addItem_move (espeakdata_languages64.move());

		static unsigned char espeakdata_languages65_data[28] = {
			110, 97, 109, 101, 32, 65, 114, 97, 103, 111, 110, 101, 115, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 110, 10, 0};
		autoFileInMemory espeakdata_languages65 = FileInMemory_createWithData (27, reinterpret_cast<const char *> (&espeakdata_languages65_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/an", 
			U"an");
		me -> addItem_move (espeakdata_languages65.move());

		static unsigned char espeakdata_languages66_data[26] = {
			110, 97, 109, 101, 32, 67, 97, 116, 97, 108, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 99, 97, 10, 0};
		autoFileInMemory espeakdata_languages66 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeakdata_languages66_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/ca", 
			U"ca");
		me -> addItem_move (espeakdata_languages66.move());

		static unsigned char espeakdata_languages67_data[64] = {
			110, 97, 109, 101, 32, 83, 112, 97, 110, 105, 115, 104, 32, 40, 83, 112, 97, 105, 110, 41, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32,
			101, 115, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 116, 117, 110, 101, 115, 32, 115, 54, 32, 99, 54, 32, 113, 54, 32,
			101, 54, 10, 0};
		autoFileInMemory espeakdata_languages67 = FileInMemory_createWithData (63, reinterpret_cast<const char *> (&espeakdata_languages67_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/es", 
			U"es");
		me -> addItem_move (espeakdata_languages67.move());

		static unsigned char espeakdata_languages68_data[178] = {
			110, 97, 109, 101, 32, 83, 112, 97, 110, 105, 115, 104, 32, 40, 76, 97, 116, 105, 110, 32, 65, 109, 101, 114, 105, 99, 97, 41, 10, 108,
			97, 110, 103, 117, 97, 103, 101, 32, 101, 115, 45, 52, 49, 57, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 115, 45, 109, 120, 32,
			54, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 115, 32, 54, 10, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 101, 115, 45, 108,
			97, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 50, 10, 105, 110, 116, 111, 110, 97, 116, 105, 111, 110, 32, 50, 10, 115, 116, 114,
			101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 55, 48, 32, 50, 48, 48, 32, 32, 50, 51, 48, 32, 49, 56, 48, 32, 32, 48, 32,
			48, 32, 32, 50, 53, 48, 32, 50, 56, 48, 10, 10, 114, 101, 112, 108, 97, 99, 101, 32, 48, 48, 32, 84, 32, 115, 10, 0};
		autoFileInMemory espeakdata_languages68 = FileInMemory_createWithData (177, reinterpret_cast<const char *> (&espeakdata_languages68_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/es-419", 
			U"es-419");
		me -> addItem_move (espeakdata_languages68.move());

		static unsigned char espeakdata_languages69_data[80] = {
			110, 97, 109, 101, 32, 70, 114, 101, 110, 99, 104, 32, 40, 70, 114, 97, 110, 99, 101, 41, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32,
			102, 114, 45, 102, 114, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 114, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49,
			10, 116, 117, 110, 101, 115, 32, 115, 51, 32, 99, 51, 32, 113, 51, 32, 101, 51, 10, 0};
		autoFileInMemory espeakdata_languages69 = FileInMemory_createWithData (79, reinterpret_cast<const char *> (&espeakdata_languages69_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/fr", 
			U"fr");
		me -> addItem_move (espeakdata_languages69.move());

		static unsigned char espeakdata_languages70_data[85] = {
			110, 97, 109, 101, 32, 70, 114, 101, 110, 99, 104, 32, 40, 66, 101, 108, 103, 105, 117, 109, 41, 10, 108, 97, 110, 103, 117, 97, 103, 101,
			32, 102, 114, 45, 98, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 114, 32, 56, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101,
			115, 32, 50, 10, 116, 117, 110, 101, 115, 32, 115, 51, 32, 99, 51, 32, 113, 51, 32, 101, 51, 10, 10, 10, 0};
		autoFileInMemory espeakdata_languages70 = FileInMemory_createWithData (84, reinterpret_cast<const char *> (&espeakdata_languages70_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/fr-BE", 
			U"fr-BE");
		me -> addItem_move (espeakdata_languages70.move());

		static unsigned char espeakdata_languages71_data[87] = {
			110, 97, 109, 101, 32, 70, 114, 101, 110, 99, 104, 32, 40, 83, 119, 105, 116, 122, 101, 114, 108, 97, 110, 100, 41, 10, 108, 97, 110, 103,
			117, 97, 103, 101, 32, 102, 114, 45, 99, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 114, 32, 56, 10, 10, 100, 105, 99, 116,
			114, 117, 108, 101, 115, 32, 51, 10, 116, 117, 110, 101, 115, 32, 115, 51, 32, 99, 51, 32, 113, 51, 32, 101, 51, 10, 0};
		autoFileInMemory espeakdata_languages71 = FileInMemory_createWithData (86, reinterpret_cast<const char *> (&espeakdata_languages71_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/fr-CH", 
			U"fr-CH");
		me -> addItem_move (espeakdata_languages71.move());

		static unsigned char espeakdata_languages72_data[110] = {
			110, 97, 109, 101, 32, 73, 116, 97, 108, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 105, 116, 10, 10, 109, 97, 105, 110,
			116, 97, 105, 110, 101, 114, 32, 67, 104, 114, 105, 115, 116, 105, 97, 110, 32, 76, 101, 111, 32, 77, 32, 60, 108, 108, 97, 106, 116, 97,
			50, 48, 49, 50, 64, 103, 109, 97, 105, 108, 46, 99, 111, 109, 62, 10, 115, 116, 97, 116, 117, 115, 32, 109, 97, 116, 117, 114, 101, 10,
			10, 116, 117, 110, 101, 115, 32, 115, 52, 32, 99, 52, 32, 113, 52, 32, 101, 52, 10, 0};
		autoFileInMemory espeakdata_languages72 = FileInMemory_createWithData (109, reinterpret_cast<const char *> (&espeakdata_languages72_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/it", 
			U"it");
		me -> addItem_move (espeakdata_languages72.move());

		static unsigned char espeakdata_languages73_data[63] = {
			110, 97, 109, 101, 32, 80, 97, 112, 105, 97, 109, 101, 110, 116, 111, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 112, 97, 112, 10, 10,
			115, 116, 97, 116, 117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 98, 97, 115, 101, 50,
			10, 10, 0};
		autoFileInMemory espeakdata_languages73 = FileInMemory_createWithData (62, reinterpret_cast<const char *> (&espeakdata_languages73_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/pap", 
			U"pap");
		me -> addItem_move (espeakdata_languages73.move());

		static unsigned char espeakdata_languages74_data[96] = {
			110, 97, 109, 101, 32, 80, 111, 114, 116, 117, 103, 117, 101, 115, 101, 32, 40, 80, 111, 114, 116, 117, 103, 97, 108, 41, 10, 108, 97, 110,
			103, 117, 97, 103, 101, 32, 112, 116, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 112, 116, 45, 112, 116, 10, 112, 104, 111, 110, 101, 109,
			101, 115, 32, 112, 116, 45, 112, 116, 10, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 105, 110, 116, 111, 110, 97, 116, 105,
			111, 110, 32, 50, 10, 0};
		autoFileInMemory espeakdata_languages74 = FileInMemory_createWithData (95, reinterpret_cast<const char *> (&espeakdata_languages74_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/pt", 
			U"pt");
		me -> addItem_move (espeakdata_languages74.move());

		static unsigned char espeakdata_languages75_data[110] = {
			110, 97, 109, 101, 32, 80, 111, 114, 116, 117, 103, 117, 101, 115, 101, 32, 40, 66, 114, 97, 122, 105, 108, 41, 10, 108, 97, 110, 103, 117,
			97, 103, 101, 32, 112, 116, 45, 98, 114, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 112, 116, 32, 54, 10, 10, 100, 105, 99, 116, 114,
			117, 108, 101, 115, 32, 50, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 50, 48, 48, 32, 49, 49, 53, 32, 50, 51,
			48, 32, 50, 51, 48, 32, 48, 32, 48, 32, 50, 53, 48, 32, 50, 55, 48, 10, 10, 0};
		autoFileInMemory espeakdata_languages75 = FileInMemory_createWithData (109, reinterpret_cast<const char *> (&espeakdata_languages75_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/pt-BR", 
			U"pt-BR");
		me -> addItem_move (espeakdata_languages75.move());

		static unsigned char espeakdata_languages76_data[27] = {
			110, 97, 109, 101, 32, 82, 111, 109, 97, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 114, 111, 10, 0};
		autoFileInMemory espeakdata_languages76 = FileInMemory_createWithData (26, reinterpret_cast<const char *> (&espeakdata_languages76_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/roa/ro", 
			U"ro");
		me -> addItem_move (espeakdata_languages76.move());

		static unsigned char espeakdata_languages77_data[48] = {
			110, 97, 109, 101, 32, 71, 117, 97, 114, 97, 110, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 103, 110, 10, 100, 105, 99, 116, 114,
			117, 108, 101, 115, 32, 49, 10, 119, 111, 114, 100, 115, 32, 48, 32, 49, 10, 0};
		autoFileInMemory espeakdata_languages77 = FileInMemory_createWithData (47, reinterpret_cast<const char *> (&espeakdata_languages77_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/sai/gn", 
			U"gn");
		me -> addItem_move (espeakdata_languages77.move());

		static unsigned char espeakdata_languages78_data[42] = {
			110, 97, 109, 101, 32, 65, 109, 104, 97, 114, 105, 99, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 109, 10, 10, 115, 116, 97, 116,
			117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages78 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeakdata_languages78_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/sem/am", 
			U"am");
		me -> addItem_move (espeakdata_languages78.move());

		static unsigned char espeakdata_languages79_data[348] = {
			110, 97, 109, 101, 32, 65, 114, 97, 98, 105, 99, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 114, 10, 112, 104, 111, 110, 101, 109,
			101, 115, 32, 97, 114, 10, 112, 105, 116, 99, 104, 32, 49, 54, 48, 32, 49, 50, 48, 10, 114, 111, 117, 103, 104, 110, 101, 115, 115, 32,
			48, 10, 115, 112, 101, 101, 100, 32, 55, 48, 10, 10, 102, 111, 114, 109, 97, 110, 116, 32, 48, 32, 49, 48, 53, 32, 56, 48, 32, 49,
			53, 48, 32, 10, 102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 49, 49, 48, 32, 56, 48, 32, 49, 54, 48, 32, 10, 102, 111, 114, 109,
			97, 110, 116, 32, 50, 32, 49, 49, 48, 32, 55, 48, 32, 49, 53, 48, 32, 10, 102, 111, 114, 109, 97, 110, 116, 32, 51, 32, 49, 49,
			48, 32, 55, 48, 32, 49, 53, 48, 32, 10, 102, 111, 114, 109, 97, 110, 116, 32, 52, 32, 49, 49, 53, 32, 56, 48, 32, 50, 48, 48,
			32, 10, 102, 111, 114, 109, 97, 110, 116, 32, 53, 32, 49, 49, 53, 32, 56, 48, 32, 49, 48, 48, 32, 10, 102, 111, 114, 109, 97, 110,
			116, 32, 54, 32, 49, 49, 48, 32, 55, 48, 32, 49, 53, 48, 32, 10, 102, 111, 114, 109, 97, 110, 116, 32, 55, 32, 49, 49, 48, 32,
			55, 48, 32, 49, 48, 48, 32, 10, 102, 111, 114, 109, 97, 110, 116, 32, 56, 32, 49, 49, 48, 32, 55, 48, 32, 49, 53, 48, 32, 10,
			10, 115, 116, 114, 101, 115, 115, 65, 100, 100, 32, 48, 32, 48, 32, 45, 49, 48, 32, 45, 49, 48, 32, 48, 32, 48, 32, 49, 48, 32,
			52, 48, 32, 10, 101, 99, 104, 111, 32, 50, 48, 32, 49, 48, 32, 10, 118, 111, 105, 99, 105, 110, 103, 32, 55, 53, 32, 10, 99, 111,
			110, 115, 111, 110, 97, 110, 116, 115, 32, 49, 53, 48, 32, 49, 53, 48, 10, 0};
		autoFileInMemory espeakdata_languages79 = FileInMemory_createWithData (347, reinterpret_cast<const char *> (&espeakdata_languages79_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/sem/ar", 
			U"ar");
		me -> addItem_move (espeakdata_languages79.move());

		static unsigned char espeakdata_languages80_data[42] = {
			110, 97, 109, 101, 32, 77, 97, 108, 116, 101, 115, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 109, 116, 10, 10, 115, 116, 97, 116,
			117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages80 = FileInMemory_createWithData (41, reinterpret_cast<const char *> (&espeakdata_languages80_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/sem/mt", 
			U"mt");
		me -> addItem_move (espeakdata_languages80.move());

		static unsigned char espeakdata_languages81_data[666] = {
			110, 97, 109, 101, 32, 67, 104, 105, 110, 101, 115, 101, 32, 40, 77, 97, 110, 100, 97, 114, 105, 110, 41, 10, 108, 97, 110, 103, 117, 97,
			103, 101, 32, 99, 109, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 122, 104, 45, 99, 109, 110, 10, 108, 97, 110, 103, 117, 97, 103,
			101, 32, 122, 104, 10, 112, 104, 111, 110, 101, 109, 101, 115, 32, 122, 104, 10, 100, 105, 99, 116, 105, 111, 110, 97, 114, 121, 32, 122, 104,
			10, 119, 111, 114, 100, 115, 32, 49, 10, 112, 105, 116, 99, 104, 32, 56, 48, 32, 49, 49, 56, 10, 10, 100, 105, 99, 116, 95, 109, 105,
			110, 32, 49, 48, 48, 48, 48, 48, 10, 10, 47, 47, 102, 111, 114, 32, 115, 111, 109, 101, 32, 100, 105, 97, 108, 101, 99, 116, 115, 10,
			10, 47, 47, 91, 101, 110, 93, 58, 32, 114, 101, 112, 108, 97, 99, 101, 32, 110, 103, 32, 119, 105, 116, 104, 32, 110, 10, 47, 47, 91,
			122, 104, 93, 58, 32, 239, 191, 189, 222, 186, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191,
			189, 110, 103, 239, 191, 189, 239, 191, 189, 239, 191, 189, 110, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 78, 32, 110, 10,
			10, 47, 47, 91, 101, 110, 93, 58, 32, 114, 101, 112, 108, 97, 99, 101, 32, 114, 102, 120, 32, 99, 111, 110, 115, 111, 110, 97, 110, 116,
			115, 10, 47, 47, 91, 122, 104, 93, 58, 32, 239, 191, 189, 222, 190, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191, 189,
			239, 191, 189, 239, 191, 189, 114, 239, 191, 189, 239, 191, 189, 239, 191, 189, 108, 239, 191, 189, 239, 191, 189, 122, 239, 191, 189, 239, 191, 189,
			101, 114, 239, 191, 189, 239, 191, 189, 239, 191, 189, 101, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 116, 115, 46, 104, 32,
			116, 115, 104, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 116, 115, 46, 32, 116, 115, 10, 47, 47, 114, 101, 112, 108, 97,
			99, 101, 32, 48, 32, 115, 46, 32, 115, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 105, 46, 32, 105, 91, 10, 47, 47,
			114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 122, 46, 32, 108, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 122, 46, 32,
			122, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32, 64, 114, 32, 64, 10, 10, 47, 47, 91, 101, 110, 93, 58, 32, 114, 101,
			112, 108, 97, 99, 101, 32, 98, 101, 103, 105, 110, 110, 105, 110, 103, 32, 110, 32, 111, 114, 32, 108, 10, 47, 47, 91, 122, 104, 93, 58,
			32, 239, 191, 189, 239, 191, 189, 239, 191, 189, 239, 191, 189, 110, 108, 239, 191, 189, 239, 191, 189, 110, 239, 191, 189, 239, 191, 189, 239, 191,
			189, 108, 239, 191, 189, 239, 191, 189, 108, 239, 191, 189, 239, 191, 189, 239, 191, 189, 110, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32,
			50, 32, 110, 32, 108, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 50, 32, 108, 32, 110, 10, 10, 47, 47, 91, 101, 110, 93, 58,
			32, 114, 101, 112, 108, 97, 99, 101, 32, 98, 101, 103, 105, 110, 110, 105, 110, 103, 32, 119, 32, 119, 105, 116, 104, 32, 118, 10, 47, 47,
			91, 122, 104, 93, 58, 32, 119, 239, 191, 189, 239, 191, 189, 239, 191, 189, 118, 10, 47, 47, 114, 101, 112, 108, 97, 99, 101, 32, 48, 32,
			119, 32, 32, 118, 10, 0};
		autoFileInMemory espeakdata_languages81 = FileInMemory_createWithData (665, reinterpret_cast<const char *> (&espeakdata_languages81_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/sit/cmn", 
			U"cmn");
		me -> addItem_move (espeakdata_languages81.move());

		static unsigned char espeakdata_languages82_data[26] = {
			110, 97, 109, 101, 32, 66, 117, 114, 109, 101, 115, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 109, 121, 10, 0};
		autoFileInMemory espeakdata_languages82 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeakdata_languages82_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/sit/my", 
			U"my");
		me -> addItem_move (espeakdata_languages82.move());

		static unsigned char espeakdata_languages83_data[210] = {
			110, 97, 109, 101, 32, 67, 104, 105, 110, 101, 115, 101, 32, 40, 67, 97, 110, 116, 111, 110, 101, 115, 101, 41, 10, 108, 97, 110, 103, 117,
			97, 103, 101, 32, 121, 117, 101, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 122, 104, 45, 121, 117, 101, 10, 108, 97, 110, 103, 117, 97,
			103, 101, 32, 122, 104, 32, 56, 10, 10, 116, 114, 97, 110, 115, 108, 97, 116, 111, 114, 32, 122, 104, 121, 10, 112, 104, 111, 110, 101, 109,
			101, 115, 32, 122, 104, 121, 10, 100, 105, 99, 116, 105, 111, 110, 97, 114, 121, 32, 122, 104, 121, 10, 10, 47, 47, 32, 105, 110, 116, 101,
			114, 112, 114, 101, 116, 32, 69, 110, 103, 108, 105, 115, 104, 32, 108, 101, 116, 116, 101, 114, 115, 32, 97, 115, 32, 49, 61, 69, 110, 103,
			108, 105, 115, 104, 32, 119, 111, 114, 100, 115, 44, 32, 50, 61, 106, 121, 117, 116, 112, 105, 110, 103, 10, 100, 105, 99, 116, 114, 117, 108,
			101, 115, 32, 49, 10, 10, 119, 111, 114, 100, 115, 32, 49, 10, 100, 105, 99, 116, 95, 109, 105, 110, 32, 49, 48, 48, 48, 48, 10, 0};
		autoFileInMemory espeakdata_languages83 = FileInMemory_createWithData (209, reinterpret_cast<const char *> (&espeakdata_languages83_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/sit/yue", 
			U"yue");
		me -> addItem_move (espeakdata_languages83.move());

		static unsigned char espeakdata_languages84_data[46] = {
			110, 97, 109, 101, 32, 65, 122, 101, 114, 98, 97, 105, 106, 97, 110, 105, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 97, 122, 10, 10,
			115, 116, 97, 116, 117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages84 = FileInMemory_createWithData (45, reinterpret_cast<const char *> (&espeakdata_languages84_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/trk/az", 
			U"az");
		me -> addItem_move (espeakdata_languages84.move());

		static unsigned char espeakdata_languages85_data[44] = {
			110, 97, 109, 101, 32, 75, 121, 114, 103, 121, 122, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 107, 121, 10, 10, 116, 117, 110, 101, 115,
			32, 115, 51, 32, 99, 51, 32, 113, 51, 32, 101, 51, 10, 0};
		autoFileInMemory espeakdata_languages85 = FileInMemory_createWithData (43, reinterpret_cast<const char *> (&espeakdata_languages85_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/trk/ky", 
			U"ky");
		me -> addItem_move (espeakdata_languages85.move());

		static unsigned char espeakdata_languages86_data[26] = {
			110, 97, 109, 101, 32, 84, 117, 114, 107, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 116, 114, 10, 0};
		autoFileInMemory espeakdata_languages86 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeakdata_languages86_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/trk/tr", 
			U"tr");
		me -> addItem_move (espeakdata_languages86.move());

		static unsigned char espeakdata_languages87_data[24] = {
			110, 97, 109, 101, 32, 84, 97, 116, 97, 114, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 116, 116, 10, 0};
		autoFileInMemory espeakdata_languages87 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeakdata_languages87_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/trk/tt", 
			U"tt");
		me -> addItem_move (espeakdata_languages87.move());

		static unsigned char espeakdata_languages88_data[28] = {
			110, 97, 109, 101, 32, 69, 115, 116, 111, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 101, 116, 10, 10, 0};
		autoFileInMemory espeakdata_languages88 = FileInMemory_createWithData (27, reinterpret_cast<const char *> (&espeakdata_languages88_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/urj/et", 
			U"et");
		me -> addItem_move (espeakdata_languages88.move());

		static unsigned char espeakdata_languages89_data[26] = {
			110, 97, 109, 101, 32, 70, 105, 110, 110, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 102, 105, 10, 0};
		autoFileInMemory espeakdata_languages89 = FileInMemory_createWithData (25, reinterpret_cast<const char *> (&espeakdata_languages89_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/urj/fi", 
			U"fi");
		me -> addItem_move (espeakdata_languages89.move());

		static unsigned char espeakdata_languages90_data[62] = {
			110, 97, 109, 101, 32, 72, 117, 110, 103, 97, 114, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 117, 10, 111, 112, 116,
			105, 111, 110, 32, 98, 114, 97, 99, 107, 101, 116, 32, 48, 32, 48, 10, 112, 105, 116, 99, 104, 32, 56, 49, 32, 49, 49, 55, 10, 10,
			10, 0};
		autoFileInMemory espeakdata_languages90 = FileInMemory_createWithData (61, reinterpret_cast<const char *> (&espeakdata_languages90_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/urj/hu", 
			U"hu");
		me -> addItem_move (espeakdata_languages90.move());

		static unsigned char espeakdata_languages91_data[60] = {
			110, 97, 109, 101, 32, 82, 117, 115, 115, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 114, 117, 10, 10, 114, 101, 112, 108,
			97, 99, 101, 32, 48, 51, 32, 97, 32, 97, 35, 10, 10, 100, 105, 99, 116, 95, 109, 105, 110, 32, 32, 50, 48, 48, 48, 48, 10, 0};
		autoFileInMemory espeakdata_languages91 = FileInMemory_createWithData (59, reinterpret_cast<const char *> (&espeakdata_languages91_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zle/ru", 
			U"ru");
		me -> addItem_move (espeakdata_languages91.move());

		static unsigned char espeakdata_languages92_data[112] = {
			110, 97, 109, 101, 32, 66, 117, 108, 103, 97, 114, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 98, 103, 10, 10, 115, 116,
			114, 101, 115, 115, 65, 109, 112, 32, 49, 51, 32, 49, 50, 32, 49, 55, 32, 49, 55, 32, 50, 48, 32, 50, 50, 32, 50, 50, 32, 50,
			49, 32, 10, 115, 116, 114, 101, 115, 115, 76, 101, 110, 103, 116, 104, 32, 49, 56, 48, 32, 49, 55, 48, 32, 32, 50, 48, 48, 32, 50,
			48, 48, 32, 32, 50, 48, 48, 32, 50, 48, 48, 32, 32, 50, 49, 48, 32, 50, 50, 48, 10, 0};
		autoFileInMemory espeakdata_languages92 = FileInMemory_createWithData (111, reinterpret_cast<const char *> (&espeakdata_languages92_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zls/bg", 
			U"bg");
		me -> addItem_move (espeakdata_languages92.move());

		static unsigned char espeakdata_languages93_data[231] = {
			110, 97, 109, 101, 32, 66, 111, 115, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 98, 115, 10, 112, 104, 111, 110, 101,
			109, 101, 115, 32, 104, 114, 10, 10, 112, 105, 116, 99, 104, 32, 56, 49, 32, 49, 50, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 48,
			32, 49, 48, 48, 32, 49, 48, 48, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 32, 57, 55, 32, 32, 57, 55,
			32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 50, 32, 32, 57, 55, 32, 32, 57, 55, 32, 49, 48, 48, 10, 102, 111, 114,
			109, 97, 110, 116, 32, 51, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 52, 32, 32,
			57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 53, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49,
			48, 48, 10, 10, 115, 116, 114, 101, 115, 115, 65, 100, 100, 32, 49, 48, 32, 49, 48, 32, 48, 32, 48, 32, 48, 32, 48, 32, 45, 51,
			48, 32, 45, 51, 48, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 51, 32, 52, 10, 0};
		autoFileInMemory espeakdata_languages93 = FileInMemory_createWithData (230, reinterpret_cast<const char *> (&espeakdata_languages93_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zls/bs", 
			U"bs");
		me -> addItem_move (espeakdata_languages93.move());

		static unsigned char espeakdata_languages94_data[263] = {
			110, 97, 109, 101, 32, 67, 114, 111, 97, 116, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 104, 114, 10, 108, 97, 110, 103,
			117, 97, 103, 101, 32, 104, 98, 115, 10, 10, 47, 47, 32, 97, 116, 116, 114, 105, 98, 117, 116, 101, 115, 32, 116, 111, 119, 97, 114, 100,
			115, 32, 33, 118, 97, 114, 105, 97, 110, 116, 51, 10, 112, 105, 116, 99, 104, 32, 56, 49, 32, 49, 50, 48, 10, 102, 111, 114, 109, 97,
			110, 116, 32, 48, 32, 49, 48, 48, 32, 49, 48, 48, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 32, 57, 55,
			32, 32, 57, 55, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 50, 32, 32, 57, 55, 32, 32, 57, 55, 32, 49, 48, 48,
			10, 102, 111, 114, 109, 97, 110, 116, 32, 51, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116,
			32, 52, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 53, 32, 32, 57, 55, 32, 49,
			48, 50, 32, 49, 48, 48, 10, 10, 115, 116, 114, 101, 115, 115, 65, 100, 100, 32, 49, 48, 32, 49, 48, 32, 48, 32, 48, 32, 48, 32,
			48, 32, 45, 51, 48, 32, 45, 51, 48, 10, 100, 105, 99, 116, 114, 117, 108, 101, 115, 32, 49, 10, 0};
		autoFileInMemory espeakdata_languages94 = FileInMemory_createWithData (262, reinterpret_cast<const char *> (&espeakdata_languages94_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zls/hr", 
			U"hr");
		me -> addItem_move (espeakdata_languages94.move());

		static unsigned char espeakdata_languages95_data[29] = {
			110, 97, 109, 101, 32, 77, 97, 99, 101, 100, 111, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 109, 107, 10, 0};
		autoFileInMemory espeakdata_languages95 = FileInMemory_createWithData (28, reinterpret_cast<const char *> (&espeakdata_languages95_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zls/mk", 
			U"mk");
		me -> addItem_move (espeakdata_languages95.move());

		static unsigned char espeakdata_languages96_data[44] = {
			110, 97, 109, 101, 32, 83, 108, 111, 118, 101, 110, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 108, 10, 10, 115, 116,
			97, 116, 117, 115, 32, 116, 101, 115, 116, 105, 110, 103, 10, 0};
		autoFileInMemory espeakdata_languages96 = FileInMemory_createWithData (43, reinterpret_cast<const char *> (&espeakdata_languages96_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zls/sl", 
			U"sl");
		me -> addItem_move (espeakdata_languages96.move());

		static unsigned char espeakdata_languages97_data[251] = {
			110, 97, 109, 101, 32, 83, 101, 114, 98, 105, 97, 110, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 114, 10, 10, 47, 47, 32, 97,
			116, 116, 114, 105, 98, 117, 116, 101, 115, 32, 116, 111, 119, 97, 114, 100, 115, 32, 33, 118, 97, 114, 105, 97, 110, 116, 51, 32, 112, 105,
			116, 99, 104, 32, 56, 48, 32, 49, 50, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 48, 32, 49, 48, 48, 32, 49, 48, 48, 32, 49,
			48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 49, 32, 32, 57, 55, 32, 32, 57, 55, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97,
			110, 116, 32, 50, 32, 32, 57, 55, 32, 32, 57, 55, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 51, 32, 32, 57, 55,
			32, 49, 48, 50, 32, 49, 48, 48, 10, 102, 111, 114, 109, 97, 110, 116, 32, 52, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48,
			10, 102, 111, 114, 109, 97, 110, 116, 32, 53, 32, 32, 57, 55, 32, 49, 48, 50, 32, 49, 48, 48, 10, 10, 115, 116, 114, 101, 115, 115,
			65, 100, 100, 32, 49, 48, 32, 49, 48, 32, 48, 32, 48, 32, 48, 32, 48, 32, 45, 51, 48, 32, 45, 51, 48, 10, 100, 105, 99, 116,
			114, 117, 108, 101, 115, 32, 50, 32, 52, 10, 0};
		autoFileInMemory espeakdata_languages97 = FileInMemory_createWithData (250, reinterpret_cast<const char *> (&espeakdata_languages97_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zls/sr", 
			U"sr");
		me -> addItem_move (espeakdata_languages97.move());

		static unsigned char espeakdata_languages98_data[24] = {
			110, 97, 109, 101, 32, 67, 122, 101, 99, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 99, 115, 10, 0};
		autoFileInMemory espeakdata_languages98 = FileInMemory_createWithData (23, reinterpret_cast<const char *> (&espeakdata_languages98_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zlw/cs", 
			U"cs");
		me -> addItem_move (espeakdata_languages98.move());

		static unsigned char espeakdata_languages99_data[39] = {
			110, 97, 109, 101, 32, 80, 111, 108, 105, 115, 104, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 112, 108, 10, 10, 105, 110, 116, 111, 110,
			97, 116, 105, 111, 110, 32, 50, 10, 0};
		autoFileInMemory espeakdata_languages99 = FileInMemory_createWithData (38, reinterpret_cast<const char *> (&espeakdata_languages99_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zlw/pl", 
			U"pl");
		me -> addItem_move (espeakdata_languages99.move());

		static unsigned char espeakdata_languages100_data[25] = {
			110, 97, 109, 101, 32, 83, 108, 111, 118, 97, 107, 10, 108, 97, 110, 103, 117, 97, 103, 101, 32, 115, 107, 10, 0};
		autoFileInMemory espeakdata_languages100 = FileInMemory_createWithData (24, reinterpret_cast<const char *> (&espeakdata_languages100_data), 
			U"/home/david/projects/espeak-ng/espeak-ng-data/lang/zlw/sk", 
			U"sk");
		me -> addItem_move (espeakdata_languages100.move());

		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySet not created.");
	}
}


