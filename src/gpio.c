#include "gpio.h"
#include "init.h"
#include <stdlib.h>

void led_init(void) {
  rcc_periph_clock_enable(GPIO_LED_RCC);

  gpio_set_mode(GPIO_LED_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO_LED_BL);
  gpio_set_mode(GPIO_LED_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO_LED_INFO);
}

void led_info(bool state) {
  if (state)
    gpio_set(GPIO_LED_PORT, GPIO_LED_INFO);
  else
    gpio_clear(GPIO_LED_PORT, GPIO_LED_INFO);
}

void led_boot(bool state) {
  if (state)
    gpio_set(GPIO_LED_PORT, GPIO_LED_BL);
  else
    gpio_clear(GPIO_LED_PORT, GPIO_LED_BL);
}

