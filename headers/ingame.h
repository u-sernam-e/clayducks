#ifndef INGAMEFILE
#define INGAMEFILE
#include <array>
#include "course.h"

struct Ball // idk if this will need to be a class at some point
{
    enum State
    {
        ACTIVE,
        LOSS,
        WIN
    };

    State state; // add timer so there can be an animation before finish screen

    Vector2 pos;
    Vector2 vel;
    float rad;

    std::array<float, 20> stopVels; // used to stop the ball to hit over 2 seconds (10 times a second)
    int stopVelIterator; // IDK WHAT TO NAME IT, ok
    float timeSinceLastVel;

    float collAng; // last collision angle
    bool rolling;
};

class PhysFPSController
{
private:
    float physFPS{};
    float timeSinceLastFrame{};
    float previousTimeSinceLastFrame{};
    bool physFrame{};
public:
    void initialize(float FPS) { physFPS = FPS; timeSinceLastFrame = 0; }
    void update()
    { 
        previousTimeSinceLastFrame = timeSinceLastFrame;

        physFrame = false;
        timeSinceLastFrame += lowerLimitFrameTime();
        if (timeSinceLastFrame > 1/physFPS)
        {
            timeSinceLastFrame = fmodf(timeSinceLastFrame, 1/physFPS);
            physFrame = true;
        }
    }

    bool isPhysFrame() { return physFrame; }
    float getPhysFPS() { return physFPS; }
    void setPhysFPS(float FPS) { physFPS = FPS; }
    float getPhysFrameTime() { return 1/physFPS; }
    float getTimeSinceLastFrame() { return timeSinceLastFrame; }
};

class InGame
{
private:
    Ball m_bl;
    Vector2 m_displayBallPos;

    Camera2D m_cam;
    PhysFPSController m_PFC;

    Shader m_shad;
    RenderTexture2D m_shadTex;
    unsigned int m_shadBallPosLoc;
    unsigned int m_shadBallRadLoc;
    unsigned int m_shadZoomLoc;
    unsigned int m_shadScreenSizeLoc;

    float timerifj;
    float max;

    void updateShaderThings(Course& crs);
public:
    void initialize(Course& crs);
    void update(Course& crs);
    void draw(Course& crs);

    Ball::State getState(Course& crs);
};

#endif