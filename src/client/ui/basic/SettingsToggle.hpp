#include <ui/basic/ToggleButton.hpp>

class SettingsToggle : public ToggleButton {
private:
    std::string m_property;

protected:
    void callback() override;

public:
    SettingsToggle(std::string const& property, std::string const& text, MiniFunction<void(ToggleButton*, bool)> const& callback);
};