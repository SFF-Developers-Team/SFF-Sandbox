#include <game.hpp>
#include <Sprite.hpp>
#include <Debug.hpp>
#include <GenericTools.hpp>
#include <iostream>
#include <GamePacket.hpp>
#include <player.hpp>
void Game::init() {
    InitWindow(m_screenWidth, m_screenHeight, "FriendsTeam Sandbox");
    SetTargetFPS(60);

    Sprite::loadSprites();

    m_world = new World(256, 64);
    m_player = new Player(m_world);
    

    m_player->camera.offset = {m_screenWidth / 2.0f, m_screenHeight / 2.0f};

    while (!WindowShouldClose()) {
        this->update();
        this->render();
    }

    CloseWindow();
}
    
void Game::render() {
    BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode2D(this->m_player->camera);
            m_world->draw(m_debug);
            m_player->draw();
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
    if (IsKeyPressed(KEY_F6)) {
        std::vector<SerializedObject::SObject> data = {};
        std::vector<SerializedObject *> objs = {};

        data.push_back((new Player(m_world))->encodeObject());
        data.push_back((new Block(Block::BlockType::AIR))->encodeObject());
        data.push_back((new Player(m_world))->encodeObject());
        data.push_back((new Block(Block::BlockType::DIRT))->encodeObject());
        data.push_back((new Player(m_world))->encodeObject());
        data.push_back((new Block(Block::BlockType::GRASS))->encodeObject());
        data.push_back((new Player(m_world))->encodeObject());
        data.push_back((new Block(Block::BlockType::STONE))->encodeObject());


        GamePacket *packet = new GamePacket(data);
        packet->allowObjectCleanup(false);
        packet->processPacket();

        std::cout << "created packet which consits of " << std::to_string(packet->getPacketSize()) << " bytes of data and contains " << std::to_string(packet->getPacketObjects()) << " objects.\n";
        std::cout << GenericTools::vectorToString<unsigned char>(packet->getDecoded()) << std::endl;

        delete packet;
    }
    if (IsKeyPressed(KEY_F7)) {
        std::vector<Block *> blocks_test;

        for (int i = 0; i < 16; i++) {
            Block *b = new Block(Block::BlockType::DIRT);
            b->setPosition(i, i);

            blocks_test.push_back(b);
        }

        unsigned int bytes = 0;

        std::cout << "m_world->m_blocks.size() = " + std::to_string(m_world->m_blocks.size()) + "\n";

        std::vector<Block::SObject> obj1 = {};
        std::vector<Block::SObject> obj2 = {};

        for (Block *block : blocks_test) {
            auto data = block->encodeObject();

            obj1.push_back(data);
        }

        GenericTools::deleteVec<Block>(blocks_test);

        GamePacket *packet = new GamePacket(obj1);
        packet->allowObjectCleanup(false);
        packet->processPacket();

        std::cout << "created packet which consits of " << std::to_string(packet->getPacketSize()) << " bytes of data and contains " << std::to_string(packet->getPacketObjects()) << " objects.\n";

        blocks_test = SerializedObject::convertVector<Block>(packet->getEncoded());

        for (Block *block : blocks_test) {
            auto data = block->encodeObject();

            obj2.push_back(data);

            bytes += data.size();
            std::cout << ": " + GenericTools::vectorToString<unsigned char>(data) + "\n";
        }

        int errors = 0;
        for (int i = 0; i < obj1.size() && i < obj2.size(); i++) {
            if (obj1[i] != obj2[i]) errors++;
        }

        if (obj1.size() != obj2.size()) {
            errors += std::max<int>(obj1.size() - obj2.size(), obj2.size() - obj1.size());
        }

        std::cout << std::to_string(blocks_test.size()) << " blocks were serialized into " << std::to_string(bytes) << " bytes of raw data\n" + std::to_string(errors) << " ERRORS\n";

        GenericTools::deleteVec<Block>(blocks_test);
        delete packet;
    }

    std::string fps = "FPS: " + std::to_string(GetFPS());

    Debug::addString(fps);

    m_world->update(m_player->getPosition(), m_renderDistance);
    m_player->update(m_world->m_hitboxes);
}
