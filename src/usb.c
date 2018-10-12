#include "usb.h"
#include "init.h"
#include "gpio.h"
#include "hid_def.h"

void endpoint_in_callback(usbd_device *usbd, uint8_t ep);
void endpoint_out_callback(usbd_device *usbd, uint8_t ep);

static const struct usb_device_descriptor dev = {
  .bLength            = USB_DT_DEVICE_SIZE,
  .bDescriptorType    = USB_DT_DEVICE,
  .bcdUSB             = 0x0200,
  .bDeviceClass       = 0,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize0    = 64,
  .idVendor           = 0x1209,
  .idProduct          = 0xCC86,
  .bcdDevice          = 0x0300,
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 3,
  .bNumConfigurations = 1,
};

static uint8_t hid_report_descriptor[] = {
  HID_USAGE_PAGE (GENERIC_DESKTOP),
  HID_USAGE(JOYSTICK),
  HID_COLLECTION (APPLICATION),
    HID_REPORT_ID(1),
    HID_COLLECTION (PHYSICAL),
      HID_USAGE_PAGE (BUTTONS),
      HID_USAGE_MINIMUM (1, 1),
      HID_USAGE_MAXIMUM (1, 8),
      HID_LOGICAL_MINIMUM (1, 0),
      HID_LOGICAL_MAXIMUM (1, 1),
      HID_REPORT_COUNT (8),
      HID_REPORT_SIZE (1),
      HID_INPUT (DATA, VARIABLE, ABSOLUTE),
      HID_USAGE_PAGE (GENERIC_DESKTOP),
      HID_USAGE (X),
      HID_USAGE (Y),
      HID_LOGICAL_MINIMUM (1, (uint8_t) -127),
      HID_LOGICAL_MAXIMUM (1, 127),
      HID_REPORT_SIZE (8),
      HID_REPORT_COUNT (2),
      HID_INPUT (DATA, VARIABLE, ABSOLUTE),
    HID_END_COLLECTION (PHYSICAL),
  HID_END_COLLECTION (APPLICATION),
  HID_USAGE_PAGE (GENERIC_DESKTOP),
  HID_USAGE(JOYSTICK),
  HID_COLLECTION (APPLICATION),
    HID_REPORT_ID(2),
    HID_COLLECTION (PHYSICAL),
      HID_USAGE_PAGE (BUTTONS),
      HID_USAGE_MINIMUM (1, 1),
      HID_USAGE_MAXIMUM (1, 8),
      HID_LOGICAL_MINIMUM (1, 0),
      HID_LOGICAL_MAXIMUM (1, 1),
      HID_REPORT_COUNT (8),
      HID_REPORT_SIZE (1),
      HID_INPUT (DATA, VARIABLE, ABSOLUTE),
      HID_USAGE_PAGE (GENERIC_DESKTOP),
      HID_USAGE (X),
      HID_USAGE (Y),
      HID_LOGICAL_MINIMUM (1, (uint8_t) -127),
      HID_LOGICAL_MAXIMUM (1, 127),
      HID_REPORT_SIZE (8),
      HID_REPORT_COUNT (2),
      HID_INPUT (DATA, VARIABLE, ABSOLUTE),
    HID_END_COLLECTION (PHYSICAL),
  HID_END_COLLECTION (APPLICATION),
  HID_USAGE_PAGE (GENERIC_DESKTOP),
  HID_USAGE(JOYSTICK),
  HID_COLLECTION (APPLICATION),
    HID_REPORT_ID(3),
    HID_COLLECTION (PHYSICAL),
      HID_USAGE_PAGE (BUTTONS),
      HID_USAGE_MINIMUM (1, 1),
      HID_USAGE_MAXIMUM (1, 8),
      HID_LOGICAL_MINIMUM (1, 0),
      HID_LOGICAL_MAXIMUM (1, 1),
      HID_REPORT_COUNT (8),
      HID_REPORT_SIZE (1),
      HID_INPUT (DATA, VARIABLE, ABSOLUTE),
      HID_USAGE_PAGE (GENERIC_DESKTOP),
      HID_USAGE (X),
      HID_USAGE (Y),
      HID_LOGICAL_MINIMUM (1, (uint8_t) -127),
      HID_LOGICAL_MAXIMUM (1, 127),
      HID_REPORT_SIZE (8),
      HID_REPORT_COUNT (2),
      HID_INPUT (DATA, VARIABLE, ABSOLUTE),
    HID_END_COLLECTION (PHYSICAL),
  HID_END_COLLECTION (APPLICATION),
  HID_USAGE_PAGE (GENERIC_DESKTOP),
  HID_USAGE(JOYSTICK),
  HID_COLLECTION (APPLICATION),
    HID_REPORT_ID(4),
    HID_COLLECTION (PHYSICAL),
      HID_USAGE_PAGE (BUTTONS),
      HID_USAGE_MINIMUM (1, 1),
      HID_USAGE_MAXIMUM (1, 8),
      HID_LOGICAL_MINIMUM (1, 0),
      HID_LOGICAL_MAXIMUM (1, 1),
      HID_REPORT_COUNT (8),
      HID_REPORT_SIZE (1),
      HID_INPUT (DATA, VARIABLE, ABSOLUTE),
      HID_USAGE_PAGE (GENERIC_DESKTOP),
      HID_USAGE (X),
      HID_USAGE (Y),
      HID_LOGICAL_MINIMUM (1, (uint8_t) -127),
      HID_LOGICAL_MAXIMUM (1, 127),
      HID_REPORT_SIZE (8),
      HID_REPORT_COUNT (2),
      HID_INPUT (DATA, VARIABLE, ABSOLUTE),
    HID_END_COLLECTION (PHYSICAL),
  HID_END_COLLECTION (APPLICATION),
};

