#pragma once
#include <SerializedObject.hpp>

#define MP_BUF_SIZE (65344)
#define CREATE_PACKET std::make_shared<GamePacket>

class GamePacket : public SerializedObject {
private:
    using SerializedObject::deserialize;

public:
    GamePacket(Header header) { 
        add(header); 

        m_offset = 0;    
    }

    template<typename... Args>
    GamePacket(Header const header, Args const&... arg) : GamePacket(header) {
        add(arg...);

        m_offset = 0;
    }

    template<typename... Args>
    GamePacket(Args const&... arg) {
        add(arg...);

        m_offset = 0;
    }

    ByteVector serialize() override {
        return bytes();
    }

    void print() {
        std::string res;
        for(int i = 0; i < size(); i++) { 
            res.append(std::format("{:02X} ", data()[i]));
        }

        logD("GamePacket: {}", res);
    }
};