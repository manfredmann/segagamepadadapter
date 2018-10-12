#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "misc.h"
#include "gpio.h"

#define GP12_RCC   RCC_GPIOA
#define GP12_PORT  GPIOA
#define GP34_RCC   RCC_GPIOC
#define GP34_PORT  GPIOC

#define GP_S_RCC      RCC_GPIOB
#define GP_S_PORT     GPIOB

#define GP_EN      GPIO8

#define GP1_DATA0  GPIO0
#define GP1_DATA1  GPIO1
#define GP1_DATA2  GPIO2
#define GP1_DATA3  GPIO3
#define GP1_DATA4  GPIO4
#define GP1_DATA5  GPIO5
#define GP1_SELECT GPIO0

#define GP2_DATA0  GPIO6
#define GP2_DATA1  GPIO7
#define GP2_DATA2  GPIO8
#define GP2_DATA3  GPIO9
#define GP2_DATA4  GPIO10
#define GP2_DATA5  GPIO15
#define GP2_SELECT GPIO1

#define GP3_DATA0  GPIO0
#define GP3_DATA1  GPIO1
#define GP3_DATA2  GPIO2
#define GP3_DATA3  GPIO3
#define GP3_DATA4  GPIO4
#define GP3_DATA5  GPIO5
#define GP3_SELECT GPIO2

#define GP4_DATA0  GPIO6
#define GP4_DATA1  GPIO7
#define GP4_DATA2  GPIO8
#define GP4_DATA3  GPIO9
#define GP4_DATA4  GPIO10
#define GP4_DATA5  GPIO11
#define GP4_SELECT GPIO3

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

#define GAMEPADS_COUNT  4

#define gamepads_en()   gpio_clear(GP_S_PORT, GP_EN)
#define gamepads_den()  gpio_set(GP_S_PORT, GP_EN)

typedef struct {
  uint8_t buttons;
  int8_t x;
  int8_t y;
} gamepad_data_t;

void             gamepads_init(void);
gamepad_data_t  *gamepad_read(uint8_t gamepad);

#endif
