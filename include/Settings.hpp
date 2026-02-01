#pragma once

#include <cstdint>
#include <chrono>
#include <array>

using pin = uint8_t;

inline constexpr uint8_t CMPS14_ADDRESS = 0x60;
inline constexpr uint8_t IR_ADDRESS = 0x55;
inline constexpr uint8_t PIXY_ADDRESS = 0x54;

inline constexpr int PWM_FREQUENCY = 1000;
inline constexpr int PWM_RESOLUTION = 8;

inline constexpr pin ENA = 27;

inline constexpr std::array<pin, 3> PWM_PINS = {18, 14, 33};
inline constexpr std::array<pin, 3> DIR_PINS = {19, 12, 32};

inline constexpr pin LIGHT_BARRIER_PIN = 36;
inline constexpr int LIGHT_BARRIER_THRESHOLD = 3000;

inline constexpr auto BALL_ALIGNED_THRESHOLD = std::chrono::seconds(3);

enum class MOTOR_CONFIGURATION : int {
    RIGHT = 1,
    BACK = 0,
    LEFT = 2,
};
