/*
 *  (C) Copyright 2005 Przemys³aw Pawe³czyk <przemoc@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* PCSpk - PC-Speaker Server - version 0.0.3 */

#include<fcntl.h>
#include<netdb.h>
#include<netinet/in.h>
#include<signal.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<sys/param.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<linux/kd.h>

#include"error.h"
#include"opts.h"

#define SIZE	65535

/* --- begin of modified fragments from http://johnath.com/beep/beep.c (beep-1.2.2) */

#ifndef CLOCK_TICK_RATE
#define CLOCK_TICK_RATE 1193180
#endif

/* Momma taught me never to use globals, but we need something the signal
   handlers can get at.*/
int console_fd = -1;

/* If we get interrupted, it would be nice to not leave the speaker beeping in
   perpetuity. */
void handle_signal(int signum) {
	switch(signum) {
	case SIGINT:
	case SIGTERM:
		if(console_fd >= 0) {
			/* Kill the sound, quit gracefully */
			ioctl(console_fd, KIOCSOUND, 0);
			close(console_fd);
			exit(signum);
		} else {
			/* Just quit gracefully */
			exit(signum);
		}
	}
}

inline void play_beep(unsigned short freq, unsigned short length) {
	/* Beep */
	if(freq) if(ioctl(console_fd, KIOCSOUND, CLOCK_TICK_RATE/freq) < 0) errorp("ioctl", &console_fd);
	/* Look ma, I'm not ansi C compatible! */
	usleep(1000 * length);             /* wait...	*/
	ioctl(console_fd, KIOCSOUND, 0);   /* stop beep  */
}

/* --- end of fragments */

int main(int argc, char* argv[]) {
	char host[MAXHOSTNAMELEN] = {HOST}, fileconf[] = {FILECONF}, localconf[PATH_MAX], *home;
	unsigned short port = PORT, help = 0;
	struct stat st;
	option_item opt[3] = {
		{OPT_BOOL, "help", 0, (uptr)&help},
		{OPT_CHAR + MAXHOSTNAMELEN, "host", 'h', (uptr)host},
		{OPT_SHORTU, "port", 'p', (uptr)&port},
	};
	options_list opts = {opt, 3, OPT_ALLOCATED};
	if(!stat(fileconf, &st)) get_options_from_file(fileconf, &opts);	
	get_options_from_arg(argc, argv, &opts);	
	if(help || port == 0 || host[0] == 0) {
		printf(
"Usage: pcspkd [OPTIONS]\n\
Starts PC-Speaker Server listening on HOSTNAME:PORT.\n\
\n\
Options:\n\
  -h, --host=HOST  default: %s\n\
  -p, --port=PORT  default: %hu\n\
      --help       prints this screen\n\
\n\
E-mail bug reports to: przemoc@gmail.com .\n\
Be sure to include the word ``pcspk'' somewhere in the ``Subject:'' field.\n", host, port);
		exit(EXIT_SUCCESS);
	}
	int s, t;
	struct sockaddr_in sa;
	struct hostent *he;
	unsigned short l, buf[2];
	char con0[] = {"/dev/console"};
	char con1[] = {"/proc/self/fd/0"};
	char *con = (geteuid())?con1:con0;
	pid_t pid;
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	if((he = gethostbyname(host)) == NULL) error("Unable to get informations about host name\n");
	bzero(&sa, sizeof(struct sockaddr_in));
	bcopy(he->h_addr, &sa.sin_addr, he->h_length);
	sa.sin_family = he->h_addrtype;
	sa.sin_port = htons(port);
	if((s = socket(PF_INET, SOCK_STREAM, 0)) == -1) errorp("Unable to create socket", NULL);
	if(bind(s, (struct sockaddr*) &sa, sizeof(struct sockaddr)) == -1) errorp("Unable to bind socket", &s);
	listen(s, 1);
	if((pid = fork()) == -1) errorp("Unable to create process", &s);
	else if(pid > 0) {
		close(s);
		exit(EXIT_SUCCESS);
	}
	umask(0);
	if(chdir("/") == -1) errorp("Unable to change current directory to root /", &s);
	while((t = accept(s, NULL, NULL)) != -1) {
		read(t, buf, 2);
		if(buf[0] == 'SP') {
			if((console_fd = open(con, O_WRONLY)) == -1) errorp("Could not open console for writing", &s);
			while(read(t, buf, 4) == 4) play_beep(buf[0], buf[1]);
			close(console_fd);
		}
		close(t);
	}
	errorp("Unable to accept socket", &s);
}
