/* Copyright 2024 RephlexZero (@RephlexZero)
PROUDLY STOLEN BY MKASS420
SPDX-License-Identifier: GPL-V-ROT-EBAL */
#pragma once
#include <stdint.h>

extern uint8_t lut[4096];

uint16_t distance_to_adc(uint8_t distance);

uint8_t adc_to_distance(uint16_t adc);

void generate_lut(void);
