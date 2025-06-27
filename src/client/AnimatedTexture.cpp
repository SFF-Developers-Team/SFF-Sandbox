#include "AnimatedTexture.hpp"
#include <Rectangle.hpp>
#include <algorithm>

AnimatedTexture::AnimatedTexture(raylib::Texture2D& texture, std::initializer_list<int> const& typeList, int frames, int tpf) 
    : m_texture(texture), m_typeList(typeList), m_frames(frames), m_tpf(tpf), m_ticks(0), m_frame(0), m_type(0), m_newType(0) {}

void AnimatedTexture::OnTick() {
    if (m_ticks > m_tpf) {
        m_ticks = 0;
        m_frame++;
    }

    if (m_frame > m_frames || m_frame >= m_typeList[m_type + 1]) {
        m_type = m_newType;
        m_frame = m_typeList[m_type];
    }
    
    m_ticks++;
}

void AnimatedTexture::SetType(int type, bool force) {
    m_newType = std::clamp(type, 0, (int)m_typeList.size());

    if (force) {
        m_frame = m_typeList[type];
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