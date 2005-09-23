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
 
/* opts - Przemoc's Options Reader - version 0.1 */

#define OPT_BOOL	0x00
#define	OPT_SHORT	0x01
#define	OPT_SHORTU	0x02
#define OPT_LONG	0x04
#define OPT_LONGU	0x08
#define	OPT_FLOAT	0x10
#define OPT_DOUBLE	0x20
#define OPT_LDOUBLE	0x40
#define OPT_CHAR	0x80

#define OPT_AUTO_ALLOC	0
#define	OPT_ALLOCATED	1

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
} uptr;

typedef struct {
	unsigned char type;
	char *name;
	char shortname;
	uptr ptr;
} option_item;

typedef struct {
	option_item *items;
	unsigned char count;
	unsigned char alloc;
} options_list;

void alloc_options(options_list *);
void free_options(options_list *);
void read_option_from_arg(option_item *, int, char **, int *);
void read_option_from_file(option_item *, char *);
int get_options_from_arg(int, char **, options_list *);
int get_options_from_file(char *, options_list *);
