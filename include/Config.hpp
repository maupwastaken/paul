//
// Created by Jan Schlegel on 23.07.25.
//

#pragma once

#include <cstdint>
#include <chrono>
#include <array>

using pin = uint8_t;

inline constexpr uint8_t IR_ADDRESS = 0x55;
inline constexpr uint8_t PIXY_ADDRESS = 0x54;

inline constexpr int IMG_CENTER_X = 158;
inline constexpr int IMG_CENTER_Y = 104;

inline constexpr int PWM_FREQUENCY = 1000;
inline constexpr int PWM_RESOLUTION = 8;

inline constexpr pin KICK = 17;

inline constexpr pin ENA = 27;

inline constexpr std::array<pin, 5> PWM_PINS = {18, 14, 33, 26};
inline constexpr std::array<pin, 5> DIR_PINS = {19, 12, 32, 25};

enum class MOTOR_CONFIGURATION : int {
    RIGHT = 1,
    BACK = 0 ,
    LEFT = 2,
};

inline constexpr std::chrono::milliseconds KICK_STRENGTH = std::chrono::milliseconds(10);
inline constexpr std::chrono::milliseconds MAX_KICK_STRENGTH = std::chrono::milliseconds(23);
