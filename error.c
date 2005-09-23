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

/* error.c - part of pcspk 0.0.3 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void error(const char *msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

void errorp(const char *msg, int *sockfd) {
	perror(msg);
	if(sockfd!=NULL && *sockfd>0) close(*sockfd);
	exit(EXIT_FAILURE);
}
