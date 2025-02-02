#pragma once
#include <ui/UI.hpp>
#include <string>
#include <vector>
#include <ui/MenuBase.hpp>

class CreditsScene : public MenuBase {
private:
    struct Developer {
        std::string const name;
        std::string const role;
    };

    std::shared_ptr<Container> m_developersBox;
    std::shared_ptr<Container> m_licensesBox;

public:
    CreditsScene();
};