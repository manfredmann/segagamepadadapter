#include "init.h"
#include "gpio.h"
#include "usb.h"

int main(void) {
  clock_init();
  uart_init();
  systick_init();
  led_init();
  usb_init();

  printf("Reset\r\n");

  _msleep(10);

  for (volatile int i = 0; i < 3; i++) {
    gpio_set(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
    gpio_clear(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
  }

  gamepads_init();
  gamepads_en();
  while(1) {
    usb_send();
    _msleep(1);
  }
  gamepads_den();

  return 0;
}
