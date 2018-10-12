#include "gamepad.h"
#include <stdlib.h>

#define DELAY() for (volatile uint8_t i = 0; i < 4; i++){}

static gamepad_data_t   gamepad_data;
static uint8_t          gamepad_count;
static uint8_t          gamepad_buttons[12];
static gamepad_gpio_t   *gamepad_gpio = NULL;

static void init_port(uint32_t port) {
  switch (port) {
    case GPIOA:
      rcc_periph_clock_enable(RCC_GPIOA);
      break;
    case GPIOB:
      rcc_periph_clock_enable(RCC_GPIOB);
      break;
    case GPIOC:
      rcc_periph_clock_enable(RCC_GPIOC);
      break;
    case GPIOD:
      rcc_periph_clock_enable(RCC_GPIOD);
      break;
    case GPIOE:
      rcc_periph_clock_enable(RCC_GPIOE);
      break;
    case GPIOF:
      rcc_periph_clock_enable(RCC_GPIOF);
      break;
    case GPIOG:
      rcc_periph_clock_enable(RCC_GPIOG);
      break;
  }
}

void gamepads_init(gamepad_gpio_t *gamepads, uint8_t count) {
  gamepad_count = count;
  gamepad_gpio  = gamepads;

  for (uint8_t i = 0; i < count; ++i) {
    init_port(gamepads[i].data0.port);
    init_port(gamepads[i].data1.port);
    init_port(gamepads[i].data2.port);
    init_port(gamepads[i].data3.port);
    init_port(gamepads[i].data4.port);
    init_port(gamepads[i].data5.port);
    init_port(gamepads[i].select.port);

    gpio_set_mode(gamepads[i].data0.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, gamepads[i].data0.pin);
    gpio_set_mode(gamepads[i].data1.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, gamepads[i].data1.pin);
    gpio_set_mode(gamepads[i].data2.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, gamepads[i].data2.pin);
    gpio_set_mode(gamepads[i].data3.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, gamepads[i].data3.pin);
    gpio_set_mode(gamepads[i].data4.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, gamepads[i].data4.pin);
    gpio_set_mode(gamepads[i].data5.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, gamepads[i].data5.pin);

    gpio_set_mode(gamepads[i].select.port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, gamepads[i].select.pin);

  }
}

gamepad_data_t *gamepad_read(uint8_t gamepad) {
  if (gamepad_gpio == NULL) {
    return NULL;
  }

  gamepad_gpio_t *gp = &gamepad_gpio[gamepad];

  gpio_clear(gp->select.port, gp->select.pin);
  DELAY();

  gamepad_buttons[BTN_A]      = !gpio_get(gp->data4.port, gp->data4.pin);
  gamepad_buttons[BTN_START]  = !gpio_get(gp->data5.port, gp->data5.pin);

  gpio_set(gp->select.port, gp->select.pin);
  DELAY();

  gamepad_buttons[BTN_UP]     = !gpio_get(gp->data0.port, gp->data0.pin);
  gamepad_buttons[BTN_DOWN]   = !gpio_get(gp->data1.port, gp->data1.pin);
  gamepad_buttons[BTN_LEFT]   = !gpio_get(gp->data2.port, gp->data2.pin);
  gamepad_buttons[BTN_RIGHT]  = !gpio_get(gp->data3.port, gp->data3.pin);
  gamepad_buttons[BTN_B]      = !gpio_get(gp->data4.port, gp->data4.pin);
  gamepad_buttons[BTN_C]      = !gpio_get(gp->data5.port, gp->data5.pin);

  gpio_clear(gp->select.port, gp->select.pin);
  DELAY();

  gpio_set(gp->select.port, gp->select.pin);
  DELAY();

  gpio_clear(gp->select.port, gp->select.pin);
  DELAY();

  gpio_set(gp->select.port, gp->select.pin);
  DELAY();

  gamepad_buttons[BTN_MODE]   = !gpio_get(gp->data3.port, gp->data3.pin);
  gamepad_buttons[BTN_X]      = !gpio_get(gp->data2.port, gp->data2.pin);
  gamepad_buttons[BTN_Y]      = !gpio_get(gp->data1.port, gp->data1.pin);
  gamepad_buttons[BTN_Z]      = !gpio_get(gp->data0.port, gp->data0.pin);

  gpio_clear(gp->select.port, gp->select.pin);
  DELAY();

  gpio_set(gp->select.port, gp->select.pin);
  DELAY();

  //Временная затычка. На макете только один геймпад
  if (gamepad > 0) {
    gamepad_data.buttons  = 0x00;
    gamepad_data.y = 0;
    gamepad_data.x = 0;
    return &gamepad_data;
  }

  gamepad_data.buttons  = 0x00;
  gamepad_data.buttons |= (gamepad_buttons[BTN_A]     & 0x1) << 0;
  gamepad_data.buttons |= (gamepad_buttons[BTN_B]     & 0x1) << 1;
  gamepad_data.buttons |= (gamepad_buttons[BTN_C]     & 0x1) << 2;
  gamepad_data.buttons |= (gamepad_buttons[BTN_X]     & 0x1) << 3;
  gamepad_data.buttons |= (gamepad_buttons[BTN_Y]     & 0x1) << 4;
  gamepad_data.buttons |= (gamepad_buttons[BTN_Z]     & 0x1) << 5;
  gamepad_data.buttons |= (gamepad_buttons[BTN_START] & 0x1) << 6;
  gamepad_data.buttons |= (gamepad_buttons[BTN_MODE]  & 0x1) << 7;

  if (gamepad_buttons[BTN_UP]) {
    gamepad_data.y = -127;
  } else if (gamepad_buttons[BTN_DOWN]) {
    gamepad_data.y = 127;
  } else {
    gamepad_data.y = 0;
  }

  if (gamepad_buttons[BTN_LEFT]) {
    gamepad_data.x = -127;
  } else if (gamepad_buttons[BTN_RIGHT]) {
    gamepad_data.x = 127;
  } else {
    gamepad_data.x = 0;
  }

  return &gamepad_data;
}
