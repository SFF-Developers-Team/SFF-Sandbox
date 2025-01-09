#pragma once

#include <Scene.hpp>
#include <Game.hpp>
#include <Debug.hpp>
#include <Timer.hpp>
#include <TileMap.hpp>
#include <WorldGenNormal.hpp>
#include <RenderManager.hpp>
#include <string>
#include <cmath>
#include <vector>
#include <GitHash.hpp>

class MenuScene : public Scene {
public:
    void draw() override;
};