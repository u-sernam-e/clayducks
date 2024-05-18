#include "anim.h"
#include <string>

void Anim::update()
{
    if (isGoing() && m_rows * m_columns * m_timeBtweenFrames < m_timeSinceStart + lowerLimitFrameTime())
        m_justEnded = true;
    else m_justEnded = false;
    m_timeSinceStart += lowerLimitFrameTime();
}

void Anim::draw()
{
    int currentFrame{static_cast<int>(m_timeSinceStart / m_timeBtweenFrames)};
    float col{static_cast<float>(currentFrame%m_columns)};
    float row{static_cast<float>(currentFrame/m_columns)};
    if (isGoing())
        DrawTexturePro(m_txtr, {col*getTileSize().x, row*getTileSize().y, getTileSize().y, getTileSize().x}, {0, 0, getTileSize().x*m_scale, getTileSize().y*m_scale}, -m_pos, m_rot, m_tint);
}

void Anim::start(Vector2 pos, float scale, float rotation, Color tint)
{
    m_pos = pos;
    m_scale = scale;
    m_rot = rotation;
    m_tint = tint;
    m_timeSinceStart = 0;
}