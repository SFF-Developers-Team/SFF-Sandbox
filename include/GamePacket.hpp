#pragma once

#include <vector>

class SerializedObject;
class World;
class GamePacket {
private:
    struct PacketInto {
        unsigned char type;
        unsigned int offset;
        unsigned int size;

        bool operator ==(const PacketInto &r) {
            return (type == r.type) && (offset == r.offset) && (size == r.size);
        }
    };

    bool _shouldEncode;
    
    std::vector<SerializedObject *> _objects;
    std::vector<unsigned char> _data;

    bool _allowObjectCleanup = false;

    std::vector<struct PacketInto> findOffsets();
    bool analyzeOffsets(std::vector<struct PacketInto> &info);

    void performDecode();
    void performEncode();

    void clearObjects();
public:    
    World* m_world;
    GamePacket(std::vector<SerializedObject *> objects);
    GamePacket(std::vector<unsigned char> data);
    GamePacket(std::vector<std::vector<unsigned char>> data);

    ~GamePacket();

    void processPacket();
    bool canBeDecoded();

    std::vector<SerializedObject *> &getEncoded();
    std::vector<unsigned char> &getDecoded();

    void allowObjectCleanup(bool flag);

    int getPacketSize();
    int getPacketObjects();
};