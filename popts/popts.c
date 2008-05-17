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

/**
	\file
	\brief    popts - PP's Options Reader
	\version  0.5

	\b ChangeLog:

	** 0.5
	 - added #include guards
	 - improved code formatting

	** 0.4
	 - added description for each option and printing fuction
	 - added ignoring part beginning with = in option's name,
	   because name is printed in mentioned function

	** 0.3
	 - fixed improper behaviour on reading with \a ignore_unknown option
	 - GPL license changed to BSD license
	 - changed name (as you can see above)

	** 0.2
	 - added ignore_unknown option to \a get_options_from_file

	** 0.1
	 - initial release of: opts - Przemoc's Options Reader
**/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<ctype.h>
#include"../pbase/alloc.h"
#include"popts.h"

/// allocates suitable amount of memory for each option in list

void alloc_options(options_list_t *opts)
{
	int i;
	for (i = 0; i < opts->count; i++) {
		if (opts->items[i].type >= OPT_CHAR)
			opts->items[i].ptr.v = alloc0((opts->items[i].type - (OPT_CHAR - 1)) * sizeof(char));
		else switch(opts->items[i].type) {
			case OPT_BOOL:
			case OPT_SHORT:
			case OPT_SHORTU:  opts->items[i].ptr.v = alloc0(sizeof(short)); break;
			case OPT_LONG:
			case OPT_LONGU:   opts->items[i].ptr.v = alloc0(sizeof(long)); break;
			case OPT_FLOAT:   opts->items[i].ptr.v = alloc0(sizeof(float)); break;
			case OPT_DOUBLE:  opts->items[i].ptr.v = alloc0(sizeof(double)); break;
			case OPT_LDOUBLE: opts->items[i].ptr.v = alloc0(sizeof(long double)); break;
		}
	}
	opts->alloc = OPT_ALLOCATED;
}

/// frees options list

void free_options(options_list_t *opts)
{
	int i;
	for (i = 0; i < opts->count; i++) free(opts->items[i].ptr.v)
		;
	opts->alloc = OPT_AUTO_ALLOC;
}

/// reads k-th argument from argv table of argc items

static void read_option_from_arg(option_item_t *item, int argc, char *argv[], int *k)
{
	int s = 0;
	if (item->type == OPT_BOOL)
		(*item->ptr.b)++;
	else {
		if (*k >= argc) {
			fprintf(stderr, "Missing argument for option: %s\n", item->name);
			exit(EXIT_FAILURE);
		}
		if (item->type >= OPT_CHAR)
			strncpy(item->ptr.c, argv[*k], item->type - OPT_CHAR);
		else {
			switch(item->type) {
				case OPT_SHORT:   s = sscanf(argv[*k], "%hd", item->ptr.s); break;
				case OPT_SHORTU:  s = sscanf(argv[*k], "%hu", item->ptr.su); break;
				case OPT_LONG:    s = sscanf(argv[*k], "%ld", item->ptr.l); break;
				case OPT_LONGU:   s = sscanf(argv[*k], "%lu", item->ptr.lu); break;
				case OPT_FLOAT:   s = sscanf(argv[*k], "%f",  item->ptr.f); break;
				case OPT_DOUBLE:  s = sscanf(argv[*k], "%lf", item->ptr.d); break;
				case OPT_LDOUBLE: s = sscanf(argv[*k], "%Lf", item->ptr.dl); break;
			}
			if (!s) {
				fprintf(stderr, "Bad argument for option %s: %s\n", item->name, argv[*k]);
				exit(EXIT_FAILURE);
			}
		}
	}
	(*k)++;
}

/// reads option from string arg

static void read_option_from_str(option_item_t *item, char *arg)
{
	int s = 0;
	if (arg == NULL && item->type == OPT_BOOL)
		(*item->ptr.b)++;
	else if (item->type >= OPT_CHAR)
		strncpy(item->ptr.c, arg, item->type - OPT_CHAR);
	else {
		switch(item->type) {
			case OPT_BOOL:    s = sscanf(arg, "%hd", item->ptr.b); break;
			case OPT_SHORT:   s = sscanf(arg, "%hd", item->ptr.s); break;
			case OPT_SHORTU:  s = sscanf(arg, "%hu", item->ptr.su); break;
			case OPT_LONG:    s = sscanf(arg, "%ld", item->ptr.l); break;
			case OPT_LONGU:   s = sscanf(arg, "%lu", item->ptr.lu); break;
			case OPT_FLOAT:   s = sscanf(arg, "%f",  item->ptr.f); break;
			case OPT_DOUBLE:  s = sscanf(arg, "%lf", item->ptr.d); break;
			case OPT_LDOUBLE: s = sscanf(arg, "%Lf", item->ptr.dl); break;
		}
		if (!s) {
			fprintf(stderr, "Bad argument for option %s: %s\n", item->name, arg);
			exit(EXIT_FAILURE);
		}
	}
}


