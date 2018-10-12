#ifndef GPIO_H
#define GPIO_H

#include "misc.h"

#define GPIO_LED_PORT   GPIOB
#define GPIO_LED_RCC    RCC_GPIOB
#define GPIO_LED_BL     GPIO9
#define GPIO_LED_INFO   GPIO10

void  led_init(void);
void  led_info(bool state);
void  led_boot(bool state);

#endif // GPIO_H
