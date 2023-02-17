/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "ym3012_dac.pio.h"
#include "hardware/irq.h"  // interrupts

#define PIN_BASE 0
#define AUDIO_PIN 28

#define PWM_WRAP 50

// uint16_t samples[100000] = { 0 };

uint16_t last_sample;

void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));
    pwm_set_gpio_level(AUDIO_PIN, last_sample);
}

int main() {
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    sleep_ms(5000);
    printf("Hello world\n");

    // Init PWM for audio out
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    // Setup PWM interrupt to fire when PWM cycle is complete
//     pwm_clear_irq(audio_pin_slice);
//     pwm_set_irq_enabled(audio_pin_slice, true);
    // set the handle function above
//     irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler);
//     irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 8.0f);
    pwm_config_set_wrap(&config, PWM_WRAP);
    pwm_set_gpio_level(AUDIO_PIN, 0);
    pwm_init(audio_pin_slice, &config, true);

    // Init state machine for PIO
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &ym3012_dac_program);
    ym3012_dac_init(pio, sm, offset, PIN_BASE);

//     for (int i = 0; i < 100000; i++) {
//         samples[i] = ym3012_dac_get_sample(pio, sm) * (16384/1024); // 16384 is 2^16 (because we're using a uint16_t), 1024 is 2^10 (because it's a 10-bit DAC)
//     }
//     for (int i = 0; i < 100000; i++) {
//         printf("%d\n", samples[i]);
//     }
    while (true) {
        last_sample = ym3012_dac_get_sample(pio, sm) * 4; //(16384/1024); // 16384 is 2^16 (because we're using a uint16_t), 1024 is 2^10 (because it's a 10-bit DAC)
//         printf("%d\n", last_sample);
        pwm_set_gpio_level(AUDIO_PIN, last_sample);
    }
}
