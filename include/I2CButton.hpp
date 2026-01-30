#pragma once

#include <bitset>
#include <array>

enum class ButtonSide : int {
    LEFT = 0,
    RIGHT,
};

enum class Color : int {
    OFF = 0,
    GREEN,
    RED,
    YELLOW,
    BLUE,
    CYAN,
    MAGENTA,
    WHITE
};

class I2CButton {
public:
    explicit I2CButton(uint8_t address);

    void setColor(ButtonSide button, Color color);

    void update();

    bool getButtonState(ButtonSide button) const;

    bool getButtonStateOnce(ButtonSide button);

private:
    uint8_t _address;

    std::bitset<8> _data;

    std::array<bool, 2> _buttonStates{};
    std::array<bool, 2> _buttonStateOnce{};

    std::pair<Color, Color> _buttonColors;
};
