/*
 * This file is part of the SEGA Gamepad adapter project.
 *
 * Copyright (C) 2018 Roman Serov <roman@serov.co>
 *
 * SEGA Gamepad adapter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SEGA Gamepad adapter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SEGA Gamepad adapter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "usb_cdc.h"
#include <string.h>

static const struct usb_endpoint_descriptor uart_comm_endp[] = {{
  .bLength            = USB_DT_ENDPOINT_SIZE,
  .bDescriptorType    = USB_DT_ENDPOINT,
  .bEndpointAddress   = 0x87,
  .bmAttributes       = USB_ENDPOINT_ATTR_INTERRUPT,
  .wMaxPacketSize     = 16,
  .bInterval          = 255,
}};

static const struct usb_endpoint_descriptor uart_data_endp[] = {{
  .bLength          = USB_DT_ENDPOINT_SIZE,
  .bDescriptorType  = USB_DT_ENDPOINT,
  .bEndpointAddress = 0x06,
  .bmAttributes     = USB_ENDPOINT_ATTR_BULK,
  .wMaxPacketSize   = 64,
  .bInterval        = 0x01,
}, {
  .bLength          = USB_DT_ENDPOINT_SIZE,
  .bDescriptorType  = USB_DT_ENDPOINT,
  .bEndpointAddress = 0x86,
  .bmAttributes     = USB_ENDPOINT_ATTR_BULK,
  .wMaxPacketSize   = 64,
  .bInterval        = 0x01,
}};

static const struct {
  struct usb_cdc_header_descriptor header;
  struct usb_cdc_call_management_descriptor call_mgmt;
  struct usb_cdc_acm_descriptor acm;
  struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed)) uart_cdcacm_functional_descriptors = {
  .header = {
    .bFunctionLength    = sizeof(struct usb_cdc_header_descriptor),
    .bDescriptorType    = CS_INTERFACE,
    .bDescriptorSubtype = USB_CDC_TYPE_HEADER,
    .bcdCDC             = 0x0110,
  },
  .call_mgmt = {
    .bFunctionLength    = sizeof(struct usb_cdc_call_management_descriptor),
    .bDescriptorType    = CS_INTERFACE,
    .bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
    .bmCapabilities     = 0,
    .bDataInterface     = 6,
  },
  .acm = {
    .bFunctionLength    = sizeof(struct usb_cdc_acm_descriptor),
    .bDescriptorType    = CS_INTERFACE,
    .bDescriptorSubtype = USB_CDC_TYPE_ACM,
    .bmCapabilities     = 2, /* SET_LINE_CODING supported*/
  },
  .cdc_union = {
    .bFunctionLength        = sizeof(struct usb_cdc_union_descriptor),
    .bDescriptorType        = CS_INTERFACE,
    .bDescriptorSubtype     = USB_CDC_TYPE_UNION,
    .bControlInterface      = 5,
    .bSubordinateInterface0 = 6,
   }
};

struct usb_interface_descriptor uart_comm_iface[] = {{
  .bLength = USB_DT_INTERFACE_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE,
  .bInterfaceNumber   = 5,
  .bAlternateSetting  = 0,
  .bNumEndpoints      = 1,
  .bInterfaceClass    = USB_CLASS_CDC,
  .bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
  .bInterfaceProtocol = USB_CDC_PROTOCOL_AT,
  .iInterface         = 0,

  .endpoint = uart_comm_endp,

  .extra = &uart_cdcacm_functional_descriptors,
  .extralen = sizeof(uart_cdcacm_functional_descriptors)
}};

struct usb_interface_descriptor uart_data_iface[] = {{
  .bLength            = USB_DT_INTERFACE_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE,
  .bInterfaceNumber   = 6,
  .bAlternateSetting  = 0,
  .bNumEndpoints      = 2,
  .bInterfaceClass    = USB_CLASS_DATA,
  .bInterfaceSubClass = 0,
  .bInterfaceProtocol = 0,
  .iInterface         = 9,

  .endpoint = uart_data_endp,
}};

struct usb_iface_assoc_descriptor uart_assoc = {
  .bLength            = USB_DT_INTERFACE_ASSOCIATION_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE_ASSOCIATION,
  .bFirstInterface    = 5,
  .bInterfaceCount    = 2,
  .bFunctionClass     = USB_CLASS_CDC,
  .bFunctionSubClass  = USB_CDC_SUBCLASS_ACM,
  .bFunctionProtocol  = USB_CDC_PROTOCOL_AT,
  .iFunction          = 0,
};


static void cdc_set_modem_state(usbd_device *dev, uint8_t iface, bool dsr, bool dcd) {
  char buf[10];
  struct usb_cdc_notification *notif = (void*)buf;

  notif->bmRequestType  = 0xA1;
  notif->bNotification  = USB_CDC_NOTIFY_SERIAL_STATE;
  notif->wValue         = 0;
  notif->wIndex         = iface;
  notif->wLength        = 2;

  buf[8] = (dsr ? 2 : 0) | (dcd ? 1 : 0);
  buf[9] = 0;

  usbd_ep_write_packet(dev, 0x85 + iface, buf, 10);
}

static enum usbd_request_return_codes cdcacm_control_request(usbd_device *dev,
    struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
    void (**complete)(usbd_device *dev, struct usb_setup_data *req)) {
  (void) complete;
  (void) buf;
  (void) len;

  switch(req->bRequest) {
  case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
    cdc_set_modem_state(dev, req->wIndex, true, true);
    return USBD_REQ_HANDLED;
  case USB_CDC_REQ_SET_LINE_CODING:
    if(*len < sizeof(struct usb_cdc_line_coding))
      return USBD_REQ_NOTSUPP;

    return USBD_REQ_HANDLED;
  }
  return USBD_REQ_NOTSUPP;
}

// Некрасиво и опасно, но на данный момент не так важно

static usbd_device *usbd_dev = NULL;

int _write(int file, char *ptr, int len) {
  if (file == STDOUT_FILENO || file == STDERR_FILENO) {

    for (uint16_t i = 0; i < len; i++) {
      if (ptr[i] == '\n') {
        usart_send_blocking(USART1, '\r');
      }
      usart_send_blocking(USART1, ptr[i]);
    }

    return len;
  }
  errno = EIO;
  return -1;
}

static void usbuart_usb_out_cb(usbd_device *dev, uint8_t ep) {
  (void) dev;
  (void) ep;
}

static void usbuart_usb_in_cb(usbd_device *dev, uint8_t ep) {
  (void) dev;
  (void) ep;
}

void usb_cdc_setconfig(usbd_device *dev) {
  usbd_dev = dev;

  usbd_ep_setup(dev, 0x06, USB_ENDPOINT_ATTR_BULK, 64, usbuart_usb_out_cb);
  usbd_ep_setup(dev, 0x86, USB_ENDPOINT_ATTR_BULK, 64, usbuart_usb_in_cb);
  usbd_ep_setup(dev, 0x87, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

  usbd_register_control_callback(dev,
      USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
      USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
      cdcacm_control_request);

  cdc_set_modem_state(dev, 2, true, true);
}
