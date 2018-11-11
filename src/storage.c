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

#include "storage.h"

static uint8_t  *buf = NULL;
static uint8_t  *mem = (uint8_t *) STORAGE_ADDR;

static void storage_write_page(uint32_t addr, uint8_t *pdata) {
  __asm__ volatile ("CPSID I\n");
  flash_unlock();

  for (uint32_t volatile word = 0; word < STORAGE_BUF_SIZE; word += sizeof(uint32_t)) {
    uint32_t data;
    data  = ((uint32_t) pdata[word + 0]);
    data |= ((uint32_t) pdata[word + 1]) << 8;
    data |= ((uint32_t) pdata[word + 2]) << 16;
    data |= ((uint32_t) pdata[word + 3]) << 24;

    flash_program_word(addr + word, data);
  }

  flash_lock();
  __asm__ volatile ("CPSIE I\n");
}

static void storage_read_page(uint32_t addr, uint8_t *data) {
  __asm__ volatile ("CPSID I\n");
  flash_unlock();

  memcpy(data, (uint32_t *) addr, sizeof(uint8_t) * STORAGE_BUF_SIZE);

  flash_lock();
  __asm__ volatile ("CPSIE I\n");
}

static void storage_erase_page(uint32_t addr) {
  __asm__ volatile ("CPSID I\n");
  flash_unlock();

  flash_erase_page(addr);

  flash_lock();
  __asm__ volatile ("CPSIE I\n");
}

static uint32_t storage_find_free(void) {
  storage_macro_t cheat;

  for (uint32_t offset = 0; offset < STORAGE_MAX_MACRO * STORAGE_MACRO_SIZE; offset += STORAGE_MACRO_SIZE) {

    uint32_t flash_offset = STORAGE_FLASH_OFFSET(offset);
    uint32_t page_offset  = STORAGE_PAGE_OFFSET(offset);

    storage_read_page((uint32_t)(mem + flash_offset), buf);

    memcpy(&cheat, buf + page_offset, sizeof(storage_macro_t));

    if (cheat.name[0] == '\0') {
      return offset;
    }
  }

  return STORAGE_BLOCK_USED;
}

static uint32_t storage_find_macro(char *name) {
  storage_macro_t macro;

  for (uint32_t offset = 0; offset < STORAGE_MAX_MACRO * STORAGE_MACRO_SIZE; offset += STORAGE_MACRO_SIZE) {

    uint32_t flash_offset = STORAGE_FLASH_OFFSET(offset);
    uint32_t page_offset  = STORAGE_PAGE_OFFSET(offset);

    storage_read_page((uint32_t)(mem + flash_offset), buf);

    memcpy(&macro, buf + page_offset, sizeof(storage_macro_t));

    if (strcmp(name, macro.name) == 0) {
      return offset;
    }
  }

  return STORAGE_MACRO_NOT_FOUND;
}

void storage_init(void) {
  if (buf == NULL) {
    buf = malloc(STORAGE_BUF_SIZE * sizeof(uint8_t));
  }

  memset(buf, 0, STORAGE_BUF_SIZE * sizeof(uint8_t));
}

void storage_clear(void) {
  debugf("  ---------------- Storage clear ----------------\n");
  memset(buf, 0, STORAGE_BUF_SIZE * sizeof(uint8_t));

  for (uint8_t page = 0; page < STORAGE_PAGES; page++) {
    storage_erase_page(STORAGE_ADDR + (STORAGE_PAGE_SIZE * page));
    storage_write_page(STORAGE_ADDR + (STORAGE_PAGE_SIZE * page), buf);
  }

  debugf("  --------------------- End ---------------------\n");
}

