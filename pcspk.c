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
	PCSpk - PC-Speaker Client
	@version	0.0.6
	@file
	
	@todo
		- File reading via -f option
		- More formats handling
		- Sending data in low-endian format
		- Tokenization of input
**/

#define _XOPEN_SOURCE 600
#include<math.h>
#include<netdb.h>
#include<netinet/in.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/param.h>
#include<unistd.h>
#include<curses.h>
#include<ctype.h>

#include"config.h"
#include"pbase/error.h"
#include"popts/popts.h"
#include"pbase/str.h"

/** Default size of frequencies buffer */
#define	SIZE	4097

/** Defualt length of tone */
#define	DEFAULT_LENGTH	200

char notes[] = "CdDeEFgGaABH";

/** Reads frequencies and lengths of tones from stdin */
void read_freq(unsigned short *buf, unsigned short *i)
{
	char b;
	struct {
		unsigned short freq;
		unsigned short len;
	} sound = {0, 0};
	unsigned short *p = &sound.freq;
	while (read(0, &b, 1) == 1) {
		if (b >= '0' && b <= '9') {
			b -= '0';
			*p *= 10;
			*p += b;
		}
		else if (b == ':') {
			p = &sound.len;
			*p = 0;
		}
		else if (b == ' ') {
			p = &sound.freq;
			buf[(*i)++] = sound.freq;
			buf[(*i)++] = sound.len ? sound.len : DEFAULT_LENGTH;
			sound.freq = sound.len = 0;
		}
	}
	if (sound.freq) {
		buf[(*i)++] = sound.freq;
		buf[(*i)++] = sound.len ? sound.len : DEFAULT_LENGTH;
	}
}

/** Translates given note and octave into equivalent frequency */
unsigned short note2freq(int note, int octave)
{
	char *tmp;
	if (!note || (tmp = strchr(notes, note)) == NULL)
		return 0;
	/* exp2(...) doesn't work correctly (always equal 0) so I use exp(log(2)*...) */
	return (unsigned short) lrint((exp(log(2) * (octave + ((int)(tmp - notes + 1) - 22.0) / 12.0))) * 55.0);
}

/** Reads notes from stdin */
void read_note(unsigned short *buf, unsigned short *i, unsigned short shift)
{
	char b;
	double spfb;
	long int bpm = 0;
	char t[3];
	read(0, &b, 1);
	bpm += 100 * (b - '0');
	read(0, &b, 1);
	bpm += 10 * (b - '0');
	read(0, &b, 1);
	bpm += (b - '0');
	spfb = 240000.0 / bpm;
	read(0, t, 1);
	while (read(0, t, 3) == 3) {
		t[1] -= '0';
		t[1] += shift;
		t[2] -='0' - 1;
		if (t[2] > 10)
			t[2] -= 'A' - ('9' + 1);
		buf[(*i)++] = note2freq(t[0], t[1]);
		buf[(*i)++] = (unsigned short) lrint(spfb / t[2]);
		read(0, t, 1);
	}
}

/** Sends currently pressed notes */
void play_note(int socket, unsigned short shift)
{
	const char lower[] = "zsxcfvgbnjmk,l./q2w3er5t6y7ui9o0p[=]";
	const char upper[] = "ZSXCFVGBNJMK<L>?Q@W#ER%T^Y&UI(O)P{+}";
	const char notes[] = "ABHCdDeEFgGaABHCCdDeEFgGaABHCdDeEFgG";
	const char trans[] = "///000000000000111111111111122222222";
	const unsigned short head[2] = {HEAD, 1};
	unsigned short freq;
	int b, i, o;
	char *ptr = NULL;
	/** @todo	Avoid using of curses if possible */
	initscr();
	cbreak();
	send(socket, &head, 4, 0);
	while ((b = getch())) {
		o = 3;
		if ((ptr = strchr(lower, b)) != NULL)
			i = (int)(ptr - lower);
		else if ((ptr = strchr(upper, b)) != NULL) {
			i = (int)(ptr - upper);
			o++;
		}
		if (ptr == NULL)
			b = 0;
		else {
			b = notes[i];
			o += trans[i] - '0';
		}
		freq = note2freq(b, o + shift);
		send(socket, &freq, 2, 0);
	}
}

