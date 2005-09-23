include Makefile.cnf

PCSPK-DIR=${ETCDIR}/pcspk
PCSPK-CNF=${PCSPK-DIR}/pcspk.conf
PCSPKD-CNF=${PCSPK-DIR}/pcspkd.conf

default : compile
install : install-core install-examples
all : compile install
uninstall : uninstall-core uninstall-examples

compile :
	${CC} ${CFLAGS} -DFILECONF=\""${PCSPKD-CNF}\"" -DHOST=\""${HOST}\"" -DPORT=${PORT} error.c opts.c pcspkd.c -o pcspkd
	${CC} ${CFLAGS} -DFILECONF=\""${PCSPK-CNF}\"" -DHOST=\""${HOST}\"" -DPORT=${PORT} error.c opts.c pcspk.c -lm -o pcspk
	${SED} "s|SBINDIR|${SBINDIR}|" rc.pcspkd.template > rc.pcspkd
	${CHM} 700 rc.pcspkd

clean : 
	${RM} pcspkd pcspk

install-core : 
	${CP} pcspkd ${SBINDIR}/
	${CP} pcspk ${BINDIR}/

install-examples : 
	${CP} fcat ${BINDIR}/
	${CP} fuga ${BINDIR}/
	${CP} fuga2 ${BINDIR}/
	${CP} cosmic ${BINDIR}/

install-configs :
	${MKD} ${PCSPK-DIR}
	${ECHO} "host = ${HOST}\nport = ${PORT}" > ${PCSPKD-CNF}
	${CHM} 600 ${PCSPKD-CNF}
	${ECHO} "host = ${HOST}\nport = ${PORT}" > ${PCSPK-CNF}
	${CHM} 644 ${PCSPK-CNF}

uninstall-core :
	${RM} ${SBINDIR}/pcspkd
	${RM} ${BINDIR}/pcspk

uninstall-examples :
	${RM} ${BINDIR}/fcat
	${RM} ${BINDIR}/fuga
	${RM} ${BINDIR}/fuga2
	${RM} ${BINDIR}/cosmic

uninstall-old-examples :
	${RM} ${PREFIX}/bin/freq2beep
	${RM} ${PREFIX}/bin/note2beep
	${RM} ${PREFIX}/bin/fuga
	${RM} ${PREFIX}/bin/fuga2
	${RM} ${PREFIX}/bin/cosmic

uninstall-configs :
	${RM} ${PCSPK-CNF}
	${RM} ${PCSPKD-CNF}
