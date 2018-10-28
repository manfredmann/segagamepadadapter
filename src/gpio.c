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

#include "gpio.h"
#include "init.h"
#include <stdlib.h>

void led_init(void) {
  rcc_periph_clock_enable(GPIO_LED_RCC);

  gpio_set_mode(GPIO_LED_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO_LED_BL);
  gpio_set_mode(GPIO_LED_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO_LED_INFO);
}

void led_info(bool state) {
  if (state)
    gpio_set(GPIO_LED_PORT, GPIO_LED_INFO);
  else
    gpio_clear(GPIO_LED_PORT, GPIO_LED_INFO);
}

void led_boot(bool state) {
  if (state)
    gpio_set(GPIO_LED_PORT, GPIO_LED_BL);
  else
    gpio_clear(GPIO_LED_PORT, GPIO_LED_BL);
}

