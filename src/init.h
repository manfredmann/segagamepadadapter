#ifndef INIT_H
#define INIT_H

#include "misc.h"

void systick_init(void);
void clock_init(void);
void gpio_init(void);
void uart_init(void);

void _usleep(uint32_t delay);
void _msleep(uint32_t delay);
int _write(int file, char *ptr, int len);


#endif // INIT_H
