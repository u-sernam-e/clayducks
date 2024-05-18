#include "slider.h"

float calculateValue(Vector2 pos, Vector2 size, bool isVert)
{
    float min;
    float s;
    float mouse;
    if (isVert)
    {
        min = pos.y;
        s = size.y;
        mouse = GetMousePosition().y;
    }
    else
    {
        min = pos.x;
        s = size.x;
        mouse = GetMousePosition().x;
    }
    float val{ (mouse - min) / s };
    if (val < 0)
        val = 0;
    if (val > 1)
        val = 1;
    return val;
}

void Slider::update()
{
    bool mouseColliding{ CheckCollisionPointRec(GetMousePosition(), {m_pos.x, m_pos.y, static_cast<float>(m_lineTxtr.width), static_cast<float>(m_lineTxtr.height)}) || CheckCollisionPointRec(GetMousePosition(), {m_pos.x + (m_lineTxtr.width/2 - m_knobTxtr.width/2), m_pos.y - m_knobTxtr.height/2 + m_lineTxtr.height*m_value, static_cast<float>(m_knobTxtr.width), static_cast<float>(m_knobTxtr.height)}) };
    if (m_state == ACTIVE)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            m_value = calculateValue(m_pos, {static_cast<float>(m_lineTxtr.width), static_cast<float>(m_lineTxtr.height)}, m_isVertical);
        else if (mouseColliding)
            m_state = HOVER;
        else
            m_state = UP;
    }
    else if (mouseColliding)
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            m_state = ACTIVE;
        else
            m_state = HOVER;
    }
    else
        m_state = UP;
}

void Slider::draw()
{
    Color tint;
    switch (m_state)
    {
        case ACTIVE: {
            tint = m_activeTint;
            break;
        }
        case UP: {
            tint = m_upTint;
            break;
        }
        case HOVER: {
            tint = m_hoverTint;
            break;
        }
        default: {
            tint = {255, 0, 0, 255};
            break;
        }
    }
    DrawTexture(m_lineTxtr, m_pos.x, m_pos.y, WHITE);
    if (m_isVertical)
        DrawTexture(m_knobTxtr, m_pos.x + (m_lineTxtr.width/2 - m_knobTxtr.width/2), m_pos.y - m_knobTxtr.height/2 + m_lineTxtr.height*m_value, tint);
    else
        DrawTexture(m_knobTxtr, m_pos.x - m_knobTxtr.width/2 + m_lineTxtr.width*m_value, m_pos.y + (m_lineTxtr.height/2 - m_knobTxtr.height/2), tint);
}