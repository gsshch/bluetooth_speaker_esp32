/*
 * ssd1351.c
 *
 *  Created on: 2018-03-14 18:04
 *      Author: Jack Chen <redchenjs@live.com>
 */

#include <string.h>

#include "device/spi.h"
#include "driver/ssd1351.h"

#define SSD1351_GPIO_PIN_DC   23
#define SSD1351_GPIO_PIN_RST  14

spi_transaction_t spi1_trans[6];

void ssd1351_init_board(void)
{
    gpio_set_direction(SSD1351_GPIO_PIN_DC,  GPIO_MODE_OUTPUT);
    gpio_set_direction(SSD1351_GPIO_PIN_RST, GPIO_MODE_OUTPUT);
    gpio_set_level(SSD1351_GPIO_PIN_DC,  0);
    gpio_set_level(SSD1351_GPIO_PIN_RST, 0);

    memset(spi1_trans, 0, sizeof(spi1_trans));
}

void ssd1351_setpin_dc(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(SSD1351_GPIO_PIN_DC, dc);
}

void ssd1351_setpin_reset(uint8_t rst)
{
    gpio_set_level(SSD1351_GPIO_PIN_RST, rst);
}

void ssd1351_write_cmd(uint8_t cmd)
{
    esp_err_t ret;

    spi1_trans[0].length = 8;
    spi1_trans[0].tx_buffer = &cmd;
    spi1_trans[0].user = (void*)0;

    ret = spi_device_transmit(spi1, &spi1_trans[0]);
    assert(ret == ESP_OK);
}

void ssd1351_write_data(uint8_t data)
{
    esp_err_t ret;

    spi1_trans[0].length = 8;
    spi1_trans[0].tx_buffer = &data;
    spi1_trans[0].user = (void*)1;

    ret = spi_device_transmit(spi1, &spi1_trans[0]);
    assert(ret == ESP_OK);
}

void ssd1351_refresh_gram(uint8_t *gram)
{
    esp_err_t ret;

    spi1_trans[0].length = 8;
    spi1_trans[0].tx_data[0] = 0x15;    // Set Column Address
    spi1_trans[0].user = (void*)0;
    spi1_trans[0].flags = SPI_TRANS_USE_TXDATA;

    spi1_trans[1].length = 2*8;
    spi1_trans[1].tx_data[0] = 0x00;    // startx
    spi1_trans[1].tx_data[1] = SSD1351_SCREEN_WIDTH - 1;    // endx
    spi1_trans[1].user = (void*)1;
    spi1_trans[1].flags = SPI_TRANS_USE_TXDATA;

    spi1_trans[2].length = 8,
    spi1_trans[2].tx_data[0] = 0x75;    // Set Row Address
    spi1_trans[2].user = (void*)0;
    spi1_trans[2].flags = SPI_TRANS_USE_TXDATA;

    spi1_trans[3].length = 2*8,
    spi1_trans[3].tx_data[0] = 0x00;    // starty
    spi1_trans[3].tx_data[1] = SSD1351_SCREEN_HEIGHT - 1;   // endy
    spi1_trans[3].user = (void*)1;
    spi1_trans[3].flags = SPI_TRANS_USE_TXDATA;

    spi1_trans[4].length = 8,
    spi1_trans[4].tx_data[0] = 0x5c;    // Set Write RAM
    spi1_trans[4].user = (void*)0;
    spi1_trans[4].flags = SPI_TRANS_USE_TXDATA;

    spi1_trans[5].length = SSD1351_SCREEN_WIDTH*SSD1351_SCREEN_HEIGHT*2*8;
    spi1_trans[5].tx_buffer = gram;
    spi1_trans[5].user = (void*)1;

    //Queue all transactions.
    for (int x=0; x<6; x++) {
        ret=spi_device_queue_trans(spi1, &spi1_trans[x], portMAX_DELAY);
        assert(ret==ESP_OK);
    }

    for (int x=0; x<6; x++) {
        spi_transaction_t* ptr;
        ret=spi_device_get_trans_result(spi1, &ptr, portMAX_DELAY);
        assert(ret==ESP_OK);
        assert(ptr==spi1_trans+x);
    }
}
