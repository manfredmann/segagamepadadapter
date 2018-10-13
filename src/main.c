#include "init.h"
#include "gpio.h"
#include "usb.h"
#include "usb_hid.h"
#include "gamepad.h"

static gamepad_gpio_t gamepads[4] = {
  {
    .data0.port  = GPIOA,
    .data1.port  = GPIOA,
    .data2.port  = GPIOA,
    .data3.port  = GPIOA,
    .data4.port  = GPIOA,
    .data5.port  = GPIOA,
    .select.port = GPIOB,
    .data0.pin   = GPIO0,
    .data1.pin   = GPIO1,
    .data2.pin   = GPIO2,
    .data3.pin   = GPIO3,
    .data4.pin   = GPIO4,
    .data5.pin   = GPIO5,
    .select.pin  = GPIO0,
  },
  {
    .data0.port  = GPIOA,
    .data1.port  = GPIOA,
    .data2.port  = GPIOA,
    .data3.port  = GPIOA,
    .data4.port  = GPIOA,
    .data5.port  = GPIOA,
    .select.port = GPIOB,
    .data0.pin   = GPIO6,
    .data1.pin   = GPIO7,
    .data2.pin   = GPIO8,
    .data3.pin   = GPIO9,
    .data4.pin   = GPIO10,
    .data5.pin   = GPIO15,
    .select.pin  = GPIO1,
  },
  {
    .data0.port  = GPIOC,
    .data1.port  = GPIOC,
    .data2.port  = GPIOC,
    .data3.port  = GPIOC,
    .data4.port  = GPIOC,
    .data5.port  = GPIOC,
    .select.port = GPIOB,
    .data0.pin   = GPIO0,
    .data1.pin   = GPIO1,
    .data2.pin   = GPIO2,
    .data3.pin   = GPIO3,
    .data4.pin   = GPIO4,
    .data5.pin   = GPIO5,
    .select.pin  = GPIO2,
  },
  {
    .data0.port  = GPIOC,
    .data1.port  = GPIOC,
    .data2.port  = GPIOC,
    .data3.port  = GPIOC,
    .data4.port  = GPIOC,
    .data5.port  = GPIOC,
    .select.port = GPIOB,
    .data0.pin   = GPIO6,
    .data1.pin   = GPIO7,
    .data2.pin   = GPIO8,
    .data3.pin   = GPIO9,
    .data4.pin   = GPIO10,
    .data5.pin   = GPIO11,
    .select.pin  = GPIO3,
  }
};

int main(void) {
  clock_init();
  uart_init();
  systick_init();
  led_init();
  usb_init();

  _msleep(10);

  for (volatile int i = 0; i < 3; i++) {
    gpio_set(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
    gpio_clear(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
  }

  gamepads_init(gamepads, 4);

/*  gamepad_simple_cheat_t cheat;
  gamepads_cheat_init(&cheat);

  cheat.button = BTN_C;
  cheat.pressed[0] = BTN_DOWN;
  cheat.pressed[1] = BTN_A;
  cheat.pressed[2] = BTN_B;
  gamepads_cheat_set(cheat, 0);*/

  gamepads_en();
  while(1) {
    usb_hid_poll();
  }
  gamepads_den();

  return 0;
}