uint8_t storage_add_macro(storage_macro_t *macro, storage_btns_t buttons) {

  debugf("  ------------------ Add macro ------------------\n");
  debugf("- Adding %s\n", macro->name);
  debugf("- Trying to find free block...\n");

  uint32_t offset = storage_find_free();

  if (offset == STORAGE_BLOCK_USED) {
    debugf("- Couldn't find free space\n");
    return STORAGE_BLOCK_USED;
  }

  debugf("- Free block at: 0x%04X\n", (unsigned int) offset);

  uint32_t flash_offset = STORAGE_FLASH_OFFSET(offset);
  uint32_t page_offset  = STORAGE_PAGE_OFFSET(offset);

  debugf("- Flash addr:    0x%08X\n", (unsigned int) mem);
  debugf("- Flash offset:  0x%04X (0x%08X)\n", (unsigned int) flash_offset, (unsigned int) mem + flash_offset);
  debugf("- Page offset:   0x%02X\n", (unsigned int) page_offset);

  storage_read_page((uint32_t) mem + flash_offset, buf);
  storage_erase_page((uint32_t) mem + flash_offset);

  memcpy(buf + page_offset, macro, sizeof(storage_macro_t));

  for (uint8_t btn = 0; btn < macro->act_count; ++btn) {
    uint8_t *btn_offset = buf + page_offset + sizeof(storage_macro_t) + (sizeof(storage_btn_t) * btn);
    memcpy(btn_offset, buttons.acts + btn, sizeof(storage_btn_t));
  }

  for (uint8_t btn = 0; btn < macro->press_count; ++btn) {
    uint8_t *btn_offset = buf + page_offset + sizeof(storage_macro_t) + (sizeof(storage_btn_t) * STORAGE_MAX_ACTS) + (sizeof(storage_btn_t) * btn);
    memcpy(btn_offset, buttons.pressed + btn, sizeof(storage_btn_t));
  }

  storage_write_page((uint32_t )mem + flash_offset, buf);
  debugf("  --------------------- End ---------------------\n");

  return STORAGE_MACRO_OK;
}

uint8_t storage_remove_macro(storage_macro_t macro) {
  debugf("  ---------------- Remove macro -----------------\n");
  debugf("- Removing %s\n", macro.name);

  uint32_t offset = storage_find_macro(macro.name);

  if (offset == STORAGE_MACRO_NOT_FOUND) {
    debugf("- Couldn't find macro\n");
    debugf("  --------------------- End ---------------------\n");
    return STORAGE_MACRO_NOT_FOUND;
  }

  debugf("- Block at:      0x%04X\n", (unsigned int) offset);

  uint32_t flash_offset = STORAGE_FLASH_OFFSET(offset);
  uint32_t page_offset  = STORAGE_PAGE_OFFSET(offset);

  debugf("- Flash addr:    0x%08X\n", (unsigned int) mem);
  debugf("- Flash offset:  0x%04X (0x%08X)\n", (unsigned int) flash_offset, (unsigned int) mem + flash_offset);
  debugf("- Page offset:   0x%02X\n", (unsigned int) page_offset);

  storage_read_page((uint32_t)(mem + flash_offset), buf);
  memcpy(&macro, buf + page_offset, sizeof(storage_macro_t));

  macro.name[0] = '\0';

  memcpy(buf + page_offset, &macro, sizeof(macro));
  storage_erase_page((uint32_t) mem + flash_offset);

  memcpy(buf + page_offset, &macro, sizeof(macro));
  storage_write_page((uint32_t )mem + flash_offset, buf);

  debugf("  --------------------- End ---------------------\n");
  return STORAGE_MACRO_OK;
}

