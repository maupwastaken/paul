//
// Created by Jan Schlegel on 01.02.26.
//

#pragma once

#include "Controller.hpp"
#include "WorldInfo.hpp"
#include "I2CButton.hpp"

class Logic {
public:
    Logic();

    void run();

    void standBy();

private:
    std::unique_ptr<Controller> _controller;

    std::unique_ptr<WorldInfo> _worldInfo;

    std::unique_ptr<I2CButton> _button;
};
