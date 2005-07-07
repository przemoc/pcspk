/*
 * This code is copyright (C) Przemys³aw Pawe³czyk, 2005.
 *
 * version: 0.0.1 - draft
 */
 
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(int argc, char *argv[]) {
  char buf[65536]="-f 1 -l 0\0";
  long int freq,bpm=0;
  int s;
  double spfb;
  char t[3],i;
  struct sockaddr_in saddr;
  s=socket(PF_INET, SOCK_STREAM, 0);
  saddr.sin_family=AF_INET;
  saddr.sin_port=htons(PORT);
  inet_aton(IP, &saddr.sin_addr);
  if(connect(s,(const struct sockaddr *)&saddr,sizeof(struct sockaddr_in))==0) {
    read(0,&i,1);
    bpm+=100*(i-'0');
    read(0,&i,1);
    bpm+=10*(i-'0');
    read(0,&i,1);
    bpm+=(i-'0');
    spfb=240000.0/bpm;
    read(0,&i,1);
    while(read(0,t,3)==3) {
      switch(t[0]) {
      case 'C':   i=1;break;
      case 'd':   i=2;break;
      case 'D':   i=3;break;
      case 'e':   i=4;break;
      case 'E':   i=5;break;    
      case 'F':   i=6;break;
      case 'g':   i=7;break;
      case 'G':   i=8;break;    
      case 'a':   i=9;break;
      case 'A':   i=10;break;
      case 'B':   i=11;break;    
      case 'H':   i=12;break;
      }
      t[1]-='0';
      t[2]-='0'-1;
      if(t[2]>10)t[2]-='A'-('9'+1);
      freq=(t[0]!='P')?lround((exp(log(2)*(t[1]+(i-22.0)/12.0)))*55.0):1;
      sprintf(&buf[strlen(buf)]," -n -f%ld -l%ld\0",freq,lround(spfb/t[2]));
      read(0,t,1);
    }
    send(s, buf, strlen(buf), 0);
  }
  close(s);
  return 0;
}
