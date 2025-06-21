#pragma once
#include <ui/basic/ListContainer.hpp>
#include <scenes/MenuBase.hpp>
#include <ui/basic/Button.hpp>
#include <string>
#include <vector>

class SettingsScene : public MenuBase {
private:
    std::vector<std::shared_ptr<ListContainer>> m_categories;
    std::string m_dropText;
    Button* m_keySelect;
    bool m_autoResolution;
    
public:
    SettingsScene();

    void update() override;
};