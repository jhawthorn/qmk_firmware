/*


Copyright 2013 Oleg Kostyuk <cub.uanic@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include "wait.h"
#include "action_layer.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "debounce.h"

#include "../../lib/lufa/LUFA/Drivers/Peripheral/SPI.h"

#include QMK_KEYBOARD_H

static const pin_t col_pins[] = MATRIX_COL_PINS;

static matrix_row_t raw_matrix[MATRIX_ROWS];  // raw values
static matrix_row_t matrix[MATRIX_ROWS];      // debounced values

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }
__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }
__attribute__((weak)) void matrix_init_user(void) {}
__attribute__((weak)) void matrix_scan_user(void) {}

matrix_row_t matrix_get_row(uint8_t row) {
    return matrix[row];
}

/* MCP23S17 registers */
#define MCP_GPIO   0x12
#define MCP_GPPU   0x0C
#define MCP_IOCON  0x0A
#define MCP_IODIR  0x00

#define CLOCK_PIN B3
#define LATCH_PIN E6
#define DATA_PIN  B7

void matrix_print(void) {
    print("\nr/c 0123456789ABCDEF\n");

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row);
        print(": ");
        print_bin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

void matrix_init(void) {
    setPinOutput(CLOCK_PIN);
    setPinOutput(LATCH_PIN);
    setPinOutput(DATA_PIN);

    // Unless hardware debouncing - Init the configured debounce routine
    debounce_init(MATRIX_ROWS);

    // This *must* be called for correct keyboard behavior
    matrix_init_quantum();
}

static void select_row(uint8_t row) {
    for(int i = 0; i < 8; i++) {
        writePin(DATA_PIN, i != row);
        writePinHigh(CLOCK_PIN);
        writePinLow(CLOCK_PIN);
    }
    writePinHigh(LATCH_PIN);
    writePinLow(LATCH_PIN);
}

static void deselect_rows(void) {
    select_row(8);
}

static matrix_row_t read_cols(uint8_t row) {
    matrix_row_t val = 0;
    for (int i = 0; i < MATRIX_COLS; i++) {
        pin_t pin = col_pins[i];
        val |= readPin(pin) ? 0 : (1 << i);
    }
    return val;
}

uint8_t matrix_scan(void) {
    bool matrix_has_changed = false;

    for (int i = 0; i < MATRIX_ROWS; i++) {
        select_row(i);
        matrix_row_t temp = read_cols(i);

        if (raw_matrix[i] != temp) {
            matrix_has_changed = true;
            raw_matrix[i] = temp;
        }
    }
    deselect_rows();

    // Unless hardware debouncing - use the configured debounce routine
    debounce(raw_matrix, matrix, MATRIX_ROWS, matrix_has_changed);

    // This *must* be called for correct keyboard behavior
    matrix_scan_quantum();

    return matrix_has_changed;
}

void modelh_set_leds(int num_lock, int caps_lock, int scroll_lock) {
}

bool led_update_kb(led_t led_state) {
    bool res = led_update_user(led_state);
    if(res) {
        modelh_set_leds(led_state.num_lock, led_state.caps_lock, led_state.scroll_lock);
    }
    return res;
}
