#ifndef SLIDER
#define SLIDER
#include "rayextended.h"
#include "ob.h"

class Slider : public Ob
{
private:
    enum State
    {
        UP,
        ACTIVE,
        HOVER,
        MAXBUTTONSTATE
    };
    State m_state{};
    Vector2 m_pos{};
    bool m_isVertical{};
    float m_value{};
    Color m_upTint{};
    Color m_hoverTint{};
    Color m_activeTint{};
    const Texture2D& m_knobTxtr{};
    const Texture2D& m_lineTxtr{};
public:
    Slider(Vector2 pos, const Texture2D& knobTexture, const Texture2D& lineTexture, Color upTint, Color activeTint, Color hoverTint, bool isVertical)
        : m_pos{pos}
        , m_knobTxtr{knobTexture}
        , m_lineTxtr{lineTexture}
        , m_upTint{upTint}
        , m_hoverTint{hoverTint}
        , m_activeTint{activeTint}
        , m_isVertical{isVertical}
    {}

    void update() override;
    void draw() override;

    float value() { return m_value; }
};

#endif