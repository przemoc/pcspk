/*
 * This code is copyright (C) Przemys³aw Pawe³czyk, 2005.
 *
 * version: 0.0.2
 */

#include<stdio.h>
#include<math.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/param.h>
#include<netdb.h>
#include<netinet/in.h>

#define SIZE	4097

int main(int argc, char* argv[]) {
    char hostname[MAXHOSTNAMELEN+1];
    unsigned short port, format;
    if(argc==1 || argc>4) {
	printf(
"Usage: pcspk [FORMAT [HOSTNAME]] PORT\n\
Sends data from stdin to PC-Speaker Server listening on HOSTNAME:PORT.\n\
\n\
where default HOSTNAME is 127.0.0.1\n\
      format = 0 - array of space seperated freq[:length] values (default)\n\
               1 - 3-digit tempo plus array of space seperated TOL values:\n\
T is letter - tone: C,d(des),D,e(es),E,F,g(ges),G,a(as),A,B,H,P(pause)\n\
O is digit - octave: 0-9\n\
L is digit/letter - length: L=0-9,A-Z <=> 1/(L+1)\n\
\n\
E-mail bug reports to: przemoc@gmail.com .\n\
Be sure to include the word ``pcspk'' somewhere in the ``Subject:'' field.\n\
",argv[0]);
	return 0;
    } else {
	if(!sscanf(argv[argc-1],"%hu",&port)) {
	    fprintf(stderr, "Invalid port\n");
	    return -1;
	}
	strncpy(hostname, (argc>3)?argv[2]:"127.0.0.1\0", MAXHOSTNAMELEN);
	if(argc>2) {
	    if(!sscanf(argv[1], "%hu", &format) || format>1) {
		fprintf(stderr, "Invalid format\n");
		return -1;
	    }
	}
	else format=0;
    }
    int s;
    struct sockaddr_in sa;
    struct hostent *he;
    unsigned short int buf[SIZE]={'SP'},l;
    if((he=gethostbyname(hostname))==NULL) {
	fprintf(stderr, "Unable to get informations about host name\n");
	return -1;
    }
    bzero(&sa, sizeof(struct sockaddr_in));
    bcopy(he->h_addr, &sa.sin_addr, he->h_length);
    sa.sin_family=he->h_addrtype;
    sa.sin_port=htons(port);
    if((s=socket(PF_INET, SOCK_STREAM, 0))==-1) {
	perror("Unable to create socket");
	return -1;
    }
    if(connect(s, (const struct sockaddr *) &sa, sizeof(struct sockaddr_in))==-1) {
	perror("Unable to connect");
	close(s);
	return -1;
    }
    unsigned short i=1;
    char b;
    if(format==0) {
	struct {
	    unsigned short freq;
	    unsigned short len;
	} sound={0,0};
	unsigned short *p=&sound.freq;
	while(read(0, &b, 1)==1) {
	    if(b>='0' && b<='9') {
		b-='0';
		*p*=10;
		*p+=b;
	    }
    	    else if(b==':') {
    		p=&sound.len;
		*p=0;
    	    }
    	    else if(b==' ') {
    		p=&sound.freq;
		buf[i++]=sound.freq;
		buf[i++]=(sound.len)?sound.len:100;
		sound.freq=sound.len=0;
    	    }
	}
	if(sound.freq) {
	    buf[i++]=sound.freq;
	    buf[i++]=(sound.len)?sound.len:100;
	}
    } else if(format==1) {
	double spfb;
	long int bpm=0;
	unsigned short freq;
	char t[3];
        read(0, &b, 1);
	bpm+=100*(b-'0');
        read(0, &b, 1);
        bpm+=10*(b-'0');
        read(0, &b, 1);
        bpm+=(b-'0');
        spfb=240000.0/bpm;
        read(0, t, 1);
	while(read(0, t, 3)==3) {
	    switch(t[0]) {
	        case 'C': b=1;break;
		case 'd': b=2;break;
		case 'D': b=3;break;
		case 'e': b=4;break;
		case 'E': b=5;break;
		case 'F': b=6;break;
		case 'g': b=7;break;
		case 'G': b=8;break;
		case 'a': b=9;break;
		case 'A': b=10;break;
		case 'B': b=11;break;
		case 'H': b=12;break;
	    }
	    t[1]-='0';
	    t[2]-='0'-1;
	    if(t[2]>10) t[2]-='A'-('9'+1);
	    freq=(t[0]!='P')?lround((exp(log(2)*(t[1]+(b-22.0)/12.0)))*55.0):1;
	    buf[i++]=freq;
	    buf[i++]=lround(spfb/t[2]);
	    read(0, t, 1);
	}
    }
    send(s, buf, i<<1, 0);
    close(s);
    return 0;
}
