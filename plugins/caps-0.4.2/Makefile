PREFIX = /usr/local

CC = g++
OPTS = -O2 -ffast-math -funroll-loops -Wall -fPIC -DPIC
#OPTS = -g -DDEBUG 

-include defines.make

CFLAGS = $(OPTS) $(_CFLAGS)

PLUG = caps
VERSION = 0.4.2

SOURCES = $(wildcard *.cc)
OBJECTS	= $(SOURCES:.cc=.o) 
HEADERS = $(wildcard *.h) $(wildcard dsp/*.h) $(wildcard util/*.h) $(wildcard dsp/tonestack/*.h)

PDF = releases/caps-$(VERSION).pdf

DEST = $(PREFIX)/lib/ladspa
RDFDEST = $(PREFIX)/share/ladspa/rdf

# all systems go ##############################################################

all: depend $(PLUG).so tags

run: all
	#sudo python -i test.py
	#python -i offline.py
	#python tools/test-denormals.py
	#sudo python tools/caps-test.py scape.rack
	sudo python tools/caps-test.py ts.rack
	#gdb python -x gdb-batch
	#python tools/make-pdf.py

pdf: all tools/make-ps.py
	VERSION=$(VERSION) python tools/make-ps.py | ps2pdf - $(PDF)
	xpdf $(PDF) 

rdf: $(PLUG).rdf
caps.rdf: all tools/make-rdf.py
	python tools/make-rdf.py > $(PLUG).rdf

$(PLUG).so: $(OBJECTS)
	$(CC) -nostartfiles $(CFLAGS) -shared -o $@ $(OBJECTS)

.cc.s: 
	$(CC) $(CFLAGS) -S $<

.cc.o: depend
	$(CC) $(CFLAGS) -I/usr/local/include -c $<

tags: $(SOURCES) $(HEADERS)
	@echo making tags
	@-if [ -x /usr/bin/ctags ]; then ctags $(SOURCES) $(HEADERS) ; fi

install: all
	strip $(PLUG).so
	install -d $(DEST)
	install -m 644 $(PLUG).so $(DEST)
	install -d $(RDFDEST)
	install -m 644 $(PLUG).rdf $(RDFDEST)

fake-install: all
	-rm $(DEST)/$(PLUG).so
	ln -s `pwd`/$(PLUG).so $(DEST)/$(PLUG).so

rdf-install:
	install -d $(RDFDEST)
	install -m 644 $(PLUG).rdf $(RDFDEST)

uninstall:
	-rm $(DEST)/$(PLUG).so
	-rm $(DEST)/$(PLUG)-ng.so

clean:
	rm -f *.o *.so *.s depend

dist: all pdf
	cp $(PDF) /www/quitte/dsp/
	touch /www/quitte/dsp/caps_$(VERSION).tar.gz
	tools/make-dist.py caps $(VERSION) $(CFLAGS)
	@rm /www/quitte/dsp/caps*.tar.gz
	@cp releases/caps_$(VERSION).tar.gz /www/quitte/dsp

depend: $(SOURCES) $(HEADERS)
	$(CC) -MM $(CFLAGS) $(DEFINES) $(SOURCES) > depend

-include depend
