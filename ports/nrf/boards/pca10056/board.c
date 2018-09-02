/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include <stdbool.h>
#include "boards/board.h"
#include "nrfx.h"
#include "usb.h"

#include "shared-bindings/displayio/FourWire.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "tick.h"

displayio_fourwire_obj_t board_display_obj;

#define ILI9341_SWRESET     0x01
#define ILI9341_SLPOUT      0x11

#define ILI9341_GAMMASET    0x26
#define ILI9341_DISPOFF     0x28
#define ILI9341_DISPON      0x29

#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C

#define ILI9341_MADCTL      0x36
#define ILI9341_PIXFMT      0x3A

#define ILI9341_FRMCTR1     0xB1
#define ILI9341_DFUNCTR     0xB6

#define ILI9341_PWCTR1      0xC0
#define ILI9341_PWCTR2      0xC1
#define ILI9341_VMCTR1      0xC5
#define ILI9341_VMCTR2      0xC7
#define ILI9341_PWCTRSEQ    0xCB
#define ILI9341_PWCTRB      0xCF

#define ILI9341_GMCTRP1     0xE0
#define ILI9341_GMCTRN1     0xE1
#define ILI9341_TIMCTRA     0xE8
#define ILI9341_TIMCTRB     0xEA

#define ILI9341_ENGMCTR     0xF2
#define ILI9341_PUMP        0xF7

uint8_t display_init_sequence[] = {
	ILI9341_SWRESET, 0,
	ILI9341_DISPOFF, 0,
	ILI9341_PWCTRB,  3, 0x00, 0XC1, 0X30,
	ILI9341_TIMCTRA, 3, 0x85, 0x00, 0x78,
	ILI9341_PWCTRSEQ, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
	ILI9341_PUMP, 1, 0x20,
	ILI9341_TIMCTRB, 2, 0x00, 0x00,
	ILI9341_PWCTR1, 1, 0x23,
	ILI9341_PWCTR2, 1, 0x10,
	ILI9341_VMCTR1, 2, 0x3e, 0x28,
	ILI9341_VMCTR2, 1, 0x86,
	ILI9341_MADCTL, 1, 0x28,
	ILI9341_PIXFMT, 1, 0x55,
	ILI9341_FRMCTR1, 2, 0x00, 0x18,
	ILI9341_DFUNCTR, 3, 0x08, 0x82, 0x27,
	ILI9341_ENGMCTR, 1, 0x00,
	ILI9341_GAMMASET, 1, 0x01,
	ILI9341_GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
	ILI9341_GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
	ILI9341_SLPOUT, 0,
	ILI9341_DISPON, 0,
};

extern pin_obj_t pin_P0_11;
extern pin_obj_t pin_P1_08;
extern pin_obj_t pin_P1_09;
extern pin_obj_t pin_P0_12;
extern pin_obj_t pin_P0_07;
extern pin_obj_t pin_P0_06;

static digitalio_digitalinout_obj_t bl;

void board_init(void) {
    // Clock
    NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
    NRF_CLOCK->TASKS_LFCLKSTART = 1UL;

    usb_init();

    common_hal_digitalio_digitalinout_construct(&bl, &pin_P0_06);
    common_hal_digitalio_digitalinout_switch_to_output(&bl, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_set_value(&bl, true);

    board_display_obj.base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(&board_display_obj,
        &pin_P0_11, &pin_P1_08, &pin_P1_09, &pin_P0_12, &pin_P0_07,
        320, 240, 0, 0, 16, 0x2a, 0x2b, 0x2c);

    uint32_t i = 0;
    common_hal_displayio_fourwire_begin_transaction(&board_display_obj);
    while (i < sizeof(display_init_sequence)) {
        uint8_t *cmd = display_init_sequence + i;
        uint8_t data_size = *(cmd + 1);
        uint8_t *data = cmd + 2;
        common_hal_displayio_fourwire_send(&board_display_obj, true, cmd, 1);
        common_hal_displayio_fourwire_send(&board_display_obj, false, data, data_size);
        if (*cmd == ILI9341_SWRESET || *cmd == ILI9341_DISPOFF || *cmd == ILI9341_SLPOUT) {
            mp_hal_delay_ms(120);
        }
        i += 2 + data_size;

    }
    common_hal_displayio_fourwire_end_transaction(&board_display_obj);
}

bool board_requests_safe_mode(void) {
  return false;
}

void reset_board(void) {
    common_hal_displayio_fourwire_show(&board_display_obj, NULL);
}





