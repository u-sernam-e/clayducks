#ifndef INGAMEFILE
#define INGAMEFILE
#include "course.h"

struct Ball // idk if this will need to be a class at some point
{
    Vector2 pos;
    Vector2 vel;
};

class InGame
{
private:
    Ball m_bl;
    Camera2D m_cam;
public:
    void update(Course& crs);
    void draw(Course& crs);
};

#endif