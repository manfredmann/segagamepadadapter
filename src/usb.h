#ifndef USB_H
#define USB_H

#include "misc.h"
#include "gamepad.h"

void usb_init(uint8_t gamepads_count);
void usb_send(void);

#endif
