/*
 * SPDX-FileCopyrightText: © 2026 Florian Vaussard
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MEISHI_LEDS_PWM_H
#define MEISHI_LEDS_PWM_H

#include <zephyr/device.h>

/**
 * @brief State of the PWM LEDs
 */
struct leds_pwm {
	/** PWM LED controller device, NULL if not available. */
	const struct device *led_pwm;
};

/**
 * @brief Initialize the PWM LEDs
 *
 * Retrieve the "pwm-leds" device from the devicetree and check that it is
 * ready. Having at least one LED is checked at build time.
 *
 * @param leds		LEDs state to initialize
 *
 * @retval 0 on success
 * @retval -ENODEV if the device is not ready
 */
int leds_pwm_init(struct leds_pwm *leds);

/**
 * @brief Ramp the brightness of all LEDs simultaneously
 *
 * The brightness is changed linearly from 0 to 100% (or from 100% to 0) in
 * steps of 1%. This function blocks until the ramp is complete.
 *
 * @param leds			LEDs state, as initialized by leds_pwm_init()
 * @param on			true to ramp up (turn on), false to ramp down (turn off)
 * @param ramp_duration_ms	Duration of the ramp, in milliseconds. Must be at
 *				least 100 ms (1 ms per brightness step)
 *
 * @retval 0 on success
 * @retval -EINVAL if @p ramp_duration_ms is too short
 * @retval <0 other negative errno code from the LED driver
 */
int leds_ramp_all(const struct leds_pwm *leds, bool on, unsigned int ramp_duration_ms);

/**
 * @brief Ramp the LEDs one after the other, in a snake-like animation
 *
 * Each LED is ramped up (or down) in turn, in devicetree order, with a delay
 * between consecutive LEDs computed so that the whole animation lasts
 * approximately @p snake_duration_ms. This function blocks until the
 * animation is complete.
 *
 * @param leds			LEDs state, as initialized by leds_pwm_init()
 * @param on			true to ramp up (turn on), false to ramp down (turn off)
 * @param led_ramp_ms		Duration of the ramp of a single LED, in milliseconds.
 *				Must be at least 100 ms (1 ms per brightness step)
 * @param snake_duration_ms	Total duration of the animation, in milliseconds.
 *				Must be long enough to fit all the individual LED ramps, plus a
 *				non-zero delay between them
 *
 * @retval 0 on success
 * @retval -EINVAL if there are less than 2 LEDs, or if the durations are invalid
 * @retval <0 other negative errno code from the LED driver
 */
int leds_snake(const struct leds_pwm *leds, bool on, unsigned int led_ramp_ms, unsigned int snake_duration_ms);

#endif /* MEISHI_LEDS_PWM_H */