static const struct {
  struct usb_hid_descriptor hid_descriptor;
  struct {
    uint8_t bReportDescriptorType;
    uint16_t wDescriptorLength;
  } __attribute__((packed)) hid_report;
} __attribute__((packed)) hid_function = {
  .hid_descriptor = {
    .bLength          = sizeof(hid_function),
    .bDescriptorType  = USB_DT_HID,
    .bcdHID           = 0x0111,
    .bCountryCode     = 23,
    .bNumDescriptors  = 1,
  },
  .hid_report = {
    .bReportDescriptorType  = USB_DT_REPORT,
    .wDescriptorLength      = sizeof(hid_report_descriptor),
  },
};

static const struct usb_endpoint_descriptor hid_endpoints[] = {
  {
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x81,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = 4,
    .bInterval        = 0x01,
  },
/*  {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x01,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = 16,
    .bInterval        = 0x08,
  }*/
};

static const struct usb_interface_descriptor hid_iface = {
  .bLength            = USB_DT_INTERFACE_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE,
  .bInterfaceNumber   = 0,
  .bAlternateSetting  = 0,
  .bNumEndpoints      = 1,
  .bInterfaceClass    = USB_CLASS_HID,
  .bInterfaceSubClass = 0,
  .bInterfaceProtocol = 0,
  .iInterface         = 0,

  .endpoint           = hid_endpoints,

  .extra              = &hid_function,
  .extralen           = sizeof(hid_function),
};

static const struct usb_interface ifaces[] = {{
  .num_altsetting = 1,
  .altsetting     = &hid_iface,
}};

static const struct usb_config_descriptor config = {
  .bLength              = USB_DT_CONFIGURATION_SIZE,
  .bDescriptorType      = USB_DT_CONFIGURATION,
  .wTotalLength         = 0,
  .bNumInterfaces       = 1,
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

static uint8_t usbd_control_buffer[128];

static int hid_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
      void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req)) {
  (void) complete;
  (void) usbd_dev;

  if ((req->bmRequestType != 0x81) ||
     (req->bRequest != USB_REQ_GET_DESCRIPTOR) ||
     (req->wValue != 0x2200))
    return 0;

  *buf = (uint8_t *)hid_report_descriptor;
  *len = sizeof(hid_report_descriptor);
  return 1;
}

static void hid_set_config(usbd_device *usbd_dev, uint16_t wValue) {
  (void) wValue;
  (void) usbd_dev;

  usbd_ep_setup(usbd_dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, 4, endpoint_in_callback);
  //usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_INTERRUPT, 64, endpoint_out_callback);

  usbd_register_control_callback(usbd_dev,
                                 USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
                                 USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
                                 hid_control_request);
}

static usbd_device *usbd_dev;
static uint8_t      cycle       = 0;
static uint8_t      cycle_count = 0;

void usb_init(uint8_t gamepads_count) {
  cycle_count = gamepads_count;

  nvic_enable_irq(NVIC_USB_LP_CAN_RX0_IRQ);
  nvic_enable_irq(NVIC_USB_WAKEUP_IRQ);

  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_USB);

  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);

  gpio_set(GPIOB, GPIO4);
  _msleep(250);

  gpio_clear(GPIOB, GPIO4);
  _msleep(250);

  usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
  usbd_register_set_config_callback(usbd_dev, hid_set_config);
}

void usb_send(void) {
  if (cycle_count != 0)
    usbd_ep_write_packet(usbd_dev, 0x01, NULL, 0);
}

void endpoint_in_callback(usbd_device *usbd, uint8_t ep) {
  (void) usbd;
  (void) ep;

  if ((cycle + 1) % 2) {
    led_info(true);
  } else {
    led_info(false);
  }

  gamepad_data_t *gamepad_data = gamepad_read(cycle);

  uint8_t buf[4];

  buf[0] = cycle + 1;
  buf[1] = gamepad_data->buttons;
  buf[2] = (uint8_t) gamepad_data->x;
  buf[3] = (uint8_t) gamepad_data->y;

  usbd_ep_write_packet(usbd, ep, buf, sizeof(buf));

  if ((cycle + 1) == cycle_count) {
    cycle = 0;
  } else {
    ++cycle;
  }
}

void endpoint_out_callback(usbd_device *usbd, uint8_t ep) {
  (void) usbd;
  (void) ep;
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
