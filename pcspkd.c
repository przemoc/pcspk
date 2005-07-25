/*
 * This code is copyright (C) Przemys³aw Pawe³czyk, 2005.
 *
 * version: 0.0.2
 */

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/param.h>
#include<netdb.h>
#include<netinet/in.h>

#define SIZE	65535

int main(int argc, char* argv[]) {
    char hostname[MAXHOSTNAMELEN+1];
    unsigned short port;
    if(argc==1 || argc>3) {
	printf(
"Usage: pcspkd [HOSTNAME] PORT\n\
Starts PC-Speaker Server listening on HOSTNAME:PORT.\n\
\n\
where default HOSTNAME is 127.0.0.1.\n\
\n\
E-mail bug reports to: przemoc@gmail.com .\n\
Be sure to include the word ``pcspk'' somewhere in the ``Subject:'' field.\n\
",argv[0]);
	return 0;
    } else {
	strncpy(hostname, (argc>2)?argv[1]:"127.0.0.1\0", MAXHOSTNAMELEN);
	if(!sscanf(argv[argc-1], "%hu", &port)) {
	    fprintf(stderr, "Invalid port\n");
	    return -1;
	}
    }
    int s, t;
    struct sockaddr_in sa;
    struct hostent *he;
    char cmd[SIZE]="beep -f1 -l0\0";
    unsigned short l, buf[2];
    pid_t pid, sid;
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
    if(bind(s, (struct sockaddr*) &sa, sizeof(struct sockaddr))==-1) {
	perror("Unable to bind socket");
	close(s);
	return -1;
    }
    listen(s,1);
    if((pid=fork())==-1) {
	perror("Unable to create process");
	close(s);
	return -1;
    }
    else if(pid>0) {
	close(s);
	return 0;
    }
    umask(0);
    if((sid=setsid())==-1) {
	perror("Unable to create new session");
	close(s);
	return -1;
    }
    if(chdir("/")==-1) {
	perror("Unable to change current directory to root /");
	close(s);
	return -1;
    }
    while(1) {
	if((t=accept(s, NULL, NULL))==-1) {
	    perror("Unable to accept socket");
	    close(s);
	    return -1;
	}
	read(t,buf,2);
	if(buf[0]=='SP'){ 
	    cmd[12]=0;
	    while(read(t,buf,4)==4) {
		l=strlen(cmd);
	        snprintf(&cmd[l], SIZE-l, " -n -f%hu -l%hu\0", buf[0], buf[1]);
	    }
	    system(cmd);
	}
	close(t);
    }
    close(s);
    return 0;
}
