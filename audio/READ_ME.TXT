In order to make PortAudio run on MacOS X versions 10.2 and 10.3,
Paul Boersma erased from the source files everything that has to do
with blocking I/O, ring buffers, and sample rate conversion.
The files in this directory therefore do not represent the original PortAudio.

Paul Boersma, January 3, 2007.
