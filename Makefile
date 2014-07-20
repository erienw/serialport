
PROGRAM = main.exe

ifndef WINE
CC      = gcc
WRES    = windres
else
CC      = winegcc
WRES    = wrc
endif

ifdef DEBUG
ODIR    = Debug
CFLAGS  = -g -D_DEBUG -mno-cygwin
else
ODIR    = Release
CFLAGS  = 
endif

SRCDIR  = .
LIBDIRS =
INCDIRS =
LIBS    = -mwindows -lcomctl32 -lcomdlg32 -lwinmm -ladvapi32 -lwinspool

SRCS = $(SRCDIR)/main.c\
	   $(SRCDIR)/rs232.c\
	   $(SRCDIR)/rs232.rc\
	   $(SRCDIR)/data.c

OBJS = $(ODIR)/main.obj\
	   $(ODIR)/rs232.obj\
	   $(ODIR)/rs232.res\
	   $(ODIR)/data.obj

HDRS = $(SRCDIR)/resource.h \
	   $(SRCDIR)/rs232.h\
	   $(SRCDIR)/data.h

ALLOBJS  = $(OBJS)
ALLBIN   = $(ODIR)/$(PROGRAM)

all: $(ODIR)/$(PROGRAM)

cleanobjs:
	rm -f $(ALLOBJS)

cleanbin:
	rm -f $(ALLBIN)

clean: cleanobjs cleanbin

cleanall: cleanobjs cleanbin

$(ODIR)/$(PROGRAM): $(OBJS) $(HDRS)
	$(CC) -o $(ODIR)/$(PROGRAM) $(OBJS) $(INCDIRS) $(LIBDIRS) $(LIBS)

$(ODIR)/%.res : %.rc $(HDRS) $(ODIR)
	$(WRES) --use-temp-file -O coff $< $@

$(ODIR)/%.obj : %.cpp $(HDRS) $(ODIR)
	$(CC) $(CFLAGS) -c $(INCDIRS) -o $@ $<

$(ODIR)/%.obj : %.c $(HDRS) $(ODIR)
	$(CC) $(CFLAGS) -c $(INCDIRS) -o $@ $<

$(ODIR):
	mkdir $@
