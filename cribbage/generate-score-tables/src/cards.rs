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

use super::math::{self, Combinations, Subsets};
use std::array::IntoIter as ArrayIter;
use std::borrow::{Borrow, BorrowMut};
use std::fmt::{self, Debug, Display};
use std::iter;
use std::ops::{Deref, DerefMut};

#[derive(Clone, Copy, Eq, PartialEq)]
pub struct Card {
    pub rank: Rank,
    pub suit: Suit,
}

impl Display for Card {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}{}", self.rank, self.suit)
    }
}

impl Debug for Card {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if f.alternate() {
            write!(f, "Card({self})")
        } else {
            write!(f, "{self}")
        }
    }
}

impl Card {
    const COUNT: usize = Rank::COUNT * Suit::COUNT;

    pub fn value(self) -> u8 {
        (self.rank as u8 + 1).min(10)
    }
}

define_enum! {
    #[repr(u8)]
    #[derive(Clone, Copy, Eq, PartialEq, Ord, PartialOrd)]
    pub enum Rank {
        Ace,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Ten,
        Jack,
        Queen,
        King,
    }
}

impl Display for Rank {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", &"A23456789TJQK"[*self as usize..][..1])
    }
}

impl Debug for Rank {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if f.alternate() {
            write!(f, "Rank({self})")
        } else {
            write!(f, "{self}")
        }
    }
}

define_enum! {
    #[derive(Clone, Copy, Eq, PartialEq)]
    pub enum Suit {
        Spades,
        Hearts,
        Diamonds,
        Clubs,
    }
}

impl Display for Suit {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", &"SHDC"[*self as usize..][..1])
    }
}

impl Debug for Suit {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if f.alternate() {
            write!(f, "Suit({self})")
        } else {
            write!(f, "{self}")
        }
    }
}

#[derive(Clone, Copy)]
pub struct Deck<const CAPACITY: usize = { Card::COUNT }> {
    cards: [Card; CAPACITY],
    len: usize,
}

impl<const N: usize> Deck<N> {
    pub const CAPACITY: usize = N;

    pub const fn new() -> Self {
        Self {
            cards: [Card {
                rank: Rank::Ace,
                suit: Suit::Spades,
            }; N],
            len: 0,
        }
    }

    pub const fn capacity(&self) -> usize {
        Self::CAPACITY
    }

    pub fn len(&self) -> usize {
        self.len
    }

    pub fn cards(&self) -> &[Card] {
        &self.cards[..self.len]
    }

    pub fn cards_mut(&mut self) -> &mut [Card] {
        &mut self.cards[..self.len]
    }

    pub fn push(&mut self, card: Card) {
        self.cards[self.len] = card;
        self.len += 1;
    }

    pub fn remove_at(&mut self, i: usize) -> Card {
        self[i..].rotate_left(1);
        self.len -= 1;
        self.cards[self.len]
    }

    pub fn remove(&mut self, card: Card) -> bool {
        if let Some(i) = self.iter().position(|c| *c == card) {
            self.remove_at(i);
            true
        } else {
            false
        }
    }

    pub fn subsets(self) -> Subsets<Self, N> {
        math::subsets(self)
    }

    pub fn combinations<const LEN: usize>(self) -> Combinations<Self, LEN> {
        math::combinations(self)
    }
}

impl Deck {
    pub const FULL: Self = {
        let mut deck = Self::new();
        let mut i = 0;
        while i < Self::CAPACITY {
            deck.cards[i] = Card {
                rank: Rank::VARIANTS[i / Suit::COUNT],
                suit: Suit::VARIANTS[i % Suit::COUNT],
            };
            i += 1;
        }
        deck.len = Self::CAPACITY;
        deck
    };
}

impl<const N: usize> Display for Deck<N> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let sep = match f.alternate() {
            false => ", ",
            true => " ",
        };
        write!(f, "[")?;
        for (i, card) in self.into_iter().enumerate() {
            if i > 0 {
                write!(f, "{sep}")?;
            }
            write!(f, "{card}")?;
        }
        write!(f, "]")
    }
}

impl<const N: usize> Debug for Deck<N> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if f.alternate() {
            write!(f, "Deck({self})")
        } else {
            write!(f, "{self}")
        }
    }
}

impl<const N: usize> Deref for Deck<N> {
    type Target = [Card];

    fn deref(&self) -> &Self::Target {
        self.cards()
    }
}

impl<const N: usize> DerefMut for Deck<N> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        self.cards_mut()
    }
}

impl<const N: usize> Borrow<[Card]> for Deck<N> {
    fn borrow(&self) -> &[Card] {
        self
    }
}

impl<const N: usize> BorrowMut<[Card]> for Deck<N> {
    fn borrow_mut(&mut self) -> &mut [Card] {
        self
    }
}

impl<const N: usize> IntoIterator for Deck<N> {
    type IntoIter = iter::Take<ArrayIter<Card, N>>;
    type Item = Card;

    fn into_iter(self) -> Self::IntoIter {
        self.cards.into_iter().take(self.len())
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum Location {
    Hand,
    Crib,
}

/// Number of cards dealt to each player.
pub const NUM_DEALT: usize = 6;

/// Number of cards discarded by each player.
pub const NUM_DISCARDED: usize = 2;

/// Minimum number of cards in a run.
pub const MIN_RUN: usize = 3;

pub type Hand = Deck<NUM_DEALT>;

impl Hand {
    pub fn fifteens(self) -> u8 {
        let count = self
            .subsets()
            .map(|cards| cards.map(|c| c.value()).sum())
            .filter(|s: &u8| *s == 15)
            .count();
        cast!(count * 2)
    }

    pub fn runs(self) -> u8 {
        let mut counts = Rank::VARIANTS.map(|_| 0);
        for card in self {
            counts[card.rank as usize] += 1;
        }
        let score: usize = counts
            .split(|c| *c == 0)
            .filter(|run| run.len() >= MIN_RUN)
            .map(|run| run.len() * run.iter().product::<usize>())
            .sum();
        cast!(score)
    }

    pub fn pairs(self) -> u8 {
        let count = self
            .combinations::<2>()
            .filter(|[c1, c2]| c1.rank == c2.rank)
            .count();
        cast!(count * 2)
    }

    fn score_ranks(self) -> u8 {
        self.fifteens() + self.runs() + self.pairs()
    }

    pub fn score(self, starter: Card, location: Location) -> u8 {
        let suit = self[0].suit;
        let flush = if !self.iter().all(|c| c.suit == suit) {
            0
        } else if starter.suit == suit {
            cast!(self.len() + 1)
        } else if location == Location::Hand {
            cast!(self.len())
        } else {
            0
        };

        let mut jack = starter;
        jack.rank = Rank::Jack;
        let nobs = self.into_iter().any(|c| c == jack) as u8;

        let mut full = self;
        full.push(starter);
        flush + nobs + full.score_ranks()
    }
}
