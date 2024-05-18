#include "button.h"

void Button::update()
{
    Vector2 activePos{(m_anchorRight ? GetScreenWidth() - m_pos.x : m_pos.x), (m_anchorBot ? GetScreenHeight() - m_pos.y : m_pos.y)};
    
    if (CheckCollisionPointRec(GetMousePosition(), {activePos.x, activePos.y, m_size.x, m_size.y}))
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            m_state = PRESSED;
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            m_state = RELEASED;
        else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            m_state = DOWN;
        else
            m_state = HOVER;
    }
    else
    {
        m_state = UP;
    }
}

int maxXTextSize(const std::string& txt, Font font, int xSize, int depth) // for the text when no texture
{
    int maxSize{xSize-10};
    int minSize{};
    for (int i{}; i < depth; ++i)
    {
        int guess{(maxSize + minSize) / 2};
        int guessXSize{MeasureTextEx(font, txt.c_str(), guess, 2).x};
        if (guessXSize > xSize-10)
        {
            maxSize = guess;
            continue;
        }
        if (guessXSize < xSize-10)
        {
            minSize = guess;
            continue;
        }
        if (guessXSize == xSize-10)
            break;
    }
    return (maxSize + minSize) / 2;
}

void Button::draw()
{
    if (m_hidden)
        return;

    Vector2 activePos{(m_anchorRight ? GetScreenWidth() - m_pos.x : m_pos.x), (m_anchorBot ? GetScreenHeight() - m_pos.y : m_pos.y)};

    Color col{};
    switch (m_state)
    {
        case HOVER:
            col = m_hoverTint;
            break;
        case UP:
        case RELEASED:
            col = m_upTint;
            break;
        case DOWN:
        case PRESSED:
            col = m_downTint;
            break;
    }
    if (m_hasTxtr)
        DrawTextureNPatch(m_txtr, m_nPatchInfo, {activePos.x, activePos.y, m_size.x, m_size.y}, {0, 0}, 0, col);
    else
        DrawRectangleV(activePos, m_size, col);

    int fsize{std::min(static_cast<int>(m_size.y) - 10, maxXTextSize(m_txt, m_font, m_size.x, 10))};
    DrawTextEx(m_font, m_txt.c_str(), Vector2{(activePos.x + m_size.x/2) - MeasureTextEx(m_font, m_txt.c_str(), fsize, 2).x/2, (activePos.y + m_size.y/2) - fsize/2}, fsize, 2, WHITE);
}


bool Button::pressed()
{
    return m_state == PRESSED;
}

bool Button::down()
{
    return m_state == DOWN || m_state == PRESSED;
}

bool Button::released()
{
    return m_state == RELEASED;
}

bool Button::hover()
{
    return m_state == HOVER || m_state == RELEASED;
}