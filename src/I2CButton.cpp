//
// Created by Jan Schlemiel on 19.05.25.
//

#include "I2CButton.hpp"

#include <Wire.h>
#include <utility>

I2CButton::I2CButton(uint8_t address){
    _address = address;
}

void I2CButton::setColor(ButtonSide button, Color color) {
    if (button == ButtonSide::RIGHT) {
        _buttonColors.first = color;
    }
    if (button == ButtonSide::LEFT) {
        _buttonColors.second = color;
    }
}

void I2CButton::update() {
    /*
     * I2C ButtonSide reading:
     *
     * - 0b00000000: Empty Buffer (no button pressed!)
     *
     * - if bit 0 == 0 left button is pressed
     * - if bit 6 == 0 right button is pressed
     */
    std::array<bool, 2> lastButtonStates = _buttonStates;

    const int led1 = std::to_underlying(_buttonColors.first) * 2;
    int led2 = std::to_underlying(_buttonColors.second) * 16;

    if (led2 > 63) {
        led2 = led2 + 64;
    }

    const int transmissionValue = 255 - led1 - led2;

    Wire.beginTransmission(_address);
    Wire.write(static_cast<uint8_t>(transmissionValue));
    Wire.endTransmission();

    Wire.requestFrom(_address, 1);

    while (Wire.available()) {
        _data = Wire.read();

        _buttonStates[std::to_underlying(ButtonSide::RIGHT)] = !_data.test(0);
        _buttonStates[std::to_underlying(ButtonSide::LEFT)] = !_data.test(6);
    }

    for (int i = 0; i < _buttonStateOnce.size(); i++) {
        _buttonStateOnce[i] = _buttonStates[i] && !lastButtonStates[i];
    }
}

bool I2CButton::getButtonState(ButtonSide button) const {
    return _buttonStates[std::to_underlying(button)];
}

bool I2CButton::getButtonStateOnce(ButtonSide button) {
    bool returnValue = _buttonStateOnce[std::to_underlying(button)];
    _buttonStateOnce[std::to_underlying(button)] = false;
    return returnValue;
}
