#ifndef USB_HID_H
#define USB_HID_H

#include "misc.h"

extern struct usb_interface_descriptor hid_iface_j0;
extern struct usb_interface_descriptor hid_iface_j1;
extern struct usb_interface_descriptor hid_iface_j2;
extern struct usb_interface_descriptor hid_iface_j3;
extern struct usb_interface_descriptor hid_iface_config;

void usb_hid_setconfig(usbd_device *dev);
void usb_hid_poll(void);

#endif
