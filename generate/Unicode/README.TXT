File generate/Unicode/README.TXT
Paul Boersma 2022-05-17

Steps to generate the source file UCD_features_generated.h,
which is to be put in the kar folder:

1. Download UnicodeData.txt from unicode.org.

2. Prepend the following header, using a simple text editor:

	code;name;category;combining;bidi;decomp;num1;num2;num3;mirror;dum1;dum2;upper;lower;title

After this, the file can be read as a Table in Praat with
"Read Table from semicolon-separated file...".
After that, the Table can be viewed with "View & Edit",
and it is easy to extract information with commands such as
"Extract rows where column (text)..." and "Extract rows where...".

For information on the meanings of the features, see the following attached files (version 14, from 2021):
- UAX #44  Unicode Character Database.html  (replaced colon by space in file name)
- UnicodeData File Format.html
- UnicodeStandard.pdf

3. Run the script UCD_features_generated_h.praat. This creates UCD_features_generated.h.
The details of the process are discussed in UCD_features_generated_h.praat.

## Timing.

Step 3 is computationally intensive, and therefore a good test of the speed of the
Praat scripting language.

The measured time on my 2018 MacBook Pro with 2.9 GHz Intel Core i9:

Praat 6.2.12: 24.6 seconds.
