/*
 * led.c
 *
 *  Created on: 14 рту. 2018 у.
 *      Author: RLeonov
 */

#include "config.h"
#include "rexos/userspace/stm32/stm32_driver.h"
#include "rexos/userspace/gpio.h"

void led_init()
{
    gpio_enable_pin(LED_ORANGE_PIN, GPIO_MODE_OUT);
}

void led_on()
{
    gpio_set_pin(LED_ORANGE_PIN);
}

void led_off()
{
    gpio_reset_pin(LED_ORANGE_PIN);
}
