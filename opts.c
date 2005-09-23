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

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include"opts.h"

void alloc_options(options_list *opts) {
	int i;
	for (i = 0; i < opts->count; i++) {
		if(opts->items[i].type >= OPT_CHAR) opts->items[i].ptr.v = calloc(opts->items[i].type - (OPT_CHAR - 1), sizeof(char));
		else switch(opts->items[i].type) {
			case OPT_BOOL:
			case OPT_SHORT:
			case OPT_SHORTU:  opts->items[i].ptr.v = calloc(1, sizeof(short)); break;
			case OPT_LONG:
			case OPT_LONGU:   opts->items[i].ptr.v = calloc(1, sizeof(long)); break;
			case OPT_FLOAT:   opts->items[i].ptr.v = calloc(1, sizeof(float)); break;
			case OPT_DOUBLE:  opts->items[i].ptr.v = calloc(1, sizeof(double)); break;
			case OPT_LDOUBLE: opts->items[i].ptr.v = calloc(1, sizeof(long double)); break;
		}
	}
	opts->alloc = OPT_ALLOCATED;
}

void free_options(options_list *opts) {
	int i;
	for(i = 0; i < opts->count; i++) free(opts->items[i].ptr.v);
	opts->alloc = OPT_AUTO_ALLOC;
}

void read_option_from_arg(option_item *item, int argc, char *argv[], int *k) {
	int s;
	if(item->type == OPT_BOOL) (*item->ptr.b)++;
	else {
		if(*k >= argc) {
			fprintf(stderr, "Missing argument for option: %s\n", item->name);
			exit(EXIT_FAILURE);
		}
		if(item->type >= OPT_CHAR) strncpy(item->ptr.c, argv[*k], item->type - OPT_CHAR);
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
			if(!s) {
	            fprintf(stderr, "Bad argument for option %s: %s\n", item->name, argv[*k]);
	            exit(EXIT_FAILURE);
			}
		}
	}
	(*k)++;
}

void read_option_from_file(option_item *item, char *arg) {
	int s;
	if(arg == NULL && item->type == OPT_BOOL) (*item->ptr.b)++;
	else if(item->type >= OPT_CHAR) strncpy(item->ptr.c, arg, item->type - OPT_CHAR);
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
		if(!s) {
            fprintf(stderr, "Bad argument for option %s: %s\n", item->name, arg);
            exit(EXIT_FAILURE);
		}
	}
}


int get_options_from_file(char *fileconf, options_list *opts) {
	int j, n = 0;
	char *temp;
	if(opts->alloc == OPT_AUTO_ALLOC) alloc_options(opts);
	if(fileconf == NULL) return 0;
	FILE *file;
	char buf[1024];
	if((file = fopen(fileconf, "r")) == NULL) {
		fprintf(stderr, "%s: %s\n", fileconf, strerror(errno));
		exit(EXIT_FAILURE);
	};
	while(fgets(buf, 1024, file) != NULL) if(buf[0] != '#') {
		temp = strtok(buf, " =\t\n");
		for(j = 0; j < opts->count && strcmp(opts->items[j].name, temp); j++) ;
		if(j >= opts->count) {
			fprintf(stderr, "Unknown option: %s\n", temp);
			exit(EXIT_FAILURE);
		}
		if((temp = strtok(NULL, " =\t\n")) != NULL || opts->items[j].type == OPT_BOOL) {
			read_option_from_file(&(opts->items[j]), temp);
			n++;
		} else {
			fprintf(stderr, "Missing argument for option: %s\n", opts->items[j].name);
			exit(EXIT_FAILURE);
		}
	}
	fclose(file);
	return n;
}

int get_options_from_arg(int argc, char *argv[], options_list *opts) {
	int i, j, k, temp, n = 0;
	if(opts->alloc == OPT_AUTO_ALLOC) alloc_options(opts);
	if(argc == 1) return 0;
	option_item **ptr = (option_item**) calloc(256, sizeof(void*));
	for(i = 0; i < opts->count; i++) ptr[opts->items[i].shortname] = &(opts->items[i]);
	i = 0;
	while(++i < argc) {
		k = i;
		if(argv[i][0] == '-') {
			if(argv[i][1] != '-') {
				j = 0; k++;
				while(argv[i][++j]) {
					if(ptr[argv[i][j]] == NULL || ptr[argv[i][j]]->ptr.v == NULL) {
						fprintf(stderr, "Unknown option: %c\n", argv[i][j]);
						exit(EXIT_FAILURE);
					}
					if(ptr[argv[i][j]]->type == OPT_BOOL) k--;
					read_option_from_arg(ptr[argv[i][j]], argc, argv, &k);
					n++;
				}
			} else {
				for(j = 0; j < opts->count && (temp = strlen(opts->items[j].name)) && strncmp(opts->items[j].name, &argv[i][2], temp); j++) ;
				if(j < opts->count) {
					if(opts->items[j].type == OPT_BOOL || (argv[i][2+temp] == '=' && *(argv[k] += 3 + temp))) {
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
			for(j = 0; j < opts->count && opts->items[j].shortname; j++) ;
			if(j >= opts->count) {
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
