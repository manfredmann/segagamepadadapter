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
#include "gpio.h"
#include "usb.h"
#include "usb_hid.h"
#include "gamepad.h"
#include "storage.h"

static gamepad_gpio_t gamepads[4] = {
  {
    .data0.port  = GPIOA,
    .data1.port  = GPIOA,
    .data2.port  = GPIOA,
    .data3.port  = GPIOA,
    .data4.port  = GPIOA,
    .data5.port  = GPIOA,
    .select.port = GPIOB,
    .data0.pin   = GPIO0,
    .data1.pin   = GPIO1,
    .data2.pin   = GPIO2,
    .data3.pin   = GPIO3,
    .data4.pin   = GPIO4,
    .data5.pin   = GPIO5,
    .select.pin  = GPIO0,
  },
  {
    .data0.port  = GPIOA,
    .data1.port  = GPIOA,
    .data2.port  = GPIOA,
    .data3.port  = GPIOA,
    .data4.port  = GPIOA,
    .data5.port  = GPIOA,
    .select.port = GPIOB,
    .data0.pin   = GPIO6,
    .data1.pin   = GPIO7,
    .data2.pin   = GPIO8,
    .data3.pin   = GPIO9,
    .data4.pin   = GPIO10,
    .data5.pin   = GPIO15,
    .select.pin  = GPIO1,
  },
  {
    .data0.port  = GPIOC,
    .data1.port  = GPIOC,
    .data2.port  = GPIOC,
    .data3.port  = GPIOC,
    .data4.port  = GPIOC,
    .data5.port  = GPIOC,
    .select.port = GPIOB,
    .data0.pin   = GPIO0,
    .data1.pin   = GPIO1,
    .data2.pin   = GPIO2,
    .data3.pin   = GPIO3,
    .data4.pin   = GPIO4,
    .data5.pin   = GPIO5,
    .select.pin  = GPIO2,
  },
  {
    .data0.port  = GPIOC,
    .data1.port  = GPIOC,
    .data2.port  = GPIOC,
    .data3.port  = GPIOC,
    .data4.port  = GPIOC,
    .data5.port  = GPIOC,
    .select.port = GPIOB,
    .data0.pin   = GPIO6,
    .data1.pin   = GPIO7,
    .data2.pin   = GPIO8,
    .data3.pin   = GPIO9,
    .data4.pin   = GPIO10,
    .data5.pin   = GPIO11,
    .select.pin  = GPIO3,
  }
};

int main(void) {
  clock_init();
  dwt_init();
  uart_init();
  led_init();
  usb_init();

  debugf("%c[2J%c ", 27, 27);
  debugf("---------------------------------------------------------------------------\n");
  debugf("| SEGA Gamepad adapter    Copyright (C) 2018 Roman Serov <roman@serov.co> |\n");
  debugf("| Thanks to:              Cluster, lizardqueengointotheclub27             |\n");
  debugf("| License:                GPL-3.0  See <http://www.gnu.org/licenses/>     |\n");
  debugf("---------------------------------------------------------------------------\n");
  debugf("\n");
  debugf("- Startup (orange LED blinking)...\n");

  for (volatile int i = 0; i < 5; i++) {
    gpio_set(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
    gpio_clear(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
  }

  debugf("- Gamepads init...\n");
  gamepads_init(gamepads, 4);

  debugf("- Storage init...\n");
  storage_init();
  //storage_clear();
  storage_get_macro_list();
  //update_gamepads_macro();
  //storage_clear();

  gamepads_en();
  while(1) {
    usb_hid_poll();
  }
  gamepads_den();

  return 0;
}
