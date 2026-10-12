/*
 * SPDX-FileCopyrightText: © 2026 Florian Vaussard
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef MEISHI_USB_MSC_H
#define MEISHI_USB_MSC_H

/**
 * @brief Initialize the Flash disk and the USB MSC device
 *
 * @return 0 on success
 * @return A negative errno on failure
 */
int usb_msc_init(void);

#endif /* MEISHI_USB_MSC_H */
