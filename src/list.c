#include "list.h"

static list_node_t *list_make_node(void *value) {
  list_node_t *node = malloc(sizeof(list_node_t));
  node->value  = value;
  node->next   = NULL;

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
    list->head = list_make_node(value);
    list->size = 1;
    list->bottom = list->head;
    return;
  }

  list_node_t *node = list->bottom;

  node->next = list_make_node(value);
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
