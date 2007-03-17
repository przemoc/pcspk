/*
 *  (C) Copyright 2005-7 Przemys³aw Pawe³czyk <przemoc@gmail.com>
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

/* PCSpk - PC-Speaker Client - version 0.0.4 */

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

#include"config.h"
#include"error.h"
#include"popts.h"
#include"str.h"

#define SIZE	4097

#define DEFAULT_LENGTH	200

void read_freq(unsigned short *buf, unsigned short *i) {
	char b;
	struct {
		unsigned short freq;
		unsigned short len;
	} sound = {0, 0};
	unsigned short *p = &sound.freq;
	while(read(0, &b, 1) == 1) {
		if(b >= '0' && b <= '9') {
			b -= '0';
			*p *= 10;
			*p += b;
		}
		else if(b == ':') {
			p = &sound.len;
			*p = 0;
		}
		else if(b == ' ') {
			p = &sound.freq;
			buf[(*i)++] = sound.freq;
			buf[(*i)++] = sound.len ? sound.len : DEFAULT_LENGTH;
			sound.freq = sound.len = 0;
		}
	}
	if(sound.freq) {
		buf[(*i)++] = sound.freq;
		buf[(*i)++] = (sound.len)?sound.len:DEFAULT_LENGTH;
	}
}

void read_note(unsigned short *buf, unsigned short *i, unsigned short shift) {
	char b;
	double spfb;
	long int bpm = 0;
	unsigned short freq;
	char t[3];
	read(0, &b, 1);
	bpm += 100 * (b - '0');
	read(0, &b, 1);
	bpm += 10 * (b - '0');
	read(0, &b, 1);
	bpm += (b - '0');
	spfb = 240000.0 / bpm;
	read(0, t, 1);
	while(read(0, t, 3) == 3) {
		switch(t[0]) {
			case 'C': b = 1; break;
			case 'd': b = 2; break;
			case 'D': b = 3; break;
			case 'e': b = 4; break;
			case 'E': b = 5; break;
			case 'F': b = 6; break;
			case 'g': b = 7; break;
			case 'G': b = 8; break;
			case 'a': b = 9; break;
			case 'A': b = 10; break;
			case 'B': b = 11; break;
			case 'H': b = 12; break;
		}
		t[1] -= '0';
		t[1] += shift;
		t[2] -='0' - 1;
		if(t[2] > 10)
			t[2] -= 'A' - ('9' + 1);
		/* exp2(...) doesn't work correctly (always equal 0) so I use exp(log(2)*...)
		freq = (t[0] != 'P')?(unsigned short) lrint(exp2(t[1] + (b - 22.0) / 12.0) * 55.0):1; */
		freq = (t[0] != 'P')?(unsigned short) lrint((exp(log(2) * (t[1] + (b - 22.0) / 12.0))) * 55.0):0;
		buf[(*i)++] = freq;
		buf[(*i)++] = (unsigned short) lrint(spfb / t[2]);
		read(0, t, 1);
	}
}

int main(int argc, char* argv[]) {
	char host[MAXHOSTNAMELEN]={HOST}, fileconf[] = {FILECONF},
		localconf[PATH_MAX], *home;
	unsigned short port = PORT, note = 0, help = 0, shift = 0;
	struct stat st;
	option_item_t opt[5] = {
		{OPT_BOOL, "help", 0, (uptr_t)&help},
		{OPT_CHAR + MAXHOSTNAMELEN, "host", 'h', (uptr_t)host},
		{OPT_BOOL, "note", 'n', (uptr_t)&note},
		{OPT_SHORTU, "port", 'p', (uptr_t)&port},
		{OPT_BOOL, "shift", 's', (uptr_t)&shift},
	};
    options_list_t opts = {opt, 5, OPT_ALLOCATED};
	int s;
	struct sockaddr_in sa;
	struct hostent *he;
	unsigned short buf[SIZE] = {HEAD};
	unsigned short i = 1;
	
	if(!stat(fileconf, &st))
		get_options_from_file(fileconf, &opts, 0);
	if(
		(home = getenv("HOME")) != NULL
		&& strcpy(localconf, home)
		&& strcpy(&localconf[strlen(localconf)], "/.pcspkrc")
		&& !stat(localconf, &st)
	)
	get_options_from_file(localconf, &opts, 0);
	get_options_from_arg(argc, argv, &opts);
	if(help || port == 0 || host[0] == 0) {
		printf(
"Usage: pcspk [OPTIONS]\n\
Sends data from stdin to PC-Speaker Server listening on HOST:PORT.\n\
\n\
Options:\n\
  -h, --host=HOST  default: %s\n\
  -p, --port=PORT  default: %hu\n\
  -n, --note  indicates that data are in note-notation (see below)\n\
              3-digit tempo plus array of space seperated TOL values:\n\
T is letter - tone: C,d(des),D,e(es),E,F,g(ges),G,a(as),A,B,H,P(pause)\n\
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
	if((he = gethostbyname(host)) == NULL)
		error(str_nohost);
	bzero(&sa, sizeof(struct sockaddr_in));
	bcopy(he->h_addr, &sa.sin_addr, he->h_length);
	sa.sin_family = he->h_addrtype;
	sa.sin_port = htons(port);
	if((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		errorp(str_nosocket, NULL);
	if(connect(s, (const struct sockaddr *) &sa, sizeof(struct sockaddr_in)) == -1)
		errorp(str_noconnect, &s);
	if(!note)
		read_freq(buf, &i);
	else
		read_note(buf, &i, shift);
	send(s, buf, i<<1, 0);
	close(s);
	exit(EXIT_SUCCESS);
}
