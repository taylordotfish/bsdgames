/*
 * Copyright (C) 2025 taylor.fish <contact@taylor.fish>
 *
 * This file is part of generate-score-tables.
 *
 * generate-score-tables is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * generate-score-tables is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with generate-score-tables. If not, see
 * <https://www.gnu.org/licenses/>.
 */

use super::cards::{Card, Deck, Hand, Location, NUM_DISCARDED, Rank, Suit};
use super::math;
use std::fmt::{self, Display};

#[derive(Clone, Copy)]
struct Adjustment(pub i32);

impl Default for Adjustment {
    fn default() -> Self {
        Self(-10000)
    }
}

fn adjustment(cards: [Card; NUM_DISCARDED]) -> Adjustment {
    let mut deck = Deck::FULL;
    let mut crib = Hand::new();
    for card in cards {
        deck.remove(card);
        crib.push(card);
    }
    let iter = deck.combinations::<NUM_DISCARDED>().map(|extra| {
        let (mut deck, mut crib) = (deck, crib);
        for card in extra {
            deck.remove(card);
            crib.push(card);
        }
        const LOCATION: Location = if cfg!(feature = "classic-broken") {
            Location::Hand
        } else {
            Location::Crib
        };
        deck.into_iter()
            .map(|c| u64::from(crib.score(c, LOCATION)))
            .sum::<u64>()
    });
    Adjustment(cast!(iter.sum::<u64>()))
}

#[derive(Clone, Copy, Default)]
struct TableEntry {
    pub same: Adjustment,
    pub diff: Adjustment,
}

fn table_index<R>(ranks: R) -> usize
where
    R: IntoIterator<Item = Rank>,
{
    ranks.into_iter().fold(0, |n, r| n * Rank::COUNT + r as usize)
}

pub struct Adjustments {
    table: Box<[TableEntry]>,
}

pub fn adjustments() -> Adjustments {
    let mut table = Vec::new();
    table.resize(
        Rank::COUNT.pow(NUM_DISCARDED as u32),
        TableEntry::default(),
    );

    let mut progress = 0;
    const RANK_CHOOSE_2: usize = Rank::COUNT * (Rank::COUNT - 1) / 2;
    const PROGRESS_MAX: usize = Rank::COUNT + RANK_CHOOSE_2 * 2;
    let mut increment_progress = || {
        progress += 1;
        eprint!("\r{progress}/{PROGRESS_MAX}");
    };

    for rank in Rank::VARIANTS {
        let card = Card {
            rank,
            suit: Suit::Spades,
        };
        let mut cards = [card; NUM_DISCARDED];
        for (i, card) in cards.iter_mut().enumerate() {
            card.suit = Suit::VARIANTS[i % Suit::COUNT];
        }
        let index = table_index([rank; NUM_DISCARDED]);
        table[index].diff = adjustment(cards);
        increment_progress();
    }

    for ranks in math::combinations::<_, NUM_DISCARDED>(Rank::VARIANTS) {
        let mut cards = ranks.map(|rank| Card {
            rank,
            suit: Suit::Spades,
        });
        let index = table_index(ranks);
        table[index].same = adjustment(cards);
        increment_progress();
        cards[0].suit = Suit::Hearts;
        table[index].diff = adjustment(cards);
        increment_progress();
    }
    eprint!("\r");
    Adjustments {
        table: table.into(),
    }
}

fn fmt_table_def<A>(
    f: &mut fmt::Formatter<'_>,
    name: &str,
    values: A,
) -> fmt::Result
where
    A: ExactSizeIterator<Item = Adjustment>,
{
    const WIDTH: usize = 9;
    let len = values.len();
    write!(f, "const long {name}[{len}] = {{")?;
    for (i, adj) in values.enumerate() {
        let prefix = if i % WIDTH == 0 {
            "\n    "
        } else {
            " "
        };
        let suffix = if i < len - 1 {
            ","
        } else {
            ""
        };
        write!(f, "{prefix}{}{suffix}", adj.0)?;
    }
    write!(f, "\n}};")
}

impl Display for Adjustments {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt_table_def(
            f,
            "crbescr",
            self.table.iter().copied().map(|a| a.same),
        )?;
        write!(f, "\n\n")?;
        fmt_table_def(
            f,
            "crbnescr",
            self.table.iter().copied().map(|a| a.diff),
        )
    }
}
