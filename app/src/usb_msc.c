/*
 * Based on Zephyr's samples/subsys/usb/mass:
 * - Copyright (c) 2016 Intel Corporation.
 * - Copyright (c) 2019-2020 Nordic Semiconductor ASA
 *
 * SPDX-FileCopyrightText: © 2026 Florian Vaussard
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/class/usbd_msc.h>

#include "usb_msc.h"

LOG_MODULE_REGISTER(usb_msc, CONFIG_LOG_DEFAULT_LEVEL);

/* VID / PID from Zephyr sample...
 * TODO: request a PID from https://pid.codes/
 */
#define USB_VID			0x2fe3
#define USB_PID			0x0008

#define VENDOR			"Vaussard"
#define PRODUCT			"e-meishi"

/* Worst case power scenario @85°C:
 * - STM32U3: 6.7mA (48MHz)
 * - IS25LP128: 20mA (during program / erase)
 * - APT1608: 7x 5mA
 *
 * Total: 61.7mA
 * Use 100mA to have some margin and count as a low-power device
 */
#define USBD_MAX_POWER		50 /* x 2mA = 100mA */

USBD_DEVICE_DEFINE(meishi_usbd,
		   DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
		   USB_VID, USB_PID);

USBD_DESC_LANG_DEFINE(meishi_lang);
USBD_DESC_MANUFACTURER_DEFINE(meishi_mfr, VENDOR);
USBD_DESC_PRODUCT_DEFINE(meishi_product, PRODUCT);
IF_ENABLED(CONFIG_HWINFO, (USBD_DESC_SERIAL_NUMBER_DEFINE(meishi_sn)));

USBD_DESC_CONFIG_DEFINE(fs_cfg_desc, "FS Configuration");

/* Full-speed only (STM32U3), bus-powered, no remote wakeup */
USBD_CONFIGURATION_DEFINE(meishi_fs_config,
			  0,
			  USBD_MAX_POWER, &fs_cfg_desc);

#define DISK_NAME	"NOR"

USBD_DEFINE_MSC_LUN(nor, DISK_NAME, VENDOR, PRODUCT, "0.00");

static int usb_init(void)
{
	int err;

	err = usbd_add_descriptor(&meishi_usbd, &meishi_lang);
	if (err) {
		LOG_ERR("Failed to initialize language descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&meishi_usbd, &meishi_mfr);
	if (err) {
		LOG_ERR("Failed to initialize manufacturer descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&meishi_usbd, &meishi_product);
	if (err) {
		LOG_ERR("Failed to initialize product descriptor (%d)", err);
		return err;
	}

#if defined(CONFIG_HWINFO)
	err = usbd_add_descriptor(&meishi_usbd, &meishi_sn);
	if (err) {
		LOG_ERR("Failed to initialize SN descriptor (%d)", err);
		return err;
	}
#endif

	err = usbd_add_configuration(&meishi_usbd, USBD_SPEED_FS,
				     &meishi_fs_config);
	if (err) {
		LOG_ERR("Failed to add Full-Speed configuration (%d)", err);
		return err;
	}

	err = usbd_register_all_classes(&meishi_usbd, USBD_SPEED_FS, 1, NULL);
	if (err) {
		LOG_ERR("Failed to register classes (%d)", err);
		return err;
	}

	/* Single-function device: class information is in the interface descriptor */
	usbd_device_set_code_triple(&meishi_usbd, USBD_SPEED_FS, 0, 0, 0);

	err = usbd_init(&meishi_usbd);
	if (err) {
		LOG_ERR("Failed to initialize device support (%d)", err);
		return err;
	}

	err = usbd_enable(&meishi_usbd);
	if (err) {
		LOG_ERR("Failed to enable device support (%d)", err);
		return err;
	}

	return 0;
}

int usb_msc_init(void)
{
	int err;

	/* Initialize the disk */
	err = disk_access_ioctl(DISK_NAME, DISK_IOCTL_CTRL_INIT, NULL);
	if (err != 0) {
		LOG_ERR("Failed to initialize %s disk: %d", DISK_NAME, err);
		return err;
	}

	/* Initialize USB MSC */
	err = usb_init();
	if (err) {
		return err;
	}

	LOG_INF("The device is put in USB mass storage mode");

	return 0;
}
