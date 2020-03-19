# Makefile of the library libf2c.a
# David Weenink 20200319

include ../../../makefile.defs

CPPFLAGS = -I ../../../melder -I ..

OBJECTS = derf_.o \
	derfc_.o dolio.o dtime_.o \
	pow_di.o s_cat.o \
	s_cmp.o s_copy.o

.PHONY: all clean

all: libf2c.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libf2c.a

libf2c.a: $(OBJECTS)
	touch libf2c.a
	rm libf2c.a
	$(AR) cq libf2c.a $(OBJECTS)
	$(RANLIB) libf2c.a

$(OBJECTS): ../*.h ../../../melder/*.h
