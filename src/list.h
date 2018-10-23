#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdint.h>

typedef struct list_node {
  void             *value;
  struct list_node *next;
} list_node_t;

typedef struct {
  list_node_t   *next;
  uint32_t      size;
} list_t;

list_t    *list_init(void);
void      list_push_back(list_t *list, void *value);
uint32_t  list_length(list_t *list);
void      *list_get(list_t *list, uint32_t i);
void      list_delete(list_t *list, uint32_t i);

#endif
