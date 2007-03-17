include Makefile.cnf

OBJS = alloc.o error.o popts.o str_en.o
MODS = pcspk pcspkd
EPLS = fcat fuga fuga2 cosmic siemens.gawk
LIBS = -lm

CONF = $(ETCDIR)/pcspk.conf

SETTINGS = -DFILECONF=\""$(CONF)\""

default : $(MODS)
install : install-core install-examples
all : compile install
uninstall : uninstall-core uninstall-examples

%.o : %.c
	$(CC) $(CFLAGS) $(SETTINGS) -c -o $@ $<

pcspk :	$(OBJS) config.h pcspk.c 
	$(CC) $(CFLAGS) $(SETTINGS) $(OBJS) $(LIBS) $(LDFLAGS) $@.c -o $@

pcspkd :	$(OBJS) config.h pcspkd.c 
	$(CC) $(CFLAGS) $(SETTINGS) $(OBJS) $(LIBS) $(LDFLAGS) $@.c -o $@

clean : 
	$(RM) rc.pcspkd init.pcspk $(OBJS) $(MODS:=.o) pcspkd pcspk

install-core : $(MODS)
	$(INSTALL) -c -o root -g root -m 755 pcspkd $(SBINDIR)
	$(INSTALL) -c -o root -g root -m 755 pcspk $(BINDIR)

install-examples :
	$(INSTALL) -c -o root -g root -m 755 $(EPLS) $(BINDIR)

install-config :
	$(ECHO) "host = $(HOST)\nport = $(PORT)" > $(CONF)
	$(CHM) 644 $(CONF)

install-debian :
	$(SED) "s|SBINDIR|$(SBINDIR)|" init.pcspk.template > init.pcspk
	$(INSTALL) -c -o root -g root -m 755 init.pcspk /etc/init.d/pcspk
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
	$(INSTALL) -c -o root -g root -m 755 rc.pcspkd /etc/rc.d

uninstall-slackware :
	$(RM) /etc/rc.d/rc.pcspkd

uninstall-core :
	$(RM) $(SBINDIR)/pcspkd $(BINDIR)/pcspk

uninstall-examples :
	$(CD) $(BINDIR); $(RM) $(EPLS)

uninstall-config :
	$(RM) $(ETCDIR)/pcspk.conf

uninstall-configs :
	$(RM) $(ETCDIR)/pcspk
