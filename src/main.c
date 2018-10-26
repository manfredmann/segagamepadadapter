#include "init.h"
#include "gpio.h"
#include "usb.h"
#include "usb_hid.h"
#include "gamepad.h"
#include "storage.h"

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

  _msleep(2000);

  debugf("%c[2J%c ", 27, 27);
  debugf("--------------------------------------------------\n");
  debugf("| SEGA Gamepad adapter      (c) manfredmann 2018 |\n");
  debugf("| Thanks to: Cluster, lizardqueengointotheclub27 |\n");
  debugf("--------------------------------------------------\n");
  debugf("- Startup (orange LED blinking)...\n");

  for (volatile int i = 0; i < 3; i++) {
    gpio_set(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
    gpio_clear(GPIO_LED_PORT, GPIO_LED_INFO);
    _msleep(150);
  }

  debugf("- Gamepads init...\n");
  gamepads_init(gamepads, 4);

  debugf("- Storage init...\n");
  storage_init();
  //storage_clear();

  //Временно. Просто проверка, что оно работает

  storage_map_t map = storage_get_macro_list();

  for (uint8_t i = 0; i < STORAGE_MAX_MACRO; ++i) {
    if (map.blocks[i] == STORAGE_BLOCK_CLEAR) {
      continue;
    }

    storage_btns_t buttons;
    storage_macro_t macro;

    storage_get_macro(i, &macro, &buttons);

    gamepad_cheat_t *cheat = gamepads_cheat_init();

    for (uint8_t j = 0; j < macro.act_count; ++j) {
      gamepad_cheat_btn_t btn;

      btn.button = (buttons.acts + j)->btn;
      btn.delay_time = 0;
      btn.press_time = 0;
      gamepads_cheat_add_btn(cheat->act_buttons, btn);
    }

    for (uint8_t j = 0; j < macro.press_count; ++j) {
      gamepad_cheat_btn_t btn;

      btn.button = (buttons.pressed + j)->btn;
      btn.delay_time = (buttons.pressed + j)->time_delay;
      btn.press_time = (buttons.pressed + j)->time_press;
      btn.keep_pressed = (buttons.pressed + j)->keep;
      gamepads_cheat_add_btn(cheat->press_buttons, btn);
    }

    free(buttons.acts);
    free(buttons.pressed);

    gamepads_cheat_add(cheat, 1);
  }

  gamepads_en();
  while(1) {
    usb_hid_poll();
  }
  gamepads_den();

  return 0;
}
