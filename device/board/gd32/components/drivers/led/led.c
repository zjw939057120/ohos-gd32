/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * LED GPIO driver for GD32F470ZI.
 * LED1 on PA8, LED2 on PE3, both active high (high level = LED on).
 * Push-pull output, no pull-up/down, 50MHz drive. LEDs start off.
 */

#include "led.h"

/* LED1: PA8, LED2: PE3 */
#define LED1_PORT           GPIOA
#define LED1_PIN            GPIO_PIN_8
#define LED2_PORT           GPIOE
#define LED2_PIN            GPIO_PIN_3

void LedInit(void)
{
    /* enable GPIOA and GPIOE clocks */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOE);

    /* PA8 / PE3: push-pull output, no pull, 50MHz */
    gpio_mode_set(LED1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED1_PIN);
    gpio_output_options_set(LED1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED1_PIN);
    gpio_mode_set(LED2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED2_PIN);
    gpio_output_options_set(LED2_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED2_PIN);

    /* LEDs off initially (active high) */
    gpio_bit_reset(LED1_PORT, LED1_PIN);
    gpio_bit_reset(LED2_PORT, LED2_PIN);
}

void LedOn(void)
{
    gpio_bit_set(LED1_PORT, LED1_PIN);
    gpio_bit_set(LED2_PORT, LED2_PIN);
}

void LedOff(void)
{
    gpio_bit_reset(LED1_PORT, LED1_PIN);
    gpio_bit_reset(LED2_PORT, LED2_PIN);
}

void LedToggle(void)
{
    gpio_bit_toggle(LED1_PORT, LED1_PIN);
    gpio_bit_toggle(LED2_PORT, LED2_PIN);
}
