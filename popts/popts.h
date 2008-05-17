/*
 * Copyright (C) 2005-8 Przemys³aw Pawe³czyk <przemoc@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 *    Neither name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/* popts - PP's Options Reader - version 0.5 */

#ifndef POPTS_H_
#define POPTS_H_

#define	OPT_BOOL	0x00
#define	OPT_SHORT	0x01
#define	OPT_SHORTU	0x02
#define	OPT_LONG	0x04
#define	OPT_LONGU	0x08
#define	OPT_FLOAT	0x10
#define	OPT_DOUBLE	0x20
#define	OPT_LDOUBLE	0x40
#define	OPT_CHAR	0x80

#define	OPT_AUTO_ALLOC	0
#define	OPT_ALLOCATED	1

#define	OPT_BUF_SIZE	1024

/// Universal pointer

typedef union {
	char *c;
	short *b;
	short *s;
	unsigned short *su;
	long *l;
	unsigned long *lu;
	float *f;
	double *d;
	long double *dl;
	void *v;
} uptr_t;

/// Option item

typedef struct {
	short type;
	char *name;
	char shortname;
	uptr_t ptr;
	char *desc;
} option_item_t;

/// List of option items

typedef struct {
	option_item_t *items;
	unsigned char count;
	unsigned char alloc;
} options_list_t;

void alloc_options(options_list_t *);
void free_options(options_list_t *);
int get_options_from_arg(int, char **, options_list_t *);
int get_options_from_file(char *, options_list_t *, char);
void print_options_description(options_list_t *);

#endif /* POPTS_H_ */
