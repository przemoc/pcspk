HOSTNAME=127.0.0.1
PORT=11111
PREFIX=/usr/local
BEEPDIR=${PREFIX}/sbin

CC=gcc
CP=cp
RM=rm
SED=sed
CHMOD=chmod
GET=wget

default : compile
beep-all : beep-download beep beep-install
beep-remove : beep-clean beep-delete
install : install-core install-examples
all : compile install
world : beep-all all
uninstall : uninstall-core uninstall-examples

beep-download :
	${GET} http://www.johnath.com/beep/beep.c

beep-delete : 
	${RM} beep.c

beep :
	${CC} ${CFLAGS} beep.c -o beep

beep-clean : 
	${RM} beep

beep-install :
	${CP} beep ${BEEPDIR}/

beep-uninstall :
	${RM} ${BEEPDIR}/beep

compile :
	${CC} ${CFLAGS} pcspkd.c -o pcspkd
	${CC} ${CFLAGS} pcspk.c -o pcspk -lm 
	${SED} "s|PREFIX|${PREFIX}|;s|HOSTNAME|${HOSTNAME}|;s|PORT|${PORT}|" rc.pcspkd.template > rc.pcspkd
	${SED} "s|PREFIX|${PREFIX}|;s|HOSTNAME|${HOSTNAME}|;s|PORT|${PORT}|" freq2beep.template > freq2beep
	${SED} "s|PREFIX|${PREFIX}|;s|HOSTNAME|${HOSTNAME}|;s|PORT|${PORT}|" note2beep.template > note2beep
	${CHMOD} 700 rc.pcspkd
	${CHMOD} 755 freq2beep note2beep

clean : 
	${RM} pcspkd pcspk rc.pcspkd freq2beep note2beep

install-core : 
	${CP} pcspkd ${PREFIX}/sbin/
	${CP} pcspk ${PREFIX}/bin/

install-examples : 
	${CP} freq2beep ${PREFIX}/bin/
	${CP} note2beep ${PREFIX}/bin/
	${CP} fuga ${PREFIX}/bin/
	${CP} fuga2 ${PREFIX}/bin/
	${CP} cosmic ${PREFIX}/bin/

uninstall-core :
	${RM} ${PREFIX}/sbin/pcspkd
	${RM} ${PREFIX}/bin/pcspkd

uninstall-examples :
	${RM} ${PREFIX}/bin/freq2beep
	${RM} ${PREFIX}/bin/note2beep
	${RM} ${PREFIX}/bin/fuga
	${RM} ${PREFIX}/bin/fuga2
	${RM} ${PREFIX}/bin/cosmic
