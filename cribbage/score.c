/*	$NetBSD: score.c,v 1.10 2003/08/07 09:37:10 agc Exp $	*/

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
static char sccsid[] = "@(#)score.c	8.1 (Berkeley) 5/31/93";
#else
__RCSID("$NetBSD: score.c,v 1.10 2003/08/07 09:37:10 agc Exp $");
#endif
#endif /* not lint */

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deck.h"
#include "cribbage.h"

/*
 * the following arrays give the sum of the scores of the (50 2)*48 = 58800
 * hands obtainable for the crib given the two cards whose ranks index the
 * array.  the two arrays are for the case where the suits are equal and
 * not equal respectively
 */
const long crbescr[169] = {
    -10000, 263247, 270303, 323739, 339189, 252549, 242073, 244623, 239679,
    234855, 247695, 228855, 222471, -10000, -10000, 404235, 287127, 340917,
    258939, 253941, 251115, 245439, 241467, 254307, 235467, 229083, -10000,
    -10000, -10000, 325407, 380049, 253437, 258207, 254391, 244149, 245895,
    258735, 239895, 233511, -10000, -10000, -10000, -10000, 413517, 294207,
    247857, 255171, 249303, 245691, 258531, 239691, 233307, -10000, -10000,
    -10000, -10000, -10000, 419097, 379257, 340593, 339405, 415281, 428121,
    409281, 402897, -10000, -10000, -10000, -10000, -10000, -10000, 327807,
    290271, 330087, 227907, 240747, 221907, 215523, -10000, -10000, -10000,
    -10000, -10000, -10000, -10000, 399903, 258111, 221223, 237615, 218775,
    212391, -10000, -10000, -10000, -10000, -10000, -10000, -10000, -10000,
    292095, 255207, 233115, 217827, 211443, -10000, -10000, -10000, -10000,
    -10000, -10000, -10000, -10000, -10000, 287055, 264963, 211191, 208359,
    -10000, -10000, -10000, -10000, -10000, -10000, -10000, -10000, -10000,
    -10000, 297939, 244167, 202851, -10000, -10000, -10000, -10000, -10000,
    -10000, -10000, -10000, -10000, -10000, -10000, 295707, 254391, -10000,
    -10000, -10000, -10000, -10000, -10000, -10000, -10000, -10000, -10000,
    -10000, -10000, 235551, -10000, -10000, -10000, -10000, -10000, -10000,
    -10000, -10000, -10000, -10000, -10000, -10000, -10000
};

const long crbnescr[169] = {
    325272, 260772, 267828, 321264, 336714, 250074, 239598, 242148, 237204,
    232380, 246348, 226380, 219996, -10000, 342528, 401760, 284652, 338442,
    256464, 251466, 248640, 242964, 238992, 252960, 232992, 226608, -10000,
    -10000, 362280, 322932, 377574, 250962, 255732, 251916, 241674, 243420,
    257388, 237420, 231036, -10000, -10000, -10000, 360768, 411042, 291732,
    245382, 252696, 246828, 243216, 257184, 237216, 230832, -10000, -10000,
    -10000, -10000, 528768, 416622, 376782, 338118, 336930, 412806, 426774,
    406806, 400422, -10000, -10000, -10000, -10000, -10000, 369864, 325332,
    287796, 327612, 225432, 239400, 219432, 213048, -10000, -10000, -10000,
    -10000, -10000, -10000, 359160, 397428, 255636, 218748, 236268, 216300,
    209916, -10000, -10000, -10000, -10000, -10000, -10000, -10000, 331320,
    289620, 252732, 231768, 215352, 208968, -10000, -10000, -10000, -10000,
    -10000, -10000, -10000, -10000, 325152, 284580, 263616, 208716, 205884,
    -10000, -10000, -10000, -10000, -10000, -10000, -10000, -10000, -10000,
    321240, 296592, 241692, 200376, -10000, -10000, -10000, -10000, -10000,
    -10000, -10000, -10000, -10000, -10000, 348600, 294360, 253044, -10000,
    -10000, -10000, -10000, -10000, -10000, -10000, -10000, -10000, -10000,
    -10000, 308664, 233076, -10000, -10000, -10000, -10000, -10000, -10000,
    -10000, -10000, -10000, -10000, -10000, -10000, 295896
};

static const int ichoose2[5] = { 0, 0, 2, 6, 12 };
static int pairpoints, runpoints;		/* Globals from pairuns. */

/*
 * scorehand:
 *	Score the given hand of n cards and the starter card.
 *	n must be <= 4
 */
int
scorehand(hand, starter, n, crb, do_explain)
	const CARD hand[];
	CARD starter;
	int n;
	BOOLEAN crb;		/* true if scoring crib */
	BOOLEAN do_explain;	/* true if must explain this hand */
{
	int i, k;
	int score;
	BOOLEAN flag;
	CARD h[(CINHAND + 1)];
	char buf[32];

	explan[0] = '\0';	/* initialize explanation */
	score = 0;
	flag = TRUE;
	k = hand[0].suit;
	for (i = 0; i < n; i++) {	/* check for flush */
		flag = (flag && (hand[i].suit == k));
		if (hand[i].rank == JACK)	/* check for his nibs */
			if (hand[i].suit == starter.suit) {
				score++;
				if (do_explain)
					strcat(explan, "His Nobs");
			}
		h[i] = hand[i];
	}

	if (flag && n >= CINHAND) {
		if (do_explain && explan[0] != '\0')
			strcat(explan, ", ");
		if (starter.suit == k) {
			score += 5;
			if (do_explain)
				strcat(explan, "Five-flush");
		} else
			if (!crb) {
				score += 4;
				if (do_explain && explan[0] != '\0')
					strcat(explan, ", Four-flush");
				else
					strcpy(explan, "Four-flush");
			}
	}
	if (do_explain && explan[0] != '\0')
		strcat(explan, ", ");
	h[n] = starter;
	sorthand(h, n + 1);	/* sort by rank */
	i = 2 * fifteens(h, n + 1);
	score += i;
	if (do_explain) {
		if (i > 0) {
			(void) sprintf(buf, "%d points in fifteens", i);
			strcat(explan, buf);
		} else
			strcat(explan, "No fifteens");
	}
	i = pairuns(h, n + 1);
	score += i;
	if (do_explain) {
		if (i > 0) {
			(void) sprintf(buf, ", %d points in pairs, %d in runs",
			    pairpoints, runpoints);
			strcat(explan, buf);
		} else
			strcat(explan, ", No pairs/runs");
	}
	return (score);
}

