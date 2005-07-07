BEEPDIR=/usr/local/sbin
PREFIX=/usr/local
IP=127.0.0.1
PORT=11111

CC=gcc
DEFINES=-DIP=\"${IP}\" -DPORT=${PORT}

default : compile
beep-all : beep-download beep beep-install
beep-remove : beep-clean beep-delete
all : compile install
world : beep-all all

beep-download :
	wget http://www.johnath.com/beep/beep.c

beep-delete : 
	rm beep.c

beep :
	${CC} ${CFLAGS} beep.c -o beep

beep-clean : 
	rm beep

beep-install :
	cp beep ${BEEPDIR}/

beep-uninstall :
	rm ${BEEPDIR}/beep

compile :
	${CC} ${CFLAGS} ${DEFINES} freq2beep.c -o freq2beep
	${CC} ${CFLAGS} ${DEFINES} note2beep.c -o note2beep -lm 
	sed "s|IP|${IP}|;s|PORT|${PORT}|;s|BEEPDIR|${BEEPDIR}|" rc.pcspk.template > rc.pcspk
	chmod 700 rc.pcspk

clean : 
	rm freq2beep note2beep rc.pcspk

install : 
	cp freq2beep ${PREFIX}/bin/
	cp note2beep ${PREFIX}/bin/
	cp fuga ${PREFIX}/bin/
	cp fuga2 ${PREFIX}/bin/
	cp cosmic ${PREFIX}/bin/

uninstall :
	rm ${PREFIX}/freq2beep
	rm ${PREFIX}/note2beep
	rm ${PREFIX}/fuga
	rm ${PREFIX}/fuga2
	rm ${PREFIX}/cosmic