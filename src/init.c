/*
 * This file is part of the SEGA Gamepad adapter project.
 *
 * Copyright (C) 2018 Roman Serov <roman@serov.co>
 *
 * SEGA Gamepad adapter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SEGA Gamepad adapter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SEGA Gamepad adapter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "init.h"

void clock_init(void) {
  rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

void dwt_init(void) {
  dwt_enable_cycle_counter();
}

static __inline uint32_t delta(uint32_t t0, uint32_t t1) {
  return (t1 - t0);
}

void _usleep(uint32_t delay) {
  uint32_t t0 = dwt_read_cycle_counter();
  uint32_t us_count_tic = delay * 72;
  while (delta(t0, dwt_read_cycle_counter()) < us_count_tic);
}

void _msleep(uint32_t delay) {
  _usleep(delay * 1000);
}

void uart_init(void) {
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_USART1);
  rcc_periph_clock_enable(RCC_AFIO);

  gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON, AFIO_MAPR_USART1_REMAP);

  gpio_set_mode(GPIO_BANK_USART1_RE_TX, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_USART1_RE_TX);
  gpio_set_mode(GPIO_BANK_USART1_RE_RX, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RE_RX);

  usart_set_baudrate(USART1, 921600);
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_mode(USART1, USART_MODE_TX_RX);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

  usart_enable(USART1);
}

int _write(int file, char *ptr, int len) {
  if (file == STDOUT_FILENO || file == STDERR_FILENO) {

    for (uint16_t i = 0; i < len; i++) {
      if (ptr[i] == '\n') {
        usart_send_blocking(USART1, '\r');
      }
      usart_send_blocking(USART1, ptr[i]);
    }

    return len;
  }
  errno = EIO;
  return -1;
}