/*
 * fifteens:
 *	Return number of fifteens in hand of n cards
 */
int
fifteens(hand, n)
	const CARD hand[];
	int n;
{
	int *sp, *np;
	int i;
	const CARD *endp;
	static int sums[15], nsums[15];

	np = nsums;
	sp = sums;
	i = 16;
	while (--i) {
		*np++ = 0;
		*sp++ = 0;
	}
	for (endp = &hand[n]; hand < endp; hand++) {
		i = hand->rank + 1;
		if (i > 10)
			i = 10;
		np = &nsums[i];
		np[-1]++;	/* one way to make this */
		sp = sums;
		while (i < 15) {
			*np++ += *sp++;
			i++;
		}
		sp = sums;
		np = nsums;
		i = 16;
		while (--i)
			*sp++ = *np++;
	}
	return sums[14];
}

/*
 * pairuns returns the number of points in the n card sorted hand
 * due to pairs and runs
 * this routine only works if n is strictly less than 6
 * sets the globals pairpoints and runpoints appropriately
 */
int
pairuns(h, n)
	const CARD h[];
	int n;
{
	int i;
	int runlength, runmult, lastmult, curmult;
	int mult1, mult2, pair1, pair2;
	BOOLEAN run;

	run = TRUE;
	runlength = 1;
	mult1 = 1;
	pair1 = -1;
	mult2 = 1;
	pair2 = -1;
	curmult = runmult = 1;
	for (i = 1; i < n; i++) {
		lastmult = curmult;
		if (h[i].rank == h[i - 1].rank) {
			if (pair1 < 0) {
				pair1 = h[i].rank;
				mult1 = curmult = 2;
			} else {
				if (h[i].rank == pair1) {
					curmult = ++mult1;
				} else {
					if (pair2 < 0) {
						pair2 = h[i].rank;
						mult2 = curmult = 2;
					} else {
						curmult = ++mult2;
					}
				}
			}
			if (i == (n - 1) && run) {
				runmult *= curmult;
			}
		} else {
			curmult = 1;
			if (h[i].rank == h[i - 1].rank + 1) {
				if (run) {
					++runlength;
				} else {
							/* only if old short */
					if (runlength < 3) {
						run = TRUE;
						runlength = 2;
						runmult = 1;
					}
				}
				runmult *= lastmult;
			} else {
							/* if just ended */
				if (run)
					runmult *= lastmult;
				run = FALSE;
			}
		}
	}
	pairpoints = ichoose2[mult1] + ichoose2[mult2];
	runpoints = (runlength >= 3 ? runlength * runmult : 0);
	return (pairpoints + runpoints);
}

/*
 * pegscore tells how many points crd would get if played after
 * the n cards in tbl during pegging
 */
int
pegscore(crd, tbl, n, sum)
	CARD crd;
	const CARD tbl[];
	int n, sum;
{
	BOOLEAN got[RANKS];
	int i, j, scr;
	int k, lo, hi;

	sum += VAL(crd.rank);
	if (sum > 31)
		return (-1);
	if (sum == 31 || sum == 15)
		scr = 2;
	else
		scr = 0;
	if (!n)
		return (scr);
	j = 1;
	while ((crd.rank == tbl[n - j].rank) && (n - j >= 0))
		++j;
	if (j > 1)
		return (scr + ichoose2[j]);
	if (n < 2)
		return (scr);
	lo = hi = crd.rank;
	for (i = 0; i < RANKS; i++)
		got[i] = FALSE;
	got[crd.rank] = TRUE;
	k = -1;
	for (i = n - 1; i >= 0; --i) {
		if (got[tbl[i].rank])
			break;
		got[tbl[i].rank] = TRUE;
		if (tbl[i].rank < lo)
			lo = tbl[i].rank;
		if (tbl[i].rank > hi)
			hi = tbl[i].rank;
		for (j = lo; j <= hi; j++)
			if (!got[j])
				break;
		if (j > hi)
			k = hi - lo + 1;
	}
	if (k >= 3)
		return (scr + k);
	else
		return (scr);
}

/*
 * adjust takes a two card hand that will be put in the crib
 * and returns an adjusted normalized score for the number of
 * points such a crib will get.
 *
 * the score is multiplied by 58800 to minimize loss of precision
 * and should be divided by that number later.
 */
int
adjust(cb)
	const CARD cb[];
{
	long scr;
	int i, c0, c1;

	c0 = cb[0].rank;
	c1 = cb[1].rank;
	if (c0 > c1) {
		i = c0;
		c0 = c1;
		c1 = i;
	}
	if (cb[0].suit != cb[1].suit)
		scr = crbnescr[RANKS * c0 + c1];
	else
		scr = crbescr[RANKS * c0 + c1];
	if (scr <= 0) {
		printf("\nADJUST: internal error %d %d\n", c0, c1);
		exit(93);
	}
	return scr;
}
