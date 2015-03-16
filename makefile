# ======== COMPILER ========

CC      = gcc
#OPT	= -Wall -g \
#          -DPURIFY_VERSION=\"0.1\" -DPURIFY_BUILD=\"`svnversion -n .`\"
OPT	= -Wall -O3 -fopenmp \
          -DPURIFY_VERSION=\"0.1\" \
          -DPURIFY_BUILD=\"`git rev-parse HEAD`\"


# ======== LINKS ========

PROGDIR    = ..

PURIFYDIR  = $(PROGDIR)/purify
PURIFYLIB  = $(PURIFYDIR)/lib
PURIFYLIBNM= purify
PURIFYSRC  = $(PURIFYDIR)/src
PURIFYBIN  = $(PURIFYDIR)/bin
PURIFYOBJ  = $(PURIFYSRC)
PURIFYINC  = $(PURIFYDIR)/include
PURIFYDOC  = $(PURIFYDIR)/doc

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
  FFTWDIR      = $(PROGDIR)/fftw-3.2.2_fPIC
endif
ifeq ($(UNAME), Darwin)
  FFTWDIR      = $(PROGDIR)/fftw
endif
FFTWINC	     = $(FFTWDIR)/include
FFTWLIB      = $(FFTWDIR)/lib
FFTWLIBNM    = fftw3

CFITSIODIR   = $(PROGDIR)/cfitsio
CFITSIOINC   = $(CFITSIODIR)/include
CFITSIOLIB   = $(CFITSIODIR)/lib
CFITSIOLIBNM = cfitsio

SOPTDIR  = $(PROGDIR)/sopt
SOPTINC  = $(SOPTDIR)/include
SOPTLIB  = $(SOPTDIR)/lib
SOPTLIBNM= sopt

TIFFDIR      = $(PROGDIR)/tiff
TIFFINC	     = $(TIFFDIR)/include
TIFFLIB      = $(TIFFDIR)/lib
TIFFLIBNM    = tiff


# ======== SOURCE LOCATIONS ========

vpath %.c $(PURIFYSRC)
vpath %.h $(PURIFYINC)


# ======== FFFLAGS ========

FFLAGS  = -I$(FFTWINC) -I$(CFITSIOINC) -I$(PURIFYINC) -I$(SOPTINC) -I$(TIFFINC)


# ======== LDFLAGS ========

ifeq ($(UNAME), Linux)
  LDFLAGS = -static 
endif
ifeq ($(UNAME), Darwin)
  LDFLAGS = 
endif
LDFLAGS += -L$(PURIFYLIB) -l$(PURIFYLIBNM)         \
           -L$(SOPTLIB) -l$(SOPTLIBNM)             \
           -L$(CFITSIOLIB) -l$(CFITSIOLIBNM)       \
           -L$(FFTWLIB) -l$(FFTWLIBNM)             \
           -L$(TIFFLIB) -l$(TIFFLIBNM)
LDFLAGS += -lm -lblas


# ======== OBJECT FILES TO MAKE ========

PURIFYOBJS = $(PURIFYOBJ)/purify_ran.o            \
             $(PURIFYOBJ)/purify_error.o          \
             $(PURIFYOBJ)/purify_visibility.o     \
             $(PURIFYOBJ)/purify_image.o          \
             $(PURIFYOBJ)/purify_measurement.o    \
             $(PURIFYOBJ)/purify_utils.o    \
             $(PURIFYOBJ)/purify_sparsemat.o

PURIFYHEADERS = purify_error.h                   \
                purify_types.h                   \
                purify_ran.h                     \
                purify_visibility.h              \
                purify_image.h                   \
                purify_measurement.h             \
                purify_utils.h                   \
                purify_sparsemat.h 

PURIFYPROGS = $(PURIFYBIN)/purify_about         \
              $(PURIFYBIN)/example_m31          \
              $(PURIFYBIN)/example_30dor        \
              $(PURIFYBIN)/example_ami  


# ======== MAKE RULES ========

$(PURIFYOBJ)/%.o: %.c $(PURIFYHEADERS)
	$(CC) $(OPT) $(FFLAGS) -c $< -o $@

.PHONY: default
default: lib

.PHONY: all
all: lib test prog

.PHONY: prog
prog: $(PURIFYPROGS)
$(PURIFYBIN)/%: %.c $(PURIFYLIB)/lib$(PURIFYLIBNM).a
	$(CC) $(OPT) $(FFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: test
test: $(PURIFYBIN)/purify_test
$(PURIFYBIN)/purify_test: $(PURIFYOBJ)/purify_test.c $(PURIFYLIB)/lib$(PURIFYLIBNM).a
	$(CC) $(OPT) $(FFLAGS) $< -o $@ $(LDFLAGS)



.PHONY: runtest
runtest: test
	$(PURIFYBIN)/purify_test

.PHONY: cleantest
cleantest: 
	rm -rf data/test/*


# Library

.PHONY: lib
lib: $(PURIFYLIB)/lib$(PURIFYLIBNM).a
$(PURIFYLIB)/lib$(PURIFYLIBNM).a: $(PURIFYOBJS)
	ar -r $(PURIFYLIB)/lib$(PURIFYLIBNM).a $(PURIFYOBJS)


# Documentation 

.PHONY: doc
doc:
	doxygen $(PURIFYSRC)/doxygen.config
.PHONY: cleandoc
cleandoc:
	rm -f $(PURIFYDOC)/html/*


# Cleaning up

.PHONY: clean
clean:	tidy cleantest
	rm -f $(PURIFYOBJ)/*.o
	rm -f $(PURIFYLIB)/lib$(PURIFYLIBNM).a
	rm -rf $(PURIFYBIN)/*

.PHONY: tidy
tidy:
	rm -f *~
	rm -f $(PURIFYSRC)/*~ 

.PHONY: cleanall
cleanall: clean cleandoc