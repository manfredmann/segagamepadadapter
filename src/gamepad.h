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

#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "misc.h"
#include "gpio.h"
#include "list.h"
#include "init.h"

#define BTN_UP    0
#define BTN_DOWN  1
#define BTN_LEFT  2
#define BTN_RIGHT 3
#define BTN_A     4
#define BTN_B     5
#define BTN_C     6
#define BTN_X     7
#define BTN_Y     8
#define BTN_Z     9
#define BTN_START 10
#define BTN_MODE  11

#define gamepads_en()   gpio_clear(GPIOB, GPIO8)
#define gamepads_den()  gpio_set(GPIOB, GPIO8)

typedef struct {
  uint8_t buttons;
  int8_t x;
  int8_t y;
} gamepad_data_t;

typedef struct {
  uint32_t port;
  uint16_t pin;
} gamepad_pin_t;

typedef struct {
  gamepad_pin_t data0;
  gamepad_pin_t data1;
  gamepad_pin_t data2;
  gamepad_pin_t data3;
  gamepad_pin_t data4;
  gamepad_pin_t data5;
  gamepad_pin_t select;
} gamepad_gpio_t;

typedef struct {
  uint8_t  button;
  bool     keep_pressed;
  uint16_t time_delay;
  uint16_t time_delay_last;
  uint16_t time_press;
  uint16_t time_press_last;
} gamepad_macro_btn_t;

typedef struct {
  list_t    *act_buttons;
  list_t    *press_buttons;
} gamepad_macro_t;

typedef uint8_t *gamepad_buttons_t;

void             gamepads_init(gamepad_gpio_t *gamepads, uint8_t count);

gamepad_macro_t *gamepads_macro_init(void);
void             gamepads_macro_add(gamepad_macro_t *macro, uint8_t gamepad);
void             gamepads_macro_add_btn(list_t *buttons, gamepad_macro_btn_t btn);
void             gamepads_macro_clear(void);

gamepad_data_t  *gamepad_read(uint8_t gamepad);

#endif
