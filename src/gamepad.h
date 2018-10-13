#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "misc.h"
#include "gpio.h"

#define BTN_UP    0
#define BTN_DOWN  1
#define BTN_LEFT  2
#define BTN_RIGHT 3
#define BTN_A     4
#define BTN_B     5
#define BTN_C     6
#define BTN_X     7
#define BTN_Y     8
#define BTN_Z     9
#define BTN_START 10
#define BTN_MODE  11

#define gamepads_en()   gpio_clear(GPIOB, GPIO8)
#define gamepads_den()  gpio_set(GPIOB, GPIO8)

typedef struct {
  uint8_t buttons;
  int8_t x;
  int8_t y;
} gamepad_data_t;

typedef struct {
  uint32_t port;
  uint16_t pin;
} gamepad_pin_t;

typedef struct {
  gamepad_pin_t data0;
  gamepad_pin_t data1;
  gamepad_pin_t data2;
  gamepad_pin_t data3;
  gamepad_pin_t data4;
  gamepad_pin_t data5;
  gamepad_pin_t select;
} gamepad_gpio_t;

/*typedef struct {
  uint8_t button;
  uint8_t pressed[12];
} gamepad_simple_cheat_t;*/

typedef uint8_t gamepad_buttons_t[12];

void             gamepads_init(gamepad_gpio_t *gamepads, uint8_t count);
//void             gamepads_cheat_init(gamepad_simple_cheat_t *cheat);
//void             gamepads_cheat_set(gamepad_simple_cheat_t cheat, uint8_t n);
//void             gamepads_cheat_unset(uint8_t n);

gamepad_data_t  *gamepad_read(uint8_t gamepad);

#endif
