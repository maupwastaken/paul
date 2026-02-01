//
// Created by Jan Schlegel on 01.02.26.
//

#include "Logic.hpp"

Logic::Logic() {
    _controller = std::make_unique<Controller>();
    _worldInfo = std::make_unique<WorldInfo>(false);
    _button = std::make_unique<I2CButton>(0x20);
}
