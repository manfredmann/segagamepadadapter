#include "usb_hid.h"
#include "gpio.h"
#include "hid_def.h"
#include "gamepad.h"

static uint8_t hid_report_descriptor[] = {
  HID_USAGE_PAGE (GENERIC_DESKTOP),
  HID_USAGE(JOYSTICK),
  HID_COLLECTION (APPLICATION),
    HID_COLLECTION (PHYSICAL),
      HID_USAGE_PAGE (BUTTONS),
      HID_USAGE_MINIMUM (1, 1),
      HID_USAGE_MAXIMUM (1, 8),
      HID_LOGICAL_MINIMUM (1, 0),
      HID_LOGICAL_MAXIMUM (1, 1),
      HID_REPORT_COUNT (8),
      HID_REPORT_SIZE (1),
      HID_INPUT (DATA, VARIABLE, ABSOLUTE),
  HID_END_COLLECTION (PHYSICAL),
  HID_COLLECTION (PHYSICAL),
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

static const struct usb_endpoint_descriptor hid_endpoints_j0[] = {
  {
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x81,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = 6,
    .bInterval        = 0x02,
  },
};

static const struct usb_endpoint_descriptor hid_endpoints_j1[] = {
  {
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x82,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = 6,
    .bInterval        = 0x02,
  },
};

static const struct usb_endpoint_descriptor hid_endpoints_j2[] = {
  {
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x83,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = 6,
    .bInterval        = 0x02,
  },
};

static const struct usb_endpoint_descriptor hid_endpoints_j3[] = {
  {
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x84,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = 6,
    .bInterval        = 0x02,
  },
};

struct usb_interface_descriptor hid_iface_j0 = {
  .bLength            = USB_DT_INTERFACE_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE,
  .bInterfaceNumber   = 0,
  .bAlternateSetting  = 0,
  .bNumEndpoints      = 1,
  .bInterfaceClass    = USB_CLASS_HID,
  .bInterfaceSubClass = 0,
  .bInterfaceProtocol = 0,
  .iInterface         = 4,

  .endpoint           = hid_endpoints_j0,

  .extra              = &hid_function,
  .extralen           = sizeof(hid_function),
};

struct usb_interface_descriptor hid_iface_j1 = {
  .bLength            = USB_DT_INTERFACE_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE,
  .bInterfaceNumber   = 1,
  .bAlternateSetting  = 0,
  .bNumEndpoints      = 1,
  .bInterfaceClass    = USB_CLASS_HID,
  .bInterfaceSubClass = 0,
  .bInterfaceProtocol = 0,
  .iInterface         = 5,

  .endpoint           = hid_endpoints_j1,

  .extra              = &hid_function,
  .extralen           = sizeof(hid_function),
};

struct usb_interface_descriptor hid_iface_j2 = {
  .bLength            = USB_DT_INTERFACE_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE,
  .bInterfaceNumber   = 2,
  .bAlternateSetting  = 0,
  .bNumEndpoints      = 1,
  .bInterfaceClass    = USB_CLASS_HID,
  .bInterfaceSubClass = 0,
  .bInterfaceProtocol = 0,
  .iInterface         = 6,

  .endpoint           = hid_endpoints_j2,

  .extra              = &hid_function,
  .extralen           = sizeof(hid_function),
};

struct usb_interface_descriptor hid_iface_j3 = {
  .bLength            = USB_DT_INTERFACE_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE,
  .bInterfaceNumber   = 3,
  .bAlternateSetting  = 0,
  .bNumEndpoints      = 1,
  .bInterfaceClass    = USB_CLASS_HID,
  .bInterfaceSubClass = 0,
  .bInterfaceProtocol = 0,
  .iInterface         = 7,

  .endpoint           = hid_endpoints_j3,

  .extra              = &hid_function,
  .extralen           = sizeof(hid_function),
};

static enum usbd_request_return_codes hid_control_request(usbd_device *usbd_dev,
                                                          struct usb_setup_data *req,
                                                          uint8_t **buf,
                                                          uint16_t *len,
                                                          usbd_control_complete_callback *complete) {
  (void) complete;
  (void) usbd_dev;

  switch(req->bRequest) {
    case USB_REQ_GET_DESCRIPTOR: {
      *buf = (uint8_t *)hid_report_descriptor;
      *len = sizeof(hid_report_descriptor);
      return USBD_REQ_HANDLED;
    }
    default:
      return USBD_REQ_NOTSUPP;
  }
}

static void endpoint_in_callback(usbd_device *usbd, uint8_t ep) {
  (void) usbd;
  (void) ep;

  uint8_t buf[3];

  //Так как на макете только один геймпад, отправляем информацию только от первого

  switch (ep) {
    case 0x01: {
      gamepad_data_t *gamepad_data = gamepad_read(0);
      buf[0] = gamepad_data->buttons;
      buf[1] = (uint8_t) gamepad_data->x;
      buf[2] = (uint8_t) gamepad_data->y;
      break;
    }
    case 0x02: {
      buf[0] = 0x00;
      buf[1] = 0;
      buf[2] = 0;
      break;
    }
    case 0x03: {
      buf[0] = 0x00;
      buf[1] = 0;
      buf[2] = 0;
      break;
    }
    case 0x04: {
      buf[0] = 0x00;
      buf[1] = 0;
      buf[2] = 0;
      break;
    }
  }

  usbd_ep_write_packet(usbd, ep, buf, sizeof(buf));
}

static usbd_device *usbd_dev = NULL;

void usb_hid_setconfig(usbd_device *dev) {
  usbd_dev = dev;

  usbd_ep_setup(dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, 6, endpoint_in_callback);
  usbd_ep_setup(dev, 0x82, USB_ENDPOINT_ATTR_INTERRUPT, 6, endpoint_in_callback);
  usbd_ep_setup(dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 6, endpoint_in_callback);
  usbd_ep_setup(dev, 0x84, USB_ENDPOINT_ATTR_INTERRUPT, 6, endpoint_in_callback);

  usbd_register_control_callback(dev,
                                 USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
                                 USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
                                 hid_control_request);
}

void usb_hid_poll(void) {
  usbd_ep_write_packet(usbd_dev, 0x81, NULL, 0);
  usbd_ep_write_packet(usbd_dev, 0x82, NULL, 0);
  usbd_ep_write_packet(usbd_dev, 0x83, NULL, 0);
  usbd_ep_write_packet(usbd_dev, 0x84, NULL, 0);
}
