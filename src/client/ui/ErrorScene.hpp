#pragma once
#include <ui/MenuBase.hpp>

class ErrorScene : public MenuBase {
private:
    std::string m_message;

public:
    ErrorScene(std::string const& message);
};