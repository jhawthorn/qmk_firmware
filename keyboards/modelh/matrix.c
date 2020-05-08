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

/* CS pin: F4 */
static void mcp_cs_enable(void) {
    PORTF &= ~(1 << 4);
}

static void mcp_cs_disable(void) {
    PORTF |= (1 << 4);
}

static void mcp_write(uint8_t device, uint8_t addr, uint16_t data) {
    mcp_cs_enable();

    SPI_SendByte(0b01000000 | ((device) << 1));
    SPI_SendByte(addr);
    SPI_SendByte(data & 0xff);
    SPI_SendByte((data >> 8) & 0xff);

    mcp_cs_disable();
}

static uint16_t mcp_read(uint8_t device, uint8_t addr) {
    mcp_cs_enable();

    SPI_SendByte(0b01000001 | ((device) << 1));
    SPI_SendByte(addr);

    uint8_t low_byte = SPI_ReceiveByte();
    uint8_t high_byte = SPI_ReceiveByte();

    mcp_cs_disable();

    return low_byte | (high_byte << 8);
}

static void mcp_init(void) {
    // Mode 3
    SPI_Init(
            SPI_SPEED_FCPU_DIV_8 |
            SPI_ORDER_MSB_FIRST |
            SPI_SCK_LEAD_FALLING |
            SPI_SAMPLE_TRAILING |
            SPI_MODE_MASTER
            );

    /* CS pin: F4 */
    DDRF |= (1 << 4);
    mcp_cs_disable();

    /* sleep? */

    mcp_cs_enable();
    SPI_SendByte(0b01000000);
    SPI_SendByte(MCP_IOCON);
    SPI_SendByte(0x18);
    mcp_cs_disable();

    mcp_write(0, MCP_IODIR,  0xffff);
    mcp_write(0, MCP_GPPU,   0xffff);

    /* Device 1: All pins read and 0 */
    mcp_write(1, MCP_IODIR,  0xffff);
    mcp_write(1, MCP_GPIO,   0x0000);
}

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
    mcp_init();

    // Unless hardware debouncing - Init the configured debounce routine
    debounce_init(MATRIX_ROWS);

    // This *must* be called for correct keyboard behavior
    matrix_init_quantum();
}

static void select_row(uint8_t row) {
    mcp_write(1, MCP_IODIR,  0xffff ^ (1 << row));
    mcp_write(1, MCP_GPIO,   0x0000);
}

static matrix_row_t read_cols(uint8_t row) {
    return mcp_read(0, MCP_GPIO);
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

    // Unless hardware debouncing - use the configured debounce routine
    debounce(raw_matrix, matrix, MATRIX_ROWS, matrix_has_changed);

    // This *must* be called for correct keyboard behavior
    matrix_scan_quantum();

    return matrix_has_changed;
}

