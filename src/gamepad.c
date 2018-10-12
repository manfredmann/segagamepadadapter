#include "gamepad.h"

#define DELAY() for (volatile uint8_t i = 0; i < 4; i++){}

void gamepads_init(void) {
  rcc_periph_clock_enable(GP12_RCC);
  rcc_periph_clock_enable(GP_S_RCC);

  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP1_DATA0);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP1_DATA1);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP1_DATA2);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP1_DATA3);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP1_DATA4);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP1_DATA5);

  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP2_DATA0);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP2_DATA1);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP2_DATA2);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP2_DATA3);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP2_DATA4);
  gpio_set_mode(GP12_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP2_DATA5);

  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP3_DATA0);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP3_DATA1);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP3_DATA2);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP3_DATA3);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP3_DATA4);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP3_DATA5);

  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP4_DATA0);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP4_DATA1);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP4_DATA2);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP4_DATA3);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP4_DATA4);
  gpio_set_mode(GP34_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GP4_DATA5);

  gpio_set_mode(GP_S_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GP1_SELECT);
  gpio_set_mode(GP_S_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GP2_SELECT);
  gpio_set_mode(GP_S_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GP3_SELECT);
  gpio_set_mode(GP_S_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GP4_SELECT);

  gpio_set_mode(GP_S_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GP_EN);
}

static gamepad_data_t gamepad_data;

gamepad_data_t *gamepad_read(uint8_t gamepad) {
  uint8_t         gamepads[GAMEPADS_COUNT][12];

  uint32_t gp_port;
  uint32_t gp_s_port;
  uint16_t gp_s;
  uint16_t gp_data0;
  uint16_t gp_data1;
  uint16_t gp_data2;
  uint16_t gp_data3;
  uint16_t gp_data4;
  uint16_t gp_data5;


  switch (gamepad) {
    default:
    case 0: {
      gp_port   = GP12_PORT;
      gp_data0  = GP1_DATA0;
      gp_data1  = GP1_DATA1;
      gp_data2  = GP1_DATA2;
      gp_data3  = GP1_DATA3;
      gp_data4  = GP1_DATA4;
      gp_data5  = GP1_DATA5;

      gp_s_port = GP_S_PORT;
      gp_s      = GP1_SELECT;
      break;
    }
    case 1: {
      gp_port   = GP12_PORT;
      gp_data0  = GP2_DATA0;
      gp_data1  = GP2_DATA1;
      gp_data2  = GP2_DATA2;
      gp_data3  = GP2_DATA3;
      gp_data4  = GP2_DATA4;
      gp_data5  = GP2_DATA5;

      gp_s_port = GP_S_PORT;
      gp_s      = GP2_SELECT;
      break;
    }
    case 2: {
      gp_port   = GP34_PORT;
      gp_data0  = GP3_DATA0;
      gp_data1  = GP3_DATA1;
      gp_data2  = GP3_DATA2;
      gp_data3  = GP3_DATA3;
      gp_data4  = GP3_DATA4;
      gp_data5  = GP3_DATA5;

      gp_s_port = GP_S_PORT;
      gp_s      = GP3_SELECT;
    }
    case 3: {
      gp_port   = GP34_PORT;
      gp_data0  = GP4_DATA0;
      gp_data1  = GP4_DATA1;
      gp_data2  = GP4_DATA2;
      gp_data3  = GP4_DATA3;
      gp_data4  = GP4_DATA4;
      gp_data5  = GP4_DATA5;

      gp_s_port = GP_S_PORT;
      gp_s      = GP1_SELECT;
    }
  }

  gpio_clear(gp_s_port, gp_s);
  DELAY();

  gamepads[0][BTN_A]      = !gpio_get(gp_port, gp_data4);
  gamepads[0][BTN_START]  = !gpio_get(gp_port, gp_data5);

  gpio_set(gp_s_port, gp_s);
  DELAY();

  gamepads[0][BTN_UP]     = !gpio_get(gp_port, gp_data0);
  gamepads[0][BTN_DOWN]   = !gpio_get(gp_port, gp_data1);
  gamepads[0][BTN_LEFT]   = !gpio_get(gp_port, gp_data2);
  gamepads[0][BTN_RIGHT]  = !gpio_get(gp_port, gp_data3);
  gamepads[0][BTN_B]      = !gpio_get(gp_port, gp_data4);
  gamepads[0][BTN_C]      = !gpio_get(gp_port, gp_data5);

  gpio_clear(gp_s_port, gp_s);
  DELAY();

  gpio_set(gp_s_port, gp_s);
  DELAY();

  gpio_clear(gp_s_port, gp_s);
  DELAY();

  gpio_set(gp_s_port, gp_s);
  DELAY();

  gamepads[0][BTN_MODE]   = !gpio_get(gp_port, gp_data3);
  gamepads[0][BTN_X]      = !gpio_get(gp_port, gp_data2);
  gamepads[0][BTN_Y]      = !gpio_get(gp_port, gp_data1);
  gamepads[0][BTN_Z]      = !gpio_get(gp_port, gp_data0);

  gpio_clear(gp_s_port, gp_s);
  DELAY();

  gpio_set(gp_s_port, gp_s);
  DELAY();

  if (gamepad > 0) {
    gamepad_data.buttons  = 0x00;
    gamepad_data.y = 0;
    gamepad_data.x = 0;
    return &gamepad_data;
  }

  gamepad_data.buttons  = 0x00;
  gamepad_data.buttons |= (gamepads[gamepad][BTN_A] & 0x1) << 0;
  gamepad_data.buttons |= (gamepads[gamepad][BTN_B] & 0x1) << 1;
  gamepad_data.buttons |= (gamepads[gamepad][BTN_C] & 0x1) << 2;
  gamepad_data.buttons |= (gamepads[gamepad][BTN_X] & 0x1) << 3;
  gamepad_data.buttons |= (gamepads[gamepad][BTN_Y] & 0x1) << 4;
  gamepad_data.buttons |= (gamepads[gamepad][BTN_Z] & 0x1) << 5;
  gamepad_data.buttons |= (gamepads[gamepad][BTN_START] & 0x1) << 6;
  gamepad_data.buttons |= (gamepads[gamepad][BTN_MODE] & 0x1) << 7;

  if (gamepads[gamepad][BTN_UP]) {
    gamepad_data.y = -127;
  } else if (gamepads[gamepad][BTN_DOWN]) {
    gamepad_data.y = 127;
  } else {
    gamepad_data.y = 0;
  }

  if (gamepads[gamepad][BTN_LEFT]) {
    gamepad_data.x = -127;
  } else if (gamepads[gamepad][BTN_RIGHT]) {
    gamepad_data.x = 127;
  } else {
    gamepad_data.x = 0;
  }

  return &gamepad_data;
}
