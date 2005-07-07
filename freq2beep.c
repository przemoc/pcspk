/*
 * This code is copyright (C) Przemys³aw Pawe³czyk, 2005.
 *
 * version: 0.0.1 - draft
 */

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main() {
  int s;
  unsigned short int t[2]={0,0};
  char buf[65536]="-f 1 -l 0\0",b,m=0;
  struct sockaddr_in saddr;
  s=socket(PF_INET, SOCK_STREAM, 0);
  saddr.sin_family=AF_INET;
  saddr.sin_port=htons(PORT);
  inet_aton(IP, &saddr.sin_addr);
  if(connect(s,(const struct sockaddr *)&saddr,sizeof(struct sockaddr_in))==0) {
    while(read(0,&b,1)==1) {
      if(b>='0'&&b<='9'){b-='0';t[m]*=10;t[m]+=b;}
      else if(b==':'){m=1;t[1]=0;}
      else if(b==' '){m=0;sprintf(&buf[strlen(buf)]," -n -f%hu -l%hu\0",t[0],(t[1])?t[1]:100);t[0]=0;t[1]=0;}
    }
    if(t[0])sprintf(&buf[strlen(buf)]," -n -f%hu -l%hu\0",t[0],(t[1])?t[1]:100);
    send(s, buf, strlen(buf), 0);
  }
  close(s);
}
