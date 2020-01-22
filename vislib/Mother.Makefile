################################################
# visulazation v2 makefile.
# (c) Jon DuBois 2004
# 10/22/2004
# See license.txt for license details.
################################################
OWNER = root:root
MODE = u=rwX,go=rX
CPP = g++
C = gcc
AR = ar
WINDRES = windres
OUTFLAG = -o
COMPILEFLAG = -c
ARCHIVEFLAG = -r

CPPFLAGS += -Wall -x c++ -ansi -pedantic -I ./
CFLAGS += -Wall -x c -ansi -pedantic -I ./
LDFLAGS += -mwindows -L./ -l$(LIBNAME)

ROOTDIR = /mingw
LIBDIR = lib
INCDIR = include
BINDIR = bin

%.o: %.cpp
	$(CPP) $(COMPILEFLAG) $(CPPFLAGS) $< $(OUTFLAG) $@

%.o: %.c
	$(C) $(COMPILEFLAG) $(CFLAGS) $< $(OUTFLAG) $@

%.o: %.rc
	$(WINDRES) $< $(OUTFLAG) $@

.PHONY: all 
all: $(LIB) $(BIN) $(TEST)

.PHONY: install 
install: clean prod
ifdef LIB 
	cp -f $(LIB) $(ROOTDIR)/$(LIBDIR)
	chown $(OWNER) $(ROOTDIR)/$(LIBDIR)/$(LIB)
	chmod $(MODE) $(ROOTDIR)/$(LIBDIR)/$(LIB)
endif
ifdef BIN
	cp -f $(BIN) $(ROOTDIR)/$(BINDIR)
	chown $(OWNER) $(ROOTDIR)/$(BINDIR)/$(BIN)
	chmod $(MODE) $(ROOTDIR)/$(BINDIR)/$(BIN)
endif
ifdef PROJECTINC
	rm -rf $(ROOTDIR)/$(INCDIR)/$(PROJECTINC)	
	mkdir $(ROOTDIR)/$(INCDIR)/$(PROJECTINC)
	cp -f $(HDRS:%=$(PROJECTINC)/%) $(ROOTDIR)/$(INCDIR)/$(PROJECTINC)
	chown -R $(OWNER) $(ROOTDIR)/$(INCDIR)/$(PROJECTINC)
	chmod -R $(MODE) $(ROOTDIR)/$(INCDIR)/$(PROJECTINC)
endif


$(LIB): $(LIBOBJS)
	$(AR) $(ARCHIVEFLAG) $@ $^

$(TEST): $(TESTOBJS) $(LIB)
	$(CPP) $^ $(LDFLAGS) $(OUTFLAG) $@

$(BIN): $(BINOBJS) $(LIB)
	$(CPP) $^ $(LDFLAGS) $(OUTFLAG) $@

$(LIBOBJS) $(BINOBJS) $(TESTOBJS): $(HDRS:%=$(PROJECTINC)/%)


.PHONY: prod
prod: CPPFLAGS += -O2 -DNO_ASSESS
prod: all strip
	#Production release flags set

.PHONY: debug 
debug: CPPFLAGS += -DDBG -ggdb 
debug: all
	#Memory test flags set

.PHONY: memtest 
memtest: CPPFLAGS += -DDBG -DMEM_TEST -ggdb 
memtest: all
	#Memory test flags set

.PHONY: gprof
gprof: CPPFLAGS += -pg
gprof: LDFLAGS += -lgmon -pg 
gprof: all
	#Profiling flag set

.PHONY: strip
strip:
	strip $(BIN) $(TEST)

.PHONY: clean 
clean:
	rm -rf ./*.o ./*~ gmon.* $(LIB) $(BIN) $(TEST)

.PHONY: tarball
tarball: clean
	rm -f ../$(PROJECT).tgz
	tar -C ../ -chzf ../$(PROJECT).tgz $(PROJECT)
