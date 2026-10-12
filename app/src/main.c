/*
 * SPDX-FileCopyrightText: © 2026 Florian Vaussard
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "usb_msc.h"

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

int main(void)
{
	int err;

	/* Initialize USB MSC */
	err = usb_msc_init();
	if (err) {
		return err;
	}

	while (true) {
		k_sleep(K_FOREVER);
	}

	return 0;
}
