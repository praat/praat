# Makefile for library glpk.
# Paul Boersma, March 31, 2008

include ../makefile.defs

CFLAGS = 

all: libglpk.a

clean:
	rm *.o

OBJECTS = glpapi01.o glpapi02.o glpapi03.o glpapi04.o glpapi05.o glpapi06.o \
   glpapi07.o glpapi08.o glpapi09.o glpapi10.o glpapi11.o glpapi12.o glpapi13.o \
   glpavl.o glpbfd.o glpbfx.o glpcli.o glpdmp.o glpfhv.o glpgmp.o glphbm.o \
   glpios01.o glpios02.o glpios03.o glpios04.o glpios05.o glpios06.o glpipm.o \
   glpipp01.o glpipp02.o glplib01.o glplib02.o glplib03.o glplib04.o glplib05.o \
   glplib06.o glplib07.o glplib08.o glplib09.o glplib10.o glplib11.o glplib12.o \
   glplpf.o glplpp01.o glplpp02.o \
   glplpx01.o glplpx02.o glplpx03.o glplpx04.o glplpx05.o \
   glplpx06.o glplpx07.o glplpx08.o glplpx09.o glplpx10.o \
   glplpx11.o glplpx12.o glplpx13.o glplpx14.o glplpx15.o \
   glplpx16.o glplpx17.o glplpx18.o glplpx19.o glplpx20.o \
   glpluf.o glplux.o glpmat.o \
   glpmpl01.o glpmpl02.o glpmpl03.o glpmpl04.o glpmpl05.o \
   glppds.o glpqmd.o glprgr.o glprng01.o glprng02.o \
   glpscf.o glpscg.o glpspm.o glpspx01.o glpspx02.o \
   glpsql.o glpssx01.o glpssx02.o glptsp.o

$(OBJECTS): *.h

libglpk.a: $(OBJECTS)
	touch libglpk.a
	rm libglpk.a
	ar cq libglpk.a $(OBJECTS)
	$(RANLIB) libglpk.a

# end of Makefile
