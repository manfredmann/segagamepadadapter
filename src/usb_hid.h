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

#ifndef USB_HID_H
#define USB_HID_H

#include "misc.h"
#include "storage.h"

extern struct usb_interface_descriptor hid_iface_j0;
extern struct usb_interface_descriptor hid_iface_j1;
extern struct usb_interface_descriptor hid_iface_j2;
extern struct usb_interface_descriptor hid_iface_j3;
extern struct usb_interface_descriptor hid_iface_config;

void usb_hid_setconfig(usbd_device *dev);
void usb_hid_poll(void);
void update_gamepads_macro(void);

#endif
