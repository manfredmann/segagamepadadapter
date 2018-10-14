#ifndef USB_CDC_H
#define USB_CDC_H

#include "misc.h"

extern struct usb_iface_assoc_descriptor  uart_assoc;
extern struct usb_interface_descriptor    uart_comm_iface[];
extern struct usb_interface_descriptor    uart_data_iface[];

void usb_cdc_setconfig(usbd_device *dev);
int _write(int file, char *ptr, int len);

#endif
