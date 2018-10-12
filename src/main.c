#include "init.h"
#include "gpio.h"
#include "usb.h"

int main(void) {
  clock_init();
  uart_init();
  systick_init();
  led_init();
  usb_init(4);

  _msleep(10);

  for (volatile int i = 0; i < 3; i++) {
    gpio_set(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
    gpio_clear(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
  }

  gamepad_gpio_t gamepads[4];

  gamepads[0].data0.port  = GPIOA;
  gamepads[0].data1.port  = GPIOA;
  gamepads[0].data2.port  = GPIOA;
  gamepads[0].data3.port  = GPIOA;
  gamepads[0].data4.port  = GPIOA;
  gamepads[0].data5.port  = GPIOA;
  gamepads[0].select.port = GPIOB;
  gamepads[0].data0.pin   = GPIO0;
  gamepads[0].data1.pin   = GPIO1;
  gamepads[0].data2.pin   = GPIO2;
  gamepads[0].data3.pin   = GPIO3;
  gamepads[0].data4.pin   = GPIO4;
  gamepads[0].data5.pin   = GPIO5;
  gamepads[0].select.pin  = GPIO0;

  gamepads[1].data0.port  = GPIOA;
  gamepads[1].data1.port  = GPIOA;
  gamepads[1].data2.port  = GPIOA;
  gamepads[1].data3.port  = GPIOA;
  gamepads[1].data4.port  = GPIOA;
  gamepads[1].data5.port  = GPIOA;
  gamepads[1].select.port = GPIOB;
  gamepads[1].data0.pin   = GPIO6;
  gamepads[1].data1.pin   = GPIO7;
  gamepads[1].data2.pin   = GPIO8;
  gamepads[1].data3.pin   = GPIO9;
  gamepads[1].data4.pin   = GPIO10;
  gamepads[1].data5.pin   = GPIO15;
  gamepads[1].select.pin  = GPIO1;

  gamepads[2].data0.port  = GPIOC;
  gamepads[2].data1.port  = GPIOC;
  gamepads[2].data2.port  = GPIOC;
  gamepads[2].data3.port  = GPIOC;
  gamepads[2].data4.port  = GPIOC;
  gamepads[2].data5.port  = GPIOC;
  gamepads[2].select.port = GPIOB;
  gamepads[2].data0.pin   = GPIO0;
  gamepads[2].data1.pin   = GPIO1;
  gamepads[2].data2.pin   = GPIO2;
  gamepads[2].data3.pin   = GPIO3;
  gamepads[2].data4.pin   = GPIO4;
  gamepads[2].data5.pin   = GPIO5;
  gamepads[2].select.pin  = GPIO2;

  gamepads[3].data0.port  = GPIOC;
  gamepads[3].data1.port  = GPIOC;
  gamepads[3].data2.port  = GPIOC;
  gamepads[3].data3.port  = GPIOC;
  gamepads[3].data4.port  = GPIOC;
  gamepads[3].data5.port  = GPIOC;
  gamepads[3].select.port = GPIOB;
  gamepads[3].data0.pin   = GPIO6;
  gamepads[3].data1.pin   = GPIO7;
  gamepads[3].data2.pin   = GPIO8;
  gamepads[3].data3.pin   = GPIO9;
  gamepads[3].data4.pin   = GPIO10;
  gamepads[3].data5.pin   = GPIO11;
  gamepads[3].select.pin  = GPIO3;

  gamepads_init(gamepads, 4);
  gamepads_en();
  while(1) {
    usb_send();
    _msleep(1);
  }
  gamepads_den();

  return 0;
}
