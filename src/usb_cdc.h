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

#ifndef USB_CDC_H
#define USB_CDC_H

#include "misc.h"

extern struct usb_iface_assoc_descriptor  uart_assoc;
extern struct usb_interface_descriptor    uart_comm_iface[];
extern struct usb_interface_descriptor    uart_data_iface[];

void usb_cdc_setconfig(usbd_device *dev);
int _write(int file, char *ptr, int len);

#endif
