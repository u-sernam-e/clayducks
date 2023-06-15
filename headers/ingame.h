#ifndef INGAMEFILE
#define INGAMEFILE
#include "course.h"

struct Ball // idk if this will need to be a class at some point
{
    Vector2 pos;
    Vector2 vel;
    float rad;
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
public:
    void initialize(Course& crs);
    void update(Course& crs);
    void draw(Course& crs);

    void updateShaderThings(Course& crs);
};

#endif