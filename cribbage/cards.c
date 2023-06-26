/*	$NetBSD: cards.c,v 1.7 2003/08/07 09:37:08 agc Exp $	*/

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
static char sccsid[] = "@(#)cards.c	8.1 (Berkeley) 5/31/93";
#else
__RCSID("$NetBSD: cards.c,v 1.7 2003/08/07 09:37:08 agc Exp $");
#endif
#endif /* not lint */

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "deck.h"
#include "cribbage.h"


#define RIFFLE_CUT_N 1
#define RIFFLE_CUT_D 3
#define OVERHAND_MAX 13

/*
 * Initialize a deck of cards to contain one of each type.
 */
void
makedeck(d)
	CARD    d[];
{
	int i, j, k;

	i = time(NULL);
	i = ((i & 0xff) << 8) | ((i >> 8) & 0xff) | 1;
	srand(i);
	k = 0;
	for (i = 0; i < RANKS; i++)
		for (j = 0; j < SUITS; j++) {
			d[k].suit = j;
			d[k++].rank = i;
		}
}

static void riffleonce(CARD []);

static void
riffleonce(d)
	CARD d[];
{
	static const int cn = RIFFLE_CUT_N;
	static const int cd = RIFFLE_CUT_D;

	CARD tmp[CARDS];
	int mid;
	int i;
	int lo, hi;
	int r;

	memcpy(tmp, d, sizeof(CARD) * CARDS);
	mid = CARDS * cn / cd + (rand() >> 4) % (CARDS * cn / cd);
	lo = mid;
	hi = CARDS;

	for (i = CARDS; i > 0;) {
		r = (rand() >> 4) % (lo + hi - mid);
		if (r < lo) {
			d[--i] = tmp[--lo];
		} else {
			d[--i] = tmp[--hi];
		}
	}
}

static void overhandonce(CARD []);

static void
overhandonce(d)
	CARD d[];
{
	CARD tmp[CARDS];
	int i;
	int r;

	memcpy(tmp, d, sizeof(CARD) * CARDS);
	for (i = CARDS; i > 0; i -= r) {
		r = 1 + (rand() >> 4) % (i < OVERHAND_MAX ? i : OVERHAND_MAX);
		memcpy(d + CARDS - i, tmp + i - r, sizeof(CARD) * r);
	}
}

static void cutonce(CARD []);

static void
cutonce(d)
	CARD d[];
{
	CARD tmp[CARDS];
	int r;

	memcpy(tmp, d, sizeof(CARD) * CARDS);
	r = MINCUT + (rand() >> 4) % (CARDS - MINCUT * 2);
	memcpy(d, tmp + r, sizeof(CARD) * (CARDS - r));
	memcpy(d + CARDS - r, tmp, sizeof(CARD) * r);
}

static void realisticshuffle(CARD []);

static void
realisticshuffle(d)
	CARD d[];
{
	typedef enum {
		SHUFRIFFLE,
		SHUFOVERHAND,
		SHUFMAX
	} SHUFACT;

	static const SHUFACT acts[] = {SHUFRIFFLE, SHUFRIFFLE, SHUFOVERHAND};
	static const SHUFACT actpts[] = {2, 1};

	int count;
	int runlen;
	SHUFACT act;
	SHUFACT prev;

	count = 10 + (rand() >> 4) % 6;
	runlen = 0;
	for (prev = SHUFMAX; count > 0; prev = act) {
		if (runlen >= 6)
			act = (act + 1) % SHUFMAX;
		else
			act = acts[(rand() >> 4) % 3];
		if (act == prev)
			runlen += actpts[act];
		else
			runlen = 0;
		count -= actpts[act];

		switch (act) {
		case SHUFRIFFLE:
			riffleonce(d);
			break;
		case SHUFOVERHAND:
			overhandonce(d);
			overhandonce(d);
			break;
		default:
			break;
		}
	}
	cutonce(d);
}

static void uniformshuffle(CARD []);

/*
 * Given a deck of cards, shuffle it -- i.e. randomize it
 * see Knuth, vol. 2, page 125.
 */
static void
uniformshuffle(d)
	CARD d[];
{
	int j, k;
	CARD c;

	for (j = CARDS; j > 0; --j) {
		k = (rand() >> 4) % j;		/* random 0 <= k < j */
		c = d[j - 1];			/* exchange (j - 1) and k */
		d[j - 1] = d[k];
		d[k] = c;
	}
}

void
shuffle(d)
	CARD d[];
{
	if (realshuf)
		realisticshuffle(d);
	else
		uniformshuffle(d);
	logdeck(d);
}

/*
 * return true if the two cards are equal...
 */
int
eq(a, b)
	CARD a, b;
{
	return ((a.rank == b.rank) && (a.suit == b.suit));
}

/*
 * is_one returns TRUE if a is in the set of cards b
 */
int
is_one(a, b, n)
	CARD a;
	const CARD b[];
	int n;
{
	int i;

	for (i = 0; i < n; i++)
		if (eq(a, b[i]))
			return (TRUE);
	return (FALSE);
}

/*
 * remove the card a from the deck d of n cards
 */
void
cremove(a, d, n)
	CARD a, d[];
	int n;
{
	int i, j;

	for (i = j = 0; i < n; i++)
		if (!eq(a, d[i]))
			d[j++] = d[i];
	if (j < n)
		d[j].suit = d[j].rank = EMPTY;
}

/*
 * move card at index src to dst
 */
void
cmove(d, src, dst)
	CARD d[];
	int src;
	int dst;
{
	CARD c;
	c = d[src];
	if (dst < src)
		memmove(d + dst + 1, d + dst, sizeof(CARD) * (src - dst));
	else
		memmove(d + src, d + src + 1, sizeof(CARD) * (dst - src));
	d[dst] = c;
}

/*
 * sorthand:
 *	Sort a hand of n cards
 */
void
sorthand(h, n)
	CARD h[];
	int n;
{
	CARD *cp, *endp;
	CARD c;

	for (endp = &h[n]; h < endp - 1; h++)
		for (cp = h + 1; cp < endp; cp++)
			if ((cp->rank < h->rank) ||
			    (cp->rank == h->rank && cp->suit < h->suit)) {
				c = *h;
				*h = *cp;
				*cp = c;
			}
}
