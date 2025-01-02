#pragma once
#include <SerializedObject.hpp>
#include <Types.hpp>
#include <cstddef>
#include <map>
#include <variant>

class Block : public SerializedObject {
public:
    enum ID : uint8_t {
        AIR = 0,
        GRASS,
        DIRT,
        STONE,
        COBLESTONE,
        PLANKS,
        WOOL,
        BEDROCK
    };

    enum TagID : uint8_t {
        COLOR,
        GHOST
    };

    typedef std::variant<Col3u, bool> TagValue;

protected:
    ID m_id;
    int32_t m_x;
    int32_t m_y;
    uint8_t m_layer;
    std::map<TagID, std::variant<Col3u, bool>> m_tags;
    
public:
    Block(ID id = ID::AIR, int32_t x = 0, int32_t y = 0, uint8_t layer = 1);
    Block(Block& block);

    Rectf getHitbox();

    virtual void update();

    void setPos(int32_t x, int32_t y, uint8_t layer);
    void setID(ID id) { m_id = id; }

    void setTag(TagID key, TagValue value);
    void removeTag(TagID key);
    bool hasTag(TagID key);
    
    template <typename T>
    T getTag(TagID key) { return std::get<T>(m_tags[key]); }

    ByteVector serialize() override;
    size_t deserialize(ByteVector const& bytes) override;

    auto const getID() { return m_id; }
    auto const getPos() { return Vec2i {m_x, m_y}; }
    auto const getLayer() { return m_layer; };
};