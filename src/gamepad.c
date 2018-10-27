#include "gamepad.h"
#include <stdlib.h>

#define DELAY()       for (volatile uint8_t i = 0; i < 3; i++){}

static gamepad_data_t         gamepad_data;
static uint8_t                gamepad_count;
static gamepad_buttons_t      gamepad_buttons;
static gamepad_gpio_t         *gamepad_gpio   = NULL;
static list_t                 *gamepad_macros = NULL;

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

  gamepad_buttons = malloc(sizeof(uint8_t) * 12 * gamepad_count);
  gamepad_macros = malloc(sizeof(list_t) * gamepad_count);

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

    gamepad_macros[i].head    = NULL;
    gamepad_macros[i].bottom  = NULL;
    gamepad_macros[i].size    = 0;
  }
}

gamepad_macro_t *gamepads_macro_init(void) {
  gamepad_macro_t *macro = malloc(sizeof(gamepad_macro_t));

  macro->act_buttons = list_init();
  macro->press_buttons = list_init();

  return macro;
}

void gamepads_macro_add(gamepad_macro_t *macro, uint8_t gamepad) {
  list_push_back(&gamepad_macros[gamepad], macro);
}

void gamepads_macro_add_btn(list_t *buttons, gamepad_macro_btn_t btn) {
  gamepad_macro_btn_t *tmp = malloc(sizeof(gamepad_macro_btn_t));
  *tmp = btn;
  list_push_back(buttons, tmp);
}

void gamepads_macro_clear(void) {
  list_node_t *node       = NULL;

  for (uint8_t gamepad = 0; gamepad < gamepad_count; ++gamepad) {
    while ((node = list_iter(&gamepad_macros[gamepad], node)) != NULL) {
      gamepad_macro_t *macro = (gamepad_macro_t *) node->value;

      list_clear(macro->act_buttons);
      list_clear(macro->press_buttons);
      free(macro->act_buttons);
      free(macro->press_buttons);
    }
    list_clear(&gamepad_macros[gamepad]);
  }
}

static void gamepads_macro_accept(gamepad_macro_t *macro, gamepad_buttons_t buttons) {
  bool activate = false;

  list_node_t *node = NULL;

  while ((node = list_iter(macro->act_buttons, node)) != NULL) {
    gamepad_macro_btn_t *btn = (gamepad_macro_btn_t *) node->value;
    if (buttons[btn->button] & 0x1) {
      activate = true;
    } else {
      activate = false;
      break;
    }
  }

  node = NULL;

  while ((node = list_iter(macro->press_buttons, node)) != NULL) {
    gamepad_macro_btn_t *btn = (gamepad_macro_btn_t *) node->value;

    if (activate) {
      if (btn->time_delay_last == 0) {
        if (!btn->keep_pressed) {
          if (btn->time_press_last > 0) {
            buttons[btn->button] = 0xFF;
            --btn->time_press_last;
          }
        } else {
          buttons[btn->button] = 0xFF;
        }
      } else {
        --btn->time_delay_last;
      }
    } else {
      btn->time_delay_last = btn->time_delay;
      btn->time_press_last = btn->time_press;
    }
  }
}

gamepad_data_t *gamepad_read(uint8_t gamepad) {
  if (gamepad_gpio == NULL) {
    return NULL;
  }

  gamepad_gpio_t    *gp     = &gamepad_gpio[gamepad];
  gamepad_buttons_t buttons = gamepad_buttons + (gamepad * 12);

  gpio_clear(gp->select.port, gp->select.pin);
  DELAY();

  buttons[BTN_A]      = !gpio_get(gp->data4.port, gp->data4.pin);
  buttons[BTN_START]  = !gpio_get(gp->data5.port, gp->data5.pin);

  gpio_set(gp->select.port,   gp->select.pin);
  DELAY();

  buttons[BTN_UP]     = !gpio_get(gp->data0.port, gp->data0.pin);
  buttons[BTN_DOWN]   = !gpio_get(gp->data1.port, gp->data1.pin);
  buttons[BTN_LEFT]   = !gpio_get(gp->data2.port, gp->data2.pin);
  buttons[BTN_RIGHT]  = !gpio_get(gp->data3.port, gp->data3.pin);
  buttons[BTN_B]      = !gpio_get(gp->data4.port, gp->data4.pin);
  buttons[BTN_C]      = !gpio_get(gp->data5.port, gp->data5.pin);

  gpio_clear(gp->select.port, gp->select.pin);
  DELAY();

  gpio_set(gp->select.port,   gp->select.pin);
  DELAY();

  gpio_clear(gp->select.port, gp->select.pin);
  DELAY();

  gpio_set(gp->select.port,   gp->select.pin);
  DELAY();

  buttons[BTN_MODE]   = !gpio_get(gp->data3.port, gp->data3.pin);
  buttons[BTN_X]      = !gpio_get(gp->data2.port, gp->data2.pin);
  buttons[BTN_Y]      = !gpio_get(gp->data1.port, gp->data1.pin);
  buttons[BTN_Z]      = !gpio_get(gp->data0.port, gp->data0.pin);

  gpio_clear(gp->select.port, gp->select.pin);
  DELAY();

  gpio_set(gp->select.port,   gp->select.pin);
  DELAY();

  // Применим макросы
  list_node_t *node = NULL;
  while ((node = list_iter(&gamepad_macros[gamepad], node)) != NULL) {
    gamepad_macro_t *macro = (gamepad_macro_t *) node->value;
    gamepads_macro_accept(macro, buttons);
  }

  gamepad_data.buttons  = 0x00;
  gamepad_data.buttons |= (buttons[BTN_A]     & 0x1) << 0;
  gamepad_data.buttons |= (buttons[BTN_B]     & 0x1) << 1;
  gamepad_data.buttons |= (buttons[BTN_C]     & 0x1) << 2;
  gamepad_data.buttons |= (buttons[BTN_X]     & 0x1) << 3;
  gamepad_data.buttons |= (buttons[BTN_Y]     & 0x1) << 4;
  gamepad_data.buttons |= (buttons[BTN_Z]     & 0x1) << 5;
  gamepad_data.buttons |= (buttons[BTN_START] & 0x1) << 6;
  gamepad_data.buttons |= (buttons[BTN_MODE]  & 0x1) << 7;

  if (buttons[BTN_UP]) {
    gamepad_data.y = -127;
  } else if (buttons[BTN_DOWN]) {
    gamepad_data.y = 127;
  } else {
    gamepad_data.y = 0;
  }

  if (buttons[BTN_LEFT]) {
    gamepad_data.x = -127;
  } else if (buttons[BTN_RIGHT]) {
    gamepad_data.x = 127;
  } else {
    gamepad_data.x = 0;
  }

  return &gamepad_data;
}
