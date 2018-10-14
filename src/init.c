#include "init.h"

volatile uint32_t system_millis = 0;

void clock_init(void) {
  rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

void sys_tick_handler(void) {
  system_millis++;
}

void _usleep(uint32_t delay) {
  uint32_t wake = system_millis + delay;
  while (wake > system_millis);
}

void _msleep(uint32_t delay) {
  _usleep(delay * 1000);
}

void systick_init(void) {
  systick_set_reload(72);
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  systick_counter_enable();
  systick_interrupt_enable();
}

void uart_init(void) {
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_USART1);
  rcc_periph_clock_enable(RCC_AFIO);

  gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON, AFIO_MAPR_USART1_REMAP);

  gpio_set_mode(GPIO_BANK_USART1_RE_TX, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_USART1_RE_TX);
  gpio_set_mode(GPIO_BANK_USART1_RE_RX, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RE_RX);

  usart_set_baudrate(USART1, 921600);
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_mode(USART1, USART_MODE_TX_RX);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

  usart_enable(USART1);
}