int main(int argc, char* argv[])
{
	char host[MAXHOSTNAMELEN]={HOST}, fileconf[] = {FILECONF},
		localconf[PATH_MAX], *home;
	unsigned short port = PORT, note = 0, help = 0, shift = 0, free = 0;
	struct stat st;
	option_item_t opt[6] = {
		{OPT_BOOL,   "help",   0,  (uptr_t)&help},
		{OPT_CHAR + MAXHOSTNAMELEN, "host", 'h', (uptr_t)host},
		{OPT_BOOL,   "keyb",  'k', (uptr_t)&free},
		{OPT_BOOL,   "note",  'n', (uptr_t)&note},
		{OPT_SHORTU, "port",  'p', (uptr_t)&port},
		{OPT_BOOL,   "shift", 's', (uptr_t)&shift},
	};
	options_list_t opts = {opt, 6, OPT_ALLOCATED};
	int s;
	struct sockaddr_in sa;
	struct hostent *he;
	unsigned short buf[SIZE] = {HEAD, 0};
	unsigned short i = 2;
	
	if (!stat(fileconf, &st))
		get_options_from_file(fileconf, &opts, 0);
	if (
		(home = getenv("HOME")) != NULL
		&& strcpy(localconf, home)
		&& strcpy(&localconf[strlen(localconf)], "/.pcspkrc")
		&& !stat(localconf, &st)
	)
	get_options_from_file(localconf, &opts, 0);
	get_options_from_arg(argc, argv, &opts);
	if (help || port == 0 || host[0] == 0) {
		printf(
"Usage: pcspk [OPTIONS]\n\
Sends data from stdin to PC-Speaker Server listening on HOST:PORT.\n\
\n\
Options:\n\
  -h, --host=HOST  default: %s\n\
  -p, --port=PORT  default: %hu\n\
  -k, --keyb  free play on keyboard\n\
              use below keys on keyboard (QWERTY layout assumed) to play:\n\
                     s    f g  j k l    2 3  5 6 7    9 0  =\n\
                    z x  c v bn m , .  q w er t y u  i o p[ ]\n\
                                       /\n\
              use shift to increase octave\n\
  -n, --note  indicates that data are in note-notation (see below)\n\
              3-digit tempo plus array of space seperated TOL values:\n\
                T is letter - tone: C,d(des),D,e(es),E,F,g(ges),G,a(as),A,B,H\n\
                                or: P(pause)\n\
                O is digit - octave: 0-9\n\
                L is digit/letter - length: L = 0-9, A-Z <=> 1/(L+1)\n\
              otherway: array of space seperated freq[:length] values (default)\n\
  -s, --shift      shift up one octave\n\
      --help       prints this screen\n\
\n\
E-mail bug reports to: przemoc@gmail.com .\n\
Be sure to include the word ``pcspk'' somewhere in the ``Subject:'' field.\n", host, port);
		exit(EXIT_SUCCESS);
	}
	if ((he = gethostbyname(host)) == NULL)
		error(str_nohost);
	memset(&sa, 0, sizeof(struct sockaddr_in));
	memcpy(&sa.sin_addr, he->h_addr_list[0], he->h_length);
	sa.sin_family = he->h_addrtype;
	sa.sin_port = htons(port);
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		errorp(str_nosocket);
	if (connect(s, (const struct sockaddr *) &sa, sizeof(struct sockaddr_in)) == -1)
		errorp(str_noconnect);
	if (free)
		play_note(s, shift);
	else {
		if (!note)
			read_freq(buf, &i);
		else
			read_note(buf, &i, shift);
		send(s, buf, i << 1, 0);
	}
	close(s);
	exit(EXIT_SUCCESS);
}
