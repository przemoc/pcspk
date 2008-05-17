/*
 *  (C) Copyright 2005-8 Przemys³aw Pawe³czyk <przemoc@gmail.com>
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
	\file
	\brief	Allocation-related functions
**/

#include<stdlib.h>
#include<stdio.h>
#include"str.h"

/// Allocates dynamic memory

void *alloc(size_t size)
{
	void *ptr;
	if ((ptr = malloc(size)) == NULL) {
		fprintf(stderr, str_nomem);
		exit(EXIT_FAILURE);
	}
	return ptr;
}

/// Allocates dynamic memory and fills it with zeros

void *alloc0(size_t size)
{
	void *ptr;
	if ((ptr = calloc(1, size)) == NULL) {
		fprintf(stderr, str_nomem);
		exit(EXIT_FAILURE);
	}
	return ptr;
}

/// Changes size of memory block

void *ralloc(void *ptr, size_t size)
{
	void *nptr;
	if ((nptr = realloc(ptr, size)) == NULL) {
		fprintf(stderr, str_nomem);
		exit(EXIT_FAILURE);
	}
	return nptr;
}
