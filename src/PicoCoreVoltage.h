#pragma once

#include "hardware/vreg.h"
#include "pico/stdlib.h"

#ifndef VREG_VSEL
#define VREG_VSEL VREG_VOLTAGE_1_20
#endif

inline void pico_set_core_voltage() {
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(10);
}

inline float pico_get_core_voltage() {
    switch(VREG_VSEL) {
        case VREG_VOLTAGE_0_85: return 0.85f;
        case VREG_VOLTAGE_0_90: return 0.90f;
        case VREG_VOLTAGE_0_95: return 0.95f;
        case VREG_VOLTAGE_1_00: return 1.00f;
        case VREG_VOLTAGE_1_05: return 1.05f;
        case VREG_VOLTAGE_1_10: return 1.10f;
        case VREG_VOLTAGE_1_15: return 1.15f;
        case VREG_VOLTAGE_1_20: return 1.20f;
        case VREG_VOLTAGE_1_25: return 1.25f;
        case VREG_VOLTAGE_1_30: return 1.30f;
        default: return 9.99f;
    }
}
