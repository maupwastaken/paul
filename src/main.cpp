#include <iostream>

#include "Logic.hpp"
#include "I2CButton.hpp"

std::unique_ptr<Logic> logic;
std::unique_ptr<I2CButton> button;

bool isRunning = false;

void setup() {
    logic = std::make_unique<Logic>();
    button = std::make_unique<I2CButton>(0x20);

    button->setColor(ButtonSide::LEFT, Color::RED);
    button->setColor(ButtonSide::RIGHT, Color::RED);
}

void loop() {
    button->update();

    if (button->getButtonStateOnce(ButtonSide::LEFT)) {
        isRunning = !isRunning;
    }
    if (button->getButtonStateOnce(ButtonSide::RIGHT)) {
        logic->worldInfo->setHeadingOrigin();
    }

    button->setColor(ButtonSide::RIGHT, logic->worldInfo->getGoalExists() ? Color::WHITE : Color::OFF);
    button->setColor(ButtonSide::LEFT, logic->worldInfo->getHasBall() ? Color::WHITE : Color::OFF);


    if (isRunning) {
        logic->run();
    } else {
        logic->standBy();
    }

    vTaskDelay(pdMS_TO_TICKS(1));
}
