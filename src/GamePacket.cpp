#include <GamePacket.hpp>
#include <SerializedObject.hpp>
#include <GenericTools.hpp>

GamePacket::GamePacket(std::vector<SerializedObject *> objects) {
    _objects = objects;
    _shouldEncode = true;
}
GamePacket::GamePacket(std::vector<unsigned char> data) {
    _data = data;
    _shouldEncode = false;
}
GamePacket::GamePacket(std::vector<std::vector<unsigned char>> data) {
    _shouldEncode = false;

    for (std::vector<unsigned char> &vec : data) {
        GenericTools::addVectors<unsigned char>(
            &_data,
            vec
        );
    }
}

std::vector<SerializedObject *> &GamePacket::getEncoded() {
    return _objects;
}
std::vector<unsigned char> &GamePacket::getDecoded() {
    return _data;
}

#include <Block.hpp>
#include <player.hpp>

void GamePacket::performDecode() {
    auto offsets = findOffsets();
    if (!analyzeOffsets(offsets)) {
        printf("GamePacket::performDecode(): error 1\n");

        return;
    }

    clearObjects();

    for (int i = 0; i < offsets.size(); i++) {
        GamePacket::PacketInto obj = offsets[i];
        SerializedObject::SObject object_data = {};

        for (unsigned int j = 0; j < obj.size; j++) {
            object_data.push_back(_data[obj.offset + j]);
        }

        switch (obj.type) {
            case 0: {
                Block *_obj = new Block(Block::BlockType::AIR);

                _obj->decodeObject(object_data);
                _objects.push_back(_obj);

                break;
            }
            case 1: {
                Player *_obj = new Player();
                
                _obj->decodeObject(object_data);
                _objects.push_back(_obj);

                break;
            }
        }
    }
}
void GamePacket::performEncode() {
    _data.clear();

    for (auto object : _objects) {
        GenericTools::addVectors<unsigned char>(
            &_data,
            object->encodeObject()
        );
    }
}

void GamePacket::processPacket() {
    if (_shouldEncode) return performEncode();
    performDecode();
}

bool GamePacket::canBeDecoded() {
    auto offsets = findOffsets();

    if (!analyzeOffsets(offsets)) {
        printf("GamePacket::canBeDecoded(): error 2\n");

        return false;
    }

    return true;
}
std::vector<struct GamePacket::PacketInto> GamePacket::findOffsets() {
    std::vector<struct GamePacket::PacketInto> result = {};

    if (_data.size() == 0) {
        printf("GamePacket::findOffsets: error 6\n");

        return result;
    }

    for (unsigned int i = 0; i < _data.size(); i++) {
        struct PacketInto pel = {};

        pel.type = _data[i];
        pel.offset = i;

        unsigned int sz = SerializedObject::getSizeForObject(pel.type);
        
        pel.size = sz;

        result.push_back(pel);

        if (sz < 0) {
            printf("GamePacket::findOffsets: error 5\n");

            return result;
        }

        i += sz - 1;
    }

    return result;
}
bool GamePacket::analyzeOffsets(std::vector<struct PacketInto> &info) {
    if (info.size() == 0) {
        printf("GamePacket::analyzeOffsets(): error 3\n");

        return false;
    }

    for (auto &el : info) {
        if (el.size < 0) {
            printf("GamePacket::analizeOffsets(): error 4\n");

            return false;
        }
    }

    return true;
}

void GamePacket::allowObjectCleanup(bool flag) {
    _allowObjectCleanup = flag;
}

GamePacket::~GamePacket() {
    clearObjects();
}

void GamePacket::clearObjects() {
    if (_allowObjectCleanup) {
        GenericTools::deleteVec<SerializedObject>(_objects);
    } else {
        _objects.clear();
    }
}

int GamePacket::getPacketSize() {
    return _data.size();
}
int GamePacket::getPacketObjects() {
    return findOffsets().size();
}