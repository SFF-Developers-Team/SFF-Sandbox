#pragma once
#include <ui/MenuBase.hpp>

class ErrorScene : public MenuBase {
public:
    ErrorScene(std::string const& message);

    void onPush();
};