int optcmplen(const char *str, const char *opt, int *optlen)
{
	char *eq = strchr(opt, '=');
	int len = (eq != NULL) ? (int)(eq - opt) : strlen(opt);
	if (optlen != NULL)
		*optlen = len;
	return strncmp(str, opt, len);
}


int get_options_from_file(char *fileconf, options_list_t *opts, char ignore_unknown)
{
	int j, n = 0;
	char *token;
	FILE *file;
	char buf[OPT_BUF_SIZE];
	if (opts->alloc == OPT_AUTO_ALLOC)
		alloc_options(opts);
	if (fileconf == NULL)
		return 0;
	if ((file = fopen(fileconf, "r")) == NULL) {
		fprintf(stderr, "%s: %s\n", fileconf, strerror(errno));
		exit(EXIT_FAILURE);
	};
	while (fgets(buf, OPT_BUF_SIZE, file) != NULL) if (buf[0] != '#') {
		token = strtok(buf, " =\t\n");
		for (j = 0; j < opts->count && opts->items[j].shortname && optcmplen(token, opts->items[j].name, NULL); j++)
			;
		if (j >= opts->count) {
			if (!ignore_unknown) {
				fprintf(stderr, "Unknown option: %s\n", token);
				exit(EXIT_FAILURE);
			}
			continue;
		}
		if ((token = strtok(NULL, " =\t\n")) != NULL || opts->items[j].type == OPT_BOOL) {
			read_option_from_str(&(opts->items[j]), token);
			n++;
		} else {
			fprintf(stderr, "Missing argument for option: %s\n", opts->items[j].name);
			exit(EXIT_FAILURE);
		}
	}
	fclose(file);
	return n;
}



int get_options_from_arg(int argc, char *argv[], options_list_t *opts)
{
	int i, j, k, temp = 0, n = 0;
	option_item_t **ptr;
	if (opts->alloc == OPT_AUTO_ALLOC)
		alloc_options(opts);
	if (argc == 1)
		return 0;
	ptr = (option_item_t**) alloc0(256 * sizeof(void*));
	for (i = 0; i < opts->count; i++)
		ptr[(int) opts->items[i].shortname] = &(opts->items[i]);
	i = 0;
	while (++i < argc) {
		k = i;
		if (argv[i][0] == '-') {
			if (argv[i][1] != '-') {
				j = 0; k++;
				while (argv[i][++j]) {
					if (ptr[(int) argv[i][j]] == NULL || ptr[(int) argv[i][j]]->ptr.v == NULL) {
						fprintf(stderr, "Unknown option: %c\n", argv[i][j]);
						exit(EXIT_FAILURE);
					}
					if (ptr[(int) argv[i][j]]->type == OPT_BOOL)
						k--;
						read_option_from_arg(ptr[(int) argv[i][j]], argc, argv, &k);
					n++;
				}
			} else {
				for (j = 0; j < opts->count && opts->items[j].shortname && optcmplen(&argv[i][2], opts->items[j].name, &temp); j++)
					;
				if (j < opts->count) {
					if (opts->items[j].type == OPT_BOOL || (argv[i][2 + temp] == '=' && *(argv[k] += 3 + temp))) {
						read_option_from_arg(&(opts->items[j]), argc, argv, &k);
						n++;
					} else {
						fprintf(stderr, "Missing argument for option: %s\n", opts->items[j].name);
						exit(EXIT_FAILURE);
					}
				}
				else {
					fprintf(stderr, "Unknown option: %s\n", &argv[i][2]);
					exit(EXIT_FAILURE);
				}
			}
			i = k - 1;
		} else {
			for (j = 0; j < opts->count && opts->items[j].shortname; j++)
				;
			if (j >= opts->count) {
				fprintf(stderr, "Too many arguments\n");
				exit(EXIT_FAILURE);
			}
			opts->items[j].shortname = 1;
			read_option_from_arg(&(opts->items[j]), argc, argv, &k);
			n++;
		}
	}
	free(ptr);
	return n;
}



void print_options_description(options_list_t *opts)
{
	static int i = 0;
	static int max = 0;
	int j = 0;
	if (!i)
		for (i = 0; i < opts->count; i++)
			if ((j = strlen(opts->items[i].name)) > max)
				max = j;
	for (i = 0; i < opts->count; i++) {
		printf("  ");
		if (opts->items[i].shortname) {
			if (isalnum(opts->items[i].shortname))
				printf("-%c, ", opts->items[i].shortname);
			else
				printf("    ");
		}
		if (opts->items[i].name != NULL) {
			if (opts->items[i].shortname)
				printf("--%-*s  ", max, opts->items[i].name);
			else
				printf("%-*s  ", max + 6, opts->items[i].name);
		}
		else
			printf("    ");
		if (opts->items[i].desc != NULL)
			printf("%s\n", opts->items[i].desc);
	}
}
