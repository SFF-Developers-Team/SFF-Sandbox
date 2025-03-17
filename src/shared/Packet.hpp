#pragma once
#include <pch.h>
#include <DataStream.hpp>
#include <enet.h>
#include <format>

#define CREATE_PACKET std::make_shared<Packet>

class Packet : public DataStream {
public:
    template<typename... Args>
    Packet(Args const&... args) { (add(args), ...); }  
    Packet(ENetPacket* packet) : DataStream(packet->data, packet->data + packet->dataLength) {}
    Packet(ByteVector const& bytes) : DataStream(bytes) {}
};

template <>
struct std::formatter<Packet> {
    char separator = ' ';

    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();

        if (it != ctx.end() && *it != '}') {
            separator = *it++;
        }

        return it;
    }

    auto format(Packet const& p, std::format_context& ctx) const {
        auto it = ctx.out();

        for (auto i = 0; i < p.size(); i++) {
            it = std::format_to(it, "{:02X}", p[i]);

            if (i < p.size() - 1) {
                *it++ = separator;
            }
        }

        return it;
    }
};