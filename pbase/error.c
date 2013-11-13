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
	Error-handling functions
	@file
**/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

/** Prints a message to stderr and exits */
void error(const char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

/** Prints a system error message to stderr and exits */
void errorp(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
