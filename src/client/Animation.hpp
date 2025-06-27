#pragma once

class Animation {
public:
    Animation(int frames, int fps = 10);

    void SetFrame(int frame) { m_frame = frame; };
    void SetFps(int fps) { m_fps = fps; }
    void Update();

protected:
    int m_frames;
    int m_frame;
    int m_fps;
};