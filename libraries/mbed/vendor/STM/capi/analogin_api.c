/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
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
#include "analogin_api.h"

#if DEVICE_ANALOGIN

#include "cmsis.h"
#include "pinmap.h"
#include "error.h"

#define ADC_10BIT_RANGE             0x3FF
#define ADC_12BIT_RANGE             0xFFF

static const PinMap PinMap_ADC[] = {
    {PA_0, ADC0_0, 3, 0},
    {PA_1, ADC0_1, 3, 0},
    {PA_2, ADC0_2, 3, 0},
    {PA_3, ADC0_3, 3, 0},
    {PA_4, ADC0_4, 3, 0},
    {PA_5, ADC0_5, 3, 0},
    {PA_6, ADC0_6, 3, 0},
    {PA_7, ADC0_7, 3, 0},
    {PB_0, ADC0_8, 3, 0},
    {PB_1, ADC0_9, 3, 0},
    {PC_0, ADC0_10, 3, 0},
    {PC_1, ADC0_11, 3, 0},
    {PC_2, ADC0_12, 3, 0},
    {PC_3, ADC0_13, 3, 0},
    {PC_4, ADC0_14, 3, 0},
    {PC_5, ADC0_15, 3, 0},
    {NC,    NC,     0, 0}
};

#   define ADC_RANGE    ADC_12BIT_RANGE

void analogin_init(analogin_t *obj, PinName pin) {
    obj->adc = (ADCName)pinmap_peripheral(pin, PinMap_ADC);
    if (obj->adc == (uint32_t)NC) {
        error("ADC pin mapping failed");
    }

    // ensure power is turned on
    RCC->APB1ENR |= RCC_APB2ENR_ADC1EN;

    // Enable the ADC
    ADC1->CR2 |= ADC_CR2_ADON;

    pinmap_pinout(pin, PinMap_ADC);
}

static inline uint32_t adc_read(analogin_t *obj) {
    // Select the appropriate channel
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= (int) obj->adc;

    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // Wait for conversion to finish
    while (!(ADC1->SR & ADC_SR_EOC));

    uint32_t data = ADC1->DR;
    return data; // 12 bit
}

static inline uint32_t adc_read_u32(analogin_t *obj) {
    uint32_t value;
    value = adc_read(obj);
    return value;
}

uint16_t analogin_read_u16(analogin_t *obj) {
    uint32_t value = adc_read_u32(obj);

    return (value << 4) | ((value >> 8) & 0x000F); // 12 bit
}

float analogin_read(analogin_t *obj) {
    uint32_t value = adc_read_u32(obj);
    return (float)value * (1.0f / (float)ADC_RANGE);
}

#endif
