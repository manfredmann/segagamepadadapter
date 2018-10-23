#include "list.h"

static list_node_t *list_make_node(void *value) {
  list_node_t *node = malloc(sizeof(list_node_t));
  node->value = value;
  node->next  = NULL;

  return node;
}

list_t *list_init(void) {
  list_t *list = malloc(sizeof(list_t));
  list->next = NULL;
  list->size = 0;

  return list;
}

void list_push_back(list_t *list, void *value) {
  uint32_t i = 1;

  if (list->next == NULL) {
    list->next = list_make_node(value);
    list->size = 1;
    return;
  }

  list_node_t *node = list->next;

  while (node->next != NULL) {
    node = node->next;
    ++i;
  }

  node->next = list_make_node(value);
  list->size = i + 1;
}

uint32_t list_length(list_t *list) {
  uint32_t i = 0;

  list_node_t *head = list->next;

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
    return list->next->value;
  }

  list_node_t *node = list->next;

  for (uint32_t x = 0; x < i; ++x) {
    node = node->next;
  }

  return node->value;
}

void list_delete(list_t *list, uint32_t i) {
  if (i >= list->size) {
    return;
  }

  if (i == 0) {
    if (list->next->next != NULL) {
      list_node_t *tmp = list->next->next;
      free(list->next);
      list->next = tmp;
    } else {
      free(list->next);
      list->next = NULL;
    }
    list->size--;
    return;
  }

  list_node_t *node = list->next;

  for (uint32_t x = 1; x < i; ++x) {
    node = node->next;
  }

  if (node->next->next != NULL) {
    list_node_t *tmp = node->next->next;
    free(node->next);
    node->next = tmp;
  } else {
    free(node->next);
    node->next = NULL;
  }
  list->size--;
}
