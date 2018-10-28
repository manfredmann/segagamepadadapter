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

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdint.h>

typedef struct list_node {
  void *value;
  struct list_node *previous;
  struct list_node *next;
} list_node_t;

typedef struct {
  list_node_t   *head;
  list_node_t   *bottom;
  uint32_t      size;
} list_t;

list_t      *list_init(void);
void        list_push_back(list_t *list, void *value);
uint32_t    list_length(list_t *list);
void        *list_get(list_t *list, uint32_t i);
list_node_t *list_iter(list_t *list, list_node_t *node);
void        list_clear(list_t *list);

#endif
