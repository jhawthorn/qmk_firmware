/* Copyright 2020 John Hawthorn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "quantum.h"

void modelh_set_leds(int num_lock, int caps_lock, int scroll_lock);

/* This is a shortcut to help you visually see your layout.
 *
 * The first section contains all of the arguments representing the physical
 * layout of the board and position of the keys.
 *
 * The second converts the arguments into a two-dimensional array which
 * represents the switch matrix.
 */

#define LAYOUT( \
    K5A,      K5B, K5C, K5D, K5E, K5F, K5G, K5H, K5I, K5J, K5K, K5L, K5M,   K5N, K5O, K5P, \
    \
    K4A, K4B, K4C, K4D, K4E, K4F, K4G, K4H, K4I, K4J, K4K, K4L, K4M, K4N,   K4O, K4P, K4Q,   K4R, K4S, K4T, K4U, \
    K3A, K3B, K3C, K3D, K3E, K3F, K3G, K3H, K3I, K3J, K3K, K3L, K3M, K3N,   K3O, K3P, K3Q,   K3R, K3S, K3T, K3U, \
    K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, K2I, K2J, K2K, K2L, K2M, K2N,                    K2O, K2P, K2Q, \
    K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, K1I, K1J, K1K, K1L,      K1M,        K1N,        K1O, K1P, K1Q, K1R, \
    K0A,      K0B,                K0C,                     K0D,      K0E,   K0F, K0G, K0H,   K0I,      K0J \
) \
{ \
/* 07 */ { K1H,   K0C,   K1G,   KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, K1L,   K0G,   K0H, K4U, K0F,   K0D   }, \
/* 06 */ { K1I,   K2N,   K1F,   K1E,   K1D,   K1C,   K1M,   K0E,   K1J,   K1K,   K2M,   K4R,   K4S, K4T, K5P,   KC_NO }, \
/* 05 */ { K2H,   K3N,   K2E,   K2D,   K2C,   K2B,   KC_NO, KC_NO, K2I,   K2J,   K2K,   K1O,   K1P, K1Q, K1R,   KC_NO }, \
/* 04 */ { K3H,   KC_NO, K3E,   K3D,   K3C,   K3B,   KC_NO, KC_NO, K3I,   K3J,   K3K,   K3R,   K3S, K3T, K3U,   K5O   }, \
/* 03 */ { K4H,   K5K,   K4E,   K4D,   K4C,   K4B,   KC_NO, KC_NO, K4I,   K4J,   K4K,   K5L,   K5M, K3Q, K3P,   K5N   }, \
/* 02 */ { K4G,   K5J,   K4F,   K5C,   K5B,   K4A,   KC_NO, K0A,   K4M,   K5I,   K4L,   K3O,   K4O, K4Q, K4P,   KC_NO }, \
/* 01 */ { K3G,   K4N,   K3F,   K5D,   K2A,   K3A,   K1A,   KC_NO, K3M,   K5H,   K3L,   K2O,   K2P, K2Q, KC_NO, KC_NO }, \
/* 00 */ { K2G,   K5F,   K2F,   K5E,   K1B,   K5A,   KC_NO, KC_NO, K5G,   KC_NO, K2L,   KC_NO, K0I, K0J, K1N,   K0B   }, \
}
/*         0      1      2      3      4      5      6      7      8      9      A      B      C    D    E      F       */
