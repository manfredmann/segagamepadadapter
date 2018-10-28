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

#include "list.h"

static list_node_t *list_make_node(void *value) {
  list_node_t *node = malloc(sizeof(list_node_t));
  node->value     = value;
  node->next      = NULL;
  node->previous  = NULL;

  return node;
}

list_t * list_init(void) {
  list_t *list = malloc(sizeof(list_t));
  list->head    = NULL;
  list->bottom  = NULL;
  list->size    = 0;

  return list;
}

void list_push_back(list_t *list, void *value) {
  if (list->head == NULL) {
    list->head    = list_make_node(value);
    list->bottom  = list->head;
    list->size    = 1;
    return;
  }

  list_node_t *node = list->bottom;

  node->next = list_make_node(value);
  node->next->previous = node;

  list->bottom = node->next;

  ++list->size;
}

uint32_t list_length(list_t *list) {
  uint32_t i = 0;

  list_node_t *head = list->head;

  while (head != NULL) {
    ++i;
    head = head->next;
  }

  return list->size;
}

void *list_get(list_t *list, uint32_t i) {
  if (i >= list->size) {
    return NULL;
  }

  if (i == 0) {
    return list->head->value;
  }

  list_node_t *node = list->head;

  for (uint32_t x = 0; x < i; ++x) {
    node = node->next;
  }

  return node->value;
}

list_node_t *list_iter(list_t *list, list_node_t *node) {
  if (node == NULL) {
    return list->head;
  } else {
    if (node->next != NULL) {
      return node->next;
    } else {
      return NULL;
    }
  }
}

void list_clear(list_t *list) {
  if (list->bottom == NULL) {
    return;
  }

  while (list->head != list->bottom) {
    list_node_t *node = list->bottom;
    list->bottom = node->previous;

    if (node->value != NULL) {
      free(node->value);
    }

    free(node);
  }

  if (list->bottom->value != NULL) {
    free(list->bottom->value);
  }

  free(list->bottom);
  list->bottom = NULL;
  list->head   = NULL;
  list->size   = 0;
}
