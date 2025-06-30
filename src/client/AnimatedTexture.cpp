#include "AnimatedTexture.hpp"
#include "ResourceManager.hpp"
#include "Types.hpp"
#include <Rectangle.hpp>
#include <stdexcept>

AnimatedTexture::AnimatedTexture(std::string const& textureName, int* types, int frames, int tpf)
    : AnimatedTexture(ResourceManager::Get().GetTexture(textureName), types, frames, tpf) {}

AnimatedTexture::AnimatedTexture(raylib::Texture2D& texture, int* types, int frames, int tpf) 
    : m_texture(texture), m_types(types), m_frames(frames), m_tpf(tpf), m_ticks(0), m_frame(0), m_type(0), m_newType(0), m_direction(DIRECTION_LEFT) {}

void AnimatedTexture::OnTick() {
    if (m_ticks > m_tpf) {
        m_ticks = 0;
        m_frame++;
    }

    if (m_frame > m_frames || m_frame >= m_types[m_type + 1]) {
        m_type = m_newType;
        m_frame = m_types[m_type];
    }
    
    m_ticks++;
}

void AnimatedTexture::SetType(int type, bool force) {
    if (type < 0) {
        throw std::invalid_argument("Animation type can't be below zero!");
    }
    
    m_newType = type;

    if (force) {
        m_frame = m_types[type];
    }
}

void AnimatedTexture::Draw(RVector2 position) {
    int const frameWidth = m_texture.GetWidth() / m_frames;

    RRectangle source(
        m_frame * frameWidth, 0.f,
        frameWidth * m_direction, m_texture.GetHeight()
    );
    
    m_texture.Draw(source, position);
}

void AnimatedTexture::Draw(RRectangle dest) {
    int const frameWidth = m_texture.GetWidth() / m_frames;

    RRectangle source(
        m_frame * frameWidth, 0.f,
        frameWidth * m_direction, m_texture.GetHeight()
    );
    
    m_texture.Draw(source, dest);
}