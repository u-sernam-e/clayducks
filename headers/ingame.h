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
    int stopVelIterator; // IDK WHAT TO NAME IT, ok, the position in the array it is in on this frame
    float timeSinceLastVel;

    bool justCollided;

    float collAng; // last collision angle THESE TWO VARIABLES ARE UNUSED
    bool rolling;
};

class PhysFPSController
{
private:
    float physFPS{};
    float timeSinceLastFrame{};
    int amountPhysFrames{};
public:
    void initialize(float FPS) { physFPS = FPS; }
    void update()
    {
        timeSinceLastFrame += lowerLimitFrameTime();
        if (amountPhysFrames > 0)
        {
            timeSinceLastFrame = fmodf(timeSinceLastFrame, 1/physFPS);
        }
        amountPhysFrames = timeSinceLastFrame * physFPS;
    }

    bool isPhysFrame() { return amountPhysFrames > 0; }
    int getAmountPhysFrames() { return amountPhysFrames; }
    float getPhysFPS() { return physFPS; }
    void setPhysFPS(float FPS) { physFPS = FPS; }
    float getPhysFrameTime() { return 1/physFPS; }
    float getTimeSinceLastFrame() { return timeSinceLastFrame; }
};

class InGame
{
private:
    float M_GRAVITY = 3000; // could have these const, but I guess they could be changed
    float M_BALLSTOPSPEED = M_GRAVITY/30;


    Ball m_bl;
    Vector2 m_displayBallPos;

    float m_blPower;
    int m_strokes;

    Camera2D m_cam;
    PhysFPSController m_PFC;

    Shader m_shad;
    RenderTexture2D m_shadTex;
    unsigned int m_shadBallPosLoc;
    unsigned int m_shadBallRadLoc;
    unsigned int m_shadZoomLoc;
    unsigned int m_shadScreenSizeLoc;

    void doBallPhysics(Course& crs);
    void updateShaderThings(Course& crs);
public:
    void initialize(Course& crs);
    void update(Course& crs);
    void draw(Course& crs);

    Ball::State getState(Course& crs);
    int getStrokes() {return m_strokes;}
};

#endif