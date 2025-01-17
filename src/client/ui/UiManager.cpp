#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <ui/UiManager.hpp>
#include <raygui.h>
#include <Logger.hpp>
#include <string>
#include <cmath>



UiManager::UiManager() {}

void UiManager::drawMenuLogo() {
    auto rm = RenderManager::get();
    auto tm = TextureManager::get();

    auto sw = static_cast<float>(GetScreenWidth());

    auto sff = tm->getTexture("sff.png");
    auto sffY = 100.f + ((float)sin(GetTime()) * 30);
    rm->drawTexture("sff.png", {sw / 2 - sff.width / 2, sffY, static_cast<float>(sff.width), static_cast<float>(sff.height)});

    auto text = "Sandbox";
    auto textH = 35;
    auto textW = MeasureText(text, textH);
    DrawText(text, sw / 2 - textW / 2, sffY + sff.width * 0.7f, 35, WHITE);
}