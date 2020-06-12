/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/busio/UART.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"


#include "mpconfigport.h"
#include "lib/utils/interrupt_char.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/translate.h"
#include "supervisor/shared/tick.h"

void spi_reset(void) {

}

static spi_device_handle_t spi;
#define LCD_HOST    SPI2_HOST
#define DMA_CHAN    0

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
        const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi,
        const mcu_pin_obj_t * miso) {

    esp_err_t ret;

    self->clock_pin = clock;
    self->MOSI_pin = mosi;
    self->MISO_pin = miso;

    printf("1\r\n");
    spi_bus_config_t buscfg={
        .miso_io_num=miso->number,
        .mosi_io_num=mosi->number,
        .sclk_io_num=clock->number,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
    };

    printf("2\r\n");
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=SPI_MASTER_FREQ_80M,           //Clock out at 10 MHz
        .mode=0,                                //SPI mode 0
        .queue_size=10,                          //We want to be able to queue 7 transactions at a time
    };

    printf("3\r\n");
    ret = spi_bus_initialize(LCD_HOST, &buscfg, DMA_CHAN);
    ESP_ERROR_CHECK(ret);
    printf("4, %d\r\n", ret);

    printf("5\r\n");
    ret = spi_bus_add_device(LCD_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
    printf("6, %d\r\n", ret);

    // uint8_t sercom_index;
    // uint32_t clock_pinmux = 0;
    // bool mosi_none = mosi == NULL;
    // bool miso_none = miso == NULL;
    // uint32_t mosi_pinmux = 0;
    // uint32_t miso_pinmux = 0;
    // uint8_t clock_pad = 0;
    // uint8_t mosi_pad = 0;
    // uint8_t miso_pad = 0;
    // uint8_t dopo = 255;

    // if (sercom == NULL) {
    //     mp_raise_ValueError(translate("Invalid pins"));
    // }

    // // Set up SPI clocks on SERCOM.
    // samd_peripherals_sercom_clock_init(sercom, sercom_index);

    #if defined(MICROPY_HW_APA102_SCK) && defined(MICROPY_HW_APA102_MOSI) && !CIRCUITPY_BITBANG_APA102
    // // if we're re-using the dotstar sercom, make sure it is disabled or the init will fail out
    // hri_sercomspi_clear_CTRLA_ENABLE_bit(sercom);
    #endif
    // if (spi_m_sync_init(&self->spi_desc, sercom) != ERR_NONE) {
    //     mp_raise_OSError(MP_EIO);
    // }

    // Pads must be set after spi_m_sync_init(), which uses default values from
    // the prototypical SERCOM.
    // hri_sercomspi_write_CTRLA_DOPO_bf(sercom, dopo);
    // hri_sercomspi_write_CTRLA_DIPO_bf(sercom, miso_pad);

    // Always start at 250khz which is what SD cards need. They are sensitive to
    // SPI bus noise before they are put into SPI mode.
    // uint8_t baud_value = samd_peripherals_spi_baudrate_to_baud_reg_value(250000);
    // if (spi_m_sync_set_baudrate(&self->spi_desc, baud_value) != ERR_NONE) {
    //     // spi_m_sync_set_baudrate does not check for validity, just whether the device is
    //     // busy or not
    //     mp_raise_OSError(MP_EIO);
    // }

    // gpio_set_pin_direction(clock->number, GPIO_DIRECTION_OUT);
    // gpio_set_pin_pull_mode(clock->number, GPIO_PULL_OFF);
    // gpio_set_pin_function(clock->number, clock_pinmux);
    // claim_pin(clock);
    // self->clock_pin = clock->number;

    // if (mosi_none) {
    //     self->MOSI_pin = NO_PIN;
    // } else {
    //     gpio_set_pin_direction(mosi->number, GPIO_DIRECTION_OUT);
    //     gpio_set_pin_pull_mode(mosi->number, GPIO_PULL_OFF);
    //     gpio_set_pin_function(mosi->number, mosi_pinmux);
    //     self->MOSI_pin = mosi->number;
    //     claim_pin(mosi);
    // }

    // if (miso_none) {
    //     self->MISO_pin = NO_PIN;
    // } else {
    //     gpio_set_pin_direction(miso->number, GPIO_DIRECTION_IN);
    //     gpio_set_pin_pull_mode(miso->number, GPIO_PULL_OFF);
    //     gpio_set_pin_function(miso->number, miso_pinmux);
    //     self->MISO_pin = miso->number;
    //     claim_pin(miso);
    // }

    // spi_m_sync_enable(&self->spi_desc);
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    // never_reset_sercom(self->spi_desc.dev.prvt);

    never_reset_pin(self->clock_pin);
    never_reset_pin(self->MOSI_pin);
    never_reset_pin(self->MISO_pin);
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->clock_pin == NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
    // allow_reset_sercom(self->spi_desc.dev.prvt);

    // spi_m_sync_disable(&self->spi_desc);
    // spi_m_sync_deinit(&self->spi_desc);
    reset_pin(self->clock_pin);
    reset_pin(self->MOSI_pin);
    reset_pin(self->MISO_pin);
    self->clock_pin = NULL;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
        uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    // If the settings are already what we want then don't reset them.
    // if (hri_sercomspi_get_CTRLA_CPHA_bit(hw) == phase &&
    //     hri_sercomspi_get_CTRLA_CPOL_bit(hw) == polarity &&
    //     hri_sercomspi_read_CTRLB_CHSIZE_bf(hw) == ((uint32_t)bits - 8) &&
    //     hri_sercomspi_read_BAUD_BAUD_bf(hw) == baud_reg_value) {
    //     return true;
    // }

    // Disable, set values (most or all are enable-protected), and re-enable.
    // spi_m_sync_disable(&self->spi_desc);
    // hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);

    // hri_sercomspi_write_CTRLA_CPHA_bit(hw, phase);
    // hri_sercomspi_write_CTRLA_CPOL_bit(hw, polarity);
    // hri_sercomspi_write_CTRLB_CHSIZE_bf(hw, bits - 8);
    // hri_sercomspi_write_BAUD_BAUD_bf(hw, baud_reg_value);
    // hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);

    // spi_m_sync_enable(&self->spi_desc);
    // hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);

    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
    // CRITICAL_SECTION_ENTER()
        if (!self->has_lock) {
            grabbed_lock = true;
            self->has_lock = true;
        }
    // CRITICAL_SECTION_LEAVE();
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
        const uint8_t *data, size_t len) {
    if (len == 0) {
        return true;
    }

    //printf("%s: len %d\r\n", __func__, len);

   uint8_t *next_chunk = (uint8_t *) data;

    while (len > 0) {
        size_t chunk_size = MIN(len, SOC_SPI_MAXIMUM_BUFFER_SIZE);
        uint8_t *chunk = next_chunk;

        spi_transaction_t t = { 0 };
        t.length = chunk_size*8;                 //Len is in bytes, transaction length is in bits.
        t.tx_buffer = chunk;               //Data

        spi_device_transmit(spi, &t);  //Transmit!

        next_chunk += chunk_size;
        len -= chunk_size;
    }

    //int ret;
    //assert(ret==ESP_OK);            //Should have had no issues.


    // int32_t status;
    //if (len >= 16) {
        // status = sercom_dma_write(self->spi_desc.dev.prvt, data, len);
    //} else {
        // struct io_descriptor *spi_io;
        // spi_m_sync_get_io_descriptor(&self->spi_desc, &spi_io);
        // status = spi_io->write(spi_io, data, len);
    //}
    return true; // Status is number of chars read or an error code < 0.
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
        uint8_t *data, size_t len, uint8_t write_value) {
    if (len == 0) {
        return true;
    }

    printf("%s: len %d\r\n", __func__, len);

    //int ret;
    spi_transaction_t t = { 0 };
    t.length = len*8;                 //Len is in bytes, transaction length is in bits.
    t.rx_buffer = data;               //Data

    spi_device_transmit(spi, &t);  //Transmit!
    //assert(ret==ESP_OK);            //Should have had no issues.


    // int32_t status;
    //if (len >= 16) {
        // status = sercom_dma_read(self->spi_desc.dev.prvt, data, len, write_value);
    //} else {
        // self->spi_desc.dev.dummy_byte = write_value;

        // struct io_descriptor *spi_io;
        // spi_m_sync_get_io_descriptor(&self->spi_desc, &spi_io);

        // status = spi_io->read(spi_io, data, len);
    //}
    return true; // Status is number of chars read or an error code < 0.
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, uint8_t *data_out, uint8_t *data_in, size_t len) {
    if (len == 0) {
        return true;
    }

    printf("%s: len %d\r\n", __func__, len);

    //int ret;
    spi_transaction_t t = { 0 };
    t.length = len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer = data_out;               //Data
    t.rx_buffer = data_in;
    spi_device_transmit(spi, &t);  //Transmit!
    //assert(ret==ESP_OK);            //Should have had no issues.


    // int32_t status;
    //if (len >= 16) {
        // status = sercom_dma_transfer(self->spi_desc.dev.prvt, data_out, data_in, len);
   // } else {
        // struct spi_xfer xfer;
        // xfer.txbuf = data_out;
        // xfer.rxbuf = data_in;
        // xfer.size = len;
        // status = spi_m_sync_transfer(&self->spi_desc, &xfer);
   // }
    return true; // Status is number of chars read or an error code < 0.
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t* self) {
    // return samd_peripherals_spi_baud_reg_value_to_baudrate(hri_sercomspi_read_BAUD_reg(self->spi_desc.dev.prvt));
    return 40*1000*1000;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t* self) {
    // void * hw = self->spi_desc.dev.prvt;
    // return hri_sercomspi_get_CTRLA_CPHA_bit(hw);
    return 0;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t* self) {
    // void * hw = self->spi_desc.dev.prvt;
    // return hri_sercomspi_get_CTRLA_CPOL_bit(hw);
    return 0;
}
