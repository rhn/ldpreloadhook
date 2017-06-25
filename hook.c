/*
 * ldpreloadhook - a quick open/close/ioctl/read/write/free/strcmp/strncmp symbol hooker
 * Copyright (C) 2012-2013 Pau Oliva Fora <pof@eslack.org>
 *
 * Based on vsound 0.6 source code:
 *   Copyright (C) 2004 Nathan Chantrell <nsc@zorg.org>
 *   Copyright (C) 2003 Richard Taylor <r.taylor@bcs.org.uk>
 *   Copyright (C) 2000,2001 Erik de Castro Lopo <erikd@zip.com.au>
 *   Copyright (C) 1999 James Henstridge <james@daa.com.au>
 * Based on esddsp utility that is part of esound:
 *   Copyright (C) 1998, 1999 Manish Singh <yosh@gimp.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * 1) Compile:
 *   gcc -fPIC -c -o hook.o hook.c
 *   gcc -shared -o hook.so hook.o -ldl
 * 2) Usage:
 *   LD_PRELOAD="./hook.so" command
 *   LD_PRELOAD="./hook.so" SPYFILE="/file/to/spy" command
 *   LD_PRELOAD="./hook.so" SPYFILE="/file/to/spy" DELIMITER="***" command
 * to spy the content of buffers free'd by free(), set the environment
 * variable SPYFREE, for example:
 *   LD_PRELOAD="./hook.so" SPYFREE=1 command
 * to spy the strings compared using strcmp(), set the environment
 * variable SPYSTR, for example:
 *   LD_PRELOAD="./hook.so" SPYSTR=1 command
 * to spy memcpy() buffers set the env variable SPYMEM
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <SDL/SDL_events.h>
#include <stdbool.h>


int g_obvio=0;
#define DPRINTF(format, args...)	if (!g_obvio) { g_obvio=1; fprintf(stderr, format, ## args); g_obvio=0; }

#ifndef RTLD_NEXT
#define RTLD_NEXT ((void *) -1l)
#endif

#ifdef __FreeBSD__
typedef unsigned long request_t;
#else
typedef int request_t;
#endif

typedef void (*sighandler_t)(int);


static void _libhook_init() __attribute__ ((constructor));
static void _libhook_init() {   
	/* causes segfault on some android, uncomment if you need it */
	//unsetenv("LD_PRELOAD");
	printf("[] Hooking!\n");
}

static void print_key(SDL_keysym *sym) {
	DPRINTF("sc: %d kc: %d mod: %x u: %x", sym->scancode, sym->sym, sym->mod, sym->unicode);
}

bool ismod(SDL_keysym *sym) {
	return (sym->scancode == 133 && sym->sym == 311 && sym->mod == 0);
}

void update_key(SDL_keysym *sym) {
	switch (sym->scancode) {
		case 31:
			sym->scancode = 21;
			sym->sym = 121;
			sym->unicode = 0x79;
			break;
		case 32:
			sym->scancode = 45;
			sym->sym = 107;
			sym->unicode = 0x6b;
			break;
		case 33:
			sym->scancode = 30;
			sym->sym = 117;
			sym->unicode = 0x75;
			break;
		case 45:
			sym->scancode = 43;
			sym->sym = 104;
			sym->unicode = 0x68;
			break;
		case 46:
			sym->scancode = 60;
			sym->sym = 46;
			sym->unicode = 0x2e;
			break;
		case 47:
			sym->scancode = 46;
			sym->sym = 108;
			sym->unicode = 0x6c;
			break;
		case 59:
			sym->scancode = 56;
			sym->sym = 98;
			sym->unicode = 0x62;
			break;
		case 60:
			sym->scancode = 44;
			sym->sym = 106;
			sym->unicode = 0x6a;
			break;
		case 61:
			sym->scancode = 57;
			sym->sym = 110;
			sym->unicode = 0x6e;
			break;
		default:
			break;
	}
}


void handle_ev(SDL_Event *event) {
	static bool has_mod = false;
	switch (event->type) {
		case SDL_KEYDOWN:
			/*DPRINTF("DOWN ");
			print_key(&event->key.keysym);
			DPRINTF("\n");*/
			if (ismod(&event->key.keysym)) {
				DPRINTF("MOD_\n");
				has_mod = true;
			} else if (has_mod) {
	//			DPRINTF("UPDATE ");
				update_key(&event->key.keysym);
		//		print_key(&event->key.keysym);
			//	DPRINTF("\n");
			}
			break;
		case SDL_KEYUP:
/*			DPRINTF("UP ");
			print_key(&event->key.keysym);
			DPRINTF("\n");*/
			if (ismod(&event->key.keysym)) {
				has_mod = false;
				DPRINTF("MOD^\n");
			} else if (has_mod) {
		//		DPRINTF("UPDATED ");
				update_key(&event->key.keysym);
			//	print_key(&event->key.keysym);
				//DPRINTF("\n");
			}
			break;
		default:
			break;
	}
}


int SDL_WaitEvent(SDL_Event *event) {
	static int (*func) (SDL_Event*) = NULL;
	if (!func) {
		DPRINTF("Hookeing\n");
		func = (int (*) (SDL_Event*)) dlsym (RTLD_NEXT, "SDL_WaitEvent");
		DPRINTF("Hookeed\n");
	}
	int ret = func(event);
	handle_ev(event);
	return ret;
}

int SDL_PollEvent(SDL_Event *event) {
	static int (*func) (SDL_Event*) = NULL;
	if (!func) {
		DPRINTF("Hookeing\n");
		func = (int (*) (SDL_Event*)) dlsym (RTLD_NEXT, "SDL_PollEvent");
		DPRINTF("Hookeed\n");
	}
	int ret = func(event);
	handle_ev(event);
	return ret;
}
