#pragma once

#include "Types.hpp"
#include <Rectangle.hpp>
#include <Texture.hpp>

class AnimatedTexture {
public:
    /**
     * @brief Construct a new Animated Texture object
     * 
     * @param textureName - name of loaded texture with animation frames
     * @param types - list with frame offsets for each animation type
     * @param frames - frames count
     * @param tpf - ticks per frame
     */
    AnimatedTexture(std::string const& textureName, int* types, int frames, int tpf = 7);

    /**
     * @brief Construct a new Animated Texture object
     * 
     * @param texture - texture with animation frames
     * @param types - list with frame offsets for each animation type
     * @param frames - frames count
     * @param tpf - ticks per frame
     */
    AnimatedTexture(raylib::Texture2D& texture, int* types, int frames, int tpf = 7);

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
     * @param force - instantly set current frame to first frame of animation type
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

    int* m_types;
    int m_frames;
    int m_frame;
    int m_tpf;
    int m_ticks;
    int m_type;
    int m_newType;
};