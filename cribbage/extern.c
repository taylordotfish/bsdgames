/*	$NetBSD: extern.c,v 1.7 2003/08/07 09:37:10 agc Exp $	*/

/*-
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)extern.c	8.1 (Berkeley) 5/31/93";
#else
__RCSID("$NetBSD: extern.c,v 1.7 2003/08/07 09:37:10 agc Exp $");
#endif
#endif /* not lint */

#include <curses.h>

#include "deck.h"
#include "cribbage.h"

BOOLEAN	explain		= FALSE;	/* player mistakes explained */
BOOLEAN	iwon		= FALSE;	/* if comp won last game */
BOOLEAN	quiet		= FALSE;	/* if suppress random mess */
BOOLEAN	rflag		= FALSE;	/* if all cuts random */
BOOLEAN	realshuf	= FALSE;	/* realistic shuffling */

char	explan[128];			/* explanation */

int	cgames		= 0;		/* number games comp won */
int	cskunk		= 0;		/* number games comp skunked */
int	cdblskunk	= 0;		/* number games comp 2x skunked */
int	cscore		= 0;		/* comp score in this game */
int	gamecount	= 0;		/* number games played */
int	glimit		= LGAME;	/* game playe to glimit */
int	knownum		= 0;		/* number of cards we know */
int	pgames		= 0;		/* number games player won */
int	pskunk		= 0;		/* number games player skunked */
int	pdblskunk	= 0;		/* number games player 2x skunked */
int	pscore		= 0;		/* player score in this game */

CARD	chand[FULLHAND];		/* computer's hand */
CARD	crib[CINHAND];			/* the crib */
CARD	deck[CARDS];			/* a deck */
CARD	known[CARDS];			/* cards we have seen */
CARD	phand[FULLHAND];		/* player's hand */
CARD	turnover;			/* the starter */

WINDOW	*Compwin;			/* computer's hand window */
WINDOW	*Msgwin;			/* messages for the player */
WINDOW	*Playwin;			/* player's hand window */
WINDOW	*Tablewin;			/* table window */
