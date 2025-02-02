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
    std::shared_ptr<Container> m_textBox;
    std::shared_ptr<Text> m_licenseText;

public:
    CreditsScene();
};