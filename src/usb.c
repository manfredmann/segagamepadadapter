#include "usb.h"
#include "init.h"
#include "gpio.h"
#include "usb_hid.h"

static usbd_device *usbd_dev;

static const struct usb_device_descriptor dev = {
  .bLength            = USB_DT_DEVICE_SIZE,
  .bDescriptorType    = USB_DT_DEVICE,
  .bcdUSB             = 0x0200,
  .bDeviceClass       = 0xEF,
  .bDeviceSubClass    = 2,
  .bDeviceProtocol    = 1,
  .bMaxPacketSize0    = 64,
  .idVendor           = 0x1209,
  .idProduct          = 0xCC86,
  .bcdDevice          = 0x0003,
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 3,
  .bNumConfigurations = 1,
};


static const struct usb_interface ifaces[] = {
  {
    .num_altsetting = 1,
    .altsetting     = &hid_iface_j0,
  },
  {
    .num_altsetting = 1,
    .altsetting     = &hid_iface_j1,
  },
  {
    .num_altsetting = 1,
    .altsetting     = &hid_iface_j2,
  },
  {
    .num_altsetting = 1,
    .altsetting     = &hid_iface_j3,
  },
};

static const struct usb_config_descriptor config = {
  .bLength              = USB_DT_CONFIGURATION_SIZE,
  .bDescriptorType      = USB_DT_CONFIGURATION,
  .wTotalLength         = 0,
  .bNumInterfaces       = 4,
  .bConfigurationValue  = 1,
  .iConfiguration       = 0,
  .bmAttributes         = 0x80,
  .bMaxPower            = 0xFA,
  .interface            = ifaces,
};

static const char *usb_strings[] = {
  "Manfred's Technologies(c)",
  "SEGA Gamepad adapter",
  "DEADBEEF",
};

static uint8_t usbd_control_buffer[256];

static void usb_set_config(usbd_device *usbd_dev, uint16_t wValue) {
  (void) wValue;
  (void) usbd_dev;

  usb_hid_setconfig(usbd_dev);
}

void usb_init(void) {
  nvic_enable_irq(NVIC_USB_LP_CAN_RX0_IRQ);
  nvic_enable_irq(NVIC_USB_HP_CAN_TX_IRQ);
  nvic_enable_irq(NVIC_USB_WAKEUP_IRQ);

  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_USB);

  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);

  gpio_set(GPIOB, GPIO4);
  _msleep(10);

  gpio_clear(GPIOB, GPIO4);
  _usleep(200);

  usbd_dev = usbd_init(&st_usbfs_v1_usb_driver,
                       &dev,
                       &config,
                       usb_strings,
                       3,
                       usbd_control_buffer,
                       sizeof(usbd_control_buffer)
  );

  usbd_register_set_config_callback(usbd_dev, usb_set_config);
}

void usb_wakeup_isr(void) {
  usbd_poll(usbd_dev);
}

void usb_hp_can_tx_isr(void) {
  usbd_poll(usbd_dev);
}

void usb_lp_can_rx0_isr(void) {
  usbd_poll(usbd_dev);
}
