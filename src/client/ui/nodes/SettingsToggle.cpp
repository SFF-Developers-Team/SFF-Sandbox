#include <ui/nodes/SettingsToggle.hpp>
#include <SettingsManager.hpp>
#include <Logger.hpp>

SettingsToggle::SettingsToggle(std::string const& property, std::string const& text, MiniFunction<void(ToggleButton*, bool)> const& callback) 
    : ToggleButton(text, callback), m_property(property) { m_flag = SettingsManager::get()->getValue<bool>(m_property, false); }

void SettingsToggle::callback() {
    ToggleButton::callback();
    SettingsManager::get()->setValue<bool>(m_property, m_flag);
}