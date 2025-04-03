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

macro_rules! define_enum {
    ($(#[$attr:meta])* $vis:vis enum $name:ident {
        $($variant:ident,)*
    }) => {
        $(#[$attr])*
        $vis enum $name {
            $($variant,)*
        }

        impl $name {
            pub const COUNT: usize = Self::VARIANTS.len();
            pub const VARIANTS: [Self; [$(Self::$variant,)*].len()] = [
                $(Self::$variant,)*
            ];
        }
    };
}

macro_rules! cast {
    ($expr:expr) => {
        cast!($expr, _)
    };

    ($expr:expr, $ty:ty) => {
        if ::core::cfg!(debug_assertions) {
            ::core::convert::TryInto::try_into($expr).expect("cast failed")
        } else {
            $expr as $ty
        }
    };
}

mod adjust;
mod cards;
mod math;

fn main() {
    println!("{}", adjust::adjustments());
}

#[allow(dead_code)]
fn allow_unused() {
    let _ = cards::Deck::<0>::capacity;
}
