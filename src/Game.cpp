#include <Game.hpp>
#include <Debug.hpp>
#include <iostream>
#include <Player.hpp>
#include <WorldGenNormal.hpp>


void Game::init() {
    InitWindow(m_screenWidth, m_screenHeight, "SFF Sandbox");
    SetTargetFPS(60);
    
    m_blocksMap = new TileMap("assets/blocks.png", Vector2 {16, 16});
    m_world = new World(256, 128);

    if(FileExists("world.dat")){
        int worldSize;
        auto worldData = LoadFileData("world.dat", &worldSize);
        auto worldBytes = ByteVector(worldData, worldData + worldSize);

        m_world->deserialize(worldBytes);
    } else {
        m_world->generate(new WorldGenNormal(m_world));
    }

    while (!WindowShouldClose()) {
        this->update();
        this->render();
    }

    m_world->save();

    CloseWindow();
}

Game::~Game() {
    delete m_world;
    delete m_blocksMap;
}
    
void Game::render() {
    BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode2D(this->m_world->getPlayer()->getCamera());
            m_world->draw(Debug::m_debug);
        EndMode2D();

        if(Debug::m_debug){
            Debug::draw();
        }
    EndDrawing();
}

void Game::update() {
    if(IsKeyPressed(KEY_F3)) {
        Debug::m_debug = !Debug::m_debug;
    }

    if(IsKeyPressed(KEY_F6)) {
        m_world->save();
    }

    std::string fps = "FPS: " + std::to_string(GetFPS());

    Debug::addString(fps);

    m_world->update(m_renderDistance);
}