storage_map_t storage_get_macro_list(void) {
  debugf("  ----------------- Macros list -----------------\n");

  uint8_t i = 0;

  storage_macro_t macro;
  storage_map_t map;
  memset(map.blocks, STORAGE_BLOCK_CLEAR, STORAGE_MAX_MACRO);


  for (uint32_t offset = 0; offset < STORAGE_MAX_MACRO * STORAGE_MACRO_SIZE; offset += STORAGE_MACRO_SIZE) {
    uint32_t flash_offset = STORAGE_FLASH_OFFSET(offset);
    uint32_t page_offset  = STORAGE_PAGE_OFFSET(offset);

    storage_read_page((uint32_t)(mem + flash_offset), buf);
    memcpy(&macro, buf + page_offset, sizeof(storage_macro_t));

    if (macro.name[0] == '\0') {
      debugf("- Block %2d: CLEAN\n", i);
      map.blocks[i] = STORAGE_BLOCK_CLEAR;
    } else {
      debugf("- Block %2d: USED  %s\n", i, macro.name);
      map.blocks[i] = STORAGE_BLOCK_USED;
    }

    if (macro.act_count != 0) {
      //debugf("-  Activate buttons:\n");

      storage_btn_t *btn_data = (storage_btn_t *) (buf + page_offset + sizeof(storage_macro_t));
      for (uint8_t btn = 0; btn < macro.act_count; ++btn) {
        //debugf("-    Button: %d\n", btn_data->btn);
        ++btn_data;
      }

      //debugf("-  Buttons sequence:\n");

      btn_data = (storage_btn_t *) (buf + page_offset + sizeof(storage_macro_t) + (sizeof(storage_btn_t) * STORAGE_MAX_ACTS));
      for (uint8_t btn = 0; btn < macro.press_count; ++btn) {
        //debugf("-    Button: %d, Delay: %d, Press: %d, Keep: %d\n", btn_data->btn, btn_data->time_delay, btn_data->time_press, btn_data->keep);
        ++btn_data;
      }
    }
    ++i;
  }

  debugf("  --------------------- End ---------------------\n");

  return map;
}

uint8_t storage_get_macro(uint8_t block_id, storage_macro_t *macro, storage_btns_t *buttons) {
  debugf("  ------------------ Get macro ------------------\n");

  uint32_t offset = STORAGE_MACRO_SIZE * block_id;

  debugf("- Block at:      0x%04X\n", (unsigned int) offset);

  uint32_t flash_offset = STORAGE_FLASH_OFFSET(offset);
  uint32_t page_offset  = STORAGE_PAGE_OFFSET(offset);

  debugf("- Flash addr:    0x%08X\n", (unsigned int) mem);
  debugf("- Flash offset:  0x%04X (0x%08X)\n", (unsigned int) flash_offset, (unsigned int) mem + flash_offset);
  debugf("- Page offset:   0x%02X\n", (unsigned int) page_offset);

  storage_read_page((uint32_t)(mem + flash_offset), buf);
  memcpy(macro, buf + page_offset, sizeof(storage_macro_t));

  if (macro->name[0] == '\0') {
    debugf("- Block %2d: CLEAN\n", block_id);
    return STORAGE_BLOCK_CLEAR;
  } else {
    debugf("- Block %2d: USED  %s\n", block_id, macro->name);
  }

  buttons->acts     = malloc(sizeof(storage_btn_t) * macro->act_count);
  buttons->pressed  = malloc(sizeof(storage_btn_t) * macro->press_count);

  memcpy(buttons->acts,    buf + page_offset + sizeof(storage_macro_t), sizeof(storage_btn_t) * macro->act_count);
  memcpy(buttons->pressed, buf + page_offset + sizeof(storage_macro_t) + (sizeof(storage_btn_t) * STORAGE_MAX_ACTS), sizeof(storage_btn_t) * macro->press_count);

  debugf("-  Activate buttons:\n");

  storage_btn_t *btn = buttons->acts;
  for (uint8_t btn_i = 0; btn_i < macro->act_count; ++btn_i) {
    debugf("-    Button: %d\n", btn->btn);
    ++btn;
  }

  debugf("-  Buttons sequence:\n");

  btn = buttons->pressed;
  for (uint8_t btn_i = 0; btn_i < macro->press_count; ++btn_i) {
    debugf("-    Button: %d, Delay: %d, Press: %d, Keep: %d\n", btn->btn, btn->time_delay, btn->time_press, btn->keep);
    ++btn;
  }

  debugf("  --------------------- End ---------------------\n");

  return STORAGE_BLOCK_USED;
}
