#ifndef ANIM
#define ANIM
#include "rayextended.h"
#include "ob.h"

class Anim : public Ob
{
private:
    Texture2D m_txtr{};
    int m_rows{};
    int m_columns{};
    float m_timeBtweenFrames{}; // in seconds
    float m_timeSinceStart{};   // also in seconds
    bool m_justEnded{};
    Vector2 m_pos{};
    float m_scale{};
    float m_rot{};
    Color m_tint{};
public:
    Anim(Texture2D texture, int rows, int columns, float fps)
        : m_txtr{texture}
        , m_rows{rows}
        , m_columns{columns}
        , m_timeBtweenFrames{(1/fps)} // convert fps to milliseconds
        , m_timeSinceStart{m_rows * m_columns * m_timeBtweenFrames} // start the time at after the animation so it doesn't start at initialization
    {}

    void update() override;
    void draw() override;

    void start(Vector2 pos, float scale, float rotation, Color tint);
    bool isGoing() { return m_rows * m_columns * m_timeBtweenFrames > m_timeSinceStart; }
    bool justEnded() { return m_justEnded; }
    void changePos(Vector2 pos) { m_pos = pos; }
    Texture2D getTxtr() { return m_txtr; }
    Vector2 getTileSize() { return {static_cast<float>(m_txtr.width / m_columns), static_cast<float>(m_txtr.height / m_rows)}; }
};

#endif