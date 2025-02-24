#include <ui/nodes/Slider.hpp>
#include <SettingsManager.hpp>

template <typename T>
class SettingsSlider : public Slider<T> {
protected:
    std::string m_property;

    void callback() {
        SettingsManager::get()->setValue<T>(m_property, this->m_value);
        Slider<T>::callback();
    }

public:
    SettingsSlider(std::string const& property, std::string const& text, T min, T max, T def, MiniFunction<void(Slider<T>*, T)> const& callback)
        : Slider<T>(text, min, max, callback), m_property(property) {
        this->m_value = SettingsManager::get()->getValue<T>(property, def);
    }
};