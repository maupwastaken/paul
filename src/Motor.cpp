//
// Created by Jan Schlegel on 23.07.25.
//

#include "Motor.hpp"
#include "Config.hpp"

#include <Arduino.h>
#include <iostream>

Motor::Motor(pin pwm, pin dir) : _pwm(pwm), _dir(dir) {
    pinMode(_dir, OUTPUT);

    ledcAttach(_pwm, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcWrite(_pwm, 0);
}

void Motor::speed(int speed) const {
    speed = std::clamp(speed, -100, 100);

    digitalWrite(_dir, speed > 0 ? LOW : HIGH);

    ledcWrite(_pwm, map(std::abs(speed), 0, 100, 0, _max));
}

