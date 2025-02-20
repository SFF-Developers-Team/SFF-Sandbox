#pragma once
#include <SerializedObject.hpp>
#include <Logger.hpp>
#include <enet.h>

#define CREATE_PACKET std::make_shared<Packet>

class Packet : public SerializedObject {
private:
    using SerializedObject::deserialize;

public:
    Packet() {}

    Packet(ENetPacket* packet) {
        resize(packet->dataLength);
        std::copy(packet->data, packet->data + size(), data());
    }

    Packet(Header header) { 
        add(header);
    }

    Packet(ByteVector const& bytes) {
        add(bytes);
        reset();
    }

    template<typename... Args>
    Packet(Header const header, Args const&... arg) : Packet(header) {
        add(arg...);
    }

    template<typename... Args>
    Packet(Args const&... arg) {
        add(arg...);
    }

    ByteVector serialize() override {
        return bytes();
    }

    void print() {
        std::string res;
        for(auto i = 0; i < size(); i++) { 
            res.append(std::format("{:02X} ", data()[i]));
        }

        logD("GamePacket: {}", res);
    }
};