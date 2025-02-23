#pragma once
#include <ui/nodes/ListContainer.hpp>
#include <ui/MenuBase.hpp>
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