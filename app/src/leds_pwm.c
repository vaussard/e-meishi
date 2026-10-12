/*
 * Module to play some trivial LED PWM animations
 *
 * Initially based on Zephyr's samples/drivers/led/pwm/:
 * - Copyright (c) 2020 Seagate Technology LLC
 *
 * SPDX-FileCopyrightText: © 2026 Florian Vaussard
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(leds_pwm, CONFIG_LOG_DEFAULT_LEVEL);

#include "leds_pwm.h"

#define LED_PWM_NODE_ID		DT_COMPAT_GET_ANY_STATUS_OKAY(pwm_leds)
#define NUM_LEDS		DT_CHILD_NUM_STATUS_OKAY(LED_PWM_NODE_ID)

BUILD_ASSERT(NUM_LEDS > 0, "No PWM LEDs found");

/* Set the brightness of all the LEDs at the same time */
static int leds_all_set_brightness(const struct device *led_pwm, unsigned int level)
{
	int err;

	for (size_t i = 0; i < NUM_LEDS; i++) {
		err = led_set_brightness(led_pwm, i, level);
		if (err < 0) {
			LOG_ERR("err=%d", err);
			return err;
		}
	}

	return 0;
}

/* Ramp (up/down) the brightness of:
 * - All the LEDs if `led` is negative
 * - A specific LED otherwise
 */
static int leds_ramp(const struct device *led_pwm, bool on, int led, unsigned int ramp_duration_ms)
{
	const unsigned int ramp_step_ms = ramp_duration_ms / LED_BRIGHTNESS_MAX;
	int level;
	int err;

	if (ramp_step_ms == 0) {
		return -EINVAL;
	}
	if (led >= NUM_LEDS) {
		return -EINVAL;
	}

	if (on) {
		for (level = 0; level <= LED_BRIGHTNESS_MAX; level++) {
			if (led < 0) {
				err = leds_all_set_brightness(led_pwm, level);
			} else {
				err = led_set_brightness(led_pwm, led, level);
			}
			if (err < 0) {
				return err;
			}
			k_msleep(ramp_step_ms);
		}
	} else {
		for (level = LED_BRIGHTNESS_MAX; level >= 0; level--) {
			if (led < 0) {
				err = leds_all_set_brightness(led_pwm, level);
			} else {
				err = led_set_brightness(led_pwm, led, level);
			}
			if (err < 0) {
				return err;
			}
			k_msleep(ramp_step_ms);
		}
	}

	return 0;
}

int leds_ramp_all(const struct leds_pwm *leds, bool on, unsigned int ramp_duration_ms)
{
	return leds_ramp(leds->led_pwm, on, -1, ramp_duration_ms);
}

int leds_snake(const struct leds_pwm *leds, bool on, unsigned int led_ramp_ms, unsigned int snake_duration_ms)
{
	unsigned int leds_ramp_duration_ms;
	unsigned int snake_delay_ms;
	int err;

	if (led_ramp_ms == 0) {
		return -EINVAL;
	}
	if (NUM_LEDS <= 1) {
		return -EINVAL;
	}

	/* Total duration of the LED ramp-up/down */
	leds_ramp_duration_ms = (led_ramp_ms * NUM_LEDS);

	if (snake_duration_ms < leds_ramp_duration_ms) {
		/* Animation cannot last less than the total LED ramp-up/down */
		return -EINVAL;
	}

	/* Compute the delay between each LED */
	snake_delay_ms = (snake_duration_ms - leds_ramp_duration_ms) / (NUM_LEDS - 1);

	if (snake_delay_ms == 0) {
		return -EINVAL;
	}

	for (unsigned int led = 0; led < NUM_LEDS; led++) {
		err = leds_ramp(leds->led_pwm, on, led, led_ramp_ms);
		if (err) {
			return err;
		}

		if (led < NUM_LEDS - 1) {
			k_msleep(snake_delay_ms);
		}
	}

	return 0;
}

int leds_pwm_init(struct leds_pwm *leds)
{
	leds->led_pwm = DEVICE_DT_GET(LED_PWM_NODE_ID);
	if (!device_is_ready(leds->led_pwm)) {
		LOG_ERR("Device %s is not ready", leds->led_pwm->name);
		leds->led_pwm = NULL;
		return -ENODEV;
	}

	return 0;
}
