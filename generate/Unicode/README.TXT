File generate/unicode/README.TXT
Paul Boersma, 20190618

Steps to generate the source file UnicodeData.cpp,
which is to be put in the sys folder:

1. Download UnicodeData.txt from unicode.org.

2. Prepend the following header, using a simple text editor:

	code;name;category;combining;bidi;decomp;num1;num2;num3;mirror;dum1;dum2;upper;lower;title

After this, the file can be read as a Table in Praat with
"Read Table from semicolon-separated file...".
After that, the Table can be viewed with "View & Edit",
and it is easy to extract information with commands such as
"Extract rows where column (text)..." and "Extract rows where...".

For information on the meanings of the features, see the attached HTML file and
the attached UnicodeStandard-10.0.pdf (both from June 2017).

3. Run the script UnicodeData.cpp.praat. This creates UnicodeData.cpp.
The details of the process are discussed in UnicodeData.cpp.praat.

## Timing.

Step 3 is computationally intensive, and therefore a good test of the speed of the
Praat scripting language.

The measured time on my 2018 MacBook Pro with 2.9 GHz Intel Core i9:

Praat 6.0.55: 21 seconds.


