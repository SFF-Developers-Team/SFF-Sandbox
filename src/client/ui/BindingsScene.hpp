#pragma once
#include <ui/MenuBase.hpp>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <toml.hpp>

class BindingsScene : public MenuBase {
private:

public:
    void draw() override;
};