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
