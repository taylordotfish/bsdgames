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

use std::array;
use std::borrow::Borrow;

/// `SIZE_BOUND` must be at least the length of `pool`.
pub fn subsets<C, const SIZE_BOUND: usize>(pool: C) -> Subsets<C, SIZE_BOUND> {
    Subsets::new(pool)
}

#[derive(Clone)]
pub struct Subsets<C, const SIZE_BOUND: usize> {
    pool: C,
    n: usize,
}

impl<C, const N: usize> Subsets<C, N> {
    pub fn new(pool: C) -> Self {
        Self {
            pool,
            n: 0,
        }
    }
}

impl<C, const N: usize> Iterator for Subsets<C, N>
where
    C: IntoIterator + Borrow<[C::Item]>,
    C::Item: Copy,
{
    type Item = array::IntoIter<C::Item, N>;

    fn next(&mut self) -> Option<Self::Item> {
        let pool = self.pool.borrow();
        debug_assert!(pool.len() <= N, "`SIZE_BOUND` is too small");
        if self.n >= 1 << pool.len() {
            return None;
        }

        let mut comb = [pool[0]; N];
        let len = (0..pool.len())
            .filter(|i| self.n & (1 << *i) != 0)
            .enumerate()
            .map(|(ci, pi)| {
                comb[ci] = pool[pi];
            })
            .count();
        comb.rotate_right(N - len);

        let mut iter = comb.into_iter();
        if let Some(skip) = (N - len).checked_sub(1) {
            iter.nth(skip);
        }
        self.n += 1;
        Some(iter)
    }
}

pub fn combinations<C, const LEN: usize>(pool: C) -> Combinations<C, LEN> {
    Combinations::new(pool)
}

#[derive(Clone)]
pub struct Combinations<C, const LEN: usize> {
    pool: C,
    indices: [usize; LEN],
}

impl<C, const N: usize> Combinations<C, N> {
    pub fn new(pool: C) -> Self {
        let mut indices = [0; N];
        for (i, n) in indices.iter_mut().enumerate() {
            *n = i;
        }
        let last = indices.last_mut().unwrap();
        *last = last.wrapping_sub(1);
        Self {
            pool,
            indices,
        }
    }
}

impl<C, const N: usize> Iterator for Combinations<C, N>
where
    C: IntoIterator + Borrow<[C::Item]>,
    C::Item: Copy,
{
    type Item = [C::Item; N];

    fn next(&mut self) -> Option<Self::Item> {
        let pool = self.pool.borrow();
        let mut iter =
            self.indices.into_iter().enumerate().filter(|&(i, n)| {
                let candidate = n.wrapping_add(1);
                let remaining = N - 1 - i;
                candidate + remaining < pool.len()
            });
        let (start, _) = iter.next_back()?;
        let value = self.indices[start].wrapping_add(1);
        for (i, n) in &mut self.indices[start..].iter_mut().enumerate() {
            *n = value + i;
        }
        Some(self.indices.map(|i| pool[i]))
    }
}
