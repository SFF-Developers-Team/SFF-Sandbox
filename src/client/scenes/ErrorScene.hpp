#pragma once
#include <scenes/MenuBase.hpp>

class ErrorScene : public MenuBase {
public:
    ErrorScene(std::string const& message);

    void onPush();
};