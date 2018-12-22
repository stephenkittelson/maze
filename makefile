#makefile for MAZE by Stephen Kittelson <stickman2000@juno.com>

#DEBUG = 1
#GDB = 1
LIBDIR = ../lib/
OBJDIR = obj/

#no need for changes beyond this point

ifdef DEBUG
	OBJFILES := $(addprefix $(OBJDIR),$(patsubst %.c,%.o,$(wildcard *.c))) $(LIBDIR)std.o
else
	OBJFILES := $(addprefix $(OBJDIR),$(patsubst %.c,%.o,$(wildcard *.c)))
endif

ifdef DEBUG
	LFLAGS = 
	ifdef GDB
		CFLAGS = -DDEBUG -DLIBDIR=\"$(LIBDIR)\" -ggdb -Wall -O2
	else
		CFLAGS = -DDEBUG -DLIBDIR=\"$(LIBDIR)\" -Wall -O2
	endif
else
	LFLAGS = -s
	CFLAGS = -DLIBDIR=\"$(LIBDIR)\" -Wall -O2
endif

.PHONY: all clean depend


all: maze.exe $(wildcard *.c) $(wildcard *.h)

maze.exe : $(OBJFILES)
	gcc $(LFLAGS) -o maze.exe $(OBJFILES) -lalleg

obj/io.o : io.c io.h
	gcc $(CFLAGS) -o obj/io.o -c io.c

obj/main.o : main.c
	gcc $(CFLAGS) -o obj/main.o -c main.c

ifdef DEBUG
$(LIBDIR)std.o : $(LIBDIR)std.c $(LIBDIR)std.h
	gcc $(CFLAGS) -o $(LIBDIR)std.o -c $(LIBDIR)std.c
endif

-include makefile.dep

clean : 
	del obj\*.o
	del maze.exe

depend :
	gcc -DLIBDIR=\"$(LIBDIR)\" -MM $(wildcard *.c) > makefile.dep
	depfix < makefile.dep > temp.dep
	move /Y temp.dep makefile.dep
