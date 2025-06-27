#pragma once

#include "Types.hpp"
#include <Rectangle.hpp>
#include <Texture.hpp>
#include <initializer_list>

class AnimatedTexture {
public:
    /**
     * @brief Construct a new Animated Texture object
     * 
     * @param texture - texture of animation frames
     * @param typeList - list with frame offsets for each animation type
     * @param frames - frames count
     * @param tpf - ticks per frame
     */
    AnimatedTexture(raylib::Texture2D& texture, std::initializer_list<int> const& typeList, int frames, int tpf = 7);

    /**
     * @brief Set current frame
     */
    void SetFrame(int frame) { m_frame = frame; };
    
    /**
     * @brief Set ticks per frame
     */
    void SetTpf(int tpf) { m_tpf = tpf; }

    /**
     * @brief Set frame direction
     */
    void SetDirection(Direction direction) { m_direction = direction; }
    
    /**
     * @brief Set animation type
     * 
     * @param force - instantly set current frame to first frame of type
     */
    void SetType(int type, bool force = false);

    void OnTick();

    /**
     * @brief Draw current frame of animation
     */
    void Draw(RVector2 position);
    void Draw(RRectangle dest);

protected:
    raylib::Texture2D& m_texture;
    Direction m_direction;
    std::vector<int> m_typeList; // temp solution i guess?

    int m_frames;
    int m_frame;
    int m_tpf;
    int m_ticks;
    int m_type;
    int m_newType;
};