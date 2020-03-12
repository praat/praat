# Makefile of the library liblapack.a
# David Weenink 
# Generated on Thu Mar 12 12:19:11 2020
# with the script "CLAPACK_copyFiles_to_Praat.praat".
# For CLAPACK version 3.1.1.1.

include ../../../makefile.defs

CPPFLAGS = -I ..

OBJECTS = dbdsdc.o dbdsqr.o ddisna.o \
	dgbbrd.o dgbcon.o dgbequ.o \
	dgbrfs.o dgbsv.o dgbsvx.o \
	dgbtf2.o dgbtrf.o dgbtrs.o \
	dgebak.o dgebal.o dgebd2.o \
	dgebrd.o dgecon.o dgeequ.o \
	dgees.o dgeesx.o dgeev.o \
	dgeevx.o dgegs.o dgegv.o \
	dgehd2.o dgehrd.o dgelq2.o \
	dgelqf.o dgels.o dgelsd.o \
	dgelss.o dgelsx.o dgelsy.o \
	dgeql2.o dgeqlf.o dgeqp3.o \
	dgeqpf.o dgeqr2.o dgeqrf.o \
	dgerfs.o dgerq2.o dgerqf.o \
	dgesc2.o dgesdd.o dgesv.o \
	dgesvd.o dgesvx.o dgetc2.o \
	dgetf2.o dgetrf.o dgetri.o \
	dgetrs.o dggbak.o dggbal.o \
	dgges.o dggesx.o dggev.o \
	dggevx.o dggglm.o dgghrd.o \
	dgglse.o dggqrf.o dggrqf.o \
	dggsvd.o dggsvp.o dgtcon.o \
	dgtrfs.o dgtsv.o dgtsvx.o \
	dgttrf.o dgttrs.o dgtts2.o \
	dhgeqz.o dhsein.o dhseqr.o \
	disnan.o dlabad.o dlabrd.o \
	dlacn2.o dlacon.o dlacpy.o \
	dladiv.o dlae2.o dlaebz.o \
	dlaed0.o dlaed1.o dlaed2.o \
	dlaed3.o dlaed4.o dlaed5.o \
	dlaed6.o dlaed7.o dlaed8.o \
	dlaed9.o dlaeda.o dlaein.o \
	dlaev2.o dlaexc.o dlag2.o \
	dlag2s.o dlags2.o dlagtf.o \
	dlagtm.o dlagts.o dlagv2.o \
	dlahqr.o dlahr2.o dlahrd.o \
	dlaic1.o dlaisnan.o dlaln2.o \
	dlals0.o dlalsa.o dlalsd.o \
	dlamch.o dlamrg.o dlaneg.o \
	dlangb.o dlange.o dlangt.o \
	dlanhs.o dlansb.o dlansp.o \
	dlanst.o dlansy.o dlantb.o \
	dlantp.o dlantr.o dlanv2.o \
	dlapll.o dlapmt.o dlapy2.o \
	dlapy3.o dlaqgb.o dlaqge.o \
	dlaqp2.o dlaqps.o dlaqr0.o \
	dlaqr1.o dlaqr2.o dlaqr3.o \
	dlaqr4.o dlaqr5.o dlaqsb.o \
	dlaqsp.o dlaqsy.o dlaqtr.o \
	dlar1v.o dlar2v.o dlarf.o \
	dlarfb.o dlarfg.o dlarft.o \
	dlarfx.o dlargv.o dlarnv.o \
	dlarra.o dlarrb.o dlarrc.o \
	dlarrd.o dlarre.o dlarrf.o \
	dlarrj.o dlarrk.o dlarrr.o \
	dlarrv.o dlartg.o dlartv.o \
	dlaruv.o dlarz.o dlarzb.o \
	dlarzt.o dlas2.o dlascl.o \
	dlasd0.o dlasd1.o dlasd2.o \
	dlasd3.o dlasd4.o dlasd5.o \
	dlasd6.o dlasd7.o dlasd8.o \
	dlasda.o dlasdq.o dlasdt.o \
	dlaset.o dlasq1.o dlasq2.o \
	dlasq3.o dlasq4.o dlasq5.o \
	dlasq6.o dlasr.o dlasrt.o \
	dlassq.o dlasv2.o dlaswp.o \
	dlasy2.o dlasyf.o dlatbs.o \
	dlatdf.o dlatps.o dlatrd.o \
	dlatrs.o dlatrz.o dlatzm.o \
	dlauu2.o dlauum.o dlazq3.o \
	dlazq4.o dopgtr.o dopmtr.o \
	dorg2l.o dorg2r.o dorgbr.o \
	dorghr.o dorgl2.o dorglq.o \
	dorgql.o dorgqr.o dorgr2.o \
	dorgrq.o dorgtr.o dorm2l.o \
	dorm2r.o dormbr.o dormhr.o \
	dorml2.o dormlq.o dormql.o \
	dormqr.o dormr2.o dormr3.o \
	dormrq.o dormrz.o dormtr.o \
	dpbcon.o dpbequ.o dpbrfs.o \
	dpbstf.o dpbsv.o dpbsvx.o \
	dpbtf2.o dpbtrf.o dpbtrs.o \
	dpocon.o dpoequ.o dporfs.o \
	dposv.o dposvx.o dpotf2.o \
	dpotrf.o dpotri.o dpotrs.o \
	dppcon.o dppequ.o dpprfs.o \
	dppsv.o dppsvx.o dpptrf.o \
	dpptri.o dpptrs.o dptcon.o \
	dpteqr.o dptrfs.o dptsv.o \
	dptsvx.o dpttrf.o dpttrs.o \
	dptts2.o drscl.o dsbev.o \
	dsbevd.o dsbevx.o dsbgst.o \
	dsbgv.o dsbgvd.o dsbgvx.o \
	dsbtrd.o dsgesv.o dspcon.o \
	dspev.o dspevd.o dspevx.o \
	dspgst.o dspgv.o dspgvd.o \
	dspgvx.o dsprfs.o dspsv.o \
	dspsvx.o dsptrd.o dsptrf.o \
	dsptri.o dsptrs.o dstebz.o \
	dstedc.o dstegr.o dstein.o \
	dstemr.o dsteqr.o dsterf.o \
	dstev.o dstevd.o dstevr.o \
	dstevx.o dsycon.o dsyev.o \
	dsyevd.o dsyevr.o dsyevx.o \
	dsygs2.o dsygst.o dsygv.o \
	dsygvd.o dsygvx.o dsyrfs.o \
	dsysv.o dsysvx.o dsytd2.o \
	dsytf2.o dsytrd.o dsytrf.o \
	dsytri.o dsytrs.o dtbcon.o \
	dtbrfs.o dtbtrs.o dtgevc.o \
	dtgex2.o dtgexc.o dtgsen.o \
	dtgsja.o dtgsna.o dtgsy2.o \
	dtgsyl.o dtpcon.o dtprfs.o \
	dtptri.o dtptrs.o dtrcon.o \
	dtrevc.o dtrexc.o dtrrfs.o \
	dtrsen.o dtrsna.o dtrsyl.o \
	dtrti2.o dtrtri.o dtrtrs.o \
	dtzrqf.o dtzrzf.o icmax1.o \
	ieeeck.o ilaenv.o ilaver.o \
	iparmq.o lsame.o lsamen.o 

.PHONY: all clean

all: liblapack.a

clean:
	$(RM) $(OBJECTS)
	$(RM) liblapack.a

liblapack.a: $(OBJECTS)
	touch liblapack.a
	rm liblapack.a
	$(AR) cq liblapack.a $(OBJECTS)
	$(RANLIB) liblapack.a

$(OBJECTS): ../*.h ../../../melder/*.h
