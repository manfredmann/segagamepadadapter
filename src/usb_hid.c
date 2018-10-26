#include "usb_hid.h"
#include "gpio.h"
#include "hid_def.h"
#include "gamepad.h"

#define SGAD_CMD_MACRO_ADD_START  0x2007001
#define SGAD_CMD_MACRO_ACT        0x2007002
#define SGAD_CMD_MACRO_PRESS      0x2007003
#define SGAD_CMD_MACRO_ADD_STOP   0x2007004
#define SGAD_CMD_MACRO_CLEAR      0x2007005
#define SGAD_CMD_MACRO_READ_START 0x2007006
#define SGAD_CMD_MACRO_READ_STOP  0x2007007

#define SGAD_CMD_UNKNOWN          0x2007099
#define SGAD_CMD_OK               0x2007100

typedef struct {
  uint32_t cmd;
} __attribute__((packed)) sgad_cmd_t ;

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

static uint8_t hid_config_report_descriptor[] = {
  HID_USAGE_PAGE (GENERIC_DEVICE),
  HID_USAGE (UNDEFINED),
  HID_COLLECTION (APPLICATION),
    HID_LOGICAL_MINIMUM (1, 0),
    HID_LOGICAL_MAXIMUM (1, 255),
    HID_REPORT_SIZE (8),
    HID_REPORT_SIZE (64),
    HID_USAGE (UNDEFINED),
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

static const struct {
  struct usb_hid_descriptor hid_descriptor;
  struct {
    uint8_t bReportDescriptorType;
    uint16_t wDescriptorLength;
  } __attribute__((packed)) hid_report;
} __attribute__((packed)) hid_config_function = {
  .hid_descriptor = {
    .bLength          = sizeof(hid_config_function),
    .bDescriptorType  = USB_DT_HID,
    .bcdHID           = 0x0111,
    .bCountryCode     = 23,
    .bNumDescriptors  = 1,
  },
  .hid_report = {
    .bReportDescriptorType  = USB_DT_REPORT,
    .wDescriptorLength      = sizeof(hid_config_report_descriptor),
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

static const struct usb_endpoint_descriptor hid_config_endpoints[] = {
  {
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x85,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = 64,
    .bInterval        = 0x0A,
  },
  {
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x05,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = 64,
    .bInterval        = 0x0A,
  }
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

struct usb_interface_descriptor hid_iface_config = {
  .bLength            = USB_DT_INTERFACE_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE,
  .bInterfaceNumber   = 4,
  .bAlternateSetting  = 0,
  .bNumEndpoints      = 2,
  .bInterfaceClass    = USB_CLASS_HID,
  .bInterfaceSubClass = 0,
  .bInterfaceProtocol = 0,
  .iInterface         = 8,

  .endpoint           = hid_config_endpoints,

  .extra              = &hid_config_function,
  .extralen           = sizeof(hid_config_function),
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
      switch (req->wIndex) {
        default:
        case 0:
        case 1:
        case 2:
        case 3:
          *buf = (uint8_t *)hid_report_descriptor;
          *len = sizeof(hid_report_descriptor);
          break;
        case 4:
          *buf = (uint8_t *)hid_config_report_descriptor;
          *len = sizeof(hid_config_report_descriptor);
      }

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
      gamepad_data_t *gamepad_data = gamepad_read(1);
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

static void config_endpoint_in_callback(usbd_device *usbd, uint8_t ep) {
  (void) usbd;
  (void) ep;
}

static storage_macro_t *macro_new           = NULL;
static storage_btn_t   *macro_new_acts      = NULL;
static storage_btn_t   *macro_new_press     = NULL;
static uint32_t        macro_new_acts_ptr   = 0;
static uint32_t        macro_new_press_ptr  = 0;

static void config_endpoint_out_callback(usbd_device *usbd, uint8_t ep) {
  uint8_t buf[63];

  sgad_cmd_t cmd;
  sgad_cmd_t answ;

  usbd_ep_read_packet(usbd, ep, buf, sizeof(buf));
  memcpy(&cmd, buf, sizeof(sgad_cmd_t));

  //debugf("- Received cmd: 0x%04X\n", (unsigned int) cmd.cmd);

  switch (cmd.cmd) {
    case SGAD_CMD_MACRO_CLEAR: {
      answ.cmd = SGAD_CMD_OK;
      storage_clear();
      break;
    }
    case SGAD_CMD_MACRO_ADD_START: {
      answ.cmd = SGAD_CMD_OK;

      if (macro_new != NULL) {
        free(macro_new);
        macro_new = NULL;

        if (macro_new_acts != NULL) {
          free(macro_new_acts);
          macro_new_acts = NULL;
        }

        if (macro_new_press != NULL) {
          free(macro_new_press);
          macro_new_press = NULL;
        }
      }

      macro_new = malloc(sizeof(storage_macro_t));
      memcpy(macro_new, buf + sizeof(sgad_cmd_t), sizeof(storage_macro_t));

      if (macro_new->act_count == 0 || macro_new->press_count == 0) {
        free(macro_new);
        answ.cmd = SGAD_CMD_UNKNOWN;
        break;
      }

      macro_new_acts      = malloc(sizeof(storage_btn_t) * macro_new->act_count);
      macro_new_press     = malloc(sizeof(storage_btn_t) * macro_new->press_count);
      macro_new_acts_ptr  = 0;
      macro_new_press_ptr = 0;

      break;
    }
    case SGAD_CMD_MACRO_ACT: {
      answ.cmd = SGAD_CMD_OK;

      if (macro_new == NULL || macro_new_acts == NULL) {
        answ.cmd = SGAD_CMD_UNKNOWN;
        break;
      }

      memcpy(macro_new_acts + macro_new_acts_ptr, buf + sizeof(sgad_cmd_t), sizeof(storage_btn_t));
      ++macro_new_acts_ptr;
      break;
    }
    case SGAD_CMD_MACRO_PRESS: {
      answ.cmd = SGAD_CMD_OK;

      if (macro_new == NULL || macro_new_acts == NULL) {
        answ.cmd = SGAD_CMD_UNKNOWN;
        break;
      }

      memcpy(macro_new_press + macro_new_press_ptr, buf + sizeof(sgad_cmd_t), sizeof(storage_btn_t));
      ++macro_new_press_ptr;
      break;
    }
    case SGAD_CMD_MACRO_ADD_STOP: {
      answ.cmd = SGAD_CMD_OK;

      if ((macro_new_acts_ptr != macro_new->act_count) || (macro_new_press_ptr != macro_new->press_count)) {
        answ.cmd = SGAD_CMD_UNKNOWN;
        break;
      }

      storage_add_macro(macro_new, macro_new_acts, macro_new_press);

      free(macro_new);
      free(macro_new_acts);
      free(macro_new_press);

      macro_new       = NULL;
      macro_new_acts  = NULL;
      macro_new_press = NULL;

      break;
    }
    default: {
      answ.cmd = SGAD_CMD_UNKNOWN;
    }
  }

  memcpy(buf, &answ, sizeof(sgad_cmd_t));
  usbd_ep_write_packet(usbd, ep, buf, sizeof(buf));

  //debugf("- Answered cmd: 0x%04X \n", (unsigned int) answ.cmd);
}

static usbd_device *usbd_dev = NULL;

void usb_hid_setconfig(usbd_device *dev) {
  usbd_dev = dev;

  usbd_ep_setup(dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, 6, endpoint_in_callback);
  usbd_ep_setup(dev, 0x82, USB_ENDPOINT_ATTR_INTERRUPT, 6, endpoint_in_callback);
  usbd_ep_setup(dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 6, endpoint_in_callback);
  usbd_ep_setup(dev, 0x84, USB_ENDPOINT_ATTR_INTERRUPT, 6, endpoint_in_callback);

  usbd_ep_setup(dev, 0x85, USB_ENDPOINT_ATTR_INTERRUPT, 64, config_endpoint_in_callback);
  usbd_ep_setup(dev, 0x05, USB_ENDPOINT_ATTR_INTERRUPT, 64, config_endpoint_out_callback);

  usbd_register_control_callback(dev,
                                 USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
                                 USB_REQ_TYPE_TYPE     | USB_REQ_TYPE_RECIPIENT,
                                 hid_control_request);
}

void usb_hid_poll(void) {
  usbd_ep_write_packet(usbd_dev, 0x81, NULL, 0);
  usbd_ep_write_packet(usbd_dev, 0x82, NULL, 0);
  usbd_ep_write_packet(usbd_dev, 0x83, NULL, 0);
  usbd_ep_write_packet(usbd_dev, 0x84, NULL, 0);
}
