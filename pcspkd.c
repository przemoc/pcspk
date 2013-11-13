/*
 *  (C) Copyright 2005-8 Przemyslaw Pawelczyk <przemoc@gmail.com>
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

/**
	PCSpk - PC-Speaker Server
	@version	0.0.6
	@file

	This source code contains fragments inspired by
	http://johnath.com/beep/beep.c (beep-1.2.2)
	
	@todo
		- Add support for more platforms and OSs
		- Add option for logging played music
*/

#define _BSD_SOURCE

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
#if defined(__linux__)
#	include<linux/kd.h>
#elif defined(__DragonFly__) || defined(__FreeBSD__)
#	include<sys/kbio.h>
#	define	__bsd__	1
#elif defined(__NetBSD__) || defined(__OpenBSD__)
#	include<dev/wscons/wsdisplay_usl_io.h>
#	define	__bsd__	1
#endif

#include"config.h"
#include"pbase/error.h"
#include"popts/popts.h"
#include"pbase/str.h"

#define	DEFAULT_FREE_LENGTH	1000

/** Intel 8254's cycles per second */
#ifndef CLOCK_TICK_RATE
#define	CLOCK_TICK_RATE	1193182
#endif

#define	MAX_PERD 0xffff
#define	MIN_FREQ ((CLOCK_TICK_RATE / MAX_PERD) + (CLOCK_TICK_RATE % MAX_PERD > 0))

/** Console file descriptor kept for gracefully quiting */
int console_fd = -1;

/** Signal handler for turning off the speaker on interrupts */
void handle_signal(int signum)
{
	switch(signum) {
	case SIGINT:
	case SIGTERM:
		if (console_fd >= 0) {
			ioctl(console_fd, KDMKTONE, 0);
			close(console_fd);
			exit(signum);
		} else {
			exit(signum);
		}
	}
}

/** Plays beep and sleeps while playing */
void play_beep(unsigned short freq, unsigned short length)
{
	static unsigned short perd = MAX_PERD;
	if (ioctl(
		console_fd,
		KDMKTONE,
		freq ?
			(freq >= MIN_FREQ ?
				(perd = CLOCK_TICK_RATE / freq) + (length << 16) :
				MAX_PERD) :
#ifdef __linux__
			0
#else
			MAX_PERD + (1 << 16)
#endif
	) < 0)
		errorp("ioctl");
	usleep(1000 * length);
}

/** Plays beep without sleeping */
void play_beep_nosleep(unsigned short freq)
{
	static unsigned short perd = MAX_PERD;
	if (ioctl(
		console_fd,
		KDMKTONE,
		freq ?
			(freq >= MIN_FREQ ?
				(perd = CLOCK_TICK_RATE / freq) + (DEFAULT_FREE_LENGTH << 16) :
				MAX_PERD) :
#ifdef __linux__
			0
#else
			MAX_PERD + (1 << 16)
#endif
	) < 0)
		errorp("ioctl");
}

int main(int argc, char* argv[])
{
	char host[MAXHOSTNAMELEN] = {HOST}, fileconf[] = {FILECONF};
	unsigned short port = PORT, help = 0;
	struct stat st;
	option_item_t opt[3] = {
		{OPT_BOOL, "help", 0, (uptr_t)&help},
		{OPT_CHAR + MAXHOSTNAMELEN, "host", 'h', (uptr_t)host},
		{OPT_SHORTU, "port", 'p', (uptr_t)&port},
	};
	options_list_t opts = {opt, 3, OPT_ALLOCATED};
	int s, t, fd;
	struct sockaddr_in sa;
	struct hostent *he;
	unsigned short buf[2];
#ifdef __linux__
	char con0[] = "/dev/console";
	char con1[] = "/proc/self/fd/0";
	char *con = geteuid() ? con1 : con0;
#endif
#ifdef __bsd__
	char con[] = "/dev/console";
#endif

	if (!stat(fileconf, &st))
		get_options_from_file(fileconf, &opts, 0);
	get_options_from_arg(argc, argv, &opts);
	if (help || port == 0 || host[0] == 0) {
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
Be sure to include the word ``pcspk'' somewhere in the ``Subject:'' field.\n",
		host, port);
		exit(EXIT_SUCCESS);
	}
	if ((he = gethostbyname(host)) == NULL) 
		error(str_nohost);
	memset(&sa, 0, sizeof(struct sockaddr_in));
	memcpy(&sa.sin_addr, he->h_addr, he->h_length);
	sa.sin_family = he->h_addrtype;
	sa.sin_port = htons(port);
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		errorp(str_nosocket);
	if (bind(s, (struct sockaddr*) &sa, sizeof(struct sockaddr)) == -1)
		errorp(str_nosocketbind);
	listen(s, 1);
#ifndef DEBUG
	/* Orphan the child. */
	switch (fork()) {
	case 0:
		break;
	case -1:
		errorp(str_noprocess);
		exit(EXIT_FAILURE);
		break;
	default:
		close(s);
		/* Don't send signals to children. */
		_exit(EXIT_SUCCESS);
	}
	/* Detach from controlling tty (if you're superuser).*/
	if (geteuid() ? 0 : setsid() < 0)
		errorp(str_nosetsid);
	/* Set the umask. */
	umask(0);
	/* Set the root directory as the working directory. */
	if (chdir("/") == -1)
		errorp(str_nocdroot);
#endif /* DEBUG */
	if ((console_fd = open(con, O_WRONLY | O_NOCTTY)) == -1)
		errorp(str_noconsolewriting);
	play_beep(0, 0);
	close(console_fd);
#ifndef DEBUG
	/* Redirect standard io (except stdin if you're not superuser. */
	if (!geteuid()) {
		fd = open("/dev/null", O_RDONLY | O_NOCTTY);
		if (fd != 0) {
			dup2(fd, 0);
			close(fd);
		}
	}
	fd = open("/dev/null", O_WRONLY | O_NOCTTY);
	if (fd != 1) {
		dup2(fd, 1);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY | O_NOCTTY);
	if (fd != 2) {
		dup2(fd, 2);
		close(fd);
	}
#endif /* DEBUG */
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	while ((t = accept(s, NULL, NULL)) != -1) {
		read(t, buf, 4);
		if (buf[0] == HEAD) {
			if ((console_fd = open(con, O_WRONLY | O_NOCTTY)) == -1)
				errorp(str_noconsolewriting);
			if (!buf[1])	/* PCSpk Protocol 0 - sleep mode */
				while (read(t, buf, 4) == 4)
					play_beep(buf[0], buf[1]);
			else	/* PCSpk Protocol 1 - interactive mode */
				while (read(t, buf, 2) == 2) 
					play_beep_nosleep(buf[0]);
			close(console_fd);
		}
		close(t);
	}
	errorp(str_nosocketaccept);
}
