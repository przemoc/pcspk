include Makefile.cnf

OBJS = pbase/pbase.o popts/popts.o str_en.o
MODS = pcspk pcspkd
EPLS = pcspk-examples siemens.gawk
LIBS = -lm -lcurses
DIRS = -std=c99

CFLAGS += -Wall

CONF = $(ETCDIR)/pcspk.conf

SETTINGS = -DFILECONF=\""$(CONF)\""

default : $(MODS)
install : install-core install-examples
all : compile install
uninstall : uninstall-core uninstall-examples

%.o : %.c
	$(CC) $(CFLAGS) $(SETTINGS) -c -o $@ $<

str_en.o : pbase/str_en.c str_config.h
	$(CC) $(CFLAGS) -c -o $@ $< -include str_config.h

pbase/pbase.o:
	cd pbase && $(MAKE)

popts/popts.o:
	cd popts && $(MAKE)

doc : popts/*.c pbase/*.c $(MODS:=.c) config.h str_config.h
	doxygen
	$(TCH) doc/

pcspk :	$(OBJS) config.h pcspk.c 
	$(CC) $(CFLAGS) $(SETTINGS) $(OBJS) $(DIRS) $(LDFLAGS) $@.c -o $@ $(LIBS)

pcspkd : $(OBJS) config.h pcspkd.c 
	$(CC) $(CFLAGS) $(SETTINGS) $(OBJS) $(DIRS) $(LDFLAGS) $@.c -o $@ $(LIBS)

examples :
	if $(WHICH) gawk >/dev/null 2>&1; then $(SED) -ie "1s|.*|#!`$(WHICH) gawk` -f|" siemens.gawk; else echo 'No gawk detected!'; exit 1; fi

clean :
	$(RM) rc.pcspkd init.pcspk $(OBJS) $(MODS:=.o) $(MODS) doc/

distclean: clean
	cd pbase && $(MAKE) clean
	cd popts && $(MAKE) clean

install-core : $(MODS)
	$(INSTALL) -c -o root -g 0 -m 755 pcspkd $(SBINDIR)
	$(INSTALL) -c -o root -g 0 -m 755 pcspk $(BINDIR)

install-examples : examples
	$(INSTALL) -c -o root -g 0 -m 755 $(EPLS) $(BINDIR)

install-config :
	$(ECHO) "host = $(HOST)\nport = $(PORT)" > $(CONF)
	$(CHM) 644 $(CONF)

install-debian :
	$(SED) "s|SBINDIR|$(SBINDIR)|" init.pcspk.template > init.pcspk
	$(INSTALL) -c -o root -g 0 -m 755 init.pcspk /etc/init.d/pcspk
	$(LN) -s /etc/init.d/pcspk /etc/rc2.d/S99pcspk
	$(LN) -s /etc/init.d/pcspk /etc/rc3.d/S99pcspk
	$(LN) -s /etc/init.d/pcspk /etc/rc4.d/S99pcspk
	$(LN) -s /etc/init.d/pcspk /etc/rc5.d/S99pcspk
	$(LN) -s /etc/init.d/pcspk /etc/rc0.d/K99pcspk
	$(LN) -s /etc/init.d/pcspk /etc/rc6.d/K99pcspk

uninstall-debian :
	$(RM) /etc/init.d/pcspk /etc/rc2.d/S99pcspk /etc/rc3.d/S99pcspk /etc/rc4.d/S99pcspk /etc/rc5.d/S99pcspk /etc/rc0.d/K99pcspk /etc/rc6.d/K99pcspk

install-slackware :
	$(SED) "s|SBINDIR|$(SBINDIR)|" rc.pcspkd.template > rc.pcspkd
	$(INSTALL) -c -o root -g 0 -m 755 rc.pcspkd /etc/rc.d

uninstall-slackware :
	$(RM) /etc/rc.d/rc.pcspkd

uninstall-core :
	$(RM) $(SBINDIR)/pcspkd $(BINDIR)/pcspk

uninstall-examples :
	$(CD) $(BINDIR) && $(RM) $(EPLS)

uninstall-config :
	$(RM) $(ETCDIR)/pcspk.conf

uninstall-configs :
	$(RM) $(ETCDIR)/pcspk
