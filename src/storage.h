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

#ifndef STORAGE_H
#define STORAGE_H

#include "misc.h"

/*
 * Всё развалится как карточный домик, стоит только тронуть один дефайн.
 * Не красиво, потом перепишу, сейчас главное - получить рабочий вариант.
 * */

#define STORAGE_PAGE_SIZE   2048
#define STORAGE_ADDR        0x08078000

#define STORAGE_MAX_MACRO   128
#define STORAGE_PAGES       (STORAGE_MAX_MACRO / 8)
#define STORAGE_BUF_SIZE    STORAGE_PAGE_SIZE

#define STORAGE_MAX_ACTS    6
#define STORAGE_MAX_PRESS   28

/* Должно быть 256 байт */
#define STORAGE_MACRO_SIZE  (sizeof(storage_macro_t) + (sizeof(storage_btn_t) * STORAGE_MAX_ACTS) + (sizeof(storage_btn_t) * STORAGE_MAX_PRESS))

#define STORAGE_FLASH_OFFSET(x) ((x & 0x7800) >> 11) * STORAGE_PAGE_SIZE;
#define STORAGE_PAGE_OFFSET(x)  ((x & 0x0700) >> 8)  * STORAGE_MACRO_SIZE;

#define STORAGE_MACRO_NOT_FOUND 255
#define STORAGE_MACRO_OK        0
#define STORAGE_BLOCK_USED      255
#define STORAGE_BLOCK_CLEAR     0

typedef struct {
  uint8_t   btn;
  uint16_t  time_delay;
  uint16_t  time_press;
  uint8_t   keep;
} __attribute__((packed)) storage_btn_t;

typedef struct {
  storage_btn_t *acts;
  storage_btn_t *pressed;
} __attribute__((packed)) storage_btns_t;

typedef struct {
  char    name[46];
  uint8_t act_count;
  uint8_t press_count;
  uint8_t gamepads[4];
} __attribute__((packed)) storage_macro_t;

typedef struct {
  uint8_t blocks[STORAGE_MAX_MACRO];
} __attribute__((packed)) storage_map_t;

void          storage_init(void);
void          storage_clear(void);
uint8_t       storage_add_macro(storage_macro_t *macro, storage_btns_t buttons);
uint8_t       storage_remove_macro(storage_macro_t macro);
void          storage_print(void);
storage_map_t storage_get_macro_list(void);
uint8_t       storage_get_macro(uint8_t block_id, storage_macro_t *macro, storage_btns_t *buttons);

#endif

