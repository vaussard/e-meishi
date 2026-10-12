/*
 * SPDX-FileCopyrightText: © 2026 Florian Vaussard
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "leds_pwm.h"
#include "usb_msc.h"

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

static struct leds_pwm leds;

int main(void)
{
	int err;

	/* Initialize USB MSC */
	err = usb_msc_init();
	if (err) {
		return err;
	}

	/* Initialize LEDS. This is not fatal, USB MSC is the main feature. */
	err = leds_pwm_init(&leds);
	if (err) {
		LOG_ERR("Failed to initialize the LEDs (%d)", err);
		while (true) {
			k_sleep(K_FOREVER);
		}
	}

	/* Run the LEDs animations directly from the main thread */
	while (true) {
		leds_snake(&leds, true, 700, 7000);

		leds_ramp_all(&leds, false, 2000);
		k_msleep(500);

		leds_ramp_all(&leds, true, 2000);
		k_msleep(500);

		leds_snake(&leds, false, 700, 7000);

		k_msleep(500);
	}

	return 0;
}
